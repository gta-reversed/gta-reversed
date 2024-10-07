/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Radar.h"
#include "EntryExitManager.h"
#include <extensions/enumerate.hpp>

constexpr std::array<airstrip_info, NUM_AIRSTRIPS> airstrip_table = { // 0x8D06E0
    airstrip_info{ { +1750.0f,  -2494.0f }, 180.0f, 1000.0f }, // AIRSTRIP_LS_AIRPORT
    airstrip_info{ { -1373.0f,  +120.00f }, 315.0f, 1500.0f }, // AIRSTRIP_SF_AIRPORT
    airstrip_info{ { +1478.0f,  +1461.0f }, 90.0f,  1200.0f }, // AIRSTRIP_LV_AIRPORT
    airstrip_info{ { +175.00f,  +2502.0f }, 180.0f, 1000.0f }  // AIRSTRIP_VERDANT_MEADOWS
};

// Array of TXD slot indices for each radar section's texture
static std::array<std::array<int32, MAX_RADAR_WIDTH_TILES>, MAX_RADAR_HEIGHT_TILES>& gRadarTextures = *(std::array<std::array<int32, MAX_RADAR_WIDTH_TILES>, MAX_RADAR_HEIGHT_TILES>*)0xBA8478;

// 0x8D0720
SpriteFileName CRadar::RadarBlipFileNames[] = {
    // name,                 maskName
    { nullptr,               nullptr },
    { nullptr,               nullptr },
    { "radar_centre",        nullptr },
    { "arrow",               nullptr },
    { "radar_north",         nullptr },
    { "radar_airYard",       nullptr },
    { "radar_ammugun",       nullptr },
    { "radar_barbers",       nullptr },
    { "radar_BIGSMOKE",      nullptr },
    { "radar_boatyard",      nullptr },
    { "radar_burgerShot",    nullptr },
    { "radar_bulldozer",     nullptr },
    { "radar_CATALINAPINK",  nullptr },
    { "radar_CESARVIAPANDO", nullptr },
    { "radar_chicken",       nullptr },
    { "radar_CJ",            nullptr },
    { "radar_CRASH1",        nullptr },
    { "radar_diner",         nullptr },
    { "radar_emmetGun",      nullptr },
    { "radar_enemyAttack",   nullptr },
    { "radar_fire",          nullptr },
    { "radar_girlfriend",    nullptr },
    { "radar_hostpitaL",     nullptr },
    { "radar_LocoSyndicate", nullptr },
    { "radar_MADDOG",        nullptr },
    { "radar_mafiaCasino",   nullptr },
    { "radar_MCSTRAP",       nullptr },
    { "radar_modGarage",     nullptr },
    { "radar_OGLOC",         nullptr },
    { "radar_pizza",         nullptr },
    { "radar_police",        nullptr },
    { "radar_propertyG",     nullptr },
    { "radar_propertyR",     nullptr },
    { "radar_race",          nullptr },
    { "radar_RYDER",         nullptr },
    { "radar_saveGame",      nullptr },
    { "radar_school",        nullptr },
    { "radar_qmark",         nullptr },
    { "radar_SWEET",         nullptr },
    { "radar_tattoo",        nullptr },
    { "radar_THETRUTH",      nullptr },
    { "radar_waypoint",      nullptr },
    { "radar_TorenoRanch",   nullptr },
    { "radar_triads",        nullptr },
    { "radar_triadsCasino",  nullptr },
    { "radar_tshirt",        nullptr },
    { "radar_WOOZIE",        nullptr },
    { "radar_ZERO",          nullptr },
    { "radar_dateDisco",     nullptr },
    { "radar_dateDrink",     nullptr },
    { "radar_dateFood",      nullptr },
    { "radar_truck",         nullptr },
    { "radar_cash",          nullptr },
    { "radar_flag",          nullptr },
    { "radar_gym",           nullptr },
    { "radar_impound",       nullptr },
    { "radar_light",         nullptr },
    { "radar_runway",        nullptr },
    { "radar_gangB",         nullptr },
    { "radar_gangP",         nullptr },
    { "radar_gangY",         nullptr },
    { "radar_gangN",         nullptr },
    { "radar_gangG",         nullptr },
    { "radar_spray",         nullptr }
};

constexpr float RADAR_MIN_RANGE = 180.0f;
constexpr float RADAR_MAX_RANGE = 350.0f;
constexpr float RADAR_MIN_SPEED = 0.3f;
constexpr float RADAR_MAX_SPEED = 0.9f;

void CRadar::InjectHooks() {
    RH_ScopedClass(CRadar);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x587FB0);
    RH_ScopedInstall(LoadTextures, 0x5827D0);
    RH_ScopedInstall(DrawLegend, 0x5828A0);
    RH_ScopedInstall(LimitRadarPoint, 0x5832F0);
    RH_ScopedInstall(Shutdown, 0x585940);
    RH_ScopedInstall(SetMapCentreToPlayerCoords, 0x585B20);
    RH_ScopedInstall(InitFrontEndMap, 0x585960);
    RH_ScopedInstall(CalculateBlipAlpha, 0x583420);
    // RH_ScopedInstall(TransformRadarPointToScreenSpace, 0x583480, true);
    RH_ScopedInstall(TransformRealWorldPointToRadarSpace, 0x583530);
    RH_ScopedInstall(CalculateCachedSinCos, 0x583670);
    RH_ScopedInstall(SetBlipSprite, 0x583D70);
    RH_ScopedInstall(SetBlipAlwaysDisplayInZoom, 0x583DB0);
    RH_ScopedInstall(DrawYouAreHereSprite, 0x584960);
    RH_ScopedInstall(ChangeBlipColour, 0x583AB0);
    RH_ScopedOverloadedInstall(ClearActualBlip, "OG", 0x587C10, void (*)(int32));
    RH_ScopedInstall(ClearBlip, 0x587CE0);
    RH_ScopedOverloadedInstall(ClearBlipForEntity, "OG", 0x587C60, void(*)(eBlipType, int32));
    RH_ScopedInstall(RequestMapSection, 0x584B50);
    RH_ScopedInstall(RemoveMapSection, 0x584BB0);
    RH_ScopedInstall(RemoveRadarSections, 0x584BF0);
    RH_ScopedInstall(DrawRadarSprite, 0x585FF0);
    RH_ScopedInstall(DrawMap, 0x586B00);
    RH_ScopedInstall(DrawRadarMap, 0x586880);
    RH_ScopedOverloadedInstall(StreamRadarSections, "", 0x5858D0, void (*)(const CVector&));
    RH_ScopedInstall(SetupRadarRect, 0x584A80);
    RH_ScopedInstall(GetActualBlipArrayIndex, 0x582870);
    RH_ScopedInstall(LimitToMap, 0x583350);
    RH_ScopedInstall(DrawRotatingRadarSprite, 0x584850);
    RH_ScopedInstall(SetBlipFriendly, 0x583EB0);
    RH_ScopedInstall(ChangeBlipDisplay, 0x583D20);
    RH_ScopedInstall(SetBlipEntryExit, 0x583F00);
    RH_ScopedInstall(SetShortRangeCoordBlip, 0x583920);
    RH_ScopedInstall(ChangeBlipScale, 0x583CC0);
    RH_ScopedInstall(GetRadarTraceColour, 0x584770);
    RH_ScopedInstall(ShowRadarMarker, 0x584480);
    RH_ScopedInstall(DrawRadarMask, 0x585700);
    RH_ScopedInstall(Load, 0x5D53C0);
    RH_ScopedGlobalInstall(Save, 0x5D5860, {.reversed = false});
    RH_ScopedInstall(SetBlipFade, 0x583E00); // unused
    RH_ScopedInstall(SetCoordBlipAppearance, 0x583E50);
    RH_ScopedInstall(SetCoordBlip, 0x583820);
    RH_ScopedInstall(SetEntityBlip, 0x5839A0);
    RH_ScopedInstall(DisplayThisBlip, 0x583B40);
    RH_ScopedInstall(ChangeBlipBrightness, 0x583C70);
    RH_ScopedInstall(ShowRadarTrace, 0x583F40);
    RH_ScopedInstall(ShowRadarTraceWithHeight, 0x584070);
    RH_ScopedInstall(DrawCoordBlip, 0x586D60);

    RH_ScopedInstall(SetupAirstripBlips, 0x587D20); // TEST
    RH_ScopedInstall(DrawBlips, 0x588050);
    // RH_ScopedInstall(ClipRadarPoly, 0x585040);
    // RH_ScopedInstall(DrawAreaOnRadar, 0x5853D0);
    // RH_ScopedInstall(StreamRadarSections, 0x584C50);
    RH_ScopedInstall(AddBlipToLegendList, 0x5859F0);
    RH_ScopedInstall(Draw3dMarkers, 0x585BF0);
    RH_ScopedInstall(DrawRadarSection, 0x586110, {.reversed = false}); // Run-Time Check Failure #2 - Stack around the variable 'texCoords' was corrupted. Possibly wrong calling conv
    RH_ScopedInstall(DrawRadarSectionMap, 0x586520);
    RH_ScopedInstall(DrawRadarGangOverlay, 0x586650);
    // RH_ScopedInstall(DrawEntityBlip, 0x587000);
    // RH_ScopedGlobalInstall(LineRadarBoxCollision, 0x584E00);

    // unused
    RH_ScopedInstall(GetNewUniqueBlipIndex, 0x582820);
    RH_ScopedInstall(TransformRadarPointToRealWorldSpace, 0x5835A0);
    RH_ScopedGlobalInstall(IsPointInsideRadar, 0x584D40);
    RH_ScopedGlobalInstall(GetTextureCorners, 0x584D90);
    RH_ScopedGlobalInstall(ClipRadarTileCoords, 0x584B00);    
}

// 0x587FB0
void CRadar::Initialise() {
    airstrip_blip = 0;
    airstrip_location = AIRSTRIP_LS_AIRPORT;

    for (auto& trace : ms_RadarTrace) {
        ClearActualBlip(trace);
    }

    m_radarRange = 350.0f;

    for (auto y = 0u; y < MAX_RADAR_HEIGHT_TILES; y++) {
        for (auto x = 0u; x < MAX_RADAR_WIDTH_TILES; x++) {
            gRadarTextures[y][x] = CTxdStore::FindTxdSlot(std::format("radar{:02d}", y * MAX_RADAR_WIDTH_TILES + x).c_str());
        }
    }
}

// 0x585940
void CRadar::Shutdown() {
    rng::for_each(RadarBlipSprites, &CSprite2d::Delete);
    RemoveRadarSections();
}

// 0x5827D0
void CRadar::LoadTextures() {
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("hud"));

    for (auto i = 0; i < MAX_RADAR_SPRITES; i++) {
        const auto [texture, mask] = RadarBlipFileNames[i];
        RadarBlipSprites[i].SetTexture(texture); // mask unused
    }

    CTxdStore::PopCurrentTxd();
}

/*!
 * @brief Creates a unique blip handle by blip index from `ms_RadarTrace`. (Inlined)
 * @addr 0x582820
 * @returns Unique blip handle
 */
