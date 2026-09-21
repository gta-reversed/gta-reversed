/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Train.h"

#include "Buoyancy.h"
#include "CarCtrl.h"

#include "Tasks/TaskTypes/TaskComplexEnterCar.h"
#include "Tasks/TaskTypes/TaskComplexEnterCarAsDriver.h"
#include "Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCarAndWander.h"
#include "Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.h"
#include "Tasks/TaskComplexSequence.h"
#include "PedGroups.h"
#include "Population.h"
#include "CarEnterExit.h"

CVector CTrain::aStationCoors[6] = { // 0x8D48F8
    CVector{ 1741.0f, -1954.0f, 15.0f },
    CVector{ 1297.0f, -1898.0f, 3.0f  },
    CVector{ -1945.0f, 128.0f,  29.0f },
    CVector{ 1434.0f,  2632.0f, 13.0f },
    CVector{ 2783.0f,  1758.0f, 12.0f },
    CVector{ 2865.0f,  1281.0f, 12.0  }
};

auto& pTrackNodes = StaticRef<CTrainNode*[4]>(0xC38024);
auto& NumTrackNodes = StaticRef<std::array<int32, 4>>(0xC38014);
auto& arrTotalTrackLength = StaticRef<std::array<float, 4>>(0xC37FEC);
auto& StationDist = StaticRef<std::array<float, 6>>(0xC38034);
auto& TrainConfigs = StaticRef<int32[16][16]>(0x8D44F8);
auto& TrainGenCounter = StaticRef<int32>(0xC38064);
auto& bPlayerNearStation = StaticRef<bool>(0xC38068);

void CTrain::InjectHooks() {
    RH_ScopedVirtualClass(CTrain, 0x872370, 66);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(Constructor, 0x6F6030);
    RH_ScopedInstall(InitTrains, 0x6F7440);
    RH_ScopedInstall(ReadAndInterpretTrackFile, 0x6F55D0);
    RH_ScopedInstall(Shutdown, 0x6F58D0);
    RH_ScopedInstall(UpdateTrains, 0x6F5900);
    RH_ScopedInstall(FindCoorsFromPositionOnTrack, 0x6F59E0);
    RH_ScopedInstall(FindMaximumSpeedToStopAtStations, 0x6F5BA0);
    RH_ScopedInstall(FindNumCarriagesPulled, 0x6F5CD0);
    RH_ScopedInstall(OpenTrainDoor, 0x6F5D80);
    RH_ScopedInstall(AddPassenger, 0x6F5D90);
    RH_ScopedInstall(RemovePassenger, 0x6F5DA0);
    RH_ScopedInstall(DisableRandomTrains, 0x6F5DB0);
    RH_ScopedInstall(RemoveOneMissionTrain, 0x6F5DC0);
    RH_ScopedInstall(ReleaseOneMissionTrain, 0x6F5DF0);
    RH_ScopedInstall(SetTrainSpeed, 0x6F5E20);
    RH_ScopedInstall(SetTrainCruiseSpeed, 0x6F5E50);
    RH_ScopedInstall(FindCaboose, 0x6F5E70);
    RH_ScopedInstall(FindEngine, 0x6F5E90);
    RH_ScopedInstall(FindCarriage, 0x6F5EB0);
    RH_ScopedInstall(FindSideStationIsOn, 0x6F5EF0);
    RH_ScopedInstall(FindNextStationPositionInDirection, 0x6F5F00);
    RH_ScopedInstall(IsInTunnel, 0x6F6320);
    RH_ScopedInstall(RemoveRandomPassenger, 0x6F6850);
    RH_ScopedInstall(RemoveMissionTrains, 0x6F6A20);
    RH_ScopedInstall(RemoveAllTrains, 0x6F6AA0);
    RH_ScopedInstall(ReleaseMissionTrains, 0x6F6B60);
    RH_ScopedInstall(FindClosestTrackNode, 0x6F6BD0);
    RH_ScopedInstall(FindPositionOnTrackFromCoors, 0x6F6CC0);
    RH_ScopedInstall(FindNearestTrain, 0x6F7090);
    RH_ScopedInstall(SetNewTrainPosition, 0x6F7140);
    RH_ScopedInstall(IsNextStationAllowed, 0x6F7260);
    RH_ScopedInstall(SkipToNextAllowedStation, 0x6F72F0);
    RH_ScopedInstall(CreateMissionTrain, 0x6F7550);
    RH_ScopedInstall(DoTrainGenerationAndRemoval, 0x6F7900);
    RH_ScopedInstall(AddNearbyPedAsRandomPassenger, 0x6F8170);
    RH_ScopedVMTInstall(ProcessControl, 0x6F86A0);

    RH_ScopedGlobalInstall(ProcessTrainAnnouncements, 0x6F5910);
    RH_ScopedGlobalInstall(PlayAnnouncement, 0x6F5920);
    RH_ScopedGlobalInstall(MarkSurroundingEntitiesForCollisionWithTrain, 0x6F6640);
    RH_ScopedGlobalInstall(TrainHitStuff<CPtrListSingleLink<CPhysical*>>, 0x6F5CF0);
}

// 0x6F6030
CTrain::CTrain(int32 modelIndex, eVehicleCreatedBy createdBy) : CVehicle(createdBy) {
    m_nVehicleSubType = VEHICLE_TYPE_TRAIN;
    m_nVehicleType = VEHICLE_TYPE_TRAIN;

    const auto mi = CModelInfo::GetModelInfo(modelIndex)->AsVehicleModelInfoPtr();
    m_pHandlingData = gHandlingDataMgr.GetVehiclePointer(mi->m_nHandlingId);
    m_nHandlingFlagsIntValue = m_pHandlingData->m_nHandlingFlags;

    CVehicle::SetModelIndex(modelIndex);
    SetupModelNodes();

    std::memset(&m_aDoors, 0, sizeof(m_aDoors));
    if (m_nModelIndex == MODEL_STREAKC) {
        m_aDoors[DOOR_LEFT_FRONT].Init(1.25f, 0.25f, DOOR_AXIS_NEG_Y, DOOR_AXIS_Z, DOOR_EXTRA_BASED);
        m_aDoors[DOOR_RIGHT_FRONT].Init(1.25f, 0.25f, DOOR_AXIS_NEG_Y, DOOR_AXIS_Z, DOOR_EXTRA_BASED);
    } else {
        m_aDoors[DOOR_LEFT_FRONT].Init(TWO_PI / -5.0f, 0.0f, DOOR_AXIS_NEG_Y, DOOR_AXIS_Z, DOOR_EXTRA_BASED);
        m_aDoors[DOOR_RIGHT_FRONT].Init(TWO_PI / +5.0f, 0.0f, DOOR_AXIS_NEG_Y, DOOR_AXIS_Z, DOOR_EXTRA_BASED);
    }

    { // todo:
    trainFlags.bClockwiseDirection = true;
    trainFlags.bIsLastCarriage = true;
    trainFlags.bIsFrontCarriage = true;
    trainFlags.bStopsAtStations = true;
    }

    m_nPassengersGenerationState = 0;
    m_nNumPassengersToLeave = CGeneral::GetRandomNumber() & 3;
    m_nNumPassengersToEnter = (CGeneral::GetRandomNumber() & 3) + 1;
    m_pTemporaryPassenger = nullptr;
    m_nMaxPassengers = 5;
    physicalFlags.bDisableSimpleCollision = true;
    SetUsesCollision(true);
    m_nTimeWhenCreated = CTimer::GetTimeInMS();
    field_5C8 = 0;
    m_nTrackId = 0;
    m_fCurrentRailDistance = 0.0f;
    m_fTrainSpeed = 0.0f;
    m_nTimeWhenStoppedAtStation = 0;
    mi->ChooseVehicleColour(m_nPrimaryColor, m_nSecondaryColor, m_nTertiaryColor, m_nQuaternaryColor, 1);
    m_fMass = m_pHandlingData->m_fMass;
    m_fTurnMass = m_pHandlingData->m_fTurnMass;
    m_vecCentreOfMass = m_pHandlingData->m_vecCentreOfMass;
    m_fElasticity = 0.05f;
    m_fBuoyancyConstant = m_pHandlingData->m_fBuoyancyConstant;
    m_fAirResistance = GetDefaultAirResistance();

    physicalFlags.bAddMovingCollisionSpeed = true;
    m_bTunnelTransition = true;
    m_pPrevCarriage = nullptr;
    m_pNextCarriage = nullptr;
    SetStatus(STATUS_TRAIN_MOVING);
    m_autoPilot.m_speed = 0.0f;
    m_autoPilot.SetCruiseSpeed(0);
    m_vehicleAudio.Initialise(this);
}

void CTrain::SetupModelNodes() {
    std::ranges::fill(m_aTrainNodes, nullptr);
    CClumpModelInfo::FillFrameArray(GetRpClump(), m_aTrainNodes.data());
}

