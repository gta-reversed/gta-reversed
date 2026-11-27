#include "StdInc.h"

#include "EntryExit.h"
#include "EntryExitManager.h"
#include "TaskComplexGotoDoorAndOpen.h"
#include "TaskSimpleUninterruptable.h"
#include "TaskComplexFollowLeaderInFormation.h"
#include "TaskComplexFacial.h"
#include "Garages.h"
#include "PlayerPed.h"
#include "Object.h"
#include "Interior/InteriorManager_c.h"
#include "Events/EventGroupEvent.h"
#include "Events/EventLeaderEntryExit.h"
#include "Rubbish.h"
#include "Shopping.h"
#include "Clothes.h"
#include "Audio/AudioZones.h"

void CEntryExit::InjectHooks() {
    RH_ScopedClass(CEntryExit);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(GenerateAmbientPeds, 0x43E8B0);
    RH_ScopedInstall(GetEntryExitToDisplayNameOf, 0x43E650);
    RH_ScopedInstall(FindValidTeleportPoint, 0x43EAF0);
    RH_ScopedInstall(IsInArea, 0x43E460);
    RH_ScopedInstall(GetPositionRelativeToOutsideWorld, 0x43EA00, {.Locked = true});
    RH_ScopedInstall(TransitionStarted, 0x43FFD0);
    RH_ScopedInstall(TransitionFinished, 0x4404A0);
    RH_ScopedInstall(RequestObjectsInFrustum, 0x43E690);
    RH_ScopedInstall(RequestAmbientPeds, 0x43E6D0);
    RH_ScopedInstall(WarpGangWithPlayer, 0x43F1F0);
    RH_ScopedInstall(ProcessStealableObjects, 0x43E990);
}

// Code based on 0x43FA00
CEntryExit::CEntryExit(
    CVector center,
    CVector entranceRange,
    float entranceAngleDeg,
    CVector exit,
    float exitAngle,
    eAreaCodes area,
    CEntryExit::eFlags flags,
    int32 skyColor,
    int32 timeOn, int32 timeOff,
    int32 numberOfPeds,
    const char* name
) :
    m_Flags{(uint16)flags}, // Set here, this way we can directly use the flag fields instead of using the enum
    m_EntranceRect{
        center.x - entranceRange.x / 2.f,
        center.y - entranceRange.y / 2.f,
        center.x + entranceRange.x / 2.f,
        center.y + entranceRange.y / 2.f,
    },
    m_ExitPos{ exit + CVector{ 0.f, 0.f, 1.f } },
    m_Area{ area },
    m_NumberOfPeds{ (uint8)numberOfPeds },
    m_ExitAngle{ exitAngle },
    m_SkyColor{ (uint8)skyColor },
    m_EntranceZ{ center.z + 1.f },
    m_EntranceAngleRad{ DegreesToRadians(entranceAngleDeg) }
{
    std::tie(m_TimeOn, m_TimeOff) = [&]() -> std::pair<uint8, uint8> {
        if (bUnknownBurglary && CGeneral::RandomBool(50.f)) {
            return { 0, 0 };
        } else if (bBurglaryAccess) {
            return { 0, 24 };
        } else {
            return { timeOn, timeOff };
        }
    }();

    if (CEntryExitManager::ms_bBurglaryHousesEnabled || !bBurglaryAccess) {
        bEnableAccess = true;
    }

    if (name){
        strcpy_s(m_Name, name);
    } else {
        m_Name[0] = 0;
    }
}

// 0x43EA90
bool IsTeleportPointValid(const CVector& origin, const CVector& target) {
    return !CWorld::TestSphereAgainstWorld(target, 0.35f, nullptr, true, true, true, true, true, false)
        && CWorld::GetIsLineOfSightClear(origin, target, true, true, false, true, true, false, false);
}

