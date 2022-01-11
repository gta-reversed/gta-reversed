/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"
#include "PathFind.h"
#include "eCarMission.h"

enum eCarDrivingStyle : int8
{
    DRIVINGSTYLE_STOP_FOR_CARS,
    DRIVINGSTYLE_SLOW_DOWN_FOR_CARS,
    DRIVINGSTYLE_AVOID_CARS,
    DRIVINGSTYLE_PLOUGH_THROUGH,
    DRIVINGSTYLE_STOP_FOR_CARS_IGNORE_LIGHTS
};

class CAutoPilot {
public:
    CAutoPilot();

public:
    CNodeAddress        m_currentAddress;
    CNodeAddress        m_startingRouteNode;
    CNodeAddress        m_endingRouteNode;
    int32               field_C;
    uint32              m_nSpeedScaleFactor;
    CCarPathLinkAddress m_nCurrentPathNodeInfo;
    CCarPathLinkAddress m_nNextPathNodeInfo;
    CCarPathLinkAddress m_nPreviousPathNodeInfo;
    char                field_1A[2];
    uint32              m_nTimeToStartMission;
    uint32              m_nTimeSwitchedToRealPhysics;
    int8                _smthPrev;
    int8                _smthCurr;
    int8                _smthNext;
    int8                m_nCurrentLane;
    int8                m_nNextLane;
    eCarDrivingStyle    m_nCarDrivingStyle;
    eCarMission         m_nCarMission;
    char                m_nTempAction;
    uint32              m_nTempActionTime;
    uint32              _someStartTime;
    uint8               m_ucTempActionMode;
    uint8               m_ucCarMissionModeCounter;
    char                field_36[2];
    float               m_speed;
    float               m_fMaxTrafficSpeed;
    uint8               m_nCruiseSpeed;
    char                field_41;
    char                field_42[2];
    float               field_44;
    uint8               m_ucHeliTargetDist;
    uint8               m_ucHeliSpeedMult;
    char                field_4A;
    union {
        uint8 m_nCarCtrlFlags;
        struct carCtrlFlags {
            uint8 bHonkAtCar : 1;
            uint8 bHonkAtPed : 1;
            uint8 bAvoidLevelTransitions : 1;
            uint8 bStayInFastLane : 1;
            uint8 bStayInSlowLane : 1;
            uint8 bDoTargetCatchupCheck : 1;
            uint8 bCantGoAgainstTraffic : 1;
            uint8 bHeliFollowTarget : 1;
        } carCtrlFlags;
    };
    union {
        uint8 m_nMovementFlags;
        struct movementFlags {
            uint8 bIsStopped : 1;
            uint8 bIsParked : 1;
        } movementFlags;
    };
    char            m_nStraightLineDistance;
    uint8           m_ucCarFollowDist;
    uint8           m_ucHeliTargetDist2;
    char            field_50;
    char            field_51;
    char            field_52[10];
    CVector         m_vecDestinationCoors;
    CNodeAddress    m_aPathFindNodesInfo[8];
    uint16          m_nPathFindNodesCount;
    char            field_8A[2];
    class CVehicle* m_pTargetCar;
    class CEntity*  m_pCarWeMakingSlowDownFor;
    int8            m_vehicleRecordingId;
    bool            m_bPlaneDogfightSomething;
    int16           field_96;

    void SetCarMission(eCarMission carMission) {
        if (m_nCarMission != MISSION_CRASH_PLANE_AND_BURN && m_nCarMission != MISSION_CRASH_HELI_AND_BURN)
            m_nCarMission = carMission;
    }

    void ModifySpeed(float target);
};

VALIDATE_SIZE(CAutoPilot, 0x98);
