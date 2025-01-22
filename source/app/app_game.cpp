#include "StdInc.h"

#include <tracy/Tracy.hpp>

#include "app_game.h"
#include "LoadingScreen.h"
#include "PlantMgr.h"
#include "Shadows.h"
#include "Clouds.h"
#include "MovingThings.h"
#include "BreakManager_c.h"
#include "RealTimeShadowManager.h"
#include "Credits.h"
#include "Securom.h"
#include "UserDisplay.h"
#include "Garages.h"
#include "UIRenderer.h"
#include "Gamma.h"
#include "Birds.h"
#include "Skidmarks.h"
#include "Ropes.h"
#include "Glass.h"
#include "WaterCannons.h"
#include "VehicleRecording.h"
#include "PostEffects.h"
#include "CarFXRenderer.h"

#include "extensions/Configs/FastLoader.hpp"

void AppGameInjectHooks() {
    RH_ScopedCategory("App");
    RH_ScopedNamespaceName("Game");

    RH_ScopedGlobalInstall(GameInit, 0x5BF3B0);
    RH_ScopedGlobalInstall(InitialiseGame, 0x53E580);
    RH_ScopedGlobalInstall(TheGame, 0x53DF10);

    RH_ScopedGlobalInstall(RwInitialize, 0x5BF390);
    RH_ScopedGlobalInstall(RwTerminate, 0x53D910);

    RH_ScopedGlobalInstall(RenderEffects, 0x53E170);
    RH_ScopedGlobalInstall(RenderScene, 0x53DF40);
    RH_ScopedGlobalInstall(RenderMenus, 0x53E530);
    RH_ScopedGlobalInstall(Render2dStuff, 0x53E230, { .locked = true }); // Must be hooked at all times otherwise game locks!
    RH_ScopedGlobalInstall(RenderDebugShit, 0x53E160);

    RH_ScopedGlobalInstall(Idle, 0x53E920, { .locked = true }); // Must be hooked at all times otherwise game locks!
    RH_ScopedGlobalInstall(FrontendIdle, 0x53E770, { .locked = true }); // Must be hooked at all times otherwise imgui stops working!
}

// 0x5BF3B0
void GameInit() {
    VERIFY(RwInitialize(nullptr));
    srand(RsTimer());
}

// 0x53E580
void InitialiseGame() {
    ZoneScoped;

    static int16& version_number = *(int16*)(0xB72C68);
    version_number = 78;

    CGame::Initialise(GAME_LEVEL_FILE);
    AudioEngine.Restart();
    if (FrontEndMenuManager.m_bLoadingData) {
        FrontEndMenuManager.m_bStartGameLoading = true;
    }
}

// unused
// 0x53DF10
void TheGame() {
    printf("Into TheGame!!!\n");
    CTimer::Initialise();
    CGame::Initialise(GAME_LEVEL_FILE);
    CLoadingScreen::SetLoadingBarMsg("Starting Game", nullptr);
    CLoadingScreen::NewChunkLoaded();
}

// 0x5BF390
bool RwInitialize(void* param) {
    return RsRwInitialize(param) && CGame::InitialiseRenderWare();
}

// 0x53D910
void RwTerminate() {
    CGame::ShutdownRenderWare();
    ResetGammaWhenExiting();
    RsRwTerminate();
}

// 0x53E170
void RenderEffects() {
    ZoneScoped;

    CBirds::Render();
    CSkidmarks::Render();
    CRopes::Render();
    CGlass::Render();
    CMovingThings::Render();
    CVisibilityPlugins::RenderReallyDrawLastObjects();
    CCoronas::Render();
    g_fx.Render(TheCamera.m_pRwCamera, false);
    CWaterCannons::Render();
    CWaterLevel::RenderWaterFog();
    CClouds::MovingFogRender();
    CClouds::VolumetricCloudsRender();
    if (CHeli::NumberOfSearchLights || CTheScripts::NumberOfScriptSearchLights) {
        CHeli::Pre_SearchLightCone();
        CHeli::RenderAllHeliSearchLights();
        CTheScripts::RenderAllSearchLights();
        CHeli::Post_SearchLightCone();
    }
    CWeaponEffects::Render();
    if (CReplay::Mode != MODE_PLAYBACK && !CPad::GetPad(0)->DisablePlayerControls) {
        FindPlayerPed()->DrawTriangleForMouseRecruitPed();
    }
    CSpecialFX::Render();
    CVehicleRecording::Render();
    CPointLights::RenderFogEffect();
    CRenderer::RenderFirstPersonVehicle();
    CPostEffects::Render();

    notsa::ui::UIRenderer::GetSingleton().Render3D();
}

