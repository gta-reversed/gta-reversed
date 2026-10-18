#pragma once

#include "TaskComplex.h"
#include <Enums/eTargetDoor.h>

class CVehicle;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskComplexCarSlowBeDraggedOutAndStandUp : public CTaskComplex {
public:
    CVehicle*   m_Vehicle;
    eTargetDoor m_Door;

public:
    static constexpr auto Type = TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT_AND_STAND_UP;

    CTaskComplexCarSlowBeDraggedOutAndStandUp(CVehicle* vehicle, int32 door);
    ~CTaskComplexCarSlowBeDraggedOutAndStandUp() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskComplexCarSlowBeDraggedOutAndStandUp(m_Vehicle, m_Door); } // 0x64A190
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CTaskComplexCarSlowBeDraggedOutAndStandUp* Constructor(CVehicle* veh, int32 door) { this->CTaskComplexCarSlowBeDraggedOutAndStandUp::CTaskComplexCarSlowBeDraggedOutAndStandUp(veh, door); return this; }
    CTaskComplexCarSlowBeDraggedOutAndStandUp* Destructor() { this->CTaskComplexCarSlowBeDraggedOutAndStandUp::~CTaskComplexCarSlowBeDraggedOutAndStandUp(); return this; }
};
