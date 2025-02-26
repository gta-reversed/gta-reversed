#include "StdInc.h"

#include "Clouds.h"
#include "PostEffects.h"

float& CClouds::m_fVolumetricCloudMaxDistance = *reinterpret_cast<float*>(0xC6AA58);
uint32& CClouds::m_VolumetricCloudsUsedNum = *reinterpret_cast<uint32*>(0xC6AA5C);

float& CClouds::ms_cameraRoll = *reinterpret_cast<float*>(0xC6AA64);

int32& CClouds::IndividualRotation = *reinterpret_cast<int32*>(0xC6AA6C);
float& CClouds::CloudRotation = *reinterpret_cast<float*>(0xC6AA70);

tVolumetricClouds& CClouds::ms_vc = *reinterpret_cast<tVolumetricClouds*>(0xC6AAB0);

tMovingFog& CClouds::ms_mf = *reinterpret_cast<tMovingFog*>(0xC6C158);

CVector& CClouds::PlayerCoords = *reinterpret_cast<CVector*>(0xC6E958); // gVecPlayerCoors
CVector& CClouds::CameraCoors = *reinterpret_cast<CVector*>(0xC6E964);  // gVecCameraCoors

float& CurrentFogIntensity = *reinterpret_cast<float*>(0x8D5798);

RwTexture*& gpMoonMask = *reinterpret_cast<RwTexture**>(0xC6AA74);
RwTexture*& gpCloudTex = *reinterpret_cast<RwTexture**>(0xC6AA78);
RwTexture*& gpCloudMaskTex = *reinterpret_cast<RwTexture**>(0xC6AA78 + 0x4);

void CClouds::InjectHooks() {
    RH_ScopedClass(CClouds);
    RH_ScopedCategoryGlobal();

    // Clouds
    RH_ScopedInstall(Init, 0x7138D0);
    RH_ScopedInstall(Update, 0x712FF0);
    RH_ScopedInstall(Shutdown, 0x712FA0);
    RH_ScopedInstall(SetUpOneSkyPoly, 0x713060);
    RH_ScopedInstall(Render, 0x713950);
    RH_ScopedInstall(RenderSkyPolys, 0x714650);
    RH_ScopedInstall(RenderBottomFromHeight, 0x7154B0, { .reversed = false });

    // Moving fog
    RH_ScopedInstall(MovingFogInit, 0x713660);
    RH_ScopedInstall(MovingFog_Create, 0x713760);
    RH_ScopedInstall(MovingFog_Delete, 0x713730);
    RH_ScopedInstall(MovingFog_Update, 0x716B10);
    RH_ScopedInstall(MovingFog_GetFXIntensity, 0x7136D0);
    RH_ScopedInstall(MovingFog_GetWind, 0x7136E0);
    RH_ScopedInstall(MovingFog_GetFirstFreeSlot, 0x713710);
    RH_ScopedInstall(MovingFogRender, 0x716C90);

    // Volumetric clouds
    RH_ScopedInstall(VolumetricCloudsInit, 0x7131C0);
    RH_ScopedInstall(VolumetricClouds_Create, 0x715F40);
    RH_ScopedInstall(VolumetricClouds_Delete, 0x7135F0);
    RH_ScopedInstall(VolumetricClouds_GetFirstFreeSlot, 0x7135C0);
    RH_ScopedInstall(VolumetricCloudsGetMaxDistance, 0x713630);
    RH_ScopedInstall(VolumetricCloudsRender, 0x716380);
}

// 0x7138D0
void CClouds::Init() {
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("particle"));
    gpCloudTex = RwTextureRead("cloud1", nullptr);
    gpCloudMaskTex = RwTextureRead("cloudmasked", nullptr);
    gpMoonMask = RwTextureRead("lunar", "lunarm");
    ms_vc.texture = RwTextureRead("cloudhigh", "cloudhighm");
    CTxdStore::PopCurrentTxd();
    CloudRotation = 0.0f;
    VolumetricCloudsInit();
    MovingFogInit();
}

// 0x712FF0
void CClouds::Update() {
    ZoneScoped;

    CloudRotation = std::sin(TheCamera.m_fOrientation - 0.85f) * CWeather::Wind * 0.001f + CloudRotation;
    IndividualRotation += (int32)((CTimer::GetTimeStep() * CWeather::Wind * 0.5f + 0.3f) * 60.0f);
}

// 0x712FA0
void CClouds::Shutdown() {
    RwTextureDestroy(gpCloudTex);
    gpCloudTex = nullptr;

    RwTextureDestroy(gpCloudMaskTex);
    gpCloudMaskTex = nullptr;

    RwTextureDestroy(ms_vc.texture);
    ms_vc.texture = nullptr;
}

// 0x713060
void CClouds::SetUpOneSkyPoly(CVector vert1pos, CVector vert2pos, CVector vert3pos, CVector vert4pos, uint8 topRed, uint8 topGreen, uint8 topBlue,
                              uint8 bottomRed, uint8 bottomGreen, uint8 bottomBlue) {
    uint16 uiStartVertex = uiTempBufferVerticesStored;
    uint16 iStartIndex = uiTempBufferIndicesStored;

    RwIm3DVertexSetRGBA(&TempBufferVertices.m_3d[uiStartVertex + 0], topRed, topGreen, topBlue, 0xFF);
    RwIm3DVertexSetPos(&TempBufferVertices.m_3d[uiStartVertex + 0], vert1pos.x, vert1pos.y, vert1pos.z);
    RwIm3DVertexSetU(&TempBufferVertices.m_3d[uiStartVertex + 0], 0.f);
    RwIm3DVertexSetV(&TempBufferVertices.m_3d[uiStartVertex + 0], 0.f);

    RwIm3DVertexSetRGBA(&TempBufferVertices.m_3d[uiStartVertex + 1], topRed, topGreen, topBlue, 0xFF);
    RwIm3DVertexSetPos(&TempBufferVertices.m_3d[uiStartVertex + 1], vert2pos.x, vert2pos.y, vert2pos.z);
    RwIm3DVertexSetU(&TempBufferVertices.m_3d[uiStartVertex + 1], 0.f);
    RwIm3DVertexSetV(&TempBufferVertices.m_3d[uiStartVertex + 1], 0.f);

    RwIm3DVertexSetRGBA(&TempBufferVertices.m_3d[uiStartVertex + 2], bottomRed, bottomGreen, bottomBlue, 0xFF);
    RwIm3DVertexSetPos(&TempBufferVertices.m_3d[uiStartVertex + 2], vert3pos.x, vert3pos.y, vert3pos.z);
    RwIm3DVertexSetU(&TempBufferVertices.m_3d[uiStartVertex + 2], 0.f);
    RwIm3DVertexSetV(&TempBufferVertices.m_3d[uiStartVertex + 2], 0.f);

    RwIm3DVertexSetRGBA(&TempBufferVertices.m_3d[uiStartVertex + 3], bottomRed, bottomGreen, bottomBlue, 0xFF);
    RwIm3DVertexSetPos(&TempBufferVertices.m_3d[uiStartVertex + 3], vert4pos.x, vert4pos.y, vert4pos.z);
    RwIm3DVertexSetU(&TempBufferVertices.m_3d[uiStartVertex + 3], 0.f);
    RwIm3DVertexSetV(&TempBufferVertices.m_3d[uiStartVertex + 3], 0.f);

    aTempBufferIndices[iStartIndex + 0] = uiStartVertex;
    aTempBufferIndices[iStartIndex + 1] = uiStartVertex + 2;
    aTempBufferIndices[iStartIndex + 2] = uiStartVertex + 1;
    aTempBufferIndices[iStartIndex + 3] = uiStartVertex + 1;
    aTempBufferIndices[iStartIndex + 4] = uiStartVertex + 2;
    aTempBufferIndices[iStartIndex + 5] = uiStartVertex + 3;

    uiTempBufferIndicesStored += 6;
    uiTempBufferVerticesStored += 4;
}

