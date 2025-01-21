#pragma once

#include "TaskComplex.h"
#include "Vehicle.h"
#include <Enums/eTargetDoor.h>

class CVehicle;
class CTaskSimpleCarFallOut;
class CTaskUtilityLineUpPedWithCar;

class NOTSA_EXPORT_VTABLE CTaskComplexCarSlowBeDraggedOut : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT;

    CTaskComplexCarSlowBeDraggedOut(CVehicle* vehicle, eTargetDoor door, bool a4);
    ~CTaskComplexCarSlowBeDraggedOut() override = default;

    CTask*    Clone() const override { return new CTaskComplexCarSlowBeDraggedOut(m_Vehicle, m_Door, m_IsDraggedBySelf); } // 0x64A120
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }

private:
    CTask* CreateSubTask(eTaskType taskType);
    CTaskUtilityLineUpPedWithCar* CreateTaskUtilityLineUpPedWithCar(CPed* ped);

    void PrepareVehicleForPedExit(CPed* ped);
    void PreparePedForVehicleExit(CPed* ped);

private:
    CVehicle::Ref                                 m_Vehicle{};
    eTargetDoor                                   m_Door{};
    bool                                          m_IsDraggedBySelf{};
    std::unique_ptr<CTaskUtilityLineUpPedWithCar> m_LineUpUtility{};

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CTaskComplexCarSlowBeDraggedOut* Constructor(CVehicle* car, eTargetDoor a3, bool a4) { this->CTaskComplexCarSlowBeDraggedOut::CTaskComplexCarSlowBeDraggedOut(car, a3, a4); return this; }
    CTaskComplexCarSlowBeDraggedOut* Destructor() { this->CTaskComplexCarSlowBeDraggedOut::~CTaskComplexCarSlowBeDraggedOut(); return this; }
};
VALIDATE_SIZE(CTaskComplexCarSlowBeDraggedOut, 0x1C);
