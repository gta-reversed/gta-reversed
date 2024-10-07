/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
#include "Vector.h"
class FxSystem;

enum eSwimState : uint16 {
    SWIM_TREAD                = 0,
    SWIM_SPRINT               = 1,
    SWIM_SPRINTING            = 2,
    SWIM_DIVE_UNDERWATER      = 3,
    SWIM_UNDERWATER_SPRINTING = 4,
    SWIM_BACK_TO_SURFACE      = 5
};

class CPed;
class CPlayerPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleSwim : public CTaskSimple {
public:
    bool        m_bFinishedBlending;
    bool        m_bAnimBlockRefAdded;
    eSwimState  m_nSwimState;
    AnimationId m_AnimID;
    float       m_fAnimSpeed;
    CVector     m_vecPos;
    CPed*       m_pPed;
    float       m_fRotationX;           // updated all the time.
    float       m_fTurningRotationY;    // updated when ped is turning.
    float       m_fUpperTorsoRotationX; //  updated when ped diving or coming back to surface
    float       m_fAimingRotation;      // The direction that the ped is facing. Updated all the time.
    float       m_fStateChanger;        // It is normally 0, but updated when changing swimming states.
    CEntity*    m_pEntity;
    CVector     m_pClimbPos;
    float       m_fAngle;
    uint8       m_nSurfaceType;
    float       m_fRandomMoveBlendRatio; // Used in CTaskSimpleSwim::ProcessControlAI
    float       m_fSwimStopTime;
    uint32      m_nTimeStep;
    FxSystem_c* m_pFxSystem;
    bool        m_bTriggerWaterSplash;

    static constexpr inline float SWIM_DIVE_UNDER_ANGLE = -35.0f; // 0x8D2FC4
    static constexpr inline float SWIM_STOP_TIME = 15.0f; // 0x8D2FC0
    // static constexpr inline float LEG_SWING_MAX_ANGLE = 0.7854f; //

public:
    static constexpr auto Type = TASK_SIMPLE_SWIM;

    CTaskSimpleSwim(const CVector* pos, CPed* ped);
    ~CTaskSimpleSwim() override;

    CTask* Clone() const override { return new CTaskSimpleSwim{&m_vecPos, m_pPed}; } // 0x68B050
    eTaskType GetTaskType() const override { return Type; }; // 0x6889F0
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;

    void CreateFxSystem(CPed* ped, RwMatrix* pRwMatrix);
    void DestroyFxSystem();

    void ApplyRollAndPitch(CPed* ped) const;
    void ProcessSwimAnims(CPed* ped);
    void ProcessSwimmingResistance(CPed* ped);
    void ProcessEffects(CPed* ped);
    void ProcessControlAI(CPed* ped);
    void ProcessControlInput(CPlayerPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimpleSwim* Constructor(CVector* pos, CPed* ped);
};
VALIDATE_SIZE(CTaskSimpleSwim, 0x64);
