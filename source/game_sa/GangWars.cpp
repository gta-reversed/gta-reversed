/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "GangWars.h"
#include "GangWarsSaveStructure.h"
#include "ModelIndices.h"
#include "TaskComplexWander.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexCarDriveWander.h"
#include "TaskSimpleCarDrive.h"
#include "TaskComplexWanderGang.h"
#include "TaskSimpleGangDriveBy.h"
#include "Hud.h"

#include <numbers>

// 0x5D3EB0
bool CGangWars::Load() {
    CGenericGameStorage::LoadDataFromWorkBuffer<uint32>(); // Discard
    auto data = CGenericGameStorage::LoadDataFromWorkBuffer<CGangWarsSaveStructure>();
    data.Extract();
    return true;
}

// 0x5D5530
bool CGangWars::Save() {
    CGangWarsSaveStructure data{};
    data.Construct();
    CGenericGameStorage::SaveDataToWorkBuffer(sizeof(CGangWarsSaveStructure));
    CGenericGameStorage::SaveDataToWorkBuffer(data);
    return true;
}

// 0x443920
void CGangWars::InitAtStartOfGame() {
    ZoneScoped;

    ms_State              = NOT_IN_WAR;
    ms_State2             = NO_ATTACK;
    ms_NumSpecificZones   = 0;
    ms_Provocation        = 0.0f;
    ms_GangWarsActive     = false;
    ms_IsPlayerOnAMission = false;
}

// 0x443950
void CGangWars::AddKillToProvocation(ePedType pedType) {
    if (!ms_CanTriggerGangWarWhenOnAMission && CTheScripts::IsPlayerOnAMission()) {
        return;
    }

    if (pedType != PED_TYPE_GANG1 && pedType != PED_TYPE_GANG3) {
        return;
    }

    if (ms_NumSpecificZones == 0) {
        ms_Provocation += 1.0f;
        return;
    }

    for (const auto zone : GetSpecificZones()) {
        if (CTheZones::GetZoneInfo(CTheZones::GetNavigationZone(zone))->GangStrength[pedType - PED_TYPE_GANG1] != 0) {
            ms_Provocation += 1.0f;
            return;
        }
    }
}

// 0x445B30
bool CGangWars::AttackWaveOvercome() {
    auto numPedsAlive = 0u, numPedsNearPlayer = 0u;

    for (auto& ped : GetPedPool()->GetAllValid()) {
        if (!ped.bPartOfAttackWave) {
            continue;
        }

        if (ped.IsStateDying()) {
            ped.bPartOfAttackWave = false;
            ped.SetCharCreatedBy(PED_GAME);
            continue;
        }

        numPedsAlive++;
        if (DistanceBetweenPoints2D(ped.GetPosition2D(), FindPlayerCoors()) < 45.0f) {
            numPedsNearPlayer++;
        }
    }

    return numPedsAlive <= 1 && numPedsNearPlayer == 0;
}

