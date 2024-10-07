#include "StdInc.h"

#include "UIRenderer.h"

#include "WaterCannons.h"
#include "TheCarGenerators.h"
#include "Radar.h"
#include "Credits.h"
#include "Birds.h"
#include "MovingThings.h"
#include "PlantMgr.h"
#include "Occlusion.h"
#include "InteriorManager_c.h"
// #include "ProcObjectMan_c.h"
#include "WaterCreatureManager_c.h"
#include "MenuManager.h"
#include "FireManager.h"
#include "Fx.h"
#include "BreakManager_c.h"
#include "BoneNodeManager_c.h"
#include "Shadows.h"
// todo: #include "ShadowManager.h"
#include "PedType.h"
#include "IKChainManager_c.h"
#include "CreepingFire.h"
#include "Skidmarks.h"
#include "CarCtrl.h"
#include "TagManager.h"
#include "Clouds.h"
#include "StuntJumpManager.h"
#include "CustomCarPlateMgr.h"
#include "TaskSimpleFight.h"
#include "Conversations.h"
#include "EntryExitManager.h"
#include "TempColModels.h"
#include "PedStats.h"
#include "UserDisplay.h"
#include "PostEffects.h"
#include "Rope.h"
#include "Ropes.h"
#include "Glass.h"
#include "TheScripts.h"
#include "LoadingScreen.h"
#include "GridRef.h"
#include "MenuSystem.h"
#include "RealTimeShadowManager.h"
#include "VehicleRecording.h"
#include "ColAccel.h"
#include "app.h"
#include "Garages.h"
#include "TaskSimpleClimb.h"
#include "PedAttractor.h"
#include "MBlur.h"
#include "CustomBuildingRenderer.h"
#include "Hud.h"
#include "CarFXRenderer.h"
#include "ProcObjectMan.h"
#include "InterestingEvents.h"
#include "WindModifiers.h"
#include "GrassRenderer.h"

void CGame::InjectHooks() {
    RH_ScopedClass(CGame);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(CanSeeOutSideFromCurrArea, 0x53C4A0);
    RH_ScopedInstall(CanSeeWaterFromCurrArea, 0x53C4B0);
    RH_ScopedInstall(Init1, 0x5BF840);
    RH_ScopedInstall(Init2, 0x5BA1A0);
    RH_ScopedInstall(Init3, 0x5BA400);
    RH_ScopedInstall(Initialise, 0x53BC80);
    RH_ScopedInstall(InitialiseCoreDataAfterRW, 0x5BFA90);
    RH_ScopedInstall(InitialiseEssentialsAfterRW, 0x5BA160);
    RH_ScopedInstall(InitialiseOnceBeforeRW, 0x53BB50);
    RH_ScopedInstall(InitialiseRenderWare, 0x5BD600);
    RH_ScopedInstall(InitialiseWhenRestarting, 0x53C680);
    RH_ScopedInstall(Process, 0x53BEE0);
    RH_ScopedInstall(ReInitGameObjectVariables, 0x53BCF0);
    RH_ScopedInstall(ReloadIPLs, 0x53BED0);
    RH_ScopedInstall(ShutDownForRestart, 0x53C550);
    RH_ScopedInstall(Shutdown, 0x53C900);
    RH_ScopedInstall(ShutdownRenderWare, 0x53BB80);
    RH_ScopedInstall(DrasticTidyUpMemory, 0x53C810);
    RH_ScopedInstall(FinalShutdown, 0x53BC30);
    RH_ScopedInstall(TidyUpMemory, 0x53C500);

    RH_ScopedGlobalInstall(CameraDestroy, 0x72FD90);
    RH_ScopedGlobalInstall(ValidateVersion, 0x5BA0BC);
}

// 0x72FD90
static void CameraDestroy(RwCamera* rwCamera) {
    if (!rwCamera)
        return;

    if (auto parent = rwObjectGetParent(&rwCamera->object.object); parent) {
        rwObjectHasFrameSetFrame(rwCamera, NULL);
        RwFrameDestroy((RwFrame*)parent);
    }

    const auto DestroyIncludingParent = [](auto* buffer) {
        if (!buffer)
            return;

        RwRasterDestroy(buffer);
        if (auto parent = RwRasterGetParent(buffer); parent && parent != buffer) {
            RwRasterDestroy(parent);
        }
    };

    DestroyIncludingParent(rwCamera->frameBuffer);
    DestroyIncludingParent(rwCamera->zBuffer);
    RwCameraDestroy(rwCamera);
}

