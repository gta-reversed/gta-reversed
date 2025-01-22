#include "StdInc.h"

#include "Occlusion.h"
#include "Occluder.h"
#include "ActiveOccluder.h"

COccluder(&COcclusion::aInteriorOccluders)[MAX_INTERIOR_OCCLUDERS] = *(COccluder(*)[MAX_INTERIOR_OCCLUDERS])0xC73CC8;
COccluder(&COcclusion::aOccluders)[MAX_MAP_OCCLUDERS] = *(COccluder(*)[MAX_MAP_OCCLUDERS])0xC73FA0;
CActiveOccluder(&COcclusion::aActiveOccluders)[MAX_ACTIVE_OCCLUDERS] = *(CActiveOccluder(*)[MAX_ACTIVE_OCCLUDERS])0xC78610;

int32& COcclusion::NumInteriorOcculdersOnMap = *(int32*)0xC73CC4;
int32& COcclusion::NumOccludersOnMap = *(int32*)0xC73F98;
int32& COcclusion::NumActiveOccluders = *(int32*)0xC73CC0;
int16& COcclusion::FarAwayList = *(int16*)0x8D5D68;
int16& COcclusion::NearbyList = *(int16*)0x8D5D6C;
int16& COcclusion::ListWalkThroughFA = *(int16*)0x8D5D70;
int16& COcclusion::PreviousListWalkThroughFA = *(int16*)0x8D5D74;

float& COcclusion::gMinXInOccluder = *(float*)0xC73CAC;
float& COcclusion::gMaxXInOccluder = *(float*)0xC73CA8;
float& COcclusion::gMinYInOccluder = *(float*)0xC73CA4;
float& COcclusion::gMaxYInOccluder = *(float*)0xC73CA0;

bool(&COcclusion::gOccluderCoorsValid)[8] = *(bool(*)[8])0xC73CB0;
CVector(&COcclusion::gOccluderCoors)[8] = *(CVector(*)[8])0xC798E0;
CVector(&COcclusion::gOccluderCoorsOnScreen)[8] = *(CVector(*)[8])0xC79950;
CVector& COcclusion::gCenterOnScreen = *(CVector*)0xC79940;

