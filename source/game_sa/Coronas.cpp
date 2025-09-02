#include "StdInc.h"
#include <reversiblebugfixes/Bugs.hpp>
#include "Coronas.h"

std::array<char[26], eCoronaType::CORONATYPE_COUNT> aCoronaSpriteNames = {
    "coronastar",
    "coronastar",
    "coronamoon",
    "coronareflect",
    "coronaheadlightline",
    "",
    "",
    "",
    "",
    "coronaringb"
}; // 0x8D4950

std::array<char[26], eCoronaType::CORONATYPE_COUNT> aCoronaSpriteNamesm = {
    "",
    "",
    "",
    "coronareflectm",
    "",
    "",
    "",
    "",
    "",
    ""
}; // 0x88E538

constexpr CFlareDefinition HeadLightsFlareDef[27]{
    { 4.00f,  5.0f, { 60, 60, 60 },    200, 4 },
    { 3.00f,  7.0f, { 40, 40, 40 },    200, 4 },
    { 2.00f,  5.0f, { 40, 40, 40 },    200, 4 },
    { 1.50f,  6.0f, { 90, 90, 90 },    200, 4 },
    { 1.25f,  5.0f, { 40, 40, 40 },    200, 4 },
    { 0.80f,  14.f, { 60, 60, 60 },    200, 4 },
    { 0.60f,  4.0f, { 40, 40, 40 },    200, 4 },
    { 0.25f,  10.f, { 60, 60, 60 },    200, 4 },
    { 0.10f,  6.0f, { 30, 30, 30 },    200, 4 },
    { 0.05f,  14.f, { 50, 50, 50 },    200, 4 },
    { -0.03f, 3.0f, { 30, 30, 30 },    200, 4 },
    { -0.10f, 6.0f, { 60, 60, 60 },    200, 4 },
    { -0.30f, 5.0f, { 30, 30, 30 },    200, 4 },
    { -0.40f, 60.f, { 30, 30, 30 },    200, 4 },
    { -0.55f, 4.0f, { 40, 40, 40 },    200, 4 },
    { -0.75f, 14.f, { 50, 50, 50 },    200, 4 },
    { -0.90f, 5.2f, { 35, 35, 35 },    200, 4 },
    { -1.00f, 11.f, { 55, 55, 55 },    200, 4 },
    { -1.20f, 3.5f, { 35, 35, 35 },    200, 4 },
    { -1.35f, 9.0f, { 50, 50, 50 },    200, 4 },
    { -1.70f, 54.f, { 35, 35, 35 },    200, 4 },
    { -2.00f, 5.0f, { 50, 50, 50 },    200, 4 },
    { -2.50f, 4.5f, { 35, 35, 35 },    200, 4 },
    { -3.00f, 14.f, { 50, 50, 50 },    200, 4 },
    { -6.00f, 24.f, { 70, 70, 70 },    200, 4 },
    { -9.00f, 14.f, { 70, 50, 70 },    200, 4 },
    { 0.00f,  0.0f, { 255, 255, 255 }, 255, 0 }
}; // 0x8D4D88

constexpr CFlareDefinition SunFlareDef[27]{
    { 4.00f,  8.00f, { 36, 30, 24 },    200, 4 },
    { 3.00f,  11.2f, { 24, 18, 15 },    200, 4 },
    { 2.00f,  8.00f, { 24, 12, 12 },    200, 4 },
    { 1.50f,  9.60f, { 54, 54, 48 },    200, 4 },
    { 1.25f,  8.00f, { 24, 24, 18 },    200, 4 },
    { 0.80f,  22.4f, { 36, 30, 24 },    200, 4 },
    { 0.60f,  6.40f, { 24, 15, 12 },    200, 4 },
    { 0.25f,  16.0f, { 36, 30, 30 },    200, 4 },
    { 0.10f,  9.60f, { 18, 18, 18 },    200, 4 },
    { 0.05f,  22.4f, { 36, 30, 24 },    200, 4 },
    { -0.03f, 4.80f, { 18, 18, 18 },    200, 4 },
    { -0.10f, 9.60f, { 42, 42, 42 },    200, 4 },
    { -0.30f, 8.00f, { 18, 6, 6 },      200, 4 },
    { -0.40f, 96.0f, { 18, 12, 9 },     200, 4 },
    { -0.55f, 6.40f, { 18, 18, 12 },    200, 4 },
    { -0.75f, 22.4f, { 42, 24, 18 },    200, 4 },
    { -0.90f, 8.32f, { 21, 12, 18 },    200, 4 },
    { -1.00f, 17.6f, { 42, 13, 18 },    200, 4 },
    { -1.20f, 5.60f, { 21, 12, 12 },    200, 4 },
    { -1.35f, 14.4f, { 42, 42, 24 },    200, 4 },
    { -1.70f, 86.8f, { 21, 15, 15 },    200, 4 },
    { -2.00f, 8.00f, { 48, 30, 30 },    200, 4 },
    { -2.50f, 7.20f, { 21, 15, 12 },    200, 4 },
    { -3.00f, 22.4f, { 42, 30, 24 },    200, 4 },
    { -6.00f, 38.4f, { 42, 42, 30 },    200, 4 },
    { -9.00f, 22.4f, { 42, 30, 36 },    200, 4 },
    { 0.00f,  0.00f, { 255, 255, 255 }, 255, 0 }
}; // 0x8D4B68

