/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <rwplcore.h>

#include "Vector.h"
#include "eBirdsBiome.h"

/*
    researched by LINK/2012
    for 'The Birds Update' mod
*/

class CBirds;

struct CBirdColor {
public:
    RwUInt8 cRed;
    RwUInt8 cGreen;
    RwUInt8 cBlue;

    inline void Set(RwUInt8 cColor) {
        cRed = cColor;
        cGreen = cColor;
        cBlue = cColor;
    }

    inline void Set(RwUInt8 cR, RwUInt8 cG, RwUInt8 cB) {
        cRed = cR;
        cGreen = cG;
        cBlue = cB;
    }

    inline void Scale(RwReal fScale) {
        cRed = static_cast<RwUInt8>(static_cast<RwReal>(cRed) * fScale);
        cGreen = static_cast<RwUInt8>(static_cast<RwReal>(cGreen) * fScale);
        cBlue = static_cast<RwUInt8>(static_cast<RwReal>(cBlue) * fScale);
    }
};
VALIDATE_SIZE(CBirdColor, 0x3);

enum class eBirdMode : uint8 {
    BIRD_DRAW_UPDATE   = 0x1, // Bird is drawn, but doesn't update position
    BIRD_DRAW_NOUPDATE = 0x2, // Bird is drawn and updates position
};

class CBird {
public:
    CVector         m_vecPosn;            // Bird position
    CVector         m_vecCurrentVelocity; // Velocity in the current frame
    CVector         m_vecTargetVelocity;  // Velocity (constant if the bird is not flying in circles)
    float           m_fAngle;             // Bird angle
    uint32          m_nUpdateAfterMS;     // Always 0, if a value is written here, bird position isn't updated till CTimer::GetTimeInMS() is bigger than this value
    uint32          m_nWingStillness;     // How still are the wings
    float           m_fSize;              // Bird size
    float           m_fMaxBirdDistance;   // When the bird is in this distance from the camera, it will disappear.
                                          // Used also in rendering process to determine the alpha level of the bird.
    eBirdMode       m_eBirdMode;          // Always 1, if set to 2 then bird is drawn but doesn't update it's position, if it's set to anything else then bird isn't updated nor drawn
    CBirdColor      m_BodyColor;          // Body color
    CBirdColor      m_WingsColor;         // Wing tips color
    bool            m_bCreated;           // This flags indicates if in this index there's a bird created
    bool            m_bMustDoCurves;      // If this flag is true the bird will fly in circles
};
VALIDATE_SIZE(CBird, 0x44);

class CBirds {
public:
    static inline auto& bHasBirdBeenShot = StaticRef<bool>(0xC6A8A0);
    static inline auto& uiNumberOfBirds = StaticRef<uint32>(0xC6A8A4);
    static inline auto& aBirds = StaticRef<std::array<CBird, 6>>(0xC6A8B0);
    static inline auto& vecBirdShotAt = StaticRef<CVector>(0xC6AA48);

    static std::array<float, 6>  faCreationCoorsX;
    static std::array<float, 6>  faCreationCoorsY;
    static std::array<float, 6>  faCreationCoorsZ;

    static std::array<float, 8>  faRenderCoorsU;
    static std::array<float, 8>  faRenderCoorsV;
    static std::array<float, 8>  faRenderPosY;

    static std::array<uint32, 30> auRenderIndices;

public:
    static void Init();
    static void CreateNumberOfBirds(CVector vecStartPos, CVector vecTargetPos, int32 iBirdCount, eBirdsBiome biome, bool bCheckObstacles);
    static void Shutdown();
    static void Update();
    static void Render();
    static void HandleGunShot(const CVector* pointA, const CVector* pointB);

    static void InjectHooks();
};

