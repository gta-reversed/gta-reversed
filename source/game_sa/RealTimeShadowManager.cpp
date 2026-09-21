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
    RH_ScopedInstall(GetRealTimeShadow, 0x706970, { .reversed = false });
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
        if (const auto owner = shdw->m_pOwner) {
            delete shdw; // `shdw->m_pOwner` nulled out by this
            delete owner; // Why?
        }
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
    const auto ReCreateRaster = [](CShadowCamera& camera) {
        auto* const raster = RwCameraGetRaster(camera.m_pRwCamera);
        const auto  width  = RwRasterGetWidth(raster);
        RwCameraSetRaster(camera.m_pRwCamera, nullptr);
        RwRasterDestroy(raster);
        const auto newRaster = RwRasterCreate(width, width, 0, rwRASTERTYPECAMERATEXTURE);
        RwCameraSetRaster(camera.m_pRwCamera, newRaster);
        RwTextureSetRaster(camera.m_pRwRenderTexture, newRaster);
    };
    for (auto* shadow : m_apShadows) {
        ReCreateRaster(shadow->m_camera);
        ReCreateRaster(shadow->m_blurCamera);
    }
    ReCreateRaster(m_BlurCamera);
    ReCreateRaster(m_GradientCamera);
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
CRealTimeShadow& CRealTimeShadowManager::GetRealTimeShadow(CPhysical* physical) {
    // Decompiled from binary via rig (entity+0x36 type check, +0x598 driver check, player +0x46D
    // flag / +0x58C vehicle / speed at +0x44..+0x4C vs 0.3, slot scan over m_apShadows).
    // Member mapping: m_apShadows starts at +0x4 (16 slots), CPhysical::m_pShadowData at +0x134,
    // CRealTimeShadow::m_bKeepAlive at +0x4, m_nIntensity at +0x5.
    // 0x706520 (shadow setup for the entity) has no named hook/signature in the repo, so it is
    // invoked via plugin::Call to preserve exact behavior.
    bool allowShadow = true;
    bool useFirstSlot = false;
    // Binary: if ((entityType & 7) != ENTITY_TYPE_PED || !m_pDriver) useFirstSlot = (m_pDriver == null).
    // NOTE: for peds +0x598 overlaps ped data (not a real driver pointer); the binary reads it anyway.
    const bool isPed = physical->GetIsTypePed();
    CVehicle* const asVeh = physical->AsVehicle();
    if (isPed || !asVeh->m_pDriver) {
        useFirstSlot = asVeh->m_pDriver == nullptr;
    }
    if (!useFirstSlot) {
        const auto& player = CWorld::Players[CWorld::PlayerInFocus];
        auto* const playerPed = player.m_pPed;
        auto* const playerVeh = playerPed ? playerPed->m_pVehicle : nullptr;
        // Binary reads playerPed+0x46D bit0 and playerPed+0x58C (vehicle), then speed at +0x44..+0x4C.
        const bool flagSet = playerPed && ((*(reinterpret_cast<const uint8*>(playerPed) + 0x46D)) & 1) != 0;
        if (flagSet && playerVeh && playerVeh->GetMoveSpeed().SquaredMagnitude() > sq(0.3f)) {
            allowShadow = false;
        }
    }
    CRealTimeShadow* shadow = nullptr;
    if (m_bInitialised && allowShadow) {
        if (useFirstSlot) {
            shadow = m_apShadows[0];
        } else {
            // Last free slot (owner == null) wins, matching the binary's 3x5 scan over slots 1..15
            // (slot 0 is only used by the useFirstSlot path above).
            for (size_t i = 1; i < std::size(m_apShadows); ++i) {
                if (!m_apShadows[i]->m_pOwner) {
                    shadow = m_apShadows[i];
                }
            }
        }
        if (shadow) {
            plugin::CallMethod<0x706520, CRealTimeShadow*, CPhysical*>(shadow, physical);
            physical->m_pShadowData = shadow;
            shadow->m_bKeepAlive = true;
            shadow->m_nIntensity = 0;
        }
    }
    return *shadow;
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
