#include "StdInc.h"

#include "TheScripts.h"
#include "UpsideDownCarCheck.h"
#include "Lines.h"
#include "FireManager.h"
#include "PedGroups.h"
#include "Checkpoint.h"
#include "Checkpoints.h"
#include "LoadingScreen.h"
#include "Scripted2dEffects.h"
#include "Shadows.h"
#include "VehicleRecording.h"
#include "TaskComplexLeaveAnyCar.h"
#include "TaskComplexWander.h"

#include "extensions/File.hpp"

static inline auto& ScriptsArray = *(std::array<CRunningScript, MAX_NUM_SCRIPTS>*)0xA8B430;

void CTheScripts::InjectHooks() {
    // Has to have these, because there seems to be something going on with the variable init order
    // For now I just changed it to use static addresses, not sure whats going on..
    assert((void*)0xA49960 == (void*)&MainSCMBlock[0]);
    assert((void*)(0xA7A6A0) == (void*)&MissionBlock[0]);

    RH_ScopedClass(CTheScripts);
    RH_ScopedCategory("Scripts");

    RH_ScopedInstall(Init, 0x468D50, {.enabled = false });
    RH_ScopedInstall(InitialiseAllConnectLodObjects, 0x470960);
    RH_ScopedInstall(InitialiseConnectLodObjects, 0x470940);
    RH_ScopedInstall(InitialiseSpecialAnimGroupsAttachedToCharModels, 0x474730);
    RH_ScopedInstall(InitialiseSpecialAnimGroup, 0x474710);
    RH_ScopedInstall(ReadObjectNamesFromScript, 0x486720);
    RH_ScopedInstall(UpdateObjectIndices, 0x486780);
    RH_ScopedInstall(ReadMultiScriptFileOffsetsFromScript, 0x4867C0);
    RH_ScopedInstall(AddScriptCheckpoint, 0x4935A0);
    RH_ScopedInstall(AddScriptEffectSystem, 0x492F90);
    RH_ScopedInstall(AddScriptSearchLight, 0x493000);
    RH_ScopedInstall(AddScriptSphere, 0x483B30);
    RH_ScopedInstall(AddToBuildingSwapArray, 0x481140);
    RH_ScopedInstall(AddToInvisibilitySwapArray, 0x481200);
    RH_ScopedInstall(AddToListOfConnectedLodObjects, 0x470980);
    RH_ScopedInstall(AddToListOfSpecialAnimGroupsAttachedToCharModels, 0x474750);
    RH_ScopedInstall(AddToSwitchJumpTable, 0x470390);
    RH_ScopedInstall(AddToVehicleModelsBlockedByScript, 0x46B200);
    RH_ScopedInstall(AddToWaitingForScriptBrainArray, 0x46AB60);
    RH_ScopedInstall(CleanUpThisObject, 0x4866C0);
    RH_ScopedInstall(CleanUpThisPed, 0x486300);
    RH_ScopedInstall(CleanUpThisVehicle, 0x486670);
    RH_ScopedInstall(ClearAllVehicleModelsBlockedByScript, 0x46A840);
    RH_ScopedInstall(ClearAllSuppressedCarModels, 0x46A7C0);
    RH_ScopedInstall(ClearSpaceForMissionEntity, 0x486B00);
    RH_ScopedInstall(DoScriptSetupAfterPoolsHaveLoaded, 0x5D3390);
    RH_ScopedInstall(GetActualScriptThingIndex, 0x4839A0);
    RH_ScopedInstall(GetNewUniqueScriptThingIndex, 0x483720);
    RH_ScopedInstall(GetScriptIndexFromPointer, 0x464D20);
    RH_ScopedInstall(GetUniqueScriptThingIndex, 0x4810C0);
    RH_ScopedInstall(ReinitialiseSwitchStatementData, 0x470370);
    // RH_ScopedInstall(RemoveFromVehicleModelsBlockedByScript, 0x0);
    RH_ScopedInstall(RemoveFromWaitingForScriptBrainArray, 0x46ABC0);
    RH_ScopedInstall(RemoveScriptCheckpoint, 0x4936C0);
    RH_ScopedInstall(RemoveScriptEffectSystem, 0x492FD0);
    RH_ScopedInstall(RemoveScriptSearchLight, 0x493160);
    RH_ScopedInstall(RemoveScriptSphere, 0x483BA0);
    RH_ScopedInstall(RemoveScriptTextureDictionary, 0x465A40);
    RH_ScopedInstall(RemoveThisPed, 0x486240);
    RH_ScopedOverloadedInstall(StartNewScript, "last-idle", 0x464C20, CRunningScript* (*)(uint8*));
    RH_ScopedOverloadedInstall(StartNewScript, "indexed", 0x464C90, CRunningScript* (*)(uint8*, uint16));
    RH_ScopedInstall(UndoBuildingSwaps, 0x481290);
    RH_ScopedInstall(IsPedStopped, 0x486110);
    RH_ScopedInstall(IsPlayerOnAMission, 0x464D50);
    RH_ScopedInstall(IsVehicleStopped, 0x4861F0);
    RH_ScopedInstall(Load, 0x5D4FD0);
    RH_ScopedInstall(Save, 0x5D4C40);
    RH_ScopedInstall(WipeLocalVariableMemoryForMissionScript, 0x464BB0);
    RH_ScopedInstall(HasCarModelBeenSuppressed, 0x46A810);
    RH_ScopedInstall(HasVehicleModelBeenBlockedByScript, 0x46A890);
    RH_ScopedInstall(StartTestScript, 0x464D40);
    RH_ScopedInstall(Process, 0x46A000);
    RH_ScopedInstall(ProcessAllSearchLights, 0x4939F0);
    RH_ScopedInstall(ProcessWaitingForScriptBrainArray, 0x46CF00);
    RH_ScopedInstall(UndoEntityInvisibilitySettings, 0x4812D0);
    RH_ScopedInstall(PrintListSizes, 0x4646D0);
    RH_ScopedOverloadedInstall(DrawDebugSquare, "", 0x486840, void(*)(float,float,float,float));
    RH_ScopedInstall(DrawDebugAngledSquare, 0x486990);
    // RH_ScopedInstall(DrawDebugCube, 0x0);
    // RH_ScopedInstall(DrawDebugAngledCube, 0x0)
    RH_ScopedInstall(DrawScriptSpritesAndRectangles, 0x464980);
    RH_ScopedInstall(ScriptDebugCircle2D, 0x485C20);
    RH_ScopedInstall(DrawScriptSpheres, 0x4810E0);
    RH_ScopedOverloadedInstall(HighlightImportantArea, "", 0x485E00, void(*)(uint32,float,float,float,float,float));
    RH_ScopedInstall(HighlightImportantAngledArea, 0x485EF0);
    RH_ScopedInstall(ScriptDebugLine3D, 0x485DE0);
    // RH_ScopedInstall(RenderTheScriptDebugLines, 0x0);
    RH_ScopedInstall(RenderAllSearchLights, 0x493E30);
    RH_ScopedInstall(ScriptConnectLodsFunction, 0x470A20);
    RH_ScopedInstall(ScriptAttachAnimGroupToCharModel, 0x474800);
    RH_ScopedInstall(UseSwitchJumpTable, 0x4703C0);
    RH_ScopedInstall(AttachSearchlightToSearchlightObject, 0x4934F0);
    RH_ScopedInstall(CheckStreamedScriptVersion, 0x464FF0);
}

// 0x468D50
void CTheScripts::Init() {
    rng::fill(ScriptSpace, 0u);
    rng::fill(LocalVariablesForCurrentMission, tScriptParam{});

    CRunningScript* nextScript = nullptr;
    for (auto& script : ScriptsArray) {
        script.Init();
        script.m_pPrev = nullptr;
        script.m_pNext = nextScript;

        if (nextScript) {
            nextScript->m_pPrev = &script;
        }
        nextScript = &script;
    }
    pActiveScripts = nullptr;
    pIdleScripts   = nextScript;

    MissionCleanUp.Init();
    UpsideDownCars.Init();
    StuckCars.Init();
    ScriptsForBrains.Init();
    ScriptResourceManager.Initialise();
    rng::for_each(EntitiesWaitingForScriptBrain, &tScriptBrainWaitEntity::Clear);

    if (CGame::bMissionPackGame) {
        char scrFile[MAX_PATH]{};

        while (FrontEndMenuManager.CheckMissionPackValidMenu()) {
            CFileMgr::SetDirMyDocuments();
            notsa::format_to_sz(scrFile, "MPACK//MPACK{:d}//SCR.SCM", CGame::bMissionPackGame);

            if (auto f = notsa::File(scrFile, "rb"); f && f.Read(ScriptSpace.data(), MAIN_SCRIPT_SIZE) >= 1) {
                break;
            }
        }
    } else {
        CFileMgr::SetDir("data\\script");

        VERIFY(notsa::File("main.scm", "rb").Read(ScriptSpace.data(), MAIN_SCRIPT_SIZE) >= 1);
    }
    CFileMgr::SetDir("");

    StoreVehicleIndex     = -1;
    StoreVehicleWasRandom = true;
    OnAMissionFlag        = false;
    LastMissionPassedTime = -1;
    LastRandomPedId       = -1;

    rng::fill(UsedObjectArray, tUsedObject{});
    ReadObjectNamesFromScript();
    UpdateObjectIndices();
    rng::fill(MultiScriptArray, 0u);
    NumberOfUsedObjects                        = 0;
    bAlreadyRunningAMissionScript              = false;
    bUsingAMultiScriptFile                     = true;
    MainScriptSize                             = 0;
    LargestMissionScriptSize                   = 0;
    NumberOfMissionScripts                     = 0;
    NumberOfExclusiveMissionScripts            = 0;
    LargestNumberOfMissionScriptLocalVariables = 0;

    ReadMultiScriptFileOffsetsFromScript();
    if (!CGame::bMissionPackGame) {
        StreamedScripts.ReadStreamedScriptData();
    }

    ForceRandomCarModel                    = -1;
    FailCurrentMission                     = 0;
    ScriptPickupCycleIndex                 = 0;
    bMiniGameInProgress                    = false;
    bDisplayNonMiniGameHelpMessages        = true;
    bPlayerHasMetDebbieHarry               = false;
    RiotIntensity                          = 0;
    bPlayerIsOffTheMap                     = false;
    RadarZoomValue                         = 0;
    RadarShowBlipOnAllLevels               = false;
    HideAllFrontEndMapBlips                = false;
    bDisplayHud                            = true;
    fCameraHeadingWhenPlayerIsAttached     = 0.0f;
    fCameraHeadingStepWhenPlayerIsAttached = 0.0f;
    bEnableCraneRaise                      = true;
    bEnableCraneLower                      = true;
    bEnableCraneRelease                    = true;
    bDrawCrossHair                         = eCrossHairType::NONE;
    gAllowScriptedFixedCameraCollision     = false;
    bAddNextMessageToPreviousBriefs        = true;
    bScriptHasFadedOut                     = false;
    bDrawOddJobTitleBeforeFade             = true;
    bDrawSubtitlesBeforeFade               = true;

    rng::fill(ScriptSphereArray, tScriptSphere{});
    rng::fill(IntroTextLines, tScriptText{});

    NumberOfIntroTextLinesThisFrame = 0;
    UseTextCommands                 = eUseTextCommandState::DISABLED;
    bUseMessageFormatting           = false;
    MessageCentre                   = 0;
    MessageWidth                    = 0;

    rng::fill(IntroRectangles, tScriptRectangle{});
    NumberOfIntroRectanglesThisFrame = 0;

    RemoveScriptTextureDictionary();

    rng::fill(BuildingSwapArray, tBuildingSwap{});
    rng::fill(InvisibilitySettingArray, nullptr);
    ClearAllSuppressedCarModels();
    ClearAllVehicleModelsBlockedByScript();
    InitialiseAllConnectLodObjects();
    InitialiseSpecialAnimGroupsAttachedToCharModels();
    ScriptEffectSystemArray.fill(tScriptEffectSystem{});

    ScriptSearchLightArray.fill(tScriptSearchlight{});
    NumberOfScriptSearchLights = 0;

    ScriptCheckpointArray.fill(tScriptCheckpoint{});
    NumberOfScriptCheckpoints = 0;

    ScriptSequenceTaskArray.fill(tScriptSequence{});

    CScripted2dEffects::Init();
    CTaskSequences::Init();
    CPedGroups::Init();
    CInformFriendsEventQueue::Init();
    CInformGroupEventQueue::Init();
    CDecisionMakerTypes::GetInstance();

}