// 0x5BA0BC
void ValidateVersion() {
    auto file = CFileMgr::OpenFile("models\\coll\\peds.col", "rb");
    if (!file) {
        NOTSA_UNREACHABLE("Invalid version\nmodels\\coll\\peds.col file not found.");
    }
    CFileMgr::Seek(file, 100, 0);

    // FORMAT: grandtheftauto3Version <mj>.<mn>, encoded and spreaded into the file.
    char buf[128];
    for (auto i = 0; i < 128; i++) {
        CFileMgr::Read(file, &buf[i], 1u);
        buf[i] -= 23;

        if (buf[i] == '\0') {
            break;
        }
        CFileMgr::Seek(file, 99, 1);
    }

    if (strncmp(buf, "grandtheftauto3", 15u) != 0) {
        NOTSA_UNREACHABLE("Invalid version\npeds.col version text does not start with 'grandtheftauto3'.\nText was '{}'", buf);
    }

    static char(&version_name)[64] = *reinterpret_cast<char(*)[64]>(0xB72C28);

    strncpy_s(version_name, &buf[15], 64u);
    CFileMgr::CloseFile(file);
}

// 0x53BB80
void CGame::ShutdownRenderWare() {
    RwMatrixDestroy(m_pWorkingMatrix1);
    RwMatrixDestroy(m_pWorkingMatrix2);
    CLoadingScreen::Shutdown();
    CHud::Shutdown();
    CFont::Shutdown();
    rng::for_each(CWorld::Players, &CPlayerInfo::DeletePlayerSkin);
    CPlayerSkin::Shutdown();
    DestroyDebugFont();
    LightsDestroy(Scene.m_pRpWorld);
    CVisibilityPlugins::Shutdown();
    RpWorldRemoveCamera(Scene.m_pRpWorld, Scene.m_pRwCamera);
    RpWorldDestroy(Scene.m_pRpWorld);
    CameraDestroy(Scene.m_pRwCamera);
    Scene.m_pRpWorld = nullptr;
    Scene.m_pRwCamera = nullptr;
    D3DResourceSystem::CancelBuffering();
    CPostEffects::Close();
}

// 0x53C4A0
bool CGame::CanSeeOutSideFromCurrArea() { // pattern: !CGame::currArea 
    return currArea == AREA_CODE_NORMAL_WORLD;
}

// 0x53C4B0
bool CGame::CanSeeWaterFromCurrArea() {
    return CanSeeOutSideFromCurrArea() || currArea == AREA_CODE_5;
}

// 0x53C500
void CGame::TidyUpMemory(bool a1, bool clearD3Dmem) {
    if (FindPlayerPed(PED_TYPE_PLAYER1) && clearD3Dmem) {
        DrasticTidyUpMemory(a1);
    }
}

// 0x53C810
void CGame::DrasticTidyUpMemory(bool a1) {
    D3DResourceSystem::TidyUpD3DTextures(100);
    D3DResourceSystem::TidyUpD3DIndexBuffers(200);
}

