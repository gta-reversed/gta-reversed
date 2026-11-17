#pragma once

#include "TaskSimple.h"

class CAnimBlendAssociation;
class CEvent;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleBeKickedOnGround : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_BE_KICKED_ON_GROUND;

    static void InjectHooks();

    CTaskSimpleBeKickedOnGround() = default;
    CTaskSimpleBeKickedOnGround(const CTaskSimpleBeKickedOnGround&) : CTaskSimpleBeKickedOnGround{} {}
    ~CTaskSimpleBeKickedOnGround() = default;

    static CTaskSimpleBeKickedOnGround* FinishAnimBeKickedOnGroundCB(CAnimBlendAssociation* anim, void* data);

    void StartAnim(CPed* ped);

    CTask*    Clone() const override  { return new CTaskSimpleBeKickedOnGround{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;

private:
public:
    bool                   m_IsFinished{};
    CAnimBlendAssociation* m_Anim{};
};