tBlipHandle CRadar::GetNewUniqueBlipIndex(int32 index) {
    auto& trace = ms_RadarTrace[index];
    if (trace.m_nCounter >= std::numeric_limits<uint16>::max() - 1)
        trace.m_nCounter = 1; // Wrap back to 1
    else
        trace.m_nCounter++; // Increment
    return index | (trace.m_nCounter << 16);
}

/*!
 * @param blip Blip handle
 * @addr 0x582870
 * @returns Index of the handle in `ms_RadarTrace`
 */
int32 CRadar::GetActualBlipArrayIndex(tBlipHandle blip) {
    if (blip == -1)
        return -1;

    const auto  traceIndex = static_cast<uint16>(blip);
    const auto& trace      = ms_RadarTrace[traceIndex];
    const auto  counter    = static_cast<uint16>(blip >> 16);
    if (counter != trace.m_nCounter || !trace.m_bTrackingBlip)
        return -1;

    return traceIndex;
}

// 0x5828A0
void CRadar::DrawLegend(int32 x, int32 y, eRadarSprite blipType) {
    CFont::PrintString(
        (float)x + SCREEN_STRETCH_X(20.0f),
        (float)y + SCREEN_STRETCH_Y(3.0f),
        GetBlipName(blipType)
    );

    if (blipType > -1) { // The blip is a sprite, so just draw it.
        RadarBlipSprites[blipType].Draw(
            {
                (float)x,                           (float)y,
                (float)x + SCREEN_STRETCH_X(16.0f), (float)y + SCREEN_STRETCH_X(16.0f)
            },
            // NOTE: `y + SCREEN_STRETCH_X(16.0f)` is correct. It is here to make sprites
            // square instead of dependent on the aspect ratio.
            { 255, 255, 255, 255 }
        );
        return;
    }

    static auto& legendTraceHeight = StaticRef<eRadarTraceHeight, 0xBAA350>(); // = eRadarTraceHeight::RADAR_TRACE_LOW;
    static auto& legendTraceTimer  = StaticRef<uint32, 0xBAA354>(); // = CTimer::GetTimeInMS();

    if (CTimer::GetTimeInMSPauseMode() - legendTraceTimer > 600) {
        legendTraceTimer = CTimer::GetTimeInMSPauseMode();

        legendTraceHeight = [] {
            switch (legendTraceHeight) {
            case RADAR_TRACE_LOW:    return RADAR_TRACE_HIGH;
            case RADAR_TRACE_HIGH:   return RADAR_TRACE_NORMAL;
            case RADAR_TRACE_NORMAL: return RADAR_TRACE_LOW;
            default:
                NOTSA_UNREACHABLE();
            }
        }();
    }

    const auto posX = std::round(SCREEN_STRETCH_X(8.0f) + (float)x);
    const auto posY = std::round(SCREEN_STRETCH_Y(8.0f) + (float)y);

    const auto x4px = SCREEN_STRETCH_X(4.0f);
    const auto x5px = SCREEN_STRETCH_X(5.0f), y5px = SCREEN_STRETCH_Y(5.0f);
    const auto x7px = SCREEN_STRETCH_X(7.0f), y7px = SCREEN_STRETCH_Y(7.0f);

    const auto& arrowColor   = ArrowBlipColour[-blipType];
    const auto  outlineColor = CRGBA{0, 0, 0, 255};

    switch (legendTraceHeight) {
    case RADAR_TRACE_LOW: {
        CSprite2d::Draw2DPolygon(
            posX - x7px, posY + y7px,
            posX + x7px, posY + y7px,
            posX,        posY - x7px,
            posX,        posY - x7px,
            outlineColor
        );

        CSprite2d::Draw2DPolygon(
            posX + x5px, posY + y5px,
            posX - x5px, posY + y5px,
            posX,        posY - x5px,
            posX,        posY - x5px,
            arrowColor
        );
        break;
    }
    case RADAR_TRACE_HIGH: {
        CSprite2d::Draw2DPolygon(
            posX + x7px, posY - y7px,
            posX - x7px, posY - y7px,
            posX,        posY + y7px,
            posX,        posY + y7px,
            outlineColor
        );

        CSprite2d::Draw2DPolygon(
            posX + x5px, posY - y5px,
            posX - x5px, posY - y5px,
            posX,        posY + y5px,
            posX,        posY + y5px,
            arrowColor
        );
        break;
    }
    case RADAR_TRACE_NORMAL: {
        CSprite2d::DrawRect(
            {
                posX - x5px,
                posY - x5px,
                posX + x5px,
                posY + x5px
            },
            outlineColor
        );

        CSprite2d::DrawRect(
            {
                posX - x4px,
                posY - x4px,
                posX + x4px,
                posY + x4px
            },
            arrowColor
        );
        break;
    }
    }
}

/*!
 * @brief Limits a 2D vector to the radar. (which is a unit circle)
 * @brief This function does not effect the vector if the map is being drawn.
 * @param point The vector to be limited.
 * @addr 0x5832F0
 * @returns Magnitude of the vector before limiting.
 */
float CRadar::LimitRadarPoint(CVector2D& point) {
    const auto mag = point.Magnitude();

    if (FrontEndMenuManager.m_bDrawingMap)
        return mag;

    if (mag > 1.0f) {
        // not in unit circle
        point.Normalise();
    }

    return mag;
}

// 0x583350
void CRadar::LimitToMap(float& x, float& y) {
    const auto zoom = FrontEndMenuManager.m_bMapLoaded ? FrontEndMenuManager.m_fMapZoom : 140.0f;

    {
        const auto min = SCREEN_STRETCH_X(FrontEndMenuManager.m_vMapOrigin.x - zoom);
        const auto max = SCREEN_STRETCH_X(FrontEndMenuManager.m_vMapOrigin.x + zoom);
        x = std::clamp(x, min, max);
    }

    {
        const auto min = SCREEN_STRETCH_Y(FrontEndMenuManager.m_vMapOrigin.y - zoom);
        const auto max = SCREEN_STRETCH_Y(FrontEndMenuManager.m_vMapOrigin.y + zoom);
        y = std::clamp(y, min, max);
    }
}

/*!
 * @brief Calculates transparency of a blip based on distance from the player.
 * @brief This function returns full opacity value if the map is being drawn.
 * @brief Blip transparency is not used in SA. It's a VC feature.
 * @param distance Distance from the player
 * @addr 0x583420
 * @returns Transparency value in range of [70,255].
 */
uint8 CRadar::CalculateBlipAlpha(float distance) {
    if (FrontEndMenuManager.m_bDrawingMap) {
        return 255;
    }

    const auto alpha = 255 - (uint32)(distance / 6.0f * 255.0f);
    return (uint8)std::max((float)alpha, 70.0f);
}

/*!
 * @brief Transforms a radar point to screen.
 * @brief Unhooked by default for now. Causes `DrawRadarSection` to crash.
 * @addr 0x583480
 */
void CRadar::TransformRadarPointToScreenSpace(CVector2D& out, const CVector2D& in) {
    if (FrontEndMenuManager.m_bDrawingMap) {
        out = {
            FrontEndMenuManager.m_vMapOrigin.x + FrontEndMenuManager.m_fMapZoom * in.x,
            FrontEndMenuManager.m_vMapOrigin.y - FrontEndMenuManager.m_fMapZoom * in.y
        };
    } else {
        out = {
            SCREEN_STRETCH_X(94.0f) / 2.0f + SCREEN_STRETCH_X(40.0f) + SCREEN_STRETCH_X(94.0f * in.x) / 2.0f,
            SCREEN_STRETCH_FROM_BOTTOM(104.0f) + SCREEN_STRETCH_Y(76.0f) / 2.0f - SCREEN_STRETCH_Y(76.0f * in.y) / 2.0f
        };
    }
}

/*!
 * @brief Transforms a real coordinate to radar coordinate.
 * @addr 0x583530
 */
void CRadar::TransformRealWorldPointToRadarSpace(CVector2D& out, const CVector2D& in) {
    out = CachedRotateClockwise((in - vec2DRadarOrigin) / m_radarRange);
}

/*!
 * @brief Transforms a radar coordinate to real coordinate. (Unused)
 * @addr 0x5835A0
 */
void CRadar::TransformRadarPointToRealWorldSpace(CVector2D& out, const CVector2D& in) {
    out = CachedRotateCounterclockwise(in) * m_radarRange + vec2DRadarOrigin;
}

/*!
 * @brief Transforms a radar coordinate to texture coordinate. (Inlined, see CRadar::DrawRadarSection)
 * @addr 0x583600
 */
void CRadar::TransformRealWorldToTexCoordSpace(CVector2D& out, const CVector2D& in, int32 x, int32 y) {
    out = CVector2D{
        +(in.x - (float(500 * x) - 3000.0f)),
        -(in.y - ((500 * float(MAX_RADAR_HEIGHT_TILES - y)) - 3000.0f))
    } / 500.0f;
}

// 0x583670
void CRadar::CalculateCachedSinCos() {
    if (FrontEndMenuManager.m_bDrawingMap) {
        cachedSin = std::sin(0.0f);
        cachedCos = std::cos(0.0f);

        return;
    }

    const auto SaveAngle = [](float angle) {
        m_fRadarOrientation = angle;
        cachedSin = std::sin(angle);
        cachedCos = std::cos(angle);
    };

    if (TheCamera.GetLookDirection() == LOOKING_DIRECTION_FORWARD) {
        SaveAngle(TheCamera.GetHeading());

        return;
    }

    const auto& activeCam = CCamera::GetActiveCamera();
    const auto targetEntity = activeCam.m_pCamTargetEntity;

    const auto directionToTarget = [&] {
        if (activeCam.m_nMode == MODE_1STPERSON || TheCamera.GetLookDirection() == LOOKING_DIRECTION_UNKNOWN_1) {
            return targetEntity->GetMatrix().GetForward().Normalized();
        } else {
            return targetEntity->GetPosition() - activeCam.m_vecSourceBeforeLookBehind;
        }
    }();

    SaveAngle(directionToTarget.Heading());
}

/*!
 * @brief Creates a new coordinate blip (i.e. tracing blip with no sprite)
 * @param type Type
 * @param posn Position
 * @param color Color (Unused)
 * @param blipDisplay Display option
 * @param scriptName Script name (Unused) (from Android)
 * @addr 0x583820
 * @returns Handle of the blip created.
 */
tBlipHandle CRadar::SetCoordBlip(eBlipType type, CVector posn, eBlipColour color, eBlipDisplay blipDisplay, const char* scriptName) {
    auto index = FindTraceNotTrackingBlipIndex();
    if (index == -1)
        return -1;

    auto& t = ms_RadarTrace[index];

    t.m_vPosition        = posn;
    t.m_nBlipDisplayFlag = blipDisplay;
    t.m_nBlipType        = type;
    t.m_nColour          = BLIP_COLOUR_DESTINATION;
    t.m_fSphereRadius    = 1.f;
    t.m_nEntityHandle    = 0;
    t.m_nBlipSize        = 1;
    t.m_nBlipSprite      = eRadarSprite::RADAR_SPRITE_NONE;
    t.m_bBright          = true;
    t.m_bTrackingBlip    = true;
    t.m_pEntryExit       = nullptr;

    return GetNewUniqueBlipIndex(index);
}