// 0x43E8B0
void CEntryExit::GenerateAmbientPeds(const CVector& posn) {
    CPopulation::bInPoliceStation = false;
    if (CGame::currArea == eAreaCodes::AREA_CODE_NORMAL_WORLD) {
        CPopulation::NumberOfPedsInUseInterior = 0;
        float exitAngle = DegreesToRadians(ms_SpawnPoint->m_ExitAngle);
        CPopulation::CreateWaitingCoppers(posn, exitAngle);
    } else {
        constexpr const char* PoliceStationEnExNames[]{ "POLICE1", "POLICE2", "POLICE3", "POLICE4" };
        CPopulation::bInPoliceStation = rng::any_of(PoliceStationEnExNames, [this](auto&& n) { return _stricmp(m_Name, n) == 0; });
        auto numPedsToSpawn = m_Link ? m_Link->m_NumberOfPeds : m_NumberOfPeds;
        CPopulation::NumberOfPedsInUseInterior = numPedsToSpawn;
        CPopulation::PopulateInterior(numPedsToSpawn, posn);
    }
}

// 0x43E650
CEntryExit* CEntryExit::GetEntryExitToDisplayNameOf() {
    if (ms_SpawnPoint->GetArea() == eAreaCodes::AREA_CODE_NORMAL_WORLD || !HasNameSet()) {
        return nullptr;
    }
    // TODO: Probably inlined from `CEntryExitManager`
    const auto& stack = CEntryExitManager::ms_entryExitStack;
    const auto  posn  = CEntryExitManager::ms_entryExitStackPosn;
    if (GetArea() != eAreaCodes::AREA_CODE_NORMAL_WORLD && posn > 1 && stack[posn - 1] == ms_SpawnPoint) {
        return stack[posn - 2];
    }
    return this;
}

// 0x43EA00
// TODO: `_asm` shouldn't be removed, because the caller `EntryExitManager::GetPositionRelativeToOutsideWorld`
// uses it as well (and without reserving `edx` it is going to crash)
void CEntryExit::GetPositionRelativeToOutsideWorld(CVector& outPos) {
    _asm { push edx };

    const auto enex = GetLinkedOrThis(); // this = GetLinkedOrThis();
    if (enex->m_Area != eAreaCodes::AREA_CODE_NORMAL_WORLD) {
        outPos += GetPosition() - enex->m_ExitPos;
    }

    _asm { pop edx };
}

// Return center of entrance rect
CVector CEntryExit::GetPosition() const {
    return CVector{ m_EntranceRect.GetCenter(), m_EntranceZ };
}

CVector2D CEntryExit::GetPosition2D() const {
    return CVector2D{ m_EntranceRect.GetCenter() };
}

// Returns the matrix with which `rectEntrance`'s points were calculated.
CMatrix CEntryExit::GetRectEntranceMatrix() const {
    CMatrix mat;
    mat.SetRotateZ(m_EntranceAngleRad);
    mat.GetPosition() = GetPosition();
    return mat;
}

bool CEntryExit::IsVisibleByTime() const {
    return CClock::GetIsTimeInRange(m_TimeOn, m_TimeOff);
}

// Transforms a point into the entrance rect
CVector CEntryExit::TransformEntrancePoint(const CVector& point) const {
    return GetRectEntranceMatrix().TransformPoint(point - GetPosition());
}

// 0x43EAF0
void CEntryExit::FindValidTeleportPoint(CVector& outTeleportPoint) {
    outTeleportPoint = ms_SpawnPoint->m_ExitPos;

    if (!CWorld::TestSphereAgainstWorld(outTeleportPoint, 0.35f, nullptr, true, true, true, true, true, false)) {
        return;
    }
    
    // Test 2 * 8 spheres around the spawn point, and return whichever
    // doesn't collide with the world and the line of sight between it and `outPoint` is clear
    for (auto r : { 1.25f, 2.f }) { // Test with 2 ranges
        constexpr auto NumTestPoints{ 8 };
        for (auto i = 0; i < NumTestPoints; i++) {
            const auto rot{ (float)i * TWO_PI / (float)NumTestPoints };
            const auto point = outTeleportPoint + CVector{
                std::cos(rot) * r,
                std::sin(rot) * r,
                0.f
            };
            if (IsTeleportPointValid(outTeleportPoint, point)) {
                outTeleportPoint = point;
                return;
            }
        }
    }
}

bool CEntryExit::HasNameSet() const {
    return m_Name[0] != 0;
}

const CRect& CEntryExit::GetEntranceRect() const {
    return m_EntranceRect;
}

uint8 CEntryExit::GetMyOrLinkedArea() const {
    return m_Link ? m_Link->m_Area : m_Area;
}