void CCoronas::InjectHooks() {
    RH_ScopedClass(CCoronas);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(RenderOutGeometryBufferForReflections, 0x6FAA10);
    RH_ScopedInstall(Init, 0x6FAA70);
    RH_ScopedInstall(Shutdown, 0x6FAB00);
    RH_ScopedInstall(Update, 0x6FADF0);
    RH_ScopedInstall(Render, 0x6FAEC0);
    RH_ScopedInstall(RenderReflections, 0x6FB630);
    RH_ScopedInstall(RenderSunReflection, 0x6FBAA0);
    RH_ScopedOverloadedInstall(RegisterCorona, "type", 0x6FC180, void(*)(uint32, CEntity*, uint8, uint8, uint8, uint8, const CVector&, float, float, RwTexture*, eCoronaFlareType, uint8, uint8, uint8, float, bool, float, bool, float, bool, bool));
    RH_ScopedOverloadedInstall(RegisterCorona, "texture", 0x6FC580, void(*)(uint32, CEntity*, uint8, uint8, uint8, uint8, const CVector&, float, float, eCoronaType, eCoronaFlareType, uint8, uint8, int32, float, bool, float, bool, float, bool, bool));

    RH_ScopedInstall(UpdateCoronaCoors, 0x6FC4D0);
    RH_ScopedInstall(DoSunAndMoon, 0x6FC5A0);
}

// Initialises coronas
// 0x6FAA70
void CCoronas::Init() {
    {
        CTxdStore::ScopedTXDSlot txd{ "particle" };
        for (auto&& [tex, name, maskName] : rng::zip_view{ gpCoronaTexture, aCoronaSpriteNames, aCoronaSpriteNamesm }) {
            if (!tex) {
                tex = RwTextureRead(name, maskName);
            }
        }
    }
    rng::fill(aCoronas, CRegisteredCorona{});
}

// Terminates coronas
// 0x6FAB00
void CCoronas::Shutdown() {
    // original used SunScreenX
    for (auto& t : gpCoronaTexture) {
        if (t) {
            RwTextureDestroy(std::exchange(t, nullptr));
        }
    }
}

enum CameraLookFlags : int32 {
    LOOK_LEFT  = 1 << 0,
    LOOK_RIGHT = 1 << 1,
    LOOK_BACK  = 1 << 2,
    LOOK_FRONT = 1 << 3
};

// Updates coronas
// 0x6FADF0
void CCoronas::Update() {
    ZoneScoped;

    static int32 LastCamLook; // 0xC3EF58
    
    LightsMult = std::min(CTimer::GetTimeStep() * 0.03f + LightsMult, 1.f);

    const auto& cc = TheCamera.m_aCams[TheCamera.m_nActiveCam];

    int32 CamLook = 0;
    if (cc.m_bLookingLeft) {
        CamLook |= LOOK_LEFT;
    }
    if (cc.m_bLookingRight) {
        CamLook |= LOOK_RIGHT;
    }
    if (cc.m_bLookingBehind) {
        CamLook |= LOOK_BACK;
    }
    if (!TheCamera.GetLookDirection()) {
        CamLook |= LOOK_FRONT;
    }

    if (std::exchange(LastCamLook, CamLook) == CamLook) {
        bChangeBrightnessImmediately = std::max(bChangeBrightnessImmediately - 1, 0);
    } else {
        bChangeBrightnessImmediately = 3;
    }

    for (auto& corona : aCoronas) {
        if (corona.IsActive()) {
            corona.Update();
        }
    }
}