/*!
 * @brief Creates a new short-range coordinate blip (i.e. tracing blip with no sprite)
 * @param type Type
 * @param posn Position
 * @param color Color
 * @param blipDisplay Display option
 * @param scriptName Script name (Unused) (from Android)
 * @addr 0x583920
 * @returns Handle of the blip created.
 */
tBlipHandle CRadar::SetShortRangeCoordBlip(eBlipType type, CVector posn, eBlipColour color, eBlipDisplay blipDisplay, const char* scriptName) {
    if (const auto blip = SetCoordBlip(type, posn, color, blipDisplay, scriptName); blip != -1) {
        ms_RadarTrace[GetActualBlipArrayIndex(blip)].m_bShortRange = true;
        return blip;
    }

    return -1;
}

/*!
 * @brief Creates a new coordinate blip (i.e. tracing blip with no sprite) bound to an entity.
 * @param type Type
 * @param entityHandle Opaque entity handle? (CEntity*?)
 * @param arg2 Unknown argument (Unused)
 * @param blipDisplay Display option
 * @addr 0x5839A0
 * @returns Handle of the blip created.
 */
tBlipHandle CRadar::SetEntityBlip(eBlipType type, int32 entityHandle, uint32 arg2, eBlipDisplay blipDisplay) {
    if (const auto index = FindTraceNotTrackingBlipIndex(); index != -1) {
        auto& t = ms_RadarTrace[index];

        t.m_nBlipDisplayFlag = blipDisplay;
        t.m_nColour = (type == BLIP_CHAR || type == BLIP_CAR) ? BLIP_COLOUR_THREAT : BLIP_COLOUR_GREEN;
        t.m_nEntityHandle = entityHandle;
        t.m_fSphereRadius = 1.f;
        t.m_nBlipSize = 1;
        t.m_nBlipDisplayFlag = blipDisplay;
        t.m_nBlipType = type;
        t.m_nBlipSprite = eRadarSprite::RADAR_SPRITE_NONE;
        t.m_bBright = true;
        t.m_bTrackingBlip = true;
        t.m_pEntryExit = nullptr;

        return GetNewUniqueBlipIndex(index);
    }

    return -1;
}

/*!
 * @brief Change blip color to one of the color of a radar color set.
 * @addr 0x583AB0
 */
void CRadar::ChangeBlipColour(tBlipHandle blip, eBlipColour color) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1) {
        ms_RadarTrace[index].m_nColour = color;
    }
}

/*!
 * @brief Checks if the blip is revealed.
 * @param blipIndex Index of the blip in `ms_RadarTrace`
 * @addr 0x583AF0
 * @returns True if it's revealed to the player.
 */
bool CRadar::HasThisBlipBeenRevealed(int32 blipIndex) {
    if (!FrontEndMenuManager.m_bDrawingMap
        || !ms_RadarTrace[blipIndex].m_bShortRange
        || CTheZones::ZonesRevealed > 80
        || CTheZones::GetCurrentZoneLockedOrUnlocked(ms_RadarTrace[blipIndex].m_vPosition)
    ) {
        return true;
    }
    return false;
}

/*!
 * @brief Checks if a blip sprite is allowed to be drawn right now.
 * @param spriteId Blip sprite
 * @param priority Importance of displaying the blip.
 * @addr 0x583B40
 * @returns True if it's allowed to be drawn.
 */
bool CRadar::DisplayThisBlip(eRadarSprite spriteId, int8 priority) {
    if (CGame::CanSeeOutSideFromCurrArea() && FindPlayerPed()->m_nAreaCode == AREA_CODE_NORMAL_WORLD) {
        switch (spriteId) {
        case RADAR_SPRITE_NONE:
        case RADAR_SPRITE_WHITE:
        case RADAR_SPRITE_CENTRE:
        case RADAR_SPRITE_MAP_HERE:
        case RADAR_SPRITE_NORTH:
            return true;

        case RADAR_SPRITE_AIRYARD:
        case RADAR_SPRITE_AMMUGUN:
        case RADAR_SPRITE_BARBERS:
        case RADAR_SPRITE_BOATYARD:
        case RADAR_SPRITE_BURGERSHOT:
        case RADAR_SPRITE_BULLDOZER:
        case RADAR_SPRITE_CHICKEN:
        case RADAR_SPRITE_DINER:
        case RADAR_SPRITE_HOSTPITAL:
        case RADAR_SPRITE_MODGARAGE:
        case RADAR_SPRITE_PIZZA:
        case RADAR_SPRITE_POLICE:
        case RADAR_SPRITE_RACE:
        case RADAR_SPRITE_SAVEGAME:
        case RADAR_SPRITE_SCHOOL:
        case RADAR_SPRITE_TATTOO:
        case RADAR_SPRITE_TSHIRT:
        case RADAR_SPRITE_DATEDISCO:
        case RADAR_SPRITE_DATEDRINK:
        case RADAR_SPRITE_DATEFOOD:
        case RADAR_SPRITE_TRUCK:
        case RADAR_SPRITE_CASH:
        case RADAR_SPRITE_FLAG:
        case RADAR_SPRITE_GYM:
        case RADAR_SPRITE_IMPOUND:
        case RADAR_SPRITE_SPRAY:
            return (FrontEndMenuManager.m_ShowLocationsBlips && priority < 0) || priority == 1;

        case RADAR_SPRITE_BIGSMOKE:
        case RADAR_SPRITE_CATALINAPINK:
        case RADAR_SPRITE_CESARVIAPANDO:
        case RADAR_SPRITE_CJ:
        case RADAR_SPRITE_CRASH1:
        case RADAR_SPRITE_EMMETGUN:
        case RADAR_SPRITE_GIRLFRIEND:
        case RADAR_SPRITE_LOGOSYNDICATE:
        case RADAR_SPRITE_MADDOG:
        case RADAR_SPRITE_MAFIACASINO:
        case RADAR_SPRITE_MCSTRAP:
        case RADAR_SPRITE_OGLOC:
        case RADAR_SPRITE_RYDER:
        case RADAR_SPRITE_QMARK:
        case RADAR_SPRITE_SWEET:
        case RADAR_SPRITE_THETRUTH:
        case RADAR_SPRITE_TORENORANCH:
        case RADAR_SPRITE_TRIADS:
        case RADAR_SPRITE_TRIADSCASINO:
        case RADAR_SPRITE_WOOZIE:
        case RADAR_SPRITE_ZERO:
        case RADAR_SPRITE_GANGB:
        case RADAR_SPRITE_GANGP:
        case RADAR_SPRITE_GANGY:
        case RADAR_SPRITE_GANGN:
        case RADAR_SPRITE_GANGG:
            return (FrontEndMenuManager.m_ShowContactsBlips && priority < 0) || priority == 3;

        default:
            return (FrontEndMenuManager.m_ShowOtherBlips && priority < 0) || priority == 2;
        }
    }

    switch (spriteId) {
    case RADAR_SPRITE_NONE:
    case RADAR_SPRITE_WHITE:
    case RADAR_SPRITE_CENTRE:
    case RADAR_SPRITE_MAP_HERE:
    case RADAR_SPRITE_NORTH:
    case RADAR_SPRITE_MAFIACASINO:
    case RADAR_SPRITE_SCHOOL:
    case RADAR_SPRITE_WAYPOINT:
    case RADAR_SPRITE_TRIADSCASINO:
    case RADAR_SPRITE_CASH:
        return true;

    default:
        return false;
    }
}

/*!
 * @brief Changes the brightness of a blip. (Unused)
 * @param blip Blip handle
 * @param brightness Brightness flag. (=1: bright, ≠1: not bright)
 * @addr 0x583C70
 */
void CRadar::ChangeBlipBrightness(tBlipHandle blip, int32 brightness) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1) {
        ms_RadarTrace[index].m_bBright = brightness == 1 ? true : false;
    }
}

/*!
 * @brief Changes the size of a blip.
 * @brief This function does not effect the blip size if the map is being drawn.
 * @addr 0x583CC0
 */
void CRadar::ChangeBlipScale(tBlipHandle blip, int32 size) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1) {
        ms_RadarTrace[index].m_nBlipSize = FrontEndMenuManager.m_bDrawingMap ? 1 : size;
    }
}

/*!
 * @brief Changes the display option of a blip.
 * @addr 0x583D20
 */
void CRadar::ChangeBlipDisplay(tBlipHandle blip, eBlipDisplay blipDisplay) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1) {
        ms_RadarTrace[index].m_nBlipDisplayFlag = blipDisplay;
    }
}

/*!
 * @brief Changes the sprite of a blip.
 * @param blip Blip
 * @param spriteId Blip sprite
 * @addr 0x583D70
 */
void CRadar::SetBlipSprite(tBlipHandle blip, eRadarSprite spriteId) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1 && ms_RadarTrace[index].m_bTrackingBlip) {
        ms_RadarTrace[index].m_nBlipSprite = spriteId;
    }
}

/*!
 * @brief Sets a blip to shown always in zoom.
 * @addr 0x583DB0
 */
void CRadar::SetBlipAlwaysDisplayInZoom(tBlipHandle blip, bool display) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1 && ms_RadarTrace[index].m_bTrackingBlip) {
        ms_RadarTrace[index].m_bBlipRemain = display;
    }
}

/*!
 * @brief Sets a blip to fade. (Unused?)
 * @addr 0x583E00
 */
void CRadar::SetBlipFade(tBlipHandle blip, bool fade) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1) {
        ms_RadarTrace[index].m_bBlipFade = fade;
    }
}

/*!
 * @brief Changes the appearnace of a blip.
 * @addr 0x583E50
 */
void CRadar::SetCoordBlipAppearance(tBlipHandle blip, eBlipAppearance appearance) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1) {
        auto& trace = ms_RadarTrace[index];
        if (trace.m_nBlipType != eBlipType::BLIP_CAR) {
            return;
        }

        switch (appearance) { // Seems like there's a 3rd appearance type?
        case BLIP_FLAG_FRIEND:
        case BLIP_FLAG_THREAT:
        case BLIP_FLAG_UNK:
            trace.m_nAppearance = appearance;
            break;
        }
    }
}

/*!
 * @brief Sets a blip as friendly.
 * @addr 0x583EB0
 */
void CRadar::SetBlipFriendly(tBlipHandle blip, bool friendly) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1) {
        ms_RadarTrace[index].m_bFriendly = friendly;
    }
}

/*!
 * @brief Sets an entry point to a blip.
 * @addr 0x583F00
 */
void CRadar::SetBlipEntryExit(tBlipHandle blip, CEntryExit* enex) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1) {
        auto& trace = ms_RadarTrace[index];
        if (trace.m_bTrackingBlip) {
            trace.m_pEntryExit = enex;
        }
    }
}

