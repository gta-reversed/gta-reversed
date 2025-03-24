#pragma once

#include "TaskComplex.h"

class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskComplexArrestPed : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_ARREST_PED;

    explicit CTaskComplexArrestPed(CPed* ped);

    CTask* Clone() const override { return new CTaskComplexArrestPed(m_PedToArrest); }
    eTaskType GetTaskType() const override { return Type; }
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;
    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexArrestPed* Constructor(CPed* ped) { this->CTaskComplexArrestPed::CTaskComplexArrestPed(ped); return this; }
    CTaskComplexArrestPed* Destructor() { this->CTaskComplexArrestPed::~CTaskComplexArrestPed(); return this; }

protected:
    bool      m_HasNewPedToArrest{}; //!< If the target ped has changed
    CPed::Ref m_PedToArrest{};       //!< The ped we want to arrest
    float     m_DistanceToTarget{};  //!< Unused
    float     m_AttackRange{};       //!< Unused
    float     m_ArriveRange{};       //!< Unused
    CVehicle* m_Vehicle{};           //!< Vehicle to destroy
};
VALIDATE_SIZE(CTaskComplexArrestPed, 0x24);
