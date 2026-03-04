/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "HudColours.h"
#include "3dMarker.h"
#include "RGBA.h"
#include "Vector.h"

struct RpClump;
struct RpAtomic;

struct tUser3dMarkers {
    bool    IsUsed;
    CVector Position;
    uint32  Red, Green, Blue;

    auto GetColor() const { return CRGBA((uint8)Red, (uint8)Green, (uint8)Blue, 255u); }

    auto IsInUse() const { return IsUsed; }
    void Render(RpClump* clump) const;
};

VALIDATE_SIZE(tUser3dMarkers, 0x1C);

struct tDirectionArrows {
    bool    IsUsed;
    CVector Position;
    float   Size;
    CVector Normal;       // Normal - That is, the direction it points to
    uint32  Red, Green, Blue, Alpha;

    auto GetColor() const { return CRGBA((uint8)Red, (uint8)Green, (uint8)Blue, (uint8)Alpha); }

    auto IsInUse() const { return IsUsed; }
    void Render(RpClump* clump);
};

VALIDATE_SIZE(tDirectionArrows, 0x30);

class C3dMarkers {
public:
    static inline auto& IgnoreRenderLimit = StaticRef<bool, 0xC7C704>();

    static inline auto& ms_user3dMarkers = StaticRef<std::array<tUser3dMarkers, MARKER3D_COUNT>, 0xC80258>();

    static inline auto& m_user3dMarkerColor = StaticRef<CRGBA, 0xC7C620>();

    static inline auto& ms_directionArrows = StaticRef<std::array<tDirectionArrows, 5>, 0xC802E8>();

    static inline auto& m_pRpClumpArray = StaticRef<std::array<RpClump*, MARKER3D_COUNT>, 0xC7C6DC>();
    static inline auto& m_aMarkerArray = StaticRef<std::array<C3dMarker, 32>, 0xC7DD58>();
    static inline auto& NumActiveMarkers = StaticRef<uint32, 0xC7C6D8>();

    static inline auto& m_colDiamond = StaticRef<uint8, 0x8D5D8B>(); // default 255
    // static inline auto& m_colDiamondMat; // unknown, unused
    static inline auto& m_angleDiamondDeg = StaticRef<float, 0xC7C700>();

public:
    static void InjectHooks();

    //
    // Generic functions
    //

    static void Init();
    static void Shutdown();

    //! Main render function
    static void Render();

    //! Periodic update stuff
    static void Update();

    static void ForceRender(bool bEnable);
    static RpClump* LoadMarker(const char* modelName);

    //
    // Directional Arrows
    //

    //! @return slot index; -1 if not found
    static int32 DirectionArrowFindFirstFreeSlot();
    static void  DirectionArrowSet(CVector posn, float size, int32 red, int32 green, int32 blue, int32 alpha, float dir_x, float dir_y, float dir_z);
    static void  DirectionArrowsInit();
    static void  DirectionArrowsDraw();

    //
    // Regular 3d markers
    //

    //! This shit does modify `posn`, sadly we can't make it into a constant....
    static C3dMarker* PlaceMarker(uint32 id, e3dMarkerType type, CVector& posn, float size, uint8 red, uint8 green, uint8 blue, uint8 alpha, uint16 pulsePeriod, float pulseFraction, int16 rotateRate, CVector dir, bool zCheck);

    //! This shit does modify `posn`, sadly we can't make it into a constant....
    static void PlaceMarkerCone(uint32 id, CVector& point, float size, uint8 red, uint8 green, uint8 blue, uint8 alpha, uint16 pulsePeriod, float pulseFraction, int16 rotateRate, bool bEnableCollision);

    //! This shit does modify `posn`, sadly we can't make it into a constant....
    static void PlaceMarkerSet(uint32 id, e3dMarkerType type, CVector& posn, float size, uint8 red, uint8 green, uint8 blue, uint8 alpha, uint16 pulsePeriod, float pulseFraction, int16 rotateRate);

    static void Render3dMarkers();

private:
    static C3dMarker* FindById(uint32 id);
    static C3dMarker* FindFree();

public:

    //
    // User markers
    //

    // WorkBuffer stuff (savefile)
    static bool LoadUser3dMarkers();
    static bool SaveUser3dMarkers();

    //! only set material color (m_user3dMarkerColor) for first material in first atomic
    static RpAtomic* User3dMarkerAtomicCB(RpAtomic* atomic, void*);

    static void User3dMarkerDelete(int32 slotIndex);
    static void User3dMarkerDeleteAll();

    //! @return slot index; -1 if none
    static int32 User3dMarkerFindFirstFreeSlot();

    //! @return slot index; -1 if not created
    static int32 User3dMarkerSet(float x, float y, float z, eHudColours colour);

    //! Render the markers
    static void User3dMarkersDraw();
};