// 0x53C900
bool CGame::Shutdown() {
    g_breakMan.Exit();
    g_interiorMan.Exit();
    g_procObjMan.Exit();
    g_waterCreatureMan.Exit();
    D3DResourceSystem::SetUseD3DResourceBuffering(false);
    CStencilShadows::Shutdown();
    CPlantMgr::Shutdown();
    CGrassRenderer::Shutdown();
    CRopes::Shutdown();
    CVehicleRecording::ShutDown();
    CReplay::FinishPlayback();
    CReplay::EmptyReplayBuffer();
    CTrain::Shutdown();
    CBirds::Shutdown();
    CSpecialFX::Shutdown();
    CGarages::Shutdown();
    CStuntJumpManager::Shutdown();
    CMovingThings::Shutdown();
    CWeapon::ShutdownWeapons();
    CPedType::Shutdown();

    rng::for_each(CWorld::Players, [](auto& player) {
        if (player.m_pPed) {
            CWorld::Remove(player.m_pPed);
            delete player.m_pPed;
            player.m_pPed = nullptr;
        }
        player.Clear();
    });

    CRenderer::Shutdown();
    CWorld::ShutDown();
    CEntryExitManager::Shutdown();
    g_fx.Exit();
    g_boneNodeMan.Exit();
    g_ikChainMan.Exit();
    g_realTimeShadowMan.Exit();
    CModelInfo::ShutDown();
    CAnimManager::Shutdown();
    CCutsceneMgr::Shutdown();
    CVehicleModelInfo::DeleteVehicleColourTextures();
    CVehicleModelInfo::ShutdownEnvironmentMaps();
    CRadar::Shutdown();
    CStreaming::Shutdown();
    CTxdStore::GameShutdown();
    CCollision::Shutdown();
    CWaterLevel::Shutdown();
    CClouds::Shutdown();
    CShadows::Shutdown();
    CCoronas::Shutdown();
    CSkidmarks::Shutdown();
    CWeaponEffects::Shutdown();
    CTimeCycle::Shutdown();
    CCover::m_ListOfProcessedBuildings.Flush();
    CPedGroups::CleanUpForShutDown();
    CTaskSequences::CleanUpForShutdown();
    CInformGroupEventQueue::Flush();
    CInformFriendsEventQueue::Flush();
    CPools::ShutDown();
    CCarFXRenderer::Shutdown();
    CCustomBuildingRenderer::Shutdown();
    CVehicleModelInfo::ShutdownLightTexture();
    CVehicle::Shutdown();
    CCarFXRenderer::Shutdown();
    CCustomCarPlateMgr::Shutdown();
    CCustomRoadsignMgr::Shutdown();
    CHud::ReInitialise();
    CTxdStore::RemoveTxdSlot(gameTxdSlot);
    CTxdStore::RemoveTxdSlot(CTxdStore::FindTxdSlot("particle"));
    col1[1].m_boundBox.m_vecMin.z = 0.0f;
    col1[0].m_pColData = nullptr;
    CTaskSimpleClimb::Shutdown();

    { // todo: move to CPedAttractor::Shutdown() or something
        // delete CPedAttractor::ms_tasks.First;
        // CPedAttractor::ms_tasks.First = 0;
        // CPedAttractor::ms_tasks.Last = 0;
        // CPedAttractor::ms_tasks.End = 0;
        CPedAttractor::ms_tasks = {};
    }

    CTheScripts::RemoveScriptTextureDictionary();
    CMBlur::MotionBlurClose();
    CdStreamRemoveImages();
    D3DResourceSystem::Shutdown();
    return true;
}

// 0x53C550
void CGame::ShutDownForRestart() {
    D3DResourceSystem::SetUseD3DResourceBuffering(false);
    CVehicleRecording::ShutDown();
    CReplay::FinishPlayback();
    CReplay::EmptyReplayBuffer();
    CMovingThings::Shutdown();
    rng::for_each(CWorld::Players, [](auto& info) { info.Clear(); });
    CTheScripts::UndoBuildingSwaps();
    CTheScripts::UndoEntityInvisibilitySettings();
    g_interiorMan.Exit();
    g_procObjMan.Exit();
    g_waterCreatureMan.Exit();
    CRopes::Shutdown();
    CWorld::ClearForRestart();
    CGameLogic::ClearSkip(false);
    CTimer::Shutdown();
    CStreaming::ReInit();
    CRadar::RemoveRadarSections();
    FrontEndMenuManager.UnloadTextures();
    CSetPieces::Init();
    CConversations::Clear();
    CPedToPlayerConversations::Clear();
    CPedType::Shutdown();
    CSpecialFX::Shutdown();
    gFireManager.Shutdown();
    g_fx.Reset();
    g_breakMan.ResetAll();
    g_boneNodeMan.Reset();
    g_ikChainMan.Reset();
    g_realTimeShadowMan.Exit();
    CTheZones::ResetZonesRevealed();
    CEntryExitManager::ShutdownForRestart();
    CShopping::ShutdownForRestart();
    CTagManager::ShutdownForRestart();
    CStuntJumpManager::ShutdownForRestart();
    FindPlayerPed(PED_TYPE_PLAYER1);
    CVehicle::ms_forceVehicleLightsOff = false;
}

