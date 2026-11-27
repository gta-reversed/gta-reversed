#pragma once

#include "ShadowCamera.h"

class CRealTimeShadow;

class CRealTimeShadowManager {
public:
    static constexpr int32 NUM_REALTIME_SHADOWS = 16;

public:
    char             m_bInitialised{};
    char             m_bNeedsReinit{}; // AKA `m_bNotReInitialized`
    CRealTimeShadow* m_apShadows[NUM_REALTIME_SHADOWS]{};
    CShadowCamera    m_BlurCamera{};
    CShadowCamera    m_GradientCamera{};

public:
    static void InjectHooks();

    void Init();
    void Exit();
    void ReInit();
    void Update();

    /*!
    * @addr 0x706970
    * @brief Set up a shadow for `physical`. Despite the name, this is what creates the shadow (player 1 always gets the first one)
    * @return The shadow, or `nullptr` if none is free, or if the player is in a fast vehicle and `physical` isn't player 1
    */
    CRealTimeShadow* GetRealTimeShadow(CPhysical* physical);
    void             DoShadowThisFrame(CPhysical* physical);
    void             ReturnRealTimeShadow(CRealTimeShadow* pShadow);
};
VALIDATE_SIZE(CRealTimeShadowManager, 0x54);

extern CRealTimeShadowManager& g_realTimeShadowMan;
