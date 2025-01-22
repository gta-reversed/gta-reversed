#include "StdInc.h"

#include "TaskSimpleAchieveHeading.h"
#include "IKChainManager_c.h"

void CTaskSimpleAchieveHeading::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleAchieveHeading, 0x86fd74, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(SetUpIK, 0x667F20);
    RH_ScopedVMTInstall(Clone, 0x66CCF0);
    RH_ScopedVMTInstall(GetTaskType, 0x667E60);
    RH_ScopedVMTInstall(MakeAbortable, 0x667EB0);
    RH_ScopedVMTInstall(ProcessPed, 0x668060);
}

// 0x667E20
CTaskSimpleAchieveHeading::CTaskSimpleAchieveHeading(float desiredHeading, float changeRateMult, float tolerance) :
    m_DesiredHeadingRad{ desiredHeading },
    m_HeadingChangeRate{ changeRateMult },
    m_HeadingToleranceRad{ tolerance }
{
}

//! @notsa
CTaskSimpleAchieveHeading::CTaskSimpleAchieveHeading(CPed* looker, CEntity* lookTowards, float changeRateMult, float tolerance) :
    CTaskSimpleAchieveHeading{
        (lookTowards->GetPosition2D() - looker->GetPosition2D()).Heading(),
        changeRateMult,
        tolerance
    }
{
}

// 0x668060
bool CTaskSimpleAchieveHeading::ProcessPed(CPed* ped) {
    if (ped->bInVehicle) {
        return true;
    }

    ped->RestoreHeadingRate();
    ped->m_fHeadingChangeRate *= m_HeadingChangeRate;
    ped->SetMoveState(PEDMOVE_STILL);
    ped->SetMoveAnim();
    ped->m_fAimingRotation = m_DesiredHeadingRad;

    if (std::fabs(CGeneral::LimitRadianAngle(std::fabs(m_DesiredHeadingRad - ped->m_fCurrentRotation))) < m_HeadingToleranceRad) {
        ped->m_fAimingRotation = ped->m_fCurrentRotation;
        ped->RestoreHeadingRate();
        QuitIK(ped);
        return true;
    }

    SetUpIK(ped);
    return false;
}

// 0x667EB0
bool CTaskSimpleAchieveHeading::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    QuitIK(ped);

    switch (priority) {
    case ABORT_PRIORITY_IMMEDIATE:
        ped->m_fAimingRotation = ped->m_fCurrentRotation;
        ped->RestoreHeadingRate();
        return true;
    default:
        m_HeadingToleranceRad  = TWO_PI;
        ped->m_fAimingRotation = ped->m_fCurrentRotation;
        return false;
    }
}

// 0x667E80
void CTaskSimpleAchieveHeading::QuitIK(CPed* ped) const {
    if (m_IsDoingIK && g_ikChainMan.IsLooking(ped)) {
        g_ikChainMan.AbortLookAt(ped, 250);
    }
}

// 0x667F20
void CTaskSimpleAchieveHeading::SetUpIK(CPed* ped) {
    if (ped == FindPlayerPed() && !CPad::GetPad()->DisablePlayerControls) {
        return;
    }

    if (!ped->GetIsOnScreen()) {
        return;
    }

    if (m_IsDoingIK || g_ikChainMan.GetLookAtEntity(ped) || ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_IK)) {
        return;
    }

    if (m_Parent && notsa::contains({ TASK_COMPLEX_AVOID_ENTITY, TASK_COMPLEX_AVOID_OTHER_PED_WHILE_WANDERING }, m_Parent->GetTaskType())) { // That's kinda hacky :D
        return;
    }

    CVector lookAt = ped->GetPosition() + CVector{ 2.f * std::sin(m_DesiredHeadingRad), 2.f * std::cos(m_DesiredHeadingRad), 0.61f };
    g_ikChainMan.LookAt("TaskAchvHeading", ped, nullptr, 5'000, BONE_UNKNOWN, &lookAt, false, 0.25f, 500, 3, false);
    m_IsDoingIK = true;
}

// @addr unk
void CTaskSimpleAchieveHeading::SetHeading(float heading, float maxHeading, float changeRateMult) {
    m_DesiredHeadingRad   = heading;
    m_HeadingChangeRate   = changeRateMult;
    m_HeadingToleranceRad = maxHeading;
}