// 0x6F7440
void CTrain::InitTrains() {
    ZoneScoped;

    bDisableRandomTrains = false;
    GenTrain_Status = 0;

    constexpr const char* filenames[] {
        "data\\paths\\tracks.dat",
        "data\\paths\\tracks3.dat",
        "data\\paths\\tracks2.dat",
        "data\\paths\\tracks4.dat",
    };
    for (auto i = 0u; i < std::size(pTrackNodes); ++i) {
        if (!pTrackNodes[i]) {
            CTrain::ReadAndInterpretTrackFile(filenames[i], pTrackNodes, NumTrackNodes.data(), arrTotalTrackLength.data(), i);
        }
    }

    for (auto i = 0u; i < std::size(aStationCoors); ++i) {
        int32 trackId;
        const auto nodeIndex = CTrain::FindClosestTrackNode(aStationCoors[i], &trackId);
        StationDist[i] = pTrackNodes[0][nodeIndex].GetDistanceFromStart(); // NOTE: Always uses track 0's nodes (As per original)
    }
}

// 0x6F55D0
void CTrain::ReadAndInterpretTrackFile(const char* filename, CTrainNode** nodes, int32* lineCount, float* totalDist, int32 skipStations) {
    if (!*nodes) {
        char* const fileBuf = new char[0xB530];
        CFileMgr::LoadFile(filename, reinterpret_cast<uint8*>(fileBuf), 0xB530, "rb");

        // Read the first line: the node count, optionally preceded by a "processed" marker line
        int32 pos = 0;
        int32 len = 0;
        if (fileBuf[pos] != '\n') {
            do {
                gString[len++] = fileBuf[pos++];
            } while (fileBuf[pos] != '\n');
        }
        gString[len] = '\0';
        ++pos;

        if (strcmp(gString, "processed") == 0) {
            len = 0;
            while (fileBuf[pos] != '\n') {
                gString[len++] = fileBuf[pos++];
            }
            gString[len] = '\0';
            ++pos;
        }

        (void)sscanf(gString, "%d", lineCount);

        *nodes = new CTrainNode[*lineCount];

        CVector* stationOut = aStationCoors;
        for (int32 i = 0; i < *lineCount; ++i) {
            // NOTE: An empty line leaves `gString` untouched, so it re-parses the previous line (As per original)
            if (fileBuf[pos] != '\n') {
                char* dst = gString;
                do {
                    *dst++ = fileBuf[pos++];
                } while (fileBuf[pos] != '\n');
            }
            ++pos;

            float x, y, z;
            int32 hasStation;
            (void)sscanf(gString, "%f %f %f %d", &x, &y, &z, &hasStation);

            (*nodes)[i].SetX(x);
            (*nodes)[i].SetY(y);
            (*nodes)[i].SetZ(z);

            if (skipStations == 0 && hasStation != 0) {
                *stationOut++ = CVector{ x, y, z };
            }
        }

        delete[] fileBuf;
    }

    // (Re)compute each node's distance from the start of the track, plus the total track length
    if (*lineCount <= 0) {
        *totalDist = 0.0f;
        return;
    }
    float dist = 0.0f;
    const int32 count = *lineCount;
    for (int32 i = 1; i <= count; ++i) {
        (*nodes)[i - 1].SetDistanceFromStart(dist);
        const float dx = (*nodes)[i - 1].GetX() - (*nodes)[i % count].GetX();
        const float dy = (*nodes)[i - 1].GetY() - (*nodes)[i % count].GetY();
        dist += sqrt(dx * dx + dy * dy);
    }
    *totalDist = dist;
}

// 0x6F58D0
void CTrain::Shutdown() {
    for (auto node : pTrackNodes) {
        delete node;
        node = nullptr;
    }
}

// 0x6F5900
void CTrain::UpdateTrains() {
    ZoneScoped;

    // NOP
}

// 0x6F5910
void ProcessTrainAnnouncements() {
    // NOP
}

// 0x6F5920
void PlayAnnouncement(uint8 arg0, uint8 arg1) {
    // NOP
}

// 0x6F59E0
void CTrain::FindCoorsFromPositionOnTrack(float railDistance, int32 trackId, CVector* outCoors) {
    const auto numTrackNodes = NumTrackNodes[trackId];
    if (numTrackNodes <= 0) {
        return;
    }

    const auto trackNodes = pTrackNodes[trackId];
    for (int32 nodeIndex = 0; nodeIndex < numTrackNodes; nodeIndex++) {
        const auto& node         = trackNodes[nodeIndex];
        const auto& nextNode     = trackNodes[(nodeIndex + 1) % numTrackNodes];
        const float distToNode   = railDistance - node.GetDistanceFromStart();
        const float distToNext   = nextNode.GetDistanceFromStart() - railDistance;
        if (distToNode < 0.0f || distToNext < 0.0f) {
            continue;
        }

        const float factor = 1.0f / (distToNode + distToNext);
        *outCoors = (node.GetPosn() * distToNext + nextNode.GetPosn() * distToNode) * factor;
        return;
    }
}

// 0x6F5BA0
bool CTrain::FindMaximumSpeedToStopAtStations(float* speed) {
    *speed = 50.0f;

    float distToClosestStation = 10000.0f;
    if (m_nTrackId != 0) {
        return false;
    }

    const float totalTrackLength = arrTotalTrackLength[0];
    const bool  bClockwise       = trainFlags.bClockwiseDirection;
    for (const float stationDist : StationDist) {
        float distToStation = stationDist - m_fCurrentRailDistance;
        if (bClockwise) {
            distToStation += 40.0f;
        } else {
            distToStation -= 40.0f;
        }

        while (totalTrackLength * 0.5f < distToStation) {
            distToStation -= totalTrackLength;
        }

        while (distToStation < -totalTrackLength * 0.5f) {
            distToStation += totalTrackLength;
        }

        if (bClockwise) {
            if (distToStation <= 0.0f) {
                continue;
            }
        } else {
            if (distToStation >= 0.0f) {
                continue;
            }
            distToStation = -distToStation;
        }

        if (distToStation < distToClosestStation) {
            distToClosestStation = distToStation;
        }
    }

    if (distToClosestStation >= 500.0f) {
        *speed = 100000.0f;
    } else {
        *speed = (1.0f - (500.0f - distToClosestStation) * 0.002f) * 50.0f;
    }

    return distToClosestStation < 5.0f;
}

// 0x6F5CD0
uint32 CTrain::FindNumCarriagesPulled() {
    uint32 num;
    CTrain* carriage = m_pNextCarriage;
    for (num = 0; carriage; ++num) {
        carriage = carriage->m_pNextCarriage;
    }
    return num;
}

// 0x6F5D80
void CTrain::OpenTrainDoor(float state) {
    // NOP
}

// 0x6F5D90
void CTrain::AddPassenger(CPed* ped) {
    // NOP
}

// 0x6F5DA0
void CTrain::RemovePassenger(CPed* ped) {
    // NOP
}

// 0x6F5DB0
void CTrain::DisableRandomTrains(bool disable) {
    bDisableRandomTrains = disable;
}

// 0x6F5DC0
void CTrain::RemoveOneMissionTrain(CTrain* train) {
    if (!train)
        return;

    CTrain* next;
    CTrain* _train = train;
    do {
        next = _train->m_pNextCarriage;
        CWorld::Remove(_train);
        delete _train;
        _train = next;
    } while (next);
}

// 0x6F5DF0
void CTrain::ReleaseOneMissionTrain(CTrain* train) {
    for (auto* head = train; head; head = head->m_pNextCarriage) {
        head->trainFlags.bMissionTrain = false;
    }
}

// 0x6F5E20
void CTrain::SetTrainSpeed(CTrain* train, float speed) {
    train->m_fTrainSpeed = speed / 50.0f;
    if (!train->trainFlags.bClockwiseDirection) {
        train->m_fTrainSpeed = -train->m_fTrainSpeed;
    }
}

// 0x6F5E50
void CTrain::SetTrainCruiseSpeed(CTrain* train, float speed) {
    train->m_autoPilot.SetCruiseSpeed((uint8)speed);
}

// 0x6F5E70
CTrain* CTrain::FindCaboose(CTrain* train) {
    while (train->m_pNextCarriage) {
        train = train->m_pNextCarriage;
    }
    return train;
}

// 0x6F5E90
CTrain* CTrain::FindEngine(CTrain* train) {
    while (train->m_pPrevCarriage) {
        train = train->m_pPrevCarriage;
    }
    return train;
}

// 0x6F5EB0
CTrain* CTrain::FindCarriage(CTrain* train, uint8 carriage) {
    if (carriage == 0) {
        return train;
    }

    uint8 numCarriages = 0;
    do {
        train = train->m_pNextCarriage;
        if (!train) {
            return nullptr;
        }
        ++numCarriages;
    } while (numCarriages < carriage);

    return train;
}

// 0x6F5EF0
bool CTrain::FindSideStationIsOn() const {
    return trainFlags.bClockwiseDirection; // ?
}