/*!
 * @brief Limits stretched X, Y values to the map
 * @brief This function does nothing if the map is not being drawn.
 * @brief This code piece seems fairly common.. Perhaps its inlined?
 */
void Limit(float& x, float& y) {
    if (FrontEndMenuManager.m_bDrawingMap) {
        x = SCREEN_STRETCH_X(x);
        y = SCREEN_STRETCH_Y(y);

        CRadar::LimitToMap(x, y);
    }
}

/*!
 * @brief Draws a square coordinate blip to the map.
 * @addr 0x583F40
 */
void CRadar::ShowRadarTrace(float x, float y, uint32 size, CRGBA color) {
    Limit(x, y);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));

    // Draw background rect - Since it's 1px bigger on each side when we draw the next rect on top of it will act as a border
    CSprite2d::DrawRect({
        x - float(size - 1),
        y - float(size - 1),
        x + float(size + 1),
        y + float(size + 1)
    }, { 0, 0, 0, color.a });

    // Now draw actual rect on top of it
    CSprite2d::DrawRect({
        x - float(size),
        y - float(size),
        x + float(size),
        y + float(size)
    }, color);
}

/*!
 * @brief Draws a coordinate blip to the map with height information.
 * @addr 0x584070
 */
void CRadar::ShowRadarTraceWithHeight(float x, float y, uint32 size, uint32 R, uint32 G, uint32 B, uint32 A, eRadarTraceHeight height) {
    Limit(x, y);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));

    // NOTE: Those RGBA parameters are 4-bytes per channel.
    // then we construct 1-byte per channel CRGBA. It's stupid isn't it?
    // NOTE: {R,G,B,A} > 255 is possible, so we can't assert for x <= UCHAR_MAX.
    const auto r = static_cast<uint8>(R), g = static_cast<uint8>(G), b = static_cast<uint8>(B), a = static_cast<uint8>(A);

    const auto size0 = float(size + 0), size1 = float(size + 1);
    const auto size2 = float(size + 2), size3 = float(size + 3);

    switch (height) {
    case RADAR_TRACE_HIGH:
        CSprite2d::Draw2DPolygon( // draw black border
            x,                           y + SCREEN_STRETCH_Y(size3),
            x,                           y + SCREEN_STRETCH_Y(size3),
            x + SCREEN_STRETCH_X(size3), y - SCREEN_STRETCH_Y(size2),
            x - SCREEN_STRETCH_X(size3), y - SCREEN_STRETCH_Y(size2),
            { 0, 0, 0, a }
        );
        CSprite2d::Draw2DPolygon( // draw triangle
            x,                           y + SCREEN_STRETCH_Y(size1),
            x,                           y + SCREEN_STRETCH_Y(size1),
            x + SCREEN_STRETCH_X(size1), y - SCREEN_STRETCH_Y(size1),
            x - SCREEN_STRETCH_X(size1), y - SCREEN_STRETCH_Y(size1),
            { r, g, b, a }
        );
        break;
    case RADAR_TRACE_NORMAL:
        CSprite2d::DrawRect( // draw black border
            {
                x - SCREEN_STRETCH_X(size1), y - SCREEN_STRETCH_Y(size1),
                x + SCREEN_STRETCH_X(size1), y + SCREEN_STRETCH_Y(size1)
            },
            { 0, 0, 0, a }
        );

        CSprite2d::DrawRect( // draw box
            {
                x - SCREEN_STRETCH_X(size0), y - SCREEN_STRETCH_Y(size0),
                x + SCREEN_STRETCH_X(size0), y + SCREEN_STRETCH_Y(size0)
            },
            { r, g, b, a }
        );
        break;
    case RADAR_TRACE_LOW:
        CSprite2d::Draw2DPolygon( // draw black border
            x + SCREEN_STRETCH_X(size3), y + SCREEN_STRETCH_Y(size2),
            x - SCREEN_STRETCH_X(size3), y + SCREEN_STRETCH_Y(size2),
            x,                           y - SCREEN_STRETCH_Y(size3),
            x,                           y - SCREEN_STRETCH_Y(size3),
            { 0, 0, 0, a }
        );

        CSprite2d::Draw2DPolygon( // draw triangle
            x + SCREEN_STRETCH_X(size1), y + SCREEN_STRETCH_Y(size1),
            x - SCREEN_STRETCH_X(size1), y + SCREEN_STRETCH_Y(size1),
            x,                           y - SCREEN_STRETCH_Y(size1),
            x,                           y - SCREEN_STRETCH_Y(size1),
            { r, g, b, a }
        );
        break;
    }
}

// show debug line at this position
// 0x584480
void CRadar::ShowRadarMarker(CVector p, uint32 color, float radius) {
    auto& mat   = TheCamera.m_mCameraMatrix;
    auto& up    = mat.GetUp();
    auto& right = mat.GetRight();

    auto r0 = radius * 0.5f;
    auto r1 = radius * 1.4f;

    CTheScripts::ScriptDebugLine3D(p + r1 * up,    p + r0 * up,    color, color);
    CTheScripts::ScriptDebugLine3D(p - r1 * up,    p - r0 * up,    color, color);
    CTheScripts::ScriptDebugLine3D(p + r1 * right, p + r0 * right, color, color);
    CTheScripts::ScriptDebugLine3D(p - r1 * right, p - r0 * right, color, color);
}

/*!
 * @brief Get game color from the blip color.
 * @addr 0x584770
 * @returns Game color
 */
uint32 CRadar::GetRadarTraceColour(eBlipColour color, bool bright, bool friendly) {
    switch (color) {
    case BLIP_COLOUR_RED:
    case BLIP_COLOUR_REDCOPY:
        return HudColour.GetIntColour(bright ? HUD_COLOUR_RED : HUD_COLOUR_DARK_RED);
    case BLIP_COLOUR_GREEN:
        return HudColour.GetIntColour(bright ? HUD_COLOUR_GREEN : HUD_COLOUR_DARK_GREEN);
    case BLIP_COLOUR_BLUE:
    case BLIP_COLOUR_BLUECOPY:
        return HudColour.GetIntColour(bright ? HUD_COLOUR_LIGHT_BLUE : HUD_COLOUR_BLUE);
    case BLIP_COLOUR_WHITE:
        return HudColour.GetIntColour(bright ? HUD_COLOUR_LIGHT_GRAY : HUD_COLOUR_DARK_GRAY);
    case BLIP_COLOUR_YELLOW:
        return HudColour.GetIntColour(bright ? HUD_COLOUR_CREAM : HUD_COLOUR_GOLD);
    case BLIP_COLOUR_THREAT:
        return HudColour.GetIntColour(friendly ? HUD_COLOUR_BLUE : HUD_COLOUR_RED);
    case BLIP_COLOUR_DESTINATION:
        return HudColour.GetIntColour(HUD_COLOUR_CREAM);
    default:
        return color;
    }
}

/*
 * @brief Draw a sprite to that is rotated.
 * @addr 0x584850
 */
void CRadar::DrawRotatingRadarSprite(CSprite2d& sprite, float x, float y, float angle, uint32 width, uint32 height, CRGBA color) {
    Limit(x, y);
    CVector2D verts[4];
    for (auto i = 0u; i < std::size(verts); i++) {
        const auto theta = (float)i * HALF_PI + (angle - FRAC_PI_4);

        verts[i].x = std::sin(theta) * (float)width + x;
        verts[i].y = std::cos(theta) * (float)height + y;
    }

    sprite.Draw(verts[3].x, verts[3].y, verts[2].x, verts[2].y, verts[0].x, verts[0].y, verts[1].x, verts[1].y, color);
}

// 0x584960
void CRadar::DrawYouAreHereSprite(float x, float y) {
    static auto& mapYouAreHereTimer = *(uint32*)0xBAA358;
    static auto& mapYouAreHereDisplay = *(bool*)0x8D0930;

    if (CTimer::GetTimeInMSPauseMode() - mapYouAreHereTimer > 700) {
        mapYouAreHereTimer = CTimer::GetTimeInMSPauseMode();
        mapYouAreHereDisplay = !mapYouAreHereDisplay;
    }

    if (mapYouAreHereDisplay) {
        const auto angle = FindPlayerHeading(0) + DegreesToRadians(180.0f);
        const auto circleAngle = angle + DegreesToRadians(90.0f);

        DrawRotatingRadarSprite(
            RadarBlipSprites[RADAR_SPRITE_MAP_HERE],
            x + 17.0f * std::cos(circleAngle),
            y - 17.0f * std::sin(circleAngle),
            angle,
            (uint32)(SCREEN_STRETCH_X(25.0f)),
            (uint32)(SCREEN_STRETCH_Y(25.0f)),
            { 255, 255, 255, 255 }
        );
    }

    MapLegendList[MapLegendCounter++] = RADAR_SPRITE_MAP_HERE;
}

// 0x584A80
void CRadar::SetupRadarRect(int32 x, int32 y) {
    m_radarRect.left   = float(500 * (x - 7));
    m_radarRect.top    = float(500 * (7 - y));
    m_radarRect.right  = float(500 * (x - 4));
    m_radarRect.bottom = float(500 * (4 - y));
}

// unused
// 0x584B00
// Returns true if either coords had to be clipped
bool ClipRadarTileCoords(int32& x, int32& y) {
    // Not quite the way they've done it, but nicer.

    const auto ox = x, oy = y;

    x = std::clamp(x, 0, (int32)MAX_RADAR_WIDTH_TILES - 1);
    y = std::clamp(y, 0, (int32)MAX_RADAR_HEIGHT_TILES - 1);

    return ox != x || oy != y;
}

// 0x584B50
void CRadar::RequestMapSection(int32 x, int32 y) {
    if (!IsMapSectionInBounds(x, y))
        return;

    if (const auto texture = gRadarTextures[y][x]; texture != -1) {
        CStreaming::RequestTxdModel(texture, STREAMING_GAME_REQUIRED | STREAMING_KEEP_IN_MEMORY);
    }
}

// 0x584BB0
void CRadar::RemoveMapSection(int32 x, int32 y) {
    if (!IsMapSectionInBounds(x, y))
        return;

    if (const auto texture = gRadarTextures[y][x]; texture != -1) {
        CStreaming::RemoveTxdModel(texture);
    }
}

// 0x584BF0
void CRadar::RemoveRadarSections() {
    for (auto y = 0u; y < MAX_RADAR_HEIGHT_TILES; y++) {
        for (auto x = 0u; x < MAX_RADAR_WIDTH_TILES; x++) {
            CStreaming::RemoveTxdModel(gRadarTextures[y][x]);
        }
    }
}

/*
 * @brief Checks if a 2D point is inside the radar. (i.e. a rect that spans -1 to 1 in both directions)
 * @addr 0x584D40
 */
bool IsPointInsideRadar(const CVector2D& point) {
    return std::abs(point.x) < 1.0f
        && std::abs(point.y) < 1.0f;
}