// 0x43E460
bool CEntryExit::IsInArea(const CVector& position) {

    const auto CheckPointInEntranceRect = [this](const CVector& point) {
        if (m_EntranceRect.IsPointInside(point)) {
            if (std::abs(point.z - m_EntranceZ) < 1.0f) {
                return true;
            }
        }
        return false;
    };

    // It's quite common `m_fEntranceAngle` is 0, so I guess this is some kind of an optimization?
    return CheckPointInEntranceRect(m_EntranceAngleRad == 0.f ? position : TransformEntrancePoint(position));
}

// 0x43FFD0
bool CEntryExit::TransitionStarted(CPed* ped) {
    if (   !bEnableAccess
        || CEntryExitManager::ms_exitEnterState != 0
        || !IsVisibleByTime()
    ) {
        return false;
    }

    // If ped in vehicle check if enex accepts it
    if (ped->bInVehicle) {
        if (ped->m_pVehicle->m_pHandlingData->m_bIsBig) { // Move before the IsAuto or Bike check
            return false; // No big vehicles allowed :(
        }

        switch (ped->m_pVehicle->m_nVehicleType) {
        case eVehicleType::VEHICLE_TYPE_AUTOMOBILE: {
            if (!bCarsAndAircraft) {
                return false;
            }
            break;
        }
        case eVehicleType::VEHICLE_TYPE_BIKE: {
            if (!bBikesAndMotorcycles) {
                return false;
            }
            break;
        }
        default: // Nothing else is disallowed - TODO: Uncomment this, I want my Rhino in CJ's house
            return false;
        }
    } else if (bDisableExit) {
        return false;
    }

    if (m_Link) {
        ms_SpawnPoint = m_Link;
        m_Link->m_Link = this;
    } else {
        ms_SpawnPoint = this;
    }

    const auto spawnPointExitToUs{ ms_SpawnPoint->m_ExitPos - GetPosition() };

    if (!ped->AsPlayer()->CanPlayerStartMission()) {
        return false;
    }

    bEnteredWithoutExit = true;
    ms_Door            = nullptr;

    ped->bCanExitCar    = false;

    const auto AddPedScriptCommand = [ped](auto* task) {
        ped->GetEventGroup().Add(CEventScriptCommand{
            TASK_PRIMARY_PRIMARY,
            task,
            false
        });
    };

    if (bUnknownPairing || bFoodDateFlag || ped->bInVehicle) {
        if (spawnPointExitToUs.SquaredMagnitude() > sq(10.f)) {
            ms_IsWarping = true;
        }
    } else {
        ms_IsWarping = spawnPointExitToUs.SquaredMagnitude() > sq(10.f);

        const auto spawnPointExitToUsDir = spawnPointExitToUs.Normalized();

        const auto SetupFixedCamera = [this](CVector lookAtDir) {
            // 0x44031A
            auto fixedModePos = GetPosition() - lookAtDir * 3.f;
            fixedModePos.z += 1.f;
            TheCamera.SetCamPositionForFixedMode(fixedModePos, {});
            TheCamera.TakeControlNoEntity(GetPosition() + lookAtDir, eSwitchType::JUMPCUT, 1);
        };


        if (const auto door = CEntryExitManager::FindNearestDoor(*this, 10.f)) {
            // 0x4401AB

            AddPedScriptCommand(new CTaskComplexGotoDoorAndOpen(door));
            ms_Door = door;

            SetupFixedCamera(CVector{ CVector2D{door->GetPosition()} - GetPosition2D(), 0.f }.Normalized());
        } else {
            // 0x440246
            if (ms_IsWarping) {
                bUnknownPairing = true;
                return true;
            }
            AddPedScriptCommand(new CTaskComplexGotoDoorAndOpen(GetPosition(), GetPosition() + spawnPointExitToUsDir * 4.f));
        }
    }

    CPad::GetPad()->bPlayerOnInteriorTransition = true;
    if (!ped->bInVehicle) {
        AddPedScriptCommand(new CTaskSimpleUninterruptable);
    }

    return true;
}