// 0x6FAEC0
void CCoronas::Render() {
    ZoneScoped;

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));

    const auto raster     = RwCameraGetRaster(Scene.m_pRwCamera);
    const auto rasterSize = CVector2D{ (float)RwRasterGetWidth(raster), (float)RwRasterGetHeight(raster) };
    const auto rasterRect = CRect{ 0.f, 0.f, rasterSize.x, rasterSize.y };

    bool zTestEnable = true;
    for (auto& c : aCoronas) {
        if (!c.m_dwId) {
            continue;
        }

        if (!c.m_FadedIntensity && !c.m_Color.a) {
            continue;
        }

        const auto covidPos = c.GetPosition();

        //< 0x6FB009 - Get on-screen position, and check if it's on it
        CVector   onScrPos;
        CVector2D onScrSize;
        if (c.m_bOffScreen = !CSprite::CalcScreenCoors(covidPos, &onScrPos, &onScrSize.x, &onScrSize.y, true, true)) {
            continue;
        }

        c.m_bOffScreen = !rasterRect.IsPointInside(CVector2D{ onScrPos }); // Seems like a useless check

        // Already faded out
        if (c.m_FadedIntensity == 0) {
            continue;
        }

        // If outside farclip range, just ignore
        if (onScrPos.z >= c.m_fFarClip) {
            continue;
        }

        const auto rz = 1.f / onScrPos.z;

        //< 0x6FB0A7 - Start fading out at half the far clip distance
        const auto intensity = (int16)((float)c.m_FadedIntensity * c.CalculateIntensity(onScrPos.z, c.m_fFarClip));

        //< 0x6FB0D9 - Enable/disable Z test if necessary
        if (c.m_bCheckObstacles == zTestEnable) {
            zTestEnable = !zTestEnable;
            RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(zTestEnable));
        }

        const auto LerpColorC = [](uint8 cc, float t) {
            return (uint8)((float)cc * t);
        };

        //< 0x6FB131 - Render with texture
        if (c.m_pTexture) {
            RwRenderStateSet(rwRENDERSTATEZTESTENABLE,   RWRSTATE(TRUE));
            RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(c.m_pTexture)));

            //< 0x6FB122
            const auto scale = std::min(40.f, onScrPos.z) * CWeather::Foggyness / 40.f + 1.f;

            // NOP - The coordinates are later overwritten
            //if (c.m_dwId == 1) {
            //    scrPos.z = RwCameraGetFarClipPlane(Scene.m_pRwCamera) * 0.95f;
            //}

            //< 0x6FB24E
            if (CSprite::CalcScreenCoors(
                    covidPos - (covidPos - TheCamera.GetPosition()).Normalized() * c.m_fNearClip,
                    &onScrPos, // NOTE/BUG: Yeah, overwrites the previously calculated value. Not sure if it's intentional.
                    &onScrSize.x,
                    &onScrSize.y,
                    true,
                    true
                )) {
                CSprite::RenderOneXLUSprite_Rotate_Aspect(
                    onScrPos,
                    onScrSize * c.m_fSize * CVector2D{ 1.f, scale },
                    LerpColorC(c.m_Color.r, 1.f / scale),
                    LerpColorC(c.m_Color.g, 1.f / scale),
                    LerpColorC(c.m_Color.b, 1.f / scale),
                    intensity,
                    rz * 20.f,
                    0.f,
                    255
                );
            }
        }

        //< 0x6FB2F3 - Render flare
        if (c.m_nFlareType != FLARETYPE_NONE) {
            RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(FALSE));
            RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpCoronaTexture[CORONATYPE_SHINYSTAR])));

            //< 0x6FB35B
            const auto colorVariationMult = CGeneral::GetRandomNumberInRange(0.7f, 1.f) * (float)c.m_FadedIntensity;

            //< 0x6FB2FC [Moved here]
            auto it = [&] {
                switch (c.m_nFlareType) {
                case FLARETYPE_SUN:        return &SunFlareDef[0];
                case FLARETYPE_HEADLIGHTS: return &HeadLightsFlareDef[0];
                default:                   NOTSA_UNREACHABLE();
                }
            }();

            //< 0x6FB46C
            for (; it->Sprite; it++) {
                CEntity* hitEntity;
                CColPoint hitCP;
                if (!CWorld::ProcessLineOfSight(
                        covidPos,
                        TheCamera.GetPosition(),
                        hitCP,
                        hitEntity,
                        false,
                        true,
                        true,
                        false,
                        false,
                        false,
                        false,
                        true
                    )) { //< 0x6FB409
                    CRGBA color = {
                        static_cast<uint8>(static_cast<float>(c.m_Color.r) * colorVariationMult * it->ColorMult.x),
                        static_cast<uint8>(static_cast<float>(c.m_Color.g) * colorVariationMult * it->ColorMult.y),
                        static_cast<uint8>(static_cast<float>(c.m_Color.b) * colorVariationMult * it->ColorMult.z),
                        255
                    };
                    CSprite::RenderBufferedOneXLUSprite2D(
                        lerp(rasterSize / 2.f, CVector2D{ onScrPos }, it->Position),
                        CVector2D{ it->Size, it->Size } * 4.f,
                        color,
                        255,
                        255
                    );
                }
            }

            RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(TRUE));

            //< 0x6FB483
            if (c.m_nFlareType == FLARETYPE_HEADLIGHTS && CWeather::HeadLightsSpectrum != 0.f && CGame::CanSeeOutSideFromCurrArea()) {
                for (auto it = HeadLightsFlareDef; it->Sprite; it++) {
                    const auto RenderFlareSprite = [&,
                                                    spriteIntensity = (int16)((float)intensity * it->IntensityMult)](RwRGBA clr, float posOffset) {
                        CSprite::RenderBufferedOneXLUSprite2D(
                            lerp(rasterSize / 2.f, CVector2D{ onScrPos }, it->Position + posOffset),
                            CVector2D{ it->Size, it->Size },
                            clr,
                            spriteIntensity,
                            255
                        );
                    };
                    RenderFlareSprite({ LerpColorC(c.m_Color.r, it->ColorMult.x * CWeather::HeadLightsSpectrum), 0, 0, 255 }, +0.05f); // 0x6FB561
                    RenderFlareSprite({ 0, 0, LerpColorC(c.m_Color.b, it->ColorMult.z * CWeather::HeadLightsSpectrum), 255 }, -0.05f); // 0x6FB5EA
                }
            }
        }
    }
    CSprite::FlushSpriteBuffer();

    /* NOTE/BUG: Renderstates not restored? */
}

