#include "StdInc.h"

#include "RealTimeShadowManager.h"
#include "Shadows.h"

auto& g_realTimeShadowMan = StaticRef<CRealTimeShadowManager>(0xC40350);

void CRealTimeShadowManager::InjectHooks() {
    RH_ScopedClass(CRealTimeShadowManager);
    RH_ScopedCategory("Shadows");

    RH_ScopedInstall(Init, 0x7067C0);
    RH_ScopedInstall(ReInit, 0x706870);
    RH_ScopedInstall(ReturnRealTimeShadow, 0x705B30);
    RH_ScopedInstall(GetRealTimeShadow, 0x706970);
    RH_ScopedInstall(Update, 0x706AB0);
    RH_ScopedInstall(DoShadowThisFrame, 0x706BA0);
    RH_ScopedInstall(Exit, 0x706A60);
}

// 0x7067C0
void CRealTimeShadowManager::Init() {
    if (m_bInitialised) {
        return;
    }

    for (auto& shdw : m_apShadows) {
        shdw = new CRealTimeShadow();
        shdw->Create(true, 4, true);
    }

    m_BlurCamera.Create(6);

    m_GradientCamera.Create(6);
    m_GradientCamera.MakeGradientRaster();

    m_bInitialised = true;
}

// 0x706A60
void CRealTimeShadowManager::Exit() { // AKA `Shutdown`
    if (!m_bInitialised) {
        return;
    }

    for (auto& shdw : m_apShadows) {
        delete std::exchange(shdw, nullptr);
    }

    // Nice hack
    m_BlurCamera.Destroy();
    m_GradientCamera.Destroy();

    m_bInitialised = false;
}

// 0x705B30
void CRealTimeShadowManager::ReturnRealTimeShadow(CRealTimeShadow* shdw) {
    if (m_bInitialised) {
        shdw->m_pOwner->m_pShadowData = nullptr;
        shdw->m_pOwner = nullptr;
    }
}

// 0x706870
void CRealTimeShadowManager::ReInit() {
    // Re-create the camera's raster (with the same size)
    const auto ReInitCamera = [](CShadowCamera& cam) {
        auto* const raster = RwCameraGetRaster(cam.m_pRwCamera);
        const auto  size   = RwRasterGetWidth(raster);
        RwCameraSetRaster(cam.m_pRwCamera, nullptr);
        RwRasterDestroy(raster);

        auto* const newRaster = RwRasterCreate(size, size, 0, rwRASTERTYPECAMERATEXTURE);
        RwCameraSetRaster(cam.m_pRwCamera, newRaster);
        RwTextureSetRaster(cam.m_pRwRenderTexture, newRaster);
    };

    for (auto* const shdw : m_apShadows) {
        ReInitCamera(shdw->m_camera);
        ReInitCamera(shdw->m_blurCamera);
    }
    ReInitCamera(m_BlurCamera);
    ReInitCamera(m_GradientCamera);
    m_GradientCamera.MakeGradientRaster();
}

// 0x706AB0
void CRealTimeShadowManager::Update() {
    ZoneScoped;

    if (m_bInitialised && m_bNeedsReinit) {
        ReInit();
        m_bNeedsReinit = false;
    }

    for (const auto shdw : m_apShadows) {
        if (!shdw->m_pOwner) {
            continue;
        }

        assert(shdw->m_pOwner->m_pShadowData == shdw);

        // 0x305eed - 0x305f0f: Update intensity
        constexpr auto INTENSITY_STEP = 3u;
        if (shdw->m_bKeepAlive) {
            shdw->m_nIntensity = std::min<uint8>(100u, shdw->m_nIntensity + INTENSITY_STEP);
        } else { // Fade out
            shdw->m_nIntensity = std::max<uint8>(shdw->m_nIntensity, INTENSITY_STEP) - INTENSITY_STEP; // Avoids underflow
        }

        if (shdw->m_nIntensity) {
            shdw->Update();
            CShadows::StoreRealTimeShadow(
                shdw->m_pOwner,

                CTimeCycle::m_fShadowDisplacementX[CTimeCycle::m_CurrentStoredValue],
                CTimeCycle::m_fShadowDisplacementY[CTimeCycle::m_CurrentStoredValue],

                CTimeCycle::m_fShadowFrontX[CTimeCycle::m_CurrentStoredValue],
                CTimeCycle::m_fShadowFrontY[CTimeCycle::m_CurrentStoredValue],

                CTimeCycle::m_fShadowSideX[CTimeCycle::m_CurrentStoredValue],
                CTimeCycle::m_fShadowSideY[CTimeCycle::m_CurrentStoredValue]
            );
        } else if (m_bInitialised) {
            shdw->m_pOwner->m_pShadowData = nullptr;
            shdw->m_pOwner = nullptr;
        }
    }

    // TODO: ??? - Perhaps debug code left accidentally in?
    for (const auto shdw : m_apShadows) {
        shdw->m_bKeepAlive = false;
    }
}

// 0x706970
CRealTimeShadow* CRealTimeShadowManager::GetRealTimeShadow(CPhysical* physical) {
    const auto isPlayer = physical->GetIsTypePed() && physical->AsPed()->m_nPedType == PED_TYPE_PLAYER1;

    // 0x706996 - No shadows for others while the player is driving fast
    const auto CanCreateShadow = [isPlayer] {
        if (isPlayer) {
            return true;
        }
        const auto* const player = FindPlayerPed();
        return !player->bInVehicle || !player->m_pVehicle || player->m_pVehicle->GetMoveSpeed().SquaredMagnitude() <= sq(0.3f);
    };
    if (!m_bInitialised || !CanCreateShadow()) {
        return nullptr;
    }

    // 0x7069F3 - The first shadow is reserved for the player, others get the last free one
    const auto shdw = [&]() -> CRealTimeShadow* {
        if (isPlayer) {
            return m_apShadows[0];
        }
        const auto IsFree = [](const CRealTimeShadow* s) {
            return !s->m_pOwner;
        };
        auto free = m_apShadows | rngv::drop(1) | rngv::reverse | rngv::filter(IsFree);
        return rng::empty(free) ? nullptr : *rng::begin(free);
    }();
    if (!shdw) {
        return nullptr;
    }

    shdw->SetupForThisEntity(physical);
    physical->m_pShadowData = shdw;
    shdw->m_bKeepAlive      = true;
    shdw->m_nIntensity      = 0;
    return shdw;
}

// 0x706BA0
void CRealTimeShadowManager::DoShadowThisFrame(CPhysical* physical) {
    switch (g_fx.GetFxQuality()) {
    case FX_QUALITY_VERY_HIGH: // Always render
        break;
    case FX_QUALITY_HIGH: { // Only draw for main player
        if (physical->GetIsTypePed()) {
            if (physical->AsPed()->m_nPedType == PED_TYPE_PLAYER1) {
                break;
            }
        }
        return;
    }
    default: // For any other quality: skip
        return;
    }

    if (const auto shdw = physical->m_pShadowData) {
        shdw->m_bKeepAlive = true;
    } else {
        (void)GetRealTimeShadow(physical); // ???
    }
}