// 0x713660
void CClouds::MovingFogInit() {
    ms_mf = {};
    ms_mf.m_vecWind = CVector(0.06f, 0.06f, 0.0f);
    ms_mf.m_nPrimIndices = { 0, 1, 2, 0, 2, 3 };
}

// 0x713760
void CClouds::MovingFog_Create(CVector* posn) {
    int32 slotId = MovingFog_GetFirstFreeSlot();
    if (slotId == -1)
        return;

    ms_mf.m_vecPosn[slotId] = CVector{
        CGeneral::GetRandomNumberInRange(-58.0f, 58.0f),
        CGeneral::GetRandomNumberInRange(-58.0f, 58.0f),
        CGeneral::GetRandomNumberInRange(-5.0f,  5.0f)
    };
    ms_mf.m_vecPosn[slotId] += *posn;
    ms_mf.m_fSize[slotId] = CGeneral::GetRandomNumberInRange(4.0f, 10.0f);
    ms_mf.m_fIntensity[slotId] = 1.0f;
    ms_mf.m_fMaxIntensity[slotId] = CGeneral::GetRandomNumberInRange(8.0f, 20.0f);
    ms_mf.m_Speed[slotId] = CGeneral::GetRandomNumberInRange(0.5f, 1.2f);
    ms_mf.m_bFogSlots[slotId] = true;
}

// 0x713730
void CClouds::MovingFog_Delete(int32 fogSlotIndex) {
    fogSlotIndex = std::clamp(fogSlotIndex, 0, MAX_MOVING_FOG - 1);
    ms_mf.m_bFogSlots[fogSlotIndex] = false;
}

// 0x716B10
void CClouds::MovingFog_Update() {
    if (MovingFog_GetFXIntensity() == 0.f)
        return;

    CVector camPos = TheCamera.GetPosition();
    for (int32 i = 0; i < MAX_MOVING_FOG; i++) {
        if (!ms_mf.m_bFogSlots[i]) {
            MovingFog_Create(&camPos);
            continue;
        }

        CVector& fogPosn = ms_mf.m_vecPosn[i];
        CVector  offset  = fogPosn - camPos;

        fogPosn.x += MovingFog_GetWind().x * ms_mf.m_Speed[i];
        fogPosn.y += MovingFog_GetWind().y * ms_mf.m_Speed[i];

        if (offset.Magnitude() <= 60.f)
        {
            ms_mf.m_fIntensity[i] = std::min(ms_mf.m_fIntensity[i] + CTimer::GetTimeStep(), ms_mf.m_fMaxIntensity[i]);
        }
        else
        {
            ms_mf.m_fIntensity[i] -= CTimer::GetTimeStep();
            if (ms_mf.m_fIntensity[i] <= 0.f) {
                MovingFog_Delete(i);
            }
        }
    }
}

// 0x7136D0
float CClouds::MovingFog_GetFXIntensity() {
    return CWeather::Foggyness_SF;
}

// 0x7136E0
CVector CClouds::MovingFog_GetWind() {
    return ms_mf.m_vecWind;
}

// 0x713710
int32 CClouds::MovingFog_GetFirstFreeSlot() {
    int32 result = 0;
    while (ms_mf.m_bFogSlots[result]) {
        if (++result >= MAX_MOVING_FOG)
            return -1;
    }
    return result;
}

// 0x716C90
void CClouds::MovingFogRender() {
    ZoneScoped;

    if (MovingFog_GetFXIntensity() == 0.f || !CGame::CanSeeOutSideFromCurrArea() && FindPlayerPed()->m_nAreaCode != AREA_CODE_NORMAL_WORLD)
        return;

    // Adjust fog intensity
    {
        const float step = CTimer::GetTimeStep() / 300.f;
        if (CCullZones::CamNoRain() && CCullZones::PlayerNoRain())
            CurrentFogIntensity = std::max(CurrentFogIntensity - step, 0.f); // Decreasing [towards 0]
        else
            CurrentFogIntensity = std::min(CurrentFogIntensity + step, 1.f); // Increasing [towards 1]

        if (CWeather::UnderWaterness >= CPostEffects::m_fWaterFXStartUnderWaterness) {
            CurrentFogIntensity = 0.f;
            return;
        }

        if (CurrentFogIntensity == 0.f) {
            return;
        }
    }

    CVector camUp = TheCamera.GetUpVector(), camRight = TheCamera.GetRightVector();

    CPostEffects::ImmediateModeRenderStatesStore();
    CPostEffects::ImmediateModeRenderStatesSet();

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,   RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpCloudMaskTex)));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));

    const int32 red   = std::min(CTimeCycle::m_CurrentColours.m_nSkyBottomRed + 132, 255);
    const int32 green = std::min(CTimeCycle::m_CurrentColours.m_nSkyBottomGreen + 132, 255);
    const int32 blue  = std::min(CTimeCycle::m_CurrentColours.m_nSkyBottomBlue + 132, 255);

    int32 numVerts = 0;
    const auto RenderVertices = [&] {
        if (RwIm3DTransform(TempBufferVertices.m_3d, numVerts, nullptr, rwIM3D_VERTEXXYZ | rwIM3D_VERTEXUV)) {
            RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
            RwIm3DEnd();
        }
        numVerts = 0;
    };

    for (auto fogIdx = 0u; fogIdx < MAX_MOVING_FOG; fogIdx++) {
        if (!ms_mf.m_bFogSlots[fogIdx])
            continue;

        const auto&   pos      = ms_mf.m_vecPosn[fogIdx];
        const float   halfSize = ms_mf.m_fSize[fogIdx] / 2.f;
        const CVector fogUp    = camUp * halfSize;
        const CVector fogRight = camRight * halfSize;

        // Original code used a switch case, we're going to use a lookup table to make it nicer. (And faster)
        const struct { CVector pos; RwTexCoords uv; } corners[]{
            { pos + fogRight + fogUp,  {0.f, 0.f} }, // Top right
            { pos + fogRight - fogUp,  {1.f, 0.f} }, // Bottom right
            { pos - fogRight - fogUp,  {1.f, 1.f} }, // Bottom left
            { pos - fogRight + fogUp,  {0.f, 1.f} }, // Top left
        };

        const auto alpha = static_cast<int32>(MovingFog_GetFXIntensity() * ms_mf.m_fIntensity[fogIdx] * CurrentFogIntensity);
        for (const auto& vertIdx : ms_mf.m_nPrimIndices) {
            const auto& corner = corners[vertIdx];

            auto& vert = TempBufferVertices.m_3d[numVerts++];
            RwV3dAssign(RwIm3DVertexGetPos(&vert), &corner.pos);
            RwIm3DVertexSetRGBA(&vert, red, green, blue, alpha);
            RwIm3DVertexSetU(&vert, corner.uv.u);
            RwIm3DVertexSetV(&vert, corner.uv.v);

            // Flush buffer if it's getting full
            if (numVerts == TOTAL_TEMP_BUFFER_3DVERTICES - 2) {
                RenderVertices();
            }
        }
    }

    // Render all remaining (if any)
    if (numVerts > 0) {
        RenderVertices();
    }

    CPostEffects::ImmediateModeRenderStatesReStore();
    MovingFog_Update();
}

