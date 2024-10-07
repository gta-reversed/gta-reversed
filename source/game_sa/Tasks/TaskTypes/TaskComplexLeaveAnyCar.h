#pragma once

#include "TaskComplex.h"

class NOTSA_EXPORT_VTABLE CTaskComplexLeaveAnyCar : public CTaskComplex {
public:
    int32 m_nDelayTime;
    bool  m_sensibleLeaveCar;
    bool  m_forceGetOut;

public:
    constexpr static auto Type = TASK_COMPLEX_LEAVE_ANY_CAR;

    CTaskComplexLeaveAnyCar(int32 delayTime, bool sensibleLeaveCar, bool forceGetOut);
    ~CTaskComplexLeaveAnyCar() = default;

    CTask* Clone() const override { return new CTaskComplexLeaveAnyCar(m_nDelayTime, m_sensibleLeaveCar, m_forceGetOut); }
    eTaskType GetTaskType() const override { return Type; }
    CTask* CreateNextSubTask(CPed* ped) override { return nullptr; }
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override { return m_pSubTask; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexLeaveAnyCar* Constructor(int32 delayTime, bool sensibleLeaveCar, bool forceGetOut) { this->CTaskComplexLeaveAnyCar::CTaskComplexLeaveAnyCar(delayTime, sensibleLeaveCar, forceGetOut); return this; }
    CTaskComplexLeaveAnyCar* Destructor() { this->CTaskComplexLeaveAnyCar::~CTaskComplexLeaveAnyCar(); return this; }
};
VALIDATE_SIZE(CTaskComplexLeaveAnyCar, 0x14);
