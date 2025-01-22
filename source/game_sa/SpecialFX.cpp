#include "StdInc.h"
#include "MotionBlurStreaks.h"
#include "Checkpoints.h"
#include "SpecialFX.h"

bool& CSpecialFX::bVideoCam = *(bool*)0xC7C70C;
bool& CSpecialFX::bLiftCam = *(bool*)0xC7C70D;
bool& CSpecialFX::bSnapShotActive = *(bool*)0xC7C714;
uint32& CSpecialFX::SnapShotFrames = *(uint32*)0xC7C710;

void CSpecialFX::InjectHooks() {
    RH_ScopedClass(CSpecialFX);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x7268F0, { .reversed = false });
    RH_ScopedInstall(Update, 0x726AA0, { .reversed = false });
    RH_ScopedInstall(Shutdown, 0x723390);
    RH_ScopedInstall(AddWeaponStreak, 0x7233F0);
    RH_ScopedInstall(Render, 0x726AD0);
    RH_ScopedInstall(Render2DFXs, 0x721660, { .reversed = false });
    RH_ScopedInstall(ReplayStarted, 0x721D30);
}

// 0x7268F0
void CSpecialFX::Init() {
    plugin::Call<0x7268F0>();
}

// 0x726AA0
void CSpecialFX::Update() {
    ZoneScoped;

    plugin::Call<0x726AA0>();
}

// 0x723390
void CSpecialFX::Shutdown() {
    C3dMarkers::Shutdown();
    if (gpFinishFlagTex) {
        RwTextureDestroy(gpFinishFlagTex);
        gpFinishFlagTex = nullptr;
    }
    CMirrors::ShutDown();
}

// unused function
// 0x7233F0
void CSpecialFX::AddWeaponStreak(eWeaponType weaponType) {
    const auto plyr = FindPlayerPed();
    static CMatrix attachMat;
    RwMatrix* LTM;
    CVector end;
    if (!plyr || !plyr->m_pWeaponObject) {
        return;
    }

    const auto DoStreak = [plyr, LTM, end](CVector endO) {
        *LTM      = *RwFrameGetLTM(RpAtomicGetFrame(plyr->m_pWeaponObject));
        attachMat = CMatrix(LTM, false);
        CMotionBlurStreaks::RegisterStreak(
            reinterpret_cast<uint32>(plyr->m_pWeaponObject),
            100,
            100,
            100,
            255,
            attachMat.TransformPoint({ 0.02f, 0.05f, 0.07f }),
            attachMat.TransformVector(endO)
        );
    };


    switch (weaponType) {
    case WEAPON_BASEBALLBAT: DoStreak({ 0.246f,  0.0325f,  0.796f }); break;
    case WEAPON_GOLFCLUB:    DoStreak({ -0.054f, 0.0325f,  0.796f }); break;
    case WEAPON_KATANA:      DoStreak({ 0.096f,  -0.0175f, 1.096f }); break;
    default:                 break; // OG: No streak is created
    }
}

// 0x726AD0
void CSpecialFX::Render() {
    ZoneScoped;

    CMotionBlurStreaks::Render();
    CBulletTraces::Render();
    CBrightLights::Render();
    CShinyTexts::Render();
    C3dMarkers::Render();
    CCheckpoints::Render();
}

// 0x721660
void CSpecialFX::Render2DFXs() {
    plugin::Call<0x721660>();
}

// 0x721D30
void CSpecialFX::ReplayStarted() {
    if (bSnapShotActive) {
        bSnapShotActive = false;
        CTimer::ResetTimeScale();
    }
}
