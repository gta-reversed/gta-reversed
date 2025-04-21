/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskComplex.h"

class NOTSA_EXPORT_VTABLE CTaskComplexJump : public CTaskComplex {
public:
    enum class eForceClimb : int32 {
        DISABLE = -1,
        OK      = 0,
        FORCE   = 1
    };

public:
    static constexpr auto Type = TASK_COMPLEX_JUMP;

    explicit CTaskComplexJump(eForceClimb forceClimb = CTaskComplexJump::eForceClimb::OK);
    ~CTaskComplexJump() override = default;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override;
    CTask* ControlSubTask(CPed* ped) override { return m_pSubTask; }
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    bool   MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexJump* Constructor(eForceClimb jumpType);


    CTask* CreateSubTask(eTaskType taskType, CPed* ped);
 
public:
    eForceClimb m_ForceClimb;
    bool        m_UsePlayerLaunchForce;
};

VALIDATE_SIZE(CTaskComplexJump, 0x14);