uint8 CalculateColorWithBalance(uint8 blue, float colorBalance) {
    return lerp<uint8>(blue, 0u, colorBalance);
}

// From `CClouds::Render` [0x7139B2 - 0x713D2A]
void CClouds::Render_MaybeRenderMoon(float colorBalance) {
    // 3D position offset of the moon relative to the camera
    constexpr auto CAMERA_TO_CLOUD_OFFSET = CVector{ 0.f, -100.f, 15.f };

    // How big the [moon] mask texture is relative to the actual moon
    constexpr auto MOON_TO_MASK_SIZE_MULT = 1.7f;

    // Time range the moon is visible in
    // The default value [220] means the moon is visible starting at
    // 220 minutes before noon, and until 220 minutes after it
    // So, that's 20:20 - 03:40
    constexpr auto MOON_VISIBILITY_RANGE_MINS = 3u * 60u + 40u;

    // Unused
    //const auto clckHrs  = CClock::ms_nGameClockHours;
    //const auto clckMins = CClock::ms_nGameClockMinutes;

    if (!s_DebugSettings.Moon.Enabled) {
        return;
    }

    const auto moonVisibilityTimeMins = (size_t)std::abs(CClock::GetMinutesToday() - (float)MOON_VISIBILITY_RANGE_MINS);
    if (!s_DebugSettings.Moon.Force) {
        if (moonVisibilityTimeMins >= MOON_VISIBILITY_RANGE_MINS) { // Check is the moon not visible at the current time
            return;
        }
    }

    const auto colorB  = MOON_VISIBILITY_RANGE_MINS - moonVisibilityTimeMins;
    static_assert(MOON_VISIBILITY_RANGE_MINS == 220); // NOTE/TODO: The above will break otherwise, as it's really just a clever trick to avoid a more complex solution
    const auto colorRG = CalculateColorWithBalance(colorB, colorBalance);
    if (colorRG == 0) {
        return;
    }

    //
    // Calculate moon position on the screen [From the 3D position]
    //
    CVector   moonPosScr;
    CVector2D scrSize;
    if (!CSprite::CalcScreenCoors(TheCamera.GetPosition() + CAMERA_TO_CLOUD_OFFSET, &moonPosScr, &scrSize.x, &scrSize.y, false, true)) {
        return;
    }

    const auto z   = CDraw::GetFarClipZ();
    const auto rhw = 1.f / z;

    //
    // Draw black [textureless] sprite
    //
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,      RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,     RWRSTATE(rwBLENDONE));

    const auto moonSz = scrSize * ((float)CCoronas::MoonSize * 2.f + 4.f);
    CSprite::RenderOneXLUSprite(
        { moonPosScr.x, moonPosScr.y, z },
        moonSz,
        0, 0, 0, 255,
        rhw,
        255,
        0,
        0
    );

    //
    // Draw moon's mask
    //
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpMoonMask)));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,      RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,     RWRSTATE(rwBLENDONE));

    const auto moonMaskSz     = moonSz * MOON_TO_MASK_SIZE_MULT;
    const auto moonMaskPosScr = moonPosScr + moonMaskSz * CVector2D{
        0.7f,
        5.4f * (((float)CClock::GetGameClockDays() / 31.f - 0.5f)) // Slowly glide on the X axis according to current game day
    };
    CSprite::RenderOneXLUSprite(
        { moonMaskPosScr.x, moonMaskPosScr.y, z },
        moonMaskSz,
        0, 0, 0, 0,
        rhw,
        255,
        0,
        0
    );

    //
    // Draw the actual moon texture
    //
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpCoronaTexture[2])));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,      RWRSTATE(rwBLENDDESTALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,     RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,  RWRSTATE(FALSE));

    CSprite::RenderOneXLUSprite(
        { moonPosScr.x, moonPosScr.y, z },
        moonSz,
        colorRG, colorRG, (uint8)((float)colorB * 0.85f), 255,
        rhw,
        255,
        0,
        0
    );

    //
    // [Cleanup]: Restore generic render states
    //
    RwRenderStateSet(rwRENDERSTATESRCBLEND,  RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDONE));
}

