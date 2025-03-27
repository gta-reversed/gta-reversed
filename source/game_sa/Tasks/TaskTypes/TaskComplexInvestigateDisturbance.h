#pragma once

#include "Vector.h"
#include "TaskComplex.h"

class CTaskComplexInvestigateDisturbance;
class CEvent;
class CPed;
class CEntity;

class NOTSA_EXPORT_VTABLE CTaskComplexInvestigateDisturbance : public CTaskComplex {
public:
    CVector  m_pos{};
    CEntity* m_entity{};

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_INVESTIGATE_DISTURBANCE;

    CTaskComplexInvestigateDisturbance(CVector const& targetPos, CEntity* srcEntity);
    CTaskComplexInvestigateDisturbance(const CTaskComplexInvestigateDisturbance&);
    ~CTaskComplexInvestigateDisturbance();

    CTask*    Clone() const override { return new CTaskComplexInvestigateDisturbance{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return !m_pSubTask || m_pSubTask->MakeAbortable(ped, priority, event); }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }

private: // Wrappers for hooks
    // 0x66C720
    CTaskComplexInvestigateDisturbance* Constructor(CVector const& targetPos, CEntity* srcEntity) {
        this->CTaskComplexInvestigateDisturbance::CTaskComplexInvestigateDisturbance(targetPos, srcEntity);
        return this;
    }

    // 0x66C7A0
    CTaskComplexInvestigateDisturbance* Destructor() {
        this->CTaskComplexInvestigateDisturbance::~CTaskComplexInvestigateDisturbance();
        return this;
    }
};
