#include "StdInc.h"

#include "RoadBlocks.h"
#include "PedPlacement.h"
#include "ModelIndices.h"
#include "TaskComplexWanderCop.h"
#include "TaskSimpleStandStill.h"
#include <extensions/File.hpp>

void CRoadBlocks::InjectHooks() {
    RH_ScopedClass(CRoadBlocks);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x461100);
    RH_ScopedInstall(ClearScriptRoadBlocks, 0x460EC0);
    RH_ScopedInstall(ClearSpaceForRoadBlockObject, 0x461020);
    RH_ScopedInstall(CreateRoadBlockBetween2Points, 0x4619C0);
    RH_ScopedInstall(GenerateRoadBlockPedsForCar, 0x461170);
    RH_ScopedInstall(GenerateRoadBlocks, 0x4629E0);
    RH_ScopedInstall(GetRoadBlockNodeInfo, 0x460EE0);
    RH_ScopedInstall(RegisterScriptRoadBlock, 0x460DF0);
}

// 0x461100
void CRoadBlocks::Init() {
    rng::fill(InOrOut, true);
    GenerateDynamicRoadBlocks = false;

    if (notsa::File rbx("data\\paths\\roadblox.dat", "rb"); rbx) {
        rbx.Read(&NumRoadBlocks, sizeof(int32));
        assert(NumRoadBlocks <= MAX_ROADBLOCKS);
        rbx.Read(RoadBlockNodes.data(), RoadBlockNodes.size() * sizeof(CNodeAddress));
    } else {
        NOTSA_UNREACHABLE("roadblox.dat couldn't be opened!");
    }
    ClearScriptRoadBlocks();
}

// 0x460EC0
void CRoadBlocks::ClearScriptRoadBlocks() {
    for (auto& srb : aScriptRoadBlocks) {
        srb.IsActive = false;
    }
}

// 0x461020
// Returns true if cleared successfully.
bool CRoadBlocks::ClearSpaceForRoadBlockObject(CVector cornerA, CVector cornerB){
    int16 numEntities{};
    CEntity* entities[2]{};
    CWorld::FindObjectsIntersectingCube(
        cornerA,
        cornerB,
        &numEntities,
        std::size(entities),
        entities,
        false,
        true,
        true,
        true,
        false
    );

    if (numEntities > std::size(entities) || numEntities <= 0) {
        return numEntities <= 0;
    }

    const auto Remove = [](CEntity* e) {
        CWorld::Remove(e);
        delete e;
    };

    for (auto* entity : entities | rngv::take(numEntities)) {
        switch (entity->GetType()) {
        case ENTITY_TYPE_VEHICLE:
            if (auto* v = entity->AsVehicle(); !v->CanBeDeleted()) {
                return false;
            } else if (!v->vehicleFlags.bCreateRoadBlockPeds) {
                Remove(v);
            }
            break;
        case ENTITY_TYPE_PED:
            if (auto* p = entity->AsPed(); p->CanBeDeleted()) {
                Remove(p);
            } else {
                return false;
            }
            break;
        case ENTITY_TYPE_OBJECT:
            if (auto* o = entity->AsObject(); o->CanBeDeleted() && o->m_nObjectType != OBJECT_GAME) {
                Remove(o);
            } else {
                return false;
            }
            break;
        default:
            NOTSA_UNREACHABLE();
        }
    }

    return true;
}