// 0x6F5F00
void CTrain::FindNextStationPositionInDirection(bool clockwiseDirection, float distance, float* distanceToStation, int32* numStations) {
    int32 station = 0;
    for (; station < 6; station++) {
        if (distance < StationDist[station]) {
            break;
        }
    }
    if (station == 6) {
        station = 0;
    }

    int32 targetStation = station;
    if (!clockwiseDirection) {
        targetStation = station - 1;
        if (targetStation < 0) {
            targetStation = station + 5;
        }
    }

    if (std::abs(distance - StationDist[targetStation]) < 100.0f) {
        if (clockwiseDirection) {
            targetStation++;
        } else {
            targetStation--;
        }
        if (targetStation < 0) {
            targetStation += 6;
        }
        if (targetStation > 5) {
            targetStation = 0;
        }
    }

    *numStations       = targetStation;
    *distanceToStation = StationDist[targetStation];
}

// 0x6F6320
bool CTrain::IsInTunnel() const {
    const auto& pos = GetPosition();
    CColPoint colPoint{};
    CEntity* colEntity{};
    return CWorld::ProcessVerticalLine(pos, pos.z + 100.0f, colPoint, colEntity, true);
}

// 0x6F6640
void MarkSurroundingEntitiesForCollisionWithTrain(CVector pos, float radius, CEntity* entity, bool bOnlyVehicles) {
    int32 startSectorX = std::max(CWorld::GetSectorX(pos.x - radius), 0);
    int32 startSectorY = std::max(CWorld::GetSectorY(pos.y - radius), 0);
    int32 endSectorX   = std::min(CWorld::GetSectorX(pos.x + radius), MAX_SECTORS_X - 1);
    int32 endSectorY   = std::min(CWorld::GetSectorY(pos.y + radius), MAX_SECTORS_Y - 1);

    CWorld::AdvanceCurrentScanCode();

    for (int32 sectorY = startSectorY; sectorY <= endSectorY; ++sectorY) {
        for (int32 sectorX = startSectorX; sectorX <= endSectorX; ++sectorX) {
            auto& repeatSector = CWorld::GetRepeatSector(sectorX, sectorY);
            TrainHitStuff(repeatSector.Vehicles, entity);
            if (!bOnlyVehicles) {
                TrainHitStuff(repeatSector.Peds, entity);
                TrainHitStuff(repeatSector.Objects, entity);
            }
        }
    }
}

// 0x6F5CF0
template<typename PtrListType>
void TrainHitStuff(PtrListType& ptrList, CEntity* entity) {
    for (auto& physical : ptrList) {
        if (physical == entity) {
            continue;
        }

        physical->physicalFlags.bProcessCollisionEvenIfStationary = true;

        if (physical->GetIsTypeObject() && physical->GetIsStatic() &&
            (physical->GetModelIndex() == ModelIndices::MI_OBJECTFORMAGNOCRANE1 ||
             physical->GetModelIndex() == ModelIndices::MI_OBJECTFORMAGNOCRANE2 ||
             physical->GetModelIndex() == ModelIndices::MI_OBJECTFORMAGNOCRANE3)
        ) {
            physical->SetIsStatic(false);
            physical->AddToMovingList();
            physical->m_nFakePhysics = 0;
        }
    }
}

// 0x6F6850
void CTrain::RemoveRandomPassenger() {
    if (CReplay::Mode == MODE_PLAYBACK) {
        return;
    }

    const auto player = FindPlayerPed();
    if (player->m_pVehicle == this) { // The player is in the train
        return;
    }

    // If the player is currently entering this train, don't remove any passengers
    if (const auto task = player->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_PRIMARY)) {
        switch (task->GetTaskType()) {
        case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER:
        case TASK_COMPLEX_ENTER_CAR_AS_DRIVER:
            if (static_cast<CTaskComplexEnterCar*>(task)->GetTargetCar() == this) {
                return;
            }
            break;
        }
    }

    const bool bClockwise = trainFlags.bClockwiseDirection;
    if (player->bInVehicle && player->m_pVehicle == this) {
        if (bClockwise) {
            if (m_pDriver == player) { // The player is driving, so don't remove passengers
                return;
            }
        } else if (m_pDriver != player) {
            return;
        }
    }

    if (m_nNumPassengersToLeave == 0) {
        return;
    }

    if (m_pTemporaryPassenger) {
        if (m_pTemporaryPassenger->bInVehicle) { // The passenger we told to leave didn't do it yet
            return;
        }
        CEntity::ClearReference(m_pTemporaryPassenger);
    }

    int32 carRating = CGeneral::GetRandomNumberInRange(0, 12);
    if (carRating == 12) {
        carRating = 11;
    }
    if (CGeneral::GetRandomNumber() < 100) {
        carRating = 25;
    }

    if (const auto ped = CPopulation::AddPedInCar(this, !bClockwise, carRating, 0, false, false)) {
        ped->bJustGotOffTrain = true;

        m_nNumPassengersToLeave--;
        m_pTemporaryPassenger = ped;
        CEntity::RegisterReference(m_pTemporaryPassenger);

        ped->GetTaskManager().SetTask(
            new CTaskComplexLeaveCarAndWander{ this, TARGET_DOOR_FRONT_LEFT, 0, true },
            TASK_PRIMARY_PRIMARY
        );
    }
}

// 0x6F6A20
void CTrain::RemoveMissionTrains() {
    for (auto& vehicle : GetVehiclePool()->GetAllValid()) {
        if (vehicle.IsTrain() &&
            &vehicle != FindPlayerVehicle() &&
            vehicle.AsTrain()->trainFlags.bMissionTrain
        ) {
            CWorld::Remove(&vehicle);
            delete &vehicle;
        }
    }
}

// 0x6F6AA0
void CTrain::RemoveAllTrains() {
    for (auto&& [_, vehicle] : GetVehiclePool()->GetAllValidWithIndex() | rngv::reverse) {
        if (!vehicle.IsTrain()) {
            continue;
        }

        // Check if the player is using this train (Or any of it's carriages - That's why both directions are checked)
        bool bIsPlayerTrain = false;
        for (CTrain* carriage = vehicle.AsTrain(); carriage; carriage = carriage->m_pPrevCarriage) {
            if (carriage == FindPlayerVehicle()) {
                bIsPlayerTrain = true;
            }
        }
        for (CTrain* carriage = vehicle.AsTrain(); carriage; carriage = carriage->m_pNextCarriage) {
            if (carriage == FindPlayerVehicle()) {
                bIsPlayerTrain = true;
            }
        }

        if (!bIsPlayerTrain) {
            CWorld::Remove(&vehicle);
            delete &vehicle;
        }
    }
}

// 0x6F6B60
void CTrain::ReleaseMissionTrains() {
    for (auto& vehicle : GetVehiclePool()->GetAllValid()) {
        if (vehicle.IsTrain() && &vehicle != FindPlayerVehicle()) {
            vehicle.AsTrain()->trainFlags.bMissionTrain = false;
        }
    }
}

// 0x6F6BD0
int32 CTrain::FindClosestTrackNode(CVector posn, int32* outTrackId) {
    float closestDist = 99999.9f;
    int32 closestNode = 0;
    for (int32 trackId = 0; trackId < 4; trackId++) {
        const auto numTrackNodes = NumTrackNodes[trackId];
        for (int32 nodeIndex = 0; nodeIndex < numTrackNodes; nodeIndex++) {
            const float dist = DistanceBetweenPoints(posn, pTrackNodes[trackId][nodeIndex].GetPosn());
            if (dist < closestDist) {
                *outTrackId = trackId;
                closestNode = nodeIndex;
                closestDist = dist;
            }
        }
    }
    return closestNode;
}

// 0x6F6CC0
void CTrain::FindPositionOnTrackFromCoors() {
    const auto numTrackNodes = NumTrackNodes[m_nTrackId];
    if (numTrackNodes <= 0) {
        return;
    }

    const auto   trackNodes = pTrackNodes[m_nTrackId];
    const CVector posn      = GetPosition();
    for (int32 nodeIndex = 0; nodeIndex < numTrackNodes; nodeIndex++) {
        auto& node     = trackNodes[nodeIndex];
        auto& nextNode = trackNodes[(nodeIndex + 1) % numTrackNodes];

        const CVector2D nodePosn{ node.GetX(), node.GetY() };
        const CVector2D nextPosn{ nextNode.GetX(), nextNode.GetY() };
        const CVector2D seg = nextPosn - nodePosn;

        // How far along the segment [node, nextNode] the train's position projects to
        const float segLen = seg.Magnitude();
        const float t      = DotProduct2D(CVector2D{ posn.x, posn.y } - nodePosn, seg) / (segLen * segLen);
        if (t <= 0.001f || t >= 1.001f) {
            continue;
        }

        if (DistanceBetweenPoints2D(nodePosn + seg * t, CVector2D{ posn.x, posn.y }) < 3.0f) {
            // The train is on this segment - Update it's position on the track
            m_fCurrentRailDistance = node.GetDistanceFromStart() + (nextNode.GetDistanceFromStart() - node.GetDistanceFromStart()) * t;

            // Offset by half the length of the train (The rail distance is that of it's front)
            const auto& bbox = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->GetBoundingBox();
            m_fCurrentRailDistance -= (bbox.m_vecMax.y - bbox.m_vecMin.y) * 0.5f;
            if (m_fCurrentRailDistance <= 0.0f) {
                m_fCurrentRailDistance += arrTotalTrackLength[m_nTrackId];
            }

            m_fTrainSpeed = m_vecMoveSpeed.Magnitude();
            if (trainFlags.bClockwiseDirection == (DotProduct2D(seg, CVector2D{ m_vecMoveSpeed.x, m_vecMoveSpeed.y }) > 0.0f)) {
                m_fTrainSpeed = -m_fTrainSpeed;
            }
            return;
        }

        float fTrainNodeLighting = node.GetLightingFromCollision().GetCurrentLighting();
        const float fTrainNextNodeLighting = nextNode.GetLightingFromCollision().GetCurrentLighting();
        fTrainNodeLighting += (fTrainNextNodeLighting - fTrainNodeLighting) * t;
        m_fContactSurfaceBrightness = fTrainNodeLighting;
    }
}

