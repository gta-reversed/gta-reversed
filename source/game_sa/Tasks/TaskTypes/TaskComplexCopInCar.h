#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"

class CPed;
class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskComplexCopInCar : public CTaskComplex {
public:
    CVehicle*  m_Vehicle;
    CPed*      m_Cop;     // Same as `ped` that's passed to all functions (So basically the ped this task belongs to)
    CPed*      m_Suspect;

    CTaskTimer m_Timer1;
    CTaskTimer m_Timer2;

    uint8 m_bSuspectDeadOrLost : 1; // 0st bit - Seems to be related to cop1's existence, something like `isCop1Alive`
    uint8 m_flag0x2 : 1;            // 1nd bit
    uint8 m_flag0x4 : 1;            // 2rd bit
    uint8 m_bIsSuspectInCar : 1;    // 3th bit

public:
    static constexpr auto Type = TASK_COMPLEX_COP_IN_CAR;

    CTaskComplexCopInCar(CVehicle* vehicle, CPed* cop1, CPed* cop2, bool bSuspectDeadOrLost);
    ~CTaskComplexCopInCar() override;

    CTask* CreateSubTask(eTaskType taskType, CPed* copPed);



CTask* Clone() const override { return new CTaskComplexCopInCar{ *this }; }



eTaskType GetTaskType() const override { return Type; }

    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexCopInCar* Constructor(CVehicle* vehicle, CPed* cop1, CPed* cop2, bool flag_0x1) { this->CTaskComplexCopInCar::CTaskComplexCopInCar(vehicle, cop1, cop2, flag_0x1); return this; }
    CTaskComplexCopInCar* Destructor() { this->CTaskComplexCopInCar::~CTaskComplexCopInCar(); return this; }

                        };
VALIDATE_SIZE(CTaskComplexCopInCar, 0x34);