// 0x470960
void CTheScripts::InitialiseAllConnectLodObjects() {
    for (auto& pair : ScriptConnectLodsObjects) {
        pair = tScriptConnectLodsObject(); // InitialiseConnectLodObjects();
    }
}

// 0x470940
void CTheScripts::InitialiseConnectLodObjects(uint16 index) {
    ScriptConnectLodsObjects[index] = tScriptConnectLodsObject();
}

// 0x474730
void CTheScripts::InitialiseSpecialAnimGroupsAttachedToCharModels() {
    for (auto& group : ScriptAttachedAnimGroups) {
        group = tScriptAttachedAnimGroup(); // InitialiseSpecialAnimGroup();
    }
}

// 0x474710
void CTheScripts::InitialiseSpecialAnimGroup(uint16 index) {
    ScriptAttachedAnimGroups[index] = tScriptAttachedAnimGroup();
}

// 0x486720
void CTheScripts::ReadObjectNamesFromScript() {
    auto* usedObjs = GetSCMChunk<tSCMUsedObjectsChunk>();

    NOTSA_LOG_TRACE("Number of used objects: {}", usedObjs->m_NumberOfUsedObjects);
    NumberOfUsedObjects = usedObjs->m_NumberOfUsedObjects;
    assert(NumberOfUsedObjects < std::size(UsedObjectArray));

    for (auto&& [i, name] : rngv::enumerate(usedObjs->GetObjectNames())) {
        UsedObjectArray[i].nModelIndex = 0; // To be updated via UpdateObjectIndices.
        std::memcpy(UsedObjectArray[i].szModelName, name, sizeof(name));

        NOTSA_LOG_TRACE("Script object #{}: \"{}\"", i, usedObjs->m_UsedObjectNames[i]);
    }
}

// 0x486780
void CTheScripts::UpdateObjectIndices() {
    // First one is ignored because it's empty.
    for (auto& obj : UsedObjectArray | std::views::drop(1)) {
        CModelInfo::GetModelInfo(obj.szModelName, &obj.nModelIndex);
    }
}

// 0x4867C0
void CTheScripts::ReadMultiScriptFileOffsetsFromScript() {
    auto* sfi = GetSCMChunk<tSCMScriptFileInfoChunk>();

    NOTSA_LOG_TRACE("Main script size: {}", sfi->m_MainScriptSize);
    NOTSA_LOG_TRACE("Largest mission size: {}", sfi->m_LargestMissionScriptSize);
    NOTSA_LOG_TRACE("Number of mission scripts: {}", sfi->m_NumberOfMissionScripts);
    NOTSA_LOG_TRACE("Number of exclusive mission script: {}", sfi->m_NumberOfExclusiveMissionScripts);
    NOTSA_LOG_TRACE("Largest num of mission script local vars: {}", sfi->m_LargestNumberOfMissionScriptLocalVars);

    MainScriptSize                             = sfi->m_MainScriptSize;
    LargestMissionScriptSize                   = sfi->m_LargestMissionScriptSize;
    NumberOfExclusiveMissionScripts            = sfi->m_NumberOfExclusiveMissionScripts;
    NumberOfMissionScripts                     = sfi->m_NumberOfMissionScripts;
    LargestNumberOfMissionScriptLocalVariables = sfi->m_LargestNumberOfMissionScriptLocalVars;

    for (const auto&& [i, missionOffset] : rngv::enumerate(sfi->GetMissionOffsets())) {
        MultiScriptArray[i] = missionOffset;
    }
}

// signature changed (CVector)
// 0x4935A0
uint32 CTheScripts::AddScriptCheckpoint(CVector at, CVector pointTo, float radius, eCheckpointType type) {
    const auto cp = rng::find_if(ScriptCheckpointArray, [](auto& cp) { return !cp.IsActive(); });
    assert(cp != ScriptCheckpointArray.end()); // In vanilla game does OOB access.

    cp->m_bUsed = true;
    const auto color = [&type]() -> CRGBA {
        switch (type) {
        case eCheckpointType::TUBE:
        case eCheckpointType::ENDTUBE:
        case eCheckpointType::EMPTYTUBE:
            return { 255, 0, 0, 32 };
        case eCheckpointType::TORUS:
        case eCheckpointType::TORUS_NOFADE:
        case eCheckpointType::TORUSROT:
        case eCheckpointType::TORUSTHROUGH:
        case eCheckpointType::TORUS_UPDOWN:
        case eCheckpointType::TORUS_DOWN:
            return { 255, 0, 0, 96 };
        default:
            NOTSA_UNREACHABLE();
        }
    }();
    const auto index = GetNewUniqueScriptThingIndex(std::distance(ScriptCheckpointArray.begin(), cp), SCRIPT_THING_CHECKPOINT);
    cp->m_Checkpoint = CCheckpoints::PlaceMarker(
        index,
        type,
        at,
        pointTo,
        radius,
        color,
        1024,
        0.075f,
        0
    );

    ++NumberOfScriptCheckpoints;
    return index;
}

// 0x492F90
uint32 CTheScripts::AddScriptEffectSystem(FxSystem_c* system) {
    const auto fx = rng::find_if(ScriptEffectSystemArray, [](auto& fx) {
        return !fx.IsActive();
    });
    assert(fx != ScriptEffectSystemArray.end()); // In vanilla game does OOB access.

    fx->m_bUsed = true;
    fx->m_pFxSystem = system;
    return GetNewUniqueScriptThingIndex(std::distance(ScriptEffectSystemArray.begin(), fx), SCRIPT_THING_EFFECT_SYSTEM);
}

// signature changed (CVector)
// 0x493000
uint32 CTheScripts::AddScriptSearchLight(CVector start, CEntity* entity, CVector target, float targetRadius, float baseRadius) {
    const auto it = rng::find_if(ScriptSearchLightArray, [](auto& ssl) {
        return !ssl.IsActive();
    });
    assert(it != ScriptSearchLightArray.end()); // In vanilla game does OOB access.

    const auto idx = std::distance(ScriptSearchLightArray.begin(), it);
    RemoveScriptSearchLight(idx);

    new (&ScriptSearchLightArray[idx]) tScriptSearchlight{
        .m_bUsed            = true,
        .m_bEnableShadow    = true,
        .m_SomethingFlag    = true,
        .m_Origin           = start,
        .m_Target           = target,
        .m_fTargetRadius    = targetRadius,
        .m_fBaseRadius      = baseRadius,
        .m_AttachedEntity   = entity,
    };
    ++NumberOfScriptSearchLights;

    return CTheScripts::GetNewUniqueScriptThingIndex(idx, SCRIPT_THING_SEARCH_LIGHT);
}

// 0x483B30
uint32 CTheScripts::AddScriptSphere(uint32 id, CVector posn, float radius) {
    const auto sphere = rng::find_if(ScriptSphereArray, [](auto& sphere) {
        return !sphere.IsActive();
    });
    assert(sphere != ScriptSphereArray.end()); // In vanilla game does OOB access.

    const auto idx = std::distance(ScriptSphereArray.begin(), sphere);
    sphere->m_nId     = idx + id;
    sphere->m_vCoords = posn;
    sphere->m_bUsed   = true;
    sphere->m_fRadius = radius;
    return GetNewUniqueScriptThingIndex(idx, SCRIPT_THING_EFFECT_SYSTEM);
}

// 0x481140
void CTheScripts::AddToBuildingSwapArray(CBuilding* building, int32 oldModelId, int32 newModelId) {
    if (building->m_nIplIndex)
        return;

    for (auto& swap : BuildingSwapArray) {
        if (swap.m_pCBuilding == building) {
            if (newModelId == swap.m_nOldModelIndex) {
                swap.Clear();
            } else
                swap.m_nNewModelIndex = newModelId;

            return;
        }
    }

    for (auto& swap : BuildingSwapArray) {
        if (!swap.m_pCBuilding) {
            swap = tBuildingSwap(building, newModelId, oldModelId);
            return;
        }
    }
}

