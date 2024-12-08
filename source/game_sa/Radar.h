/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"
#include "RGBA.h"
#include "Vector2D.h"

class CSprite2d;

// Thanks to Wesser for radar-related things
enum eBlipAppearance : uint8 {
    BLIP_FLAG_FRIEND, // It selects BLIP_COLOUR_BLUE. If unset together with BLIP_FLAG_THREAT, any color.
    BLIP_FLAG_THREAT,  // It selects BLIP_COLOUR_RED. If unset together with BLIP_FLAG_FRIEND, any color.
    BLIP_FLAG_UNK,

    BLIP_FLAG_NUM // Add above this
};

enum eBlipType : uint8 {
    BLIP_NONE,          // 0
    BLIP_CAR,           // 1
    BLIP_CHAR,          // 2
    BLIP_OBJECT,        // 3
    BLIP_COORD,         // 4 - Checkpoint.
    BLIP_CONTACT_POINT, // 5 - Sphere.
    BLIP_SPOTLIGHT,     // 6
    BLIP_PICKUP,        // 7
    BLIP_AIRSTRIP       // 8
};

enum eBlipDisplay : uint8 {
    BLIP_DISPLAY_NEITHER,    // 0
    BLIP_DISPLAY_MARKERONLY, // 1
    BLIP_DISPLAY_BLIPONLY,   // 2
    BLIP_DISPLAY_BOTH        // 3
};

// See <https://www.dropbox.com/s/oi3i4f0qsbe7z10/blip_marker_colors.html> to view these colors. (TODO: dead link, fix it.)
enum eBlipColour : uint32 {
    BLIP_COLOUR_RED,        // 0
    BLIP_COLOUR_GREEN,      // 1
    BLIP_COLOUR_BLUE,       // 2
    BLIP_COLOUR_WHITE,      // 3
    BLIP_COLOUR_YELLOW,     // 4
    BLIP_COLOUR_REDCOPY,    // 5 - What? It was BLIP_COLOUR_PURPLE.
    BLIP_COLOUR_BLUECOPY,   // 6 - Why? It was BLIP_COLOUR_CYAN.
    BLIP_COLOUR_THREAT,     // 7 - If BLIP_FLAG_FRIENDLY is not set (by default) it is BLIP_COLOUR_RED, else BLIP_COLOUR_BLUE.
    BLIP_COLOUR_DESTINATION // 8 - Default color.
};