// 0x443DB0
uint32 CGangWars::CalculateTimeTillNextAttack() {
    // NOTE: Same operations as the original (0x443DB0) for the same rounding, it's 648 000 - 1 620 000 ms
    return (uint32)(CGeneral::GetRandomNumberInRange(0.0f, 0.9f) * 1'080'000.0f + 648'000.0f);
}

// 0x443F80
bool CGangWars::CanPlayerStartAGangWarHere(CZoneInfo* zoneInfo) {
    if (ms_IsTrainingMission) {
        return zoneInfo == &CTheZones::ZoneInfoArray[ms_ZoneInfoForTraining];
    }

    if (ms_NumSpecificZones == 0) {
        return true;
    }

    for (const auto zone : GetSpecificZones()) {
        if (zoneInfo == CTheZones::GetZoneInfo(CTheZones::GetNavigationZone(zone))) {
            return true;
        }
    }

    return false;
}

// 0x444040
void CGangWars::CheerVictory() {
    CPed* nearestMember{};
    FindPlayerPed()->GetPlayerGroup().FindDistanceToNearestMember(&nearestMember);
    if (!nearestMember) {
        return;
    }

    static const auto mapping = notsa::make_mapping<notsa::ci_string_view, eGlobalSpeechContext>({
        { "CHC",  CTX_GLOBAL_TAKE_TURF_LAS_COLINAS       },
        { "LFL",  CTX_GLOBAL_TAKE_TURF_LOS_FLORES        },
        { "EBE",  CTX_GLOBAL_TAKE_TURF_EAST_BEACH        },
        { "ELS",  CTX_GLOBAL_TAKE_TURF_EAST_LS           },
        { "JEF",  CTX_GLOBAL_TAKE_TURF_JEFFERSON         },
        { "GLN",  CTX_GLOBAL_TAKE_TURF_GLEN_PARK         },
        { "IWD",  CTX_GLOBAL_TAKE_TURF_IDLEWOOD          },
        { "GAN",  CTX_GLOBAL_TAKE_TURF_GANTON            },
        { "LMEX", CTX_GLOBAL_TAKE_TURF_LITTLE_MEXICO     },
        { "LIND", CTX_GLOBAL_TAKE_TURF_WILLOWFIELD       },
        { "PLS",  CTX_GLOBAL_TAKE_TURF_PLAYA_DEL_SEVILLE },
        { "SUN",  CTX_GLOBAL_TAKE_TURF_TEMPLE            },
    });
    if (const auto ctx = notsa::find_value_or(mapping, notsa::ci_string_view{ ms_ZoneToFightOver->m_InfoLabel }, CTX_GLOBAL_NO_SPEECH); ctx != CTX_GLOBAL_NO_SPEECH) {
        nearestMember->Say(ctx);
    }
}

// 0x443FF0
void CGangWars::ClearSpecificZonesToTriggerGangWar() {
    ms_NumSpecificZones = 0;
    CTheZones::FillZonesWithGangColours(false);
}

// 0x4444B0
void CGangWars::ClearTheStreets() {
    for (auto& ped : GetPedPool()->GetAllValid()) {
        if (ped.IsPlayer() || !ped.IsCivilian()) {
            continue;
        }

        if (auto* const tWander = ped.GetTaskManager().Find<CTaskComplexWander>()) {
            tWander->m_nMoveState = PEDMOVE_SPRINT;
        }
    }
}

// 0x444810
bool CGangWars::CreateAttackWave(int32 warFerocity, int32 waveId) {
    const CVector playerPos = FindPlayerCoors();
    if (CGame::currArea != AREA_CODE_NORMAL_WORLD || playerPos.z > 950.0f) {
        return false;
    }

    if (!ThePaths.AreNodesLoadedForArea(playerPos.x - 20.0f, playerPos.x + 20.0f, playerPos.y - 20.0f, playerPos.y + 20.0f)) {
        return false;
    }

    if (!PedStreamedInForThisGang(ms_Gang1)) {
        CStreaming::RequestModel(CPopulation::GetPedGroupModelId(CPopulation::GetGangGroupId(ms_Gang1), 0), STREAMING_KEEP_IN_MEMORY);
    }
    if (ms_Gang1 != ms_Gang2 && !PedStreamedInForThisGang((eGangID)ms_Gang2)) {
        CStreaming::RequestModel(CPopulation::GetPedGroupModelId(CPopulation::GetGangGroupId((eGangID)ms_Gang2), 0), STREAMING_KEEP_IN_MEMORY);
    }

    struct WaveWeapons {
        eWeaponType Weapon1, Weapon2;
        eModelID    Model1, Model2;
    };
    static constexpr WaveWeapons STRONGEST_WAVE_WEAPONS{ WEAPON_DESERT_EAGLE, WEAPON_AK47, MODEL_DESERT_EAGLE, MODEL_AK47 };

    const auto weapons = [&]() -> std::optional<WaveWeapons> {
        if (ms_IsTrainingMission) { // 0x4449ee
            switch (warFerocity) {
            case 0:  return WaveWeapons{ WEAPON_BASEBALLBAT, WEAPON_BASEBALLBAT, MODEL_BAT, MODEL_BAT };
            case 1:  return WaveWeapons{ WEAPON_PISTOL, WEAPON_PISTOL, MODEL_COLT45, MODEL_COLT45 };
            default: return WaveWeapons{ WEAPON_TEC9, WEAPON_TEC9, MODEL_TEC9, MODEL_TEC9 };
            }
        }
        switch (warFerocity) {
        case 0: return WaveWeapons{ WEAPON_BASEBALLBAT, WEAPON_PISTOL, MODEL_BAT, MODEL_COLT45 };     // 0x44494d
        case 1: return WaveWeapons{ WEAPON_PISTOL, WEAPON_MICRO_UZI, MODEL_COLT45, MODEL_MICRO_UZI }; // 0x444964
        case 2: return WaveWeapons{ WEAPON_MICRO_UZI, WEAPON_MP5, MODEL_MICRO_UZI, MODEL_MP5LNG };    // 0x444978
        case 3: return WaveWeapons{ WEAPON_MP5, WEAPON_AK47, MODEL_MP5LNG, MODEL_AK47 };              // 0x444990
        case 4: return WaveWeapons{ WEAPON_AK47, WEAPON_AK47, MODEL_AK47, MODEL_AK47 };               // 0x4449ac
        case 5: return STRONGEST_WAVE_WEAPONS;                                                        // 0x4449c2
        default:                                                                                      // 0x4449de
            // NOTE/BUG: The original uses uninitialized locals as the weapons and models here. `Update` passes 6 and 7 for the 2nd and 3rd wave when `ms_WarFerocity` is 5.
            if (notsa::bugfixes::GenericUB) {
                return STRONGEST_WAVE_WEAPONS;
            }
            return std::nullopt;
        }
    }();
    if (!weapons) {
        return false;
    }

    if (!CStreaming::IsModelLoaded(weapons->Model1) || !CStreaming::IsModelLoaded(weapons->Model2)) {
        CStreaming::RequestModel(weapons->Model1, STREAMING_DEFAULT);
        CStreaming::RequestModel(weapons->Model2, STREAMING_DEFAULT);
        return false;
    }

    if (!PedStreamedInForThisGang(ms_Gang1) || !PedStreamedInForThisGang((eGangID)ms_Gang2)) {
        return false;
    }

    CVector prevSpawnPos{ 999'999.9f, 999'999.9f, 999'999.9f };
    bool    createdAnyPed{};
    for (auto i = 0; i < 2; i++) {
        const auto gang = i == 0 ? ms_Gang1 : (eGangID)ms_Gang2;

        // Find a spawn point, preferably out of sight and away from the previous group's
        std::optional<CVector> spawnPos{};
        for (auto attempt = 0; attempt < 20; attempt++) {
            CVector      pos{};
            CNodeAddress node1{}, node2{};
            float        frac{};
            if (!CCarCtrl::GenerateCarCreationCoors2(playerPos, 1.0f, 0.0f, -1.0f, true, 50.0f, 50.0f, &pos, &node1, &node2, &frac, false, true)) {
                continue;
            }
            if (!TheCamera.IsSphereVisible(pos, 7.0f)) {
                spawnPos = pos;
                if ((prevSpawnPos - pos).Magnitude() > 15.0f) { // NOTE: 3D distance (`CVector::Dist` is 2D)
                    break;
                }
            } else if (!spawnPos) {
                spawnPos = pos;
            }
        }
        if (!spawnPos) {
            continue;
        }
        const CVector origin = *spawnPos;
        prevSpawnPos         = origin;

        // The peds are lined up next to each other, perpendicular to the player
        const CVector side    = CVector{ playerPos.y - origin.y, origin.x - playerPos.x, 0.0f }.Normalized() * 1.2f;
        const auto    numPeds = [&] {
            if (warFerocity == 5) {
                return 10;
            }
            return ms_IsTrainingMission
                ? 2
                : (int32)((ms_Difficulty * 0.3f + 0.7f) * (float)(warFerocity + 3));
        }();
        for (auto p = 0, offset = -(numPeds / 2) * 2; p < numPeds; p++, offset += 2) {
            const CVector linePos = origin + side * (float)offset;
            CVector       pedPos{
                linePos.x + CGeneral::GetRandomNumberInRange(-1.5f, 1.5f),
                linePos.y + CGeneral::GetRandomNumberInRange(-1.5f, 1.5f),
                linePos.z
            };
            int32 pedModel{};
            if (!PickStreamedInPedForThisGang(gang, pedModel)) {
                continue;
            }
            pedPos.z = CWorld::FindGroundZFor3DCoord({ pedPos.x, pedPos.y, linePos.z + 2.0f }) + 1.3f;

            // NOTE: The ped type is always `ms_Gang1`'s, even for `ms_Gang2`'s models
            auto* const ped = new CCivilianPed((ePedType)(ms_Gang1 + PED_TYPE_GANG1), pedModel);
            ped->SetPosn(pedPos);
            ped->SetCharCreatedBy(PED_MISSION);
            CWorld::Add(ped);
            ped->GetEventGroup().Add(CEventScriptCommand{
                TASK_PRIMARY_PRIMARY, new CTaskComplexKillPedOnFoot{ FindPlayerPed(), -1, 0, 0, 0, 2 },
                  false
            });

            const auto weapon = CGeneral::GetRandomNumberInRange(-0.3f, 1.3f) <= ms_Difficulty
                ? weapons->Weapon2
                : weapons->Weapon1;
            ped->GiveWeapon(weapon, 5'000, false);
            ped->SetCurrentWeapon(weapon);
            ped->SetWeaponAccuracy((uint8)(ms_Difficulty * 25.0f + 70.0f));
            ped->bPartOfAttackWave      = true;
            ped->bClearRadarBlipOnDeath = true;
            ped->m_fMaxHealth           = 120.0f;
            ped->m_fHealth              = 120.0f;

            const auto color            = GetGangColor(gang);
            const auto blip             = CRadar::SetEntityBlip(BLIP_CHAR, GetPedPool()->GetRef(ped), color, BLIP_DISPLAY_BLIPONLY);
            CRadar::ChangeBlipScale(blip, 2);
            CRadar::ChangeBlipColour(blip, color);
            createdAnyPed = true;
        }

        const auto pickupX     = origin.x + CGeneral::GetRandomNumberInRange(-2.0f, 2.0f);
        const auto pickupY     = origin.y + CGeneral::GetRandomNumberInRange(-2.0f, 2.0f);
        const auto pickupZ     = CWorld::FindGroundZFor3DCoord({ pickupX, pickupY, origin.z + 1.0f }) + 0.75f;
        const auto pickupModel = [&]() -> std::optional<ModelIndex> {
            switch (waveId) {
            case 0: // 0x44506d
                return i == 0 ? std::make_optional(ModelIndices::MI_PICKUP_HEALTH) : std::nullopt;
            case 1: // 0x44505c
                return i == 0 ? std::make_optional(ModelIndices::MI_PICKUP_BODYARMOUR) : std::nullopt;
            case 2: // 0x445043
                return i == 0 ? ModelIndices::MI_PICKUP_HEALTH : ModelIndices::MI_PICKUP_BODYARMOUR;
            default:
                return std::nullopt;
            }
        }();
        if (pickupModel) {
            CPickups::GenerateNewOne({ pickupX, pickupY, pickupZ }, *pickupModel, PICKUP_ONCE_TIMEOUT_SLOW, 0, 0, false, nullptr);
        }
    }

    if (!createdAnyPed) {
        return false;
    }

    if (ms_IsTrainingMission || ms_DriveByCar) {
        return true;
    }

    const auto carModel = CPopulation::PickGangCar(ms_Gang1);
    if (carModel < 0 || !CStreaming::IsModelLoaded(carModel)) {
        return true;
    }

    ms_DriveByCar = CCarCtrl::GenerateOneEmergencyServicesCar(carModel, FindPlayerCoors());
    if (!ms_DriveByCar) {
        return true;
    }
    CEntity::RegisterReference(ms_DriveByCar);

    auto* const car = ms_DriveByCar;
    CCarCtrl::JoinCarWithRoadSystemGotoCoors(car, FindPlayerCoors(), false, false);
    car->vehicleFlags.bPartOfAttackWave = true;
    car->m_autoPilot.SetCarMission(MISSION_DO_DRIVEBY_FARAWAY);
    car->m_autoPilot.SetCruiseSpeed(10);
    car->m_autoPilot.m_TargetEntity = reinterpret_cast<CVehicle*>(FindPlayerPed()); // NOTE: The original stores the player ped as the target
    car->m_autoPilot.SetDrivingStyle(DRIVING_STYLE_AVOID_CARS);
    car->SetStatus(STATUS_PHYSICS);
    car->vehicleFlags.bNeverUseSmallerRemovalRange = true;
    car->m_autoPilot.m_nStraightLineDistance       = 30;

    constexpr auto GANG1_CAR_RATING                = 14; // See `CPopulation::AddPedInCar`
    CCarCtrl::SetUpDriverAndPassengersForVehicle(car, GANG1_CAR_RATING + ms_Gang1, 1, false, false, 1);

    auto* const driver = car->GetDriver();
    if (driver) {
        driver->GetIntelligence()->SetPedDecisionMakerType(-1);
    }

    // NOTE: The original goes through all 8 seats, not just `GetPassengers()`
    for (auto&& [seat, passenger] : rngv::enumerate(car->m_apPassengers)) {
        if (!passenger) {
            continue;
        }
        passenger->GiveDelayedWeapon(WEAPON_MICRO_UZI, 1'500);
        passenger->SetCurrentWeapon(WEAPON_MICRO_UZI);
        passenger->GetEventGroup().Add(CEventScriptCommand{
            TASK_PRIMARY_PRIMARY, new CTaskSimpleGangDriveBy{ FindPlayerPed(), nullptr, 100.0f, 50, eDrivebyStyle::AI_ALL_DIRN, seat % 2 == 0 },
              false
        });
    }

    if (notsa::bugfixes::GenericCrashing && !driver) {
        return true;
    }
    const auto color = GetGangColor(ms_Gang2);
    const auto blip  = CRadar::SetEntityBlip(BLIP_CHAR, GetPedPool()->GetRef(driver), color, BLIP_DISPLAY_BLIPONLY);
    CRadar::ChangeBlipScale(blip, 3);
    CRadar::ChangeBlipColour(blip, color);
    driver->bClearRadarBlipOnDeath = true;

    return true;
}

// 0x4453D0
bool CGangWars::CreateDefendingGroup(int32 unused) {
    if (!PedStreamedInForThisGang(ms_Gang1)) {
        CStreaming::RequestModel(CPopulation::GetPedGroupModelId(CPopulation::GetGangGroupId(ms_Gang1), 0), STREAMING_KEEP_IN_MEMORY);
        return false;
    }

    if (!ThePaths.AreNodesLoadedForArea(ms_PointOfAttack.x, ms_PointOfAttack.x, ms_PointOfAttack.y, ms_PointOfAttack.y)) {
        return false;
    }

    const auto node = ThePaths.FindNodeClosestToCoors(ms_PointOfAttack, PATH_TYPE_VEH, 400.0f, 0, 0, 0, 0, 1);
    if (!node.IsAreaValid()) {
        return false;
    }

    const CVector nodePos = ThePaths.GetPathNode(node)->GetPosition();
    if (DistanceBetweenPoints2D(nodePos, FindPlayerCoors()) <= 40.0f) {
        return false;
    }

    const auto numPeds = (int32)((ms_Difficulty * 0.4f + 0.6f) * 10.0f);
    for (auto i = 0; i < numPeds; i++) {
        int32 pedModel{};
        if (!PickStreamedInPedForThisGang(ms_Gang1, pedModel)) {
            continue;
        }

        // NOTE: The original loads its own 2 * PI (6.2831855f) at 0x858CBC
        const auto angle = (float)i * (2.0f * std::numbers::pi_v<float>) / (float)numPeds;
        CVector    pedPos{
            nodePos.x + std::sin(angle) * (CGeneral::GetRandomNumberInRange(0.0f, 3.0f) + 2.0f),
            nodePos.y + std::cos(angle) * (CGeneral::GetRandomNumberInRange(0.0f, 3.0f) + 2.0f),
            nodePos.z
        };
        pedPos.z        = CWorld::FindGroundZFor3DCoord({ pedPos.x, pedPos.y, nodePos.z + 2.0f }) + 1.0f;

        auto* const ped = new CCivilianPed((ePedType)(ms_Gang1 + PED_TYPE_GANG1), pedModel);
        ped->SetPosn(pedPos);
        ped->m_fAimingRotation  = angle;
        ped->m_fCurrentRotation = angle;
        ped->SetHeading(angle);
        ped->SetCharCreatedBy(PED_MISSION);
        CWorld::Add(ped);
        ped->GetEventGroup().Add(CEventScriptCommand{
            TASK_PRIMARY_PRIMARY, new CTaskComplexKillPedOnFoot{ FindPlayerPed(), -1, 0, 0, 0, 2 },
              false
        });

        ped->SetWeaponAccuracy((uint8)(90.0f - ms_Difficulty * 60.0f));
        ped->GetAcquaintance().SetAsAcquaintance(ACQUAINTANCE_HATE, CPedType::GetPedFlag(PED_TYPE_PLAYER1));

        static constexpr eWeaponType WEAPONS[]{ WEAPON_PISTOL, WEAPON_MICRO_UZI, WEAPON_SHOTGUN, WEAPON_MP5 };
        const auto                   weapon = WEAPONS[i % 4];
        ped->GiveDelayedWeapon(weapon, 5'000);
        ped->SetCurrentWeapon(weapon);
        ped->bPartOfAttackWave      = true;
        ped->bClearRadarBlipOnDeath = true;

        const auto color            = GetGangColor(ms_Gang1);
        const auto blip             = CRadar::SetEntityBlip(BLIP_CHAR, GetPedPool()->GetRef(ped), color, BLIP_DISPLAY_BLIPONLY);
        CRadar::ChangeBlipScale(blip, 2);
        CRadar::ChangeBlipColour(blip, color);
    }

    for (auto n = 1; n <= 3; n++) {
        const auto    carNode    = ThePaths.FindNthNodeClosestToCoors(ms_PointOfAttack, PATH_TYPE_VEH, 100.0f, false, false, n, false, true, nullptr);
        const CVector carNodePos = ThePaths.GetPathNode(carNode)->GetPosition();
        if (DistanceBetweenPoints2D(FindPlayerCoors(), carNodePos) <= 25.0f) {
            continue;
        }

        const auto carModel = CPopulation::PickGangCar(ms_Gang1);
        if (carModel < 0 || !CStreaming::IsModelLoaded(carModel)) {
            continue;
        }

        // The car is put sideways, perpendicular to the direction from the defenders to its node
        const CVector fwd             = CVector{ carNodePos.y - nodePos.y, nodePos.x - carNodePos.x, 0.0f }.Normalized();

        auto* const car               = CCarCtrl::CreateCarForScript(carModel, carNodePos, false);
        car->GetMatrix().GetForward() = fwd;
        car->GetMatrix().GetRight()   = CVector{ fwd.y, -fwd.x, 0.0f };
        car->AsAutomobile()->PlaceOnRoadProperly();
        car->vehicleFlags.bIsLocked = false;
        car->SetVehicleCreatedBy(MISSION_VEHICLE);
        car->vehicleFlags.bPartOfAttackWave = true;
    }

    const CVector pickupPos{
        nodePos.x + CGeneral::GetRandomNumberInRange(-2.0f, 2.0f),
        nodePos.y + CGeneral::GetRandomNumberInRange(-2.0f, 2.0f),
        nodePos.z
    };
    const auto pickupZ = CWorld::FindGroundZFor3DCoord({ pickupPos.x, pickupPos.y, nodePos.z + 1.0f }) + 0.75f;
    CPickups::GenerateNewOne(
        { pickupPos.x, pickupPos.y, pickupZ },
        (CGeneral::GetRandomNumber() & 1) ? ModelIndices::MI_PICKUP_HEALTH : ModelIndices::MI_PICKUP_BODYARMOUR,
        PICKUP_ONCE_TIMEOUT_SLOW,
        0,
        0,
        false,
        nullptr
    );
    return true;
}

// 0x446400
void CGangWars::DoStuffWhenPlayerVictorious() {
    ReleasePedsInAttackWave(true, false);
    ReleaseCarsInAttackWave();
    CheerVictory();
    ms_State = NOT_IN_WAR;
    CMessages::AddMessageQ(TheText.Get("GW_YRS"), 4500, 1, true);
    CMessages::AddToPreviousBriefArray(TheText.Get("GW_YRS"));
    ms_Provocation = 0.0f;
    TellGangMembersTo(true);
    CStats::IncrementStat(STAT_RESPECT, 45.0f);
    CTheZones::FillZonesWithGangColours(false);
    ms_TimeTillNextAttack = notsa::step_down_to(ms_TimeTillNextAttack, 30'000.0f, 240'000.0f);
}

// 0x443AE0
bool CGangWars::DoesPlayerControlThisZone(CZoneInfo* zoneInfo) {
    const auto groveDensity = zoneInfo->GangStrength[GANG_GROVE];
    return groveDensity != 0 && groveDensity >= zoneInfo->GangStrength[GANG_BALLAS] + zoneInfo->GangStrength[GANG_VAGOS];
}

// 0x4439C0
bool CGangWars::DontCreateCivilians() {
    return ms_State != NOT_IN_WAR;
}

// 0x4464C0
void CGangWars::EndGangWar(bool restoreGangPedsAcquaintance) {
    ms_State = NOT_IN_WAR;

    if (ms_State2 != NO_ATTACK) {
        ms_State2             = NO_ATTACK;
        ms_TimeTillNextAttack = (float)CalculateTimeTillNextAttack();
        MakeEnemyGainInfluenceInZone(ms_Gang1, 3 * ReleasePedsInAttackWave(true, false));
    }

    ms_Provocation = 0.0f;
    CTheZones::FillZonesWithGangColours(false);
    ReleasePedsInAttackWave(true, restoreGangPedsAcquaintance);
    ReleaseCarsInAttackWave();
}

// 0x443AC0
bool CGangWars::GangWarFightingGoingOn() {
    return ms_State != NOT_IN_WAR || ms_State2 == PLAYER_CAME_TO_WAR;
}

// 0x443AA0
bool CGangWars::GangWarGoingOn() {
    return ms_State != NOT_IN_WAR || ms_State2 != NO_ATTACK;
}

// 0x445FD0
void CGangWars::MakeEnemyGainInfluenceInZone(int32 gangId, int32 density) {
    if (notsa::bugfixes::GenericCrashing && !ms_ZoneInfoToFightOver) {
        return;
    }

    const auto  controlledBefore = DoesPlayerControlThisZone(ms_ZoneInfoToFightOver);
    const auto& strength         = ms_ZoneInfoToFightOver->GangStrength;
    if (strength[GANG_BALLAS] + strength[GANG_GROVE] + strength[GANG_VAGOS] == 0) {
        return;
    }

    ms_ZoneInfoToFightOver->GangStrength[gangId] += (uint8)density;

    if (controlledBefore && !DoesPlayerControlThisZone(ms_ZoneInfoToFightOver)) {
        CStats::IncrementStat(STAT_TERRITORIES_LOST, 1.0f);
    }
}

// 0x445E80
bool CGangWars::MakePlayerGainInfluenceInZone(float removeMult) {
    const auto controlledBefore = DoesPlayerControlThisZone(ms_ZoneInfoToFightOver);

    auto& strength              = ms_ZoneInfoToFightOver->GangStrength;
    auto  totalEnemyDensity     = 0;
    for (auto&& [gang, density] : rngv::enumerate(strength)) {
        if (gang == GANG_GROVE) {
            continue;
        }
        const auto densityBefore = density;
        density                  = (uint8)((1.0f - removeMult) * (float)density);
        if (density < 4) {
            density = 0;
        }
        strength[GANG_GROVE] += densityBefore - density;
        totalEnemyDensity += density;
    }

    if (!controlledBefore && DoesPlayerControlThisZone(ms_ZoneInfoToFightOver)) {
        CStats::IncrementStat(STAT_TERRITORIES_TAKEN_OVER, 1.0f);
    }

    return totalEnemyDensity == 0;
}

// 0x4439D0
bool CGangWars::PedStreamedInForThisGang(eGangID gangId) {
    return rng::any_of(CPopulation::GetModelsInPedGroup(CPopulation::GetGangGroupId(gangId)), CStreaming::IsModelLoaded);
}

// 0x443A20
bool CGangWars::PickStreamedInPedForThisGang(eGangID gangId, int32& outPedId) {
    static auto& s_LastPickedPed = StaticRef<int32>(0x96ABD4);

    const auto groupId           = CPopulation::GetGangGroupId(gangId);
    for (auto i = 0; i < CPopulation::GetNumPedsInGroup(groupId); i++) {
        s_LastPickedPed = (s_LastPickedPed + 1) % CPopulation::GetNumPedsInGroup(groupId);
        outPedId        = CPopulation::GetPedGroupModelId(groupId, s_LastPickedPed);
        if (CStreaming::IsModelLoaded(outPedId)) {
            return true;
        }
    }
    return false;
}

// 0x443B00
bool CGangWars::PickZoneToAttack() {
    CCarCtrl::InitSequence(CTheZones::TotalNumberOfNavigationZones);

    // Pick an enemy zone (in random order)...
    for (auto i = 0; i < CTheZones::TotalNumberOfNavigationZones; i++) {
        auto* const enemyZone = CTheZones::GetNavigationZone(CCarCtrl::FindSequenceElement(i));
        if (enemyZone->m_ZoneInfoIndex == CTheZones::ZONE_INDEX_SAN_AND) {
            continue;
        }

        const auto* const enemyZoneInfo = CTheZones::GetZoneInfo(enemyZone);
        if (enemyZoneInfo->GangStrength[GANG_BALLAS] + enemyZoneInfo->GangStrength[GANG_VAGOS] < 20) {
            continue;
        }

        // ...and a Grove zone close to it, where the enemy will attack
        for (auto& zone : CTheZones::GetNavigationZones()) {
            if (zone.m_ZoneInfoIndex == CTheZones::ZONE_INDEX_SAN_AND) {
                continue;
            }

            // Not the zone of Grove Street itself
            if ((float)zone.m_fX1 <= 2500.0f && (float)zone.m_fX2 >= 2500.0f && (float)zone.m_fY1 <= -1666.0f && (float)zone.m_fY2 >= -1666.0f) {
                continue;
            }

            auto* const zoneInfo = CTheZones::GetZoneInfo(&zone);
            if (zoneInfo->GangStrength[GANG_GROVE] <= 15) {
                continue;
            }

            if (CTheZones::Calc2DDistanceBetween2Zones(enemyZone, &zone) >= 10.0f) {
                continue;
            }

            ms_ZoneInfoToFightOver = zoneInfo;
            ms_Gang1               = enemyZoneInfo->GangStrength[GANG_VAGOS] < enemyZoneInfo->GangStrength[GANG_BALLAS] ? GANG_BALLAS : GANG_VAGOS;
            ms_PointOfAttack       = CVector{
                (float)(zone.m_fX2 + zone.m_fX1) * 0.5f,
                (float)(zone.m_fY2 + zone.m_fY1) * 0.5f,
                10.0f
            };
            ms_ZoneToFightOver = &zone;
            if (DistanceBetweenPoints2D(ms_PointOfAttack, FindPlayerCoors()) > 60.0f) {
                return true;
            }
        }
    }

    return false;
}

// 0x445E20
void CGangWars::ReleaseCarsInAttackWave() {
    for (auto& vehicle : GetVehiclePool()->GetAllValid()) {
        if (vehicle.vehicleFlags.bPartOfAttackWave) {
            vehicle.vehicleFlags.bPartOfAttackWave = false;
            vehicle.SetVehicleCreatedBy(RANDOM_VEHICLE);
        }
    }
}

// Returns num of released peds
// 0x445C30
uint32 CGangWars::ReleasePedsInAttackWave(bool isEndOfWar, bool restoreGangPedsAcquaintance) {
    auto numReleasedPeds = 0u;

    for (auto& ped : GetPedPool()->GetAllValid()) {
        if (ped.bPartOfAttackWave) {
            ped.bPartOfAttackWave = false;
            ped.SetCharCreatedBy(PED_GAME);
            numReleasedPeds++;
            CRadar::ClearBlipForEntity(BLIP_CHAR, GetPedPool()->GetRef(&ped));
            ped.bClearRadarBlipOnDeath = false;

            if (restoreGangPedsAcquaintance) {
                ped.GetEventGroup().Add(CEventScriptCommand{ TASK_PRIMARY_PRIMARY, CTaskComplexWander::GetWanderTaskByPedType(&ped), false });
                ped.GetAcquaintance() = CPedType::GetPedTypeAcquaintances(ped.m_nPedType);
            }
        }

        if (isEndOfWar && ped.bClearRadarBlipOnDeath) {
            CRadar::ClearBlipForEntity(BLIP_CHAR, GetPedPool()->GetRef(&ped));
            ped.bClearRadarBlipOnDeath = false;
        }
    }

    return numReleasedPeds;
}

// 0x446570
void CGangWars::SetGangWarsActive(bool active) {
    if (active != ms_GangWarsActive) {
        CTheZones::FillZonesWithGangColours(!active);
        ms_TimeTillNextAttack = (float)CalculateTimeTillNextAttack();

        if (!active) {
            EndGangWar(false);
        }
    }
    ms_GangWarsActive = active;
}

// 0x444010
void CGangWars::SetSpecificZoneToTriggerGangWar(int32 zoneId) {
    ms_SpecificZones[ms_NumSpecificZones++] = zoneId;
    CTheZones::FillZonesWithGangColours(false);
}

// 0x444300
void CGangWars::StartDefensiveGangWar() {
    if (!PickZoneToAttack()) {
        ms_TimeTillNextAttack = (float)CalculateTimeTillNextAttack();
        return;
    }

    ms_Difficulty = ms_TerritoryUnderControlPercentage;
    CHud::SetHelpMessage(TheText.Get("GW_ATK"), true, false, true);
    ms_State2     = WAR_NOTIFIED;
    ms_FightTimer = (int32)(DistanceBetweenPoints2D(FindPlayerCoors(), ms_PointOfAttack) * 200.0f + 240'000.0f);
    ms_RadarBlip  = CRadar::SetCoordBlip(BLIP_COORD, ms_PointOfAttack, GetGangColor(ms_Gang1), BLIP_DISPLAY_BLIPONLY, "CODEGW1");
    CRadar::SetBlipSprite(ms_RadarBlip, [] {
        switch (ms_Gang1) {
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
    ms_IsPlayerCloseby                = false;
    ms_ZoneInfoToFightOver->RadarMode = 2;
    ms_ZoneInfoToFightOver->ZoneColor = CRGBA{ 255, 0, 0, 160 };
}

// 0x446050
void CGangWars::StartOffensiveGangWar() {
    ms_CoorsOfPlayerAtStartOfWar = FindPlayerCoors();
    auto* const zoneInfo         = CTheZones::GetZoneInfo(ms_CoorsOfPlayerAtStartOfWar, &ms_ZoneToFightOver);
    ms_ZoneInfoToFightOver       = zoneInfo;
    ms_Difficulty                = ms_TerritoryUnderControlPercentage;

    if (!CanPlayerStartAGangWarHere(zoneInfo)) {
        ms_Provocation = 0.0f;
        return;
    }

    // Find the strongest 2 gangs of the zone, not counting Grove.
    // NOTE: If no gang beats the previous one `ms_Gang1` / `ms_Gang2` keep their old values (like in the original)
    int32 gang1Density{}, gang2Density{}, densitySum{};
    for (auto&& [gang, density] : rngv::enumerate(zoneInfo->GangStrength)) {
        if (gang == GANG_GROVE) {
            continue;
        }
        densitySum += density;
        if (density > gang1Density) {
            ms_Gang2     = ms_Gang1;
            ms_Gang1     = (eGangID)gang;
            gang2Density = gang1Density;
            gang1Density = density;
        } else if (density > gang2Density) {
            ms_Gang2     = (int32)gang;
            gang2Density = density;
        }
    }

    ms_Provocation = 0.0f;
    if (densitySum == 0 || (ms_Gang1 != GANG_BALLAS && ms_Gang1 != GANG_VAGOS) || ms_State2 != NO_ATTACK) {
        return;
    }

    const auto provText = TheText.Get("GW_PROV");
    CMessages::AddMessageQ(provText, 4'500, 1, true);
    CMessages::AddToPreviousBriefArray(provText);
    ms_TimeStarted = CTimer::GetTimeInMS();
    ms_State       = PRE_FIRST_WAVE;
    ClearTheStreets();
    ms_WarFerocity = std::min(densitySum / 15, 2);

    if (CStats::GetStatValue(STAT_CITY_UNLOCKED) < 0.0f) {
        if (ms_TerritoryUnderControlPercentage <= 0.4f) {
            ms_WarFerocity = 0;
        } else if (ms_TerritoryUnderControlPercentage >= 0.5f) {
            ms_WarFerocity = 5;
        }
    }
    if (ms_IsTrainingMission) {
        ms_WarFerocity = 0;
    }

    if (gang1Density > gang2Density / 3) {
        ms_Gang2 = ms_Gang1;
    }

    TellGangMembersTo(false);
    ms_ZoneInfoToFightOver->RadarMode = 2;
    ms_ZoneInfoToFightOver->ZoneColor = CRGBA{ 255, 0, 0, 160 };
    ms_DriveByCar                     = nullptr;
}

// 0x445F50
void CGangWars::StrengthenPlayerInfluenceInZone(int32 density) {
    constexpr auto MAX_GROVE_DENSITY = 55;

    const auto controlledBefore      = DoesPlayerControlThisZone(ms_ZoneInfoToFightOver);

    auto& groveDensity               = ms_ZoneInfoToFightOver->GangStrength[GANG_GROVE];
    if (groveDensity < MAX_GROVE_DENSITY) {
        groveDensity = (uint8)std::min(groveDensity + density, MAX_GROVE_DENSITY);
    }

    if (!controlledBefore && DoesPlayerControlThisZone(ms_ZoneInfoToFightOver)) {
        CStats::IncrementStat(STAT_TERRITORIES_TAKEN_OVER, 1.0f);
    }
}

// 0x4465F0
void CGangWars::SwitchGangWarsActive() {
    SetGangWarsActive(!ms_GangWarsActive);
}

// 0x444530
void CGangWars::TellGangMembersTo(bool isGangWarEnding) {
    for (auto& ped : GetPedPool()->GetAllValid()) {
        if (ped.IsPlayer() || !ped.IsGangster() || ped.m_nPedType == PED_TYPE_GANG2) {
            continue;
        }

        if (!isGangWarEnding) {
            ped.GetEventGroup().Add(CEventScriptCommand{
                TASK_PRIMARY_PRIMARY, new CTaskComplexKillPedOnFoot{ FindPlayerPed(), -1, 0, 0, 0, 2 },
                  false
            });
            continue;
        }

        // NOTE: The original only checks `bInVehicle`, not `m_pVehicle`
        if (ped.bInVehicle) {
            if (ped.m_pVehicle->IsDriver(&ped)) { // 0x444604
                ped.GetEventGroup().Add(CEventScriptCommand{
                    TASK_PRIMARY_PRIMARY, new CTaskComplexCarDriveWander{ ped.m_pVehicle, DRIVING_STYLE_STOP_FOR_CARS, 10.0f },
                      false
                });
            } else { // 0x444680
                ped.GetEventGroup().Add(CEventScriptCommand{ TASK_PRIMARY_PRIMARY, new CTaskSimpleCarDrive{ ped.m_pVehicle }, false });
            }
            continue;
        }

        ped.GetEventGroup().Add(CEventScriptCommand{
            TASK_PRIMARY_PRIMARY, new CTaskComplexWanderGang{ PEDMOVE_WALK, (uint8)CGeneral::GetRandomNumberInRange(0, 8), 5'000, true, 0.5f },
              false
        });
    }
}

// FIX_BUGS: originally has int32* type, but changed to unsigned due possible UB
// 0x443D50
void CGangWars::TellStreamingWhichGangsAreNeeded(uint32& gangsBitFlags) {
    if (ms_State2 == NO_ATTACK) {
        return;
    }

    if (DistanceBetweenPoints2D(ms_PointOfAttack, FindPlayerCoors()) < 150.0f) {
        gangsBitFlags |= 1 << ms_Gang1;
    }
}

// 0x446610
void CGangWars::Update() {
    ZoneScoped;

    if (CTheScripts::IsPlayerOnAMission() && !ms_IsPlayerOnAMission && ms_NumSpecificZones == 0) {
        EndGangWar(true);
    }
    ms_IsPlayerOnAMission = CTheScripts::IsPlayerOnAMission();

    if (CCutsceneMgr::IsCutsceneProcessing()) {
        return;
    }

    if ((uint8)CTimer::GetFrameCounter() == 56) {
        UpdateTerritoryUnderControlPercentage();
    }

    if (!ms_GangWarsActive || CGameLogic::IsCoopGameGoingOn()) {
        return;
    }

    ms_Provocation = notsa::step_down_to(ms_Provocation, 0.0f, CTimer::GetTimeStep() * 0.0005f);

    // Offensive war: the attack waves
    const auto StartWave = [](eGangWarState next) {
        ms_State       = next;
        ms_TimeStarted = CTimer::GetTimeInMS();
    };
    const auto ClearedWave = [&](const char* msgKey, eGangWarState next) {
        ReleasePedsInAttackWave(false, false);
        ReleaseCarsInAttackWave();
        if (MakePlayerGainInfluenceInZone(0.3f)) {
            DoStuffWhenPlayerVictorious();
            return;
        }
        const auto msg = TheText.Get(msgKey);
        CMessages::AddMessageQ(msg, 4'500, 1, true);
        CMessages::AddToPreviousBriefArray(TheText.Get(msgKey));
        StartWave(next);
    };
    const auto waveDelayOver = CTimer::GetTimeInMS() > ms_TimeStarted + 10'000;
    switch (ms_State) {
    case NOT_IN_WAR: // 0x4466c5
        if (ms_Provocation > 2.1f) {
            StartOffensiveGangWar();
        }
        break;
    case PRE_FIRST_WAVE: // 0x4466e6
        if (waveDelayOver && CreateAttackWave(std::max(ms_WarFerocity, 0), 0)) {
            StartWave(FIRST_WAVE);
        }
        break;
    case FIRST_WAVE: // 0x446739
        if (AttackWaveOvercome()) {
            ClearedWave("GW_CLR1", PRE_SECOND_WAVE);
        }
        break;
    case PRE_SECOND_WAVE: // 0x4467c8
        if (waveDelayOver && CreateAttackWave(std::max(ms_WarFerocity + 1, 0), 1)) {
            StartWave(SECOND_WAVE);
        }
        break;
    case SECOND_WAVE: // 0x44681f
        if (AttackWaveOvercome()) {
            ClearedWave("GW_CLR2", PRE_THIRD_WAVE);
        }
        break;
    case PRE_THIRD_WAVE: // 0x4468a9
        if (waveDelayOver && CreateAttackWave(std::max(ms_WarFerocity + 2, 0), 2)) {
            StartWave(THIRD_WAVE);
        }
        break;
    case THIRD_WAVE: // 0x4468f5
        if (AttackWaveOvercome()) {
            MakePlayerGainInfluenceInZone(1.0f);
            DoStuffWhenPlayerVictorious();
        }
        break;
    default:
        NOTSA_UNREACHABLE();
    }

    // Leaving the zone of the offensive war ends it
    if (ms_State != NOT_IN_WAR) { // 0x44691c
        const CVector playerPos = FindPlayerCoors();
        const auto&   zone      = *ms_ZoneToFightOver;
        const auto    isInArea  = (float)zone.m_fX1 - 50.0f <= playerPos.x
            && (float)zone.m_fX2 + 50.0f >= playerPos.x
            && (float)zone.m_fY1 - 50.0f <= playerPos.y
            && (float)zone.m_fY2 + 50.0f >= playerPos.y;
        if (CGame::currArea != AREA_CODE_NORMAL_WORLD || isInArea) {
            ms_LastTimeInArea = CTimer::GetTimeInMS();
        } else if (CTimer::GetTimeInMS() - ms_LastTimeInArea > 30'000) { // 0x446a1f
            ms_State       = NOT_IN_WAR;
            ms_Provocation = 0.0f;
            ReleasePedsInAttackWave(true, false);
            ReleaseCarsInAttackWave();
            CTheZones::FillZonesWithGangColours(false);
            CMessages::AddMessageQ(TheText.Get("GW_FLEE"), 4500, 1, true);
            CMessages::AddToPreviousBriefArray(TheText.Get("GW_WARN"));                                                                 // NOTE: The original adds the warning to the brief, not `GW_FLEE`
        } else if (CTimer::GetTimeInMS() - ms_LastTimeInArea > 10'000 && CTimer::GetPreviousTimeInMS() - ms_LastTimeInArea <= 10'000) { // 0x446a55
            CMessages::AddMessageQ(TheText.Get("GW_WARN"), 4'500, 1, true);
            CMessages::AddToPreviousBriefArray(TheText.Get("GW_WARN"));
        }
    }

    // No enemy attack is started for at least 30 seconds after the offensive war, or while on a mission / in an interior
    if (ms_State != NOT_IN_WAR || CTheScripts::IsPlayerOnAMission() || FindPlayerCoors().z > 950.0f) { // 0x446aaa
        ms_TimeTillNextAttack = std::max(ms_TimeTillNextAttack, 30'000.0f);
    }

    // Defensive war: an enemy gang attacks one of the player's zones
    switch (ms_State2) {
    case NO_ATTACK: { // 0x446d7a
        if (CTheScripts::IsPlayerOnAMission() || ms_IsTrainingMission) {
            break;
        }
        if (const auto* const veh = FindPlayerVehicle(); veh && (veh->IsSubHeli() || veh->IsSubPlane())) {
            break;
        }
        if (ms_State == NOT_IN_WAR) {
            const auto timeStepMs = (float)(uint32)CTimer::GetTimeStepInMS();
            ms_TimeTillNextAttack -= CWeather::WeatherRegion == WEATHER_REGION_LA
                ? timeStepMs
                : timeStepMs * 0.06f;
        }
        if (ms_TimeTillNextAttack < 0.0f) {
            StartDefensiveGangWar();
        }
        break;
    }
    case WAR_NOTIFIED: { // 0x446c5c
        if (DistanceBetweenPoints2D(FindPlayerCoors(), ms_PointOfAttack) < 70.0f) {
            if (CreateDefendingGroup(0)) {
                ms_FightTimer += 30'000;
                ms_State2 = PLAYER_CAME_TO_WAR;
            }
            break;
        }
        ms_FightTimer -= (int32)CTimer::GetTimeStepInMS();
        if (ms_FightTimer < 0) { // 0x446ced - The player didn't show up
            CMessages::AddMessageQ(TheText.Get("GW_NOSH"), 4'500, 1, true);
            CMessages::AddToPreviousBriefArray(TheText.Get("GW_NOSH"));
            ms_State2 = NO_ATTACK;
            MakeEnemyGainInfluenceInZone(ms_Gang1, 30);
            CTheZones::FillZonesWithGangColours(false);
            ms_TimeTillNextAttack = (float)CalculateTimeTillNextAttack();
            CStats::DecrementStat(STAT_RESPECT, 30.0f);
        }
        break;
    }
    case PLAYER_CAME_TO_WAR: { // 0x446b12
        if (AttackWaveOvercome()) {
            ms_State2             = NO_ATTACK;
            ms_TimeTillNextAttack = (float)CalculateTimeTillNextAttack();
            CMessages::AddMessageQ(TheText.Get("GW_WON"), 4'500, 1, true);
            CMessages::AddToPreviousBriefArray(TheText.Get("GW_WON"));
            StrengthenPlayerInfluenceInZone(10);
            CTheZones::FillZonesWithGangColours(false);
            ReleasePedsInAttackWave(true, false);
            break;
        }
        ms_FightTimer -= (int32)CTimer::GetTimeStepInMS();
        if (ms_FightTimer < 0) { // 0x446bc5 - The player was too slow
            ms_State2             = NO_ATTACK;
            ms_TimeTillNextAttack = (float)CalculateTimeTillNextAttack();
            CMessages::AddMessageQ(TheText.Get("GW_SLOW"), 4'500, 1, true);
            CMessages::AddToPreviousBriefArray(TheText.Get("GW_SLOW"));
            MakeEnemyGainInfluenceInZone(ms_Gang1, 3 * ReleasePedsInAttackWave(true, false));
            CTheZones::FillZonesWithGangColours(false);
            CStats::DecrementStat(STAT_RESPECT, 30.0f);
        }
        break;
    }
    default:
        break;
    }

    if (ms_RadarBlip) { // 0x446e4b
        if (ms_State2 == NO_ATTACK || ms_State2 == PLAYER_CAME_TO_WAR) {
            CRadar::ClearBlip(ms_RadarBlip);
            ms_RadarBlip = 0;
        } else {
            // Blinks faster as the time runs out
            const auto blinkShift = ms_FightTimer > 120'000 ? 10
                : ms_FightTimer > 60'000                    ? 9
                : ms_FightTimer > 30'000                    ? 8
                                                            : 7;
            CRadar::ChangeBlipDisplay(ms_RadarBlip, (CTimer::GetTimeInMS() >> blinkShift) & 1 ? BLIP_DISPLAY_NEITHER : BLIP_DISPLAY_BLIPONLY);
        }
    }

    if (ms_State2 != NO_ATTACK) { // 0x446ece
        if (DistanceBetweenPoints2D(FindPlayerCoors(), ms_PointOfAttack) >= 150.0f) {
            ms_IsPlayerCloseby = false;
        } else if (!ms_IsPlayerCloseby) {
            CStreaming::StreamZoneModels_Gangs(FindPlayerCoors());
            ms_IsPlayerCloseby = true;
        }
    }
}

// 0x443DE0
void CGangWars::UpdateTerritoryUnderControlPercentage() {
    auto groveZones = 0, ballasZones = 0, vagosZones = 0;
    for (auto& zone : CTheZones::GetNavigationZones()) {
        if (zone.m_ZoneInfoIndex == CTheZones::ZONE_INDEX_SAN_AND) {
            continue;
        }

        const auto& strength      = CTheZones::GetZoneInfo(&zone)->GangStrength;
        const auto  enemyStrength = strength[GANG_BALLAS] + strength[GANG_VAGOS];
        if (enemyStrength == 0 && strength[GANG_GROVE] == 0) {
            continue;
        }

        if (enemyStrength <= strength[GANG_GROVE]) {
            groveZones++;
        } else if (strength[GANG_BALLAS] > strength[GANG_VAGOS]) {
            ballasZones++;
        } else {
            vagosZones++;
        }
    }

    CStats::SetStatValue(STAT_TERRITORIES_HELD, (float)groveZones);
    CStats::SetStatValue(STAT_HIGHEST_NUMBER_OF_TERRITORIES_HELD, std::max(CStats::GetStatValue(STAT_HIGHEST_NUMBER_OF_TERRITORIES_HELD), (float)groveZones));

    const auto allGangZones = groveZones + ballasZones + vagosZones;
    if (allGangZones == 0) {
        ms_TerritoryUnderControlPercentage = 0.0f;
        return;
    }
    ms_TerritoryUnderControlPercentage = (float)groveZones / (float)allGangZones;

    // Rank the gangs by the number of zones they hold (stable, like the original's bubble sort)
    struct GangRanking {
        eGangID Gang;
        int32   NumZones;
    };
    std::array<GangRanking, 3> ranking{
        {
         { GANG_GROVE, groveZones },
         { GANG_BALLAS, ballasZones },
         { GANG_VAGOS, vagosZones },
         }
    };
    rng::stable_sort(ranking, rng::greater{}, &GangRanking::NumZones);
    for (auto&& [i, r] : rngv::enumerate(ranking)) {
        ms_GangRatings[i]        = r.Gang;
        ms_GangRatingStrength[i] = r.NumZones;
    }
}

// 0x44582F NOTSA
eBlipColour CGangWars::GetGangColor(int32 gang) {
    // 0x8D1344, 0x8D1350, 0x8D135C
    static constexpr uint8 gaGangColoursR[] = { 200,  70, 255,   0, 255, 200, 240,   0, 255, 255, 0, 0 };
    static constexpr uint8 gaGangColoursG[] = {   0, 200, 200,   0, 220, 200, 140, 200, 255, 255, 0, 0 };
    static constexpr uint8 gaGangColoursB[] = { 200, 0, 0, 200, 190, 200, 240, 255, 255, 255, 0, 0 };

    return (eBlipColour)(gaGangColoursR[gang] << 24 | gaGangColoursG[gang] << 16 | gaGangColoursB[gang] << 8 | 0xFF);
}

void CGangWars::InjectHooks() {
    RH_ScopedClass(CGangWars);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(InitAtStartOfGame, 0x443920);
    RH_ScopedInstall(AddKillToProvocation, 0x443950);
    RH_ScopedInstall(AttackWaveOvercome, 0x445B30);
    RH_ScopedInstall(CalculateTimeTillNextAttack, 0x443DB0);
    RH_ScopedInstall(CanPlayerStartAGangWarHere, 0x443F80);
    RH_ScopedInstall(CheerVictory, 0x444040);
    RH_ScopedInstall(ClearSpecificZonesToTriggerGangWar, 0x443FF0);
    RH_ScopedInstall(ClearTheStreets, 0x4444B0);
    RH_ScopedInstall(CreateAttackWave, 0x444810);
    RH_ScopedInstall(CreateDefendingGroup, 0x4453D0);
    RH_ScopedInstall(DoesPlayerControlThisZone, 0x443AE0);
    RH_ScopedInstall(DoStuffWhenPlayerVictorious, 0x446400);
    RH_ScopedInstall(DontCreateCivilians, 0x4439C0);
    RH_ScopedInstall(EndGangWar, 0x4464C0);
    RH_ScopedInstall(GangWarFightingGoingOn, 0x443AC0);
    RH_ScopedInstall(GangWarGoingOn, 0x443AA0);
    RH_ScopedInstall(MakeEnemyGainInfluenceInZone, 0x445FD0);
    RH_ScopedInstall(MakePlayerGainInfluenceInZone, 0x445E80);
    RH_ScopedInstall(PedStreamedInForThisGang, 0x4439D0);
    RH_ScopedInstall(PickStreamedInPedForThisGang, 0x443A20);
    RH_ScopedInstall(PickZoneToAttack, 0x443B00);
    RH_ScopedInstall(ReleaseCarsInAttackWave, 0x445E20);
    RH_ScopedInstall(ReleasePedsInAttackWave, 0x445C30);
    RH_ScopedInstall(SetGangWarsActive, 0x446570);
    RH_ScopedInstall(SetSpecificZoneToTriggerGangWar, 0x444010);
    RH_ScopedInstall(StartDefensiveGangWar, 0x444300);
    RH_ScopedInstall(StartOffensiveGangWar, 0x446050);
    RH_ScopedInstall(StrengthenPlayerInfluenceInZone, 0x445F50);
    RH_ScopedInstall(SwitchGangWarsActive, 0x4465F0);
    RH_ScopedInstall(TellGangMembersTo, 0x444530);
    RH_ScopedInstall(TellStreamingWhichGangsAreNeeded, 0x443D50);
    RH_ScopedInstall(Update, 0x446610);
    RH_ScopedInstall(UpdateTerritoryUnderControlPercentage, 0x443DE0);
    RH_ScopedInstall(Load, 0x5D3EB0);
    RH_ScopedInstall(Save, 0x5D5530);
}
