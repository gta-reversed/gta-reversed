#include "StdInc.h"

#include "Occlusion.h"
#include "Occluder.h"
#include "ActiveOccluder.h"

void COcclusion::InjectHooks()
{
    RH_ScopedClass(COcclusion);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x71DCA0);
    RH_ScopedInstall(AddOne, 0x71DCD0);
    RH_ScopedInstall(IsPositionOccluded, 0x7200B0);
    RH_ScopedInstall(OccluderHidesBehind, 0x71E080);
    RH_ScopedInstall(ProcessBeforeRendering, 0x7201C0);
}

// 0x71DCA0
void COcclusion::Init() {
    ZoneScoped;

    NumOccludersOnMap         =  0;
    NumInteriorOcculdersOnMap =  0;
    FarAwayList               = -1;
    NearbyList                = -1;
    ListWalkThroughFA         = -1;
    PreviousListWalkThroughFA = -1;
}

// 0x71DCD0
void COcclusion::AddOne(float centerX, float centerY, float centerZ, float width, float length, float height, float rotX, float rotY, float rotZ, uint32 flags, bool isInterior)
{
    int32 numMissingDimensions = 0;

    auto iWidth = static_cast<int32>(fabs(width));
    auto iLength = static_cast<int32>(fabs(length));
    auto iHeight = static_cast<int32>(fabs(height));

    if (!iLength && !iWidth && !iHeight) {
        return;
    }

    // Get the angles in [0 : 360] space and convert to radians
    auto fRotX = DegreesToRadians(CGeneral::LimitAngle(rotX) + 180.0F);
    auto fRotY = DegreesToRadians(CGeneral::LimitAngle(rotY) + 180.0F);
    auto fRotZ = DegreesToRadians(CGeneral::LimitAngle(rotZ) + 180.0F);
    const auto fTwoPiToChar = 256.0F / TWO_PI;

    const auto UpdateOccluder = [&](COccluder& occluder) {
        occluder.m_MidX   = static_cast<int16>(centerX * 4.0F);
        occluder.m_MidY   = static_cast<int16>(centerY * 4.0F);
        occluder.m_MidZ   = static_cast<int16>(centerZ * 4.0F);
        occluder.m_Width  = static_cast<int16>((float)iWidth * 4.0F);
        occluder.m_Length = static_cast<int16>((float)iLength * 4.0F);
        occluder.m_Height = static_cast<int16>((float)iHeight * 4.0F);
        occluder.m_RotX   = static_cast<uint8>(fRotX * fTwoPiToChar);
        occluder.m_RotZ   = static_cast<uint8>(fRotZ * fTwoPiToChar);
        occluder.m_RotY   = static_cast<uint8>(fRotY * fTwoPiToChar);
    };

    if (isInterior) {
        auto& occluder = InteriorOccluders[NumInteriorOcculdersOnMap];
        UpdateOccluder(occluder);
        NumInteriorOcculdersOnMap++;
    } else {
        auto& occluder = Occluders[NumOccludersOnMap];
        UpdateOccluder(occluder);

        occluder.m_DontStream   = flags != 0;
        occluder.m_NextIndex = FarAwayList;
        FarAwayList           = NumOccludersOnMap;
        NumOccludersOnMap++;
    }
}

// 0x71E080
bool COcclusion::OccluderHidesBehind(CActiveOccluder* first, CActiveOccluder* second)
{
    if (!first->m_cLinesCount)
        return second->m_cLinesCount == 0;

    for (auto iFirstInd = 0; iFirstInd < first->m_cLinesCount; ++iFirstInd) {
        auto& firstLine = first->m_aLines[iFirstInd];
        for (auto iSecondInd = 0; iSecondInd < second->m_cLinesCount; ++iSecondInd) {
            auto& secondLine = second->m_aLines[iSecondInd];

            if (!IsPointInsideLine(
                secondLine.m_vecOrigin.x,
                secondLine.m_vecOrigin.y,
                secondLine.m_vecDirection.x,
                secondLine.m_vecDirection.y,
                firstLine.m_vecOrigin.x,
                firstLine.m_vecOrigin.y,
                0.0f)
            ) {
                return false;
            }

            if (!IsPointInsideLine(
                secondLine.m_vecOrigin.x,
                secondLine.m_vecOrigin.y,
                secondLine.m_vecDirection.x,
                secondLine.m_vecDirection.y,
                firstLine.m_vecOrigin.x + (firstLine.m_fLength * firstLine.m_vecDirection.x),
                firstLine.m_vecOrigin.y + (firstLine.m_fLength * firstLine.m_vecDirection.y),
                0.0f)
            ) {
                return false;
            }
        }
    }

    return true;
}