// 0x53DF40
void RenderScene() {
    ZoneScoped;

    const auto underWater = CWeather::UnderWaterness <= 0.0f;

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));

    if (CMirrors::TypeOfMirror == MIRROR_TYPE_NONE) {
        CMovingThings::Render_BeforeClouds();
        CClouds::Render();
    }

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESHADEMODE, RWRSTATE(rwSHADEMODEGOURAUD));

    CCarFXRenderer::PreRenderUpdate();
    CRenderer::RenderRoads();
    CCoronas::RenderReflections();
    CRenderer::RenderEverythingBarRoads();
    g_breakMan.Render(false);

    CRenderer::RenderFadingInUnderwaterEntities();

    // If under water render this immediately, otherwise
    // only after rendering the stuff below.
    if (underWater) {
        RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));
        CWaterLevel::RenderWater();
        RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLBACK));
    }

    CRenderer::RenderFadingInEntities();
    if (!CMirrors::bRenderingReflection) {
        float nearClipPlaneOld = RwCameraGetNearClipPlane(Scene.m_pRwCamera);
        float farPlane = RwCameraGetFarClipPlane(Scene.m_pRwCamera);

        float v3;
        float z = CCamera::GetActiveCamera().m_vecFront.z;
        if (z <= 0.0f)
            v3 = -z;
        else
            v3 = 0.0f;

        constexpr float flt_8CD4F0 = 2.0f;
        constexpr float flt_8CD4EC = 5.9604645e-8f; // Same as 1 / (float)(1 << 24) => 1 / 16777216.f ; Not sure if that means anything..

        float unknown = ((flt_8CD4F0 * flt_8CD4EC * 0.25f - flt_8CD4F0 * flt_8CD4EC) * v3 + flt_8CD4F0 * flt_8CD4EC) * (farPlane - nearClipPlaneOld);

        RwCameraEndUpdate(Scene.m_pRwCamera);

        RwCameraSetNearClipPlane(Scene.m_pRwCamera, unknown + nearClipPlaneOld);
        RwCameraBeginUpdate(Scene.m_pRwCamera);
        CShadows::UpdateStaticShadows();
        CShadows::RenderStaticShadows();
        CShadows::RenderStoredShadows();
        RwCameraEndUpdate(Scene.m_pRwCamera);
        RwCameraSetNearClipPlane(Scene.m_pRwCamera, nearClipPlaneOld);

        RwCameraBeginUpdate(Scene.m_pRwCamera);
    }

    g_breakMan.Render(true);
    CPlantMgr::Render();

    RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));

    if (CMirrors::TypeOfMirror == MIRROR_TYPE_NONE) {
        CClouds::RenderBottomFromHeight();
        CWeather::RenderRainStreaks();
        CCoronas::RenderSunReflection();
    }

    if (!underWater) {
        RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));
        CWaterLevel::RenderWater();
        RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLBACK));
    }

    CStencilShadows::RenderStencilShadows();
}

// 0x53E530
void RenderMenus() {
    ZoneScoped;

    if (FrontEndMenuManager.m_bMenuActive) {
        FrontEndMenuManager.DrawFrontEnd();
    }
}