// 0x6FB630
void CCoronas::RenderReflections() {
    if (s_DebugSettings.DisableWetRoadReflections) {
        return;
    }

    if (!s_DebugSettings.AlwaysRenderWetRoadReflections) {
        // Check if the roads are wet enough
        if (CWeather::WetRoads <= 0.f) {
            for (auto& c : aCoronas) {
                c.m_bHasValidHeightAboveGround = false;
            }
            return;
        }
    }

    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(RwTextureGetRaster(gpCoronaTexture[CORONATYPE_REFLECTION])));

    const auto camPos = TheCamera.GetPosition();
    for (auto&& [i, c] : rngv::enumerate(aCoronas)) {
        const auto covidPos = c.GetPosition();
        if (!c.m_bHasValidHeightAboveGround || ((i & 0xFF) + (CTimer::GetFrameCounter() & 0xFF) % 16) == 0) { //< Simplified code
            bool bGroundFound;
            const auto groundZ = CWorld::FindGroundZFor3DCoord(covidPos, &bGroundFound);
            if (bGroundFound) { // NOTE/BUG: Weird.. Why not set it according to `bGroundFound` directly?
                c.m_bHasValidHeightAboveGround = true;
                c.m_fHeightAboveGround         = covidPos.z - groundZ;
            }
        }
        if (!c.m_bHasValidHeightAboveGround) {
            continue;
        }
        if (c.m_bHasValidHeightAboveGround >= 20.f) {
            continue;
        }
        if (covidPos.z - c.m_fHeightAboveGround >= camPos.z) {
            continue;
        }
        CVector   onScrPos;
        CVector2D onScrSize;
        if (!CSprite::CalcScreenCoors(covidPos - CVector{ 0.f, 0.f, 2 * c.m_fHeightAboveGround }, &onScrPos, &onScrSize.x, &onScrSize.y, true, true)) {
            continue;
        }
        const auto clampedFarClip = std::min(55.f, c.m_fFarClip * 0.75f);
        if (onScrPos.z >= clampedFarClip) {
            continue;
        }
        const auto LerpColorC = [t = (s_DebugSettings.AlwaysRenderWetRoadReflections ? 1.f : CWeather::WetRoads)
                                     * c.CalculateIntensity(onScrPos.z, clampedFarClip)
                                     * invLerp(20.f, 0.f, c.m_fHeightAboveGround)
                                     * 230.f](uint8 cc) {
            return (uint8)((uint16)((float)cc * t) >> 8 & 0xFF); // divide by 256
        };
        CSprite::RenderBufferedOneXLUSprite(
            { onScrPos.x, onScrPos.y, notsa::bugfixes::PS2CoronaRendering ? onScrPos.z : RwIm2DGetNearScreenZMacro() },
            onScrSize * CVector2D{ 0.75f, 2.f } * c.m_fSize,
            LerpColorC(c.m_Color.r),
            LerpColorC(c.m_Color.g),
            LerpColorC(c.m_Color.b),
            128,
            1.f / RwCameraGetNearClipPlane(Scene.m_pRwCamera),
            255
        );
    }
    CSprite::FlushSpriteBuffer();

    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
}