// https://wiki.multitheftauto.com/index.php?title=Radar_Blips
enum eRadarSprite : int8 {
    RADAR_SPRITE_PLAYER_INTEREST = -5,
    RADAR_SPRITE_THREAT          = -4,
    RADAR_SPRITE_FRIEND          = -3,
    RADAR_SPRITE_OBJECT          = -2,
    RADAR_SPRITE_DESTINATION     = -1,
    RADAR_SPRITE_NONE            = 0,
    RADAR_SPRITE_WHITE,         // 1
    RADAR_SPRITE_CENTRE,        // 2
    RADAR_SPRITE_MAP_HERE,      // 3
    RADAR_SPRITE_NORTH,         // 4
    RADAR_SPRITE_AIRYARD,       // 5
    RADAR_SPRITE_AMMUGUN,       // 6
    RADAR_SPRITE_BARBERS,       // 7
    RADAR_SPRITE_BIGSMOKE,      // 8
    RADAR_SPRITE_BOATYARD,      // 9
    RADAR_SPRITE_BURGERSHOT,    // 10
    RADAR_SPRITE_BULLDOZER,     // 11
    RADAR_SPRITE_CATALINAPINK,  // 12
    RADAR_SPRITE_CESARVIAPANDO, // 13 - What? R* mistype?
    RADAR_SPRITE_CHICKEN,       // 14
    RADAR_SPRITE_CJ,            // 15
    RADAR_SPRITE_CRASH1,        // 16
    RADAR_SPRITE_DINER,         // 17
    RADAR_SPRITE_EMMETGUN,      // 18
    RADAR_SPRITE_ENEMYATTACK,   // 19
    RADAR_SPRITE_FIRE,          // 20
    RADAR_SPRITE_GIRLFRIEND,    // 21
    RADAR_SPRITE_HOSTPITAL,     // 22 - Again?
    RADAR_SPRITE_LOGOSYNDICATE, // 23
    RADAR_SPRITE_MADDOG,        // 24
    RADAR_SPRITE_MAFIACASINO,   // 25
    RADAR_SPRITE_MCSTRAP,       // 26
    RADAR_SPRITE_MODGARAGE,     // 27
    RADAR_SPRITE_OGLOC,         // 28
    RADAR_SPRITE_PIZZA,         // 29
    RADAR_SPRITE_POLICE,        // 30
    RADAR_SPRITE_PROPERTYG,     // 31
    RADAR_SPRITE_PROPERTYR,     // 32
    RADAR_SPRITE_RACE,          // 33
    RADAR_SPRITE_RYDER,         // 34
    RADAR_SPRITE_SAVEGAME,      // 35
    RADAR_SPRITE_SCHOOL,        // 36
    RADAR_SPRITE_QMARK,         // 37
    RADAR_SPRITE_SWEET,         // 38
    RADAR_SPRITE_TATTOO,        // 39
    RADAR_SPRITE_THETRUTH,      // 40
    RADAR_SPRITE_WAYPOINT,      // 41
    RADAR_SPRITE_TORENORANCH,   // 42
    RADAR_SPRITE_TRIADS,        // 43
    RADAR_SPRITE_TRIADSCASINO,  // 44
    RADAR_SPRITE_TSHIRT,        // 45
    RADAR_SPRITE_WOOZIE,        // 46
    RADAR_SPRITE_ZERO,          // 47
    RADAR_SPRITE_DATEDISCO,     // 48
    RADAR_SPRITE_DATEDRINK,     // 49
    RADAR_SPRITE_DATEFOOD,      // 50
    RADAR_SPRITE_TRUCK,         // 51
    RADAR_SPRITE_CASH,          // 52
    RADAR_SPRITE_FLAG,          // 53
    RADAR_SPRITE_GYM,           // 54
    RADAR_SPRITE_IMPOUND,       // 55
    RADAR_SPRITE_LIGHT,         // 56
    RADAR_SPRITE_RUNWAY,        // 57
    RADAR_SPRITE_GANGB,         // 58
    RADAR_SPRITE_GANGP,         // 59
    RADAR_SPRITE_GANGY,         // 60
    RADAR_SPRITE_GANGN,         // 61
    RADAR_SPRITE_GANGG,         // 62
    RADAR_SPRITE_SPRAY,         // 63
    RADAR_SPRITE_TORENO         // 64
};

enum eRadarTraceHeight : uint8 {
    RADAR_TRACE_LOW,   // 0 Up-pointing Triangle △
    RADAR_TRACE_HIGH,  // 1 Down-pointing Triangle ▽
    RADAR_TRACE_NORMAL // 2 Box □
};

/*
struct tBlipHandle {
    uint16 arrayIndex;
    uint16 number;
};
VALIDATE_SIZE(tBlipHandle, 0x4);
*/
using tBlipHandle = uint32; // TODO: Use struct above

struct airstrip_info {
    CVector2D position;
    float     direction; // angle
    float     radius; // not sure
};
VALIDATE_SIZE(airstrip_info, 0x10);

// NOTSA
enum eAirstripLocation : uint8 {
    AIRSTRIP_LS_AIRPORT,
    AIRSTRIP_SF_AIRPORT,
    AIRSTRIP_LV_AIRPORT,
    AIRSTRIP_VERDANT_MEADOWS,

    NUM_AIRSTRIPS
};

class CEntryExit;

struct tRadarTrace {
    eBlipColour  m_nColour;
    uint32       m_nEntityHandle;
    CVector      m_vPosition;
    uint16       m_nCounter;
    float        m_fSphereRadius;
    uint16       m_nBlipSize;
    CEntryExit*  m_pEntryExit;
    eRadarSprite m_nBlipSprite;

    bool         m_bBright : 1;              // It makes use of bright colors. Always set.
    bool         m_bTrackingBlip : 1;        // It is available.
    bool         m_bShortRange : 1;          // It doesn't show permanently on the radar.
    bool         m_bFriendly : 1;            // It is affected by BLIP_COLOUR_THREAT.
    bool         m_bBlipRemain : 1;          // It has the priority over the entity (it will still appear after the entity's deletion).
    bool         m_bBlipFade : 1;            // Possibly a leftover. Always unset (unused).
    uint8        m_nCoordBlipAppearance : 2; // see eBlipAppearance

