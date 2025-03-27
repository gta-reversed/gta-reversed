/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vehicle.h"
#include "Door.h"
#include "TrainNode.h"

enum eTrainNodes {
    TRAIN_NODE_NONE   = 0,
    TRAIN_DOOR_LF     = 1,
    TRAIN_DOOR_RF     = 2,
    TRAIN_WHEEL_RF1   = 3,
    TRAIN_WHEEL_RF2   = 4,
    TRAIN_WHEEL_RF3   = 5,
    TRAIN_WHEEL_RB1   = 6,
    TRAIN_WHEEL_RB2   = 7,
    TRAIN_WHEEL_RB3   = 8,
    TRAIN_WHEEL_LF1   = 9,
    TRAIN_WHEEL_LF2   = 10,
    TRAIN_WHEEL_LF3   = 11,
    TRAIN_WHEEL_LB1   = 12,
    TRAIN_WHEEL_LB2   = 13,
    TRAIN_WHEEL_LB3   = 14,
    TRAIN_BOGIE_FRONT = 15,
    TRAIN_BOGIE_REAR  = 16,

    TRAIN_NUM_NODES
};

enum eTrainPassengersGenerationState {
    TRAIN_PASSENGERS_QUERY_NUM_PASSENGERS_TO_LEAVE = 0,
    TRAIN_PASSENGERS_TELL_PASSENGERS_TO_LEAVE = 1,
    TRAIN_PASSENGERS_QUERY_NUM_PASSENGERS_TO_ENTER = 2,
    TRAIN_PASSENGERS_TELL_PASSENGERS_TO_ENTER = 3,
    TRAIN_PASSENGERS_GENERATION_FINISHED = 4
};

class NOTSA_EXPORT_VTABLE CTrain : public CVehicle {
public:
    int16    m_nNodeIndex;
    float    m_fTrainSpeed; // 1.0 - train derails
    float    m_fCurrentRailDistance;
    float    m_fLength;
    float    m_fTrainGas;   // gas pedal pressed: 255.0, moving forward: 0.0f, moving back: -255.0
    float    m_fTrainBrake; // 255.0 - braking
    union {
        struct {
            uint16 b01 : 1; // initialised with 1
            uint16 bStoppedAtStation : 1;
            uint16 bPassengersCanEnterAndLeave : 1;
            uint16 bIsFrontCarriage : 1;
            uint16 bIsLastCarriage : 1;
            uint16 bMissionTrain : 1;
            uint16 bClockwiseDirection : 1;
            uint16 bStopsAtStations : 1;

            uint16 bNotOnARailRoad : 1;
            uint16 bForceSlowDown : 1;
            uint16 bIsStreakModel : 1;
        } trainFlags;
        uint16 m_nTrainFlags;
    };
    uint32   m_nTimeWhenStoppedAtStation;
    int8     m_nTrackId;
    uint32   m_nTimeWhenCreated;
    int16    field_5C8;                    // initialized with 0, not referenced
    uint8    m_nPassengersGenerationState; // see eTrainPassengersGenerationState
    uint8    m_nNumPassengersToLeave : 4;  // 0 to 4
    uint8    m_nNumPassengersToEnter : 4;  // 0 to 4
    CPed*    m_pTemporaryPassenger;        // we tell peds to enter train and then delete them
    CTrain*  m_pPrevCarriage;
    CTrain*  m_pNextCarriage;
    CDoor    m_aDoors[6];
    RwFrame* m_aTrainNodes[TRAIN_NUM_NODES];

    static uint32& GenTrain_Track;
    static uint32& GenTrain_TrainConfig;
    static uint8& GenTrain_Direction;
    static uint32& GenTrain_GenerationNode;
    static uint32& GenTrain_Status;
    static bool& bDisableRandomTrains;
    static CVector aStationCoors[6];

    static constexpr auto Type = VEHICLE_TYPE_TRAIN;

public:
    CTrain(int32 modelIndex, eVehicleCreatedBy createdBy);

    void ProcessControl() override;
    void SetupModelNodes();

    bool FindMaximumSpeedToStopAtStations(float* speed);
    uint32 FindNumCarriagesPulled();
    void OpenTrainDoor(float state);
    void AddPassenger(CPed* ped);
    void RemovePassenger(CPed* ped);
    [[nodiscard]] bool FindSideStationIsOn() const;
    [[nodiscard]] bool IsInTunnel() const;
    void RemoveRandomPassenger();
    void FindPositionOnTrackFromCoors();
    void AddNearbyPedAsRandomPassenger();

    static void InitTrains();
    static void ReadAndInterpretTrackFile(const char* filename, CTrainNode** nodes, int32* lineCount, float* totalDist, int32 skipStations);
    static void Shutdown();
    static void UpdateTrains();
    static void FindCoorsFromPositionOnTrack(float railDistance, int32 trackId, CVector* outCoors);
    static void DisableRandomTrains(bool disable);
    static void RemoveOneMissionTrain(CTrain* train);
    static void ReleaseOneMissionTrain(CTrain* train);
    static void SetTrainSpeed(CTrain* train, float speed);
    static void SetTrainCruiseSpeed(CTrain* train, float speed);
    static CTrain* FindCaboose(CTrain* train);
    static CTrain* FindEngine(CTrain* train);
    static CTrain* FindCarriage(CTrain* train, uint8 carriage);
    static void FindNextStationPositionInDirection(bool clockwiseDirection, float distance, float* distanceToStation, int32* numStations);
    static void RemoveMissionTrains();
    static void RemoveAllTrains();
    static void ReleaseMissionTrains();
    static int32 FindClosestTrackNode(CVector posn, int32* outTrackId);
    static CTrain* FindNearestTrain(CVector posn, bool mustBeMainTrain);
    static void SetNewTrainPosition(CTrain* train, CVector posn);
    static bool IsNextStationAllowed(CTrain* train);
    static void SkipToNextAllowedStation(CTrain* train);
    static void CreateMissionTrain(CVector posn, bool clockwiseDirection, uint32 trainType, CTrain**outFirstCarriage, CTrain**outLastCarriage, int32 nodeIndex, int32 trackId, bool isMissionTrain);
    static void DoTrainGenerationAndRemoval();

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTrain* Constructor(int32 modelIndex, eVehicleCreatedBy createdBy) { this->CTrain::CTrain(modelIndex, createdBy); return this; }
};
VALIDATE_SIZE(CTrain, 0x6AC);
VALIDATE_OFFSET(CTrain, m_nTrainFlags, 0x5B8);

void ProcessTrainAnnouncements();
void PlayAnnouncement(uint8 arg0, uint8 arg1);
void TrainHitStuff(CPtrList& ptrList, CEntity* entity);
void MarkSurroundingEntitiesForCollisionWithTrain(CVector pos, float radius, CEntity* entity, bool bOnlyVehicles);
