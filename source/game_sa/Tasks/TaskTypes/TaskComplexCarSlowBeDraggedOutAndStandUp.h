#pragma once

#include "TaskComplex.h"

class CVehicle;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskComplexCarSlowBeDraggedOutAndStandUp : public CTaskComplex {
public:
    CVehicle* m_Vehicle;
    int32     dword10;

public:
    static constexpr auto Type = TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT_AND_STAND_UP;

    CTaskComplexCarSlowBeDraggedOutAndStandUp(CVehicle* vehicle, int32 a3);
    ~CTaskComplexCarSlowBeDraggedOutAndStandUp() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskComplexCarSlowBeDraggedOutAndStandUp(m_Vehicle, dword10); } // 0x64A190;
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CTaskComplexCarSlowBeDraggedOutAndStandUp* Constructor(CVehicle* veh, int32 a3) { this->CTaskComplexCarSlowBeDraggedOutAndStandUp::CTaskComplexCarSlowBeDraggedOutAndStandUp(veh, a3); return this; }
    CTaskComplexCarSlowBeDraggedOutAndStandUp* Destructor() { this->CTaskComplexCarSlowBeDraggedOutAndStandUp::~CTaskComplexCarSlowBeDraggedOutAndStandUp(); return this; }
};
