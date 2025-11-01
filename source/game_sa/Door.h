/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"

class CVehicle;

enum eDoorState : uint8 {
    DOOR_NOTHING,
    DOOR_HIT_MAX_END,
    DOOR_HIT_MIN_END,
    DOOR_POP_OPEN,
    DOOR_SLAM_SHUT
};

class CDoor {
public:
    float      m_openAngle{};
    float      m_closedAngle{};

    uint16     m_dirn{}; // Def a bitfield. 2 x 4 bits + some more
    uint8      m_axis{}; // X, Y, Z - Right/left, front/back, up/down
    eDoorState m_doorState{};

    float      m_angle{};
    float      m_prevAngle{};
    float      m_angVel{};    // Angular velocity

    static constexpr float DOOR_SPEED_MAX_CAPPED = 0.5f; // 0x8D3950

public:
    bool Process(CVehicle* vehicle, CVector& vecOldMoveSpeed, CVector& vecOldTurnSpeed, Const CVector& vecOffset);
    bool ProcessImpact(CVehicle* vehicle, CVector& vecOldMoveSpeed, CVector& vecOldTurnSpeed, Const CVector& vecOffset);
    void Open(float angRatio);

    float RetDoorAngVel() const { return m_angVel; }
    float RetDoorAngle() const { return m_angle; }
    float RetAngleWhenClosed() const { return m_closedAngle; }
    float RetAngleWhenOpen() const { return m_openAngle; }
    float GetAngleOpenRatio() const;
    bool  IsClosed() const;
    bool  IsFullyOpen() const;

    uint8 RetAxes() const { return m_axis; }
    uint8 RetDoorState() const { return m_doorState; }

    // NOTSA
    void    OpenFully()  { m_angle = m_openAngle; }
    void    CloseFully() { m_angle = m_closedAngle; }
    CVector GetRotation() const;
    void    UpdateFrameMatrix(CMatrix& mat);
    static void InjectHooks();
};

VALIDATE_SIZE(CDoor, 0x18);
