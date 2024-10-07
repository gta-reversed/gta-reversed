#pragma once

#include "TaskComplexEnterCar.h"
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexDragPedFromCar : public CTaskComplexEnterCar {
public:
    CPed* m_Ped;

public:
    static constexpr auto Type = TASK_COMPLEX_DRAG_PED_FROM_CAR;

    CTaskComplexDragPedFromCar(CPed* ped, int32 draggedPedDownTime = 0);
    ~CTaskComplexDragPedFromCar() override;

    eTaskType GetTaskType() const override { return Type; } // 0x6404C0
    CTask* Clone() const override { return new CTaskComplexDragPedFromCar(m_Ped, m_DraggedPedDownTime); } // 0x643950

    CTask* ControlSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;

};
VALIDATE_SIZE(CTaskComplexDragPedFromCar, 0x54);
extern void CTaskComplexDragPedFromCar__InjectHooks();