// 0x584D90
void GetTextureCorners(int32 x, int32 y, CVector2D* corners) {
    // Magic numbers probably come from `MAX_RADAR_WIDTH_TILES` and `MAX_RADAR_HEIGHT_TILES`

    corners[0] = {500.0f * float(x - 6), 500.0f * float(5 - y)};
    corners[1] = {500.0f * float(x - 5), 500.0f * float(5 - y)};
    corners[2] = {500.0f * float(x - 5), 500.0f * float(6 - y)};
    corners[3] = {500.0f * float(x - 6), 500.0f * float(6 - y)};
}

// Returns number of intersections
// 0x584E00
int32 LineRadarBoxCollision(CVector2D& result, const CVector2D& lineStart, const CVector2D& lineEnd) {
    return plugin::CallAndReturn<int32, 0x584E00, CVector2D&, const CVector2D&, const CVector2D&>(result, lineStart, lineEnd);
}

// 0x585040
int32 CRadar::ClipRadarPoly(CVector2D* out, const CVector2D* in) {
    return plugin::CallAndReturn<int32, 0x585040, CVector2D*, const CVector2D*>(out, in);
}

// 0x5853D0
void CRadar::DrawAreaOnRadar(const CRect& rect, const CRGBA& color, bool inMenu) {
    return plugin::Call<0x5853D0, const CRect&, const CRGBA&, bool>(rect, color, inMenu);

    if (!m_radarRect.IsRectInside(rect)) {
        return;
    }

    //// Corner positions - Not transformed
    //const CVector2D rectCorners[]{
    //    {rect.right, rect.top},     // Top right
    //    {rect.right, rect.bottom},  // Bottom right

    //    {rect.left,  rect.bottom},  // Bottom left
    //    {rect.left,  rect.top}      // Top left
    //};

    //// Corner positions - Transformed, not yet clipped
    //CVector2D polyUnclippedVertices[4];
    //rng::transform(rectCorners, polyUnclippedVertices, [](auto&& v) {
    //    CVector2D transformed;
    //    TransformRealWorldPointToRadarSpace(transformed, v);
    //    return transformed;
    //});

    //// Now clip all corners to be within the radar area
    //CVector2D polyVertices[4];
    //const auto nVerticesInsideRadar = ClipRadarPoly(polyVertices, polyUnclippedVertices);

    //RwTexCoords texCoords[4];
    //if (nVerticesInsideRadar) {
    //    for (auto i = 0; i < nVerticesInsideRadar; i++) {
    //}
}

// 0x585700
void CRadar::DrawRadarMask() {
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,     RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,         RWRSTATE(rwSHADEMODEFLAT));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION, RWRSTATE(rwALPHATESTFUNCTIONALWAYS));

    CVector2D out[8];
    CVector2D in;
    CVector2D corners[4] = {
        { 1.0f,  -1.0f },
        { 1.0f,  1.0f  },
        { -1.0f, 1.0f  },
        { -1.0,  -1.0f }
    };

    // Draw the shape we want to mask out from the radar in four segments
    for (auto& corner : corners) {
        // First point is always the corner itself
        in.x = corner.x;
        in.y = corner.y;
        TransformRadarPointToScreenSpace(out[0], in);

        // Then generate a quarter of the circle
        for (auto j = 0; j < 7; j++) {
            in.x = corner.x * std::cos(float(j) * (FRAC_PI_2 / 6.0f));
            in.y = corner.y * std::sin(float(j) * (FRAC_PI_2 / 6.0f));
            TransformRadarPointToScreenSpace(out[j + 1], in);
        };

        CSprite2d::SetMaskVertices(std::size(out), out, CSprite2d::GetNearScreenZ());
        RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::GetVertices(), std::size(out));
    }

    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION, RWRSTATE(rwALPHATESTFUNCTIONGREATER));
}

// 0x5858D0
void CRadar::StreamRadarSections(const CVector& worldPosn) {
    if (!CStreaming::ms_disableStreaming) {
        StreamRadarSections(
            (int32)std::floor((worldPosn.x + 3000.0f) / 500.0f),
            (int32)std::ceil(11.0f - (worldPosn.y + 3000.0f) / 500.0f)
        );
    }
}

// 0x584C50
void CRadar::StreamRadarSections(int32 x, int32 y) {
    plugin::Call<0x584C50, int32, int32>(x, y);
}

// 0x585960
void CRadar::InitFrontEndMap() {
    CalculateCachedSinCos();
    rng::fill(MapLegendList, RADAR_SPRITE_NONE);

    vec2DRadarOrigin.Set(0.0f, 0.0f);
    m_radarRange = WORLD_BOUND_RANGE - 10.0f;
    MapLegendCounter = 0;

    rng::fill(ArrowBlipColour, CRGBA(0, 0, 0, 0));
}

// 0x5859F0
void CRadar::AddBlipToLegendList(bool noSprite, int32 blipIndex) {
    if (!FrontEndMenuManager.m_bDrawingMap)
        return;

    auto& trace = ms_RadarTrace[blipIndex];

    const auto sprite = [&] {
        if (!noSprite)
            return eRadarSprite(blipIndex);

        switch (trace.m_nBlipType) {
        case BLIP_CAR:
        case BLIP_CHAR:
            return trace.m_bFriendly ? RADAR_SPRITE_FRIEND : RADAR_SPRITE_THREAT;
        case BLIP_OBJECT:
            return RADAR_SPRITE_OBJECT;
        case BLIP_COORD:
            return RADAR_SPRITE_DESTINATION;
        default:
            return RADAR_SPRITE_PLAYER_INTEREST;
        }
    }();

    // check if it's not already added.
    if (!notsa::contains(MapLegendList, sprite)) {
        MapLegendList[MapLegendCounter++] = sprite;

        if (noSprite) {
            const CRGBA color = GetRadarTraceColour(trace.m_nColour, trace.m_bBright, trace.m_bFriendly);

            ArrowBlipColour[-sprite].Set(color, 255);
        }
    }
}

// 0x585B20
void CRadar::SetMapCentreToPlayerCoords() {
    if (FindPlayerPed() == nullptr)
        return;

    FrontEndMenuManager.m_bDrawingMap = true;

    InitFrontEndMap();

    CVector2D posReal = FindPlayerCentreOfWorld_NoInteriorShift(0);

    if (CTheScripts::HideAllFrontEndMapBlips || CTheScripts::bPlayerIsOffTheMap)
        posReal.Set(0.0f, 0.0f);

    CVector2D posRadar;
    TransformRealWorldPointToRadarSpace(posRadar, posReal);
    LimitRadarPoint(posRadar);

    FrontEndMenuManager.m_vMousePos = posReal;
    FrontEndMenuManager.m_vMapOrigin.x = DEFAULT_SCREEN_WIDTH  / 2.0f - FrontEndMenuManager.m_fMapZoom * posRadar.x;
    FrontEndMenuManager.m_vMapOrigin.y = DEFAULT_SCREEN_HEIGHT / 2.0f + FrontEndMenuManager.m_fMapZoom * posRadar.y;
    FrontEndMenuManager.m_bDrawingMap = false;
}

// 0x585BF0
void CRadar::Draw3dMarkers() {
    const auto PutMarkerCone = [](uint32 id, CVector pos, float size, CRGBA color) {
        C3dMarkers::PlaceMarkerCone(id, pos, size, color.r, color.g, color.b, 255, 1024u, 0.2f, 5, true);
    };

    for (auto&& [i, trace] : notsa::enumerate(ms_RadarTrace)) {
        if (!trace.m_bTrackingBlip) {
            continue;
        }

        const auto color = CRGBA{GetRadarTraceColour(trace.m_nColour, trace.m_bBright, trace.m_bFriendly)};
        // TODO: make a tConeHandle or something for this.
        const auto coneHandle = (uint32)i | ((uint32)trace.m_nCounter << (sizeof(uint16) * 8));

        if (trace.m_nBlipDisplayFlag != BLIP_DISPLAY_BOTH && trace.m_nBlipDisplayFlag != BLIP_DISPLAY_MARKERONLY) {
            continue;
        }

        switch (trace.m_nBlipType) {
        case BLIP_CAR: {
            const auto vehicle = GetVehiclePool()->GetAtRef(trace.m_nEntityHandle);
            assert(vehicle); // NOTSA

            const auto posn = [vehicle] {
                auto bbMaxZ = vehicle->GetColModel()->GetBoundingBox().m_vecMax.z;
                auto vehPos = vehicle->GetPosition();

                bbMaxZ *= ModelIndices::IsNevada(vehicle->m_nModelIndex) ? (5.0f / 3.0f) : (6.0f / 5.0f);

                return vehPos + CVector{0.0f, 0.0f, bbMaxZ + 2.0f};
            }();

            PutMarkerCone(coneHandle, posn, 2.0f, color);
            break;
        }
        case BLIP_CHAR: {
            const auto ped = GetPedPool()->GetAtRef(trace.m_nEntityHandle);
            assert(ped); // NOTSA

            PutMarkerCone(coneHandle, ped->GetRealPosition() + CVector{0.0f, 0.0f, 2.7f}, 1.2f, color);
            break;
        }
        case BLIP_OBJECT:
        case BLIP_PICKUP: {
            const auto posn = [&trace]() {
                CVector ret{};
                if (trace.m_nBlipType == BLIP_OBJECT) {
                    if (const auto obj = GetObjectPool()->GetAtRef(trace.m_nEntityHandle)) {
                        const auto bbMaxZ = obj->GetColModel()->GetBoundingBox().m_vecMax.z;
                        ret = obj->GetPosition() + CVector{0.0f, 0.0f, bbMaxZ};
                    } else {
                        NOTSA_UNREACHABLE("Couldn't get the object!");
                    }
                } else { // OBJECT_PICKUP
                    if (const auto idx = CPickups::GetActualPickupIndex(trace.m_nEntityHandle); idx >= 0) {
                        ret = CPickups::aPickUps[idx].GetPosn() + CVector{0.0f, 0.0f, 2.0f};
                    } else {
                        NOTSA_UNREACHABLE("Couldn't get the pickup!");
                    }
                }
                ret.z += (CGame::currArea != 0 || FindPlayerPed()->m_nAreaCode != AREA_CODE_NORMAL_WORLD) ? 1.6f : 1.8f;

                return ret;
            }();

            PutMarkerCone(coneHandle, posn, 0.8f, color);
            break;
        }
        case BLIP_CONTACT_POINT: {
            if (CTheScripts::IsPlayerOnAMission() || !FindPlayerPed())
                break;

            if (!trace.m_bTrackingBlip && FindPlayerPed()->m_nAreaCode != AREA_CODE_NORMAL_WORLD)
                break;

            C3dMarkers::PlaceMarkerSet(coneHandle, MARKER3D_CYLINDER, trace.m_vPosition, 2.0f, 255, 0, 0, 228, 2048u, 0.2f, 0);
            break;
        }
        default:
            break;
        }
    }
}

