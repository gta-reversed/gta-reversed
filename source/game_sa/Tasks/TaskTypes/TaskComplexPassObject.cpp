#include "StdInc.h"
#include "TaskComplexPassObject.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "IKChainManager_c.h"

void CTaskComplexPassObject::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexPassObject, 0x86fb18, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6611B0);
    RH_ScopedInstall(Destructor, 0x661230);

    RH_ScopedInstall(AbortIK, 0x661290);

    RH_ScopedVMTInstall(Clone, 0x662220);
    RH_ScopedVMTInstall(GetTaskType, 0x661220);
    RH_ScopedVMTInstall(MakeAbortable, 0x664A50);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6612C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6613A0);
    RH_ScopedVMTInstall(ControlSubTask, 0x661570);
}

// 0x6611B0
CTaskComplexPassObject::CTaskComplexPassObject(CPed* ped, bool isPasser) :
    m_ped{ped},
    m_isPasser{isPasser}
{
    CEntity::SafeRegisterRef(m_ped);
}

CTaskComplexPassObject::CTaskComplexPassObject(const CTaskComplexPassObject& o) :
    CTaskComplexPassObject{ o.m_ped, o.m_isPasser }
{
}

// 0x661230
CTaskComplexPassObject::~CTaskComplexPassObject() {
    CEntity::SafeCleanUpRef(m_ped);
#ifdef FIX_BUGS
    CEntity::SafeCleanUpRef(m_entityToPass);
#endif
}

// 0x661290
void CTaskComplexPassObject::AbortIK(CPed* ped) {
    if (g_ikChainMan.IsArmPointing(eIKArm::IK_ARM_RIGHT, ped)) {
        g_ikChainMan.AbortPointArm(eIKArm::IK_ARM_RIGHT, ped, 500);
    }
}

// 0x664A50
bool CTaskComplexPassObject::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    AbortIK(ped);
    return true;
}

// 0x6612C0
CTask* CTaskComplexPassObject::CreateNextSubTask(CPed* ped) {
    if (m_ped && CTask::IsA<CTaskComplexTurnToFaceEntityOrCoord>(m_pSubTask)) {
        return new CTaskSimpleStandStill{ 999'999 };
    }
    AbortIK(ped);
    return nullptr;
}

// 0x6613A0
CTask* CTaskComplexPassObject::CreateFirstSubTask(CPed* ped) {
    if (m_ped && !m_ped->IsPlayer() && !ped->IsPlayer()) {
        // Find entity to pass
        {
            const auto passer = m_isPasser ? ped : m_ped;
            m_entityToPass = passer->GetEntityThatThisPedIsHolding();
            assert(m_entityToPass);
            CEntity::RegisterReference(m_entityToPass);
        }
        if (const auto pedToOther = (m_ped->GetPosition() - ped->GetPosition()); sq(1.5f) >= pedToOther.SquaredMagnitude()) {
            m_targetPos = m_ped->GetPosition() + pedToOther / 2.f; // Position intbetween the 2 peds
            return new CTaskComplexTurnToFaceEntityOrCoord{ m_targetPos }; 
        }
    }
    AbortIK(ped);
    return nullptr;
}

// 0x661570
CTask* CTaskComplexPassObject::ControlSubTask(CPed* ped) {
    if (m_ped) {
        if (!m_timer.IsOutOfTime()) {
            return m_pSubTask;
        }

        if (!m_isPasser && m_entityToPass) {
            if (const auto holdTask = m_ped->GetTaskManager().Find<CTaskSimpleHoldEntity>()) {
                holdTask->DropEntity(m_ped, false);
                ped->GetTaskManager().SetTaskSecondary(
                    new CTaskSimpleHoldEntity{ m_entityToPass, CVector{}, (uint8)BONE_L_BROW },
                    TASK_SECONDARY_PARTIAL_ANIM
                );
            }
        }
    }

    AbortIK(ped);
    return nullptr;
}