// From `CClouds::Render` [0x713D2A - 0x714019]
// Draws the R* logo on the sky
void CClouds::Render_MaybeRenderRockstarLogo(float colorBalance) {
    constexpr auto LOGO_VISIBLE_FROM_HRS  = 22u, // From this hour (Eg.: 22:XX)
                   LOGO_VISIBLE_UNTIL_HRS = 6u;  // Up to this hour (So at 06:00 it's not visible anymore)

    constexpr auto R_OFFSET_FROM_CAMERA    = CVector{ 100.f,   0.f, 10.f }; // Letter `R` offset from camera
    constexpr auto STAR_OFFSET_FROM_CAMERA = CVector{ 100.f, -90.f, 10.f }; // `*` [As in R*] offset from camera

    constexpr auto  STARS_NUM_POSITIONS                    = 9;
    constexpr float STARS_Y_POSITIONS[STARS_NUM_POSITIONS] = { 0.00f, 0.05f, 0.13f, 0.40f, 0.70f, 0.60f, 0.27f, 0.55f, 0.75f }; // 0x8D55EC
    constexpr float STARS_Z_POSITIONS[STARS_NUM_POSITIONS] = { 0.00f, 0.45f, 0.90f, 1.00f, 0.85f, 0.52f, 0.48f, 0.35f, 0.20f }; // 0x8D5610
    constexpr float STARS_SIZES[STARS_NUM_POSITIONS]       = { 1.00f, 1.40f, 0.90f, 1.00f, 0.60f, 1.50f, 1.30f, 1.00f, 0.80f }; // 0x8D5634

    if (!s_DebugSettings.Rockstar.Enabled) {
        return;
    }

    if (!s_DebugSettings.Rockstar.Force) {
        if (!CClock::GetIsTimeInRange(LOGO_VISIBLE_FROM_HRS, LOGO_VISIBLE_UNTIL_HRS)) {
            return;
        }
    }

    const uint32_t t = CClock::GetGameClockHours() == LOGO_VISIBLE_FROM_HRS
        ? CClock::GetGameClockMinutes() // Fading in
        : CClock::GetGameClockHours() == LOGO_VISIBLE_UNTIL_HRS - 1
              ? 60u - CClock::GetGameClockMinutes() // Fading out
              : 60u; // Static

    if (t == 0) {
        return;
    }

    const auto colorB  = (uint8)(255u * t / 60u);
    const auto colorRG = CalculateColorWithBalance(colorB, colorBalance);

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpCoronaTexture[0])));

    const auto camPos = TheCamera.GetPosition();

    //
    // Draw `R`
    //
    for (auto i = 0; i < 11; i++) {
        CVector offset = R_OFFSET_FROM_CAMERA;
        if (i >= 9) { // Clever trick to save memory I guess, re-uses the first 2 star vertices, but with X adjusted to be on the flipside
            offset.x = -offset.x;
        }

        const auto posIdx = i % STARS_NUM_POSITIONS;
        offset.y -= STARS_Y_POSITIONS[posIdx] * 90.f;
        offset.z += STARS_Z_POSITIONS[posIdx] * 80.f;

        CVector   starPosScr;
        CVector2D starSizeScr;
        if (!CSprite::CalcScreenCoors(camPos + offset, &starPosScr, &starSizeScr.x, &starSizeScr.y, false, true)) {
            continue;
        }

        const auto cc = CalculateColorWithBalance(colorB, (float)(rand() % 32) * 0.015f);

        starSizeScr *= STARS_SIZES[posIdx] * 0.8f;

        CSprite::RenderBufferedOneXLUSprite(
            starPosScr,
            starSizeScr,
            cc, cc, cc, 255,
            1.f / starPosScr.z,
            255
        );
    }

    //
    // Draw the `*`
    //
    CVector   lastStarPosScr;
    CVector2D lastStarSizeScr;
    if (CSprite::CalcScreenCoors(camPos + STAR_OFFSET_FROM_CAMERA, &lastStarPosScr, &lastStarSizeScr.x, &lastStarSizeScr.y, false, true)) {
        const auto cc = CalculateColorWithBalance(colorB, (float)(rand() % 128) * 0.0015625f + 0.5f);

        lastStarSizeScr.x *= 5.f;
        lastStarSizeScr.y *= 5.f;

        CSprite::RenderOneXLUSprite(
            lastStarPosScr,
            lastStarSizeScr,
            cc, cc, cc, 255,
            1.f / lastStarPosScr.z,
            255, 0, 0
        );
    }

    //
    // Finally, draw it
    //
    CSprite::FlushSpriteBuffer();
}

// From `CClouds::Render` [0x714019 - 0x71422A]
void CClouds::Render_RenderLowClouds(float colorBalance) {
    constexpr CVector LOW_CLOUDS_COORDS[]{
        {1.0f,   0.0f, 0.0f},
        {0.7f,  -0.7f, 1.0f},
        {0.0f,  -1.0f, 0.5f},
        {-0.7f, -0.7f, 0.0f},
        {-1.0f,  0.0f, 1.0f},
        {-0.7f,  0.7f, 0.3f},
        {0.0f,   1.0f, 0.9f},
        {0.7f,   0.7f, 0.4f},
        {0.8f,   0.4f, 1.3f},
        {-0.8f,  0.4f, 1.4f},
        {0.4f,  -0.8f, 1.2f},
        {-0.4f, -0.8f, 1.7f},
    };

    const auto colorR = CalculateColorWithBalance((uint8)CTimeCycle::m_CurrentColours.m_nLowCloudsRed, colorBalance);
    const auto colorG = CalculateColorWithBalance((uint8)CTimeCycle::m_CurrentColours.m_nLowCloudsGreen, colorBalance);
    const auto colorB = CalculateColorWithBalance((uint8)CTimeCycle::m_CurrentColours.m_nLowCloudsBlue, colorBalance);

    if (!s_DebugSettings.LowClouds.Enabled) {
        return;
    }

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpCloudTex)));

    // Calculate camera roll
    ms_cameraRoll = TheCamera.GetRoll();

    // Render clouds
    auto camPos = TheCamera.GetPosition();
    camPos.z = 0; // GTA immediately overwrites input.z of CalcScreenCoors with offset.z * 60.f + 40.f (notice that there is no "+ camPos.z", maybe R* simply confused = with +=)
    for (const auto& offset : LOW_CLOUDS_COORDS) {
        CVector   cloudPosScr;
        CVector2D cloudSizeScr;
        if (!CSprite::CalcScreenCoors(camPos + offset * CVector{800.f, 800.f, 60.f} + CVector{0.f, 0.f, 40.f}, &cloudPosScr, &cloudSizeScr.x, &cloudSizeScr.y, false, true)) {
            continue;
        }
        CSprite::RenderBufferedOneXLUSprite_Rotate_Dimension(
            cloudPosScr,
            cloudSizeScr * CVector2D{ 320.f, 40.f },
            colorR, colorG, colorB, 255,
            1.f / cloudPosScr.z,
            ms_cameraRoll,
            255
        );
    }

    CSprite::FlushSpriteBuffer();
}

// From `CClouds::Render` [0x71422A - 0x714387]
void CClouds::Render_MaybeRenderRainbows() {
    constexpr size_t NUM_RAINBOW_LINES = 6;
    const CRGBA RAINBOW_LINES_COLOR[]{
        {30, 0,  0,  255},
        {30, 15, 0,  255},
        {30, 30, 0,  255},
        {10, 30, 10, 255},
        {0,  0,  30, 255},
        {15, 0,  30, 255}
    };

    if (!s_DebugSettings.Rainbow.Enabled) {
        return;
    }

    if (!s_DebugSettings.Rainbow.Force) {
        if (CWeather::Rainbow == 0.f) {
            return;
        }
    }

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpCoronaTexture[0])));

    const auto camPos = TheCamera.GetPosition();

    for (auto i = 0; i < NUM_RAINBOW_LINES; i++) {
        const auto offset = CVector{
            (float)i * 1.5f,
            100.f,
            5.f
        };

        CVector   rblinePosScr;
        CVector2D rblineSizeScr;
        if (!CSprite::CalcScreenCoors(camPos + offset, &rblinePosScr, &rblineSizeScr.x, &rblineSizeScr.y, false, true)) {
            continue;
        }
        rblineSizeScr *= CVector2D{ 2.f, 50.f };

        const auto& clr = RAINBOW_LINES_COLOR[i];
        CSprite::RenderBufferedOneXLUSprite(
            rblinePosScr,
            rblineSizeScr,
            (uint8)((float)clr.r * CWeather::Rainbow),
            (uint8)((float)clr.g * CWeather::Rainbow),
            (uint8)((float)clr.b * CWeather::Rainbow),
            clr.a,
            1.f / rblinePosScr.z,
            255
        );
    }

    CSprite::FlushSpriteBuffer();
}