// unused
// 0x585FE0
void CRadar::SetRadarMarkerState(int32 counter, bool flag) {
    // NOP

    /* III/VC leftover
    const auto GetEntity = [=]() -> CEntity* {
        const auto handle = ms_RadarTrace[counter].m_nEntityHandle;
        switch (ms_RadarTrace[counter].m_nBlipType) {
        case BLIP_CAR:
            return GetVehiclePool()->GetAt(handle);
        case BLIP_CHAR:
            return GetPedPool()->GetAt(handle);
        case BLIP_OBJECT:
            return GetObjectPool()->GetAt(handle);
        default:
            return nullptr;
        }
    };

    if (auto* entity = GetEntity()) {
        entity->bHasBlip = flag;
    }
    */
}

/*
 * @brief Draw a sprite to specific location and opacity.
 * @addr 0x585FF0
 */
void CRadar::DrawRadarSprite(eRadarSprite spriteId, float x, float y, uint8 alpha) {
    Limit(x, y);

    const auto width  = std::floor(SCREEN_STRETCH_X(8.0f));
    const auto height = std::floor(SCREEN_STRETCH_Y(8.0f));

    if (DisplayThisBlip(spriteId, -99)) {
        RadarBlipSprites[(size_t)spriteId].Draw(
            { x - width, y - height, x + width, y + height },
            { 255, 255, 255, alpha }
        );
        AddBlipToLegendList(false, spriteId);
    }
}

// 0x586110
void CRadar::DrawRadarSection(int32 x, int32 y) {
    CVector2D clipped[4]{};
    const auto numVerts = [x, y, &clipped] {
        CVector2D corners[4]{};
        GetTextureCorners(x, y, corners);

        CVector2D rotated[4]{};
        for (auto&& [i, corner] : notsa::enumerate(corners)) {
            rotated[i] = CachedRotateClockwise((corner - vec2DRadarOrigin) / m_radarRange);
        }
        return ClipRadarPoly(clipped, rotated);
    }();

    CVector2D texCoords[4]{};
    CVector2D verts[4]{};
    for (auto i = 0; i < numVerts; i++) { // numVerts is max 4
        const auto coord = CachedRotateCounterclockwise(clipped[i]) * m_radarRange + vec2DRadarOrigin;

        TransformRealWorldToTexCoordSpace(texCoords[i], coord, x, y);
        TransformRadarPointToScreenSpace(verts[i], clipped[i]);
    }

    if (!IsMapSectionInBounds(x, y)) {
        // there is no land here, draw the sea.
        const CRGBA seaColor{111, 137, 170, 255};

        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, nullptr);
        CSprite2d::SetVertices(numVerts, verts, seaColor);
    } else {
        if (CTheScripts::bPlayerIsOffTheMap) {
            const CRGBA blankColor{204, 204, 204, 255};

            RwRenderStateSet(rwRENDERSTATETEXTURERASTER, nullptr);
            CSprite2d::SetVertices(numVerts, verts, blankColor);
        } else {
            const auto texture = [x, y] {
                if (const auto txdIndex = gRadarTextures[y][x]) {
                    if (const auto txd = CTxdStore::GetTxd(txdIndex)) {
                        return GetFirstTexture(txd);
                    }
                }
                NOTSA_UNREACHABLE("Couldn't load texture for a map section that supposed to be valid!");
            }();
            const CRGBA bg{255, 255, 255, 255};

            RwRenderStateSet(rwRENDERSTATETEXTURERASTER, texture->raster);
            CSprite2d::SetVertices(numVerts, verts, texCoords, bg);
        }
    }
    if (numVerts > 2) {
        RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::maVertices, numVerts);
    }
}

// 0x586520
void CRadar::DrawRadarSectionMap(int32 x, int32 y, CRect rect) {
    if (!IsMapSectionInBounds(x, y))
        return;

    const auto txdIndex = gRadarTextures[y][x];
    if (txdIndex == -1)
        return;

    if (const auto txd = CTxdStore::GetTxd(txdIndex)) {
        if (const auto texture = GetFirstTexture(txd)) {
            const auto bg = CRGBA{255, 255, 255, 255};
            RwRenderStateSet(rwRENDERSTATETEXTURERASTER, texture->raster);

            CSprite2d::SetVertices(rect, bg, bg, bg, bg);
            RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, CSprite2d::maVertices, 4);
        }
    }
}

// 0x586650
void CRadar::DrawRadarGangOverlay(bool inMenu) {
    static uint32& g_RadarGangResetOverlay = *(uint32*)0xBAA36C; // bool?
    static CRect& g_RadarGangOverlay = *(CRect*)0xBAA35C;

    if ((g_RadarGangResetOverlay & 1) == 0) {
        g_RadarGangResetOverlay |= 1u;
        g_RadarGangOverlay = CRect{};
    }

    if (!CGangWars::bGangWarsActive || !FrontEndMenuManager.m_abPrefsMapBlips[4])
        return;

    for (auto& zone : CTheZones::GetNavigationZones()) {
        const auto info = CTheZones::GetZoneInfo(&zone);

        if (!info || !info->radarMode || !CGangWars::CanPlayerStartAGangWarHere(info))
            continue;

        g_RadarGangOverlay = zone.GetRect();

        // todo: enum
        switch (info->radarMode) {
        case 1:
            DrawAreaOnRadar(g_RadarGangOverlay, info->ZoneColor, inMenu);
            break;
        case 2: {
            const auto color = [&]() -> CRGBA {
                const auto timeInMS = FrontEndMenuManager.m_bDrawingMap ? CTimer::GetTimeInMSPauseMode() : CTimer::GetTimeInMS();

                auto zoneColor = info->ZoneColor;
                zoneColor.a = (uint8)((std::sin((float)(timeInMS % 1024) * (1024.f / TWO_PI)) + 1.0f) / 2.0f * (float)zoneColor.a);

                return zoneColor;
            }();
            DrawAreaOnRadar(g_RadarGangOverlay, color, inMenu);
            break;
        }
        default:
            break;
        }
    }
}

// 0x586880
void CRadar::DrawRadarMap() {
    DrawRadarMask();

    auto x = (int32)std::floor((vec2DRadarOrigin.x + 3000.0f) / 500.0f);  // todo: inlined
    auto y = (int32)std::ceil(11.0f - (vec2DRadarOrigin.y + 3000.0f) / 500.0f);

    SetupRadarRect(x, y);
    StreamRadarSections(x, y);

    RwRenderStateSet(rwRENDERSTATEFOGENABLE,          RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,           RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,          RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,      RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,          RWRSTATE(D3DSHADE_FLAT));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,        RWRSTATE(rwRENDERSTATETEXTURERASTER));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,       RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,  RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,     RWRSTATE(D3DTADDRESS_CLAMP));
    RwRenderStateSet(rwRENDERSTATETEXTUREPERSPECTIVE, RWRSTATE(FALSE));

    DrawRadarSection(x - 1,     y - 1);
    DrawRadarSection(x,         y - 1);
    DrawRadarSection(x + 1,     y - 1);
    DrawRadarSection(x - 1,     y    );
    DrawRadarSection(x,         y    );
    DrawRadarSection(x + 1,     y    );
    DrawRadarSection(x - 1,     y + 1);
    DrawRadarSection(x,         y + 1);
    DrawRadarSection(x + 1,     y + 1);

    DrawRadarGangOverlay(false);

    const auto vehicle = FindPlayerVehicle();

    // Draw green rectangle when in plane
    if (vehicle && vehicle->IsSubPlane() && !ModelIndices::IsVortex(vehicle->m_nModelIndex)) {
        CVector playerPos = FindPlayerCentreOfWorld_NoInteriorShift(0);

        const auto cSin = cachedSin;
        const auto cCos = cachedCos;
        cachedSin = std::sin(PI);
        cachedCos = std::cos(PI);

        // Pitch and roll combined
        const float angle = std::atan2(-vehicle->m_matrix->GetForward().z, vehicle->m_matrix->GetUp().z);

        DrawAreaOnRadar(
            {
                playerPos.x - 1000.0f,
                playerPos.y - (2.0f * RadiansToDegrees(angle)),
                playerPos.x + 1000.0f,
                playerPos.y + 2000.0f
            },
            { 20, 175, 20, 200 },
            false
        );

        cachedSin = cSin;
        cachedCos = cCos;
    }

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(FALSE));
}

// 0x586B00
void CRadar::DrawMap() {
    const auto player = FindPlayerPed();
    const auto mapShouldDrawn = !CGame::currArea && player->m_nAreaCode == 0 && FrontEndMenuManager.m_nRadarMode != 1;

    CalculateCachedSinCos();

    const auto vehicle = FindPlayerVehicle();
    const auto playerInfo = player->GetPlayerInfoForThisPlayerPed();

    if (!vehicle || playerInfo->IsPlayerInRemoteMode()) {
        if (CTheScripts::RadarZoomValue)
            m_radarRange = RADAR_MIN_RANGE - (float)CTheScripts::RadarZoomValue;
        else
            m_radarRange = RADAR_MIN_RANGE;
    } else {
        if (vehicle && vehicle->IsSubPlane() && !ModelIndices::IsVortex(vehicle->m_nModelIndex)) {
            const auto speedZ = vehicle->GetPosition().z * 1.0f / 200.0f;

            if (speedZ < RADAR_MIN_SPEED)
                m_radarRange = RADAR_MAX_RANGE - 10.0f;
            else if (speedZ < RADAR_MAX_SPEED)
                m_radarRange = (speedZ - RADAR_MIN_SPEED) * (1.0f / 60.0f) + (RADAR_MAX_RANGE - 10.0f);
            else
                m_radarRange = RADAR_MAX_RANGE;
        } else {
            const auto speed = FindPlayerSpeed().Magnitude();

            if (speed < RADAR_MIN_SPEED)
                m_radarRange = RADAR_MIN_RANGE;
            else if (speed >= RADAR_MAX_SPEED)
                m_radarRange = RADAR_MAX_RANGE;
            else
                m_radarRange = (speed - RADAR_MIN_SPEED) * (850.0f / (RADAR_MIN_SPEED * 10.0f)) + RADAR_MIN_RANGE;
        }
    }

    vec2DRadarOrigin = []() -> CVector2D {
        if (!CGameLogic::IsCoopGameGoingOn()) [[likely]] {
            return FindPlayerCentreOfWorld_NoInteriorShift(0);
        } else if (CGameLogic::n2PlayerPedInFocus == eFocusedPlayer::PLAYER2) {
            return FindPlayerCentreOfWorld_NoInteriorShift(1);
        } else {
            return (FindPlayerCentreOfWorld_NoInteriorShift(0) + FindPlayerCentreOfWorld_NoInteriorShift(1)) / 2.0f; // Halfway between the two player's positions
        }
    }();

    if (mapShouldDrawn) {
        DrawRadarMap();
    }
}

/*
 * @brief Draw a blip by its index in `ms_RadarTrace`
 * @addr 0x586D60
 */