// 0x6F7090
CTrain* CTrain::FindNearestTrain(CVector posn, bool mustBeMainTrain) {
    CTrain* nearestTrain = nullptr;
    float nearestDist = 10000000.0f;
    for (auto&& [_, vehicle] : GetVehiclePool()->GetAllValidWithIndex() | rngv::reverse) { // NOTE: Reverse order matters (Ties are won by the one with the highest index)
        if (!vehicle.IsTrain()) {
            continue;
        }

        const float dist = (vehicle.GetPosition() - posn).Magnitude2D();
        if (dist < nearestDist && (!mustBeMainTrain || vehicle.AsTrain()->trainFlags.bIsFrontCarriage)) {
            nearestTrain = vehicle.AsTrain();
            nearestDist  = dist;
        }
    }
    return nearestTrain;
}

// 0x6F7140
void CTrain::SetNewTrainPosition(CTrain* train, CVector posn) {
    train->SetPosn(posn);
    train->FindPositionOnTrackFromCoors();
}

// 0x6F7260
bool CTrain::IsNextStationAllowed(CTrain* train) {
    while (train->m_pPrevCarriage) {
        train = train->m_pPrevCarriage;
    }

    // NOTE: `railDistance` is used as both the input and output of `FindNextStationPositionInDirection`
    float railDistance = train->m_fCurrentRailDistance;
    int32 station;
    FindNextStationPositionInDirection(train->trainFlags.bClockwiseDirection, railDistance, &railDistance, &station);

    const auto level = CTheZones::GetLevelFromPosition(aStationCoors[station]);
    return CStats::GetStatValue(STAT_CITY_UNLOCKED) + 1.0f >= (float)level;
}

// 0x6F72F0
void CTrain::SkipToNextAllowedStation(CTrain* train) {
    while (train->m_pPrevCarriage) {
        train = train->m_pPrevCarriage;
    }

    // NOTE: `railDistance` is used as both the input and output of `FindNextStationPositionInDirection`
    float railDistance = train->m_fCurrentRailDistance;
    int32 station;
    do {
        FindNextStationPositionInDirection(train->trainFlags.bClockwiseDirection, railDistance, &railDistance, &station);
    } while (CStats::GetStatValue(STAT_CITY_UNLOCKED) + 1.0f < (float)CTheZones::GetLevelFromPosition(aStationCoors[station]));

    if (train->trainFlags.bClockwiseDirection) {
        train->m_fTrainSpeed             = 0.1f;
        train->m_fCurrentRailDistance    = railDistance - 20.0f;
    } else {
        train->m_fTrainSpeed             = -0.1f;
        train->m_fCurrentRailDistance    = railDistance + 20.0f;
    }

    CStreaming::LoadScene(aStationCoors[station]);
    CStreaming::LoadAllRequestedModels(false);
    CGameLogic::PassTime((uint32)(DistanceBetweenPoints2D(aStationCoors[station], train->GetPosition()) * 0.05f + 23.0f));
}

// 0x6F7550
void CTrain::CreateMissionTrain(CVector posn, bool clockwiseDirection, uint32 trainType, CTrain** outFirstCarriage, CTrain** outLastCarriage, int32 nodeIndex, int32 trackId, bool isMissionTrain) {
    if (nodeIndex < 0) {
        nodeIndex = FindClosestTrackNode(posn, &trackId);
    }
    float railDistance = pTrackNodes[trackId][nodeIndex].GetDistanceFromStart();
    const int32* config = TrainConfigs[trainType];
    if (*config) {
        CTrain* carriages[16];
        int32 numCarriages = 0;
        CTrain* prev = nullptr;
        do {
            auto* carriage = new CTrain(*config, PERMANENT_VEHICLE);
            carriage->GetMatrix().SetTranslate(CVector{ 0.0f, 0.0f, 0.0f });
            carriage->m_nNodeIndex = (int16)nodeIndex;
            carriage->SetStatus(STATUS_ABANDONED);
            carriage->vehicleFlags.bIsLocked = true;
            carriage->m_fCurrentRailDistance = railDistance;
            carriage->trainFlags.bMissionTrain = isMissionTrain;
            carriage->trainFlags.bClockwiseDirection = clockwiseDirection;
            carriage->m_nTrackId = (int8)trackId;
            if (!isMissionTrain && !prev) {
                carriage->m_fLength = 0.0f;
            } else if (isMissionTrain && !prev) {
                carriage->trainFlags.bStopsAtStations = false;
                carriage->SetPosn(posn);
                carriage->FindPositionOnTrackFromCoors();
                carriage->m_fLength = 0.0f;
            } else {
                if (isMissionTrain) {
                    carriage->trainFlags.bStopsAtStations = false;
                }
                const auto* mi = CModelInfo::GetModelInfo(!clockwiseDirection ? prev->m_nModelIndex : carriage->m_nModelIndex);
                const auto& bbox = mi->GetColModel()->GetBoundingBox();
                const float len = bbox.m_vecMax.y - bbox.m_vecMin.y;
                if (!clockwiseDirection) {
                    carriage->m_fLength = len;
                    railDistance += len;
                } else {
                    carriage->m_fLength = -len;
                    railDistance -= len;
                }
            }
            carriages[numCarriages++] = carriage;
            if (!prev) {
                carriage->trainFlags.bIsFrontCarriage = true;
                if (outFirstCarriage) {
                    *outFirstCarriage = carriage;
                }
            } else {
                carriage->trainFlags.bIsFrontCarriage = false;
                carriage->vehicleFlags.bHasBeenOwnedByPlayer = true;
                prev->trainFlags.bIsLastCarriage = false;
            }
            carriage->trainFlags.bIsLastCarriage = true;
            if (outLastCarriage) {
                *outLastCarriage = carriage;
            }
            carriage->m_pPrevCarriage = prev;
            if (prev) {
                CEntity::RegisterReference(carriage->m_pPrevCarriage);
            }
            carriage->m_pNextCarriage = nullptr;
            if (prev) {
                prev->m_pNextCarriage = carriage;
                CEntity::RegisterReference(prev->m_pNextCarriage);
            }
            carriage->ProcessControl();
            prev = carriage;
            ++config;
        } while (*config);
        for (int32 i = numCarriages - 1; i >= 0; --i) {
            CWorld::Remove(carriages[i]);
            CWorld::Add(carriages[i]);
        }
    }
    CTrain* const first = *outFirstCarriage;
    if (first->m_nModelIndex != MODEL_TRAM) {
        CPopulation::AddPedInCar(first, true, -1, 0, false, false);
        if (first->m_pDriver) {
            first->m_pDriver->GetIntelligence()->SetPedDecisionMakerType(6);
        }
    }
    bool hasStreak = false;
    for (CTrain* c = first; c; c = c->m_pNextCarriage) {
        if (c->m_nModelIndex == MODEL_STREAK) {
            hasStreak = true;
            break;
        }
    }
    for (CTrain* c = first; c; c = c->m_pNextCarriage) {
        c->trainFlags.bIsStreakModel = hasStreak;
    }
}