// From `CClouds::Render` [0x714387 - 0x714640]
void CClouds::Render_MaybeRenderStreaks() {
    constexpr auto REPEAT_INTERVAL_MS = 8192u; // Use power-of-2 numbers here if possible
    constexpr auto VISIBILE_TIME_MS   = 800u;
    static_assert(REPEAT_INTERVAL_MS >= VISIBILE_TIME_MS);

    RwRenderStateSet(rwRENDERSTATESRCBLEND,  RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));

    if (!s_DebugSettings.Streaks.Enabled) {
        return;
    }

    if (!s_DebugSettings.Streaks.Force) {
        if (CClock::GetGameClockHours() >= 5) { // Time is between 0 and 5 AM
            return;
        }
        if (!IsExtraSunny(CWeather::OldWeatherType) || !IsExtraSunny(CWeather::NewWeatherType)) { // Both weathers must be extra sunny
            return;
        }
    }

    const auto timeMS = CTimer::GetTimeInMS();

    // This must always be checked, otherwise code breaks
    const auto repeatDelta = timeMS % REPEAT_INTERVAL_MS;
    if (repeatDelta >= VISIBILE_TIME_MS) {
        return;
    }

    const auto repeat64 = (timeMS / REPEAT_INTERVAL_MS) % 64;

    //> 0x714464
    const auto size = CVector{
        (float)((int32)(repeat64 % 7) - 3) * 0.1f,
        (float)((int32)((timeMS & 0xFFFF) / REPEAT_INTERVAL_MS) - 4) * 0.1f,
        1.f
    }.Normalized();

    //> 0x7144C7
    const auto offsetDir = CVector{
        (float)((int32)(repeat64 % 9) - 5),
        (float)((int32)(repeat64 % 10) - 5),
        0.1f
    }.Normalized();

    RenderBuffer::ClearRenderBuffer();

    const auto PushVertex = [
        basePos = offsetDir * 1000.f + TheCamera.GetPosition(),
        size
    ](float scale, CRGBA color) {
        RenderBuffer::PushVertex(
            basePos + size * scale,
            color
        );
    };

    const auto scale = (float)((VISIBILE_TIME_MS / 2 - (int32)repeatDelta) * 2);
    PushVertex(scale,        { 255, 255, 255, 225 });
    PushVertex(scale + 50.f, { 255, 255, 255, 0 });

    RenderBuffer::PushIndices({ 1, 0 }, false);

    RenderBuffer::Render(rwPRIMTYPEPOLYLINE, nullptr, rwIM3D_VERTEXRGBA | rwIM3D_VERTEXXYZ);
}

// 0x713950
void CClouds::Render() {
    if (!CGame::CanSeeOutSideFromCurrArea()) {
        return;
    }

    CCoronas::SunBlockedByClouds = false;

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDONE));

    CSprite::InitSpriteBuffer();

    const auto colorBalance = std::max(CWeather::Foggyness, CWeather::CloudCoverage);

    // I've broken up this function into several others to make the code easier to understand

    Render_MaybeRenderMoon(colorBalance); // [0x7139B2 - 0x713D2A]
    Render_MaybeRenderRockstarLogo(colorBalance); // 0x713D2A - 0x714019
    Render_RenderLowClouds(std::max(colorBalance, CWeather::ExtraSunnyness));
    Render_MaybeRenderRainbows();
    Render_MaybeRenderStreaks();
}