// 0x53BC30
void CGame::FinalShutdown() {
    ThePaths.Shutdown();
    CTempColModels::Shutdown();
    CTxdStore::Shutdown();
    CPedStats::Shutdown();
    TheText.Unload(false);
    CdStreamShutdown();
}

// 0x
void CGame::GenerateTempPedAtStartOfNetworkGame() {
    // NOP
}

// 0x5BF840
bool CGame::Init1(char const *datFile) {
    ZoneScoped;

    CMaths::InitMathsTables();
    strcpy_s(aDatFile, datFile);
    CPools::Initialise();
    CPlaceable::InitMatrixArray();
    CIniFile::LoadIniFile();
    D3DResourceSystem::SetUseD3DResourceBuffering(false);
    currLevel = LEVEL_NAME_COUNTRY_SIDE;
    currArea = AREA_CODE_NORMAL_WORLD;

    CMemoryMgr::PushMemId(MEM_TEXTURES);
    gameTxdSlot = CTxdStore::AddTxdSlot("generic");
    CTxdStore::Create(gameTxdSlot);
    CTxdStore::AddRef(gameTxdSlot);

    int32 slot = CTxdStore::AddTxdSlot("particle");
    CTxdStore::LoadTxd(slot, "MODELS\\PARTICLE.TXD");
    CTxdStore::AddRef(slot);
    CTxdStore::SetCurrentTxd(gameTxdSlot);
    CMemoryMgr::PopMemId();

    CGameLogic::InitAtStartOfGame();
    CGangWars::InitAtStartOfGame();
    CConversations::Clear();
    CPedToPlayerConversations::Clear();
    CQuadTreeNode::InitPool();

    if (!CPlantMgr::Initialise() || !CCustomRoadsignMgr::Initialise()) {
        return false;
    }

    CReferences::Init();
    CDebug::DebugInitTextBuffer();
    CTagManager::Init();
    CWeather::Init();
    CCover::Init();
    CCullZones::Init();
    COcclusion::Init();
    CCollision::Init();
    CBirds::Init();
    CEntryExitManager::Init();
    CStuntJumpManager::Init();
    CSetPieces::Init();
    CTheZones::Init();
    CUserDisplay::Init();
    CMessages::Init();
    CMessages::ClearAllMessagesDisplayedByGame(0);
    CVehicleRecording::Init();
    CRestart::Initialise();

    CMemoryMgr::PushMemId(MEM_WORLD);
    CWorld::Initialise();
    CMemoryMgr::PopMemId();

    CMemoryMgr::PushMemId(MEM_ANIMATION);
    CAnimManager::Initialise();
    CCutsceneMgr::Initialise();
    CMemoryMgr::PopMemId();

    CMemoryMgr::PushMemId(MEM_CARS);
    CCarCtrl::Init();
    CMemoryMgr::PopMemId();

    // CMemoryMgr::PushMemId(MEM_DEFAULT_MODELS);
    InitModelIndices();

    CModelInfo::Initialise();
    CPickups::Init();
    CTheCarGenerators::Init();
    CGarages::Init();
    CAudioZones::Init();
    CStreaming::InitImageList();
    CStreaming::ReadIniFile();

    CMemoryMgr::PushMemId(MEM_PATHS);
    ThePaths.Init();
    CPathFind::AllocatePathFindInfoMem();
    CMemoryMgr::PopMemId();

    CTaskSimpleFight::LoadMeleeData();
    CCheat::ResetCheats();

    CMemoryMgr::PushMemId(MEM_FX);
    g_fx.Init();
    CMemoryMgr::PopMemId();

    return true;
}

