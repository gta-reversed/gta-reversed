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
    float      m_fOpenAngle;
    float      m_fClosedAngle;

    uint16     m_nDirn; // Def a bitfield. 2 x 4 bits + some more
    uint8      m_nAxis; // X, Y, Z - Right/left, front/back, up/down
    eDoorState m_nDoorState;

    float      m_fAngle;
    float      m_fPrevAngle;
    float      m_fAngVel;    // Angular velocity

    static constexpr float DOOR_SPEED_MAX_CAPPED = 0.5f; // 0x8D3950

public:
    bool Process(CVehicle* vehicle, CVector& vecOldMoveSpeed, CVector& vecOldTurnSpeed, Const CVector& vecOffset);
    bool ProcessImpact(CVehicle* vehicle, CVector& vecOldMoveSpeed, CVector& vecOldTurnSpeed, Const CVector& vecOffset);
    void Open(float angRatio);

    float RetDoorAngVel() const { return m_fAngVel; }
    float RetDoorAngle() const { return m_fAngle; }
    float RetAngleWhenClosed() const { return m_fClosedAngle; }
    float RetAngleWhenOpen() const { return m_fOpenAngle; }
    float GetAngleOpenRatio() const;
    bool  IsClosed() const;
    bool  IsFullyOpen() const;

    uint8 RetAxes() const { return m_nAxis; }
    uint8 RetDoorState() const { return m_nDoorState; }

    // NOTSA
    void    OpenFully()  { m_fAngle = m_fOpenAngle; }
    void    CloseFully() { m_fAngle = m_fClosedAngle; }
    CVector GetRotation() const;
    void    UpdateFrameMatrix(CMatrix& mat);
    static void InjectHooks();
};

VALIDATE_SIZE(CDoor, 0x18);
