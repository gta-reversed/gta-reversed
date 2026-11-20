#include "StdInc.h"

#include "TaskComplexGotoDoorAndOpen.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskSimpleRunAnim.h"
#include "Events/EventAreaCodes.h"

void CTaskComplexGotoDoorAndOpen::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGotoDoorAndOpen, 0x86FFC0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedOverloadedInstall(Constructor, "0", 0x66BB20, CTaskComplexGotoDoorAndOpen*(CTaskComplexGotoDoorAndOpen::*)(CObject *));
    RH_ScopedOverloadedInstall(Constructor, "1", 0x66BBA0, CTaskComplexGotoDoorAndOpen*(CTaskComplexGotoDoorAndOpen::*)(CVector const&, CVector const&));
    RH_ScopedInstall(Destructor, 0x66BC00);
    RH_ScopedVMTInstall(Clone, 0x66BCA0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x66C0D0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x66BD40);
    RH_ScopedVMTInstall(ControlSubTask, 0x66C1F0);
}

// 0x66BB20
CTaskComplexGotoDoorAndOpen::CTaskComplexGotoDoorAndOpen(CObject* door) : CTaskComplex() {
    m_Object                      = door;
    m_Timer                       = {};
    m_bHasDoorObject              = true;
    m_bGoingThroughDoor           = false;
    m_bDoorWasCollidable          = false;
    m_bPlayerOnInteriorTransition = false;
    CEntity::SafeRegisterRef(m_Object);
}

// 0x66BBA0
CTaskComplexGotoDoorAndOpen::CTaskComplexGotoDoorAndOpen(const CVector& start, const CVector& end) : CTaskComplex() {
    m_Object = nullptr;
    m_Start = start;
    m_End = end;
    m_Timer                       = {};
    m_bHasDoorObject              = false;
    m_bGoingThroughDoor           = false;
    m_bPlayerOnInteriorTransition = false; // NOTE: `m_bDoorWasCollidable` is left uninitialized originally
}

// 0x66BC00
CTaskComplexGotoDoorAndOpen::~CTaskComplexGotoDoorAndOpen() {
    CEntity::SafeCleanUpRef(m_Object);
    if (m_bPlayerOnInteriorTransition) {
        CPad::GetPad()->bPlayerOnInteriorTransition = false;
    }
}

// 0x66BCA0
CTask* CTaskComplexGotoDoorAndOpen::Clone() const {
    if (m_bHasDoorObject) {
        return new CTaskComplexGotoDoorAndOpen(m_Object);
    } else {
        return new CTaskComplexGotoDoorAndOpen(m_Start, m_End);
    }
}

// 0x66BC80
bool CTaskComplexGotoDoorAndOpen::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return priority == ABORT_PRIORITY_IMMEDIATE && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);
}

// 0x66C0D0
CTask* CTaskComplexGotoDoorAndOpen::CreateNextSubTask(CPed* ped) {
    if (m_pSubTask->GetTaskType() != TASK_SIMPLE_GO_TO_POINT) {
        return nullptr;
    }

    // Went through the door, we're done
    if (m_bGoingThroughDoor) {
        if (ped->IsPlayer()) {
            CPad::GetPad()->bPlayerOnInteriorTransition = true;
        }
        return nullptr;
    }

    // At the door, now open it and go through
    auto& tm = ped->GetTaskManager();
    if (!tm.GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM)) {
        tm.SetTaskSecondary(new CTaskSimpleRunAnim{ ANIM_GROUP_DEFAULT, ANIM_ID_WALK_DOORPARTIAL, 8.f, false }, TASK_SECONDARY_PARTIAL_ANIM);
    }
    const auto task     = new CTaskSimpleGoToPoint{ PEDMOVE_WALK, m_End, 0.2f, false, false };
    m_bGoingThroughDoor = true;
    return task;
}

// 0x66BD40
CTask* CTaskComplexGotoDoorAndOpen::CreateFirstSubTask(CPed* ped) {
    if (ped->IsPlayer()) {
        CPad::GetPad()->bPlayerOnInteriorTransition = true;
        m_bPlayerOnInteriorTransition               = true;
    }
    GetEventGlobalGroup()->Add(CEventAreaCodes{ ped });
    m_Timer.Start(1'000);

    // 0x66BDAF - No door, just walk to the end position (Unless the door was deleted)
    if (!m_Object) {
        if (m_bHasDoorObject) {
            return nullptr;
        }
        m_bGoingThroughDoor = true;
        return new CTaskSimpleGoToPoint{ PEDMOVE_WALK, m_End, 0.5f, false, false };
    }

    // 0x66BDDA - Calculate the positions in front of and behind the door (on the side the ped is at)
    const auto& doorMat   = m_Object->GetMatrix();
    const auto& doorPos   = m_Object->GetPosition();
    const auto& doorFwd   = doorMat.GetForward();
    const auto  center    = doorPos + doorMat.GetRight() * 0.75f;
    const auto  isInFront = (doorPos - ped->GetPosition()).Dot(doorFwd) <= 0.f;
    const auto  dir       = isInFront ? -doorFwd : doorFwd; // Direction we go through the door

    m_Start               = center - dir * 0.5f;
    m_End                 = center + dir * 2.f;

    // 0x66BEC4 - Make sure the door doesn't block us
    if (m_Object->physicalFlags.bCollidable) {
        m_bDoorWasCollidable                           = true;
        m_Object->physicalFlags.bCollidable            = false;
        m_Object->physicalFlags.bDisableCollisionForce = false;
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
    if (!m_Timer.IsOutOfTime() && (m_Object || !m_bHasDoorObject)) {
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