// 0x4404A0
bool CEntryExit::TransitionFinished(CPed* ped) {
    CVector spawnPos = ms_SpawnPoint->m_ExitPos;

    if (auto* const held = ped->GetEntityThatThisPedIsHolding()) {
        held->SetAreaCode(ms_SpawnPoint->GetArea());
    }

    const auto AddLeaderEntryExitEvent = [ped] {
        if (auto* const grp = CPedGroups::GetPedsGroup(ped)) {
            grp->GetIntelligence().AddEvent(CEventGroupEvent{ ped, new CEventLeaderEntryExit{ ped } });
        }
    };
    const auto DisplayEnExName = [this] {
        if (const auto* const enex = GetEntryExitToDisplayNameOf()) {
            CHud::SetZoneName(TheText.Get(enex->m_Name), true);
        }
    };

    if (bUnknownPairing || bFoodDateFlag || ped->bInVehicle) {
        switch (CEntryExitManager::ms_exitEnterState) {
        case EXIT_ENTER_STATE_0: { // 0x440AB3
            TheCamera.SetFadeColour(0, 0, 0);
            TheCamera.Fade(0.5f, eFadeFlag::FADE_IN);
            CEntryExitManager::ms_exitEnterState = EXIT_ENTER_STATE_2;
            AddLeaderEntryExitEvent();
            DisplayEnExName();
            return false;
        }
        case EXIT_ENTER_STATE_2: { // 0x440A92
            if (!TheCamera.GetFading()) {
                CEntryExitManager::ms_exitEnterState = EXIT_ENTER_STATE_3;
            }
            return false;
        }
        default: { // 0x44068F
            CGame::currArea                         = ms_SpawnPoint->GetArea();
            CEntryExitManager::ms_numVisibleEntities = 0;
            break;
        }
        }
    } else { // 0x440516
        CColStore::AddCollisionNeededAtPosn(spawnPos);
        CIplStore::AddIplsNeededAtPosn(spawnPos);
        if (ms_IsWarping) { // 0x440536
            CRenderer::SetLoadingPriority(0);
            CStreaming::AddModelsToRequestList(spawnPos, STREAMING_LOADING_SCENE);
            ms_SpawnPoint->RequestObjectsInFrustum();
            ThePaths.SetPathsNeededAtPosition(spawnPos);
        }

        switch (CEntryExitManager::ms_exitEnterState) {
        case EXIT_ENTER_STATE_0: { // 0x4405E8
            CEntryExitManager::SetAreaCodeForVisibleObjects();
            CEntryExitManager::ms_exitEnterState = EXIT_ENTER_STATE_1;
            CGame::currArea                      = ms_SpawnPoint->GetArea();
            AddLeaderEntryExitEvent();
            return false;
        }
        case EXIT_ENTER_STATE_1: { // 0x44059B
            if (!notsa::isa_and_present<CTaskComplexGotoDoorAndOpen>(ped->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_PRIMARY))) {
                TheCamera.SetFadeColour(0, 0, 0);
                TheCamera.Fade(1.f, eFadeFlag::FADE_IN);
                CEntryExitManager::ms_exitEnterState = EXIT_ENTER_STATE_2;
            }
            DisplayEnExName();
            return false;
        }
        case EXIT_ENTER_STATE_2: { // 0x44057A
            if (TheCamera.GetFading()) {
                return false;
            }
            CEntryExitManager::ms_exitEnterState = EXIT_ENTER_STATE_3;
            break;
        }
        default: // 0x4406AD
            break;
        }
    }

    // 0x4406AD - Fade done, finish the transition
    ped->SetAreaCode(CGame::currArea);
    if (ped->IsInVehicle()) {
        ped->m_pVehicle->SetAreaCode(CGame::currArea);
    }
    ped->m_pEnex = CGame::CanSeeOutSideFromCurrArea() ? nullptr : this;
    CEntryExitManager::AddEntryExitToStack(this);
    ped->bCanExitCar                            = true;
    CPad::GetPad()->bPlayerOnInteriorTransition = false;
    CClothes::RebuildPlayerIfNeeded(ped->AsPlayer());

    if (bFoodDateFlag) { // 0x440718
        ms_SpawnPoint->bEnteredWithoutExit = false;
        return true;
    }

    TheCamera.SetPosn(spawnPos);
    TheCamera.RestoreWithJumpCut();
    CAudioZones::Update(true, spawnPos);
    CWaterLevel::FindNearestWaterAndItsFlow();
    CGarages::CloseHideOutGaragesBeforeSave();
    CEntryExitManager::ResetAreaCodeForVisibleObjects();
    g_interiorMan.SetEntryExitPtr(this);
    CPopulation::RemoveAllRandomPeds();
    RequestAmbientPeds();
    CStreaming::LoadAllRequestedModels(false);

    CTimer::Suspend();
    if (CGame::CanSeeOutSideFromCurrArea()) {
        RwCameraSetFarClipPlane(TheCamera.m_pRwCamera, CTimeCycle::FindFarClipForCoors(spawnPos));
    }
    ms_SpawnPoint->RequestObjectsInFrustum();
    CStreaming::LoadScene(spawnPos);
    CStreaming::LoadAllRequestedModels(false);
    GenerateAmbientPeds(spawnPos);
    if (g_interiorMan.Update()) {
        CStreaming::SetLoadVehiclesInLoadScene(false);
        CStreaming::LoadScene(spawnPos);
        CStreaming::SetLoadVehiclesInLoadScene(true);
    }
    CTimer::Resume();
    CStreaming::ClearFlagForAll(STREAMING_LOADING_SCENE);

    if (ms_SpawnPoint->m_SkyColor) {
        CTimeCycle::StartExtraColour(ms_SpawnPoint->m_SkyColor - 1, false);
    } else {
        CTimeCycle::StopExtraColour(false);
    }
    CRubbish::SetVisibility(ms_SpawnPoint->bUnknownInterior);

    const auto exitHeading = DegreesToRadians(ms_SpawnPoint->m_ExitAngle);
    if (auto* const veh = ped->GetVehicleIfInOne()) { // 0x4408C0
        veh->Teleport({ spawnPos.x, spawnPos.y, spawnPos.z - 1.f + veh->GetHeightAboveRoad() }, false);
        veh->SetHeading(exitHeading);
    } else { // 0x440941
        ms_SpawnPoint->FindValidTeleportPoint(spawnPos);
        ped->Teleport(spawnPos, false);
        ped->m_fCurrentRotation = ped->m_fAimingRotation = exitHeading;
        ped->SetHeading(exitHeading);
    }
    TheCamera.Fade(1.f, eFadeFlag::FADE_OUT);
    CTheScripts::ClearSpaceForMissionEntity(ped->GetPosition(), ped);

    if (ms_SpawnPoint->bUsedRewardEntrance) { // 0x4409E5
        CShopping::RemoveLoadedShop();
    } else if (ms_SpawnPoint->bRewardInterior) { // 0x4409F0
        CShopping::LoadShop(m_Name);
        if (m_Link) {
            bUsedRewardEntrance = true;
        }
    }

    CPopulation::ManageAllPopulation();
    CTheScripts::Process();
    if (ms_SpawnPoint->bAcceptNpcGroup) {
        WarpGangWithPlayer(ped->AsPlayer());
    }
    ProcessStealableObjects(ped);
    ms_SpawnPoint->bEnteredWithoutExit = false;
    if (ms_SpawnPoint->bDeleteEnex) {
        CEntryExitManager::DeleteOne(CEntryExitManager::GetPool()->GetIndex(ms_SpawnPoint));
    }
    ped->GetTaskManager().GetTaskSecondaryFacial()->StopAll();
    CGame::TidyUpMemory(true, true);

    return true;
}

