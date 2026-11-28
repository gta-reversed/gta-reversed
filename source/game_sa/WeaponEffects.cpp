/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "WeaponEffects.h"
#include "Sprite.h"
#include "Coronas.h"
#include "GameLogic.h"

static inline auto& s_CrossHairPulses          = StaticRef<std::array<float, MAX_NUM_WEAPON_CROSSHAIRS>>(0xC8A8A0); //!< Added to the size of the crosshair
static inline auto& s_IsCrossHairPulseGrowing  = StaticRef<std::array<bool, MAX_NUM_WEAPON_CROSSHAIRS>>(0x8D6144);
static inline auto& s_LockOnSpriteOffsetY      = StaticRef<float>(0xC8A898); //!< Of the lock-on sprite from the target (Flight crosshair)
static inline auto& s_LockOnSpriteOffsetX      = StaticRef<float>(0xC8A89C);
static inline auto& s_HeatSeekTargetChangeTime = StaticRef<uint32>(0xC8A890);
static inline auto& s_HeatSeekTarget           = StaticRef<CEntity*>(0xC8A894); //!< Only compared, never dereferenced

void CWeaponEffects::InjectHooks() {
    RH_ScopedClass(CWeaponEffects);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x742AB0);
    RH_ScopedInstall(Shutdown, 0x742B80);
    RH_ScopedInstall(IsLockedOn, 0x742BD0);
    RH_ScopedInstall(MarkTarget, 0x742BF0);
    RH_ScopedInstall(ClearCrossHair, 0x742C60);
    RH_ScopedInstall(ClearCrossHairs, 0x742C80);
    RH_ScopedInstall(ClearCrossHairImmediately, 0x742CA0);
    RH_ScopedInstall(ClearCrossHairsImmediately, 0x742CC0);
    RH_ScopedInstall(Render, 0x742CF0);
}

// 0x742AB0
void CWeaponEffects::Init() {
    for (auto& crossHair : gCrossHair) {
        crossHair.m_Pos                  = CVector{};
        crossHair.m_IsActive             = false;
        crossHair.m_Color                = CRGBA{ 255, 0, 0, 127 };
        crossHair.m_Size                 = 1.f;
        crossHair.m_Rotation             = 0.f;
        crossHair.m_TimeWhenToDeactivate = 0;
        crossHair.m_IsFlightCrossHair    = false;
        crossHair.m_IsLockedOn           = 0.f;
    }

    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("particle"));
    gpCrossHairTex          = RwTextureRead("target256", "target256m");
    gpCrossHairTexFlight[0] = RwTextureRead("lockon", "lockonA");
    gpCrossHairTexFlight[1] = RwTextureRead("lockonFire", "lockonFireA");
    CTxdStore::PopCurrentTxd();
}

// 0x742B80
void CWeaponEffects::Shutdown() {
    for (auto& crossHair : gpCrossHairTexFlight) {
        RwTextureDestroy(crossHair);
        crossHair = nullptr;
    }

    RwTextureDestroy(gpCrossHairTex);
    gpCrossHairTex = nullptr;
}

// 0x742BD0
bool CWeaponEffects::IsLockedOn(CrossHairId id) {
    return (int32)gCrossHair[id].m_IsLockedOn;
}

// 0x742BF0
void CWeaponEffects::MarkTarget(CrossHairId id, CVector pos, uint8 red, uint8 green, uint8 blue, uint8 alpha, float size, bool isFlightCrossHair) {
    auto& crossHair                  = gCrossHair[id];
    crossHair.m_Pos                  = pos;
    crossHair.m_Color                = CRGBA{ red, green, blue, alpha };
    crossHair.m_IsActive             = true;
    crossHair.m_Size                 = size;
    crossHair.m_TimeWhenToDeactivate = CROSSHAIR_MARKED;
    crossHair.m_IsFlightCrossHair    = isFlightCrossHair;
}

// 0x742C60
void CWeaponEffects::ClearCrossHair(CrossHairId id) {
    gCrossHair[id].m_TimeWhenToDeactivate = CTimer::GetTimeInMS() + 400;
}

// 0x742C80
void CWeaponEffects::ClearCrossHairs() {
    for (auto& crossHair : gCrossHair) {
        crossHair.m_IsActive = false;
    }
}

// 0x742CA0
void CWeaponEffects::ClearCrossHairImmediately(CrossHairId id) {
    gCrossHair[id].m_TimeWhenToDeactivate = CTimer::GetTimeInMS() - 100;
    gCrossHair[id].m_IsActive             = false;
}

// 0x742CC0
void CWeaponEffects::ClearCrossHairsImmediately() {
    for (auto& crossHair : gCrossHair) {
        crossHair.m_TimeWhenToDeactivate = CTimer::GetTimeInMS() - 100;
        crossHair.m_IsActive             = false;
    }
}

