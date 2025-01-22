#include "StdInc.h"

#include "Skidmark.h"

#include "Skidmarks.h"
#include "CustomBuildingDNPipeline.h"

void CSkidmark::Init(uint32 id, CVector posn, eSkidmarkType type, const bool* bloodState) {
    m_nId         = id;
    m_vPosn[0]    = posn;
    m_partDirX[0] = 0.0f;
    m_partDirY[0] = 0.0f;
    m_bActive     = true;
    m_nState      = eSkidmarkState::JUST_UPDATED;
    m_nNumParts   = 0;
    m_nType       = *bloodState ? eSkidmarkType::BLOODY : type;
    m_lastDisappearTimeUpdateMs = CTimer::GetTimeInMS() - 1'000;
}

CSphere CSkidmark::GetBoundingSphere() const {
    const CVector center = (GetLastPartPosn() + GetFirstPartPosn()) / 2.0f;
    const float radius = (GetLastPartPosn() - center).Magnitude();
    return { center, radius };
}

// see CSkidmarks::Update
void CSkidmark::Update() {
    const auto timeMS = CTimer::GetTimeInMS();
    switch (m_nState) {
    case eSkidmarkState::JUST_UPDATED: {
        if (m_bActive)
            break;

        const auto UpdateTime = [this, timeMS](uint32 low, uint32 high) {
            m_fadeBeginMs += timeMS + low;
            m_disappearAtMs += timeMS + high;
        };

        m_nState = eSkidmarkState::DISAPPEARING;
        if (m_nNumParts < 4)
            UpdateTime(2500, 5000);
        else if (m_nNumParts >= 9)
            UpdateTime(10'000, 20'000);
        else // 4 - 8 parts
            UpdateTime(5'000, 10'000);
        break;
    }
    case eSkidmarkState::DISAPPEARING: {
        if (timeMS > m_disappearAtMs)
            m_nState = eSkidmarkState::INACTIVE;
        break;
    }
    }
    m_bActive = false;
}

CRGBA CSkidmark::GetColor() const {
    const auto GetBaseColor = [this]() -> CRGBA {
        switch (m_nType) {
        case eSkidmarkType::DEFAULT: return { 0, 0, 0, 255 };
        case eSkidmarkType::SANDY:   return { 45, 31, 4, 255 };
        case eSkidmarkType::MUDDY:   return { 69, 69, 61, 255 };
        case eSkidmarkType::BLOODY:  return { 32, 0, 0, 255 };
        default: {
            NOTSA_UNREACHABLE("Unexpected skidmark type %d", (int)m_nType);
            return {};
        }
        }
    };

    const auto GetAlpha = [this]() -> uint8 {
        if (m_nState == eSkidmarkState::JUST_UPDATED || CTimer::GetTimeInMS() < m_fadeBeginMs)
            return 255u;

        // Here they wanted to be very smart, and avoided doing any float ops.
        // That's why they first multiply the dividend by 255 then divide.
        return (255u * (m_disappearAtMs - CTimer::GetTimeInMS())) / (m_disappearAtMs - m_fadeBeginMs);
    };

    // TODO: This balance param stuff is probably inlined
    const float balance = ((1.0f - CCustomBuildingDNPipeline::m_fDNBalanceParam) * 0.6f + 0.4f);
    CRGBA balancedColor = GetBaseColor() * balance;
    balancedColor.a = GetAlpha();

    return balancedColor;
}

// see CSkidmarks::Render
void CSkidmark::Render() const {
    if (m_nState == eSkidmarkState::INACTIVE)
        return;

    if (!m_nNumParts)
        return;

    const auto GetColorForPart = [&, baseColor = GetColor()](unsigned part) {
        const auto GetAlpha = [&]() -> uint8 {
            if (part == 0)
                return 0u; // First "part" has always 0 alpha.

            if (m_nNumParts == part && m_nState == eSkidmarkState::DISAPPEARING)
                return 0u; // Last part

            return (uint8)(baseColor.a * 128u / 256u); // Again, they avoided fp division.. This is just a division by 2.
        };
        RwRGBA partColor = baseColor.ToRwRGBA();
        partColor.alpha = GetAlpha();
        return partColor;
    };

    for (unsigned part = 0; part <= m_nNumParts; part++) {
        const CVector posn = m_vPosn[part];
        const CVector dir = { m_partDirX[part], m_partDirY[part], 0.0f };
        const CVector pos[] = {
            posn + dir + CVector{ {}, {}, 0.1f },
            posn - dir + CVector{ {}, {}, 0.1f },
        };
        const     float coordU    = (float)(part) * 5.01f;
        constexpr float coordsV[] = { 0.0f, 1.0f };
        const     RwRGBA color    = GetColorForPart(part);
        for (unsigned vert = 0; vert < 2; vert++) {
            RxObjSpace3DVertex* vertex = &TempBufferVertices.m_3d[2 * part + vert];

            RxObjSpace3DVertexSetPreLitColor(vertex, &color);
            RxObjSpace3DVertexSetPos(vertex, &pos[vert]);
            RxObjSpace3DVertexSetU(vertex, coordsV[vert]);
            RxObjSpace3DVertexSetV(vertex, coordU);
        }
    }

    LittleTest();

    if (RwIm3DTransform(TempBufferVertices.m_3d, 2 * m_nNumParts + 2, nullptr, rwIM3D_VERTEXUV)) {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, CSkidmarks::m_aIndices, 6 * m_nNumParts);
        RwIm3DEnd();
    }
}

// see CSkidmarks::RegisterOne
void CSkidmark::RegisterNewPart(CVector posn, CVector2D dir, float length, bool* bloodState) {
    if ((m_nType == eSkidmarkType::BLOODY) == *bloodState) {
        m_bActive = true;
        if (CTimer::GetTimeInMS() - m_lastDisappearTimeUpdateMs <= 100) {
            m_vPosn[m_nNumParts] = posn; // Update existing, because of low delta time
        } else {
            m_lastDisappearTimeUpdateMs = CTimer::GetTimeInMS();
            if (m_nNumParts >= SKIDMARK_NUM_PARTS - 1) { // The 0th "part" isn't considered as an actual part, so at most we can have this many
                m_nState = eSkidmarkState::DISAPPEARING;
                m_fadeBeginMs = CTimer::GetTimeInMS() + 10'000;
                m_disappearAtMs = CTimer::GetTimeInMS() + 20'000;
                *bloodState = false;
            } else {
                m_nNumParts++;
                m_vPosn[m_nNumParts] = posn;

                const CVector prevPosn = m_vPosn[m_nNumParts - 1];
                CVector2D dirToPrevPart = {
                    posn.y - prevPosn.y, // Swapped intentionally, unsure why though.
                    prevPosn.x - posn.x
                };

                dirToPrevPart.Normalise();
                dir.Normalise();

                const float dot = 1.0f + fabs(DotProduct2D(dirToPrevPart, dir));
                m_partDirX[m_nNumParts] = dot * dirToPrevPart.x * length / 2.0f;
                m_partDirY[m_nNumParts] = dot * dirToPrevPart.y * length / 2.0f;

                if (m_nNumParts == 1) {
                    m_partDirX[0] = m_partDirX[1];
                    m_partDirY[0] = m_partDirY[1];
                }

                if (m_nNumParts > 8)
                    *bloodState = false;
            }
        }
    } else {
        m_nState = eSkidmarkState::DISAPPEARING;
        m_fadeBeginMs = CTimer::GetTimeInMS() + 10'000;
        m_disappearAtMs = CTimer::GetTimeInMS() + 20'000;
    }
}