// 0x6F7900
void CTrain::DoTrainGenerationAndRemoval() {
    if (CTimer::GetTimeInMS() / 3000 != CTimer::GetPreviousTimeInMS() / 3000) {
        bPlayerNearStation = false;
        for (const CVector& station : aStationCoors) {
            if (DistanceBetweenPoints2D(station, FindPlayerCoors()) < 60.0f) {
                bPlayerNearStation = true;
            }
        }
    }
    uint32 timeDivisor;
    float genRadius;
    if (bPlayerNearStation) {
        timeDivisor = 1;
        genRadius = 100.0f;
    } else {
        timeDivisor = 950;
        genRadius = 70.0f;
    }
    if (CTimer::GetTimeInMS() / timeDivisor == CTimer::GetPreviousTimeInMS() / timeDivisor) {
        return;
    }
    bool foundRandomTrain = false;
    CVehiclePool* pool = GetVehiclePool();
    for (int32 i = (int32)pool->GetSize() - 1; i >= 0; i--) {
        CVehicle* vehicle = pool->GetAt(i);
        if (!vehicle || !vehicle->IsTrain()) {
            continue;
        }
        CTrain* train = vehicle->AsTrain();
        if (!train->trainFlags.bIsFrontCarriage || train->trainFlags.bMissionTrain) {
            continue;
        }
        foundRandomTrain = true;
        bool canRemove = true;
        for (CTrain* carriage = train; carriage; carriage = carriage->m_pNextCarriage) {
            if (carriage == FindPlayerVehicle()
                || DistanceBetweenPoints2D(carriage->GetPosition(), TheCamera.GetPosition()) < 220.0f) {
                canRemove = false;
            }
        }
        if (canRemove) {
            for (CTrain* carriage = train; carriage;) {
                CTrain* next = carriage->m_pNextCarriage;
                CWorld::Remove(carriage);
                delete carriage;
                carriage = next;
            }
        }
    }
    if (bDisableRandomTrains) {
        return;
    }
    if (GenTrain_Status != 0) {
        if (GenTrain_Status != 1) {
            return;
        }
        bool allModelsLoaded = true;
        for (const int32* model = TrainConfigs[GenTrain_TrainConfig]; *model; model++) {
            if (!CStreaming::IsModelLoaded(*model)) {
                CStreaming::RequestModel(*model, 8);
                allModelsLoaded = false;
            }
        }
        if (!allModelsLoaded) {
            return;
        }
        const CTrainNode* node = &pTrackNodes[GenTrain_Track][GenTrain_GenerationNode];
        const CVector2D nodePos{ node->GetX(), node->GetY() };
        if (DistanceBetweenPoints2D(nodePos, TheCamera.GetPosition()) > 60.0f) {
            CTrain* engine = nullptr;
            CreateMissionTrain({}, GenTrain_Direction, GenTrain_TrainConfig, &engine, nullptr, GenTrain_GenerationNode, GenTrain_Track, false);
            int32 speed;
            if (GenTrain_Track == 0) {
                speed = CGeneral::GetRandomNumber() % 30 + 15;
                if (DistanceBetweenPoints2D(nodePos, CVector2D{ 2222.0f, -1750.0f }) < 300.0f) {
                    speed /= 2;
                }
            } else {
                speed = CGeneral::GetRandomNumber() % 7 + 7;
            }
            float maxSpeed;
            engine->FindMaximumSpeedToStopAtStations(&maxSpeed);
            if ((float)speed < maxSpeed) {
                maxSpeed = (float)speed;
            }
            CTrain::SetTrainSpeed(engine, maxSpeed);
            reinterpret_cast<uint8*>(engine)[0x3D0] = (uint8)speed; // NOTSA: raw offset 0x3D0, unresolved per handoff
        }
        for (const int32* model = TrainConfigs[GenTrain_TrainConfig]; *model; model++) {
            CStreaming::SetModelIsDeletable(*model);
            CStreaming::SetModelTxdIsDeletable(*model);
        }
        GenTrain_Status = 0;
        return;
    }
    if (foundRandomTrain) {
        return;
    }
    const CVector camPos = TheCamera.GetPosition();
    GenTrain_Track = 0;
    GenTrain_GenerationNode = CGeneral::GetRandomNumber() % NumTrackNodes[0];
    const CTrainNode* node = &pTrackNodes[0][GenTrain_GenerationNode];
    float distToNode = DistanceBetweenPoints2D(CVector2D{ node->GetX(), node->GetY() }, camPos);
    bool generateOnTrack0 = distToNode < genRadius;
    if (generateOnTrack0) {
        const CVector playerPos = FindPlayerCoors();
        if (node->GetZ() + 6.0f <= playerPos.z
            && (CCullZones::FindTunnelAttributesForCoors(playerPos) & (TUNNEL | TUNNEL_TRANSITION)) == 0
            && (CCullZones::FindTunnelAttributesForCoors(node->GetPosn()) & TUNNEL) != 0) {
            generateOnTrack0 = false;
        }
    }
    if (generateOnTrack0) {
        GenTrain_Direction = CGeneral::GetRandomNumber() & 1;
        int32 savedNode = GenTrain_GenerationNode; // NOTSA: original reads an uninitialized stack local here when the walk below is skipped (leftover int temp = node raw z); current node is the sane equivalent
        if (distToNode < 170.0f) {
            do {
                if (!GenTrain_Direction) {
                    GenTrain_GenerationNode = (GenTrain_GenerationNode + 1) % NumTrackNodes[GenTrain_Track];
                } else {
                    GenTrain_GenerationNode--;
                    if ((int32)GenTrain_GenerationNode < 0) {
                        GenTrain_GenerationNode += NumTrackNodes[GenTrain_Track];
                    }
                }
                const CTrainNode* n = &pTrackNodes[GenTrain_Track][GenTrain_GenerationNode];
                distToNode = DistanceBetweenPoints2D(CVector2D{ n->GetX(), n->GetY() }, camPos);
            } while (distToNode < 170.0f);
        }
        GenTrain_Status = 1;
        TrainGenCounter = (TrainGenCounter + 1) % 8;
        GenTrain_TrainConfig = TrainGenCounter;
        if (distToNode > 220.0f) {
            GenTrain_GenerationNode = savedNode;
        }
        return;
    }
    if (GenTrain_Status == 0 && CWeather::WeatherRegion == WEATHER_REGION_SF) {
        GenTrain_Track = 1;
        GenTrain_GenerationNode = CGeneral::GetRandomNumber() % NumTrackNodes[1];
        const CTrainNode* node1 = &pTrackNodes[1][GenTrain_GenerationNode];
        float dist1 = DistanceBetweenPoints2D(CVector2D{ node1->GetX(), node1->GetY() }, camPos);
        if (dist1 < genRadius) {
            GenTrain_Direction = CGeneral::GetRandomNumber() & 1;
            if (dist1 < 170.0f) {
                do {
                    if (!GenTrain_Direction) {
                        GenTrain_GenerationNode = (GenTrain_GenerationNode + 1) % NumTrackNodes[GenTrain_Track];
                    } else {
                        GenTrain_GenerationNode--;
                        if ((int32)GenTrain_GenerationNode < 0) {
                            GenTrain_GenerationNode += NumTrackNodes[GenTrain_Track];
                        }
                    }
                    const CTrainNode* n = &pTrackNodes[GenTrain_Track][GenTrain_GenerationNode];
                    dist1 = DistanceBetweenPoints2D(CVector2D{ n->GetX(), n->GetY() }, camPos);
                } while (dist1 < 170.0f);
            }
            if (dist1 < 220.0f) {
                GenTrain_Status = 1;
                GenTrain_TrainConfig = (CGeneral::GetRandomNumber() & 1) + 8;
            }
        }
    }
}

