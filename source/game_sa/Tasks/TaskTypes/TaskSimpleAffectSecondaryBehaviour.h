#pragma once

#include "TaskSimple.h"

class CTask;
class CEvent;
class CPed;
class CTaskSimpleAffectSecondaryBehaviour;

/*!
 *  Keep trying until the current task (GetTaskSecondary(m_secTaskType)) is done/can be aborted
 *  using `ABORT_PRIORITY_URGENT` and then add the given task as a
 *  secondary task in it's place.
 */
class NOTSA_EXPORT_VTABLE CTaskSimpleAffectSecondaryBehaviour : public CTaskSimple {
public:
    bool           m_bAdd{};
    eSecondaryTask m_secTaskType{};
    CTask*         m_task{};

public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_AFFECT_SECONDARY_BEHAVIOUR;

    CTaskSimpleAffectSecondaryBehaviour(bool add, eSecondaryTask secondaryTaskType, CTask* task);
    CTaskSimpleAffectSecondaryBehaviour(const CTaskSimpleAffectSecondaryBehaviour&); // NOTSA

    CTask*    Clone() const override { return new CTaskSimpleAffectSecondaryBehaviour(*this); }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return false; }
    bool      ProcessPed(CPed* ped) override;

private: // Wrappers for hooks
    // 0x691270
    CTaskSimpleAffectSecondaryBehaviour* Constructor(bool add, eSecondaryTask secondaryTaskType, CTask* a4) {
        this->CTaskSimpleAffectSecondaryBehaviour::CTaskSimpleAffectSecondaryBehaviour(add, secondaryTaskType, a4);
        return this;
    }
};