// 0x53E230
void Render2dStuff() {
    ZoneScoped;

    RenderDebugShit(); // NOTSA, temp

    const auto DrawOuterZoomBox = []() {
        CPed* player = FindPlayerPed();
        eWeaponType weaponType = WEAPON_UNARMED;
        if (player)
            weaponType = player->GetActiveWeapon().m_Type;
        eCamMode camMode = CCamera::GetActiveCamera().m_nMode;
        bool firstPersonWeapon = false;
        if (camMode == MODE_SNIPER || camMode == MODE_SNIPER_RUNABOUT || camMode == MODE_ROCKETLAUNCHER || camMode == MODE_ROCKETLAUNCHER_RUNABOUT || camMode == MODE_CAMERA ||
            camMode == MODE_HELICANNON_1STPERSON) {
            firstPersonWeapon = true;
        }

        if ((weaponType == WEAPON_SNIPERRIFLE || weaponType == WEAPON_ROCKET) && firstPersonWeapon) {
            CRGBA black(0, 0, 0, 255);
            if (weaponType == WEAPON_ROCKET) {
                CSprite2d::DrawRect(CRect(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT / 2 - SCREEN_SCALE_Y(180.0f)), black);
                CSprite2d::DrawRect(CRect(0.0f, SCREEN_HEIGHT / 2 + SCREEN_SCALE_Y(170.0f), SCREEN_WIDTH, SCREEN_HEIGHT), black);
            } else {
                CSprite2d::DrawRect(CRect(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT / 2 - SCREEN_SCALE_Y(210.0f)), black);
                CSprite2d::DrawRect(CRect(0.0f, SCREEN_HEIGHT / 2 + SCREEN_SCALE_Y(210.0f), SCREEN_WIDTH, SCREEN_HEIGHT), black);
            }
            CSprite2d::DrawRect(CRect(0.0f, 0.0f, SCREEN_WIDTH / 2 - SCREEN_SCALE_X(210.0f), SCREEN_HEIGHT), black);
            CSprite2d::DrawRect(CRect(SCREEN_WIDTH / 2 + SCREEN_SCALE_X(210.0f), 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT), black);
        }
    };

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, RWRSTATE(rwRENDERSTATENARENDERSTATE));
    RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));

    CReplay::Display();
    CPickups::RenderPickUpText();
    if (TheCamera.m_bWideScreenOn && !FrontEndMenuManager.m_bWidescreenOn)
        TheCamera.DrawBordersForWideScreen();
    DrawOuterZoomBox();
    AudioEngine.DisplayRadioStationName();
    CHud::Draw();
    CSpecialFX::Render2DFXs();
    CUserDisplay::OnscnTimer.ProcessForDisplay();
    CMessages::Display(true);
    CDarkel::DrawMessages();
    CGarages::PrintMessages();
    CFont::DrawFonts();
}

// 0x53E160
void RenderDebugShit() {
    PUSH_RENDERGROUP("RenderDebugShit");
    CTheScripts::RenderTheScriptDebugLines();
#ifndef FINAL
    // if(gbShowCollisionLines) CRenderer::RenderCollisionLines();
    // ThePaths.DisplayPathData();
    // CDebug::DrawLines();
    DefinedState();
#endif
    POP_RENDERGROUP();
}

