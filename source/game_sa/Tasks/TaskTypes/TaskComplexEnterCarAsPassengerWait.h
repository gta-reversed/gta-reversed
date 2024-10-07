#pragma once

#include "Enums/eMoveState.h"

class CPed;
class CTaskComplexEnterCarAsPassengerTimed;
class CTaskComplexEnterCarAsPassengerWait;
class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskComplexEnterCarAsPassengerWait : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_WAIT;

    static void InjectHooks();

    CTaskComplexEnterCarAsPassengerWait(CVehicle* target, CPed* waitFor, bool forceFrontSeat, eMoveState ms);
    CTaskComplexEnterCarAsPassengerWait(const CTaskComplexEnterCarAsPassengerWait&);
    ~CTaskComplexEnterCarAsPassengerWait();

    CTaskComplexEnterCarAsPassengerTimed* CreateSubTask(int32 taskType, CPed* ped);

    CTask*    Clone() const override { return new CTaskComplexEnterCarAsPassengerWait{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    auto GetCar() const { return m_Car; }

private: // Wrappers for hooks
    // 0x63B320
    CTaskComplexEnterCarAsPassengerWait* Constructor(CVehicle* target, CPed* waitFor, bool forceFrontSeat, eMoveState ms) {
        this->CTaskComplexEnterCarAsPassengerWait::CTaskComplexEnterCarAsPassengerWait(target, waitFor, forceFrontSeat, ms);
        return this;
    }
    // 0x63B3C0
    CTaskComplexEnterCarAsPassengerWait* Destructor() {
        this->CTaskComplexEnterCarAsPassengerWait::~CTaskComplexEnterCarAsPassengerWait();
        return this;
    }
private:
    CVehicle*  m_Car{};
    CPed*      m_WaitForPed{};
    uint32     m_EnterCarFails{};
    bool       m_bForceFrontSeat{};
    eMoveState m_MoveState{};
    bool       m_bPlayedSample{};
};