// 0x714650
void CClouds::RenderSkyPolys() {
    CVector norm{}, pos{};

    if (TheCamera.m_matrix) {
        pos = TheCamera.m_matrix->GetPosition();
        norm = TheCamera.m_matrix->GetForward();
    } else {
        pos = TheCamera.m_placement.m_vPosn;
        float fHeading = TheCamera.m_placement.m_fHeading;
        norm.x = -sin(fHeading);
        norm.y = cos(fHeading);
    }
    norm.z = 0.f;
    norm.Normalise();

    CVector invNorm = -norm;
    float fBlendFactor = (pos.z - 25.0f) * (1.0f / 80.0f);
    fBlendFactor = std::clamp(fBlendFactor, 0.f, 1.f);
    fBlendFactor = std::max(fBlendFactor, CWeather::Foggyness);

    RwRGBA belowHorizonGrey = CTimeCycle::m_BelowHorizonGrey;
    belowHorizonGrey.red   += static_cast<RwUInt8>(fBlendFactor * static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyBottomRed - belowHorizonGrey.red));
    belowHorizonGrey.green += static_cast<RwUInt8>(fBlendFactor * static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyBottomGreen - belowHorizonGrey.green));
    belowHorizonGrey.blue  += static_cast<RwUInt8>(fBlendFactor * static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyBottomBlue - belowHorizonGrey.blue));

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATECULLMODE,          RWRSTATE(rwCULLMODECULLNONE));

    uiTempBufferIndicesStored = 0;
    uiTempBufferVerticesStored = 0;

    CVector2D scale{norm.y * SKYP_WIDTH_MULT, invNorm.x * SKYP_WIDTH_MULT}, aPos2D[4] = {};
    CVector aVertices[4] = {};
    float aPosZ[6] = {};

    aPos2D[0].x = pos.x + (invNorm.x - scale.x) * SKYP_CAM_DIST;
    aPos2D[0].y = pos.y + (invNorm.y - scale.y) * SKYP_CAM_DIST;
    aPos2D[1].x = pos.x + (invNorm.x + scale.x) * SKYP_CAM_DIST;
    aPos2D[1].y = pos.y + (invNorm.y + scale.y) * SKYP_CAM_DIST;
    aPos2D[2].x = pos.x + (norm.x - scale.x) * SKYP_CAM_DIST;
    aPos2D[2].y = pos.y + (norm.y - scale.y) * SKYP_CAM_DIST;
    aPos2D[3].x = pos.x + (norm.x + scale.x) * SKYP_CAM_DIST;
    aPos2D[3].y = pos.y + (norm.y + scale.y) * SKYP_CAM_DIST;

    aPosZ[0] = pos.z + (invNorm.z + SKYP_ABOVE_HORIZON_Z) * SKYP_CAM_DIST;
    aPosZ[1] = pos.z + (norm.z + SKYP_ABOVE_HORIZON_Z) * SKYP_CAM_DIST;
    aPosZ[2] = pos.z + (norm.z + SKYP_HORIZON_Z) * SKYP_CAM_DIST;
    aPosZ[3] = pos.z + (norm.z + SKYP_SEA_HORIZON_Z) * SKYP_CAM_DIST;
    aPosZ[4] = pos.z + (norm.z + SKYP_BELOW_HORIZON_Z) * SKYP_CAM_DIST;
    aPosZ[5] = pos.z + (invNorm.z + SKYP_BELOW_HORIZON_Z) * SKYP_CAM_DIST;

    aVertices[0].x = aPos2D[2].x;
    aVertices[0].y = aPos2D[2].y;
    aVertices[0].z = aPosZ[1];

    aVertices[1].x = aPos2D[3].x;
    aVertices[1].y = aPos2D[3].y;
    aVertices[1].z = aPosZ[1];

    aVertices[2].x = aPos2D[2].x;
    aVertices[2].y = aPos2D[2].y;
    aVertices[2].z = aPosZ[2];

    aVertices[3].x = aPos2D[3].x;
    aVertices[3].y = aPos2D[3].y;
    aVertices[3].z = aPosZ[2];

    SetUpOneSkyPoly(
        aVertices[0], aVertices[1], aVertices[2], aVertices[3],
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyTopRed),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyTopGreen),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyTopBlue),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomRed),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomGreen),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomBlue)
    );

    aVertices[0].z = aPosZ[2];
    aVertices[1].z = aPosZ[2];
    aVertices[2].z = aPosZ[3];
    aVertices[3].z = aPosZ[3];

    SetUpOneSkyPoly(
        aVertices[0], aVertices[1], aVertices[2], aVertices[3],
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomRed),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomGreen),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomBlue),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomRed),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomGreen),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomBlue)
    );

    aVertices[0].z = aPosZ[3];
    aVertices[1].z = aPosZ[3];
    aVertices[2].z = aPosZ[4];
    aVertices[3].z = aPosZ[4];

    SetUpOneSkyPoly(
        aVertices[0], aVertices[1], aVertices[2], aVertices[3],
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomRed),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomGreen),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyBottomBlue),
        belowHorizonGrey.red,
        belowHorizonGrey.green,
        belowHorizonGrey.blue
    );

    aVertices[0].z = aPosZ[1];
    aVertices[1].z = aPosZ[1];
    aVertices[2].x = aPos2D[0].x;
    aVertices[2].y = aPos2D[0].y;
    aVertices[2].z = aPosZ[0];
    aVertices[3].x = aPos2D[1].x;
    aVertices[3].y = aPos2D[1].y;
    aVertices[3].z = aPosZ[0];

    SetUpOneSkyPoly(
        aVertices[0], aVertices[1], aVertices[2], aVertices[3],
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyTopRed),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyTopGreen),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyTopBlue),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyTopRed),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyTopGreen),
        static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nSkyTopBlue)
    );

    aVertices[0].z = aPosZ[4];
    aVertices[1].x = aPos2D[3].x;
    aVertices[1].y = aPos2D[3].y;
    aVertices[1].z = aPosZ[4];
    aVertices[2].z = aPosZ[5];
    aVertices[3].z = aPosZ[5];

    SetUpOneSkyPoly(
        aVertices[0], aVertices[1], aVertices[2], aVertices[3],
        belowHorizonGrey.red,
        belowHorizonGrey.green,
        belowHorizonGrey.blue,
        belowHorizonGrey.red,
        belowHorizonGrey.green,
        belowHorizonGrey.blue
    );

    CBrightLights::RenderOutGeometryBuffer();
}

// 0x7154B0
void CClouds::RenderBottomFromHeight() {
    /****
    * Code below should be good
    * but it isn't complete...
    *****\
   
    const auto camPos = TheCamera.GetPosition();
    if (camPos.z < -90.f) { // 0x71557D [Moved up here]
        return;
    }

    const auto& cc = CTimeCycle::m_CurrentColours; // cc = color component
    const auto ClampClr = [](float clr) {
        return std::min(clr, 255.f);
    };
    const auto fcClr = CRGBA{ // fc = fluffy clouds
        (uint8)ClampClr(cc.m_nFluffyCloudsBottomRed * 2.f + 20.f),
        (uint8)ClampClr(cc.m_nFluffyCloudsBottomGreen * 1.5f),
        (uint8)ClampClr(cc.m_nFluffyCloudsBottomBlue * 1.5f),
        (uint8)255
    };

    auto lowZ = 160.f, highZ = 190.f;

    auto& windShift = StaticRef<float, 0xC6E954>();

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(RwTextureGetRaster(gpCloudMaskTex)));

    // TODO....
    */
}

//
// -- VOLUMETRIC CLOUDS --
//

