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
    RH_ScopedInstall(Render2DFXs, 0x721660);
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
    rng::copy(MBSIndices, CMotionBlurStreaks::aIndices);

    for (auto i = 0; i < 2; i++) {
        RxObjSpace3DVertexSetU(&CMotionBlurStreaks::aStreakVertices[i + 0], 0.0f);
        RxObjSpace3DVertexSetV(&CMotionBlurStreaks::aStreakVertices[i + 0], 0.0f);

        RxObjSpace3DVertexSetU(&CMotionBlurStreaks::aStreakVertices[i + 1], 1.0f);
        RxObjSpace3DVertexSetV(&CMotionBlurStreaks::aStreakVertices[i + 1], 0.0f);
    }

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

    {
        CTxdStore::ScopedTXDSlot scoped("particle");
        if (!gpFinishFlagTex) {
            gpFinishFlagTex = RwTextureRead("finishFlag", nullptr);
        }
    }

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
        RwTextureDestroy(std::exchange(gpFinishFlagTex, nullptr));
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
    if (bVideoCam) {
        CFont::SetScale(SCREEN_STRETCH_X(1.5f), SCREEN_STRETCH_Y(1.5f));
        CFont::SetBackground(false, false);
        CFont::SetProportional(true);
        CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
        CFont::SetColor({ 0, 255, 0, 200 });
        CFont::SetFontStyle(FONT_SUBTITLES);
        notsa::format_to_sz(gString, "{}", CTimer::GetFrameCounter() % 64);
        AsciiToGxtChar(gString, gGxtString);
        CFont::PrintString(0.8f * SCREEN_WIDTH, 0.8f * SCREEN_HEIGHT, gGxtString);

        for (auto y = 3.0f; y - 3.0f < SCREEN_HEIGHT; y += 4.0f) {
            RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDONE));
            RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDONE));
            CSprite2d::Draw2DPolygon(
                0.0f,         y - 3.0f,
                SCREEN_WIDTH, y - 3.0f,
                0.0f,         y - 2.0f,
                SCREEN_WIDTH, y - 2.0f,
                { 0, 100, 0, 100 }
            );
            RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDSRCALPHA));
            RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
            CSprite2d::Draw2DPolygon(
                0.0f,         y - 1.0f,
                SCREEN_WIDTH, y - 1.0f,
                0.0f,         y,
                SCREEN_WIDTH, y,
                { 0, 0, 0, 150 }
            );
        }
        RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDSRCALPHA));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));

        const auto yt = std::trunc((SCREEN_HEIGHT + 70.0f) / 2048.0f * float(CTimer::GetTimeInMS() % 2'048) - 70.0f);
        CSprite2d::Draw2DPolygon(
            0.0f,         yt,
            SCREEN_WIDTH, yt,
            0.0f,         yt + 70.0f,
            SCREEN_WIDTH, yt + 70.0f,
            { 0, 100, 0, 60 }
        );
    }

    if (bLiftCam) {
        CFont::SetScale(SCREEN_STRETCH_X(1.5f), SCREEN_STRETCH_Y(1.5f));
        CFont::SetBackground(false, false);
        CFont::SetProportional(true);
        CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
        CFont::SetColor({ 100, 100, 100, 200 });
        CFont::SetFontStyle(FONT_SUBTITLES);
        // ...nothing to print?

        for (auto y = 3.0f; y - 3.0f < SCREEN_HEIGHT; y += 4.0f) {
            RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDONE));
            RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDONE));
            CSprite2d::Draw2DPolygon(
                0.0f,         y - 3.0f,
                SCREEN_WIDTH, y - 3.0f,
                0.0f,         y - 2.0f,
                SCREEN_WIDTH, y - 2.0f,
                { 100, 100, 100, 100 }
            );
            RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDSRCALPHA));
            RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
            CSprite2d::Draw2DPolygon(
                0.0f,         y - 1.0f,
                SCREEN_WIDTH, y - 1.0f,
                0.0f,         y,
                SCREEN_WIDTH, y,
                { 0, 0, 0, 150 }
            );
        }

        const auto yt = std::trunc((SCREEN_HEIGHT + 70.0f) / 2048.0f * float(CTimer::GetTimeInMS() % 2'048) - 70.0f);
        CSprite2d::Draw2DPolygon(
            0.0f,         yt,
            SCREEN_WIDTH, yt,
            0.0f,         yt + 70.0f,
            SCREEN_WIDTH, yt + 70.0f,
            { 100, 100, 100, 60 }
        );

        for (auto i = 0; i < 200; i++) {
            const auto x = CGeneral::GetRandomNumberInRange<uint32>(RsGlobal.maximumWidth);
            const auto y = CGeneral::GetRandomNumberInRange<uint32>(RsGlobal.maximumHeight);
            CSprite2d::DrawRect(
                CRect{
                    float(x),
                    float(y),
                    float(x + 20),
                    float(y + 2)
                },
                { 255, 255, 255, 64 }
            );
        }
    }

    if (bSnapShotActive) {
        TheCamera.GetActiveCam().m_fAlphaSpeed = 0.0f;
        TheCamera.GetActiveCam().m_fBetaSpeed  = 0.0f;

        if (++SnapShotFrames <= 20) {
            CTimer::SetTimeScale(0.0f);
            if (SnapShotFrames < 10) {
                RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDONE));
                RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDONE));

                const uint8 shade = 255 - 255 * SnapShotFrames / 10;
                CSprite2d::Draw2DPolygon(
                    0.0f,         0.0f,
                    SCREEN_WIDTH, 0.0f,
                    0.0f,         SCREEN_HEIGHT,
                    SCREEN_WIDTH, SCREEN_HEIGHT,
                    { shade, shade, shade, shade }
                );

                RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDSRCALPHA));
                RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
            }
        } else {
            bSnapShotActive = false;
            CTimer::ResetTimeScale();
        }
    }
}

// 0x721D30
void CSpecialFX::ReplayStarted() {
    if (bSnapShotActive) {
        bSnapShotActive = false;
        CTimer::ResetTimeScale();
    }
}
