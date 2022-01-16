#pragma once

#include "TaskComplex.h"
#include "Heli.h"

class CTaskComplexUseSwatRope : public CTaskComplex {
public:
    uint32 m_nRopeId;
    float  m_fCoorAlongRope;
    uint8  m_bIsOnHeli;
    uint8  _pad_15[3];
    CHeli* m_pHeli;

public:
    CTaskComplexUseSwatRope(uint32 ropeId, CHeli* heli);
    CTaskComplexUseSwatRope(uint32 ropeId);
    ~CTaskComplexUseSwatRope() override;

    CTask* Clone() override;
    eTaskType GetTaskType() override { return TASK_COMPLEX_USE_SWAT_ROPE; }
    bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;
    virtual bool IsInterruptable(CPed* ped) { return false; }
    virtual bool AbortUrgently(CPed* ped) { return false; }

    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexUseSwatRope* Constructor(uint32 ropeId, CHeli* heli);

    CTask* Clone_Reversed();
    bool MakeAbortable_Reversed(CPed* ped, eAbortPriority priority, const CEvent* event);
    CTask* CreateNextSubTask_Reversed(CPed* ped);
    CTask* CreateFirstSubTask_Reversed(CPed* ped);
    CTask* ControlSubTask_Reversed(CPed* ped);
};

VALIDATE_SIZE(CTaskComplexUseSwatRope, 0x1C);