// 0x481200
void CTheScripts::AddToInvisibilitySwapArray(CEntity* entity, bool visible) {
    if (entity->m_nIplIndex)
        return;

    const auto is = rng::find(InvisibilitySettingArray, entity);
    if (is != InvisibilitySettingArray.end()) {
        // Already exists.
        if (visible) {
            *is = nullptr;
        }
        return;
    }

    const auto free = rng::find(InvisibilitySettingArray, nullptr);
    if (free == InvisibilitySettingArray.end()) {
        return;
    }

    *free = entity;
}

// 0x470980
void CTheScripts::AddToListOfConnectedLodObjects(CObject* obj1, CObject* obj2) {
    const auto idx1 = CPools::GetObjectPool()->GetRef(obj1), idx2 = CPools::GetObjectPool()->GetRef(obj2);

    const auto lod = rng::find_if(ScriptConnectLodsObjects, [idx1, idx2](auto& lod) {
        return lod.a == idx1 && lod.b == idx2;
    });
    if (lod != ScriptConnectLodsObjects.end()) {
        // Already exists.
        return;
    }

    const auto free = rng::find_if(ScriptConnectLodsObjects, [](auto& lod) { return lod.a == -1; });
    assert(free != ScriptConnectLodsObjects.end()); // In vanilla game does OOB access.

    free->a = idx1;
    free->b = idx2;
}

// 0x474750
void CTheScripts::AddToListOfSpecialAnimGroupsAttachedToCharModels(int32 modelId, Const char* ifpName) {
    const auto aag = rng::find_if(ScriptAttachedAnimGroups, [modelId, ifpName](auto& aag) {
        return aag.m_nModelID == modelId && !std::strcmp(aag.m_IfpName, ifpName);
    });
    if (aag != ScriptAttachedAnimGroups.end()) {
        // Already exists.
        return;
    }

    const auto free = rng::find_if(ScriptAttachedAnimGroups, [](auto& aag) {
        return aag.m_nModelID == MODEL_INVALID;
    });
    if (free == ScriptAttachedAnimGroups.end()) {
        return;
    }

    free->m_nModelID = modelId;
    assert(std::strlen(ifpName) < sizeof(free->m_IfpName));
    std::strcpy(free->m_IfpName, ifpName);
}

// 0x470390
void CTheScripts::AddToSwitchJumpTable(int32 switchValue, int32 switchLabelLocalAddress) {
    SwitchJumpTable[NumberOfEntriesInSwitchTable].m_nSwitchValue        = switchValue;
    SwitchJumpTable[NumberOfEntriesInSwitchTable].m_nSwitchLabelAddress = switchLabelLocalAddress;
    NumberOfEntriesInSwitchTable++;
}

// 0x46B200, unused | inlined?
void CTheScripts::AddToVehicleModelsBlockedByScript(eModelID modelIndex) {
    if (notsa::contains(VehicleModelsBlockedByScript, modelIndex)) {
        return;
    }

    const auto free = rng::find(VehicleModelsBlockedByScript, MODEL_INVALID);
    assert(free != VehicleModelsBlockedByScript.end()); // In vanilla game does OOB access.

    *free = modelIndex;
}

// 0x46AB60
void CTheScripts::AddToWaitingForScriptBrainArray(CEntity* entity, int16 specialModelIndex) {
    const auto wfsb = rng::find_if(EntitiesWaitingForScriptBrain, [entity](auto& wfsb) {
        return wfsb.m_pEntity == entity;
    });
    if (wfsb != EntitiesWaitingForScriptBrain.end()) {
        // Already exists.
        return;
    }

    const auto free = rng::find_if(EntitiesWaitingForScriptBrain, [entity](auto& wfsb) { return !wfsb.m_pEntity; });
    if (free == EntitiesWaitingForScriptBrain.end()) {
        return;
    }

    free->m_pEntity = entity;
    free->m_ScriptBrainIndex = specialModelIndex;
}

// 0x4934F0
void CTheScripts::AttachSearchlightToSearchlightObject(int32 searchLightId, CObject* tower, CObject* housing, CObject* bulb, CVector offset) {
    const auto idx = GetActualScriptThingIndex(searchLightId, SCRIPT_THING_SEARCH_LIGHT);
    if (idx < 0) {
        return;
    }
    
    assert(IsEntityPointerValid(bulb));
    assert(IsEntityPointerValid(tower));
    assert(IsEntityPointerValid(housing));

    auto& sl = ScriptSearchLightArray[idx];
    sl.m_Origin = offset;
    sl.m_AttachedEntity = nullptr;
    sl.m_Tower = tower;
    sl.m_Housing = housing;
    sl.m_Bulb = bulb;
}

// 0x464FF0
bool CTheScripts::CheckStreamedScriptVersion(RwStream* stream, const char* filename) {
    return true;
}

// 0x4866C0
void CTheScripts::CleanUpThisObject(CObject* obj) {
    if (!obj) {
        return;
    }

    if (obj->IsMissionObject()) {
        obj->m_nObjectType                = OBJECT_TEMPORARY;
        obj->m_nRemovalTime               = CTimer::GetTimeInMS() + 20'000'000;
        obj->m_nRefModelIndex             = -1;
        obj->objectFlags.bChangesVehColor = false;
        CObject::nNoTempObjects++;
    }
}

