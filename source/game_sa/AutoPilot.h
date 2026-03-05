/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Base.h"

#include "PathFind.h"
#include "NodeAddress.h"
#include "Vector.h"
#include "eCarMission.h"
#include "eCarDrivingStyle.h"

static constexpr size_t CAR_NUM_PATHNODES_LOOKAHEAD = 8;

class CVehicle;
class CEntity;

enum eAutoPilotTempAction : int8 {
    TEMPACT_NONE                     = 0,
    TEMPACT_WAIT                     = 1,
    TEMPACT_EMPTYTOBEREUSED          = 2,
    TEMPACT_REVERSE                  = 3,
    TEMPACT_HANDBRAKETURNLEFT        = 4,
    TEMPACT_HANDBRAKETURNRIGHT       = 5,
    TEMPACT_HANDBRAKESTRAIGHT        = 6,
    TEMPACT_TURNLEFT                 = 7,
    TEMPACT_TURNRIGHT                = 8,
    TEMPACT_GOFORWARD                = 9,
    TEMPACT_SWIRVELEFT               = 10,
    TEMPACT_SWIRVERIGHT              = 11,
    TEMPACT_STUCKINTRAFFIC           = 12,
    TEMPACT_REVERSE_LEFT             = 13,
    TEMPACT_REVERSE_RIGHT            = 14,
    TEMPACT_PLANE_FLY_UP             = 15,
    TEMPACT_PLANE_FLY_STRAIGHT       = 16,
    TEMPACT_PLANE_SHARP_LEFT         = 17,
    TEMPACT_PLANE_SHARP_RIGHT        = 18,
    TEMPACT_HEADON_COLLISION         = 19,
    TEMPACT_SWIRVELEFT_STOP          = 20,
    TEMPACT_SWIRVERIGHT_STOP         = 21,
    TEMPACT_REVERSE_STRAIGHT         = 22,
    TEMPACT_BOOST_USE_STEERING_ANGLE = 23,
    TEMPACT_BRAKE                    = 24,
};

class CAutoPilot {
public:
    CNodeAddress        m_OldNode, m_NewNode, m_VeryOldNode;
    uint32              m_TimeToLeaveLink; // Time
    uint32              m_TimeToGetToNextLink; // Time
    CCarPathLinkAddress m_OldLink, m_NewLink, m_VeryOldLink;

    uint32              m_LastTimeNotStuck; // Time
    uint32              m_LastTimeMoving; // Time
    int8                m_InvertDirVeryOldLink, m_InvertDirOldLink, m_InvertDirNewLink;
    int8                m_OldLane, m_NewLane;
    eCarDrivingStyle    m_DrivingMode;
    eCarMission         m_Mission;
    eAutoPilotTempAction m_TempAction;
    uint32              m_TempActionFinish; // Time
    uint32              m_LastTimeWeStartedTempActReverse; // Time
    uint8               m_WhatToTryForReverse;
    uint8               m_NumTimesWantingToChangeNodes;
    float               m_ActualSpeed;
    float               m_MaxSpeedBuffer;
    uint8               m_CruiseSpeed;
    int8                m_SpeedFromNodes;
    float               m_SpeedMult;
    uint8               m_HooverDistFromTarget; // unused?
    uint8               m_SpeedCheat;
    int8                m_AimAheadOfTarget;
    union {
        uint8 m_nCarCtrlFlags;
        struct carCtrlFlags {
            uint8 bHonkAtCar : 1; // SlowingDownForCar
            uint8 bHonkAtPed : 1; // SlowingDownForPed
            uint8 bAvoidLevelTransitions : 1; // AvoidLevelTransitions
            uint8 bStayInFastLane : 1; // AlwaysInFastLane
            uint8 bStayInSlowLane : 1; // AlwaysInSlowLane
            uint8 bDoTargetCatchupCheck : 1; // WarnTargetEntity
            uint8 bCantGoAgainstTraffic : 1; // DontGoAgainstTraffic
            uint8 bHeliFollowTarget : 1; // LeaveAfterAWhile
        } carCtrlFlags;
    };
    union {
        uint8 m_nMovementFlags;
        struct movementFlags {
            uint8 bIsStopped : 1; // WaitForValidNodes
            uint8 bIsParked : 1; // CarHasToReverseFirst
        } movementFlags;
    };
    uint8           m_AISwitchToStraightLineDist;
    uint8           m_FollowCarDist; // default 10
    uint8           m_TargetReachedDist; // default 10
    int8            m_LaneChangeCounter;
    int8            m_FramesFloating;

    // NOTE: Added in Mobile?
    int16           m_ConstrainAreaMinX, m_ConstrainAreaMaxX; // unused
    int16           m_ConstrainAreaMinY, m_ConstrainAreaMaxY; // unused

    CVector         m_TargetCoors;
    CNodeAddress    m_PathNodeList[CAR_NUM_PATHNODES_LOOKAHEAD];
    uint16          m_NumPathNodes;
    CVehicle*       m_TargetEntity; // CEntity
    CEntity*        m_ObstructingEntity; // Entity to slow down for

    int8            m_RecordingNumber;

    int8            m_Diversion;

    CAutoPilot();

    void ModifySpeed(float target);
    void RemoveOnePathNode();

    void SetCarMission(eCarMission carMission) { m_Mission = carMission; }

    void SetCarMission(eCarMission carMission, uint32 timeOffsetMs);

    void ClearCarMission() { m_Mission = MISSION_NONE; }

    void SetCruiseSpeed(uint32 s) { assert(s <= UINT8_MAX); m_CruiseSpeed = (uint8)s; }

    /*!
     * @notsa
     * @brief Set the temporary action
     * @param action The action set add
     * @param durMs  The action's duration in milliseconds
    */
    void SetTempAction(eAutoPilotTempAction action, uint32 durMs);

    /*!
     * @brief Clear current temp. act.
    */
    void ClearTempAct() { m_TempAction = TEMPACT_NONE; }

    void SetDrivingStyle(eCarDrivingStyle s) { m_DrivingMode = s; }
};

VALIDATE_SIZE(CAutoPilot, 0x98);
