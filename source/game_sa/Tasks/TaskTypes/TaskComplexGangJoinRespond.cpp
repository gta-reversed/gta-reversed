#include "StdInc.h"

#include "TaskComplexGangJoinRespond.h"
#include "TaskComplexGangLeader.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleRunAnim.h"


// 0x6616F0
CTaskComplexGangJoinRespond::CTaskComplexGangJoinRespond(bool response) :
    m_response{ response } {
}

// 0x661720
CTaskComplexGangJoinRespond::~CTaskComplexGangJoinRespond() {
    if (std::exchange(m_animsReferenced, false)) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
    }
}

// 0x6617A0
CTask* CTaskComplexGangJoinRespond::CreateNextSubTask(CPed* ped) {
    if (notsa::isa<CTaskSimpleRunAnim>(m_pSubTask)) {
        return nullptr; // Responded already, we're done
    }

    // Wait for the anims to stream in (but don't wait forever)
    if (!m_animsReferenced) {
        return ++m_attempts <= MAX_ATTEMPTS
            ? new CTaskSimpleStandStill{ 250 }
            : nullptr;
    }

    return new CTaskSimpleRunAnim{
        ANIM_GROUP_GANGS,
        m_response
            ? ANIM_ID_INVITE_YES
            : ANIM_ID_INVITE_NO,
        4.f,
        false
    };
}

// 0x6618D0
CTask* CTaskComplexGangJoinRespond::CreateFirstSubTask(CPed* ped) {
    m_attempts = 0;
    return new CTaskComplexTurnToFaceEntityOrCoord{ FindPlayerPed(0) };
}

// 0x661950
CTask* CTaskComplexGangJoinRespond::ControlSubTask(CPed* ped) {
    ped->m_fAimingRotation = (FindPlayerPed()->GetPosition2D() - ped->GetPosition2D()).Heading(); // NOTE: Removed `CGeneral::LimitRadianAngle`
    CAnimManager::StreamAnimBlock("gangs", CTaskComplexGangLeader::ShouldLoadGangAnims(), m_animsReferenced);
    return m_pSubTask;
}

void CTaskComplexGangJoinRespond::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGangJoinRespond, 0x86FB5C, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x6616F0, "0", {}, bool);
    RH_ScopedDestructorInstall(0x661720);

    RH_ScopedVMTInstall(Clone, 0x662290);
    RH_ScopedVMTInstall(MakeAbortable, 0x661790);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6617A0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6618D0);
    RH_ScopedVMTInstall(ControlSubTask, 0x661950);
}
