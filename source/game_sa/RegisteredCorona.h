/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"

#include "Vector.h"
#include "RGBA.h"

class CEntity;

enum eCoronaType {
    CORONATYPE_SHINYSTAR     = 0,  // 0x0
    CORONATYPE_HEADLIGHT     = 1,  // 0x1
    CORONATYPE_MOON          = 2,  // 0x2
    CORONATYPE_REFLECTION    = 3,  // 0x3
    CORONATYPE_HEADLIGHTLINE = 4,  // 0x4
    CORONATYPE_HEX           = 5,  // 0x5
    CORONATYPE_CIRCLE        = 6,  // 0x6
    CORONATYPE_RING          = 7,  // 0x7
    CORONATYPE_STREAK        = 8,  // 0x8
    CORONATYPE_TORUS         = 9,  // 0x9
        
    CORONATYPE_COUNT         = 10, // 0xA
};

enum eCoronaFlareType : uint8 { FLARETYPE_NONE, FLARETYPE_SUN, FLARETYPE_HEADLIGHTS };

class CRegisteredCorona {
public:
    CVector          m_vPosn{};
    uint32           m_dwId{};     // Should be unique for each corona. Address or something
    RwTexture*       m_pTexture{}; // Pointer to the actual texture to be rendered
    float            m_fSize{};
    float            m_fAngle{};    // left from III&VC
    float            m_fFarClip{};  // How far away this corona stays visible
    float            m_fNearClip{}; // How far away is the z value pulled towards camera.
    float            m_fHeightAboveGround{};
    float            m_fFadeSpeed{}; // The speed the corona fades in and out
    CRGBA            m_Color{};
    uint8            m_FadedIntensity{};           // Intensity that lags behind the given intenisty and fades out if the LOS is blocked
    uint8            m_bRegisteredThisFrame{}; // Has this corona been registered by game code this frame
    eCoronaFlareType m_nFlareType{};
    uint8            m_bUsesReflection{};
    uint8            m_bCheckObstacles : 1{};    // Do we check the LOS or do we render at the right Z value -> bLOSCheck
    uint8            m_bOffScreen : 1{};         // Set by the rendering code to be used by the update code
    uint8            m_bJustCreated{};           // If this corona has been created this frame we won't delete it (It hasn't had the time to get its OffScreen cleared)
    uint8            m_bFlashWhileFading : 1{};  // Does the corona fade out when closer to cam
    uint8            m_bOnlyFromBelow : 1{};     // This corona is only visible if the camera is below it
    uint8            m_bHasValidHeightAboveGround : 1{};   // this corona Has Valid Height Above Ground
    uint8            m_bDrawWithWhiteCore : 1{}; // This corona rendered with a small white core.
    uint8            m_bAttached : 1{};          // This corona is attached to an entity.
    CEntity*         m_pAttachedTo{};

public:
    CRegisteredCorona() = default;
    static void InjectHooks();

    bool IsActive() const { return m_dwId != 0; }
    void SetInactive() { m_dwId = 0; }
    void Update();
    auto GetPosition() const -> CVector;
    auto CalculateIntensity(float scrZ, float farClip) const -> float;
};

VALIDATE_SIZE(CRegisteredCorona, 0x3C);