void CRadar::DrawCoordBlip(int32 blipIndex, bool isSprite) {
    const auto& trace = ms_RadarTrace[blipIndex];
    if (trace.m_nBlipType == BLIP_CONTACT_POINT && CTheScripts::IsPlayerOnAMission())
        return;

    if (isSprite == !trace.HasSprite())
        return;

    if (trace.m_nBlipDisplayFlag != BLIP_DISPLAY_BOTH && trace.m_nBlipDisplayFlag != BLIP_DISPLAY_BLIPONLY)
        return;

    float realDist{};
    const auto [radarPos, screenPos] = trace.GetRadarAndScreenPos(&realDist);
    const auto zoomedDist = CTheScripts::RadarZoomValue != 0u ? 255.0f : realDist;

    if (isSprite) {
        // either the blip is close to the player or we're drawing the whole map.
        const auto canBeDrawn = !trace.m_bShortRange || zoomedDist <= 1.0f || FrontEndMenuManager.m_bDrawingMap;

        if (trace.HasSprite() && canBeDrawn && HasThisBlipBeenRevealed(blipIndex)) {
            DrawRadarSprite(trace.m_nBlipSprite, screenPos.x, screenPos.y, 255);
        }
        return;
    }

    if (trace.HasSprite())
        return;

    if (FrontEndMenuManager.m_bDrawingMap && !FrontEndMenuManager.m_ShowMissionBlips) {
        // drawing the whole map but mission blips are hidden by the player.
        return;
    }

    const auto GetHeight = [&] {
        const auto zDiff = trace.GetWorldPos().z - FindPlayerCentreOfWorld_NoInteriorShift(PED_TYPE_PLAYER1).z;

        if (zDiff > 2.0f) {
            // trace is higher
            return RADAR_TRACE_LOW;
        } else if (zDiff >= -4.0f) {
            // they are at the around the same elevation.
            return RADAR_TRACE_NORMAL;
        } else {
            // player is higher
            return RADAR_TRACE_HIGH;
        }
    };

    const auto color = trace.GetStaticColour();
    CRadar::ShowRadarTraceWithHeight(
        screenPos.x,
        screenPos.y,
        trace.m_nBlipSize,
        color.r,
        color.g,
        color.b,
        trace.m_bBlipFade ? color.a : CalculateBlipAlpha(realDist),
        GetHeight()
    );

    AddBlipToLegendList(true, blipIndex);
}

// 0x587000
void CRadar::DrawEntityBlip(int32 blipIndex, uint8 arg1) {
    plugin::Call<0x587000, int32, uint8>(blipIndex, arg1);
}

/*
 * @brief Clear a blip by its index in `ms_RadarTrace`
 * @addr 0x587C10
 */
void CRadar::ClearActualBlip(int32 blipIndex) {
    if (blipIndex < 0 || blipIndex >= MAX_RADAR_TRACES)
        return;

    ClearActualBlip(ms_RadarTrace[blipIndex]);
}

/*
 * @notsa
 * @brief Clear a blip by its trace reference.
 */
void CRadar::ClearActualBlip(tRadarTrace& trace) {
    trace.m_nBlipSize = 1;
    trace.m_fSphereRadius = 1.0f;
    trace.m_pEntryExit = nullptr;
    trace.m_nBlipSprite = RADAR_SPRITE_NONE;

    trace.m_bBright       = true;
    trace.m_bTrackingBlip = false;
    trace.m_bShortRange   = false;
    trace.m_bFriendly     = false;
    trace.m_bBlipRemain   = false;
    trace.m_bBlipFade     = false;

    trace.m_nCoordBlipAppearance = BLIP_FLAG_FRIEND;
    trace.m_nBlipDisplayFlag = BLIP_DISPLAY_NEITHER;
    trace.m_nBlipType = BLIP_NONE;
}

/*
 * @brief Clear a blip by type and handle of the entity.
 * @addr 0x587C60
 */
void CRadar::ClearBlipForEntity(eBlipType blipType, int32 entityHandle) {
    for (auto& trace : ms_RadarTrace) {
        if (trace.m_nBlipType == blipType && trace.m_nEntityHandle == entityHandle) {
            ClearActualBlip(trace);
        }
    }
}

void CRadar::ClearBlipForEntity(CPed* ped) {
    ClearBlipForEntity(BLIP_CHAR, GetPedPool()->GetIndex(ped));
}

/*
 * @brief Clear a blip
 * @addr 0x587CE0
 */
void CRadar::ClearBlip(tBlipHandle blip) {
    if (const auto index = GetActualBlipArrayIndex(blip); index != -1) {
        ClearActualBlip(index);
    }
}

CVector GetAirStripLocation(eAirstripLocation location) {
    return CVector(airstrip_table[location].position);
}

// 0x587D20
void CRadar::SetupAirstripBlips() {
    if (const auto veh = FindPlayerVehicle(); veh && veh->IsSubPlane() && !ModelIndices::IsVortex(veh->m_nModelIndex)) {
        // FIX_BUGS: FPS independent counter. SA: (CTimer::GetFrameCounter() & 4) == 0
        static auto airstripBlipCounter = CTimer::GetTimeInMS() + 200;
        if (CTimer::GetTimeInMS() >= airstripBlipCounter) {
            airstripBlipCounter = CTimer::GetTimeInMS() + 200;

            if (airstrip_blip)
                return;

            const auto pos = GetAirStripLocation(airstrip_location);
            airstrip_blip = SetCoordBlip(BLIP_AIRSTRIP, pos, BLIP_COLOUR_RED, BLIP_DISPLAY_BLIPONLY);
            return;
        }

        // NOTSA, effectively the same thing though.
        const auto location = [veh] {
            float distances[NUM_AIRSTRIPS]{};
            for (auto&& [i, table] : notsa::enumerate(airstrip_table)) {
                distances[i] = DistanceBetweenPoints2D(table.position, veh->GetPosition());
            }

            return (eAirstripLocation)std::distance(distances, rng::min_element(distances));
        }();

        if (airstrip_blip) {
            if (airstrip_location == location)
                return;

            ClearBlip(airstrip_blip);
            airstrip_location = location;
        }

        const auto pos = GetAirStripLocation(airstrip_location);
        airstrip_blip = SetCoordBlip(BLIP_AIRSTRIP, pos, BLIP_COLOUR_RED, BLIP_DISPLAY_BLIPONLY);
        return;
    }

    if (airstrip_blip) {
        // player is not flying anymore.
        ClearBlip(airstrip_blip);
        airstrip_blip = 0;
    }
}

// 0x588050
void CRadar::DrawBlips() {
    SetupAirstripBlips();
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(false));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(false));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(true));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(false));

    const auto TransformRealWorldPointToBlipSpace = [](const CVector2D& pos) {
        CVector2D radar{}, screen{};
        TransformRealWorldPointToRadarSpace(radar, pos);
        LimitRadarPoint(radar);
        TransformRadarPointToScreenSpace(screen, radar);

        return screen;
    };

    if (FrontEndMenuManager.m_bDrawingMap) {
        if (FrontEndMenuManager.m_bMapLoaded) {
            // draw the menu map ruler.
            const auto mouseX = FrontEndMenuManager.m_nMousePosX, mouseY = FrontEndMenuManager.m_nMousePosY;

            if (!CPad::IsMouseLButton() &&
                mouseX > SCREEN_STRETCH_X(60.0f) && mouseX < SCREEN_STRETCH_FROM_RIGHT(60.0f) &&
                mouseY > SCREEN_STRETCH_Y(60.0f) && mouseY < SCREEN_STRETCH_FROM_BOTTOM(60.0f) ||
                !FrontEndMenuManager.m_bDrawMouse)
            {
                auto drawPos = TransformRealWorldPointToBlipSpace(FrontEndMenuManager.m_vMousePos);
                LimitToMap(drawPos.x, drawPos.y);

                CSprite2d::DrawRect(
                    {
                        SCREEN_STRETCH_X(drawPos.x) - 1.0f,
                        0.0f,
                        SCREEN_STRETCH_X(drawPos.x) + 1.0f,
                        SCREEN_HEIGHT
                    },
                    HudColour.GetRGB(HUD_COLOUR_GOLD)
                );

                CSprite2d::DrawRect(
                    {
                        0.0f,
                        SCREEN_STRETCH_Y(drawPos.y) - 1.0f,
                        SCREEN_WIDTH,
                        SCREEN_STRETCH_Y(drawPos.y) + 1.0f,
                    },
                    HudColour.GetRGB(HUD_COLOUR_GOLD)
                );
            }
        }
    } else {
        // draw the (N) blip.
        const auto drawPos = TransformRealWorldPointToBlipSpace(CVector2D{0.0f, m_radarRange * SQRT_2} + vec2DRadarOrigin);
        DrawRadarSprite(RADAR_SPRITE_NORTH, drawPos.x, drawPos.y, 255);
    }

    // we first do whole thing with isSprite = true, then = false... yeah.
    for (const auto isSprite : {false, true}) {
        for (auto priority = 1; priority < 4; priority++) {
            for (auto&& [i, trace] : notsa::enumerate(ms_RadarTrace)) { // todo: check if looping all
                if (!trace.m_bTrackingBlip)
                    continue;

                switch (trace.m_nBlipType) {
                case BLIP_CAR:
                case BLIP_CHAR:
                case BLIP_OBJECT:
                case BLIP_PICKUP:
                    if (DisplayThisBlip(trace.m_nBlipSprite, priority)) {
                        DrawEntityBlip(i, isSprite);
                    }
                    break;
                case BLIP_COORD:
                case BLIP_CONTACT_POINT:
                    if (trace.m_nBlipSprite != RADAR_SPRITE_WAYPOINT && DisplayThisBlip(trace.m_nBlipSprite, priority)) {
                        DrawCoordBlip(i, isSprite);
                    }
                    break;
                case BLIP_SPOTLIGHT:
                case BLIP_AIRSTRIP:
                    if (priority == 3 && (!CTheScripts::bPlayerIsOffTheMap || !FrontEndMenuManager.m_bDrawingMap)) {
                        DrawEntityBlip(i, isSprite);
                    }
                    break;
                default:
                    break;
                }
            }
        }

        for (auto&& [i, trace] : notsa::enumerate(ms_RadarTrace)) { // todo: check if looping all, same thing with above.
            if (!trace.m_bTrackingBlip)
                continue;

            switch (trace.m_nBlipType) {
            case BLIP_COORD:
            case BLIP_CONTACT_POINT:
                if (trace.m_nBlipSprite == RADAR_SPRITE_WAYPOINT) {
                    DrawCoordBlip(i, true);
                }
                break;
            default:
                break;
            }
        }
    }

    const auto GetPlayerMarkerPosition = [] {
        const auto playerDirection = (FindPlayerCentreOfWorld_NoInteriorShift(0) - vec2DRadarOrigin) / m_radarRange;

        CVector2D rotatedPos = {
            cachedSin * playerDirection.y + cachedCos * playerDirection.x,
            cachedCos * playerDirection.y - cachedSin * playerDirection.x
        };
        LimitRadarPoint(rotatedPos);

        CVector2D ret{};
        TransformRadarPointToScreenSpace(ret, rotatedPos);

        return ret;
    };

    if (!FrontEndMenuManager.m_bDrawingMap) {
        for (auto i = 0; i < 2; i++) {
            const auto player = FindPlayerPed(i);
            if (!player)
                continue;

            // we don't draw the player marker while flying.
            if (auto veh = FindPlayerVehicle(i); veh && veh->IsSubPlane() && !ModelIndices::IsVortex(veh->m_nModelIndex))
                continue;

            const auto pos = GetPlayerMarkerPosition();

            const auto angle = [] {
                const auto heading = FindPlayerHeading();

                if (CCamera::GetActiveCamera().m_nMode == MODE_TOPDOWN) {
                    return heading + DegreesToRadians(180.0f);
                } else {
                    return heading - m_fRadarOrientation - DegreesToRadians(180.0f);
                }
            }();

            DrawRotatingRadarSprite(
                RadarBlipSprites[RADAR_SPRITE_CENTRE],
                pos.x,
                pos.y,
                angle,
                static_cast<uint32>(SCREEN_STRETCH_X(8.0f)),
            #ifdef FIX_BUGS
                static_cast<uint32>(SCREEN_STRETCH_Y(8.0f)),
            #else
                static_cast<uint32>(SCREEN_STRETCH_X(8.0f)),
            #endif
                player->IsHidden() ? CRGBA{50, 50, 50, 255} : CRGBA{255, 255, 255, 255}
            );
        }
    } else {
        const auto pos = GetPlayerMarkerPosition();
        DrawYouAreHereSprite(pos.x, pos.y);
    }
}