    eBlipDisplay    m_nBlipDisplayFlag : 2;
    eBlipType       m_nBlipType : 4;
    eBlipAppearance m_nAppearance : 2;

    [[nodiscard]] auto HasSprite() const { return m_nBlipSprite != eRadarSprite::RADAR_SPRITE_NONE; }
    [[nodiscard]] CRGBA GetStaticColour() const;
    [[nodiscard]] CVector GetWorldPos() const;

    std::pair<CVector2D, CVector2D> GetRadarAndScreenPos(float* radarPointDist) const;
};
VALIDATE_SIZE(tRadarTrace, 0x28);

static constexpr uint32 MAX_RADAR_WIDTH_TILES = 12;
static constexpr uint32 MAX_RADAR_HEIGHT_TILES = 12;

class CRadar {
public:
    static constexpr uint32 MAX_RADAR_SPRITES = 64;
    static constexpr uint32 MAX_RADAR_TRACES = 175;

    static inline float& m_fRadarOrientation = *(float*)0xBA8310;
    static inline float& cachedCos = *(float*)0xBA8308;
    static inline float& cachedSin = *(float*)0xBA830C;

    static SpriteFileName RadarBlipFileNames[];

    static inline float& m_radarRange = *(float*)0xBA8314; // 2990.0 by default
    static inline std::array<int16, MAX_RADAR_TRACES>& MapLegendList = *(std::array<int16, MAX_RADAR_TRACES>*)0xBA8318; // do not convert int16 to eRadarSprite, they have different sizes.
    static inline uint16& MapLegendCounter = *(uint16*)0xBA86B8; // num icons in legend
    static inline std::array<CRGBA, 6>& ArrowBlipColour = *(std::array<CRGBA, 6>*)0xBA86D4;
    static inline std::array<tRadarTrace, MAX_RADAR_TRACES>& ms_RadarTrace = *(std::array<tRadarTrace, MAX_RADAR_TRACES>*)0xBA86F0;
    static inline CVector2D& vec2DRadarOrigin = *(CVector2D*)0xBAA248;
    static inline std::array<CSprite2d, MAX_RADAR_SPRITES>& RadarBlipSprites = *(std::array<CSprite2d, MAX_RADAR_SPRITES>*)0xBAA250;
    static inline CRect& m_radarRect = *(CRect*)0x8D0920; // { 1000000.0f, -1000000.0f, -1000000.0f, 1000000.0f }

    static inline eAirstripLocation& airstrip_location = *(eAirstripLocation*)0xBA8300; // current airstrip index in airstrip_table
    static inline int32& airstrip_blip = *(int32*)0xBA8304;     // blip handle

public:
    static void InjectHooks();

    static void Initialise();
    static void Shutdown();

    static void LoadTextures();
    
    static tBlipHandle GetNewUniqueBlipIndex(int32 blipIndex);
    static int32 GetActualBlipArrayIndex(tBlipHandle blip);