// 0x6F8170
void CTrain::AddNearbyPedAsRandomPassenger() {
    const auto player = FindPlayerPed();
    if (player->m_pVehicle == this) {
        return;
    }
    if (const auto task = player->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_PRIMARY)) {
        switch (task->GetTaskType()) {
        case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER: // 0x2BC
        case TASK_COMPLEX_ENTER_CAR_AS_DRIVER:    // 0x2BD
            if (static_cast<CTaskComplexEnterCar*>(task)->GetTargetCar() == this) { // +0xC
                return;
            }
            break;
        }
    }

    const bool bClockwise = trainFlags.bClockwiseDirection; // +0x5B8 bit 6 (0x40)
    if (m_nNumPassengersToLeave == m_nNumPassengersToEnter) { // +0x5CB lo vs hi nibble
        return;
    }

    if (m_pTemporaryPassenger) { // +0x5CC
        // Wait while previous order is still in progress:
        // enterer not yet boarded (!off && !inVeh) or leaver not yet left (off && inVeh)
        if (m_pTemporaryPassenger->bJustGotOffTrain == m_pTemporaryPassenger->bInVehicle) { // +0x478 & 0x40000 vs +0x46C & 0x100
            return;
        }
        if (m_pTemporaryPassenger->GetIntelligence()->FindTaskByType(TASK_COMPLEX_LEAVE_CAR_AND_WANDER)) { // 0x600EE0, 0x2C3
            return;
        }
        CPed* const prevPassenger = m_pTemporaryPassenger;
        prevPassenger->CleanUpOldReference(reinterpret_cast<CEntity**>(&m_pTemporaryPassenger)); // 0x571A00
        if (!prevPassenger->bJustGotOffTrain && prevPassenger->bInVehicle && prevPassenger->m_pVehicle == this) {
            CPopulation::RemovePed(prevPassenger); // 0x610F20: boarded -> becomes a "passenger"
        }
        m_pTemporaryPassenger = nullptr;
    }

    // Closest eligible ped within 25m on the platform side
    CPed* bestPed = nullptr;
    float bestDistSq = 999999.0f; // 0x497423F0
    const CVector trainPos = GetPosition();
    const CVector trainRight = GetRight();
    for (auto& ped : GetPedPool()->GetAllValid()) {
        if (ped.GetCreatedBy() != PED_GAME) { // +0x484 == 1
            continue;
        }
        if (CPedGroups::GetPedsGroup(&ped)) { // 0x5F7E80
            continue;
        }
        if (ped.GetPlayerData()) { // +0x480
            continue;
        }
        if (ped.m_nPedType == PED_TYPE_COP) { // +0x598 == 6
            continue;
        }
        if (ped.bInVehicle) { // +0x46C & 0x100
            continue;
        }
        if (ped.bJustGotOffTrain) { // +0x478 & 0x40000
            continue;
        }
        if (ped.IsStateDeadForScript()) { // +0x530: 0x36 DIE, 0x37 DEAD, 0x38 DIE_BY_STEALTH
            continue;
        }
        if (ped.GetIntelligence()->FindTaskByType(TASK_COMPLEX_ENTER_CAR_AS_PASSENGER)) { // 0x2BC
            continue;
        }
        const CVector delta = ped.GetPosition() - trainPos; // 0x40FE60
        const float distSq = delta.SquaredMagnitude();      // 0x406DA0
        if (distSq > 625.0f) { // 0x872368
            continue;
        }
        // Orig: Dot(pedPos,right)+planeConst == Dot(delta,right); planeConst=-Dot(trainPos,right)
        const float side = delta.Dot(trainRight); // 0x40FDB0
        if (!bClockwise ? side > 0.0f : side < 0.0f) { // 0x858B50
            continue;
        }
        if (distSq < bestDistSq) {
            bestDistSq = distSq;
            bestPed = &ped;
        }
    }
    if (!bestPed) {
        return;
    }

    int32 doorId = 0;
    CVector doorPos{};
    CCarEnterExit::GetNearestCarDoor(bestPed, this, doorPos, doorId); // 0x6528F0

    CMatrix invTrainMatrix;
    Invert(GetMatrix(), invTrainMatrix); // 0x59B920
    CVector seekOffset = invTrainMatrix.TransformPoint(doorPos); // 0x59C890
    if (bClockwise) {
        seekOffset += trainRight; // 0x411A00
    } else {
        seekOffset -= trainRight; // 0x406D70
    }

    auto* const seekTask = new CTaskComplexSeekEntityXYOffset{ // 0x661DC0 via 0x61A5A0 (0x58)
        this, 50000, 1000, // 0xC350, 0x3E8
        StaticRef<float>(0x86FC2C), StaticRef<float>(0x86FC28), StaticRef<float>(0x86FC30),
        true, true
    };
    seekTask->GetSeekPosCalculator().SetOffset(seekOffset); // +0x44..0x4C (vtable 0x86F8F8)
    seekTask->SetMoveState(PEDMOVE_SPRINT); // +0x50 = 7

    auto* const sequence = new CTaskComplexSequence{}; // 0x632BD0 via 0x61A5A0 (0x40)
    sequence->AddTask(seekTask); // 0x632D10
    if (!bClockwise) {
        sequence->AddTask(new CTaskComplexEnterCarAsDriver{ this }); // 0x6402F0 via 0x61A5A0 (0x50)
    } else {
        sequence->AddTask(new CTaskComplexEnterCarAsPassenger{ this, 0, false }); // 0x640340 via 0x61A5A0 (0x50)
    }
    bestPed->GetTaskManager().SetTask(sequence, TASK_PRIMARY_PRIMARY); // 0x681AF0 (3, 0)

    m_nNumPassengersToLeave++; // +0x5CB low nibble
    m_pTemporaryPassenger = bestPed;
    CEntity::RegisterReference(m_pTemporaryPassenger); // 0x571B70
    // NOTE: trailing 0x59ACD0 thiscall destroys the inverted-matrix stack temp; covered by CMatrix RAII, no raw call needed.
}

