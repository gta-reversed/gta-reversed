#include "StdInc.h"

#include "AutoPilot.h"

// 0x6D5E20
CAutoPilot::CAutoPilot() : m_PathNodeList() {
    m_InvertDirNewLink = 1;
    m_InvertDirOldLink = 1;

    m_nCarCtrlFlags = 0;

    m_TimeToLeaveLink = 0;
    m_TimeToGetToNextLink = 1000;
    m_NewLane = 0;
    m_OldLane = 0;
    m_DrivingMode = DRIVING_STYLE_STOP_FOR_CARS;
    m_Mission = eCarMission::MISSION_NONE;
    m_TempAction = TEMPACT_NONE;
    SetCruiseSpeed(10);
    m_ActualSpeed = 10.0F;
    m_NumPathNodes = 0;
    m_TargetEntity = nullptr;

    m_LastTimeNotStuck = CTimer::GetTimeInMS();
    m_LastTimeMoving = CTimer::GetTimeInMS();
    m_LastTimeWeStartedTempActReverse = 0;

    m_AISwitchToStraightLineDist = 20;
    m_WhatToTryForReverse = 0;
    m_NumTimesWantingToChangeNodes = 0;
    m_SpeedFromNodes = 0;
    m_SpeedMult = 1.0f;
    m_SpeedCheat = 0;
    movementFlags.bIsStopped = false;
    movementFlags.bIsParked = false;
    m_AimAheadOfTarget = 0;
    m_FollowCarDist = 10;
    m_TargetReachedDist = 10;
    m_LaneChangeCounter = CGeneral::GetRandomNumber() % 8 + 2;
    m_RecordingNumber = -1;
    m_Diversion = false;
    m_ObstructingEntity = nullptr;
    m_MaxSpeedBuffer = 0.0F;
}

void CAutoPilot::ModifySpeed(float target) {
    plugin::CallMethod<0x41B980, CAutoPilot*, float>(this, target);
}

// 0x41B950
void CAutoPilot::RemoveOnePathNode() {
    plugin::CallMethod<0x41B950, CAutoPilot*>(this);
}

void CAutoPilot::SetCarMission(eCarMission carMission, uint32 timeOffsetMs) {
    m_Mission = carMission;
    m_LastTimeNotStuck = CTimer::GetTimeInMS() + timeOffsetMs;
}

// notsa
void CAutoPilot::SetTempAction(eAutoPilotTempAction action, uint32 durMs) {
    m_TempAction = action;
    m_TempActionFinish = CTimer::GetTimeInMS() + durMs;
}