/*
 * @brief Load radar blips from save file
 * @addr 0x5D53C0
 */
bool CRadar::Load() {
    Initialise();

    for (auto&& [i, trace] : notsa::enumerate(ms_RadarTrace)) {
        CGenericGameStorage::LoadDataFromWorkBuffer(&trace, sizeof(trace));
        if (trace.m_pEntryExit) {
            trace.m_pEntryExit = CEntryExitManager::mp_poolEntryExits->GetAt(i);
        }
    }

    return true;
}

/*
 * @brief Save radar blips to save file
 * @addr 0x5D5860
 */
bool CRadar::Save() {
    return plugin::CallAndReturn<bool, 0x5D5860>();

    for (auto& trace : ms_RadarTrace) {
        CEntryExit* enex = nullptr;
        if (trace.m_pEntryExit) {
            // todo:
        }

        const auto& blipType = trace.m_nBlipType;
        auto bIsTracking = false;
        if (blipType != BLIP_COORD && blipType != BLIP_CONTACT_POINT) {
            if (trace.m_bTrackingBlip) {
                trace.m_bTrackingBlip = false;
                bIsTracking = true;
            }
        }

        CGenericGameStorage::SaveDataToWorkBuffer(&trace, sizeof(trace));
        if (enex) {
            trace.m_pEntryExit = enex;
        }
        if (bIsTracking) {
            trace.m_bTrackingBlip = true;
        }
    }

    return true;

}

/*!
 * @notsa
 * @brief Get name of a blip type. (See CRadar::DrawLegend)
 * @returns The name of the blip type
 */
const GxtChar* CRadar::GetBlipName(eRadarSprite blipType) {
    switch (blipType) {
    case RADAR_SPRITE_PLAYER_INTEREST:  return TheText.Get("LG_56");
    case RADAR_SPRITE_THREAT:           return TheText.Get("LG_55");
    case RADAR_SPRITE_FRIEND:           return TheText.Get("LG_54");
    case RADAR_SPRITE_OBJECT:           return TheText.Get("LG_50");
    case RADAR_SPRITE_DESTINATION:      return TheText.Get("LG_49");
    case RADAR_SPRITE_NONE:             break;
    case RADAR_SPRITE_WHITE:            break;
    case RADAR_SPRITE_CENTRE:           break;
    case RADAR_SPRITE_MAP_HERE:         return TheText.Get("LG_01");
    case RADAR_SPRITE_NORTH:            break; // NOTSA
    case RADAR_SPRITE_AIRYARD:          return TheText.Get("LG_02");
    case RADAR_SPRITE_AMMUGUN:          return TheText.Get("LG_03");
    case RADAR_SPRITE_BARBERS:          return TheText.Get("LG_04");
    case RADAR_SPRITE_BIGSMOKE:         return TheText.Get("LG_05");
    case RADAR_SPRITE_BOATYARD:         return TheText.Get("LG_06");
    case RADAR_SPRITE_BURGERSHOT:       return TheText.Get("LG_07");
    case RADAR_SPRITE_BULLDOZER:        return TheText.Get("LG_66");
    case RADAR_SPRITE_CATALINAPINK:     return TheText.Get("LG_09");
    case RADAR_SPRITE_CESARVIAPANDO:    return TheText.Get("LG_10");
    case RADAR_SPRITE_CHICKEN:          return TheText.Get("LG_11");
    case RADAR_SPRITE_CJ:               return TheText.Get("LG_12");
    case RADAR_SPRITE_CRASH1:           return TheText.Get("LG_13");
    case RADAR_SPRITE_DINER:            return TheText.Get("LG_67");
    case RADAR_SPRITE_EMMETGUN:         return TheText.Get("LG_15");
    case RADAR_SPRITE_ENEMYATTACK:      return TheText.Get("LG_16");
    case RADAR_SPRITE_FIRE:             return TheText.Get("LG_17");
    case RADAR_SPRITE_GIRLFRIEND:       return TheText.Get("LG_18");
    case RADAR_SPRITE_HOSTPITAL:        return TheText.Get("LG_19");
    case RADAR_SPRITE_LOGOSYNDICATE:    return TheText.Get("LG_20");
    case RADAR_SPRITE_MADDOG:           return TheText.Get("LG_21");
    case RADAR_SPRITE_MAFIACASINO:      return TheText.Get("LG_22");
    case RADAR_SPRITE_MCSTRAP:          return TheText.Get("LG_23");
    case RADAR_SPRITE_MODGARAGE:        return TheText.Get("LG_24");
    case RADAR_SPRITE_OGLOC:            return TheText.Get("LG_25");
    case RADAR_SPRITE_PIZZA:            return TheText.Get("LG_26");
    case RADAR_SPRITE_POLICE:           return TheText.Get("LG_27");
    case RADAR_SPRITE_PROPERTYG:        return TheText.Get("LG_28");
    case RADAR_SPRITE_PROPERTYR:        return TheText.Get("LG_29");
    case RADAR_SPRITE_RACE:             return TheText.Get("LG_30");
    case RADAR_SPRITE_RYDER:            return TheText.Get("LG_31");
    case RADAR_SPRITE_SAVEGAME:         return TheText.Get("LG_32");
    case RADAR_SPRITE_SCHOOL:           return TheText.Get("LG_33");
    case RADAR_SPRITE_QMARK:            return TheText.Get("LG_63");
    case RADAR_SPRITE_SWEET:            return TheText.Get("LG_35");
    case RADAR_SPRITE_TATTOO:           return TheText.Get("LG_36");
    case RADAR_SPRITE_THETRUTH:         return TheText.Get("LG_37");
    case RADAR_SPRITE_WAYPOINT:         return TheText.Get("LG_64");
    case RADAR_SPRITE_TORENORANCH:      return TheText.Get("LG_39");
    case RADAR_SPRITE_TRIADS:           return TheText.Get("LG_40");
    case RADAR_SPRITE_TRIADSCASINO:     return TheText.Get("LG_41");
    case RADAR_SPRITE_TSHIRT:           return TheText.Get("LG_42");
    case RADAR_SPRITE_WOOZIE:           return TheText.Get("LG_43");
    case RADAR_SPRITE_ZERO:             return TheText.Get("LG_44");
    case RADAR_SPRITE_DATEDISCO:        return TheText.Get("LG_45");
    case RADAR_SPRITE_DATEDRINK:        return TheText.Get("LG_46");
    case RADAR_SPRITE_DATEFOOD:         return TheText.Get("LG_47");
    case RADAR_SPRITE_TRUCK:            return TheText.Get("LG_48");
    case RADAR_SPRITE_CASH:             return TheText.Get("LG_51");
    case RADAR_SPRITE_FLAG:             return TheText.Get("LG_52");
    case RADAR_SPRITE_GYM:              return TheText.Get("LG_53");
    case RADAR_SPRITE_IMPOUND:          return TheText.Get("LG_57");
    case RADAR_SPRITE_LIGHT:            break;
    case RADAR_SPRITE_RUNWAY:           return TheText.Get("LG_65");
    case RADAR_SPRITE_GANGB:            return TheText.Get("LG_58");
    case RADAR_SPRITE_GANGP:            return TheText.Get("LG_59");
    case RADAR_SPRITE_GANGY:            return TheText.Get("LG_60");
    case RADAR_SPRITE_GANGN:            return TheText.Get("LG_61");
    case RADAR_SPRITE_GANGG:            return TheText.Get("LG_62");
    case RADAR_SPRITE_SPRAY:            return TheText.Get("LG_34");
    case RADAR_SPRITE_TORENO:           break;
    default:
        break;
    }
    NOTSA_UNREACHABLE("Bug in GetBlipName");
    return nullptr;
}

/*!
 * @notsa
 * @brief Returns the first index in `ms_RadarTrace` that is not a tracking blip.
 */
int32 CRadar::FindTraceNotTrackingBlipIndex() {
    for (auto&& [i, v] : notsa::enumerate(ms_RadarTrace)) {
        if (!v.m_bTrackingBlip) {
            return (int32)i;
        }
    }
    return -1;
}


/*!
 * @notsa
 * @brief Returns the color of the blip's apperance (Opaque)
 */
CRGBA tRadarTrace::GetStaticColour() const {
    switch (m_nAppearance) {
    case eBlipAppearance::BLIP_FLAG_FRIEND:
        return CRadar::GetRadarTraceColour(m_nColour, m_bBright, false);

    case eBlipAppearance::BLIP_FLAG_THREAT:
        return HudColour.GetIntColour(HUD_COLOUR_BLUE);

    case eBlipAppearance::BLIP_FLAG_UNK:
        return HudColour.GetIntColour(HUD_COLOUR_RED);

    default:
        NOTSA_UNREACHABLE("Invalid blip appearance (={})", (uint8)m_nAppearance);
    }

    return 0;
}

/*!
 * @notsa
 * @brief Returns the position of the blip always relative to the world.
 */
CVector tRadarTrace::GetWorldPos() const {
    if (m_pEntryExit) {
        CVector pos{};
        m_pEntryExit->GetPositionRelativeToOutsideWorld(pos);
        return pos;
    } else {
        return m_vPosition;
    }
}

/*!
 * @notsa
 * @brief Returns radar and screen relative positions of the trace.
 * @param radarPointDist Distance to the point in radar. (Optional)
 */
std::pair<CVector2D, CVector2D> tRadarTrace::GetRadarAndScreenPos(float* radarPointDist) const {
    const auto world = GetWorldPos();

    CVector2D radar;
    CRadar::TransformRealWorldPointToRadarSpace(radar, world);

    const auto dist = CRadar::LimitRadarPoint(radar); // Normalises vector `radar`
    if (radarPointDist) {
        *radarPointDist = dist;
    }

    CVector2D screen;
    CRadar::TransformRadarPointToScreenSpace(screen, radar);

    return std::make_pair(radar, screen);
}