// 0x6F86A0
void CTrain::ProcessControl() {
    vehicleFlags.bWarnedPeds = 0;
    m_vehicleAudio.Service();
    if (gbModelViewer) {
        return;
    }

    CVector vecOldTrainPosition = GetPosition();
    float fOldTrainHeading = GetHeading();

    const float& fTotalTrackLength = arrTotalTrackLength[m_nTrackId];
    CTrainNode* trainNodes = pTrackNodes[m_nTrackId];
    auto numTrackNodes = NumTrackNodes[m_nTrackId];

    if (trainFlags.bNotOnARailRoad == 0) {
        if (!trainFlags.bIsFrontCarriage) {
            if (m_pPrevCarriage) {
                m_fTrainSpeed = m_pPrevCarriage->m_fTrainSpeed;
                m_fCurrentRailDistance = m_pPrevCarriage->m_fCurrentRailDistance + m_fLength;
            } else {
                m_fTrainSpeed *= std::pow(0.9900000095367432f, CTimer::GetTimeStep());
                m_fCurrentRailDistance += m_fTrainSpeed * CTimer::GetTimeStep();
            }

            if (trainFlags.b01 && trainFlags.bStoppedAtStation && m_nModelIndex == MODEL_STREAKC && !trainFlags.bMissionTrain) {
                CPlayerPed* localPlayer = FindPlayerPed();
                if (m_nPassengersGenerationState == TRAIN_PASSENGERS_QUERY_NUM_PASSENGERS_TO_LEAVE) {
                    if (localPlayer->m_pVehicle == this) {
                        m_nNumPassengersToLeave = 0;
                    } else {
                        m_nNumPassengersToLeave = (CGeneral::GetRandomNumber() & 3) + 1; // [1, 4]
                    }
                    m_nPassengersGenerationState = TRAIN_PASSENGERS_TELL_PASSENGERS_TO_LEAVE;
                }

                if (m_nPassengersGenerationState == TRAIN_PASSENGERS_TELL_PASSENGERS_TO_LEAVE) {
                    RemoveRandomPassenger();
                    if (m_nNumPassengersToLeave == 0) {
                        m_nPassengersGenerationState = TRAIN_PASSENGERS_QUERY_NUM_PASSENGERS_TO_ENTER;
                    }
                }

                if (m_nPassengersGenerationState == TRAIN_PASSENGERS_QUERY_NUM_PASSENGERS_TO_ENTER) {
                    if (localPlayer->m_pVehicle == this) {
                        m_nNumPassengersToEnter = 0;
                    } else {
                        m_nNumPassengersToEnter = CGeneral::GetRandomNumber() % 4 + 1; // rand(1, 4)
                    }
                    m_nPassengersGenerationState = TRAIN_PASSENGERS_TELL_PASSENGERS_TO_ENTER;
                }

                if (m_nPassengersGenerationState == TRAIN_PASSENGERS_TELL_PASSENGERS_TO_ENTER) {
                    if (trainFlags.bPassengersCanEnterAndLeave) {
                        AddNearbyPedAsRandomPassenger();
                        if (m_nNumPassengersToLeave == m_nNumPassengersToEnter) {
                            m_nPassengersGenerationState = TRAIN_PASSENGERS_GENERATION_FINISHED;
                        }
                    }
                }
            }
        } else {
            CPad* pad = CPad::GetPad();
            if (m_pDriver && m_pDriver->IsPlayer()) {
                pad = m_pDriver->AsPlayer()->GetPadFromPlayer();
            }

            uint32 numCarriagesPulled = FindNumCarriagesPulled();
            if (!trainFlags.bClockwiseDirection) {
                m_fTrainSpeed = -m_fTrainSpeed;
            }

            if (GetStatus()) {
                bool bIsStreakModel = trainFlags.bIsStreakModel;
                auto fStopAtStationSpeed = static_cast<float>(m_autoPilot.m_nCruiseSpeed);

                uint32 timeAtStation = CTimer::GetTimeInMS() - m_nTimeWhenStoppedAtStation;
                if (timeAtStation >= (bIsStreakModel ? 20'000u : 10'000u)) {
                    if (timeAtStation >= (bIsStreakModel ? 28'000u : 18'000u)) {
                        if (timeAtStation >= (bIsStreakModel ? 32'000u : 22'000u)) {
                            if (trainFlags.bStopsAtStations) {
                                float maxTrainSpeed = 0.0f;
                                if (FindMaximumSpeedToStopAtStations(&maxTrainSpeed)) {
                                    fStopAtStationSpeed = 0.0f;
                                    m_nTimeWhenStoppedAtStation = CTimer::GetTimeInMS();
                                } else {
                                    if (fStopAtStationSpeed >= maxTrainSpeed) {
                                        fStopAtStationSpeed = maxTrainSpeed;
                                    }
                                }
                            }
                        } else if (trainFlags.bStoppedAtStation) {
                            CTrain* trainCarriage = this;
                            do {
                                trainFlags.bStoppedAtStation = false;
                                trainCarriage->m_nPassengersGenerationState = TRAIN_PASSENGERS_GENERATION_FINISHED;
                                trainCarriage = trainCarriage->m_pNextCarriage;
                            } while (trainCarriage);
                        }
                    } else {
                        fStopAtStationSpeed = 0.0f;
                        if (trainFlags.bStoppedAtStation) {
                            CTrain* trainCarriage = this;
                            do {
                                trainFlags.bPassengersCanEnterAndLeave = false;
                                trainCarriage->m_nPassengersGenerationState = TRAIN_PASSENGERS_GENERATION_FINISHED;
                                trainCarriage = trainCarriage->m_pNextCarriage;
                            } while (trainCarriage);
                        }
                    }
                } else {
                    fStopAtStationSpeed = 0.0f;
                    if (!trainFlags.bStoppedAtStation) {
                        CTrain* trainCarriage = this;
                        do {
                            trainFlags.bStoppedAtStation = true;
                            trainFlags.bPassengersCanEnterAndLeave = true;
                            trainCarriage->m_nPassengersGenerationState = TRAIN_PASSENGERS_QUERY_NUM_PASSENGERS_TO_LEAVE;
                            trainCarriage = trainCarriage->m_pNextCarriage;
                        } while (trainCarriage);
                    }
                }

                fStopAtStationSpeed = fStopAtStationSpeed / 50.0f - m_fTrainSpeed;
                if (fStopAtStationSpeed > 0.0f) {
                    m_fTrainGas = fStopAtStationSpeed * 30.0f;
                    if (m_fTrainGas >= 1.0f) {
                        m_fTrainGas = 1.0f;
                    }

                    m_fTrainGas *= 255.0f;
                    m_fTrainBrake = 0.0f;
                } else {
                    float fTrainSpeed = fStopAtStationSpeed * -30.0f;
                    m_fTrainGas = 0.0f;
                    if (fTrainSpeed >= 1.0f) {
                        fTrainSpeed = 1.0f;
                    }
                    m_fTrainBrake = fTrainSpeed * 255.0f;
                }
            } else {
                float fTrainSpeed = m_fTrainSpeed;
                if (fTrainSpeed < 0.0f) {
                    fTrainSpeed = -fTrainSpeed;
                }

                if (fTrainSpeed < 0.001f) {
                    m_fTrainBrake = 0.0f;
                    m_fTrainGas = static_cast<float>(pad->GetAccelerate() - pad->GetBrake());
                } else {
                    if (m_fTrainSpeed > 0.0f) {
                        m_fTrainBrake = static_cast<float>(pad->GetBrake());
                        m_fTrainGas = static_cast<float>(pad->GetAccelerate());
                    } else {
                        m_fTrainGas = static_cast<float>(-pad->GetBrake());
                        m_fTrainBrake = static_cast<float>(pad->GetAccelerate());
                    }
                }
            }

            if (trainFlags.bForceSlowDown) {
                const CVector& vecPoint = GetPosition();
                CVector vecDistance{};
                if (CGameLogic::CalcDistanceToForbiddenTrainCrossing(vecPoint, m_vecMoveSpeed, true, vecDistance) < 230.0f) {
                    if (DotProduct(GetForwardVector(), vecDistance) <= 0.0f) {
                        m_fTrainGas = std::max(0.0f, m_fTrainGas);
                    } else {
                        m_fTrainGas = std::min(0.0f, m_fTrainGas);
                    }

                    if (CGameLogic::CalcDistanceToForbiddenTrainCrossing(vecPoint, m_vecMoveSpeed, false, vecDistance) < 230.0f) {
                        m_fTrainBrake = 512.0f;
                    }
                }
            }

            numCarriagesPulled += 3;

            m_fTrainSpeed += m_fTrainGas / 256.0f * CTimer::GetTimeStep() * 0.002f / float(numCarriagesPulled);

            if (m_fTrainBrake != 0.0f) {
                float fTrainSpeed = m_fTrainSpeed;
                if (m_fTrainSpeed < 0.0f) {
                    fTrainSpeed = -fTrainSpeed;
                }
                float fBreak = m_fTrainBrake / 256.0f * CTimer::GetTimeStep() * 0.006f / float(numCarriagesPulled);
                if (fTrainSpeed >= fBreak) {
                    if (m_fTrainSpeed < 0.0f) {
                        m_fTrainSpeed += fBreak;
                    } else {
                        m_fTrainSpeed -= fBreak;
                    }
                } else {
                    m_fTrainSpeed = 0.0f;
                }
            }

            m_fTrainSpeed *= pow(0.999750018119812f, CTimer::GetTimeStep());
            if (!trainFlags.bClockwiseDirection) {
                m_fTrainSpeed = -m_fTrainSpeed;
            }

            m_fCurrentRailDistance += CTimer::GetTimeStep() * m_fTrainSpeed;

            if (GetStatus() == STATUS_PLAYER) {

                float fTheTrainSpeed = m_fTrainSpeed;
                if (fTheTrainSpeed < 0.0f) {
                    fTheTrainSpeed = -fTheTrainSpeed;
                }
                if (fTheTrainSpeed > 1.0f * 0.95f) {
                    CPad::GetPad()->StartShake(300, 70, 0);
                    TheCamera.CamShake(0.1f, GetPosition());
                }

                fTheTrainSpeed = m_fTrainSpeed;
                if (fTheTrainSpeed < 0.0f) {
                    fTheTrainSpeed = -fTheTrainSpeed;
                }

                if (fTheTrainSpeed > 1.0f) {
                    int32 nNodeIndex = m_nNodeIndex;
                    int32 previousNodeIndex = nNodeIndex - 1;
                    if (previousNodeIndex < 0) {
                        previousNodeIndex = numTrackNodes;
                    }

                    int32 previousNodeIndex2 = previousNodeIndex - 1;
                    if (previousNodeIndex2 < 0) {
                        previousNodeIndex2 = numTrackNodes;
                    }

                    CTrainNode* pCurrentTrainNode = &trainNodes[m_nNodeIndex];
                    CTrainNode* pPreviousTrainNode = &trainNodes[previousNodeIndex];
                    CTrainNode* pPreviousTrainNode2 = &trainNodes[previousNodeIndex2];

                    CVector vecDifference1 = pCurrentTrainNode->GetPosn() - pPreviousTrainNode->GetPosn();
                    CVector vecDifference2 = pPreviousTrainNode->GetPosn() - pPreviousTrainNode2->GetPosn();
                    vecDifference1.Normalise();
                    vecDifference2.Normalise();

                    if (DotProduct(vecDifference1, vecDifference2) < 0.996f) {
                        CTrain* carriage = this;
                        bool bIsInTunnel = false;
                        while (!bIsInTunnel) {
                            bIsInTunnel = carriage->IsInTunnel();
                            carriage = carriage->m_pNextCarriage;
                            if (!carriage) {
                                if (!bIsInTunnel) {
                                    CTrain* theTrainCarriage = this;
                                    do {
                                        trainFlags.bNotOnARailRoad = true;
                                        theTrainCarriage->physicalFlags.bDisableCollisionForce = false;
                                        theTrainCarriage->physicalFlags.bDisableSimpleCollision = false;
                                        theTrainCarriage->SetIsStatic(false);
                                        theTrainCarriage = theTrainCarriage->m_pNextCarriage;
                                    } while (theTrainCarriage);

                                    CPhysical::ProcessControl();
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (m_fCurrentRailDistance < 0.0f) {
            do {
                m_fCurrentRailDistance += fTotalTrackLength;
            } while (m_fCurrentRailDistance < 0.0f);
        }

        if (m_fCurrentRailDistance >= fTotalTrackLength) {
            do {
                m_fCurrentRailDistance -= fTotalTrackLength;
            } while (m_fCurrentRailDistance >= fTotalTrackLength);
        }

        float fNextNodeTrackLength = 0.0f;
        int32 nextNodeIndex = m_nNodeIndex + 1;
        if (nextNodeIndex < numTrackNodes) {
            CTrainNode* nextTrainNode = &trainNodes[nextNodeIndex];
            fNextNodeTrackLength = nextTrainNode->GetDistanceFromStart();
        } else {
            fNextNodeTrackLength = fTotalTrackLength;
            nextNodeIndex = 0;
        }

        CTrainNode* theTrainNode = &trainNodes[m_nNodeIndex];
        float fCurrentNodeTrackLength = theTrainNode->GetDistanceFromStart();
        while (m_fCurrentRailDistance < fCurrentNodeTrackLength || fNextNodeTrackLength < m_fCurrentRailDistance) {
            int32 newNodeIndex = m_nNodeIndex - 1; // previous node
            if (fCurrentNodeTrackLength <= m_fCurrentRailDistance) {
                newNodeIndex = m_nNodeIndex + 1; // next node
            }
            m_nNodeIndex = newNodeIndex % numTrackNodes;
            m_vehicleAudio.AddAudioEvent(AE_TRAIN_CLACK, 0.0f);

            theTrainNode = &trainNodes[m_nNodeIndex];
            fCurrentNodeTrackLength = theTrainNode->GetDistanceFromStart();

            nextNodeIndex = m_nNodeIndex + 1;
            if (nextNodeIndex < numTrackNodes) {
                CTrainNode* nextTrainNode = &trainNodes[nextNodeIndex];
                fNextNodeTrackLength = nextTrainNode->GetDistanceFromStart();
            } else {
                fNextNodeTrackLength = fTotalTrackLength;
                nextNodeIndex = 0;
            }
        }

        CTrainNode* nextTrainNode = &trainNodes[nextNodeIndex];
        fNextNodeTrackLength = nextTrainNode->GetDistanceFromStart();

        float fTrackNodeDifference = fNextNodeTrackLength - fCurrentNodeTrackLength;
        if (fTrackNodeDifference < 0.0f) {
            fTrackNodeDifference += fTotalTrackLength;
        }

        float fTheDistance = (m_fCurrentRailDistance - fCurrentNodeTrackLength) / fTrackNodeDifference;
        CVector vecPosition1 = theTrainNode->GetPosn() * (1.0f - fTheDistance) + nextTrainNode->GetPosn() * fTheDistance;

        CColModel* vehicleColModel = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel();
        const CBoundingBox& bbox = vehicleColModel->GetBoundingBox();
        float fTotalCurrentRailDistance = bbox.GetLength() + m_fCurrentRailDistance;
        if (fTotalCurrentRailDistance > fTotalTrackLength) {
            fTotalCurrentRailDistance -= fTotalTrackLength;
        }

        nextNodeIndex = m_nNodeIndex + 1;
        if (nextNodeIndex < numTrackNodes) {
            fNextNodeTrackLength = trainNodes[nextNodeIndex].GetDistanceFromStart();
        } else {
            fNextNodeTrackLength = fTotalTrackLength;
            nextNodeIndex = 0;
        }

        int32 trainNodeIndex = m_nNodeIndex;
        while (fTotalCurrentRailDistance < fCurrentNodeTrackLength || fTotalCurrentRailDistance > fNextNodeTrackLength) {
            trainNodeIndex = (trainNodeIndex + 1) % numTrackNodes;

            theTrainNode = &trainNodes[trainNodeIndex];
            fCurrentNodeTrackLength = theTrainNode->GetDistanceFromStart();

            nextNodeIndex = trainNodeIndex + 1;
            if (nextNodeIndex < numTrackNodes) {
                fNextNodeTrackLength = trainNodes[nextNodeIndex].GetDistanceFromStart();
            } else {
                fNextNodeTrackLength = fTotalTrackLength;
                nextNodeIndex = 0;
            }
        }

        fNextNodeTrackLength = trainNodes[nextNodeIndex].GetDistanceFromStart();

        fTrackNodeDifference = fNextNodeTrackLength - fCurrentNodeTrackLength;
        if (fTrackNodeDifference < 0.0f) {
            fTrackNodeDifference += fTotalTrackLength;
        }

        fTheDistance = (fTotalCurrentRailDistance - fCurrentNodeTrackLength) / fTrackNodeDifference;
        CVector vecPosition2 = theTrainNode->GetPosn() * (1.0f - fTheDistance) + trainNodes[nextNodeIndex].GetPosn() * fTheDistance;

        {
            CVector& vecVehiclePosition = GetPosition();
            vecVehiclePosition = (vecPosition1 + vecPosition2) / 2.0f;
            vecVehiclePosition.z += m_pHandlingData->m_fSuspensionLowerLimit - bbox.m_vecMin.z;
        }

        GetForward() = vecPosition2 - vecPosition1;
        GetForward().Normalise();
        if (!trainFlags.bClockwiseDirection) {
            GetForward() *= -1.0f;
        }

        CVector vecTemp(0.0f, 0.0f, 1.0f);
        CrossProduct(&GetRight(), &GetForward(), &vecTemp);
        GetRight().Normalise();
        CrossProduct(&GetUp(), &GetRight(), &GetForward());

        auto fTrainNodeLighting     = theTrainNode->GetLightingFromCollision().GetCurrentLighting();
        auto fTrainNextNodeLighting = trainNodes[nextNodeIndex].GetLightingFromCollision().GetCurrentLighting();

        fTrainNodeLighting += (fTrainNextNodeLighting - fTrainNodeLighting) * fTheDistance;
        m_fContactSurfaceBrightness = fTrainNodeLighting;
        m_vecMoveSpeed = (1.0f / CTimer::GetTimeStep()) * (GetPosition() - vecOldTrainPosition);

        float fNewTrainHeading = GetHeading();
        float fHeading = fNewTrainHeading - fOldTrainHeading;
        if (fHeading <= PI) {
            if (fHeading < -PI) {
                fHeading += TWO_PI;
            }
        } else {
            fHeading -= TWO_PI;
        }

        m_vecTurnSpeed = CVector(0.0f, 0.0f, fHeading / CTimer::GetTimeStep());

        if (trainFlags.bNotOnARailRoad) {
            m_vecMoveSpeed *= -1.0f;
            m_vecTurnSpeed *= -1.0f;

            ApplyMoveSpeed();

            m_vecMoveSpeed *= -1.0f;
            m_vecTurnSpeed *= -1.0f;

            CPhysical::ProcessControl();
        } else {
            m_vecMoveSpeed.x = std::clamp(m_vecMoveSpeed.x, -2.0f, 2.0f);
            m_vecMoveSpeed.y = std::clamp(m_vecMoveSpeed.y, -2.0f, 2.0f);
            m_vecMoveSpeed.z = std::clamp(m_vecMoveSpeed.z, -2.0f, 2.0f);

            m_vecTurnSpeed.x = std::clamp(m_vecTurnSpeed.x, -0.1f, 0.1f);
            m_vecTurnSpeed.y = std::clamp(m_vecTurnSpeed.y, -0.1f, 0.1f);
            m_vecTurnSpeed.z = std::clamp(m_vecTurnSpeed.z, -0.1f, 0.1f);
        }

        UpdateRwMatrix();
        UpdateRwFrame();
        RemoveAndAdd();

        SetIsStuck(false);
        SetWasPostponed(false);
        SetIsInSafePosition(true);

        m_fMovingSpeed = DistanceBetweenPoints(GetPosition(), vecOldTrainPosition);

        if (trainFlags.bIsFrontCarriage || trainFlags.bIsLastCarriage) {
            CVector vecPoint = bbox.m_vecMax.y * GetForward();
            vecPoint += GetPosition();
            vecPoint += CTimer::GetTimeStep() * m_vecMoveSpeed;

            MarkSurroundingEntitiesForCollisionWithTrain(vecPoint, 3.0f, this, false);
        }

        if (!vehicleFlags.bWarnedPeds) {
            CCarCtrl::ScanForPedDanger(this);
        }
        return;
    } else {
        if (!GetIsStuck()) {
            float fMaxForce = 0.003f;
            float fMaxTorque = 0.0009f;
            float fMaxMovingSpeed = 0.005f;

            if (GetStatus() != STATUS_PLAYER) {
                fMaxForce = 0.006f;
                fMaxTorque = 0.0015f;
                fMaxMovingSpeed = 0.015f;
            }

            float fMaxForceTimeStep  = (fMaxForce  * CTimer::GetTimeStep()) * (fMaxForce * CTimer::GetTimeStep());
            float fMaxTorqueTimeStep = (fMaxTorque * CTimer::GetTimeStep()) * (fMaxTorque * CTimer::GetTimeStep());

            m_vecForce  = (m_vecForce  + m_vecMoveSpeed) / 2.0f;
            m_vecTorque = (m_vecTorque + m_vecTurnSpeed) / 2.0f;

            if (m_vecForce.SquaredMagnitude() > fMaxForceTimeStep ||
                m_vecTorque.SquaredMagnitude() > fMaxTorqueTimeStep ||
                m_fMovingSpeed >= fMaxMovingSpeed ||
                m_fDamageIntensity > 0.0f && m_pDamageEntity != nullptr && m_pDamageEntity->GetIsTypePed()
            ) {
                m_nFakePhysics = 0;
            } else {
                m_nFakePhysics += 1;
                if (m_nFakePhysics > 10 /*&& !plugin::Call<0x424100>()*/) {
                    // if (m_nFakePhysics > 10) { // OG redundant check
                        m_nFakePhysics = 10;
                    // }

                    ResetMoveSpeed();
                    ResetTurnSpeed();
                    SkipPhysics();
                    return;
                }
            }
        }

        CPhysical::ProcessControl();

        CVector vecMoveForce{}, vecTurnForce{};
        if (mod_Buoyancy.ProcessBuoyancy(this, m_fBuoyancyConstant, &vecMoveForce, &vecTurnForce)) {
            physicalFlags.bTouchingWater = true;

            float fTimeStep = 0.01f;
            if (CTimer::GetTimeStep() >= 0.01f) {
                fTimeStep = CTimer::GetTimeStep();
            }

            float fSpeedFactor = 1.0f - vecOldTrainPosition.z / (fTimeStep * m_fMass * 0.008f) * 0.05f;
            fSpeedFactor = std::pow(fSpeedFactor, CTimer::GetTimeStep());

            m_vecMoveSpeed *= fSpeedFactor;
            m_vecTurnSpeed *= fSpeedFactor;
            ApplyMoveForce(vecOldTrainPosition.x, vecOldTrainPosition.y, vecOldTrainPosition.z);
            ApplyTurnForce(vecTurnForce, vecMoveForce);
        }
    }
}

