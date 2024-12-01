/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include <extensions/enumerate.hpp>

#include "Garages.h"
#include "GangWars.h"
#include "GangWarsSaveStructure.h"
#include "ModelIndices.h"
#include "TaskComplexWander.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexDriveWander.h"
#include "TaskSimpleCarDrive.h"
#include "TaskComplexWanderGang.h"
#include "Hud.h"

void CGangWars::InjectHooks() {
    RH_ScopedClass(CGangWars);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(InitAtStartOfGame, 0x443920);
    RH_ScopedInstall(AddKillToProvocation, 0x443950, { .reversed = false });                  // ?
    RH_ScopedInstall(AttackWaveOvercome, 0x445B30);
    RH_ScopedInstall(CalculateTimeTillNextAttack, 0x443DB0);
    RH_ScopedInstall(CanPlayerStartAGangWarHere, 0x443F80);
    RH_ScopedInstall(CheerVictory, 0x444040);
    RH_ScopedInstall(ClearSpecificZonesToTriggerGangWar, 0x443FF0);
    RH_ScopedInstall(ClearTheStreets, 0x4444B0);
    RH_ScopedInstall(CreateAttackWave, 0x444810, { .reversed = false });                   //
    RH_ScopedInstall(CreateDefendingGroup, 0x4453D0);
    RH_ScopedInstall(DoesPlayerControlThisZone, 0x443AE0);
    RH_ScopedInstall(DoStuffWhenPlayerVictorious, 0x446400);
    RH_ScopedInstall(DontCreateCivilians, 0x4439C0);
    RH_ScopedInstall(EndGangWar, 0x4464C0);
    RH_ScopedInstall(GangWarFightingGoingOn, 0x443AC0);
    RH_ScopedInstall(GangWarGoingOn, 0x443AA0);
    RH_ScopedInstall(MakeEnemyGainInfluenceInZone, 0x445FD0);          //
    RH_ScopedInstall(MakePlayerGainInfluenceInZone, 0x445E80);
    RH_ScopedInstall(PedStreamedInForThisGang, 0x4439D0);
    RH_ScopedInstall(PickStreamedInPedForThisGang, 0x443A20);          // +-
    RH_ScopedInstall(PickZoneToAttack, 0x443B00);                      //
    RH_ScopedInstall(ReleaseCarsInAttackWave, 0x445E20);
    RH_ScopedInstall(ReleasePedsInAttackWave, 0x445C30);
    RH_ScopedInstall(SetGangWarsActive, 0x446570);
    RH_ScopedInstall(SetSpecificZoneToTriggerGangWar, 0x444010);
    RH_ScopedInstall(StartDefensiveGangWar, 0x444300);
    RH_ScopedInstall(StartOffensiveGangWar, 0x446050);                 //
    RH_ScopedInstall(StrengthenPlayerInfluenceInZone, 0x445F50);
    RH_ScopedInstall(SwitchGangWarsActive, 0x4465F0);
    RH_ScopedInstall(TellGangMembersTo, 0x444530);                     // ?
    RH_ScopedInstall(TellStreamingWhichGangsAreNeeded, 0x443D50);
    RH_ScopedInstall(Update, 0x446610, { .reversed = false });                             //
    RH_ScopedInstall(UpdateTerritoryUnderControlPercentage, 0x443DE0); //
    RH_ScopedInstall(Load, 0x5D3EB0, {.enabled = LOAD_HOOKS_ENABLED });
    RH_ScopedInstall(Save, 0x5D5530, {.enabled = SAVE_HOOKS_ENABLED });
}

// 0x5D3EB0
bool CGangWars::Load() {
    size_t size;
    CGangWarsSaveStructure data{};
    CGenericGameStorage::LoadDataFromWorkBuffer(&size, sizeof(size_t));
    CGenericGameStorage::LoadDataFromWorkBuffer(&data, sizeof(CGangWarsSaveStructure));
    data.Extract();
    return true;
}

// 0x5D5530
bool CGangWars::Save() {
    size_t size = sizeof(CGangWarsSaveStructure);
    CGangWarsSaveStructure data{};
    data.Construct();
    CGenericGameStorage::SaveDataToWorkBuffer(&size, sizeof(size_t));
    CGenericGameStorage::SaveDataToWorkBuffer(&data, sizeof(CGangWarsSaveStructure));
    return true;
}

// 0x443920
void CGangWars::InitAtStartOfGame() {
    ZoneScoped;

    State               = NOT_IN_WAR;
    State2              = NO_ATTACK;
    NumSpecificZones    = 0;
    Provocation         = 0.0f;
    bGangWarsActive     = false;
    bIsPlayerOnAMission = false;
}

// 0x443950
void CGangWars::AddKillToProvocation(ePedType pedType) {
    if (!bCanTriggerGangWarWhenOnAMission && CTheScripts::IsPlayerOnAMission())
        return;

    if (pedType != PED_TYPE_GANG1 && pedType != PED_TYPE_GANG3)
        return;

    if (NumSpecificZones == 0) {
        Provocation += 1.0f;
        return;
    }

    for (auto& zone : std::span { aSpecificZones.data(), (size_t)NumSpecificZones }) {
        auto zoneInfo = CTheZones::GetZoneInfo(CTheZones::GetNavigationZone(zone));

        if (zoneInfo->GangDensity[pedType - PED_TYPE_GANG1] != 0) {
            Provocation += 1.0f;
            return;
        }
    }
}

