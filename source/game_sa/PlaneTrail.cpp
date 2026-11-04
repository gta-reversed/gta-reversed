#include "StdInc.h"

#include "PlaneTrail.h"

constexpr auto POINT_EXPIRATION_TIME = 30'000u;

void CPlaneTrail::InjectHooks() {
    RH_ScopedClass(CPlaneTrail);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x717160);
    RH_ScopedInstall(Render, 0x717180);
    RH_ScopedInstall(RegisterPoint, 0x7172D0);
}

// 0x717160
void CPlaneTrail::Init() {
    std::ranges::fill(m_Timepoints, 0);
}

// 0x717180
void CPlaneTrail::Render(float intensity) {
    const auto maxAlpha = (int32)(intensity * 110.0f);
    if (!maxAlpha) {
        return;
    }

    auto nvert{0u}; // num of vertices stored
    RxObjSpace3DVertex vertBuff[PLANE_TRAIL_BUF_SIZE];
    for (auto i = 0u; i < PLANE_TRAIL_BUF_SIZE; i++) {
        const uint32 timeDelta = CTimer::GetTimeInMS() - m_Timepoints[i];
        if (IsPointInUse(i) && timeDelta <= POINT_EXPIRATION_TIME) {
            RxObjSpace3DVertex& vert = vertBuff[i];

            const float fAlphaMult = std::min(1.0f, ((float)POINT_EXPIRATION_TIME - (float)timeDelta) / 10'000.0f); // Clamped to 1.0f
            RwIm3DVertexSetRGBA(&vert, 255, 255, 255, (uint8)((float)maxAlpha * fAlphaMult));

            RwV3dAssign(RwIm3DVertexGetPos(&vert), &m_Positions[i]);

            nvert++;
        } else { // This one has expired, set to 0
            m_Timepoints[i] = 0;
        }
    }

    if (nvert) {
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));
        if (RwIm3DTransform(vertBuff, nvert, nullptr, rwIM3D_VERTEXXYZ | rwIM3D_VERTEXRGBA)) {
            static RwImVertexIndex indices[] = { // From 0x8D5B98, size 32
                0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16
            };
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, indices, 2 * nvert - 2);
            RwIm3DEnd();
        }
    }
}

// 0x7172D0
void CPlaneTrail::RegisterPoint(CVector pos) {
    const auto& lastUpdate = m_Timepoints[0];
    const bool bDoShift = lastUpdate && CTimer::GetTimeInMS() - lastUpdate > 2000;
    if (bDoShift) {
        // Shift right
#if __cpp_lib_shift >= 202202L 
        rng::shift_right(m_Timepoints, 1);
        rng::shift_right(m_Positions, 1);
#else
        for (auto i = PLANE_TRAIL_BUF_SIZE - 1; i; i--) {
            m_Timepoints[i] = m_Timepoints[i - 1];
            m_Positions[i] = m_Positions[i - 1];
        }
#endif
    }

    m_Positions[0] = pos;
    if (bDoShift || !IsPointInUse(0)) {
        m_Timepoints[0] = CTimer::GetTimeInMS();
    }
}

// From `CPlaneTrails::Update`
void CPlaneTrail::Update(CVector pos, const CRGBA& color, uint32 coronaIdx, uint32 timeModifierMs, uint8 afterHour, uint8 beforeHour) {
    const float fTimeProg = (float)(CTimer::GetTimeInMS() % 131072) / 20860.0f; // or * 0.000047936901.. Not sure where this comes from..
    const CVector currPos = pos * CVector(std::sin(fTimeProg), std::cos(fTimeProg), 1.0f);

    RegisterPoint(currPos);

    if (!CClock::ClockHoursInRange(afterHour, beforeHour)) {
        return;
    }

    if ((CTimer::GetTimeInMS() + timeModifierMs) & 512) {
        CCoronas::RegisterCorona(
            coronaIdx,
            nullptr,
            color.r, color.g, color.b, color.a,
            currPos,
            5.0f,
            2000.0f,
            eCoronaType::CORONATYPE_HEADLIGHT,
            eCoronaFlareType::FLARETYPE_NONE,
            eCoronaReflType::CORREFL_NONE,
            eCoronaLOSCheck::LOSCHECK_OFF,
            eCoronaTrail::TRAIL_OFF,
            0.0f,
            false,
            1.5f,
            0,
            15.0f,
            false,
            false
        );
    } else {
        CCoronas::UpdateCoronaCoors(coronaIdx, currPos, 2000.0f, 0.0f);
    }
}

bool CPlaneTrail::IsPointInUse(size_t pt) {
    return m_Timepoints[pt] != 0;
}