// 0x486300
// TODO: test
void CTheScripts::CleanUpThisPed(CPed* ped) {
    if (!ped || ped->IsCreatedByMission()) {
        return;
    }

    ped->SetCharCreatedBy(ePedCreatedBy::PED_GAME);
    if (ped->bKeepTasksAfterCleanUp) {
        return;
    }

    notsa::ScopeGuard _([]() {
        --CPopulation::ms_nTotalMissionPeds;
    });

    if (auto* veh = ped->GetVehicleIfInOne(); veh && veh->IsDriver(ped)) {
        const auto FixMission = [veh](eCarMission fix) {
            auto& mis = veh->m_autoPilot.m_nCarMission;
            if (mis != MISSION_PLANE_CRASH_AND_BURN && mis != MISSION_HELI_CRASH_AND_BURN) {
                mis = fix;
            }
        };

        switch (veh->GetType()) {
        case eVehicleType::VEHICLE_TYPE_HELI: {
            FixMission(MISSION_HELI_FLYTOCOORS);

            veh->m_autoPilot.m_vecDestinationCoors = CVector{ 10'000.0f, -10'000.0f, 1'000.0f };
            veh->AsHeli()->m_fMinAltitude          = 1000.0f;
            veh->AsHeli()->m_fMaxAltitude          = 1000.0f;
            break;
        }
        case eVehicleType::VEHICLE_TYPE_PLANE: {
            FixMission(MISSION_PLANE_FLYTOCOORS);

            veh->m_autoPilot.m_vecDestinationCoors = CVector{ 10'000.0f, 10'000.0f, 1'000.0f };
            veh->AsPlane()->m_minAltitude          = 1000.0f;
            veh->AsPlane()->m_maxAltitude          = 1000.0f;
            break;
        }
        default:
            if (veh->IsSubAutomobile() || veh->IsSubBike()) {
                CCarCtrl::JoinCarWithRoadSystem(veh);
                FixMission(MISSION_CRUISE);
            }
        }

        // Quick return: The captain goes down with the ship.
        ped->bStayInSamePlace = false; // ???

        if (auto* group = CPedGroups::GetPedsGroup(ped)) {
            if (auto& member = group->GetMembership(); member.IsFollower(ped)) {
                member.RemoveMember(ped);
            }
        }
        return;
    }
    ped->bStayInSamePlace = false; // ???

    if (auto* group = CPedGroups::GetPedsGroup(ped)) {
        if (auto& member = group->GetMembership(); member.IsFollower(ped)) {
            member.RemoveMember(ped);
        }
    }

    const auto CheckTaskExists = [ped](eTaskType type) {
        if (auto* event = ped->GetEventGroup().GetEventOfType(EVENT_SCRIPT_COMMAND)) {
            if (auto* esc = notsa::dyn_cast<CEventScriptCommand>(event); esc && esc->m_task->GetTaskType() == type) {
                return true;
            }
        }

        if (auto* task = ped->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_PRIMARY); task && task->GetTaskType() == type) {
            return true;
        }

        return false;
    };

    if (ped->IsInVehicle()) {
        if (CheckTaskExists(TASK_COMPLEX_SEQUENCE)) {
            return;
        }

        // Get them out of the car then make them wander.
        ped->GetEventGroup().Add(CEventScriptCommand(TASK_PRIMARY_PRIMARY, new CTaskComplexSequence(
            new CTaskComplexLeaveAnyCar(0, true, false),
            CTaskComplexWander::GetWanderTaskByPedType(ped)
        )));
    } else {
        if (CheckTaskExists(TASK_COMPLEX_WANDER)) {
            return;
        }

        // Make them wander.
        ped->GetEventGroup().Add(CEventScriptCommand(TASK_PRIMARY_PRIMARY, CTaskComplexWander::GetWanderTaskByPedType(ped)));
    }
}

// 0x486670
void CTheScripts::CleanUpThisVehicle(CVehicle* vehicle) {
    if (!vehicle || vehicle->IsCreatedBy(eVehicleCreatedBy::MISSION_VEHICLE)) {
        return;
    }

    vehicle->physicalFlags.bDontApplySpeed        = false;
    vehicle->physicalFlags.bDisableCollisionForce = false;
    vehicle->vehicleFlags.bIsLocked               = false;

    CCarCtrl::RemoveFromInterestingVehicleList(vehicle);
    CVehicleRecording::StopPlaybackRecordedCar(vehicle);
    vehicle->SetVehicleCreatedBy(eVehicleCreatedBy::RANDOM_VEHICLE);
}

// 0x46A840
void CTheScripts::ClearAllVehicleModelsBlockedByScript() {
    rng::fill(VehicleModelsBlockedByScript, MODEL_INVALID);
}

// 0x46A7C0
void CTheScripts::ClearAllSuppressedCarModels() {
    rng::fill(SuppressedVehicleModels, MODEL_INVALID);
}

// 0x486B00
void CTheScripts::ClearSpaceForMissionEntity(const CVector& pos, CEntity* ourEntity) {
    std::array<CEntity*, 16> colEntities{};
    int16                    numColliding{};

    CWorld::FindObjectsKindaColliding(
        pos,
        ourEntity->GetModelInfo()->GetColModel()->GetBoundRadius(),
        false,
        &numColliding,
        (int16)colEntities.max_size(),
        colEntities.data(),
        false,
        true,
        true,
        false,
        false
    );

    auto* ourColData = ourEntity->GetColData();
    if (!ourColData) {
        return;
    }

    // Disable suspension lines of vehicles.
    const auto cdNumLines = std::exchange(ourColData->m_nNumLines, 0);

    for (auto& entity : std::span{ colEntities.data(), (size_t)numColliding }) {
        if (!entity || entity == ourEntity || (entity->IsPed() && entity->AsPed()->IsInVehicle())) {
            continue;
        }

        std::array<CColPoint, 32> colPoints{};
        const auto                numCollisions = CCollision::ProcessColModels(
            ourEntity->GetMatrix(),
            *ourEntity->GetColModel(),
            entity->GetMatrix(),
            *entity->GetColModel(),
            colPoints,
            nullptr,
            nullptr,
            false
        );

        if (numCollisions <= 0) {
            continue;
        }

        if (entity->IsVehicle()) {
            auto* vehicle = entity->AsVehicle();
            if (vehicle->vehicleFlags.bIsLocked || !vehicle->CanBeDeleted()) {
                continue;
            }

            if (auto& driver = vehicle->m_pDriver) {
                CPopulation::RemovePed(driver);
                CEntity::SafeCleanUpRef(driver);
            }

            // Need to use raw pointer here instead of a reference - the m_aPassengers array is modifed in RemovePassenger() and we would crash in RemovePed afterwards
            for (const auto passenger : vehicle->GetPassengers()) {
                if (passenger) {
                    vehicle->RemovePassenger(passenger);
                    CPopulation::RemovePed(passenger);
                }
            }

            CCarCtrl::RemoveFromInterestingVehicleList(vehicle);
            CWorld::Remove(vehicle);
            delete vehicle;
        }

        if (entity->IsPed() && !entity->AsPed()->IsPlayer() && entity->AsPed()->CanBeDeleted()) {
            CPopulation::RemovePed(entity->AsPed());
        }
    }
    ourColData->m_nNumLines = cdNumLines;
}

// 0x5D3390
void CTheScripts::DoScriptSetupAfterPoolsHaveLoaded() {
    for (const auto& lod : ScriptConnectLodsObjects) {
        if (lod.a != -1 && lod.b != -1) {
            ScriptConnectLodsFunction(lod.a, lod.b);
        }
    }
}

// 0x4839A0
int32 CTheScripts::GetActualScriptThingIndex(int32 ref, eScriptThingType type) {
    if (ref == -1) {
        return -1;
    }

    const auto idx = LOWORD(ref), id = HIWORD(ref);

    switch (type) {
    case SCRIPT_THING_SPHERE:
        if (const auto& s = ScriptSphereArray[idx]; s.IsActive() && s.m_nUniqueId == id) {
            return idx;
        }
        break;
    case SCRIPT_THING_EFFECT_SYSTEM:
        if (const auto& fx = ScriptEffectSystemArray[idx]; fx.IsActive() && fx.m_nId == id) {
            return idx;
        }
        break;
    case SCRIPT_THING_SEARCH_LIGHT:
        if (const auto& sl = ScriptSearchLightArray[idx]; sl.IsActive() && sl.m_nId == id) {
            return idx;
        }
        break;
    case SCRIPT_THING_CHECKPOINT:
        if (const auto& cp = ScriptCheckpointArray[idx]; cp.IsActive() && cp.m_nId == id) {
            return idx;
        }
        break;
    case SCRIPT_THING_SEQUENCE_TASK:
        if (const auto& sqt = ScriptSequenceTaskArray[idx]; sqt.IsActive() && sqt.m_nId == id) {
            return idx;
        }
        break;
    case SCRIPT_THING_FIRE:
        if (const auto& f = gFireManager.Get(idx); f.IsScript() && f.GetId() == id) {
            return idx;
        }
        break;
    case SCRIPT_THING_2D_EFFECT:
        if (CScripted2dEffects::ms_activated[idx] && CScripted2dEffects::ScriptReferenceIndex[idx] == id) {
            return idx;
        }
        break;
    case SCRIPT_THING_DECISION_MAKER:
        CDecisionMakerTypes::GetInstance();
        if (CDecisionMakerTypes::m_IsActive[idx] && CDecisionMakerTypes::ScriptReferenceIndex[idx] == id) {
            return idx;
        }
        break;
    case SCRIPT_THING_PED_GROUP:
        if (CPedGroups::ms_activeGroups[idx] && CPedGroups::ScriptReferenceIndex[idx] == id) {
            return idx;
        }
        break;
    default:
        break;
    }
    return -1;
}

// 0x483720
// TODO: TEST REFACTOR!
int32 CTheScripts::GetNewUniqueScriptThingIndex(int32 index, eScriptThingType type) {
    const auto NewUniqueId = [index](auto& id) -> int32 {
        if (id == -1 || id == -2) {
            id = 1;
        } else {
            ++id;
        }

        return (uint32)id << 16 | index;
    };

    switch (type) {
    case eScriptThingType::SCRIPT_THING_SPHERE:
        return NewUniqueId(ScriptSphereArray[index].m_nUniqueId);
    case eScriptThingType::SCRIPT_THING_EFFECT_SYSTEM:
        return NewUniqueId(ScriptEffectSystemArray[index].m_nId);
    case eScriptThingType::SCRIPT_THING_SEARCH_LIGHT:
        return NewUniqueId(ScriptSearchLightArray[index].m_nId);
    case eScriptThingType::SCRIPT_THING_CHECKPOINT:
        return NewUniqueId(ScriptCheckpointArray[index].m_nId);
    case eScriptThingType::SCRIPT_THING_SEQUENCE_TASK:
        ScriptSequenceTaskArray[index].m_bUsed = true;
        return NewUniqueId(ScriptSequenceTaskArray[index].m_nId);
    case eScriptThingType::SCRIPT_THING_FIRE:
        return NewUniqueId(gFireManager.m_aFires[index].GetId());
    case eScriptThingType::SCRIPT_THING_2D_EFFECT:
        return NewUniqueId(CScripted2dEffects::ScriptReferenceIndex[index]);
    case eScriptThingType::SCRIPT_THING_DECISION_MAKER:
        CDecisionMakerTypes::GetInstance(); // ? TODO check if we really need this
        return NewUniqueId(CDecisionMakerTypes::ScriptReferenceIndex[index]);
    case eScriptThingType::SCRIPT_THING_PED_GROUP:
        return NewUniqueId(CPedGroups::ScriptReferenceIndex[index]);
    default:
        break;
    }

    return -1;
}

// 0x464D20
int32 CTheScripts::GetScriptIndexFromPointer(CRunningScript* thread) {
    assert(ScriptsArray.data() <= thread && thread < ScriptsArray.data() + ScriptsArray.size());
    return std::distance(ScriptsArray.data(), thread);
}

/*!
 * @param type always SCRIPT_THING_PED_GROUP
 * @addr 0x4810C0
 */
int32 CTheScripts::GetUniqueScriptThingIndex(int32 playerGroup, eScriptThingType type) {
    switch (type) {
    /* Android
    case SCRIPT_THING_DECISION_MAKER:
        CDecisionMakerTypes::GetInstance();
        return playerGroup | (CDecisionMakerTypes::ScriptReferenceIndex[playerGroup] << 16);
    */
    case SCRIPT_THING_PED_GROUP:
        return playerGroup | (CPedGroups::ScriptReferenceIndex[playerGroup] << 16);
    default:
        return OR_INTERRUPT;
    }
}

// 0x470370
void CTheScripts::ReinitialiseSwitchStatementData() {
    NumberOfEntriesStillToReadForSwitch = 0;
    ValueToCheckInSwitchStatement       = 0;
    SwitchDefaultExists                 = false;
    SwitchDefaultAddress                = 0;
    NumberOfEntriesInSwitchTable        = 0;
}

// unused
// 0x?
void CTheScripts::RemoveFromVehicleModelsBlockedByScript(int32 modelIndex) {
    for (auto& model : VehicleModelsBlockedByScript) {
        if (model == modelIndex) {
            model = MODEL_INVALID;
        }
    }
}

// 0x46ABC0
void CTheScripts::RemoveFromWaitingForScriptBrainArray(CEntity* entity, int16 modelIndex) {
    for (auto& bwe : EntitiesWaitingForScriptBrain) {
        if (bwe.m_pEntity != entity || bwe.m_ScriptBrainIndex != modelIndex) {
            continue;
        }

        bwe.m_pEntity = nullptr;
        bwe.m_ScriptBrainIndex = MODEL_INVALID;
    }
}

// 0x4936C0
void CTheScripts::RemoveScriptCheckpoint(int32 scriptIndex) {
    const auto i = GetActualScriptThingIndex(scriptIndex, eScriptThingType::SCRIPT_THING_CHECKPOINT);
    if (i == -1) {
        return;
    }

    auto& scp = ScriptCheckpointArray[i];
    if (const auto* cp = scp.m_Checkpoint) {
        CCheckpoints::DeleteCP(cp->m_ID, cp->m_Type.get_underlying());
    }
    scp.m_bUsed = false;
    scp.m_nId = 0;

    --NumberOfScriptCheckpoints;
}

// 0x492FD0
void CTheScripts::RemoveScriptEffectSystem(int32 scriptIndex) {
    const auto i = GetActualScriptThingIndex(scriptIndex, eScriptThingType::SCRIPT_THING_EFFECT_SYSTEM);
    if (i == -1) {
        return;
    }

    auto& sef = ScriptEffectSystemArray[i];
    sef.m_bUsed = false;
    sef.m_pFxSystem = nullptr;
}

// 0x493160
void CTheScripts::RemoveScriptSearchLight(int32 scriptIndex) {
    const auto i = GetActualScriptThingIndex(scriptIndex, eScriptThingType::SCRIPT_THING_SEARCH_LIGHT);
    if (i != -1) {
        ScriptSearchLightArray[i] = tScriptSearchlight{};
        --NumberOfScriptSearchLights;
    }
}

// 0x483BA0
void CTheScripts::RemoveScriptSphere(int32 scriptIndex) {
    const auto i = GetActualScriptThingIndex(scriptIndex, eScriptThingType::SCRIPT_THING_SPHERE);
    if (i == -1) {
        return;
    }

    auto& ss = ScriptSphereArray[i];
    ss.m_bUsed = false;
    ss.m_nId   = 0;
}

// 0x465A40
void CTheScripts::RemoveScriptTextureDictionary() {
    rng::for_each(ScriptSprites, &CSprite2d::Delete);
    if (const auto slot = CTxdStore::FindTxdSlot("script"); slot != -1) {
        if (const auto* txd = CTxdStore::ms_pTxdPool->GetAt(slot); txd) {
            CTxdStore::RemoveTxd(slot);
        }
    }
}

// 0x486240
void CTheScripts::RemoveThisPed(CPed* ped) {
    if (!ped) {
        return;
    }

    if (auto* veh = ped->GetVehicleIfInOne()) {
        if (veh->IsDriver(ped)) {
            veh->RemoveDriver(false);

            if (veh->m_nDoorLock == eCarLock::CARLOCK_COP_CAR) {
                veh->m_nDoorLock = eCarLock::CARLOCK_UNLOCKED;
            }

            if (ped->IsCop() && veh->IsLawEnforcementVehicle()) {
                veh->ChangeLawEnforcerState(false);
            }
        } else {
            veh->RemovePassenger(ped);
        }
    }

    const auto isMissionChar = ped->m_nCreatedBy == ePedCreatedBy::PED_MISSION;

    CWorld::RemoveReferencesToDeletedObject(ped);
    delete ped;

    if (isMissionChar) {
        --CPopulation::ms_nTotalMissionPeds;
    }
}

// 0x464C20
CRunningScript* CTheScripts::StartNewScript(uint8* startIP) {
    CRunningScript* script = pIdleScripts;

    script->RemoveScriptFromList(&pIdleScripts);
    script->Init();
    script->SetCurrentIp(startIP);
    script->AddScriptToList(&pActiveScripts);
    script->SetActive(true);

    return script;
}

// 0x464C90
CRunningScript* CTheScripts::StartNewScript(uint8* startIP, uint16 index) {
    if (!pIdleScripts) {
        return nullptr;
    }

    auto* script = pIdleScripts;
    while (script && script != &ScriptsArray[index]) {
        script = script->m_pNext;
    }

    if (!script) {
        return nullptr;
    }

    script->RemoveScriptFromList(&pIdleScripts);
    script->Init();
    script->SetCurrentIp(startIP);
    script->AddScriptToList(&pActiveScripts);
    script->SetActive(true);

    return script;
}

// 0x481290
void CTheScripts::UndoBuildingSwaps() {
    for (auto& swap : BuildingSwapArray) {
        if (swap.m_pCBuilding) {
            swap.m_pCBuilding->ReplaceWithNewModel(swap.m_nOldModelIndex);
            swap.Clear();
        }
    }
}

// 0x486110
bool CTheScripts::IsPedStopped(CPed* ped) {
    if (ped->IsInVehicle()) {
        return CTimer::GetTimeStep() / 100.f >= ped->m_pVehicle->m_fMovingSpeed;
    }
    if (!ped->IsPedStandingInPlace()) {
        return false;
    }
    if (ped->IsPlayer()) {
        if (RpAnimBlendClumpGetAssociation(ped->m_pRwClump, { ANIM_ID_RUN_STOP, ANIM_ID_RUN_STOPR, ANIM_ID_JUMP_LAUNCH, ANIM_ID_JUMP_GLIDE })) {
            return false;
        }
    }
    if (ped->bIsLanding || ped->bIsInTheAir || !ped->bIsStanding) {
        return false;
    }
    if (ped->m_vecAnimMovingShiftLocal.IsZero()) {
        return true;
    }
    return false;
}

// 0x464D50
bool CTheScripts::IsPlayerOnAMission() {
    return OnAMissionFlag && notsa::ReadAs<uint32_t>(&ScriptSpace[OnAMissionFlag]) == 1;
}

// 0x4861F0
bool CTheScripts::IsVehicleStopped(CVehicle* veh) {
    return std::max(CTimer::GetTimeStep(), CTimer::ms_fOldTimeStep) / 100.0f >= veh->m_fMovingSpeed;
}

// 0x5D4FD0
void CTheScripts::Load() {
    Init();

    const auto totalSize = CGenericGameStorage::LoadDataFromWorkBuffer<uint32>();
    auto       p         = ScriptSpace.data();

    // Load chunks
    auto nParts = totalSize / MAX_SAVED_GVAR_PART_SIZE;
    for (nParts; nParts-- > 0; p += MAX_SAVED_GVAR_PART_SIZE) {
        CGenericGameStorage::LoadDataFromWorkBuffer(p, MAX_SAVED_GVAR_PART_SIZE);
    }

    // Load remainder
    const auto remainder = totalSize % MAX_SAVED_GVAR_PART_SIZE;
    CGenericGameStorage::LoadDataFromWorkBuffer(p, remainder);

    for (auto& sfb : ScriptsForBrains.m_aScriptForBrains) {
        CGenericGameStorage::LoadDataFromWorkBuffer(sfb);
    }

    CGenericGameStorage::LoadDataFromWorkBuffer(OnAMissionFlag);
    CGenericGameStorage::LoadDataFromWorkBuffer(LastMissionPassedTime);

    for (auto& bswap : BuildingSwapArray) {
        const auto type    = CGenericGameStorage::LoadDataFromWorkBuffer<ScriptSavedObjectType>();
        const auto poolRef = CGenericGameStorage::LoadDataFromWorkBuffer<uint32>() - 1;
        CGenericGameStorage::LoadDataFromWorkBuffer(bswap.m_nNewModelIndex);
        CGenericGameStorage::LoadDataFromWorkBuffer(bswap.m_nOldModelIndex);

        bswap.m_pCBuilding = nullptr;
        switch (type) {
        case ScriptSavedObjectType::NONE:
        case ScriptSavedObjectType::INVISIBLE:
            break;
        case ScriptSavedObjectType::BUILDING:
            bswap.m_pCBuilding = CPools::GetBuildingPool()->GetAt(poolRef);
            break;
        default:
            NOTSA_UNREACHABLE();
        }

        if (auto* b = bswap.m_pCBuilding) {
            CWorld::Remove(b);
            b->ReplaceWithNewModel(bswap.m_nNewModelIndex);
            CWorld::Add(b);
        }
    }

    for (auto& is : InvisibilitySettingArray) {
        const auto type    = CGenericGameStorage::LoadDataFromWorkBuffer<ScriptSavedObjectType>();
        const auto poolRef = CGenericGameStorage::LoadDataFromWorkBuffer<uint32>() - 1;

        switch (type) {
        case ScriptSavedObjectType::NONE:
            is = nullptr;
            break;
        case ScriptSavedObjectType::INVISIBLE:
            // Not saved by the game, but the logic is still there
            if (is) {
                is->m_bUsesCollision = false;
                is->m_bIsVisible = false;
            }
            break;
        case ScriptSavedObjectType::BUILDING:
            is = nullptr;
            if (auto* obj = CPools::GetBuildingPool()->GetAt(poolRef)) {
                is                   = obj;
                is->m_bUsesCollision = false;
                is->m_bIsVisible     = false;
            }
            break;
        case ScriptSavedObjectType::OBJECT:
            is = nullptr;
            if (auto* obj = CPools::GetObjectPool()->GetAt(poolRef)) {
                is                   = obj;
                is->m_bUsesCollision = false;
                is->m_bIsVisible     = false;
            }
            break;
        case ScriptSavedObjectType::DUMMY:
            is = nullptr;
            if (auto* obj = CPools::GetDummyPool()->GetAt(poolRef)) {
                is                   = obj;
                is->m_bUsesCollision = false;
                is->m_bIsVisible     = false;
            }
            break;
        default:
            NOTSA_UNREACHABLE();
        }
    }

    for (auto& veh : VehicleModelsBlockedByScript) {
        CGenericGameStorage::LoadDataFromWorkBuffer(veh);
    }

    for (auto& lod : ScriptConnectLodsObjects) {
        CGenericGameStorage::LoadDataFromWorkBuffer(lod);
    }

    for (auto& ag : ScriptAttachedAnimGroups) {
        CGenericGameStorage::LoadDataFromWorkBuffer(ag);

        if (ag.m_nModelID != MODEL_INVALID) {
            ScriptAttachAnimGroupToCharModel(ag.m_nModelID, ag.m_IfpName);
        }
    }

    CGenericGameStorage::LoadDataFromWorkBuffer(bUsingAMultiScriptFile);
    CGenericGameStorage::LoadDataFromWorkBuffer(bPlayerHasMetDebbieHarry);

    {
        // Ignored
        CGenericGameStorage::LoadDataFromWorkBuffer<uint32>(); // MainScriptSize
        CGenericGameStorage::LoadDataFromWorkBuffer<uint32>(); // LargestMissionScriptSize
        CGenericGameStorage::LoadDataFromWorkBuffer<uint16>(); // NumberOfMissionScripts
        CGenericGameStorage::LoadDataFromWorkBuffer<uint16>(); // NumberOfExclusiveMissionScripts
        CGenericGameStorage::LoadDataFromWorkBuffer<uint32>(); // LargestNumberOfMissionScriptLocalVariables
    }

    // Unused
    // auto j = 0u;
    // for (auto* s = pActiveScripts; s; s->m_pNext)
    //     j++;

    auto numScripts = CGenericGameStorage::LoadDataFromWorkBuffer<uint32>();
    for (auto i = 0u; i < numScripts; i++) {
        auto* script = StartNewScript(nullptr, CGenericGameStorage::LoadDataFromWorkBuffer<uint16>());
        {
            const auto prev = script->m_pPrev, next = script->m_pNext;
            CGenericGameStorage::LoadDataFromWorkBuffer(*script);
            script->m_pPrev = prev;
            script->m_pNext = next;
        }
        script->SetCurrentIp(&ScriptSpace[CGenericGameStorage::LoadDataFromWorkBuffer<uint32>()]);

        for (auto& stk : script->m_IPStack) {
            if (const auto ip = CGenericGameStorage::LoadDataFromWorkBuffer<uint32>()) {
                stk = &ScriptSpace[ip];
            } else {
                stk = nullptr;
            }
        }
    }
}

// 0x5D4C40
void CTheScripts::Save() {
    const auto totalSize = GetSCMChunk<tSCMGlobalVarChunk>()->m_NextChunkOffset;
    auto       p         = ScriptSpace.data();
    CGenericGameStorage::SaveDataToWorkBuffer(totalSize);

    // Load chunks
    auto nParts = totalSize / MAX_SAVED_GVAR_PART_SIZE;
    for (nParts; nParts-- > 0; p += MAX_SAVED_GVAR_PART_SIZE) {
        CGenericGameStorage::SaveDataToWorkBuffer(p, MAX_SAVED_GVAR_PART_SIZE);
    }

    // Load remainder
    const auto remainder = totalSize % MAX_SAVED_GVAR_PART_SIZE;
    CGenericGameStorage::SaveDataToWorkBuffer(p, remainder);


    for (auto& sfb : ScriptsForBrains.m_aScriptForBrains) {
        CGenericGameStorage::SaveDataToWorkBuffer(sfb);
    }

    CGenericGameStorage::SaveDataToWorkBuffer(OnAMissionFlag);
    CGenericGameStorage::SaveDataToWorkBuffer(LastMissionPassedTime);

    for (auto& bswap : BuildingSwapArray) {
        if (auto* b = bswap.m_pCBuilding) {
            CGenericGameStorage::SaveDataToWorkBuffer(ScriptSavedObjectType::BUILDING);

            // Add 1 to the index because 0 is considered invalid and index can be 0.
            CGenericGameStorage::SaveDataToWorkBuffer(CPools::GetBuildingPool()->GetIndex(b) + 1);
        } else {
            CGenericGameStorage::SaveDataToWorkBuffer(ScriptSavedObjectType::NONE);
            CGenericGameStorage::SaveDataToWorkBuffer(0);
        }

        CGenericGameStorage::SaveDataToWorkBuffer(bswap.m_nNewModelIndex);
        CGenericGameStorage::SaveDataToWorkBuffer(bswap.m_nOldModelIndex);
    }

    for (auto& is : InvisibilitySettingArray) {
        if (!is) {
            CGenericGameStorage::SaveDataToWorkBuffer(ScriptSavedObjectType::NONE);
            CGenericGameStorage::SaveDataToWorkBuffer(0);
            continue;
        }

        switch (is->GetType()) {
        case ENTITY_TYPE_NOTHING:
            CGenericGameStorage::SaveDataToWorkBuffer(ScriptSavedObjectType::NONE);
            CGenericGameStorage::SaveDataToWorkBuffer(0);
            break;
        case ENTITY_TYPE_BUILDING:
            CGenericGameStorage::SaveDataToWorkBuffer(ScriptSavedObjectType::BUILDING);
            CGenericGameStorage::SaveDataToWorkBuffer(CPools::GetBuildingPool()->GetIndex(is->AsBuilding()) + 1);
            break;
        case ENTITY_TYPE_OBJECT:
            CGenericGameStorage::SaveDataToWorkBuffer(ScriptSavedObjectType::OBJECT);
            CGenericGameStorage::SaveDataToWorkBuffer(CPools::GetObjectPool()->GetIndex(is->AsObject()) + 1);
            break;
        case ENTITY_TYPE_DUMMY:
            CGenericGameStorage::SaveDataToWorkBuffer(ScriptSavedObjectType::DUMMY);
            CGenericGameStorage::SaveDataToWorkBuffer(CPools::GetDummyPool()->GetIndex(is->AsDummy()) + 1);
            break;
        default:
            NOTSA_UNREACHABLE();
        }
    }

    for (auto& veh : VehicleModelsBlockedByScript) {
        CGenericGameStorage::SaveDataToWorkBuffer(veh);
    }

    for (auto& lod : ScriptConnectLodsObjects) {
        CGenericGameStorage::SaveDataToWorkBuffer(lod);
    }

    for (auto& ag : ScriptAttachedAnimGroups) {
        CGenericGameStorage::SaveDataToWorkBuffer(ag);
    }

    CGenericGameStorage::SaveDataToWorkBuffer(bUsingAMultiScriptFile);
    CGenericGameStorage::SaveDataToWorkBuffer(bPlayerHasMetDebbieHarry); // ?

    CGenericGameStorage::SaveDataToWorkBuffer(MainScriptSize);
    CGenericGameStorage::SaveDataToWorkBuffer(LargestMissionScriptSize);
    CGenericGameStorage::SaveDataToWorkBuffer(NumberOfMissionScripts);
    CGenericGameStorage::SaveDataToWorkBuffer(NumberOfExclusiveMissionScripts);
    CGenericGameStorage::SaveDataToWorkBuffer(LargestNumberOfMissionScriptLocalVariables);

    auto numNonExternalScripts = 0u;
    auto* lastScript           = pActiveScripts;
    for (auto* s = pActiveScripts; s; s = s->m_pNext) {
        lastScript = s;
        if (!s->m_IsExternal && s->m_ExternalType == -1) {
            numNonExternalScripts++;
        }
    }
    CGenericGameStorage::SaveDataToWorkBuffer(numNonExternalScripts);

    for (auto* s = lastScript; s; s = s->m_pPrev) {
        if (s->m_IsExternal || s->m_ExternalType != -1) {
            continue;
        }

        CGenericGameStorage::SaveDataToWorkBuffer((int16)GetScriptIndexFromPointer(s));
        CGenericGameStorage::SaveDataToWorkBuffer(*s);
        CGenericGameStorage::SaveDataToWorkBuffer((uint32)(s->m_IP - ScriptSpace.data()));

        for (auto& stk : s->m_IPStack) {
            CGenericGameStorage::SaveDataToWorkBuffer((uint32)(stk ? stk - ScriptSpace.data() : 0));
        }
    }
}

// 0x464BB0
void CTheScripts::WipeLocalVariableMemoryForMissionScript() {
    rng::fill(LocalVariablesForCurrentMission, tScriptParam{});
}

// 0x46A810
bool CTheScripts::HasCarModelBeenSuppressed(eModelID carModelId) {
    return notsa::contains(SuppressedVehicleModels, carModelId);
}

// 0x46A890
bool CTheScripts::HasVehicleModelBeenBlockedByScript(eModelID carModelId) {
    return notsa::contains(VehicleModelsBlockedByScript, carModelId);
}

// 0x464D40
void CTheScripts::StartTestScript() {
    ZoneScoped;

    StartNewScript(MainSCMBlock.data());
}

// 0x46A000
void CTheScripts::Process() {
    ZoneScoped;

    if (CReplay::Mode == MODE_PLAYBACK) {
        return;
    }

    CommandsExecuted = 0;

    UpsideDownCars.UpdateTimers();
    StuckCars.Process();
    MissionCleanUp.CheckIfCollisionHasLoadedForMissionObjects();
    DrawScriptSpheres();
    ProcessAllSearchLights();
    ProcessWaitingForScriptBrainArray();

    if (FailCurrentMission) {
        --FailCurrentMission;
    }

    if (UseTextCommands != eUseTextCommandState::DISABLED) {
        rng::fill(IntroTextLines, tScriptText{});
        NumberOfIntroTextLinesThisFrame = 0;

        rng::fill(IntroRectangles, tScriptRectangle{});
        NumberOfIntroRectanglesThisFrame = 0;

        if (UseTextCommands == eUseTextCommandState::DISABLE_NEXT_FRAME) {
            UseTextCommands = eUseTextCommandState::DISABLED;
        }
    } else { // Check these, just in case something modifies them: If so, you can place a 'Break when data changes' breakpoint on the variable
        assert(NumberOfIntroTextLinesThisFrame == 0);
        assert(NumberOfIntroRectanglesThisFrame == 0);
    }

    const auto timeStepMS = (int32)CTimer::GetTimeStepInMS();
    LocalVariablesForCurrentMission[SCRIPT_VAR_TIMERA].iParam += timeStepMS;
    LocalVariablesForCurrentMission[SCRIPT_VAR_TIMERB].iParam += timeStepMS;

    CLoadingScreen::NewChunkLoaded();

    for (CRunningScript* it = pActiveScripts, *next{}; it; it = next) {
        next = it->m_pNext;

        it->m_LocalVars[SCRIPT_VAR_TIMERA].iParam += timeStepMS;
        it->m_LocalVars[SCRIPT_VAR_TIMERB].iParam += timeStepMS;
        it->Process();
    }

    CLoadingScreen::NewChunkLoaded();

    for (auto& ped : CPools::GetPedPool()->GetAllValid()) {
        if (ped.IsCreatedByMission()) {
            ped.GetIntelligence()->RecordEventForScript(0, 0);
        }
    }
}

// 0x4939F0
void CTheScripts::ProcessAllSearchLights() {
    ZoneScoped;

    for (auto& sl : ScriptSearchLightArray) {
        if (!sl.IsActive() || !sl.m_SomethingFlag) {
            continue;
        }

        const auto dir = sl.m_PathCoord2 - sl.m_Target;
        if (dir.SquaredMagnitude() > sq(sl.m_fPathSpeed)) { // Originally part of every case, moved it out here...
            sl.m_Target *= dir.Normalized() * sl.m_fPathSpeed;
        } else {
            switch (sl.m_nCurrentState) {
            case eScriptSearchLightState::STATE_1: {
                sl.m_Target        = sl.m_PathCoord1;
                sl.m_SomethingFlag = true;
                sl.m_nCurrentState = eScriptSearchLightState::STATE_2;
                break;
            }
            case eScriptSearchLightState::STATE_2: {
                sl.m_Target        = sl.m_PathCoord2;
                sl.m_SomethingFlag = true;
                sl.m_nCurrentState = eScriptSearchLightState::STATE_1;
                break;
            }
            case eScriptSearchLightState::STATE_3: {
                sl.m_Target = sl.m_FollowingEntity->GetPosition();
                /* flag is not altered */
                break;
            }
            case eScriptSearchLightState::STATE_4: {
                sl.m_Target        = sl.m_PathCoord1;
                sl.m_fPathSpeed    = 0.0f;
                sl.m_PathCoord1    = CVector{};
                sl.m_SomethingFlag = true;
                sl.m_nCurrentState = eScriptSearchLightState::STATE_0;
                break;
            }
            default:
                NOTSA_UNREACHABLE("Invalid SearchLight State ({})", (int)sl.m_nCurrentState);
            }
        }

        CEntity* bulb = sl.m_Bulb;
        if (!bulb) {
            continue;
        }

        const auto prevPos    = bulb->GetPosition();
        const auto tgtBulbDir = (sl.m_Target - bulb->GetPosition()).Normalized();

        const auto Transform = [&](CEntity* entity) {
            const auto rotX = std::atan2(tgtBulbDir.z, tgtBulbDir.Magnitude2D());
            const auto rotZ = tgtBulbDir.Heading();

            entity->m_matrix->RotateX(rotX);
            entity->m_matrix->RotateZ(rotZ);
            entity->GetPosition() += prevPos;
            entity->UpdateRW();
            entity->UpdateRwFrame();
        };

        Transform(bulb);
        Transform(sl.m_Housing);
    }
}

// 0x46CF00
void CTheScripts::ProcessWaitingForScriptBrainArray() {
    ZoneScoped;

    if (!FindPlayerPed())
        return;

    for (auto& e : EntitiesWaitingForScriptBrain) {
        if (!e.m_pEntity) {
            continue;
        }

        switch (const auto t = ScriptsForBrains.m_aScriptForBrains[e.m_ScriptBrainIndex].m_TypeOfBrain) {
        case 0: // TODO: enum
        case 3: // for peds?
        {
            auto*      ped = e.m_pEntity->AsPed();
            const auto idx = ScriptsForBrains.m_aScriptForBrains[ped->m_StreamedScriptBrainToLoad].m_StreamedScriptIndex;

            if (CStreaming::IsModelLoaded(SCMToModelId(idx))) {
                ScriptsForBrains.StartNewStreamedScriptBrain(
                    static_cast<uint8>(ped->m_StreamedScriptBrainToLoad), // cast?
                    ped,
                    false
                );
            } else {
                CStreaming::RequestModel(SCMToModelId(idx), STREAMING_MISSION_REQUIRED);
            }
            break;
        }
        case 1:
        case 4: // for objects?
        {
            auto* obj = e.m_pEntity->AsObject();

            switch (obj->objectFlags.b0x100000_0x200000) {
            case 1:
                if (!ScriptsForBrains.IsObjectWithinBrainActivationRange(obj, FindPlayerCentreOfWorld()))
                    break;

                [[fallthrough]];
            case 2:
                ScriptsForBrains.StartOrRequestNewStreamedScriptBrain(
                    static_cast<uint8>(obj->m_nStreamedScriptBrainToLoad), // cast?
                    obj,
                    t,
                    false
                );
                break;
            default:
                break;
            }
            break;
        }
        default:
            break;
        }
    }
}

// 0x4812D0
void CTheScripts::UndoEntityInvisibilitySettings() {
    for (auto& is : InvisibilitySettingArray) {
        if (!is) {
            continue;
        }

        is->m_bIsVisible = true;
        is->m_bUsesCollision = true;

        is = nullptr; // Remove from the array.
    }
}

// 0x4703C0
void CTheScripts::UseSwitchJumpTable(int32& switchLabelAddress) {
    switchLabelAddress = 0x0;

    const auto CheckEntryAndJump = [&](tScriptSwitchCase* swtch) {
        if (swtch && ValueToCheckInSwitchStatement != swtch->m_nSwitchValue) {
            return false;
        }

        switchLabelAddress = swtch ? swtch->m_nSwitchLabelAddress : SwitchDefaultAddress;
        ReinitialiseSwitchStatementData();
        return true;
    };

    auto ptr1 = 0u;
    auto ptr2 = NumberOfEntriesInSwitchTable - 1u;
    while (ptr2 - ptr1 > 1) {
        const auto idx = (ptr1 + ptr2) / 2;
        if (CheckEntryAndJump(&SwitchJumpTable[idx])) {
            return;
        }

        if (ValueToCheckInSwitchStatement <= SwitchJumpTable[idx].m_nSwitchValue) {
            ptr2 = idx;
        } else {
            ptr1 = idx;
        }
    }

    if (CheckEntryAndJump(&SwitchJumpTable[ptr2]) || CheckEntryAndJump(&SwitchJumpTable[ptr1])) {
        return;
    }

    CheckEntryAndJump(nullptr); // Jump to the default case
}

// 0x4646D0
void CTheScripts::PrintListSizes() {
    auto active{ 0u }, idle{ 0u };

    for (const auto* s = pActiveScripts; s; s = s->m_pNext) active++;
    for (const auto* s = pIdleScripts; s; s = s->m_pNext) idle++;

    NOTSA_LOG_DEBUG("Scripts Active: {}, Idle: {}", active, idle);
}

uint32 DbgLineColour = 0x0000FFFF; // r = 0, g = 0, b = 255, a = 255

// Usage:
//  const auto pos = FindPlayerPed()->GetPosition() + CVector{ 0.0f, 0.f, 1.0f };
//  CTheScripts::DrawDebugSquare(pos.x + 5.f, pos.y, pos.x, pos.y + 5.f);
// 0x486840
void CTheScripts::DrawDebugSquare(float x1, float y1, float x2, float y2) {
    CColPoint colPoint{};
    CEntity*  colEntity;

    CVector p1 { x1, y1, -1000.0f };
    CWorld::ProcessVerticalLine(p1, 1000.0f, colPoint, colEntity, true, false, false, false, true, false, nullptr);
    p1.z = colPoint.m_vecPoint.z + 2.0f;

    CVector p2 { x2, y1, -1000.0f };
    CWorld::ProcessVerticalLine(p2, 1000.0f, colPoint, colEntity, true, false, false, false, true, false, nullptr);
    p2.z = colPoint.m_vecPoint.z + 2.0f;

    CVector p3 { x2, y2, -1000.0f };
    CWorld::ProcessVerticalLine(p3, 1000.0f, colPoint, colEntity, true, false, false, false, true, false, nullptr);
    p3.z = colPoint.m_vecPoint.z + 2.0f;

    CVector p4 { x1, y2, -1000.0f };
    CWorld::ProcessVerticalLine(p4, 1000.0f, colPoint, colEntity, true, false, false, false, true, false, nullptr);
    p4.z = colPoint.m_vecPoint.z + 2.0f; // FIX_BUGS: missing in original code 🤷

    ScriptDebugLine3D(p1, p2, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D(p2, p3, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D(p3, p4, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D(p4, p1, DbgLineColour, DbgLineColour);
}

// float infX, float infY, float sup.x, float supY, float rotSupX, float rotSupY, float rotInfX, float rotInfY
// 0x486990
void CTheScripts::DrawDebugAngledSquare(const CVector2D& inf, const CVector2D& sup, const CVector2D& rotSup, const CVector2D& rotInf) {
    CColPoint colPoint{};
    CEntity*  colEntity;

    CVector p1 { inf.x, inf.y, -1000.0f };
    CWorld::ProcessVerticalLine(p1, 1000.0f, colPoint, colEntity, true, false, false, false, true, false, nullptr);
    p1.z = colPoint.m_vecPoint.z + 2.0f;

    CVector p2 { sup.x, sup.y, -1000.0f };
    CWorld::ProcessVerticalLine(p2, 1000.0f, colPoint, colEntity, true, false, false, false, true, false, nullptr);
    p2.z = colPoint.m_vecPoint.z + 2.0f;

    CVector p3 { rotSup.x, rotSup.y, -1000.0f };
    CWorld::ProcessVerticalLine(p3, 1000.0f, colPoint, colEntity, true, false, false, false, true, false, nullptr);
    p3.z = colPoint.m_vecPoint.z + 2.0f;

    CVector p4 { rotInf.x, rotInf.y, -1000.0f };
    CWorld::ProcessVerticalLine(p4, 1000.0f, colPoint, colEntity, true, false, false, false, true, false, nullptr);
    p4.z = colPoint.m_vecPoint.z + 2.0f;

    ScriptDebugLine3D(p1, p2, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D(p2, p3, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D(p3, p4, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D(p4, p1, DbgLineColour, DbgLineColour);
}

// (float infX, float infY, float infZ, float supX, float supY, float supZ)
void CTheScripts::DrawDebugCube(const CVector& inf, const CVector& sup) {
    ScriptDebugLine3D({ inf.x, inf.y, inf.z }, { sup.x, inf.y, inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ sup.x, inf.y, inf.z }, { sup.x, sup.y, inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ sup.x, sup.y, inf.z }, { inf.x, sup.y, inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ inf.x, sup.y, inf.z }, { inf.x, inf.y, inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ inf.x, inf.y, sup.z }, { sup.x, inf.y, sup.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ sup.x, inf.y, sup.z }, { sup.x, sup.y, sup.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ sup.x, sup.y, sup.z }, { inf.x, sup.y, sup.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ inf.x, sup.y, sup.z }, { inf.x, inf.y, sup.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ inf.x, inf.y, sup.z }, { inf.x, inf.y, inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ sup.x, inf.y, sup.z }, { sup.x, inf.y, inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ sup.x, sup.y, sup.z }, { sup.x, sup.y, inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ inf.x, sup.y, sup.z }, { inf.x, sup.y, inf.z }, DbgLineColour, DbgLineColour);
}

// (float infX, float infY, float infZ, float supX, float supY, float supZ, float rotSup, float rotSup, float rotInf, float rotInf)
void CTheScripts::DrawDebugAngledCube(const CVector& inf, const CVector& sup, const CVector2D& rotSup, const CVector2D& rotInf) {
    ScriptDebugLine3D({ inf.x,    inf.y,    inf.z    }, { sup.x,    inf.y,    inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ sup.x,    inf.y,    inf.z    }, { rotSup.x, rotSup.y, inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ rotSup.x, rotSup.y, inf.z    }, { rotInf.x, rotInf.y, inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ rotInf.x, rotInf.y, inf.z    }, { inf.x,    inf.y,    inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ inf.x,    inf.y,    sup.z    }, { sup.x,    inf.y,    sup.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ sup.x,    inf.y,    sup.z    }, { rotSup.x, rotSup.y, sup.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ rotSup.x, rotSup.y, rotInf.x }, { rotInf.y, sup.y,    sup.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ rotInf.x, rotInf.y, sup.z    }, { inf.x,    inf.y,    sup.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ inf.x,    inf.y,    sup.z    }, { inf.x,    inf.y,    inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ sup.x,    inf.y,    sup.z    }, { sup.x,    inf.y,    inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ rotSup.x, rotSup.y, sup.z    }, { rotSup.x, rotSup.y, inf.z }, DbgLineColour, DbgLineColour);
    ScriptDebugLine3D({ rotInf.x, rotInf.y, sup.z    }, { rotInf.x, rotInf.y, inf.z }, DbgLineColour, DbgLineColour);
}

// 0x464980
void CTheScripts::DrawScriptSpritesAndRectangles(bool drawBeforeFade) {
    for (const auto& rt : IntroRectangles) {
        if (rt.m_bDrawBeforeFade != drawBeforeFade) {
            continue;
        }
        switch (rt.m_nType) {
        case eScriptRectangleType::INACTIVE:
            break;
        case eScriptRectangleType::TITLE_AND_MESSAGE:
            FrontEndMenuManager.DrawWindowedText(
                rt.cornerA.x, rt.cornerA.y,
                rt.cornerB.x, // ?
                rt.gxt1,
                rt.gxt2,
                rt.m_Alignment
            );
            break;
        case eScriptRectangleType::TEXT:
            FrontEndMenuManager.DrawWindow(
                CRect{ rt.cornerA, rt.cornerB },
                rt.gxt1,
                0,
                CRGBA{ 0, 0, 0, 190 },
                rt.m_nTextboxStyle, // ?
                true
            );
            break;
        case eScriptRectangleType::MONOCOLOR:
            CSprite2d::DrawRect(
                CRect{ rt.cornerA, rt.cornerB },
                rt.m_nTransparentColor
            );
            break;
        case eScriptRectangleType::TEXTURED:
            ScriptSprites[rt.m_nTextureId].Draw(
                CRect{ rt.cornerA, rt.cornerB },
                rt.m_nTransparentColor
            );
            break;
        case eScriptRectangleType::MONOCOLOR_ANGLED: {
            // mid: Vector that points to the middle of line A-B from A.
            // vAM: A to mid.
            const auto mid = (rt.cornerA + rt.cornerB) / 2.0f;
            const auto vAM = mid - rt.cornerA;
            const auto cos = std::cos(rt.m_nAngle);
            const auto sin = std::sin(rt.m_nAngle);

            // This is 2D rotation, couldn't find a better function aside from
            // using matricies or quaternions.
            ScriptSprites[rt.m_nTextureId].Draw(
                -cos * vAM.x + sin * vAM.y + mid.x,
                -sin * vAM.x - cos * vAM.y + mid.y,
                +sin * vAM.y + cos * vAM.x + mid.x,
                +sin * vAM.x - cos * vAM.y + mid.y,
                -cos * vAM.x - sin * vAM.y + mid.x,
                +cos * vAM.y - sin * vAM.x + mid.y,
                +cos * vAM.x - sin * vAM.y + mid.x,
                +sin * vAM.x + cos * vAM.y + mid.y,
                rt.m_nTransparentColor
            );
            break;
        }
        default:
            NOTSA_UNREACHABLE("Unknown script-rect type ({})", (int32)(rt.m_nType));
        }
    }
}

// Usage:
//   const auto pos = FindPlayerPed()->GetPosition() + CVector{ 0.0f, 0.f, 1.0f };
//   CTheScripts::ScriptDebugCircle2D(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 100, 50.f, 50.f, HudColour.GetRGB(HUD_COLOUR_RED).ToInt());
// 0x485C20
void CTheScripts::ScriptDebugCircle2D(float x, float y, float width, float height, CRGBA color) {
    return plugin::Call<0x485C20, float, float, float, float, CRGBA>(x, y, width, height, color);

    // untested
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,     RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));

    const auto iters = 16;
    auto part = (CTimer::GetTimeInMS() >> 6) % iters;
    for (auto i = 0; i < iters; i++) {
        RwIm2DVertex vertex{
            .x = std::sin((float)i * PI / 8.0f) * width + x,
            .y = std::cos((float)i * PI / 8.0f) * height + y
        };
        if (part == i)
            vertex.emissiveColor = (3 * color.g / 4) | (((3 * color.b / 4) | (((3 * color.a / 4) | ((3 * color.r / 4) << 8)) << 8)) << 8); // todo: (3 * color / 4).ToIntARGB();
        else
            vertex.emissiveColor = color.ToIntARGB();
        RwIm2DVertex vertices[2] = { vertex, vertex };
        RwIm2DRenderLine(vertices, std::size(vertices), 0, 1); // todo: RwIm2DRenderLine_BUGFIX
    }

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
}

// 0x4810E0
void CTheScripts::DrawScriptSpheres() {
    ZoneScoped;

    for (auto& ss : ScriptSphereArray) {
        if (!ss.m_bUsed) {
            continue;
        }

        C3dMarkers::PlaceMarkerSet(
            ss.m_nId,
            e3dMarkerType::MARKER3D_CYLINDER,
            ss.m_vCoords,
            ss.m_fRadius,
            255,
            0,
            0,
            228,
            2048,
            0.1f,
            0
        );
    }
}

// 0x485E00
void CTheScripts::HighlightImportantArea(uint32 id, float x1, float y1, float x2, float y2, float z) {
    CVector2D inf, sup;
    if (x1 < x2) {
        inf.x = x1;
        sup.x = x2;
    } else {
        inf.x = x2;
        sup.x = x1;
    }

    if (y1 < y2) {
        inf.y = y1;
        sup.y = y2;
    } else {
        inf.y = y2;
        sup.y = y1;
    }

    CVector center;
    center.x = (inf.x + sup.x) / 2;
    center.y = (inf.y + sup.y) / 2;
    center.z = (z <= MAP_Z_LOW_LIMIT) ? CWorld::FindGroundZForCoord(center.x, center.y) : z;
    CShadows::RenderIndicatorShadow(id, SHADOW_ADDITIVE, nullptr, center, sup.x - center.x, 0.0f, 0.0f, center.y - sup.y, 0);
}

// 0x485EF0
void CTheScripts::HighlightImportantAngledArea(uint32 id, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float z) {
    CVector2D inf, sup;
    float x, y;

    x = (x1 + x2) / 2;
    y = (y1 + y2) / 2;
    sup.x = inf.x = x;
    sup.y = inf.y = y;

    x = (x2 + x3) / 2;
    y = (y2 + y3) / 2;
    inf.x = std::min(inf.x, x);
    sup.x = std::max(sup.x, x);
    inf.y = std::min(inf.y, y);
    sup.y = std::max(sup.y, y);

    x = (x3 + x4) / 2;
    y = (y3 + y4) / 2;
    inf.x = std::min(inf.x, x);
    sup.x = std::max(sup.x, x);
    inf.y = std::min(inf.y, y);
    sup.y = std::max(sup.y, y);

    x = (x4 + x1) / 2;
    y = (y4 + y1) / 2;
    inf.x = std::min(inf.x, x);
    sup.x = std::max(sup.x, x);
    inf.y = std::min(inf.y, y);
    sup.y = std::max(sup.y, y);

    CVector center;
    center.x = (inf.x + sup.x) / 2;
    center.y = (inf.y + sup.y) / 2;
    center.z = (z <= MAP_Z_LOW_LIMIT) ? CWorld::FindGroundZForCoord(center.x, center.y) : z;
    CShadows::RenderIndicatorShadow(id, SHADOW_ADDITIVE, nullptr, center, sup.x - center.x, 0.0f, 0.0f, center.y - sup.y, 0);
}

static uint16 NumScriptDebugLines;
static tStoredLine aStoredLines[MAX_NUM_STORED_LINES];

// 0x485DE0
void CTheScripts::ScriptDebugLine3D(const CVector& start, const CVector& end, uint32 color1, uint32 color2) {
#ifndef FINAL
    if (NumScriptDebugLines >= MAX_NUM_STORED_LINES)
        return;

    auto& line = aStoredLines[NumScriptDebugLines];
    line.vecInf = start;
    line.vecSup = end;
    line.color1 = color1;
    line.color2 = color2;
    NumScriptDebugLines++;
#endif
}

// 0x0 NOP
void CTheScripts::RenderTheScriptDebugLines() {
#ifndef FINAL
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    for (auto& line : std::span{ aStoredLines, NumScriptDebugLines }) {
        CLines::RenderLineWithClipping(line.vecInf, line.vecSup, line.color1, line.color2);
    }
    NumScriptDebugLines = 0;
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
#endif
}

// 0x493E30
void CTheScripts::RenderAllSearchLights() {
    ZoneScoped;

    for (const auto&& [i, light] : rngv::enumerate(ScriptSearchLightArray)) {
        if (!light.IsActive()) {
            continue;
        }

        const auto origin = [&] {
            if (auto e = notsa::coalesce(light.m_AttachedEntity.Get(), light.m_Bulb.Get())) {
                return e->GetMatrix().TransformVector(light.m_Origin) + e->GetPosition();
            }

            return light.m_Origin;
        }();

        CHeli::SearchLightCone(
            i,
            origin,
            light.m_Target,
            light.m_fTargetRadius,
            1.0f,
            light.m_bClipIfColliding,
            light.m_bEnableShadow,
            light.m_TargetSpot,
            light.vf64,
            light.vf70,
            true,
            light.m_fBaseRadius,
            0.0f,
            0.0f,
            1.0f
        );
    }
}

// 0x474800
bool CTheScripts::ScriptAttachAnimGroupToCharModel(int32 modelId, const char* ifpName) {
    auto* mi = CModelInfo::GetModelInfo(modelId);
    if (mi->GetAnimFileIndex() == CAnimManager::GetAnimationBlockIndex(ifpName)) {
        // Already attached?
        return false;
    }
    mi->SetAnimFile(ifpName);
    mi->ConvertAnimFileIndex();
    return true;
}

// 0x470A20
void CTheScripts::ScriptConnectLodsFunction(int32 lodRef1, int32 lodRef2) {
    auto obj1 = CPools::GetObjectPool()->GetAtRef(lodRef1), obj2 = CPools::GetObjectPool()->GetAtRef(lodRef2);

    obj1->m_pLod = obj2;
    ++obj2->m_nNumLodChildren;
    CWorld::Remove(obj2);
    obj2->SetupBigBuilding();
    CWorld::Add(obj2);
}