// 0x7131C0
void CClouds::VolumetricCloudsInit() {
    ms_vc.quadNormal[0] = CVector(0.0f, 1.0f, 0.0f);
    ms_vc.quadNormal[1] = CVector(0.0f, 0.0f, 1.0f);
    ms_vc.quadNormal[2] = CVector(1.0f, 0.0f, 0.0f);

    ms_vc.modelX[0] = -0.5f;
    ms_vc.modelY[0] = 0.0f;
    ms_vc.modelZ[0] = 0.5f;
    ms_vc.modelU[0] = 0.0f;
    ms_vc.modelV[0] = 0.0f;

    ms_vc.modelX[1] = 0.5f;
    ms_vc.modelY[1] = 0.0f;
    ms_vc.modelZ[1] = 0.5f;
    ms_vc.modelU[1] = 1.0f;
    ms_vc.modelV[1] = 0.0f;

    ms_vc.modelX[2] = -0.5f;
    ms_vc.modelY[2] = 0.0f;
    ms_vc.modelZ[2] = -0.5f;
    ms_vc.modelU[2] = 0.0f;
    ms_vc.modelV[2] = 1.0f;

    ms_vc.modelX[3] = 0.5f;
    ms_vc.modelY[3] = 0.0f;
    ms_vc.modelZ[3] = 0.5f;
    ms_vc.modelU[3] = 1.0f;
    ms_vc.modelV[3] = 0.0f;

    ms_vc.modelX[4] = 0.5f;
    ms_vc.modelY[4] = 0.0f;
    ms_vc.modelZ[4] = -0.5f;
    ms_vc.modelU[4] = 1.0f;
    ms_vc.modelV[4] = 1.0f;

    ms_vc.modelX[5] = -0.5f;
    ms_vc.modelY[5] = 0.0f;
    ms_vc.modelZ[5] = -0.5f;
    ms_vc.modelU[5] = 0.0f;
    ms_vc.modelV[5] = 1.0f;

    ms_vc.modelX[6] = -0.5f;
    ms_vc.modelY[6] = 0.5f;
    ms_vc.modelZ[6] = 0.0f;
    ms_vc.modelU[6] = 0.0f;
    ms_vc.modelV[6] = 0.0f;

    ms_vc.modelX[7] = 0.5f;
    ms_vc.modelY[7] = 0.5f;
    ms_vc.modelZ[7] = 0.0f;
    ms_vc.modelU[7] = 1.0f;
    ms_vc.modelV[7] = 0.0f;

    ms_vc.modelX[8] = -0.5f;
    ms_vc.modelY[8] = -0.5f;
    ms_vc.modelZ[8] = 0.0f;
    ms_vc.modelU[8] = 0.0f;
    ms_vc.modelV[8] = 1.0f;

    ms_vc.modelX[9] = 0.5f;
    ms_vc.modelY[9] = 0.5f;
    ms_vc.modelZ[9] = 0.0f;
    ms_vc.modelU[9] = 1.0f;
    ms_vc.modelV[9] = 0.0f;

    ms_vc.modelX[10] = 0.5f;
    ms_vc.modelY[10] = -0.5f;
    ms_vc.modelZ[10] = 0.0f;
    ms_vc.modelU[10] = 1.0f;
    ms_vc.modelV[10] = 1.0f;

    ms_vc.modelX[11] = -0.5f;
    ms_vc.modelY[11] = -0.5f;
    ms_vc.modelZ[11] = 0.0f;
    ms_vc.modelU[11] = 0.0f;
    ms_vc.modelV[11] = 1.0f;

    ms_vc.modelX[12] = 0.0f;
    ms_vc.modelY[12] = -0.5f;
    ms_vc.modelZ[12] = 0.5f;
    ms_vc.modelU[12] = 0.0f;
    ms_vc.modelV[12] = 0.0f;

    ms_vc.modelX[13] = 0.0f;
    ms_vc.modelY[13] = 0.5f;
    ms_vc.modelZ[13] = 0.5f;
    ms_vc.modelU[13] = 1.0f;
    ms_vc.modelV[13] = 0.0f;

    ms_vc.modelX[14] = 0.0f;
    ms_vc.modelY[14] = -0.5f;
    ms_vc.modelZ[14] = -0.5f;
    ms_vc.modelU[14] = 0.0f;
    ms_vc.modelV[14] = 1.0f;

    ms_vc.modelX[15] = 0.0f;
    ms_vc.modelY[15] = 0.5f;
    ms_vc.modelZ[15] = 0.5f;
    ms_vc.modelU[15] = 1.0f;
    ms_vc.modelV[15] = 0.0f;

    ms_vc.modelX[16] = 0.0f;
    ms_vc.modelY[16] = 0.5f;
    ms_vc.modelZ[16] = -0.5f;
    ms_vc.modelU[16] = 1.0f;
    ms_vc.modelV[16] = 1.0f;

    ms_vc.modelX[17] = 0.0f;
    ms_vc.modelY[17] = -0.5f;
    ms_vc.modelZ[17] = -0.5f;
    ms_vc.modelU[17] = 0.0f;
    ms_vc.modelV[17] = 1.0f;

    for (auto i = 0u; i < MAX_VOLUMETRIC_CLOUDS; ++i) {
        ms_vc.bUsed[i] = false;
        ms_vc.bJustCreated[i] = 0;
    }
}

// 0x715F40
void CClouds::VolumetricClouds_Create(CVector* posn) {
    using CGeneral::GetRandomNumberInRange;

    const auto rand1To5 = GetRandomNumberInRange(1.f, 5.f);

    const auto AddVolumetricCloud = [
        randMinSizeXYZ = rand1To5 * 20.f,
        randMaxSizeXY  = rand1To5 * 100.f,
        randMaxSizeZ   = rand1To5 * 40.f
    ](int32 vcidx, CVector pos) {
        ms_vc.bUsed[vcidx]        = true;
        ms_vc.bJustCreated[vcidx] = true;
        ms_vc.alpha[vcidx]        = GetRandomNumberInRange<int32>(36, 128);

        ms_vc.size[vcidx] = CVector{
            GetRandomNumberInRange(randMinSizeXYZ, randMaxSizeXY),
            GetRandomNumberInRange(randMinSizeXYZ, randMaxSizeXY),
            GetRandomNumberInRange(randMinSizeXYZ, randMaxSizeZ),
        };

        ms_vc.pos[vcidx] = pos;
    };

    if (posn) {
        const auto randMinMaxOffsetXYZ = rand1To5 * 3.f;
        for (auto i = 0; i < 5; i++) {
            const auto vcidx = VolumetricClouds_GetFirstFreeSlot();
            if (vcidx == -1) {
                return;
            }
            const auto pos = *posn = *posn + CVector::Random(-randMinMaxOffsetXYZ, randMinMaxOffsetXYZ);
            AddVolumetricCloud(
                vcidx,
                pos
            );
        }
    } else {
        const auto camPos  = TheCamera.GetPosition();
        const auto maxDist = m_fVolumetricCloudMaxDistance;
        for (auto i = 0; i < MAX_VOLUMETRIC_CLOUDS; i++) {
            AddVolumetricCloud(
                i,
                camPos + CVector{
                    GetRandomNumberInRange(-maxDist, maxDist),
                    GetRandomNumberInRange(-maxDist, maxDist),
                    GetRandomNumberInRange(-maxDist * 0.25f, maxDist * 0.25f),
                }
            );
        }
    }
}

// 0x7135F0
void CClouds::VolumetricClouds_Delete(int32 vcSlotIndex) {
    vcSlotIndex = std::clamp(vcSlotIndex, 0, MAX_VOLUMETRIC_CLOUDS - 1);
    ms_vc.bUsed[vcSlotIndex]        = false;
    ms_vc.bJustCreated[vcSlotIndex] = false;
}

// unused
// inlined into VolumetricClouds_Create
// 0x7135C0
int32 CClouds::VolumetricClouds_GetFirstFreeSlot() {
    for (auto i = 0u; i < m_VolumetricCloudsUsedNum; i++) {
        if (!ms_vc.bUsed[i]) {
            return (int32)i;
        }
    }
    return -1;
}

// 0x713630
float CClouds::VolumetricCloudsGetMaxDistance() {
    return std::min(RwCameraGetFarClipPlane(Scene.m_pRwCamera), 600.f);
}

