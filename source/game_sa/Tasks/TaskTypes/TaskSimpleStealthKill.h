/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"

class CPed;
class CEvent;
class CAnimBlendAssociation;

/*!
* Kill a ped with a knife
*/
class NOTSA_EXPORT_VTABLE CTaskSimpleStealthKill : public CTaskSimple {
public:
    bool                   m_bKeepTargetAlive{};    //< If the target ped should stay alive (Also plays a different anim in this case)
    CPed*                  m_target{};              //< Ped to kill
    AssocGroupId           m_animGrpId{};           //< Anim's group
    bool                   m_bIsAborting{};         //< If we're aborting (Cancels the animation)
    bool                   m_bIsFinished{};         //< Is the animation finished (And thus the task)
    CAnimBlendAssociation* m_anim{};                //< The kill animation
    uint32                 m_spentWaitingMs{};      //< Time we've spent waiting for the animation to be loaded (Task finished if not loaded after 10'000ms)

public:
    static void InjectHooks();

    static constexpr auto Type = TASK_SIMPLE_STEALTH_KILL;

    CTaskSimpleStealthKill(bool keepTargetAlive, CPed* target, AssocGroupId groupId);
    CTaskSimpleStealthKill(const CTaskSimpleStealthKill&);

    static void FinishAnimStealthKillCB(CAnimBlendAssociation* pAnimAssoc, void* vpTaskSimpleStealthKill);

    void ManageAnim(CPed* ped);

    CTask*    Clone() const override { return new CTaskSimpleStealthKill{ *this }; }
    eTaskType GetTaskType() const override { return TASK_SIMPLE_STEALTH_KILL; } // 0x622670
    bool      ProcessPed(CPed* ped) override;
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;

private:
    auto Constructor(bool keepTargetAlive, CPed* target, AssocGroupId groupId) {
        this->CTaskSimpleStealthKill::CTaskSimpleStealthKill(keepTargetAlive, target, groupId);
        return this;
    }
};
VALIDATE_SIZE(CTaskSimpleStealthKill, 0x20);