// 0x742CF0
void CWeaponEffects::Render() {
    ZoneScoped;

    constexpr auto FLIGHT_SPRITE_MIN_HEIGHT    = 20.f; //!< Sprites of far targets are scaled up to be this high
    constexpr auto FLIGHT_SPRITE_MAX_SIZE      = CVector2D{ 28.f, 20.f };
    constexpr auto FLIGHT_SPRITE_OUTLINE_SIZES = std::array{ 0.95f, 1.05f };
    constexpr auto SPRITE_RECIP_Z              = 0.01f;
    constexpr auto OFF_SCREEN_ARROW_SIZE       = 15.f;
    constexpr auto OFF_SCREEN_ARROW_Z          = 2.5f;
    constexpr auto NUM_TRIANGLES               = 3;
    constexpr auto TRIANGLE_HALF_ANGLE         = 15.f;
    constexpr auto WEAPON_RADIUS_TO_SIZE       = 30.f;
    constexpr auto TIME_TO_ANGLE               = TWO_PI / 1024.f; //!< One turn in 1024 ms

    if (TheCamera.m_bWideScreenOn) {
        return;
    }

    const auto SetRenderStatesForSprites = [] {
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(FALSE));
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    };
    const auto SetBlend = [](RwBlendFunction src, RwBlendFunction dst) {
        RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(src));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(dst));
    };
    const auto RestoreZStates = [] {
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(TRUE));
    };
    const auto GetSpinAngle = [] {
        return (float)(CTimer::GetTimeInMS() & 0x3FF) * TIME_TO_ANGLE;
    };
    // Screen position and size of a sprite at `pos` (That is at least `FLIGHT_SPRITE_MIN_HEIGHT` high)
    const auto CalcFlightSpriteCoors = [](const CVector& pos, CVector& outScreenPos, CVector2D& outSize) {
        if (!CSprite::CalcScreenCoors(pos, &outScreenPos, &outSize.x, &outSize.y, true, true)) {
            return false;
        }
        if (const float scale = FLIGHT_SPRITE_MIN_HEIGHT / outSize.y; scale > 1.f) {
            outSize *= scale;
        }
        return true;
    };

    for (auto&& [i, crossHair] : rngv::enumerate(gCrossHair)) {
        auto& pulse = s_CrossHairPulses[i];

        if (crossHair.m_TimeWhenToDeactivate != 0 && crossHair.m_TimeWhenToDeactivate < CTimer::GetTimeInMS()) {
            crossHair.m_IsActive             = false;
            crossHair.m_TimeWhenToDeactivate = 0;
            pulse                            = 0.f;
        }
        if (crossHair.m_TimeWhenToDeactivate != CROSSHAIR_MARKED) {
            pulse = 0.f;
        }
        auto* const player = FindPlayerPed((int32)i);
        if (!player) {
            crossHair.m_IsActive = false;
        }
        if (!crossHair.m_IsActive) {
            continue;
        }

        const CRGBA color = crossHair.m_Color;

        if (crossHair.m_IsFlightCrossHair) { // 0x742D61
            CVector   screenPos;
            CVector2D screenSize;
            if (CalcFlightSpriteCoors(crossHair.m_Pos, screenPos, screenSize)) {
                SetRenderStatesForSprites();
                SetBlend(rwBLENDSRCALPHA, rwBLENDINVSRCALPHA);
                RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpCrossHairTexFlight[WEAPONEFFECTS_LOCK_ON])));

                const CVector2D size{ std::min(screenSize.x, FLIGHT_SPRITE_MAX_SIZE.x), std::min(screenSize.y, FLIGHT_SPRITE_MAX_SIZE.y) };

                // The target's frame
                for (const float outlineSize : FLIGHT_SPRITE_OUTLINE_SIZES) {
                    CSprite::RenderOneXLUSprite_Rotate_Aspect(screenPos, size * outlineSize * 1.8f, 0, 0, 0, 255, SPRITE_RECIP_Z, 0.f, 255);
                }
                CSprite::RenderOneXLUSprite_Rotate_Aspect(screenPos, size * 1.8f, color.r, color.g, color.b, 255, SPRITE_RECIP_Z, 0.f, color.a);

                // 0x742F4A - The lock-on sprite, it circles the target, getting closer to it
                RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpCrossHairTexFlight[WEAPONEFFECTS_LOCK_ON_FIRE])));

                const float angle = GetSpinAngle();

                // NOTE: The step depends on the angle (So on the time of the frame), not on the time step
                crossHair.m_LockOnDist = std::max(crossHair.m_LockOnDist - 20.f * angle, 0.f);
                crossHair.m_IsLockedOn = crossHair.m_LockOnDist == 0.f ? 1.f : 0.f;
                s_LockOnSpriteOffsetX  = std::sin(angle) * crossHair.m_LockOnDist;
                s_LockOnSpriteOffsetY  = std::cos(angle) * crossHair.m_LockOnDist;

                const CVector lockOnPos{ screenPos.x - s_LockOnSpriteOffsetX, screenPos.y - s_LockOnSpriteOffsetY, screenPos.z };
                for (const float outlineSize : FLIGHT_SPRITE_OUTLINE_SIZES) {
                    CSprite::RenderOneXLUSprite_Rotate_Aspect(lockOnPos, size * outlineSize * 0.8f, 0, 0, 0, 255, SPRITE_RECIP_Z, angle, 255);
                }
                // NOTE: The alpha of the color is the intensity here
                CSprite::RenderOneXLUSprite_Rotate_Aspect(lockOnPos, size * 0.8f, color.r, color.g, color.b, color.a, SPRITE_RECIP_Z, angle, 255);
            }
            RestoreZStates();
            continue;
        }

        // 0x743149
        SetRenderStatesForSprites();
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(nullptr));

        CVector   screenPos;
        CVector2D screenSize;
        CSprite::CalcScreenCoors(crossHair.m_Pos, &screenPos, &screenSize.x, &screenSize.y, false, false);

        if (screenPos.x > SCREEN_WIDTH || screenPos.x < 0.f || screenPos.y > SCREEN_HEIGHT || screenPos.y < 0.f) { // 0x7435C1 - Off screen
            if (CGameLogic::IsCoopGameGoingOn()) {                                                                 // An arrow at the edge of the screen pointing towards the target
                const auto& camMat           = TheCamera.m_mCameraMatrix;

                const CVector camToTarget    = crossHair.m_Pos - camMat.GetPosition();
                const float   camAngle       = std::atan2(camToTarget.Dot(camMat.GetUp()), camToTarget.Dot(camMat.GetRight()));

                const CVector playerToTarget = crossHair.m_Pos - player->GetPosition();
                const float   playerAngle    = std::atan2(playerToTarget.Dot(camMat.GetUp()), playerToTarget.Dot(camMat.GetRight()));

                const CVector2D dir{ std::cos(playerAngle), std::sin(playerAngle) };
                const CVector2D side = dir.GetPerpRight() / 2.f;

                const auto RenderArrow = [&](CVector2D tip, float scale, uint8 r, uint8 g, uint8 b) {
                    CSprite::RenderOneXLUSprite_Triangle(
                        tip,
                        tip + (dir + side) * (OFF_SCREEN_ARROW_SIZE * scale),
                        tip + (dir - side) * (OFF_SCREEN_ARROW_SIZE * scale),
                        OFF_SCREEN_ARROW_Z,
                        r,
                        g,
                        b,
                        255,
                        1.f,
                        255
                    );
                };
                const CVector2D tip{ (1.f - std::cos(camAngle)) * SCREEN_WIDTH * 0.5f, (1.f - std::sin(camAngle)) * SCREEN_HEIGHT * 0.5f };
                RenderArrow(tip, 1.f, 0, 0, 0);
                RenderArrow(tip + dir * (OFF_SCREEN_ARROW_SIZE * 0.1f), 0.8f, color.r, color.g, color.b);
            }
        } else { // 0x7431E9 - 3 triangles around the target
            crossHair.m_Size         = std::clamp(crossHair.m_Size, 0.3f, 1.2f);

            const float z            = std::max(screenPos.z, 2.5f);
            const float weaponRadius = player->GetWeaponRadiusOnScreen();
            const float innerRadius  = weaponRadius > 0.f ? weaponRadius * WEAPON_RADIUS_TO_SIZE : crossHair.m_Size * 5.f;
            const float outerRadius  = weaponRadius > 0.f ? crossHair.m_Size * 20.f + innerRadius : crossHair.m_Size * 25.f;

            SetBlend(rwBLENDZERO, rwBLENDZERO);
            for (auto t = 0; t < NUM_TRIANGLES; t++) { // 0x7432A0
                const float angle     = (float)t * (360.f / (float)NUM_TRIANGLES);
                const auto  GetCorner = [&](float cornerAngle, float radius) {
                    const float a = DegreesToRadians(cornerAngle) + crossHair.m_Rotation;
                    return CVector2D{ screenPos.x - std::sin(a) * (radius + pulse), screenPos.y - std::cos(a) * (radius + pulse) };
                };
                const CVector2D inner  = GetCorner(angle, innerRadius);
                const CVector2D outerA = GetCorner(angle + TRIANGLE_HALF_ANGLE, outerRadius);
                const CVector2D outerB = GetCorner(angle - TRIANGLE_HALF_ANGLE, outerRadius);

                // The outline (NOTE: Only the first one is drawn with the blend states set above)
                CSprite::RenderOneXLUSprite_Triangle(inner, outerA, outerB, z, 0, 0, 0, 255, 1.f, color.a);

                const CVector2D center = (outerB + outerA + inner) * (1.f / 3.f);
                const auto      Shrink = [&](CVector2D corner) {
                    return (corner - center) * 0.75f + center;
                };
                SetBlend(rwBLENDSRCALPHA, rwBLENDINVSRCALPHA);
                RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));
                CSprite::RenderOneXLUSprite_Triangle(Shrink(inner), Shrink(outerA), Shrink(outerB), z, color.r, color.g, color.b, 255, 1.f, color.a);
            }

            // 0x743508 - NOTE: Framerate dependent
            const uint32 time = crossHair.m_TimeWhenToDeactivate;
            if (time != 0 && time != CROSSHAIR_MARKED) { // Being cleared: spin fast, grow and fade
                crossHair.m_Rotation += 0.75f;
                pulse += 2.f;
                crossHair.m_Size *= 0.9f;
            } else {
                crossHair.m_Rotation += 0.05f;
            }
            if (crossHair.m_Rotation > TWO_PI) {
                crossHair.m_Rotation = 0.f;
            }
            if (time == 0) { // 0x74355F
                if (s_IsCrossHairPulseGrowing[i]) {
                    pulse += crossHair.m_Size * 2.f;
                    if (pulse > crossHair.m_Size * 20.f) {
                        s_IsCrossHairPulseGrowing[i] = false;
                    }
                } else {
                    pulse -= 2.f;
                    if (pulse < 0.f) {
                        s_IsCrossHairPulseGrowing[i] = true;
                    }
                }
            }
        }

        // 0x7438B3
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
        RestoreZStates();
    }

    if (const auto& cam = TheCamera.GetActiveCam(); cam.m_nMode == MODE_TWOPLAYER_IN_CAR_AND_SHOOTING) { // 0x743904
        SetRenderStatesForSprites();
        SetBlend(rwBLENDSRCALPHA, rwBLENDINVSRCALPHA);
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpCrossHairTex)));

        CSprite::RenderOneXLUSprite_Rotate_Aspect(
            CVector{ (cam.m_fX_Targetting + 1.f) * SCREEN_WIDTH * 0.5f, (cam.m_fY_Targetting + 1.f) * SCREEN_HEIGHT * 0.5f, 100.f },
            CVector2D{ 10.f, 10.f },
            255,
            128,
            0,
            128,
            SPRITE_RECIP_Z,
            GetSpinAngle(),
            255
        );

        // 0x743A0F - The shooter is the 1st player if they aren't driving, otherwise the 2nd one
        // NOTE: Not `IsInVehicle`, only the pointer is checked
        auto* const player1 = FindPlayerPed(0);
        auto* const shooter = player1->m_pVehicle && player1->m_pVehicle->GetDriver() != player1 ? player1 : FindPlayerPed(1);
        auto* const target  = shooter
            ? CWeapon::FindNearestTargetEntityWithScreenCoors(cam.m_fX_Targetting, cam.m_fY_Targetting, shooter->GetActiveWeapon().GetWeaponInfo(shooter).m_fWeaponRange * 2.f, shooter->GetPosition(), nullptr, nullptr)
            : nullptr;
        if (std::exchange(s_HeatSeekTarget, target) != target) {
            s_HeatSeekTargetChangeTime = CTimer::GetTimeInMS();
        }

        CVector   screenPos;
        CVector2D screenSize;
        if (target && CalcFlightSpriteCoors(target->GetPosition(), screenPos, screenSize)) { // 0x743ACD - The sprite closes on the target and gets brighter
            const int32 targetedForMs = (int32)(CTimer::GetTimeInMS() - s_HeatSeekTargetChangeTime);
            const float sizeMult      = std::max(3.f - (float)targetedForMs / 512.f, 1.f);
            const uint8 intensity     = (uint8)std::min(targetedForMs / 4 + 70, 255);
            CSprite::RenderOneXLUSprite_Rotate_Aspect(screenPos, screenSize * sizeMult, intensity, 0, 0, intensity, SPRITE_RECIP_Z, GetSpinAngle(), 255);
        }

        RestoreZStates();
    }

    for (auto&& [i, player] : rngv::enumerate(CWorld::Players)) { // 0x743BF8
        player.m_CrossHair.Render((int32)i);
    }
}