// 0x5BA1A0
bool CGame::Init2(const char* datFile) {
    ZoneScoped;

    LoadingScreen("Loading the Game", "Add Particles");
    CTheZones::PostZoneCreation();
    CEntryExitManager::PostEntryExitsCreation();

    LoadingScreen("Loading the Game", "Setup paths");
    CPathFind::PreparePathData();

    rng::for_each(CWorld::Players, [](auto& info) {
        info.Clear();
        info.LoadPlayerSkin();
    });

    TestModelIndices();
    LoadingScreen("Loading the Game", "Setup water");
    CClothes::Init();
    CWaterLevel::WaterLevelInitialise();
    CDraw::SetFOV(120.0f);
    CDraw::ms_fLODDistance = 0.0f;

    if (!CCustomCarPlateMgr::Initialise()) {
        DEV_LOG("[CGame::Init2] CCustomCarPlateMgr::Initialise() failed");
        return false;
    }

    LoadingScreen("Loading the Game", "Setup streaming");
    CStreaming::LoadInitialVehicles();
    CStreaming::LoadInitialPeds();
    CStreaming::LoadAllRequestedModels(false);

    LoadingScreen("Loading the Game", "Load animations");
    CAnimManager::LoadAnimFiles();

    LoadingScreen("Loading the Game", "Setup streaming");
    CStreaming::LoadInitialWeapons();
    CStreaming::LoadAllRequestedModels(false);

    LoadingScreen("Loading the Game", "Ped Init");
    CPed::Initialise();
    CRenderer::Init();

    LoadingScreen("Loading the Game", "Setup game variables");
    CRadar::Initialise();
    CRadar::LoadTextures();

    LoadingScreen(nullptr, nullptr);
    CWeapon::InitialiseWeapons();
    CRoadBlocks::Init();

    LoadingScreen(nullptr, nullptr);
    CPopulation::Initialise();

    LoadingScreen(nullptr, nullptr);
    CWorld::PlayerInFocus = PED_TYPE_PLAYER1;
    CCoronas::Init();
    CShadows::Init();

    LoadingScreen(nullptr, nullptr);
    CWeaponEffects::Init();
    CSkidmarks::Init();
    CGlass::Init();
    CGarages::Init_AfterRestart();

    LoadingScreen("Loading the Game", "Load scripts");
    CTheScripts::Init();
    CGangs::Initialise();

    LoadingScreen("Loading the Game", "Setup game variables");
    CClock::Initialise(1000);
    CHeli::InitHelis();
    CCranes::InitCranes();

    LoadingScreen(nullptr, nullptr);
    CMovingThings::Init();
    CDarkel::Init();

    LoadingScreen(nullptr, nullptr);
    CGridRef::Init();
    CStats::Init();
    CClouds::Init();
    CSpecialFX::Init();

    LoadingScreen(nullptr, nullptr);
    CRopes::Init();
    CWaterCannons::Init();
    CBridge::Init();
    CPedGroups::Init();
    CInformFriendsEventQueue::Init();
    CInformGroupEventQueue::Init();

    LoadingScreen(nullptr, nullptr);
    CCredits::bCreditsGoing = false;
    CReplay::Init();
    CShopping::Init();
    LoadingScreen(nullptr, nullptr);

    return true;
}

// 0x5BA400
bool CGame::Init3(const char* datFile) {
    ZoneScoped;

    LoadingScreen("Loading the Game", "Load scene");
    CPad::GetPad(PED_TYPE_PLAYER1)->Clear(true, true);
    CPad::GetPad(PED_TYPE_PLAYER2)->Clear(true, true);
    D3DResourceSystem::SetUseD3DResourceBuffering(true);
    LoadingScreen("Loading the Game", "Procedural Interiors");
    g_interiorMan.Init();
    g_procObjMan.Init();
    g_waterCreatureMan.Init();
    g_realTimeShadowMan.Init();
    return true;
}

// 0x53BC80
void CGame::Initialise(const char* datFile) {
    ZoneScoped;

    Init1(datFile);
    CColAccel::startCache();
    CFileLoader::LoadLevel("DATA\\DEFAULT.DAT");
    CFileLoader::LoadLevel(datFile);
    CColAccel::endCache();
    Init2(datFile);
    CStencilShadows::Init();
    LoadingScreen("Loading the Game", "Start script");
    CTheScripts::StartTestScript();
    CTheScripts::Process();
    TheCamera.Process();
    Init3(datFile);
}

// 0x5BFA90
bool CGame::InitialiseCoreDataAfterRW() {
    CTempColModels::Initialise();
    gHandlingDataMgr.LoadHandlingData();
    gHandlingDataMgr.field_0 = 0.1f;
    gHandlingDataMgr.fWheelFriction = 0.9f;
    gHandlingDataMgr.field_8 = 1.0f;
    gHandlingDataMgr.field_C = 0.8f;
    gHandlingDataMgr.field_10 = 0.98f;

    g_surfaceInfos.Init();
    CPedStats::Initialise();
    CTimeCycle::Initialise();
    CPopCycle::Initialise();
    CVehicleRecording::InitAtStartOfGame();

    return AudioEngine.Initialise() && g_breakMan.Init() && g_boneNodeMan.Init() && !g_ikChainMan.Init();
}

