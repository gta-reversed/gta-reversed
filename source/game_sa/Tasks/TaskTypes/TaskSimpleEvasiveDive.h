#pragma once

#include "TaskSimple.h"
#include "Vehicle.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleEvasiveDive final : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_EVASIVE_DIVE;

    CTaskSimpleEvasiveDive(CVehicle *vehicle);
    ~CTaskSimpleEvasiveDive() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskSimpleEvasiveDive(m_EvadeVeh); }
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;

    void StartAnim(CPed* ped);
    static void FinishAnimEvasiveDiveCB(CAnimBlendAssociation *assoc, void* data);

private:
    CVehicle::Ref          m_EvadeVeh{};  //< The vehicle to evade
    bool                   m_bFinished{}; //< Has the task finished (Set when the anim has finished)
    CAnimBlendAssociation* m_DiveAnim{};  //> The dive animation
};
VALIDATE_SIZE(CTaskSimpleEvasiveDive, 0x14);
