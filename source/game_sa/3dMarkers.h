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

struct tUser3dMarker {
    bool    m_bIsUsed;
    CVector m_vecPosition;
    uint32  m_nRed;
    uint32  m_nGreen;
    uint32  m_nBlue;

    auto GetColor() const { return CRGBA((uint8)m_nRed, (uint8)m_nGreen, (uint8)m_nBlue, 255u); }

    auto IsInUse() const { return m_bIsUsed; }
    void Render(RpClump* clump) const;
};

VALIDATE_SIZE(tUser3dMarker, 0x1C);

struct tDirectionArrow {
    bool    m_bIsUsed;
    CVector m_vecPosition;
    float   m_fSize;
    CVector m_normal;       // Normal - That is, the direction it points to
    uint32  m_nRed;
    uint32  m_nGreen;
    uint32  m_nBlue;
    uint32  m_nAlpha;

    auto GetColor() const { return CRGBA((uint8)m_nRed, (uint8)m_nGreen, (uint8)m_nBlue, (uint8)m_nAlpha); }

    auto IsInUse() const { return m_bIsUsed; }
    void Render(RpClump* clump);
};

VALIDATE_SIZE(tDirectionArrow, 0x30);

class C3dMarkers {
public:
    static inline uint8& m_colDiamond = *(uint8*)0x8D5D8B; // default 255
    static inline CRGBA& m_user3dMarkerColor = *(CRGBA*)0xC7C620;
    static inline bool& IgnoreRenderLimit = *(bool*)0xC7C704;
    static inline float& m_angleDiamondDeg = *(float*)0xC7C700;
    static inline uint32& NumActiveMarkers = *(uint32*)0xC7C6D8;
    static inline std::array<RpClump*, (size_t)(MARKER3D_COUNT)>& m_pRpClumpArray = *(std::array<RpClump*, 7>*)0xC7C6DC;
    static inline std::array<tDirectionArrow, 5>& ms_directionArrows = *(std::array<tDirectionArrow, 5>*)0xC802E8;
    static inline std::array<tUser3dMarker, 5>& ms_user3dMarkers = *(std::array<tUser3dMarker, 5>*)0xC80258;
    static inline std::array<C3dMarker, 32>& m_aMarkerArray = *(std::array<C3dMarker, 32>*)0xC7DD58;

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
