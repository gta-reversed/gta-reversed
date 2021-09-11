#pragma once
#include "CTaskSimple.h"

class CTaskSimpleJump : public CTaskSimple
{
public:
    CVector m_vecClimbPos;
    float m_fClimbAngle;
    uint8 m_nClimbSurfaceType;
    uint8 _pad_19[3];
    CEntity* m_pClimbEntity;
    uint8 m_bIsFinished;
    uint8 m_bIsJumpBlocked;
    uint8 m_bClimbJump;
    uint8 m_bLaunchAnimStarted;
    uint8 m_bCanClimb;
    uint8 m_bHighJump;
    uint8 _pad_26[2];
    CAnimBlendAssociation* m_pAnim;

private:
    CTaskSimpleJump* Constructor(bool bCanClimb);
public:
    CTaskSimpleJump(bool bCanClimb);
    ~CTaskSimpleJump() override;

    static void InjectHooks();

    CTask* Clone() override;
    eTaskType GetTaskType() override { return TASK_SIMPLE_JUMP; }
    bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    bool ProcessPed(CPed* ped) override;

    CTask* Clone_Reversed();
    bool MakeAbortable_Reversed(CPed* ped, eAbortPriority priority, const CEvent* event);
    bool ProcessPed_Reversed(CPed* ped);

    bool CheckIfJumpBlocked(CPed* ped);
    void Launch(CPed* ped);
    bool StartLaunchAnim(CPed* ped);
    static void JumpAnimFinishCB(CAnimBlendAssociation* pAnim, void* data);
};

VALIDATE_SIZE(CTaskSimpleJump, 0x2C);