// 0x716380
void CClouds::VolumetricCloudsRender() {
    ZoneScoped;

    if (!s_DebugSettings.VolumetricClouds.Enabled) {
        return;
    }

    const auto plyr = FindPlayerPed();
    if (!CGame::CanSeeOutSideFromCurrArea() || !plyr->IsInCurrentArea()) {
        if (!s_DebugSettings.VolumetricClouds.Force) {
            return;
        }
    }

    m_fVolumetricCloudDensity = [] {
        switch (g_fx.GetFxQuality()) {
        case FX_QUALITY_LOW:    return 0.5f;
        case FX_QUALITY_MEDIUM: return 1.f / 3.f;
        default:                return 1.f;
        }
    }();

    m_VolumetricCloudsUsedNum = lerp(0, MAX_VOLUMETRIC_CLOUDS, m_fVolumetricCloudDensity);

    if (!m_VolumetricCloudsUsedNum) {
        return;
    }

    m_fVolumetricCloudMaxDistance = CClouds::VolumetricCloudsGetMaxDistance();

    const auto fadeOutBeginDist = m_fVolumetricCloudMaxDistance - 100.f;
    const auto fadeOutDist      = m_fVolumetricCloudMaxDistance + 200.f;

    const auto camPos = TheCamera.GetPosition();

    auto& gfVolumetricCloudFader = StaticRef<float, 0xC6E970>();
    if (m_bVolumetricCloudHeightSwitch) {
        const auto delta = CTimer::GetTimeStep() * 4.f;
        if (camPos.z < 220.f) {
            gfVolumetricCloudFader += delta;
            if (gfVolumetricCloudFader >= 255.f) {
                gfVolumetricCloudFader = 255.f;
                return;
            }
        } else {
            gfVolumetricCloudFader = std::max(0.f, gfVolumetricCloudFader - delta);
        }
    } else {
        gfVolumetricCloudFader = 0.f;
    }

    CPostEffects::ImmediateModeRenderStatesStore();
    CPostEffects::ImmediateModeRenderStatesSet();

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,   RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(ms_vc.texture)));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));

    FindPlayerPed()->GetMoveSpeed() = CVector{0.f, 0.f, 0.f};

    const auto plyrPos = FindPlayerCoors();
    const auto plyrVeh = FindPlayerVehicle();


    //> 0x71653F
    auto& gVecCameraCoors = StaticRef<CVector, 0xC6E964>();
    auto& gVecPlayerCoors = StaticRef<CVector, 0xC6E958>();

    const auto bIsCameraOrPlayerPosNotStatic = (camPos != gVecCameraCoors) || (plyrPos != gVecPlayerCoors);

    gVecCameraCoors = camPos;
    gVecPlayerCoors = plyrPos;

    if (bIsCameraOrPlayerPosNotStatic) { // If player/it's veh has moved, recreate the clouds
        const auto t = plyrVeh ? (CPlaceable*)plyrVeh : (CPlaceable*)&TheCamera;
        auto pos = (
              t->GetPosition()
            + t->GetForward() * fadeOutDist
            + CVector::Random(
                CVector{-200.f, -200.f, -50.f},
                CVector{ 200.f,  200.f, 50.f}
            )
        );
        VolumetricClouds_Create(&pos);
    }

    //> 0x7166DD -  Calculate color
    const auto& cc = CTimeCycle::m_CurrentColours;
    const auto vcClr = (uint8)std::min((
          cc.m_nSkyTopRed
        + cc.m_nSkyTopGreen
        + cc.m_nSkyTopBlue
        + cc.m_nSkyBottomRed
        + cc.m_nSkyBottomGreen
        + cc.m_nSkyBottomBlue
    ) / 6 + 64, 255);

    if (!m_VolumetricCloudsUsedNum) {
        CPostEffects::ImmediateModeRenderStatesReStore();
        return;
    }

    //
    // Actually render the vc's
    //


    // NOTE: They didn't use RenderBuffer functions, but to make our life easier [and the code nicer] we do
    RenderBuffer::ClearRenderBuffer();

    const auto RenderOutBuffer = [] {
        RenderBuffer::Render(rwPRIMTYPETRILIST, nullptr, rwIM3D_VERTEXXYZ | rwIM3D_VERTEXUV, false);
    };

    for (auto vcidx = 0u; vcidx < m_VolumetricCloudsUsedNum; vcidx++) {
        if (!ms_vc.bUsed[vcidx]) {
            continue;
        }

        auto& vcPos = ms_vc.pos[vcidx];
        auto& vcSz  = ms_vc.size[vcidx];

        const auto vcDistToCam = (camPos - vcPos).Magnitude();

        //> 0x71674E - Adjust vc position by wind [TODO: Should use TimeStep/Framedelta]
        vcPos += CVector{ m_fVolumetricCloudWindMoveFactor * CVector2D{ CWeather::WindDir } };

        //> 0x716772 - VC too far, delete it
        if (!ms_vc.bJustCreated[vcidx] && vcDistToCam > m_fVolumetricCloudMaxDistance) {
            VolumetricClouds_Delete(vcidx);
            continue;
        }

        if (vcDistToCam <= m_fVolumetricCloudMaxDistance || fadeOutDist <= vcDistToCam) {
            ms_vc.bJustCreated[vcidx] = false;
        }

        //> 0x7167DC - Alpha calculation [I don't understand it either]
        auto vcAlpha = std::max(0, ms_vc.alpha[vcidx] - (int32)gfVolumetricCloudFader);
        if (vcDistToCam > fadeOutBeginDist) {
            if (!vcAlpha) {
                continue;
            }
            if (vcDistToCam > m_fVolumetricCloudMaxDistance) {
                continue;
            }
            const auto distAlpha = std::max<int32>(0, (int32)(((m_fVolumetricCloudMaxDistance - fadeOutBeginDist) - (vcDistToCam - fadeOutBeginDist)) * (float)vcAlpha / (m_fVolumetricCloudMaxDistance - fadeOutBeginDist)));
            vcAlpha = std::min<int32>(vcAlpha, distAlpha);
            if (!vcAlpha) {
                continue;
            }
        }

        // Direction of vc to camera
        const auto vcToCamDir = (vcPos - camPos).Normalized();

        //> 0x7168F0 - Calculate quad colors
        CRGBA quadColors[3];
        for (auto i = 0; i < 3; i++) {
            quadColors[i] = {
                vcClr,
                vcClr,
                vcClr,
                (uint8)(std::abs(vcToCamDir.Dot(ms_vc.quadNormal[i])) * (float)vcAlpha)
            };
        }

        //> 0x7169F0 - Each quad has 3 vertices [As each quad consists of 2 triangles, each with 3 vertices]
        for (auto k = 0; k < 3 * 6; k++) {
            const auto quadIdx = k / 6;

            // If the buffer is full - Render it to make space for the upcoming vertices
            // This does differ a little from what they did originally, but it's fine, as the
            // end result is the same
            if (!RenderBuffer::CanFitVertices(3)) { // Must be rendered in groups of 3, otherwise triangles would be rendered with the wrong vertices
                RenderOutBuffer();
            }

            RenderBuffer::PushVertex(
                vcPos + CVector{ ms_vc.modelX[k], ms_vc.modelY[k], ms_vc.modelZ[k] } * vcSz,
                CVector2D{ms_vc.modelU[k], ms_vc.modelV[k]},
                quadColors[k / 6]
            );
        }
    }

    // Render whatever remains in the buffer
    RenderOutBuffer();

    CPostEffects::ImmediateModeRenderStatesReStore();
}
