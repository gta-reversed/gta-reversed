#include "StdInc.h"

#include "TaskComplexGotoDoorAndOpen.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskSimpleRunAnim.h"
#include "Events/EventAreaCodes.h"

// 0x66BB20
CTaskComplexGotoDoorAndOpen::CTaskComplexGotoDoorAndOpen(CObject* door) :
    m_DoorToOpen{ door },
    m_HasDoorObject{ true } {
}

// 0x66BBA0
CTaskComplexGotoDoorAndOpen::CTaskComplexGotoDoorAndOpen(const CVector& start, const CVector& end) :
    m_Start{ start },
    m_End{ end } {
}

// 0x66BC00
CTaskComplexGotoDoorAndOpen::~CTaskComplexGotoDoorAndOpen() {
    if (m_IsPlayerOnInteriorTransition) {
        CPad::GetPad()->bPlayerOnInteriorTransition = false;
    }
}

// 0x66BCA0
CTask* CTaskComplexGotoDoorAndOpen::Clone() const {
    return m_HasDoorObject
        ? new CTaskComplexGotoDoorAndOpen(m_DoorToOpen)
        : new CTaskComplexGotoDoorAndOpen(m_Start, m_End);
}

// 0x66BC80
bool CTaskComplexGotoDoorAndOpen::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return priority == ABORT_PRIORITY_IMMEDIATE && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);
}

// 0x66C0D0
CTask* CTaskComplexGotoDoorAndOpen::CreateNextSubTask(CPed* ped) {
    if (notsa::isa<CTaskSimpleGoToPoint>(m_pSubTask)) {
        // Went through the door, we're done
        if (m_IsGoingThroughDoor) {
            if (ped->IsPlayer()) {
                CPad::GetPad()->bPlayerOnInteriorTransition = true;
            }
            return nullptr;
        }

        // At the door, now open it and go through
        auto& tm = ped->GetTaskManager();
        if (!tm.GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM)) {
            tm.SetTaskSecondary(
                new CTaskSimpleRunAnim{ ANIM_GROUP_DEFAULT, ANIM_ID_WALK_DOORPARTIAL, 8.f, false },
                TASK_SECONDARY_PARTIAL_ANIM
            );
        }
        m_IsGoingThroughDoor = true;
        return new CTaskSimpleGoToPoint{ PEDMOVE_WALK, m_End, 0.2f, false, false };
    }
    return nullptr;
}

// 0x66BD40
CTask* CTaskComplexGotoDoorAndOpen::CreateFirstSubTask(CPed* ped) {
    if (ped->IsPlayer()) {
        CPad::GetPad()->bPlayerOnInteriorTransition = true;
        m_IsPlayerOnInteriorTransition              = true;
    }
    GetEventGlobalGroup()->Add(CEventAreaCodes{ ped });

    m_DoorOpenTimer.Start(1'000);

    // 0x66BDAF - No door, just walk to the end position (Unless the door was deleted)
    if (!m_DoorToOpen) {
        if (m_HasDoorObject) {
            return nullptr;
        }
        m_IsGoingThroughDoor = true;
        return new CTaskSimpleGoToPoint{ PEDMOVE_WALK, m_End, 0.5f, false, false };
    }

    // 0x66BDDA - Calculate the positions in front of and behind the door (on the side the ped is at)
    const auto& pos    = m_DoorToOpen->GetPosition();
    const auto& fwd    = m_DoorToOpen->GetForward();
    const auto  center = pos + m_DoorToOpen->GetRight() * 0.75f;
    const auto  dir    = (pos - ped->GetPosition()).Dot(fwd) <= 0.f
        ? -fwd
        : fwd;
    m_Start            = center - dir * 0.5f;
    m_End              = center + dir * 2.f;

    // 0x66BEC4 - Make sure the door doesn't block us
    if (m_DoorToOpen->physicalFlags.bCollidable) {
        m_WasDoorCollidable                                = true;
        m_DoorToOpen->physicalFlags.bCollidable            = false;
        m_DoorToOpen->physicalFlags.bDisableCollisionForce = false;
    }

    // 0x66BEF0 - Can't sprint through the door
    if (RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_SPRINT)) {
        CAnimManager::BlendAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_WALK, 1'000.f);
    }

    return new CTaskSimpleGoToPoint{ PEDMOVE_WALK, m_Start, 0.35f, false, false };
}

// 0x66C1F0
CTask* CTaskComplexGotoDoorAndOpen::ControlSubTask(CPed* ped) {
    // Abort if it's taking too long, or the door was deleted
    if (!m_DoorOpenTimer.IsOutOfTime() && (m_DoorToOpen || !m_HasDoorObject)) {
        return m_pSubTask;
    }
    if (!m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
        return m_pSubTask;
    }
    if (ped->IsPlayer()) {
        CPad::GetPad()->bPlayerOnInteriorTransition = true;
    }
    return nullptr;
}

void CTaskComplexGotoDoorAndOpen::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGotoDoorAndOpen, 0x86FFC0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x66BB20, "1", {}, CObject*);
    RH_ScopedConstructorInstall(0x66BBA0, "2", {}, const CVector&, const CVector&);
    RH_ScopedDestructorInstall(0x66BC00);

    RH_ScopedVMTInstall(Clone, 0x66BCA0);
    RH_ScopedVMTInstall(GetTaskType, 0x66BB90);
    RH_ScopedVMTInstall(MakeAbortable, 0x66BC80);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x66C0D0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x66BD40);
    RH_ScopedVMTInstall(ControlSubTask, 0x66C1F0);
}