// 0x53E920
void Idle(void* param) {
    ZoneScoped;

    /* FPS lock. Limits to 26 frames per second.
    CTimer::GetCurrentTimeInCycles();
    CTimer::GetCyclesPerMillisecond();
    do
        CurrentTimeInCycles = CTimer::GetCurrentTimeInCycles();
    while ( CurrentTimeInCycles / CTimer::GetCyclesPerMillisecond() - g_LastTickTime < 14 );
    v2 = CTimer::GetCurrentTimeInCycles();
    g_LastTickTime = v2 / CTimer::GetCyclesPerMillisecond();
    */

    CTimer::Update();
    CSprite2d::InitPerFrame();
    CFont::InitPerFrame();
    CPointLights::NumLights = 0;
    CGame::Process();
    AudioEngine.Service();
    SetLightsWithTimeOfDayColour(Scene.m_pRpWorld);
    if (!param) {
        return;
    }

    auto& cc = CTimeCycle::m_CurrentColours;
    if (FrontEndMenuManager.m_bMenuActive || TheCamera.GetScreenFadeStatus() == eNameState::NAME_FADE_IN) {
        CDraw::CalculateAspectRatio();
        CameraSize(Scene.m_pRwCamera, nullptr, SCREEN_VIEW_WINDOW, SCREEN_ASPECT_RATIO);
        CVisibilityPlugins::SetRenderWareCamera(Scene.m_pRwCamera);
        RwCameraClear(Scene.m_pRwCamera, &gColourTop, rwCAMERACLEARZ);
        if (!RsCameraBeginUpdate(Scene.m_pRwCamera)) {
            return;
        }
    }

    if (!FrontEndMenuManager.m_bMenuActive && TheCamera.GetScreenFadeStatus() != eNameState::NAME_FADE_IN) {
        if (!notsa::ui::UIRenderer::GetSingleton().GetImIO()->NavActive) { // If imgui nav is active don't center the cursor
            FrontEndMenuManager.CentreMousePointer();
        }

        CRenderer::ConstructRenderList();
        CRenderer::PreRender();
        CWorld::ProcessPedsAfterPreRender();
        g_realTimeShadowMan.Update();
        CMirrors::BeforeMainRender();

        bool started;
        if (CWeather::LightningFlash) {
            cc.m_nSkyBottomRed = 255;
            cc.m_nSkyBottomGreen = 255;
            cc.m_nSkyBottomBlue = 255;
            started = DoRWStuffStartOfFrame_Horizon(255, 255, 255, 255, 255, 255, 255);
        } else {
            started = DoRWStuffStartOfFrame_Horizon(cc.m_nSkyTopRed, cc.m_nSkyTopGreen, cc.m_nSkyTopBlue, cc.m_nSkyBottomRed, cc.m_nSkyBottomGreen, cc.m_nSkyBottomBlue, 255);
        }
        if (!started)
            return;

        DefinedState();
        RwCameraSetFarClipPlane(Scene.m_pRwCamera, cc.m_fFarClip);
        Scene.m_pRwCamera->fogPlane = cc.m_fFogStart;
        CMirrors::RenderMirrorBuffer();
        RenderScene();
        CVisibilityPlugins::RenderWeaponPedsForPC();
        CVisibilityPlugins::ms_weaponPedsForPC.Clear();
        RenderEffects();
        if (TheCamera.m_nBlurType == MOTION_BLUR_NONE || TheCamera.m_nBlurType == MOTION_BLUR_LIGHT_SCENE) {
            if (TheCamera.m_fScreenReductionPercentage > 0.0f) {
                TheCamera.SetMotionBlurAlpha(150);
            }
        }
        TheCamera.RenderMotionBlur();
        Render2dStuff();
    }

    RenderMenus();
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
    DoFade();
    CHud::DrawAfterFade();
    CMessages::Display(false);
    CFont::DrawFonts();
    CCredits::Render();
    CDebug::DebugDisplayTextBuffer();
    FlushObrsPrintfs();

    // NOTSA: ImGui menu draw loop
    notsa::ui::UIRenderer::GetSingleton().DrawLoop();

    RwCameraEndUpdate(Scene.m_pRwCamera);
    RsCameraShowRaster(Scene.m_pRwCamera);
}

// 0x53E770
void FrontendIdle() {
    ZoneScoped;

    CDraw::CalculateAspectRatio();
    CTimer::Update();
    CSprite2d::SetRecipNearClip();
    CSprite2d::InitPerFrame();
    CFont::InitPerFrame();
    CPad::UpdatePads();

    if (FrontEndMenuManager.m_bActivateMenuNextFrame) {
        FrontEndMenuManager.m_bMenuActive = true;
    }
    FrontEndMenuManager.Process();

    if (RsGlobal.quit) {
        return;
    }

    AudioEngine.Service();
    CameraSize(Scene.m_pRwCamera, nullptr, SCREEN_VIEW_WINDOW, SCREEN_ASPECT_RATIO);
    CVisibilityPlugins::SetRenderWareCamera(Scene.m_pRwCamera);

    if (g_FastLoaderConfig.ShouldLoadSaveGame()) {
        return; // Don't render anything
    }

    RwCameraClear(Scene.m_pRwCamera, &gColourTop, rwCAMERACLEARZ);
    if (RsCameraBeginUpdate(Scene.m_pRwCamera)) {
        DefinedState();

        if (FrontEndMenuManager.m_bMenuActive) {
            FrontEndMenuManager.DrawFrontEnd();
        }

        SecuromStateDisplay();
        DoFade();
        CHud::DrawAfterFade();
        CMessages::Display(false);
        CFont::DrawFonts();

        if (CCredits::bCreditsGoing && !FrontEndMenuManager.m_bMenuActive) {
            CCredits::RenderCredits();
        }

        CFont::DrawFonts();
        CDebug::DebugDisplayTextBuffer();
        FlushObrsPrintfs();

        // NOTSA: ImGui menu draw loop
        notsa::ui::UIRenderer::GetSingleton().DrawLoop();

        RwCameraEndUpdate(Scene.m_pRwCamera);
        RsCameraShowRaster(Scene.m_pRwCamera);
    }
}
