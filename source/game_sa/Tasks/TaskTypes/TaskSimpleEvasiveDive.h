#pragma once

#include "TaskSimple.h"
#include "Vehicle.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleEvasiveDive NOTSA_CLASS_FINAL : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_EVASIVE_DIVE;

    CTaskSimpleEvasiveDive(CVehicle* vehicle);
    ~CTaskSimpleEvasiveDive() override;

    static void FinishAnimEvasiveDiveCB(CAnimBlendAssociation* assoc, void* data);

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskSimpleEvasiveDive(m_ToEvade); }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;

    void        StartAnim(CPed* ped);

public:
    static void InjectHooks();

private:
    CVehicle::Ref          m_ToEvade{};   //!< The vehicle to evade
    bool                   m_bFinished{}; //!< Has the task finished (Set when the anim has finished)
    CAnimBlendAssociation* m_DiveAnim{};  //!< The dive animation
};
VALIDATE_SIZE(CTaskSimpleEvasiveDive, 0x14);
