#pragma once

#include "Enums/eMoveState.h"
#include "Vehicle.h"
#include "Ped.h"

class CTaskComplexEnterCarAsPassengerTimed;


class NOTSA_EXPORT_VTABLE CTaskComplexEnterCarAsPassengerWait : public CTaskComplex {
    static constexpr uint32 MAX_ENTER_CAR_FAILS = 16; //!< Give up after failing to get in this many times
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_WAIT;

    CTaskComplexEnterCarAsPassengerWait(CVehicle* target, CPed* waitFor, bool forceFrontSeat, eMoveState ms);
    CTaskComplexEnterCarAsPassengerWait(const CTaskComplexEnterCarAsPassengerWait&);
    ~CTaskComplexEnterCarAsPassengerWait() override = default; // 0x63B3C0


    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

    CTask*    Clone() const override { return new CTaskComplexEnterCarAsPassengerWait{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    auto GetCar() const { return m_Car; }

public:
    static void InjectHooks();

private:
    CVehicle::Ref  m_Car{};
    CPed::Ref      m_WaitForPed{};
    uint32     m_EnterCarFails{};
    bool       m_bForceFrontSeat{};
    eMoveState m_MoveState{};
    bool       m_bPlayedSample{};
};
