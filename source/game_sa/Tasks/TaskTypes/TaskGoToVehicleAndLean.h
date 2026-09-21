#pragma once

#include "TaskComplex.h"
class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskGoToVehicleAndLean : public CTaskComplex {
public:
    CVehicle* m_Vehicle;
    int32     m_LeanAnimDurationInMs;
    bool      m_LeanOnVehicle;
    bool      m_bCalcTargetPosSide; // field_15: which side of the vehicle to lean on
    uint8     field_16[2];
    CVector   m_TargetPos; // 0x18

public:
    static constexpr auto Type = TASK_COMPLEX_GOTO_VEHICLE_AND_LEAN;

    CTaskGoToVehicleAndLean(CVehicle* vehicle, int32 leanAnimDurationInMs);
    ~CTaskGoToVehicleAndLean() override;

    eTaskType GetTaskType() const override { return Type; } // 0x660ED0
    CTask* Clone() const override;
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

    void DoTidyUp(CPed* ped);
    CVector CalcTargetPos(CPed* ped); // 0x664770

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CTaskGoToVehicleAndLean* Constructor(CVehicle* vehicle, int32 leanAnimDurationInMs) { this->CTaskGoToVehicleAndLean::CTaskGoToVehicleAndLean(vehicle, leanAnimDurationInMs); return this; }
    CTaskGoToVehicleAndLean* Destructor() { this->CTaskGoToVehicleAndLean::~CTaskGoToVehicleAndLean(); return this; }
};
VALIDATE_SIZE(CTaskGoToVehicleAndLean, 0x24);