void COcclusion::InjectHooks()
{
    RH_ScopedClass(COcclusion);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x71DCA0);
    RH_ScopedInstall(AddOne, 0x71DCD0);
    RH_ScopedInstall(IsPositionOccluded, 0x7200B0);
    RH_ScopedInstall(OccluderHidesBehind, 0x71E080);
    RH_ScopedInstall(ProcessBeforeRendering, 0x7201C0, { .reversed = false });
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

    if (!iLength)
        numMissingDimensions++;
    if (!iWidth)
        numMissingDimensions++;
    if (!iHeight)
        numMissingDimensions++;

    if (numMissingDimensions > 1)
        return;

    // Get the angles in [0 : 360] space and convert to radians
    auto fRotX = DegreesToRadians(CGeneral::LimitAngle(rotX) + 180.0F);
    auto fRotY = DegreesToRadians(CGeneral::LimitAngle(rotY) + 180.0F);
    auto fRotZ = DegreesToRadians(CGeneral::LimitAngle(rotZ) + 180.0F);
    const auto fTwoPiToChar = 256.0F / TWO_PI;

    const auto UpdateOccluder = [&](COccluder& occluder) {
        occluder.m_wMidX   = static_cast<int16>(centerX * 4.0F);
        occluder.m_wMidY   = static_cast<int16>(centerY * 4.0F);
        occluder.m_wMidZ   = static_cast<int16>(centerZ * 4.0F);
        occluder.m_wWidth  = static_cast<int16>((float)iWidth * 4.0F);
        occluder.m_wLength = static_cast<int16>((float)iLength * 4.0F);
        occluder.m_wHeight = static_cast<int16>((float)iHeight * 4.0F);
        occluder.m_cRotX   = static_cast<uint8>(fRotX * fTwoPiToChar);
        occluder.m_cRotZ   = static_cast<uint8>(fRotZ * fTwoPiToChar);
        occluder.m_cRotY   = static_cast<uint8>(fRotY * fTwoPiToChar);
    };

    if (isInterior) {
        auto& occluder = aInteriorOccluders[NumInteriorOcculdersOnMap];
        UpdateOccluder(occluder);
        NumInteriorOcculdersOnMap++;
    } else {
        auto& occluder = aOccluders[NumOccludersOnMap];
        UpdateOccluder(occluder);

        if (flags)
            occluder.m_bFarAway = true;
        else
            occluder.m_bFarAway = false;

        occluder.m_nNextIndex = FarAwayList;
        FarAwayList = NumOccludersOnMap;
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

    for (auto ind = 0; ind < NumActiveOccluders; ++ind)
    {
        auto& occluder = aActiveOccluders[ind];
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
void COcclusion::ProcessBeforeRendering()
{
    return plugin::Call<0x7201C0>();

    /* see https://github.com/gta-reversed/gta-reversed-modern/pull/50#issuecomment-913173808
    NumActiveOccluders = 0;
    if (!CGame::currArea) {
        auto listCur = ListWalkThroughFA;
        auto listPrev = ListWalkThroughFA;
        bool skipProcessing = false;
        if (ListWalkThroughFA == -1) {
            listCur = FarAwayList;
            PreviousListWalkThroughFA = ListWalkThroughFA;
            ListWalkThroughFA = FarAwayList;
            if (FarAwayList == -1)
                skipProcessing = true;
        }
        else
            listPrev = PreviousListWalkThroughFA;

        if (!skipProcessing) {
            for (auto i = 0; i < NUM_OCCLUDERS_PROCESSED_PER_FRAME; ++i) {
                if (listCur == -1)
                    break;

                if (aOccluders[listCur].NearCamera()) {
                    if (listPrev == -1)
                        FarAwayList = aOccluders[listCur].m_nNextIndex;
                    else
                        aOccluders[listPrev].m_nNextIndex = aOccluders[listCur].m_nNextIndex;

                    listCur = aOccluders[listCur].m_nNextIndex;
                    aOccluders[ListWalkThroughFA].m_nNextIndex = NearbyList;
                    NearbyList = ListWalkThroughFA;
                }
                else {
                    listPrev = listCur;
                    PreviousListWalkThroughFA = listPrev;
                    listCur = aOccluders[listCur].m_nNextIndex;
                }

                ListWalkThroughFA = listCur;
            }
        }

        auto curInd = NearbyList;
        auto prevInd = -1;
        while (curInd != -1) {
            if (NumActiveOccluders < MAX_ACTIVE_OCCLUDERS) {
                auto bActive = aOccluders[curInd].ProcessOneOccluder(&aActiveOccluders[NumActiveOccluders]);
                if (bActive)
                    ++NumActiveOccluders;
            }

            if (aOccluders[curInd].NearCamera()) {
                prevInd = curInd;
                curInd = aOccluders[curInd].m_nNextIndex;
            }
            else {
                if (prevInd == -1)
                    NearbyList = aOccluders[curInd].m_nNextIndex;
                else
                    aOccluders[prevInd].m_nNextIndex = aOccluders[curInd].m_nNextIndex;

                auto nextInd = aOccluders[curInd].m_nNextIndex;
                aOccluders[curInd].m_nNextIndex = FarAwayList;
                FarAwayList = curInd;
                curInd = nextInd;
            }
        }
    }
    else {
        for (auto i = 0; i < NumInteriorOcculdersOnMap; ++i) {
            if (NumActiveOccluders < MAX_ACTIVE_OCCLUDERS) {
                auto bActive = aInteriorOccluders[i].ProcessOneOccluder(&aActiveOccluders[NumActiveOccluders]);
                if (bActive)
                    ++NumActiveOccluders;
            }
        } 
    }

    for (auto i = 0; i < NumActiveOccluders; ++i) {
        auto* checked = &aActiveOccluders[i];
        for (auto k = 0; k < NumActiveOccluders; ++k) {
            if (i != k
                && aActiveOccluders[k].m_wDepth < checked->m_wDepth
                && OccluderHidesBehind(checked, &aActiveOccluders[k])) {

                auto prev = NumActiveOccluders - 1;
                if (i < prev)
                    *checked = *(checked + 1);

                --NumActiveOccluders;
                k = NumActiveOccluders;
            }
        }
    }
    */
}