// 0x4619C0
void CRoadBlocks::CreateRoadBlockBetween2Points(CVector a, CVector b, bool isGangRoadBlock) {
    constexpr auto MAX_VEHICLES          = 5;
    constexpr auto MAX_BARRIERS          = 8;
    constexpr auto TEMP_OBJECT_LIFE_MS   = 600'000u;
    constexpr auto FIRE_SIZE             = 2.8f;
    constexpr auto FIRE_BURN_TIME_MS     = 60'000u;
    constexpr auto FIRE_NUM_GENERATIONS  = 2;
    constexpr auto FIRE_REMOVAL_DISTANCE = 92;

    const float   length                 = (b - a).Magnitude();
    const CVector center                 = (a + b) * 0.5f;
    const CVector dir                    = (b - a).Normalized();
    const CVector dirPerpRight{ CVector2D{ dir }.GetPerpRight() };

    // Perpendicular to the road block, pointing towards the player's side of it
    const CVector perp         = dirPerpRight.Normalized();
    const CVector toPlayerSide = (FindPlayerCoors() - center).Dot(perp) < 0.f ? -perp : perp;

    const auto GetRandomHeading = [](float scale) {
        return CGeneral::GetRandomNumberInRange(-128.f, 128.f) * scale;
    };
    const auto GetCornersOfBB = [](const CMatrix& mat, const CBoundingBox& bb) {
        // NOTE: The original uses the inverse (0x59C810) of the rotation here
        return std::make_pair(mat.InverseTransformVector(bb.m_vecMin) + mat.GetPosition(), mat.InverseTransformVector(bb.m_vecMax) + mat.GetPosition());
    };
    const auto SetAsTempObject = [](CObject* obj) {
        CObject::nNoTempObjects++;
        obj->m_nObjectType  = OBJECT_TEMPORARY;
        obj->m_nRemovalTime = CTimer::GetTimeInMS() + TEMP_OBJECT_LIFE_MS;
        CWorld::Add(obj);
    };

    const auto vehModel = [&]() -> eModelID {
        if (isGangRoadBlock) {
            return CPopulation::PickRiotRoadBlockCar();
        }
        const eModelID model = FindPlayerWanted()->AreArmyRequired() // 0x461B9C
            ? MODEL_BARRACKS
            : FindPlayerWanted()->AreFbiRequired()
                ? MODEL_FBIRANCH
                : FindPlayerWanted()->AreSwatRequired()
                ? MODEL_ENFORCER
                : CStreaming::GetDefaultCopCarModel(false);
        return CStreaming::IsModelLoaded(model) ? model : CStreaming::GetDefaultCopCarModel(false);
    }();
    if (vehModel == (isGangRoadBlock ? MODEL_INVALID : MODEL_COPBIKE)) {
        return;
    }

    // 0x461C2D - How many vehicles fit, and which of them are parked across the road block (Facing the player's side)
    const auto& vehBB       = CModelInfo::GetModelInfo(vehModel)->GetColModel()->GetBoundingBox();
    const float spaceAcross = vehBB.GetWidth() + 2.f;
    const float spaceAlong  = vehBB.GetLength() + 0.2f + (isGangRoadBlock ? 0.5f : 0.f);

    std::array<bool, MAX_VEHICLES> isParkedAcross{};
    float                          spaceLeft = length;
    int32                          numVehs   = 0;
    for (; numVehs < MAX_VEHICLES; numVehs++) {
        isParkedAcross[numVehs] = vehModel != MODEL_BARRACKS && !isGangRoadBlock && CGeneral::DoCoinFlip();
        const float space       = isParkedAcross[numVehs] ? spaceAcross : spaceAlong;
        if (spaceLeft < space) {
            break;
        }
        spaceLeft -= space;
    }

    CMatrix     mat{};
    const float gapBetweenVehs = spaceLeft / (float)(numVehs + 1);
    float       distAlong      = gapBetweenVehs;
    for (int32 i = 0; i < numVehs; i++) { // 0x461D72
        bool isOnItsSide = false;
        if (isParkedAcross[i]) {
            distAlong += spaceAcross * 0.5f;
            mat.GetRight()   = CVector{ toPlayerSide.y, -toPlayerSide.x, 0.f };
            mat.GetForward() = toPlayerSide;
            mat.GetUp()      = CVector{ 0.f, 0.f, 1.f };
            mat.RotateZ(GetRandomHeading(isGangRoadBlock ? 0.004f : 0.002f));
        } else { // 0x461E33
            distAlong += spaceAlong * 0.5f;
            if (isGangRoadBlock) {
                isOnItsSide = CGeneral::RandomBool(25.f);
            }
            mat.GetRight()   = isOnItsSide ? CVector{ 0.f, 0.f, 1.f } : dirPerpRight;
            mat.GetForward() = dir;
            mat.GetUp()      = isOnItsSide ? CVector{ CVector2D{ dir }.GetPerpLeft() } : CVector{ 0.f, 0.f, 1.f };
            if (CGeneral::DoCoinFlip()) {
                if (isOnItsSide) {
                    mat.RotateY(PI);
                } else {
                    mat.RotateZ(PI);
                }
            }
            mat.RotateZ(GetRandomHeading(isGangRoadBlock ? 0.006f : 0.003f));
        }

        // 0x461F58
        mat.SetTranslateOnly(a + dir * distAlong);
        mat.GetPosition().z = mat.GetPosition().z - (isOnItsSide ? vehBB.m_vecMin.x : vehBB.m_vecMin.z) + 0.3f;

        // NOTE: The original always steps by the half of `spaceAlong` here, also after a vehicle parked across
        distAlong += spaceAlong * 0.5f + gapBetweenVehs;

        const auto [cornerA, cornerB] = GetCornersOfBB(mat, vehBB);
        if (!ClearSpaceForRoadBlockObject(cornerA, cornerB)) {
            continue;
        }

        if (isGangRoadBlock && (isOnItsSide || CGeneral::RandomBool(25.f))) { // 0x462110 - A burning wreck
            auto* const wreck  = new CObject(ModelIndices::MI_ROADBLOCKFUCKEDCAR1, true);
            wreck->GetMatrix() = mat;
            wreck->SetPosn(mat.GetPosition());
            wreck->SetIsStatic(false);
            SetAsTempObject(wreck);
            gFireManager.StartFire(wreck, nullptr, FIRE_SIZE, 1, FIRE_BURN_TIME_MS, FIRE_NUM_GENERATIONS);
            if (wreck->m_pFire) {
                wreck->m_pFire->SetRemovalDist(FIRE_REMOVAL_DISTANCE);
            }
            continue;
        }

        // 0x4621EF
        auto* const veh = new CAutomobile(vehModel, RANDOM_VEHICLE, true);
        veh->SetStatus(STATUS_ABANDONED);
        mat.GetPosition().z += veh->GetHeightAboveRoad() - 0.6f;
        veh->GetMatrix() = mat;
        veh->PlaceOnRoadProperly();
        veh->SetIsStatic(false);
        veh->UpdateRwMatrix();
        veh->m_nDoorLock = CARLOCK_UNLOCKED;
        CCarCtrl::JoinCarWithRoadSystem(veh);
        veh->m_autoPilot.ClearCarMission();
        veh->m_autoPilot.ClearTempAct();
        veh->m_autoPilot.m_nCurrentLane = 0;
        veh->m_autoPilot.m_nNextLane    = 0;
        veh->m_autoPilot.m_speed        = 0.f;
        veh->m_autoPilot.SetCruiseSpeed(0);
        veh->vehicleFlags.bNeverUseSmallerRemovalRange = true;
        veh->vehicleFlags.bIsLocked                    = false;
        veh->vehicleFlags.bEngineOn                    = false;

        bool isOnFire                                  = false;
        if (isGangRoadBlock) { // 0x4622F4
            if (CGeneral::RandomBool(25.f)) {
                veh->SetTotalDamage(true);
            } else {
                veh->BlowUpCarCutSceneNoExtras(true, true, true, true);
                veh->m_nTimeWhenBlowedUp += 1'000'000;
                isOnFire = true;
            }
        } else if (veh->UsesSiren() && CGeneral::DoCoinFlip()) { // 0x462342
            veh->vehicleFlags.bSirenOrAlarm = true;
        }

        if (veh->GetMatrix().GetUp().z <= 0.94f) { // 0x4624BE - On a too steep slope
            delete veh;
            continue;
        }

        // 0x462374
        CVisibilityPlugins::SetClumpAlpha(veh->GetRpClump(), 0);
        CWorld::Add(veh);
        veh->vehicleFlags.bCreateRoadBlockPeds = true;
        veh->m_nTimeTillWeNeedThisCar          = CTimer::GetTimeInMS() + 7'000;
        veh->m_nNumPedsForRoadBlock            = numVehs > 3 ? 1 : 2;
        if (isParkedAcross[i]) { // 0x4623C6 - Peds take cover behind the open front doors
            veh->m_nPedsPositionForRoadBlock = 2;
            const auto OpenDoorIfPresent     = [veh](eDoors door) {
                const auto node = CDamageManager::GetCarNodeIndexFromDoor(door);
                if (veh->m_aCarNodes[node]) {
                    veh->OpenDoor(nullptr, node, door, 1.f, true);
                }
            };
            OpenDoorIfPresent(DOOR_LEFT_FRONT);
            if (veh->m_nNumPedsForRoadBlock > 1) {
                OpenDoorIfPresent(DOOR_RIGHT_FRONT);
            }
        } else { // 0x46242A - Peds stand on the side that is further from the player
            veh->m_nPedsPositionForRoadBlock = (veh->GetPosition() - FindPlayerCoors()).Dot(veh->GetRight()) < 0.f ? 0 : 1;
        }

        if (isOnFire) { // 0x462494
            gFireManager.StartFire(veh, nullptr, FIRE_SIZE, 1, FIRE_BURN_TIME_MS, FIRE_NUM_GENERATIONS);
            if (veh->m_pFire) {
                veh->m_pFire->SetRemovalDist(FIRE_REMOVAL_DISTANCE);
            }
        }
    }

    if (isGangRoadBlock) {
        return;
    }

    // 0x4624EE - A row of barriers in front of the vehicles (Only the cops have these)
    const auto  barrierModel       = ModelIndices::MI_ROADWORKBARRIER1;
    const auto& barrierBB          = CModelInfo::GetModelInfo(barrierModel)->GetColModel()->GetBoundingBox();
    const float barrierSpace       = barrierBB.GetWidth() + 0.5f;
    const int32 numBarriers        = std::min((int32)(length / barrierSpace), MAX_BARRIERS);
    const float gapBetweenBarriers = (length - (float)numBarriers * barrierSpace) / (float)(numBarriers + 1);

    CObject::DeleteAllTempObjectsInArea(center, length * 0.5f);

    for (int32 i = 0; i < numBarriers; i++) { // 0x4625E0
        const float barrierDistAlong = ((float)i + 0.5f) * barrierSpace + (float)(i + 1) * gapBetweenBarriers;

        mat.GetRight()               = dir;
        mat.GetForward()             = dirPerpRight;
        mat.GetUp()                  = CVector{ 0.f, 0.f, 1.f };
        mat.GetPosition()            = CVector{};
        mat.RotateZ(GetRandomHeading(0.003f));
        mat.SetTranslateOnly(a + dir * barrierDistAlong + toPlayerSide * 5.f);
        mat.GetPosition().x += CGeneral::GetRandomNumberInRange(0.f, 0.75f);
        mat.GetPosition().y += CGeneral::GetRandomNumberInRange(0.f, 0.75f);

        bool isGroundFound{};
        mat.GetPosition().z = CWorld::FindGroundZFor3DCoord(mat.GetPosition() + CVector{ 0.f, 0.f, 2.f }, &isGroundFound);
        if (!isGroundFound) {
            continue;
        }
        mat.GetPosition().z -= barrierBB.m_vecMin.z;

        const auto [cornerA, cornerB] = GetCornersOfBB(mat, barrierBB);
        if (!ClearSpaceForRoadBlockObject(cornerA, cornerB)) {
            continue;
        }

        auto* const barrier  = new CObject(barrierModel, true);
        barrier->GetMatrix() = mat;
        barrier->SetPosn(mat.GetPosition());
        SetAsTempObject(barrier);
    }
}