// 0x43E6D0
void CEntryExit::RequestAmbientPeds() {
    if (CGame::CanSeeOutSideFromCurrArea()) {
        CStreaming::RemoveDodgyPedsFromRandomSlots();
        return;
    }

    constexpr struct { const char* name; int32 models[TOTAL_LOADED_PEDS]; } mapping[]{
        { 
            "bar1", 
            {MODEL_BFYRI, MODEL_OFYST, MODEL_WFYST, MODEL_WMYST, MODEL_BMYRI, MODEL_BMYST, MODEL_OMOST, MODEL_OMYST} 
        },
        { 
            "strip2", 
            {MODEL_SBFYSTR, MODEL_SWFYSTR, MODEL_INVALID, MODEL_INVALID, MODEL_INVALID, MODEL_INVALID, MODEL_INVALID, MODEL_INVALID} 
        },
        { 
            "LAstrip", 
            {MODEL_VWFYST1, MODEL_VBFYST2, MODEL_VHFYST3, MODEL_INVALID, MODEL_INVALID, MODEL_INVALID, MODEL_INVALID, MODEL_INVALID} 
        },
        { 
            "MAFCAS", 
            {MODEL_BFORI, MODEL_HFYRI, MODEL_OMYRI, MODEL_WMYRI, MODEL_OFYST, MODEL_VHMYELV, MODEL_WMOST, MODEL_BMORI} 
        },
        { 
            "TRICAS", 
            {MODEL_BFYRI, MODEL_BMYRI, MODEL_HFORI, MODEL_HMORI, MODEL_WMORI, MODEL_WFYRI, MODEL_OMOST, MODEL_VBMYELV} 
        },
    };

    for (auto&& [name, models] : mapping) {
        if (!_stricmp(name, m_Name)) {
            CStreaming::StreamPedsIntoRandomSlots(models);
            return;
        }
    }
}
// 0x43E690
void CEntryExit::RequestObjectsInFrustum() const {
    CRenderer::RequestObjectsInDirection(ms_SpawnPoint->m_ExitPos, m_ExitAngle, STREAMING_LOADING_SCENE);
}

