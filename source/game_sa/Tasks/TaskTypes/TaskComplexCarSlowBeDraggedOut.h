#pragma once

#include "TaskComplex.h"
#include "TaskUtilityLineUpPedWithCar.h"

class CVehicle;
class CTaskSimpleCarFallOut;

class NOTSA_EXPORT_VTABLE CTaskComplexCarSlowBeDraggedOut : public CTaskComplex {
public:
    CVehicle* m_Vehicle;
    int32     m_Door;
    bool      field_14;
    CTaskUtilityLineUpPedWithCar* m_TaskUtilityLineUpPedWithCar;

public:
    static constexpr auto Type = TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT;

    CTaskComplexCarSlowBeDraggedOut(CVehicle* vehicle, int32 door, bool a4);
    ~CTaskComplexCarSlowBeDraggedOut() override;

    CTask*    Clone() const override { return new CTaskComplexCarSlowBeDraggedOut(m_Vehicle, m_Door, field_14); } // 0x64A120
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    CTask* CreateSubTask(eTaskType taskType);
    CTaskUtilityLineUpPedWithCar* CreateTaskUtilityLineUpPedWithCar(int32 a2);

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CTaskComplexCarSlowBeDraggedOut* Constructor(CVehicle* car, int32 a3, bool a4) { this->CTaskComplexCarSlowBeDraggedOut::CTaskComplexCarSlowBeDraggedOut(car, a3, a4); return this; }
    CTaskComplexCarSlowBeDraggedOut* Destructor() { this->CTaskComplexCarSlowBeDraggedOut::~CTaskComplexCarSlowBeDraggedOut(); return this; }
};
VALIDATE_SIZE(CTaskComplexCarSlowBeDraggedOut, 0x1C);