// 0x461170
void CRoadBlocks::GenerateRoadBlockPedsForCar(CVehicle* vehicle, int32 pedsPositionsType, ePedType pedType) {
    const auto Generate = [&](eModelID pedModel = MODEL_INVALID, eCopType copType = COP_TYPE_CITYCOP, bool isSpecialCop = false) {
        static constexpr auto PLACEMENTS = std::to_array<CVector>({
            { -1.5f, +1.9f, 0.0f },
            { -1.5f, -2.6f, 0.0f },
            { +1.5f, +1.9f, 0.0f },
            { +1.5f, -2.6f, 0.0f },
            { -1.5f,  0.0f, 0.0f },
            { +1.5f,  0.0f, 0.0f },
        });

        static constexpr auto SPECIAL_COP_PLACEMENTS = std::to_array<CVector>({
            {  0.0f, +3.2f, 0.0f },
            { +1.5f, -1.8f, 0.0f },
            {  0.0f, +3.2f, 0.0f },
            { -1.5f, -1.8f, 0.0f },
            { -1.5f,  0.0f, 0.0f },
            { +1.5f,  0.0f, 0.0f },
        });

        const auto placementIdx = 2 * pedsPositionsType;
        const auto radiusRatio  = vehicle->GetColModel()->GetBoundingSphere().m_fRadius
            / CModelInfo::GetModelInfo(CStreaming::GetDefaultCopCarModel(false))->GetColModel()->GetBoundingSphere().m_fRadius;

        for (auto i = 0u; i < vehicle->m_nNumPedsForRoadBlock; i++) {
            const auto offset = (isSpecialCop ? SPECIAL_COP_PLACEMENTS : PLACEMENTS)[placementIdx + i] * radiusRatio;
            const auto pos = vehicle->GetMatrix().TransformPoint(offset);

            auto* ped = [&]() -> CPed* {
                if (pedType != PED_TYPE_COP) { // 0x461560
                    return new CCivilianPed(pedType, pedModel);
                } else {
                    auto* p = new CCopPed(CStreaming::IsModelLoaded(pedModel) ? copType : COP_TYPE_CITYCOP);
                    if (copType == COP_TYPE_CITYCOP) {
                        p->SetCurrentWeapon(WEAPON_PISTOL);
                    }
                    return p;
                }
            }();

            ped->SetPosn(std::get<CVector>(CPedPlacement::FindZCoorForPed(pos)));
            ped->GetMatrix().SetRotateKeepPos({ 0.0f, 0.0f, -HALF_PI });

            if (pedType == PED_TYPE_COP) {
                auto* t = new CTaskComplexWanderCop(PEDMOVE_STILL, CGeneral::GetRandomNumberInRange(8ui8));
                t->m_nSubTaskCreatedTimer = {};
                t->m_nScanForStuffTimer   = {};
                ped->GetTaskManager().SetTask(t, TASK_PRIMARY_PRIMARY);
            }
            ped->GetTaskManager().SetTask(new CTaskSimpleStandStill(0, true), TASK_PRIMARY_DEFAULT);

            ped->bStayInSamePlace         = true;
            ped->bNotAllowedToDuck        = true;
            ped->m_nTimeTillWeNeedThisPed = CTimer::GetTimeInMS() + 10'000;
            ped->bCrouchWhenShooting      = !isSpecialCop || pedsPositionsType != 2;
            ped->bCullExtraFarAway        = true;
            CEntity::RegisterReference(ped->m_pVehicle = vehicle);
            CVisibilityPlugins::SetClumpAlpha(ped->GetRpClump(), 0);

            if (pedType != PED_TYPE_COP) {
                const auto weapon = CGangs::Gang[pedType - PED_TYPE_GANG1].GetRandomWeapon(false);
                if (weapon != WEAPON_UNARMED) {
                    ped->GiveDelayedWeapon(weapon, 25'001);
                    ped->SetCurrentWeapon(weapon);
                }
            }
            CWorld::Add(ped);
            ped->GetEventGroup().Add<CEventScriptCommand>({ TASK_PRIMARY_PRIMARY, new CTaskComplexKillPedOnFoot(FindPlayerPed()) });
        }
    };

    if (pedType == PED_TYPE_COP) {
        switch (vehicle->GetModelId()) {
        case MODEL_ENFORCER: Generate(MODEL_SWAT,    COP_TYPE_SWAT1,   true); break;
        case MODEL_BARRACKS: Generate(MODEL_ARMY,    COP_TYPE_ARMY,    true); break;
        case MODEL_FBIRANCH: Generate(MODEL_FBI,     COP_TYPE_FBI,     true); break;
        case MODEL_COPCARRU: Generate(MODEL_INVALID, COP_TYPE_CITYCOP, true); break;
        default:             Generate(MODEL_INVALID, COP_TYPE_CITYCOP, false); break;
        }
    } else if (IsPedTypeGang(pedType)) {
        for (auto i = 0; i < TOTAL_GANGS; i++) {
            if (!CPopCycle::m_pCurrZoneInfo->GangStrength[i]) {
                continue;
            }
            const auto pedModel = CGangs::ChooseGangPedModel((eGangID)i);
            if (pedModel == MODEL_INVALID) {
                continue;
            }
            Generate(pedModel);
            return;
        }
    } else {
        Generate();
    }
}

// 0x4629E0
void CRoadBlocks::GenerateRoadBlocks() {
    ZoneScoped;

    if (FindPlayerWanted()->m_ChanceOnRoadBlock && FindPlayerVehicle()) {
        if (!GenerateDynamicRoadBlocks) {
            rng::fill(InOrOut, true);
            GenerateDynamicRoadBlocks = true;
        }

        const auto counter1      = MAX_ROADBLOCKS * (CTimer::GetFrameCounter() % 16 + 1);
        const auto rbsToGenerate = std::min((uint32)NumRoadBlocks, ((counter1 % 16) + counter1) / 16);
        auto       counter2      = MAX_ROADBLOCKS * (CTimer::GetFrameCounter() % 16) / 16;

        for (; counter2 < rbsToGenerate; counter2++) {
            const auto& mrbNode = RoadBlockNodes[counter2];
            if (!ThePaths.IsAreaLoaded(mrbNode)) {
                continue;
            }
            const auto& mainNode = ThePaths.GetPathNode(mrbNode);
            const auto  playerPos = FindPlayerCoors();
            if (std::abs(playerPos.x - mainNode->GetPosition().x) >= 90.0f ||
                std::abs(playerPos.y - mainNode->GetPosition().y) >= 90.0f ||
                DistanceBetweenPoints2D(playerPos, mainNode->GetPosition()) >= 90.0f)
            {
                InOrOut[counter2] = false;
                continue;
            }

            if (InOrOut[counter2]) {
                continue;
            }
            InOrOut[counter2] = true;

            if (CGeneral::GetRandomNumberInRange(128u) >= FindPlayerWanted()->m_ChanceOnRoadBlock) {
                continue;
            }

            float mrbWidth{};
            CVector mrbDir{};
            if (!GetRoadBlockNodeInfo(mrbNode, mrbWidth, mrbDir)) {
                continue;
            }

            if (mainNode->m_nPathWidth) {
                const auto width = mainNode->m_nPathWidth / 16.0f;
                CreateRoadBlockBetween2Points(
                    mainNode->GetPosition() + mrbDir * (mrbWidth / 2.f + width),
                    mainNode->GetPosition() + mrbDir * width,
                    false
                );
                CreateRoadBlockBetween2Points(
                    mainNode->GetPosition() - mrbDir * width,
                    mainNode->GetPosition() - mrbDir * (mrbWidth / 2.f + width),
                    false
                );
                continue;
            }

            for (auto&& [i, nodeAddr] : rngv::enumerate(RoadBlockNodes)) {
                if (counter2 == i || InOrOut[i] || !ThePaths.IsAreaLoaded(nodeAddr.m_wAreaId)) {
                    continue;
                }
                const auto& node = ThePaths.GetPathNode(nodeAddr);

                if (std::abs(mainNode->GetPosition().x - node->GetPosition().x) >= 30.0f ||
                    std::abs(mainNode->GetPosition().y - node->GetPosition().y) >= 30.0f)
                {
                    continue;
                }

                float   width{};
                CVector dir{}; 
                if (!GetRoadBlockNodeInfo(nodeAddr, width, dir)) {
                    continue;
                }

                if (mrbWidth != width || dir.Dot(mrbDir) <= 0.7f) {
                    continue;
                }

                [[maybe_unused]] CColPoint col{};
                [[maybe_unused]] CEntity*  colEntity{};
                if (CWorld::ProcessLineOfSight(
                    mainNode->GetPosition() + CVector{0.0f, 0.0f, 1.0f},
                    node->GetPosition() + CVector{0.0f, 0.0f, 1.0f},
                    col,
                    colEntity,
                    true,
                    false,
                    false,
                    false,
                    false,
                    false,
                    false,
                    false
                )) {
                    continue;
                }

                const auto dirFromMain = (node->GetPosition() - mainNode->GetPosition()).Normalized();
                CreateRoadBlockBetween2Points(
                    node->GetPosition()     + dirFromMain * (mrbWidth / 2.0f),
                    mainNode->GetPosition() - dirFromMain * (mrbWidth / 2.0f),
                    false
                );
                InOrOut[i] = true;

                if (i == NumRoadBlocks) {
                    CreateRoadBlockBetween2Points(
                        mainNode->GetPosition() - dirFromMain * (mrbWidth / 2.0f),
                        mainNode->GetPosition() + dirFromMain * (mrbWidth / 2.0f),
                        false
                    );
                    break;
                }
            }
        }
    } else {
        GenerateDynamicRoadBlocks = false;
    }

    if (auto& srb = aScriptRoadBlocks[CTimer::GetFrameCounter() % MAX_SCRIPT_ROADBLOCKS]; srb.IsActive) {
        const auto c = CVector::Centroid({ srb.CornerA, srb.CornerB });

        if (DistanceBetweenPoints(FindPlayerCoors(), c) >= 90.0f) {
            srb.IsSafeToCreate = true;
        } else if (srb.IsSafeToCreate) {
            CreateRoadBlockBetween2Points(srb.CornerA, srb.CornerB, srb.IsGangRoadBlock);
            srb.IsSafeToCreate = false;
        }
    }
}

// 0x460EE0
bool CRoadBlocks::GetRoadBlockNodeInfo(CNodeAddress nodeAddress, float& outWidth, CVector& outDir) {
    auto* const node = ThePaths.GetPathNode(nodeAddress);
    assert(node);

    assert(node->m_nNumLinks >= 2);
    const auto naviLinkAddrA = ThePaths.GetNaviLink(nodeAddress.m_wAreaId, node->m_wBaseLinkId + 0),
               naviLinkAddrB = ThePaths.GetNaviLink(nodeAddress.m_wAreaId, node->m_wBaseLinkId + 1);
    if (!ThePaths.IsAreaLoaded(naviLinkAddrA.m_wAreaId) || !ThePaths.IsAreaLoaded(naviLinkAddrB.m_wAreaId)) {
        return false;
    }

    const auto &naviLinkA = ThePaths.GetCarPathLink(naviLinkAddrA),
               &naviLinkB = ThePaths.GetCarPathLink(naviLinkAddrB);

    const auto maxNumLanes = std::max(
        naviLinkA.m_numOppositeDirLanes + naviLinkA.m_numSameDirLanes,
        naviLinkB.m_numOppositeDirLanes + naviLinkB.m_numSameDirLanes
    );

    outWidth = ((float)maxNumLanes + 1.f) * 5.f;
    outDir   = CVector{ (naviLinkB.GetNodeCoors() - naviLinkA.GetNodeCoors()).GetPerpRight(), 0.f }.Normalized();

    return true;
}

// 0x460DF0
void CRoadBlocks::RegisterScriptRoadBlock(CVector cornerA, CVector cornerB, bool isGangRoadBlock) {
    auto free = rng::find_if(aScriptRoadBlocks, [](const auto& srb) { return !srb.IsActive; });
    if (free == aScriptRoadBlocks.end()) {
        // No free script roadblock found
        return;
    }

    free->CornerA         = cornerA;
    free->CornerB         = cornerB;
    free->IsActive        = true;
    free->IsSafeToCreate  = true;
    free->IsGangRoadBlock = isGangRoadBlock;
}
