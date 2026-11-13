#pragma once

#include "TaskComplex.h"
class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskGoToVehicleAndLean : public CTaskComplex {
public:
    CVehicle* m_Vehicle{};              // 0xC
    int32     m_LeanAnimDurationInMs{}; // 0x10
    bool      m_AnimsReferenced{};      // 0x14 - Is the "gangs" anim block referenced
    bool      m_LeanOnRightSide{};      // 0x15 - Set by `CalcTargetPos`
    CVector   m_TargetPos{};            // 0x18 - Position to lean on the vehicle at (Calculated by `CalcTargetPos`)

public:
    static constexpr auto Type = TASK_COMPLEX_GOTO_VEHICLE_AND_LEAN;

    CTaskGoToVehicleAndLean(CVehicle* vehicle, int32 leanAnimDurationInMs);
    ~CTaskGoToVehicleAndLean() override;

    eTaskType GetTaskType() const override { return Type; } // 0x660ED0
    CTask* Clone() const override { return new CTaskGoToVehicleAndLean(m_Vehicle, m_LeanAnimDurationInMs); } // 0x6621B0
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

    void DoTidyUp(CPed* ped);

    //! Calculate the position next to the vehicle (on the side the ped is at) to lean on it at
    CVector CalcTargetPos(const CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CTaskGoToVehicleAndLean* Constructor(CVehicle* vehicle, int32 leanAnimDurationInMs) { this->CTaskGoToVehicleAndLean::CTaskGoToVehicleAndLean(vehicle, leanAnimDurationInMs); return this; }
    CTaskGoToVehicleAndLean* Destructor() { this->CTaskGoToVehicleAndLean::~CTaskGoToVehicleAndLean(); return this; }
};

VALIDATE_SIZE(CTaskGoToVehicleAndLean, 0x24);