    static void DrawLegend(int32 x, int32 y, eRadarSprite blipType);
    static float LimitRadarPoint(CVector2D& point);
    static void LimitToMap(float& x, float& y);
    static uint8 CalculateBlipAlpha(float distance);
    static void TransformRadarPointToScreenSpace(CVector2D& out, const CVector2D& in);
    static void TransformRealWorldPointToRadarSpace(CVector2D& out, const CVector2D& in);
    static void TransformRadarPointToRealWorldSpace(CVector2D& out, const CVector2D& in);
    static void TransformRealWorldToTexCoordSpace(CVector2D& out, const CVector2D& in, int32 x, int32 y);
    static void CalculateCachedSinCos();
    static tBlipHandle SetCoordBlip(eBlipType type, CVector posn, eBlipColour color, eBlipDisplay blipDisplay, const char* scriptName = nullptr);
    static tBlipHandle SetShortRangeCoordBlip(eBlipType type, CVector posn, eBlipColour color, eBlipDisplay blipDisplay, const char* scriptName = nullptr);
    static tBlipHandle SetEntityBlip(eBlipType type, int32 entityHandle, uint32 arg2, eBlipDisplay blipDisplay);
    static void ChangeBlipColour(tBlipHandle blip, eBlipColour color);
    static bool HasThisBlipBeenRevealed(int32 blipIndex);
    static bool DisplayThisBlip(eRadarSprite spriteId, int8 priority);
    static void ChangeBlipBrightness(tBlipHandle blip, int32 brightness);
    static void ChangeBlipScale(tBlipHandle blip, int32 size);
    static void ChangeBlipDisplay(tBlipHandle blip, eBlipDisplay blipDisplay);
    static void SetBlipSprite(tBlipHandle blip, eRadarSprite spriteId);
    static void SetBlipAlwaysDisplayInZoom(tBlipHandle blip, bool display);
    static void SetBlipFade(tBlipHandle blip, bool fade);
    static void SetCoordBlipAppearance(tBlipHandle blip, eBlipAppearance appearance);
    static void SetBlipFriendly(tBlipHandle blip, bool friendly);
    static void SetBlipEntryExit(tBlipHandle blip, CEntryExit* enex);
    static void ShowRadarTrace(float x, float y, uint32 size, CRGBA color);
    static void ShowRadarTraceWithHeight(float x, float y, uint32 size, uint32 r, uint32 g, uint32 b, uint32 a, eRadarTraceHeight height);
    static void ShowRadarMarker(CVector posn, uint32 color, float radius);
    static uint32 GetRadarTraceColour(eBlipColour color, bool bright, bool friendly);
    static void DrawRotatingRadarSprite(CSprite2d& sprite, float x, float y, float angle, uint32 width, uint32 height, CRGBA color);
    static void DrawYouAreHereSprite(float x, float y);
    static void SetupRadarRect(int32 x, int32 y);
    static void RequestMapSection(int32 x, int32 y);
    static void RemoveMapSection(int32 x, int32 y);
    static void RemoveRadarSections();
    static void StreamRadarSections(const CVector& worldPosn);
    static void StreamRadarSections(int32 x, int32 y);
    static int32 ClipRadarPoly(CVector2D* out, const CVector2D* in);
    static void DrawAreaOnRadar(const CRect& rect, const CRGBA& color, bool inMenu);
    static void DrawRadarMask();
    static void InitFrontEndMap();
    static void AddBlipToLegendList(bool noSprite, int32 blipIndex);
    static void SetMapCentreToPlayerCoords();
    static void Draw3dMarkers();
    static void SetRadarMarkerState(int32 counter, bool flag);
    static void DrawRadarSprite(eRadarSprite spriteId, float x, float y, uint8 alpha);
    static void DrawRadarSection(int32 x, int32 y);
    static void DrawRadarSectionMap(int32 x, int32 y, CRect rect);
    static void DrawRadarGangOverlay(bool inMenu);
    static void DrawRadarMap();
    static void DrawMap();

    static void DrawCoordBlip(int32 blipIndex, bool isSprite);
    static void DrawEntityBlip(int32 blipIndex, uint8 arg1);
    static void ClearActualBlip(int32 blipIndex);
    static void ClearActualBlip(tRadarTrace& trace);
    static void ClearBlipForEntity(eBlipType blipType, int32 entityHandle);
    static void ClearBlipForEntity(CPed* ped);
    static void ClearBlip(tBlipHandle blip);
    static void SetupAirstripBlips();
    static void DrawBlips();

    static bool Load();
    static bool Save();

    // NOTSA
    static const GxtChar* GetBlipName(eRadarSprite sprite);
    static int32 FindTraceNotTrackingBlipIndex(); // Return the index of the first trace with the `TrackingBlip` flag NOT set

    static bool IsMapSectionInBounds(int32 x, int32 y) {
        return x >= 0 && x < MAX_RADAR_WIDTH_TILES && y >= 0 && y < MAX_RADAR_HEIGHT_TILES;
    }

    static auto CachedRotateCounterclockwise(const CVector2D& point) {
        return CVector2D{
            cachedCos * point.x - cachedSin * point.y,
            cachedSin * point.x + cachedCos * point.y
        };
    }

    static auto CachedRotateClockwise(const CVector2D& point) {
        return CVector2D{
            +cachedCos * point.x + cachedSin * point.y,
            -cachedSin * point.x + cachedCos * point.y
        };
    }
};

bool ClipRadarTileCoords(int32& x, int32& y);
bool IsPointInsideRadar(const CVector2D& point);
void GetTextureCorners(int32 x, int32 y, CVector2D* corners);
int32 LineRadarBoxCollision(CVector2D& result, const CVector2D& lineStart, const CVector2D& lineEnd);