// 0x6FAA10
void CCoronas::RenderOutGeometryBufferForReflections() {
    if (uiTempBufferIndicesStored) {
        LittleTest();
        if (RwIm3DTransform(TempBufferVertices.m_3d, uiTempBufferVerticesStored, 0, 1u)) {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, aTempBufferIndices, uiTempBufferIndicesStored);
            RwIm3DEnd();
        }
        uiTempBufferVerticesStored = 0;
        uiTempBufferIndicesStored  = 0;
    }
}

// 0x6FBAA0
void CCoronas::RenderSunReflection() {
    constexpr auto REFLECTION_SIZE   = 60.f;
    constexpr auto REFLECTION_PERIOD = 2'048; // Code is faster if this is a power-of-2

    const auto vecToSun3D = CTimeCycle::m_VectorToSun[CTimeCycle::m_CurrentStoredValue];

    if (vecToSun3D.z <= -0.05f) {
        return;
    }

    const auto camPos = TheCamera.GetPosition();

    auto t =
        invLerp(0.3f, 0.f, std::abs(vecToSun3D.z - 0.25f))
        * (1.f - CWeather::CloudCoverage)
        * (1.f - CWeather::Foggyness)
        * (1.f - CWeather::Wind);
    if (t <= 0.f) {
        return;
    }
    for (const auto& v : std::initializer_list<CVector>{
  // TODO: Magic numberz
             { 611.0f,  875.0f,  0.f },
             { -929.0f, 2364.f,  0.f },
             { -1034.f, 2640.f,  0.f },
             { 2372.f,  -1854.f, 0.f },
             { -1633.f, 106.0f,  0.f }
    }) {
        t *= std::clamp(invLerp(0.f, 100.f, (camPos - v).Magnitude2D() - 250.f), 0.f, 1.f);
    }
    t *= 0.25f;

    const auto center2D         = CVector2D{ vecToSun3D } * 40.f + CVector2D{ camPos };
    const auto vecToSun2D       = Normalized2D(vecToSun3D);
    const auto vecToSunCore2D   = vecToSun2D * (REFLECTION_SIZE / 2.f);
    const auto vecToSunCorona2D = vecToSun2D * REFLECTION_SIZE;

#define CalcColorC(_color) \
    (uint8)((float)(CTimeCycle::m_CurrentColours.m_nSunCorona##_color + CTimeCycle::m_CurrentColours.m_nSunCore##_color) * t)
    const auto PushVertex = [
        &,
                             color = CRGBA{ CalcColorC(Red), CalcColorC(Green), CalcColorC(Blue), 255 },
        posZ = CWeather::Wind * 0.5f + 0.1f
    ](CVector2D offsetToCenter, CVector2D uv) {
        RenderBuffer::PushVertex(CVector{ center2D + offsetToCenter, posZ }, uv, color);
    };
#undef CalcColorC

    RenderBuffer::ClearRenderBuffer();

    // The part closer to the camera
    {
        RenderBuffer::PushIndices({ 2, 1, 0, 2, 3, 1 }, false);

        PushVertex(vecToSunCore2D.GetPerpRight(),                    { 0.0f, 1.0f });
        PushVertex(vecToSunCore2D.GetPerpLeft(),                     { 1.0f, 1.0f });
        PushVertex(vecToSunCorona2D + vecToSunCore2D.GetPerpRight(), { 0.0f, 0.5f });
        PushVertex(vecToSunCorona2D + vecToSunCore2D.GetPerpLeft(),  { 1.0f, 0.5f });
    }

    //< 0x6FBF36 - The part that is changing position [To imitate how the water is flowing]
    const auto time = CTimer::GetTimeInMS();
    for (auto i = 0u; i < 20u; i++) {
        RenderBuffer::PushIndices({ 0, -1, -2, 0, 1, -1 }, true); // Same as the above indices, but with 2 subtracted from each

        const auto forward = vecToSun2D * (
              std::sin((float)((time + 900 * i) % REFLECTION_PERIOD) / (float)REFLECTION_PERIOD * TWO_PI) * 10.f
            + (float)(i * 970 / 20 + 30)
        );
        const auto offset = vecToSun2D * (float)(i * 1'440 / 20 + 60);

        PushVertex(offset + forward.GetPerpRight(), { 0.0f, 0.5f });
        PushVertex(offset + forward.GetPerpLeft(),  { 1.0f, 0.5f });
    }

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEFOGTYPE,           RWRSTATE(rwFOGTYPELINEAR));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(RwTextureGetRaster(gpCoronaTexture[CORONATYPE_HEADLIGHTLINE])));

    RenderBuffer::RenderStuffInBuffer();

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
}

// Registers a corona effect with a custom texture.
// Creates or updates a light corona in the game world with specified properties.
// 0x6FC180
void CCoronas::RegisterCorona(
    uint32           id,
    CEntity*         attachTo,
    uint8            red,
    uint8            green,
    uint8            blue,
    uint8            intensity,
    const CVector&   pos,
    float            size,
    float            range,
    RwTexture*       texture,
    eCoronaFlareType flareType,
    uint8            reflType,
    uint8            checkLOS,
    uint8            usesTrails,
    float            normalAngle,
    bool             neonFade,
    float            pullTowardsCam,
    bool             fullBrightAtStart,
    float            fadeSpeed,
    bool             onlyFromBelow,
    bool             whiteCore
) {
    const auto worldPos = attachTo
        ? attachTo->GetMatrix().TransformPoint(pos)
        : pos;

    const auto& camPos = TheCamera.GetPosition();
    if (sq(range) < (camPos - worldPos).SquaredMagnitude2D()) {
        return; // Corona is beyond far clip distance
    }

    // 0x6FC24A
    // Adjust intensity for neon fade effect if enabled
    uint8 adjustedAlpha = intensity;
    if (neonFade) {
        float distance3d = (camPos - worldPos).Magnitude();
        if (distance3d < 35.0f) {
            return;
        }
        if (distance3d < 50.0f) {
            adjustedAlpha *= uint8((distance3d + -35.0f) / 15.0f);
        }
    }

    // Find or allocate a corona slot
    CRegisteredCorona* corona = GetCoronaByID(id);
    if (corona) {
        if (!corona->m_Color.a && !adjustedAlpha) {
            corona->SetInactive();
            --NumCoronas;
            return;
        }
    } else { /* allocate new */
        corona = GetFree();
        if (!corona) {
            return;
        }

        // 0x6FC33D
        // Initialize new corona in free slot
        corona->m_FadedIntensity             = fullBrightAtStart ? 255 : 0;
        corona->m_bOffScreen                 = true;
        corona->m_bHasValidHeightAboveGround = false;
        corona->m_bJustCreated               = true;
        corona->m_dwId                       = id;
        NumCoronas++;
    }

    // Update corona properties
    corona->m_Color.Set(red, green, blue, adjustedAlpha);
    corona->m_vPosn                = pos;
    corona->m_fSize                = size;
    corona->m_fAngle               = normalAngle;
    corona->m_bRegisteredThisFrame = true;
    corona->m_fFarClip             = range;
    corona->m_fNearClip            = pullTowardsCam;
    corona->m_pTexture             = texture;
    corona->m_nFlareType           = flareType;
    corona->m_bUsesReflection      = reflType;
    corona->m_bCheckObstacles      = checkLOS;
    corona->m_fFadeSpeed           = fadeSpeed;

    // 0x6FC401
    corona->m_bFlashWhileFading  = neonFade;
    corona->m_bOnlyFromBelow     = onlyFromBelow;
    corona->m_bDrawWithWhiteCore = whiteCore;

    // Handle attachment to entity
    if (attachTo) {
        corona->m_bAttached   = true;
        corona->m_pAttachedTo = attachTo;
        CEntity::SetEntityReference(corona->m_pAttachedTo, attachTo);
    } else {
        corona->m_bAttached   = false;
        corona->m_pAttachedTo = nullptr;
    }
}

// Registers a corona effect using a predefined corona type.
// Delegates to the main RegisterCorona function with a texture from the type.
// 0x6FC580
void CCoronas::RegisterCorona(uint32 id, CEntity* attachTo, uint8 red, uint8 green, uint8 blue, uint8 intensity, const CVector& pos, float size, float range, eCoronaType coronaType, eCoronaFlareType flareType, uint8 reflType, uint8 checkLOS, int32 usesTrails, float normalAngle, bool neonFade, float pullTowardsCam, bool fullBrightAtStart, float fadeSpeed, bool onlyFromBelow, bool whiteCore) {
    RegisterCorona(id, attachTo, red, green, blue, intensity, pos, size, range, gpCoronaTexture[coronaType], flareType, reflType, checkLOS, usesTrails, normalAngle, neonFade, pullTowardsCam, fullBrightAtStart, fadeSpeed, onlyFromBelow, whiteCore);
}

// 0x6FC4D0
void CCoronas::UpdateCoronaCoors(uint32 id, const CVector& pos, float range, float normalAngle) {
    if (sq(range) >= (TheCamera.GetPosition() - pos).SquaredMagnitude2D()) {
        if (auto* corona = GetCoronaByID(id)) {
            corona->m_vPosn = pos;
            corona->m_fAngle = normalAngle;
        }
    }
}

// Draw sun (Moon went to CClouds since SA)
// 0x6FC5A0
void CCoronas::DoSunAndMoon() {
    ZoneScoped;

    if (!CGame::CanSeeOutSideFromCurrArea()) {
        return;
    }

    const auto vecToSun  = CTimeCycle::GetVectorToSun();
    const auto coronaPos = vecToSun * (CDraw::GetFarClipZ() * 0.95f) + TheCamera.GetPosition();

    if (vecToSun.z > -0.1f) {
        const auto DoRegisterCorona = [coronaPos](uint32 id, eCoronaFlareType ftype, float radius, bool coronaColor) {
            const auto& cc = CTimeCycle::m_CurrentColours;
            RegisterCorona(
                id,
                nullptr,
                coronaColor ? (uint8)cc.m_nSunCoronaRed : (uint8)cc.m_nSunCoreRed,
                coronaColor ? (uint8)cc.m_nSunCoronaGreen : (uint8)cc.m_nSunCoreGreen,
                coronaColor ? (uint8)cc.m_nSunCoronaBlue : (uint8)cc.m_nSunCoreBlue,
                255u,
                coronaPos,
                cc.m_fSunSize * radius,
                999999.88f,
                gpCoronaTexture[CORONATYPE_SHINYSTAR],
                ftype,
                false,
                coronaColor,
                0,
                0.f,
                false,
                1.5f,
                0,
                15.f,
                false,
                false
            );
        };
        DoRegisterCorona(1, FLARETYPE_NONE, 2.7335f, false);
        if (vecToSun.z > 0.f) { // Removed redudant check
            DoRegisterCorona(2, FLARETYPE_SUN, 6.f, true);
        }
    }

    // Debug code here

    CVector screenPos;
    float pX, pY;
    if (CSprite::CalcScreenCoors(coronaPos, &screenPos, &pX, &pY, true, true)) {
        SunScreenX = screenPos.x;
        SunScreenY = screenPos.y;
    } else {
        SunScreenY = 1000000.0f;
        SunScreenX = 1000000.0f;
    }
}

// Inlined, code from 0x6FC524 to 0x6FC53C
inline CRegisteredCorona* CCoronas::GetCoronaByID(int32 id) {
    for (auto& corona : aCoronas) {
        if (corona.m_dwId == id) {
            return &corona;
        }
    }
    return nullptr;
}

// Inlined, code from 0x6FC309 to 0x6FC31E
inline CRegisteredCorona* CCoronas::GetFree() {
    for (auto& corona : aCoronas) {
        if (!corona.IsActive()) {
            return &corona; /* Caller has to increase `NumCoronas` */
        }
    }
    return nullptr;
}