// 0x7200B0
bool COcclusion::IsPositionOccluded(CVector vecPos, float fRadius)
{
    if (!NumActiveOccluders)
        return false;

    CVector outPos;
    float screenX, screenY;
    if (!CalcScreenCoors(vecPos, outPos, screenX, screenY))
        return false;

    const auto fLongEdge = std::max(screenX, screenY);
    auto fScreenRadius = fRadius * fLongEdge;
    auto fScreenDepth = outPos.z - fRadius;

    for (size_t ind = 0; ind < NumActiveOccluders; ++ind)
    {
        auto& occluder = ActiveOccluders[ind];
        if (occluder.m_wDepth >= fScreenDepth
            || !occluder.IsPointWithinOcclusionArea(outPos.x, outPos.y, fScreenRadius)
            || !occluder.IsPointBehindOccluder(vecPos, fRadius))
        {
            continue;
        }

        return true;
    }

    return false;
}

// 0x7201C0
void COcclusion::ProcessBeforeRendering() {
    NumActiveOccluders = 0;

    // Update nearby and far-away lists
    if (CGame::CanSeeOutSideFromCurrArea()) {
        // @ 0x72021A - Update far-away list
        const auto UpdateFarAwayList = [&]{
            for (int32 size{}; size < 16 && ListWalkThroughFA != -1; size++) { // We do at most 16 iterations!
                auto& occluder = Occluders[ListWalkThroughFA]; 

                // If the occluder is now near move it into the (nearby) list
                if (occluder.NearCamera()) { // 0x72022E
                    if (PreviousListWalkThroughFA == -1) {
                        FarAwayList = occluder.GetNext();
                    } else {
                        Occluders[PreviousListWalkThroughFA].SetNext(occluder.GetNext());
                    }
                    ListWalkThroughFA = occluder.SetNext(std::exchange(NearbyList, ListWalkThroughFA)); 
                } else {
                    PreviousListWalkThroughFA = std::exchange(ListWalkThroughFA, occluder.GetNext());
                }
            }
        };

        // @ 0x720307 - Update nearby list
        const auto UpdateNearbyList = [&]{
            for (int16 i{NearbyList}, prev{-1}; i != -1; ) {
                auto& occluder = Occluders[i]; 

                // 0x720323 - Process this occluder
                if (NumActiveOccluders < ActiveOccluders.size()) {
                    if (occluder.ProcessOneOccluder(&ActiveOccluders[NumActiveOccluders])) {
                        NumActiveOccluders++;
                    }
                }

                // If the occluder is now far away move it into the faraway list
                if (!occluder.NearCamera()) { // 0x72035A (inverted!)
                    if (prev == -1) {
                        NearbyList = occluder.GetNext();
                    } else {
                        Occluders[prev].SetNext(occluder.GetNext());
                    }
                    i = occluder.SetNext(std::exchange(FarAwayList, i));
                } else { // 0x7203D6
                    prev = std::exchange(i, occluder.GetNext());
                }
            }
        };

        if (ListWalkThroughFA == -1) {
            PreviousListWalkThroughFA = -1;
            ListWalkThroughFA         = FarAwayList;
            if (FarAwayList == -1) {
                UpdateNearbyList();
            } else {
                UpdateFarAwayList();
                UpdateNearbyList();
            }
        } else {
            UpdateNearbyList();
            UpdateFarAwayList();
        }
    }

    // 0x7203FC - Process interior occluders
    for (size_t i{}; i < NumInteriorOcculdersOnMap; i++) {
        if (NumActiveOccluders > ActiveOccluders.size()) {
            break;
        }
        if (InteriorOccluders[i].ProcessOneOccluder(&ActiveOccluders[NumActiveOccluders])) {
            NumActiveOccluders++;
        }
    }

    // 0x72043A - Remove occluded occluders
    const auto [b, e] = rng::remove_if(GetActiveOccluders(), [](CActiveOccluder& o) {
        return rng::any_of(GetActiveOccluders(), [&](CActiveOccluder& i) {
            return &o != &i && i.m_wDepth < o.m_wDepth && OccluderHidesBehind(&o, &i);
        });
    });
    NumActiveOccluders -= (size_t)(std::distance(b, e));
}