// 0x445B30
bool CGangWars::AttackWaveOvercome() {
    auto pedsNearPlayer = 0u, pedsLiving = 0u;

    for (auto i = 0; i < GetPedPool()->GetSize(); i++) {
        auto ped = GetPedPool()->GetAt(i);
        if (!ped || !ped->bPartOfAttackWave)
            continue;

        if (ped->IsStateDying()) {
            ped->bDonePositionOutOfCollision = false;
            ped->bPartOfAttackWave = false;
            ped->SetCharCreatedBy(PED_GAME);
            continue;
        }

        pedsLiving++;
        if (DistanceBetweenPoints2D(ped->GetPosition2D(), FindPlayerCoors()) < 45.0f) {
            pedsNearPlayer++;
        }
    }

    return pedsLiving <= 1 && pedsNearPlayer == 0;
}

// 0x443DB0
float CGangWars::CalculateTimeTillNextAttack() {
    return CGeneral::GetRandomNumberInRange(648'000.0f, 1'620'000.0f); // todo: convert to human readable time HH::MM::SS
}

// 0x443F80
bool CGangWars::CanPlayerStartAGangWarHere(CZoneInfo* zoneInfo) {
    if (bTrainingMission)
        return zoneInfo == &CTheZones::ZoneInfoArray[ZoneInfoForTraining];

    if (NumSpecificZones == 0)
        return true;

    // inline?
    for (auto& zone : std::span{ CTheZones::NavigationZoneArray, (size_t)NumSpecificZones }) {
        if (zoneInfo == CTheZones::GetZoneInfo(&zone)) {
            return true;
        }
    }

    return false;
}

// 0x444040
void CGangWars::CheerVictory() {
    auto& playerGroup = FindPlayerPed()->GetPlayerGroup();

    CPed* nearestMember = nullptr;
    playerGroup.FindDistanceToNearestMember(&nearestMember);
    if (!nearestMember) {
        return;
    }

    static const auto mapping = notsa::make_mapping<notsa::ci_string_view, eGlobalSpeechContext>({
        {"CHC",  CTX_GLOBAL_TAKE_TURF_LAS_COLINAS      },
        {"LFL",  CTX_GLOBAL_TAKE_TURF_LOS_FLORES       },
        {"EBE",  CTX_GLOBAL_TAKE_TURF_EAST_BEACH       },
        {"ELF",  CTX_GLOBAL_TAKE_TURF_EAST_LS          },
        {"JEF",  CTX_GLOBAL_TAKE_TURF_JEFFERSON        },
        {"GLN",  CTX_GLOBAL_TAKE_TURF_GLEN_PARK        },
        {"IWD",  CTX_GLOBAL_TAKE_TURF_IDLEWOOD         },
        {"GAN",  CTX_GLOBAL_TAKE_TURF_GANTON           },
        {"LMEX", CTX_GLOBAL_TAKE_TURF_LITTLE_MEXICO    },
        {"LIND", CTX_GLOBAL_TAKE_TURF_WILLOWFIELD      },
        {"PLS",  CTX_GLOBAL_TAKE_TURF_PLAYA_DEL_SEVILLE},
        {"SUN",  CTX_GLOBAL_TAKE_TURF_TEMPLE           },
    });
    nearestMember->Say(notsa::find_value(mapping, pZoneToFightOver->m_TextLabel));
}

// 0x443FF0
void CGangWars::ClearSpecificZonesToTriggerGangWar() {
    NumSpecificZones = 0;
    CTheZones::FillZonesWithGangColours(false);
}

// 0x4444B0
void CGangWars::ClearTheStreets() {
    for (auto i = 0; i < GetPedPool()->GetSize(); i++) {
        auto ped = GetPedPool()->GetAt(i);
        if (!ped || ped->IsPlayer() || !ped->IsCivilian())
            continue;

        if (auto task = ped->GetTaskManager().Find<CTaskComplexWander>()) {
            task->m_nMoveState = PEDMOVE_SPRINT;
        }
    }
}

// 0x444810
bool CGangWars::CreateAttackWave(int32 warFerocity, int32 waveID) {
    return plugin::CallAndReturn<bool, 0x444810, int32, int32>(warFerocity, waveID);
}

// 0x4453D0
bool CGangWars::CreateDefendingGroup(int32 unused) {
    if (!PedStreamedInForThisGang(static_cast<eGangID>(Gang1))) {
        auto group = CPopulation::GetGangGroupId(static_cast<eGangID>(Gang1));
        CStreaming::RequestModel(CPopulation::GetPedGroupModelId(group, 0), STREAMING_KEEP_IN_MEMORY);

        return false;
    }

    if (!ThePaths.AreNodesLoadedForArea(PointOfAttack.x, PointOfAttack.x, PointOfAttack.y, PointOfAttack.y))
        return false;

    auto node = ThePaths.FindNodeClosestToCoors(PointOfAttack, PATH_TYPE_VEH, 400.0f, 0, 0, 0, 0, 1);
    if (!node.IsValid())
        return false;

    auto nodePos = ThePaths.GetPathNode(node)->GetPosition();
    auto playerPos = FindPlayerCoors();
    if (DistanceBetweenPoints2D(playerPos, nodePos) <= 40.0f)
        return false;

    auto pedCount = static_cast<uint32>(10.0f * (0.4f * Difficulty + 0.6f));

    int32 outPedId;
    for (auto i = 0u; i < pedCount; i++) {
        if (!PickStreamedInPedForThisGang(static_cast<eGangID>(Gang1), outPedId))
            continue;

        auto angle = (float)i * TWO_PI / (float)pedCount;

        auto pedPos = CVector{
            nodePos.x + std::sin(angle) * CGeneral::GetRandomNumberInRange(0.0f, 3.0f) + 2.0f,
            nodePos.y + std::cos(angle) * CGeneral::GetRandomNumberInRange(0.0f, 3.0f) + 2.0f,
            nodePos.z + 2.0f
        };
        pedPos.z = CWorld::FindGroundZFor3DCoord(pedPos, nullptr, nullptr);

        auto ped = new CCivilianPed(static_cast<ePedType>(Gang1 + 7), outPedId);
        ped->SetPosn(pedPos);
        ped->m_fAimingRotation = angle;
        ped->m_fCurrentRotation = angle;
        ped->SetHeading(angle);
        ped->SetCharCreatedBy(PED_MISSION);
        CWorld::Add(ped);

        auto task = new CTaskComplexKillPedOnFoot(FindPlayerPed(), -1, 0, 0, 0, 2);
        CEventScriptCommand event(TASK_PRIMARY_PRIMARY, task, false);
        ped->GetEventGroup().Add(&event);

        ped->SetWeaponAccuracy(static_cast<uint8_t>(90.0f - Difficulty * 60.0f));
        ped->GetAcquaintance().SetAsAcquaintance(ACQUAINTANCE_HATE, CPedType::GetPedFlag(PED_TYPE_PLAYER1));

        eWeaponType weaponToGive = [&i]{
            switch (i % 4) {
            case 0: return WEAPON_PISTOL;
            case 1: return WEAPON_MICRO_UZI;
            case 2: return WEAPON_SHOTGUN;
            case 3: return WEAPON_MP5;
            default: NOTSA_UNREACHABLE("suppress warning"); return WEAPON_DILDO1;
            }
        }();

        ped->GiveDelayedWeapon(weaponToGive, 5000);
        ped->SetCurrentWeapon(weaponToGive);
        ped->bPartOfAttackWave = true;
        ped->bClearRadarBlipOnDeath = true;

        auto color = GetGangColor(Gang1); // 0x44582F
        auto blip = CRadar::SetEntityBlip(BLIP_CHAR, CPools::GetPedRef(ped), color, BLIP_DISPLAY_BLIPONLY);
        CRadar::ChangeBlipScale(blip, 2);
        CRadar::ChangeBlipColour(blip, color);
    }

    for (auto i = 0; i < 3; i++) { // todo: magic number
        auto carNode = ThePaths.FindNthNodeClosestToCoors(PointOfAttack, 0, 100.0f, false, false, i, false, true, nullptr);
        auto carNodePos = ThePaths.GetPathNode(carNode)->GetPosition();

        if (DistanceBetweenPoints2D(carNodePos, playerPos) <= 25.0f)
            continue;

        auto gangCarModel = CPopulation::PickGangCar(Gang1);
        if (gangCarModel != MODEL_INVALID && CStreaming::GetInfo(gangCarModel).IsLoaded()) {
            auto fwd = carNodePos - nodePos;
            fwd.z = 0.0f;
            fwd.Normalise();

            auto car = CCarCtrl::CreateCarForScript(gangCarModel, carNodePos, false);
            car->m_matrix->GetForward() = fwd;
            car->m_matrix->GetRight() = CVector{fwd.y, -fwd.x, 0.0f};

            car->AsAutomobile()->PlaceOnRoadProperly();
            car->vehicleFlags.bIsLocked = false;
            car->SetVehicleCreatedBy(MISSION_VEHICLE);
            car->vehicleFlags.bPartOfAttackWave = true;
        }
    }

    auto pickupCoors = CVector{
        CGeneral::GetRandomNumberInRange(0.0f, 4.0f) + nodePos.x - 2.0f,
        CGeneral::GetRandomNumberInRange(0.0f, 4.0f) + nodePos.y - 2.0f,
        nodePos.z + 1.0f
    };
    pickupCoors.z = CWorld::FindGroundZFor3DCoord(pickupCoors, nullptr, nullptr) + 0.75f;

    ModelIndex pickupModel = ModelIndices::MI_PICKUP_BODYARMOUR;
    if (CGeneral::GetRandomNumberInRange(0, 2) == 1) {
        pickupModel = ModelIndices::MI_PICKUP_HEALTH;
    }

    CPickups::GenerateNewOne(pickupCoors, pickupModel, PICKUP_ONCE_TIMEOUT_SLOW, 0, 0, false, nullptr);
    return true;
}

// 0x446400
void CGangWars::DoStuffWhenPlayerVictorious() {
    ReleasePedsInAttackWave(true, false);
    ReleaseCarsInAttackWave();
    CheerVictory();
    State = NOT_IN_WAR;
    CMessages::AddMessageQ(TheText.Get("GW_YRS"), 4500, 1, true);
    CMessages::AddToPreviousBriefArray(TheText.Get("GW_YRS"), -1, -1, -1, -1, -1, -1, nullptr);
    Provocation = 0.0f;
    TellGangMembersTo(true);
    CStats::IncrementStat(STAT_RESPECT, 45.0f);
    CTheZones::FillZonesWithGangColours(false);

    TimeTillNextAttack = std::min(TimeTillNextAttack - 240'000.0f, 30'000.0f);
}

// 0x443AE0
bool CGangWars::DoesPlayerControlThisZone(CZoneInfo* zoneInfo) {
    auto enemyDensity = zoneInfo->GangDensity[GANG_BALLAS] + zoneInfo->GangDensity[GANG_VAGOS];
    auto groveDensity = zoneInfo->GangDensity[GANG_GROVE];
    return groveDensity != 0 && groveDensity >= enemyDensity;
}

// 0x4439C0
bool CGangWars::DontCreateCivilians() {
    return State != NOT_IN_WAR;
}

// 0x4464C0
void CGangWars::EndGangWar(bool end) {
    State = NOT_IN_WAR;

    if (State2 == WAR_NOTIFIED) {
        State2 = NO_ATTACK;
        TimeTillNextAttack = CalculateTimeTillNextAttack();

        auto releasedPeds = ReleasePedsInAttackWave(true, false);
        MakeEnemyGainInfluenceInZone(Gang1, 3 * releasedPeds);
    }

    Provocation = 0.0f;
    CTheZones::FillZonesWithGangColours(false);
    ReleasePedsInAttackWave(true, end);
    ReleaseCarsInAttackWave();
}

// 0x443AC0
bool CGangWars::GangWarFightingGoingOn() {
    return State != NOT_IN_WAR || State2 == PLAYER_CAME_TO_WAR;
}

// 0x443AA0
bool CGangWars::GangWarGoingOn() {
    return State != NOT_IN_WAR || State2 == WAR_NOTIFIED;
}

// 0x445FD0
void CGangWars::MakeEnemyGainInfluenceInZone(int32 gangId, int32 density) {
    if (!pZoneInfoToFightOver)
        return;

    auto totalGangDensity = pZoneInfoToFightOver->GangDensity[GANG_BALLAS]
        + pZoneInfoToFightOver->GangDensity[GANG_GROVE]
        + pZoneInfoToFightOver->GangDensity[GANG_VAGOS];

    if (!totalGangDensity)
        return;

    pZoneInfoToFightOver->GangDensity[gangId] += density;

    if (!DoesPlayerControlThisZone(pZoneInfoToFightOver)) {
        CStats::IncrementStat(STAT_TERRITORIES_LOST, 1.0f);
    }
}

// 0x445E80
bool CGangWars::MakePlayerGainInfluenceInZone(float removeMult) {
    bool controlledBefore = DoesPlayerControlThisZone(pZoneInfoToFightOver);
    uint8 totalEnemyDensity = 0u;

    // for all gangs except grove
    static constexpr eGangID gangs[] = {
        GANG_BALLAS, GANG_VAGOS, GANG_RIFA, GANG_DANANGBOYS, GANG_MAFIA, GANG_TRIAD, GANG_AZTECAS,
        GANG_UNUSED1, GANG_UNUSED2
    };
    for (auto gang : gangs) {
        auto& density = pZoneInfoToFightOver->GangDensity[gang];
        auto densityInitial = density;

        density = static_cast<uint8>((1.0f - removeMult) * (float)density);
        if (density < 4u) {
            density = 0u;
        }
        pZoneInfoToFightOver->GangDensity[GANG_GROVE] += densityInitial - density;
        totalEnemyDensity += density;
    }

    if (!controlledBefore && DoesPlayerControlThisZone(pZoneInfoToFightOver)) {
        CStats::IncrementStat(STAT_TERRITORIES_TAKEN_OVER, 1.0f);
    }

    return totalEnemyDensity == 0u;
}

// 0x4439D0
bool CGangWars::PedStreamedInForThisGang(eGangID gangId) {
    for (auto midx : CPopulation::GetModelsInPedGroup(CPopulation::GetGangGroupId(gangId))) {
        if (CStreaming::IsModelLoaded(midx)) {
            return true;
        }
    }
    return false;
}

// 0x443A20
bool CGangWars::PickStreamedInPedForThisGang(eGangID gangId, int32& outPedId) {
    auto groupId = CPopulation::GetGangGroupId(gangId);
    auto numPeds = CPopulation::GetNumPedsInGroup(groupId);
    if (groupId <= 0)
        return false;

    auto& x = *reinterpret_cast<int32*>(&CGarages::aCarsInSafeHouse[0][0].m_vPosn.x); // ?
    for (auto i = 0; i < numPeds; i++) {
        x = (x + 1) % numPeds;
        outPedId = CPopulation::GetPedGroupModelId(groupId, x);

        if (CStreaming::IsModelLoaded(outPedId))
            return true;
    }

    return false;
}

// 0x443B00
bool CGangWars::PickZoneToAttack() {
    CCarCtrl::InitSequence(CTheZones::TotalNumberOfNavigationZones);
    CZone* enemyGangZone = nullptr;

    // choose a territory that is controlled by enemy gangs
    for (auto i = 0; i < CTheZones::TotalNumberOfNavigationZones; i++) {
        auto zone = CTheZones::GetNavigationZone(CCarCtrl::FindSequenceElement(i));
        auto zoneInfo = CTheZones::GetZoneInfo(zone);

        if (!zoneInfo)
            continue;

        if (zoneInfo->GangDensity[GANG_BALLAS] + zoneInfo->GangDensity[GANG_VAGOS] >= 20) {
            enemyGangZone = zone;
            break;
        }
    }

    if (!enemyGangZone) [[unlikely]]
        return false;

    // find a close territory that is controlled by the gsf
    for (auto i = 0; i < CTheZones::TotalNumberOfNavigationZones; i++) {
        auto zone = CTheZones::GetNavigationZone(CCarCtrl::FindSequenceElement(i));
        auto zoneInfo = CTheZones::GetZoneInfo(zone);

        if (!zoneInfo)
            continue;

        if ((float)zone->m_fX1 <= 2500.0f && (float)zone->m_fX2 >= 2500.0f && (float)zone->m_fY1 <= -1666.0f && (float)zone->m_fY2 >= -1666.0f) // todo:
            continue;

        if (zoneInfo->GangDensity[GANG_GROVE] <= 15)
            continue;

        if (CTheZones::Calc2DDistanceBetween2Zones(enemyGangZone, zone) < 10.0f) {
            pZoneToFightOver = zone;
            pZoneInfoToFightOver = zoneInfo;
            PointOfAttack = CVector{
                (float)(zone->m_fX1 + zone->m_fX2) / 2.0f,
                (float)(zone->m_fY1 + zone->m_fY2) / 2.0f,
                10.0f
            };

            auto enemyGangZoneInfo = CTheZones::GetZoneInfo(enemyGangZone);
            Gang1 = (enemyGangZoneInfo->GangDensity[GANG_BALLAS] > enemyGangZoneInfo->GangDensity[GANG_VAGOS]) ? GANG_BALLAS : GANG_VAGOS;

            if (DistanceBetweenPoints2D(FindPlayerCoors(), PointOfAttack) > 60.0f)
                return true;
        }
    }

    return false;
}

// 0x445E20
void CGangWars::ReleaseCarsInAttackWave() {
    for (auto i = 0; i < GetVehiclePool()->GetSize(); ++i) {
        auto vehicle = GetVehiclePool()->GetAt(i);
        if (vehicle && vehicle->vehicleFlags.bPartOfAttackWave) {
            vehicle->vehicleFlags.bPartOfAttackWave = false;
            vehicle->SetVehicleCreatedBy(eVehicleCreatedBy::RANDOM_VEHICLE);
        }
    }
}

// Returns num of released peds
// 0x445C30
uint32 CGangWars::ReleasePedsInAttackWave(bool isEndOfWar, bool restoreGangPedsAcquaintance) {
    auto numReleasedPeds = 0u;

    for (auto i = 0; i < GetPedPool()->GetSize(); i++) {
        CPed* ped = GetPedPool()->GetAt(i);
        if (!ped)
            continue;

        if (ped->bPartOfAttackWave) {
            ped->bPartOfAttackWave = false;
            ped->SetCharCreatedBy(PED_GAME);
            numReleasedPeds++;
            CRadar::ClearBlipForEntity(BLIP_CHAR, i);
            ped->bClearRadarBlipOnDeath = false;

            if (restoreGangPedsAcquaintance) {
                auto taskWander = CTaskComplexWander::GetWanderTaskByPedType(ped);
                CEventScriptCommand event(TASK_PRIMARY_PRIMARY, taskWander, false);
                ped->GetEventGroup().Add(&event);
                ped->m_acquaintance = CPedType::GetPedTypeAcquaintances(ped->m_nPedType);
            }
        }

        if (isEndOfWar && ped->bClearRadarBlipOnDeath) {
            CRadar::ClearBlipForEntity(BLIP_CHAR, i);
            ped->bClearRadarBlipOnDeath = false;
        }
    }

    return numReleasedPeds;
}

// 0x446570
void CGangWars::SetGangWarsActive(bool active) {
    if (active != bGangWarsActive) {
        CTheZones::FillZonesWithGangColours(!active);
        TimeTillNextAttack = CalculateTimeTillNextAttack();

        if (!active)
            EndGangWar(false);
    }
    bGangWarsActive = active;
}

// 0x444010
void CGangWars::SetSpecificZoneToTriggerGangWar(int32 zoneId) {
    aSpecificZones[NumSpecificZones] = zoneId;
    NumSpecificZones++;
    CTheZones::FillZonesWithGangColours(false);
}

// 0x444300
void CGangWars::StartDefensiveGangWar() {
    if (PickZoneToAttack()) {
        Difficulty = TerritoryUnderControlPercentage;
        CHud::SetHelpMessage(TheText.Get("GW_ATK"), true, false, true);
        State2 = WAR_NOTIFIED;
        FightTimer = static_cast<uint32>(DistanceBetweenPoints2D(PointOfAttack, FindPlayerCoors()) * 200.0f + 240'000.0f);
        RadarBlip = CRadar::SetCoordBlip(BLIP_COORD, PointOfAttack, GetGangColor(Gang1), BLIP_DISPLAY_BLIPONLY, nullptr);

        CRadar::SetBlipSprite(RadarBlip, [] {
            switch (Gang1) {
            case GANG_BALLAS:
                return RADAR_SPRITE_GANGP;
            case GANG_VAGOS:
            case GANG_DANANGBOYS:
                return RADAR_SPRITE_GANGY;
            case GANG_RIFA:
                return RADAR_SPRITE_GANGB;
            default:
                return RADAR_SPRITE_ENEMYATTACK;
            }
        }());

        bPlayerIsCloseby = false;
        pZoneInfoToFightOver->radarMode = 2;
        pZoneInfoToFightOver->ZoneColor = CRGBA{ 255, 0, 0, 160 };
    } else {
        TimeTillNextAttack = CalculateTimeTillNextAttack();
    }
}

// 0x446050
void CGangWars::StartOffensiveGangWar() {
    CoorsOfPlayerAtStartOfWar = FindPlayerCoors();
    auto zoneInfo = CTheZones::GetZoneInfo(CoorsOfPlayerAtStartOfWar, &pZoneToFightOver);
    pZoneInfoToFightOver = zoneInfo;
    Difficulty = TerritoryUnderControlPercentage;

    if (!CanPlayerStartAGangWarHere(pZoneInfoToFightOver)) {
        Provocation = 0.0f;
        return;
    }

    // NOTSA
    Gang1 = (eGangID)(std::ranges::max_element(zoneInfo->GangDensity) - zoneInfo->GangDensity);
    auto gang1Density = zoneInfo->GangDensity[Gang1];
    zoneInfo->GangDensity[Gang1] = 0; // to find the second biggest

    Gang2 = std::ranges::max_element(zoneInfo->GangDensity) - zoneInfo->GangDensity;
    auto gang2Density = zoneInfo->GangDensity[Gang2];
    zoneInfo->GangDensity[Gang1] = gang1Density; // to restore

    Provocation = 0.0f;
    auto densitySum = std::accumulate(std::begin(zoneInfo->GangDensity), std::end(zoneInfo->GangDensity), 0);
    if (densitySum && (Gang1 == GANG_BALLAS || Gang1 == GANG_VAGOS)) {
        if (State2 != NO_ATTACK)
            return;

        auto provText = TheText.Get("GW_PROV");
        CMessages::AddMessageQ(provText, 4500, 1, true);
        CMessages::AddToPreviousBriefArray(provText);
        TimeStarted = CTimer::GetTimeInMS();
        State = PRE_FIRST_WAVE;
        ClearTheStreets();
        WarFerocity = std::min(densitySum / 15, 2);

        if (CStats::GetStatValue(STAT_CITY_UNLOCKED) < 0.0f) {
            if (TerritoryUnderControlPercentage > 0.4f) {
                if (TerritoryUnderControlPercentage > 0.5f)
                    WarFerocity = 5;
                else
                    WarFerocity = 0;
            }
        }
        if (bIsPlayerOnAMission)
            WarFerocity = 0;

        if (gang1Density > gang2Density / 3)
            Gang2 = Gang1;

        TellGangMembersTo(false);
        pZoneInfoToFightOver->radarMode = 2;
        zoneInfo->ZoneColor = CRGBA{ 255, 0, 0, 160 };
        pDriveByCar = nullptr;
    }
}

// 0x445F50
void CGangWars::StrengthenPlayerInfluenceInZone(int32 density) {
    bool controlledBefore = DoesPlayerControlThisZone(pZoneInfoToFightOver);

    auto& groveDensity = pZoneInfoToFightOver->GangDensity[GANG_GROVE];
    if (groveDensity < 55u) { // todo: magic number
        groveDensity = std::min(groveDensity + density, 55);
    }

    if (!controlledBefore && DoesPlayerControlThisZone(pZoneInfoToFightOver)) {
        CStats::IncrementStat(STAT_TERRITORIES_TAKEN_OVER, 1.0f);
    }
}

// 0x4465F0
void CGangWars::SwitchGangWarsActive() {
    SetGangWarsActive(!bGangWarsActive);
}

// 0x444530
void CGangWars::TellGangMembersTo(bool isGangWarEnding) {
    // return plugin::Call<0x444530, bool>(isGangWarEnding);

    for (auto i = 0; i < GetPedPool()->GetSize(); ++i) {
        auto ped = GetPedPool()->GetAt(i);
        if (!ped || ped->IsPlayer())
            continue;

        if (!ped->IsGangster() || ped->m_nPedType == PED_TYPE_GANG2)
            continue;

        if (!isGangWarEnding) {
            auto player = FindPlayerPed();
            auto task = new CTaskComplexKillPedOnFoot(player, -1, 0, 0, 0, 2);
            CEventScriptCommand esc(TASK_PRIMARY_PRIMARY, task, false);
            ped->GetEventGroup().Add(&esc);

            continue;
        }

        if (ped->IsInVehicle()) {
            CTask* task;
            if (!ped->IsInVehicleAsPassenger()) {
                task = new CTaskComplexCarDriveWander(ped->GetVehicleIfInOne(), DRIVING_STYLE_STOP_FOR_CARS, 10.0f);
            } else {
                task = new CTaskSimpleCarDrive(ped->GetVehicleIfInOne());
            }
            CEventScriptCommand event(TASK_PRIMARY_PRIMARY, task, false);
            ped->GetEventGroup().Add(&event);
        }

        auto task = new CTaskComplexWanderGang(PEDMOVE_WALK, CGeneral::GetRandomNumberInRange(0, 8), 5000, true, 0.5f);
        CEventScriptCommand event(TASK_PRIMARY_PRIMARY, task, false);
        ped->GetEventGroup().Add(&event);
    }
}

// FIX_BUGS: originally has int32* type, but changed to unsigned due possible UB
// 0x443D50
void CGangWars::TellStreamingWhichGangsAreNeeded(uint32& gangsBitFlags) {
    if (State2 == NO_ATTACK)
        return;

    if (DistanceBetweenPoints2D(PointOfAttack, FindPlayerCoors()) < 150.0f) {
        gangsBitFlags |= 1 << Gang1;
    }
}

// 0x446610
void CGangWars::Update() {
    ZoneScoped;

    return plugin::Call<0x446610>();

    if (CTheScripts::IsPlayerOnAMission() && !bIsPlayerOnAMission && NumSpecificZones == 0)
        EndGangWar(true);

    bIsPlayerOnAMission = CTheScripts::IsPlayerOnAMission();
    if (CCutsceneMgr::IsCutsceneProcessing())
        return;

    if (CTimer::m_FrameCounter == 56u)
        UpdateTerritoryUnderControlPercentage();

    if (!CGangWars::bGangWarsActive || CGameLogic::IsCoopGameGoingOn())
        return;

    Provocation = std::max(Provocation - CTimer::GetTimeStep() / 2000.0f, 0.0f);

    switch (State) {
    case NOT_IN_WAR:
        if (Provocation > 2.1f)
            StartOffensiveGangWar();

        break; // goto label_34;

    case PRE_FIRST_WAVE:
        if (CTimer::GetTimeInMS() <= TimeStarted + 10'000 || !CreateAttackWave(std::max(WarFerocity, 0), 0)) {
            // goto label_34;
        }
        State = FIRST_WAVE;
        TimeStarted = CTimer::GetTimeInMS();
        break; // goto label_35;

    case FIRST_WAVE: {
        if (!AttackWaveOvercome()) {
            // goto label_34;
        }
        ReleasePedsInAttackWave(false, false);
        ReleaseCarsInAttackWave();

        if (MakePlayerGainInfluenceInZone(0.3f)) {
            // goto label_33;
        }
        auto clr1 = TheText.Get("GW_CLR1");
        CMessages::AddMessageQ(clr1, 4500, 1, true);
        CMessages::AddToPreviousBriefArray(clr1);
        State = PRE_SECOND_WAVE;
        TimeStarted = CTimer::GetTimeInMS();
        break; // goto label_35;
    }
    case PRE_SECOND_WAVE:
        if (CTimer::GetTimeInMS() <= TimeStarted + 10'000 || !CreateAttackWave(std::max(WarFerocity, 0) + 1, 0)) {
            // goto label_34;
        }
        State = SECOND_WAVE;
        TimeStarted = CTimer::GetTimeInMS();
        break; // goto label_35;

    case SECOND_WAVE: {
        if (!AttackWaveOvercome()) {
            // goto label_34;
        }
        ReleasePedsInAttackWave(false, false);
        ReleaseCarsInAttackWave();

        if (MakePlayerGainInfluenceInZone(0.3f)) {
            // goto label_33;
        }
        auto clr2 = TheText.Get("GW_CLR2");
        CMessages::AddMessageQ(clr2, 4500, 1, true);
        CMessages::AddToPreviousBriefArray(clr2);
        State = PRE_THIRD_WAVE;
        TimeStarted = CTimer::GetTimeInMS();
        break; // goto label_35;
    }
    case PRE_THIRD_WAVE:
        if (CTimer::GetTimeInMS() <= TimeStarted + 10'000 || !CreateAttackWave(std::max(WarFerocity, 0) + 2, 0)) {
            // goto label_34;
        }
        State = THIRD_WAVE;
        TimeStarted = CTimer::GetTimeInMS();
        break; // goto label_35;

    case THIRD_WAVE: {
        if (!AttackWaveOvercome()) {
            // goto label_34;
        }
        MakePlayerGainInfluenceInZone(1.0f);

        // label_33:
        DoStuffWhenPlayerVictorious();

        // label_34:
        if (State == NOT_IN_WAR) {
            // goto label_51;
        }

        // label_35:
        auto playerPos = FindPlayerCoors();
        auto zone = pZoneToFightOver;

        bool inArea = zone->m_fX1 - 50 <= playerPos.x &&
                      zone->m_fX2 + 50 >= playerPos.x &&
                      zone->m_fY1 - 50 <= playerPos.y &&
                      zone->m_fY2 + 50 >= playerPos.y;

        if (CGame::currArea || inArea) {
            LastTimeInArea = CTimer::GetTimeInMS();
            // goto label_50;
        }

        if (CTimer::GetTimeInMS() - LastTimeInArea > 30'000) {
            State = NOT_IN_WAR;
            Provocation = 0.0f;
            ReleasePedsInAttackWave(true, false);
            ReleaseCarsInAttackWave();
            CTheZones::FillZonesWithGangColours(false);

            // label_49:
            CMessages::AddMessageQ(TheText.Get("GW_FLEE"), 4500, 1, true);
            CMessages::AddToPreviousBriefArray(TheText.Get("GW_WARN"));
            // goto label_50;
        }

        if (CTimer::GetTimeInMS() - LastTimeInArea > 10'000 && CTimer::GetPreviousTimeInMS() - LastTimeInArea <= 10'000) {
            // goto label_49;
        }

        // label_50:
        if (State != NOT_IN_WAR) {
            // goto label_53;
        }

        // label_51:
        if (CTheScripts::IsPlayerOnAMission() || playerPos.z > 950.0f) {
            // label_53:
            TimeTillNextAttack = std::min(TimeTillNextAttack, 30'000.0f);
        }

        auto veh = FindPlayerVehicle();
        if (State2 != NO_ATTACK) {
            switch (State2) {
            case WAR_NOTIFIED:
                if (DistanceBetweenPoints2D(PointOfAttack, playerPos) >= 70.0f) {
                    FightTimer -= (uint32)CTimer::GetTimeStepInMS();

                    if (FightTimer < 0) {
                        auto nosh = TheText.Get("GW_NOSH");
                        CMessages::AddMessageQ(nosh, 4500, 1, true);
                        CMessages::AddToPreviousBriefArray(nosh);

                        State2 = NO_ATTACK;
                        MakeEnemyGainInfluenceInZone(Gang1, 30);
                        CTheZones::FillZonesWithGangColours(false);
                        TimeTillNextAttack = CalculateTimeTillNextAttack();
                        CStats::DecrementStat(STAT_RESPECT, 30.0f);
                    }
                } else if (CreateDefendingGroup(0)) {
                    FightTimer += 30'000;
                    State2 = PLAYER_CAME_TO_WAR;
                }
                break;

            case PLAYER_CAME_TO_WAR:
                if (AttackWaveOvercome()) {
                    auto won = TheText.Get("GW_WON");
                    CMessages::AddMessageQ(won, 4500, 1, true);
                    CMessages::AddToPreviousBriefArray(won);

                    State2 = NO_ATTACK;
                    StrengthenPlayerInfluenceInZone(10);
                    CTheZones::FillZonesWithGangColours(false);
                    TimeTillNextAttack = CalculateTimeTillNextAttack();
                    ReleasePedsInAttackWave(true, false);
                } else {
                    FightTimer -= (uint32)CTimer::GetTimeStepInMS();

                    if (FightTimer < 0) {
                        auto slow = TheText.Get("GW_SLOW");
                        CMessages::AddMessageQ(slow, 4500, 1, true);
                        CMessages::AddToPreviousBriefArray(slow);

                        State2 = NO_ATTACK;
                        MakeEnemyGainInfluenceInZone(Gang1, 3 * ReleasePedsInAttackWave(true, false));
                        CTheZones::FillZonesWithGangColours(false);
                        TimeTillNextAttack = CalculateTimeTillNextAttack();
                        CStats::DecrementStat(STAT_RESPECT, 30.0f);
                    }
                }
            }
        } else if (!CTheScripts::IsPlayerOnAMission() && !bTrainingMission && (!veh || !veh->IsSubFlyingVehicle())) {
            if (State == NOT_IN_WAR) {
                if (CWeather::WeatherRegion == WEATHER_REGION_LA) {
                    TimeTillNextAttack -= CTimer::GetTimeStepInMS();
                } else {
                    TimeTillNextAttack -= CTimer::GetTimeStepInMS() * 0.6f;
                }
            }

            if (TimeTillNextAttack < 0.0f) {
                StartDefensiveGangWar();
            }
        }

        if (RadarBlip) {
            if (State2 == NO_ATTACK || State2 == PLAYER_CAME_TO_WAR) {
                CRadar::ClearBlip(RadarBlip);
                RadarBlip = 0;
            } else {
                auto blinkPeriod = 7;
                if (FightTimer > 120'000) {
                    blinkPeriod = 10;
                } else if (FightTimer > 60'000) {
                    blinkPeriod = 9;
                } else if (FightTimer > 30'000) {
                    blinkPeriod = 8;
                }

                if ((CTimer::GetTimeInMS() >> blinkPeriod) % 2) {
                    CRadar::ChangeBlipDisplay(RadarBlip, BLIP_DISPLAY_NEITHER);
                } else {
                    CRadar::ChangeBlipDisplay(RadarBlip, BLIP_DISPLAY_BLIPONLY);
                }
            }
        }

        if (State2 != NO_ATTACK) {
            if (DistanceBetweenPoints2D(PointOfAttack, playerPos) >= 150.0f) {
                bPlayerIsCloseby = false;
            } else if (!bPlayerIsCloseby) {
                CVector unused{};
                CStreaming::StreamZoneModels_Gangs(unused);
                bPlayerIsCloseby = true;
            }
        }
        break;
    }
    default:
        ;
        // goto label_34;
    }
}

// todo (Izzotop): make more original
// 0x443DE0
void CGangWars::UpdateTerritoryUnderControlPercentage() {
    auto ballasZones = 0, groveZones = 0, vagosZones = 0;
    if (CTheZones::TotalNumberOfNavigationZones) {
        for (auto& zone : CTheZones::NavigationZoneArray) {
            auto zoneInfo = CTheZones::GetZoneInfo(&zone);
            if (!zoneInfo)
                continue;

            auto ballasDensity = zoneInfo->GangDensity[GANG_BALLAS],
                 groveDensity = zoneInfo->GangDensity[GANG_GROVE],
                 vagosDensity = zoneInfo->GangDensity[GANG_VAGOS];

            // there should be one liner solution for this, no?
            if (groveDensity > ballasDensity + vagosDensity) {
                groveZones++;
            } else if (ballasDensity > vagosDensity) {
                ballasZones++;
            } else if (vagosDensity > ballasDensity) {
                vagosZones++;
            }
        }
    }

    CStats::SetStatValue(STAT_TERRITORIES_HELD, static_cast<float>(groveZones));
    CStats::SetNewRecordStat(STAT_HIGHEST_NUMBER_OF_TERRITORIES_HELD, static_cast<float>(groveZones));

    auto allGangZones = groveZones + ballasZones + vagosZones;
    if (allGangZones) {
        GangRatings[GANG_GROVE] = 0;
        GangRatings[GANG_BALLAS] = 1;
        GangRatings[GANG_VAGOS] = 2;

        GangRatingStrength[0] = groveZones;
        GangRatingStrength[1] = ballasZones;
        GangRatingStrength[2] = vagosZones;

        TerritoryUnderControlPercentage = static_cast<float>(groveZones) / static_cast<float>(allGangZones);

        // NOTSA code
        struct GangRanking {
            eGangID gang;
            int32 controlled;
        } ranking[3] = {
            { GANG_BALLAS, ballasZones },
            { GANG_GROVE,  groveZones  },
            { GANG_VAGOS,  vagosZones  }
        };
        std::sort(ranking, ranking + 3, [&](GangRanking a, GangRanking b) { return a.controlled > b.controlled; });

        for (auto&& [i, e] : notsa::enumerate(ranking)) {
            GangRatings[e.gang] = i;
            GangRatingStrength[i] = e.controlled;
        }
    } else {
        TerritoryUnderControlPercentage = 0.0f;
    }
}

// todo: Use macro for color conversion
// 0x44582F NOTSA
eBlipColour CGangWars::GetGangColor(int32 gang) {
    // 0x8D1344 0x8D1350 0x8D135C
    static constexpr uint8 gaGangColoursR[] = { 200,  70, 255,   0, 255, 200, 240,   0, 255, 255, 0, 0 };
    static constexpr uint8 gaGangColoursB[] = { 200,   0,   0, 200, 190, 200, 240, 255, 255, 255, 0, 0 };
    static constexpr uint8 gaGangColoursG[] = {   0, 200, 200,   0, 220, 200, 140, 200, 255, 255, 0, 0 };

    auto r = gaGangColoursR[gang], g = gaGangColoursG[gang], b = gaGangColoursB[gang];
    return (eBlipColour)(((b | (((r << 8) | g) << 8)) << 8) | 0xFF);
}