// 0x43F1F0
void CEntryExit::WarpGangWithPlayer(CPlayerPed* player) {
    auto& grp = player->GetPlayerGroup();
    if (!grp.IsActive()) {
        return;
    }

    auto& ms = grp.GetMembership();
    if (!ms.IsLeader(player)) {
        return;
    }

    constexpr auto MAX_TRIES_PER_MEMBER = 3;

    const auto& plyrPos = player->GetPosition();
    const auto& offsets = CTaskComplexFollowLeaderInFormation::ms_offsets.Offsets;

    size_t offsetIdx = 0;
    for (auto* const mem : ms.GetFollowers()) {
        if (mem == player || offsetIdx >= offsets.size()) {
            continue;
        }

        // Try the next (at most 3) formation offsets around the player
        std::optional<CVector> teleportTo{};
        for (auto i = 0; i < MAX_TRIES_PER_MEMBER && offsetIdx < offsets.size(); i++) {
            const CVector point = plyrPos + CVector{ offsets[offsetIdx++], 0.f };
            if (IsTeleportPointValid(plyrPos, point)) {
                teleportTo = point;
                break;
            }
        }
        if (!teleportTo) {
            return;
        }

        // Make them face the player
        // NOTE: Not `Heading()`, `GetRadianAngleBetweenPoints` replaces a zero `y` with a small value
        const CVector dirToPlyr = CVector{ CVector2D{ plyrPos - mem->GetPosition() }, 0.f }.Normalized();
        const auto    heading   = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(dirToPlyr.x, dirToPlyr.y, 0.f, 0.f));

        mem->Teleport(*teleportTo, false);
        mem->GetIntelligence()->FlushImmediately(false);
        mem->m_fCurrentRotation = mem->m_fAimingRotation = heading;
        mem->SetHeading(heading);
        mem->SetAreaCode(player->GetAreaCode());
        mem->m_pEnex = player->m_pEnex;
    }
}

// 0x43E990
void CEntryExit::ProcessStealableObjects(CPed* ped) {
    const auto helde = ped->GetEntityThatThisPedIsHolding();
    if (!helde) {
        return;
    }

    if (!helde->GetIsTypeObject()) {
        return; 
    }

    if (!helde->AsObject()->objectFlags.bIsLiftable) {
        return;
    }

    const auto heldobj = helde->AsObject();
    switch (heldobj->m_nObjectType) {
    case OBJECT_MISSION:
    case OBJECT_MISSION2:
        break;
    default: {
        if (ms_SpawnPoint->GetArea() != AREA_CODE_NORMAL_WORLD) {
            if (g_interiorMan.FindStealableObjectId(heldobj) != -1) {
                g_interiorMan.SetStealableObjectStolen(heldobj, false);
            }
        } else {
            g_interiorMan.SetStealableObjectStolen(heldobj, true);
        }
        break;
    }
    }
    heldobj->SetAreaCode(ms_SpawnPoint->GetArea());
}