// 0x5BA160
bool CGame::InitialiseEssentialsAfterRW() {
    CMemoryMgr::PushMemId(MEM_30);
    TheText.Load(false);
    if (!CCarFXRenderer::Initialise() || !CGrassRenderer::Initialise() || !CCustomBuildingRenderer::Initialise()) {
        return false;
    }
    CMemoryMgr::PopMemId();

    CTimer::Initialise();
    return true;
}

// 0x53BB50
void CGame::InitialiseOnceBeforeRW() {
    CMemoryMgr::Init();
    CLocalisation::Initialise();
    CFileMgr::Initialise();
    CdStreamInit(5);
    CPad::Initialise();
}

// 0x5BD600
bool CGame::InitialiseRenderWare() {
    ValidateVersion();
    D3DResourceSystem::Init();
    CTxdStore::Initialise();
    CVisibilityPlugins::Initialise();

    const auto camera = RwCameraCreate();
    if (!camera) {
        CameraDestroy(camera);
        return false;
    }

    const auto frame = RwFrameCreate();
    rwObjectHasFrameSetFrame(&camera->object.object, frame);
    camera->frameBuffer = RwRasterCreate(RsGlobal.maximumWidth, RsGlobal.maximumHeight, 0, rwRASTERTYPECAMERA);
    camera->zBuffer = RwRasterCreate(RsGlobal.maximumWidth, RsGlobal.maximumHeight, 0, rwRASTERTYPEZBUFFER);
    if (!camera->object.object.parent) {
        CameraDestroy(camera);
        return false;
    }

    Scene.m_pRwCamera = camera;
    TheCamera.Init();
    TheCamera.SetRwCamera(Scene.m_pRwCamera);
    RwCameraSetFarClipPlane(Scene.m_pRwCamera, 2000.0f);
    RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.9f);
    CameraSize(Scene.m_pRwCamera, nullptr, 0.7f, DEFAULT_ASPECT_RATIO);

    RwBBox bb;
    bb.sup = { 10'000.0f,  10'000.0f,  10'000.0f};
    bb.inf = {-10'000.0f, -10'000.0f, -10'000.0f};

    if (Scene.m_pRpWorld = RpWorldCreate(&bb); !Scene.m_pRpWorld) {
        CameraDestroy(Scene.m_pRwCamera);
        Scene.m_pRwCamera = nullptr;

        return false;
    }
    RpWorldAddCamera(Scene.m_pRpWorld, Scene.m_pRwCamera);
    LightsCreate(Scene.m_pRpWorld);
    CreateDebugFont();
    CFont::Initialise();
    CHud::Initialise();
    CPlayerSkin::Initialise();
    CPostEffects::Initialise();
    CGame::m_pWorkingMatrix1 = RwMatrixCreate();
    CGame::m_pWorkingMatrix2 = RwMatrixCreate();

    _rwD3D9DeviceSetRestoreCallback(_rwD3D9DeviceGetRestoreCallback());
    return true;
}

// 0x53C680
void CGame::InitialiseWhenRestarting() {
    const auto color = CRGBA(255, 255, 255, 255); // unused

    CTimer::Initialise();
    CEventScanner::m_sDeadPedWalkingTimer = 0;

    if (FrontEndMenuManager.m_bLoadingData) {
        FrontEndMenuManager.MessageScreen("FELD_WR", true, false);
        if (FrontEndMenuManager.m_bLoadingData) {
            CGenericGameStorage::RestoreForStartLoad();
            CStreaming::RemoveBigBuildings();
        }
    }

    ReInitGameObjectVariables();
    CTimeCycle::InitForRestart();
    CWeaponEffects::Init();
    CPlane::InitPlaneGenerationAndRemoval();

    if (FrontEndMenuManager.m_bLoadingData) {
        FrontEndMenuManager.m_bLoadingData = false;
        CGenericGameStorage::InitRadioStationPositionList();
        bool loaded = false;
        if (CGenericGameStorage::GenericLoad(loaded)) {
            CTrain::InitTrains();
        } else {
            for (auto i = 50; i > 0; --i) {
                MessageLoop();
                if (loaded) {
                    FrontEndMenuManager.MessageScreen("FES_LOC", true, false);
                } else {
                    FrontEndMenuManager.MessageScreen("FED_LFL", true, false);
                }
            }
            TheCamera.SetFadeColour(0, 0, 0);
            ShutDownForRestart();
            CTimer::Stop();
            CTimer::Initialise();
            FrontEndMenuManager.m_bLoadingData = false;
            ReInitGameObjectVariables();
            currLevel = 0;
            bMissionPackGame = 0;
            CCollision::SortOutCollisionAfterLoad();
        }
    }
    CTimer::Update();

    AudioEngine.ResetSoundEffects();
    AudioEngine.Restart();
    AudioEngine.SetMusicMasterVolume(FrontEndMenuManager.m_nRadioVolume);
    AudioEngine.SetEffectsMasterVolume(FrontEndMenuManager.m_nSfxVolume);
    AudioEngine.SetBassEnhanceOnOff(FrontEndMenuManager.m_bRadioEq);
    AudioEngine.SetRadioAutoRetuneOnOff(FrontEndMenuManager.m_bRadioAutoSelect);
    AudioEngine.InitialiseRadioStationID(FrontEndMenuManager.m_nRadioStation);
    D3DResourceSystem::SetUseD3DResourceBuffering(true);
}

// 0x53BEE0
void CGame::Process() {
    ZoneScoped;

    CPad::UpdatePads();
    g_LoadMonitor.BeginFrame();

    const auto GetTime = [] { return CTimer::GetCurrentTimeInCycles() / CTimer::GetCyclesPerMillisecond(); };

    const auto timeBeforeStreamingUpdate = GetTime();
    CStreaming::Update();
    auto updateTimeDelta = GetTime() - timeBeforeStreamingUpdate;

    CCutsceneMgr::Update();

    if (!CCutsceneMgr::IsCutsceneProcessing() && !CTimer::GetIsCodePaused()) {
        FrontEndMenuManager.Process();
    }

    CTheZones::Update();
    CCover::Update();
    CAudioZones::Update(false, TheCamera.GetPosition());
    CWindModifiers::Number = 0;

    if (!CTimer::m_UserPause && !CTimer::m_CodePause) {
        CSprite2d::SetRecipNearClip();
        CSprite2d::InitPerFrame();
        CFont::InitPerFrame();
        CCheat::DoCheats();
        CClock::Update();
        CWeather::Update();
        CTheScripts::Process();
        CCollision::Update();
        ThePaths.UpdateStreaming(false);
        CTrain::UpdateTrains();
        CHeli::UpdateHelis();
        CDarkel::Update();
        CSkidmarks::Update();
        CGlass::Update();
        CWanted::UpdateEachFrame();
        CCreepingFire::Update();
        CSetPieces::Update();
        gFireManager.Update();

        if (updateTimeDelta >= 4) {
            CPopulation::Update(false);
        } else {
            const auto timeBeforePopulationUpdate = GetTime();
            CPopulation::Update(true);
            updateTimeDelta = GetTime() - timeBeforePopulationUpdate;
        }
        CWeapon::UpdateWeapons();

        if (!CCutsceneMgr::IsRunning()) {
            CTheCarGenerators::Process();
        }

        if (CReplay::Mode != MODE_PLAYBACK) {
            CCranes::UpdateCranes();
        }

        CClouds::Update();
        CMovingThings::Update();
        CWaterCannons::Update();
        CUserDisplay::Process();
        CReplay::Update();
        CWorld::Process();

        g_LoadMonitor.EndFrame();

        if (!CTimer::bSkipProcessThisFrame) {
            CPickups::Update();
            CCarCtrl::PruneVehiclesOfInterest();
            CGarages::Update();
            CEntryExitManager::Update();
            CStuntJumpManager::Update();
            CBirds::Update();
            CSpecialFX::Update();
            CRopes::Update();
        }
        CPostEffects::Update();
        CTimeCycle::Update();
        CPopCycle::Update();

        if (g_InterestingEvents.m_b1) {
            g_InterestingEvents.ScanForNearbyEntities();
        }

        if (CReplay::ShouldStandardCameraBeProcessed()) {
            TheCamera.Process();
        } else {
            TheCamera.CCamera::ProcessFade();
        }

        CCullZones::Update();
        if (CReplay::Mode != MODE_PLAYBACK) {
            CGameLogic::Update();
            CGangWars::Update();
        }
        CConversations::Update();
        CPedToPlayerConversations::Update();
        CBridge::Update();
        CCoronas::DoSunAndMoon();
        CCoronas::Update();
        CShadows::UpdatePermanentShadows();

        CPlantMgr::Update(TheCamera.GetPosition());
        CCustomBuildingRenderer::Update();

        CStencilShadows::Process(TheCamera.GetPosition());
        if (CReplay::Mode != MODE_PLAYBACK) {
            if (updateTimeDelta < 4) {
                CCarCtrl::GenerateRandomCars();
            }
            CRoadBlocks::GenerateRoadBlocks();
            CCarCtrl::RemoveDistantCars();
            CCarCtrl::RemoveCarsIfThePoolGetsFull();
        }

        g_fx.Update(TheCamera.m_pRwCamera, CTimer::GetTimeStepInSeconds());
        g_breakMan.Update(CTimer::GetTimeStep());
        g_interiorMan.Update();
        g_procObjMan.Update();
        g_waterCreatureMan.Update(CTimer::GetTimeStepInSeconds());
    }
    CWaterLevel::PreRenderWater();
}

// 0x53BCF0
void CGame::ReInitGameObjectVariables() {
    TheCamera.Init();
    CGameLogic::InitAtStartOfGame();
    CGangWars::InitAtStartOfGame();
    CWanted::InitialiseStaticVariables();
    TheCamera.Init();
    TheCamera.SetRwCamera(Scene.m_pRwCamera);
    CSkidmarks::Clear();
    CWeather::Init();
    CCover::Init();
    CUserDisplay::Init();
    CMessages::Init();
    CRestart::Initialise();
    CPostEffects::ScriptResetForEffects();
    CWorld::bDoingCarCollisions = false;
    CHud::ReInitialise();
    CRadar::Initialise();
    CCarCtrl::ReInit();
    ThePaths.ReInit();
    CTimeCycle::Initialise();
    CPopCycle::Initialise();
    CDraw::SetFOV(120.0f);
    CDraw::ms_fLODDistance = 500.0f;
    currArea = AREA_CODE_NORMAL_WORLD;
    CPed::Initialise();
    CWeapon::InitialiseWeapons();
    CPopulation::Initialise();
    rng::for_each(CWorld::Players, [](auto& info) { info.Clear(); });
    CWorld::PlayerInFocus = PED_TYPE_PLAYER1;
    CGlass::Init();
    gbLARiots_NoPoliceCars = false;
    gbLARiots = false;
    CTheScripts::Init();
    CMenuSystem::Initialise();
    CGangs::Initialise();
    CTimer::Initialise();
    CClock::Initialise(1000u);
    CTheCarGenerators::Init();
    CHeli::InitHelis();
    CMovingThings::Init();
    CDarkel::Init();
    CStats::Init();
    CGarages::Init_AfterRestart();
    CRoadBlocks::Init();
    CStreaming::DisableCopBikes(false);
    CSpecialFX::Init();
    CRopes::Init();
    CWaterCannons::Init();
    CDecisionMakerTypesFileLoader::ReStart();
    CVehicleRecording::Init();
    gFireManager.Init();
    g_interiorMan.Init();
    g_procObjMan.Init();
    g_waterCreatureMan.Init();
    g_realTimeShadowMan.Init();
    CStreaming::RemoveInappropriatePedModels();
    AudioEngine.ResetStatistics();
    if (!FrontEndMenuManager.m_bLoadingData) {
        CCranes::InitCranes();
        CTheScripts::StartTestScript();
        CTheScripts::Process();
        TheCamera.Process();
        CTrain::InitTrains();
    }
    CPad::GetPad(PED_TYPE_PLAYER1)->Clear(true, true);
    CPad::GetPad(PED_TYPE_PLAYER2)->Clear(true, true);
}

// 0x53BED0
void CGame::ReloadIPLs() {
    // NOP
}
