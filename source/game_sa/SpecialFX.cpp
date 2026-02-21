#include "StdInc.h"
#include "MotionBlurStreaks.h"
#include "Checkpoints.h"
#include "SpecialFX.h"

void CSpecialFX::InjectHooks() {
    RH_ScopedClass(CSpecialFX);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x7268F0);
    RH_ScopedInstall(Update, 0x726AA0);
    RH_ScopedInstall(Shutdown, 0x723390);
    RH_ScopedInstall(AddWeaponStreak, 0x7233F0);
    RH_ScopedInstall(Render, 0x726AD0);
    RH_ScopedInstall(Render2DFXs, 0x721660, { .reversed = false });
    RH_ScopedInstall(ReplayStarted, 0x721D30);
}

// 0x7268F0
void CSpecialFX::Init() {
    for (auto& bt : CBulletTraces::aTraces) {
        bt.m_bExists = false;
    }

    for (auto& m : C3dMarkers::ms_user3dMarkers) {
        m.m_bIsUsed = false;
    }

    for (auto& da : C3dMarkers::ms_directionArrows) {
        da.m_bIsUsed = false;
    }

    constexpr RxVertexIndex MBSIndices[] = { 0, 1, 2, 1, 3, 2, 0, 2, 1, 1, 2, 3 };
    constexpr RxObjSpace3DVertex MBSVertices[] = {
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
    };
    rng::copy(MBSIndices, CMotionBlurStreaks::aIndices);
    rng::copy(MBSVertices, CMotionBlurStreaks::aStreakVertices);

    for (auto& mbs : CMotionBlurStreaks::aStreaks) {
        mbs.m_nId = 0;
    }

    CBrightLights::NumBrightLights = 0;
    CShinyTexts::NumShinyTexts     = 0;
    C3dMarkers::Init();
    CCheckpoints::Init();

    bVideoCam       = false;
    bLiftCam        = false;
    bSnapShotActive = false;
    SnapShotFrames  = 0;

    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("particle"));
    if (!gpFinishFlagTex) {
        gpFinishFlagTex = RwTextureRead("finishFlag", nullptr);
    }
    CTxdStore::PopCurrentTxd();

    if (CMirrors::pBuffer) {
        RwRasterDestroy(std::exchange(CMirrors::pBuffer, nullptr));
    }
    if (CMirrors::pZBuffer) {
        RwRasterDestroy(std::exchange(CMirrors::pZBuffer, nullptr));
    }
    CMirrors::TypeOfMirror = eMirrorType::MIRROR_TYPE_NONE;
    CMirrors::MirrorFlags  = 0;
}

// 0x726AA0
void CSpecialFX::Update() {
    ZoneScoped;

    for (auto& mbs : CMotionBlurStreaks::aStreaks) {
        if (mbs.m_nId) {
            mbs.Update();
        }
    }
    CBulletTraces::Update();
    CCheckpoints::Update();
    C3dMarkers::Update();
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
