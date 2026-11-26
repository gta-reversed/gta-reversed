#include "StdInc.h"

#include "TaskSimpleSlideToCoord.h"
#include "TaskSimpleStandStill.h"
#include <reversiblebugfixes/Bugs.hpp>

// 0x66C3E0
CTaskSimpleSlideToCoord::CTaskSimpleSlideToCoord(const CVector& slideToPos, float aimingRotation, float speed) :
    CTaskSimpleRunNamedAnim(),
    m_SlideToPos{ slideToPos },
    m_AimingRotation{ aimingRotation },
    m_Speed{ speed },
    m_IsFirstTime{ true },
    m_IsRunningAnim{ false } {
    // m_EndTime not initialized
    if (notsa::bugfixes::CTaskSimpleSlideToCoord_Constructor_UninitializedTimer) {
        m_EndTime = -1;
    }
}

// 0x66C450
CTaskSimpleSlideToCoord::CTaskSimpleSlideToCoord(const CVector& slideToPos, float aimingRotation, float speed, const char* animName, const char* animGroupName, uint32 animFlags, float animBlendDelta, bool runInSequence, uint32 time) :
    CTaskSimpleRunNamedAnim{ animName, animGroupName, animFlags, animBlendDelta, time, false, runInSequence, false, false },
    m_SlideToPos{ slideToPos },
    m_AimingRotation{ aimingRotation },
    m_Speed{ speed },
    m_IsFirstTime{ true },
    m_IsRunningAnim{ true },
    m_EndTime{ -1 } {
}

// 0x66D300
CTask* CTaskSimpleSlideToCoord::Clone() const {
    return m_IsRunningAnim
        ? new CTaskSimpleSlideToCoord{ m_SlideToPos, m_AimingRotation, m_Speed, m_animName, m_animGroupName, m_animFlags, m_fBlendDelta, !!m_bRunInSequence, m_Time }
        : new CTaskSimpleSlideToCoord{ m_SlideToPos, m_AimingRotation, m_Speed };
}

// 0x66C4D0
bool CTaskSimpleSlideToCoord::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return !m_IsRunningAnim || CTaskSimpleAnim::MakeAbortable(ped, priority, event);
}

// 0x66C4E0
bool CTaskSimpleSlideToCoord::ProcessPed(CPed* ped) {
    const bool isAnimDone = !m_IsRunningAnim || CTaskSimpleRunNamedAnim::ProcessPed(ped);

    if (m_EndTime == -1) {
        if (!m_IsRunningAnim) {
            m_EndTime = (int32)(CTimer::GetTimeInMS() + 2'000);
        } else if (isAnimDone) {
            m_EndTime = (int32)(CTimer::GetTimeInMS() + 500);
        }
    }

    if (m_IsFirstTime) { // 0x66C547
        CTaskSimpleStandStill{ STAND_STILL_TIME }.ProcessPed(ped);
        if (ped->IsPlayer()) {
            ped->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_DEFAULT)->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr);
        }
        ped->m_fAimingRotation = m_AimingRotation;
        m_IsFirstTime          = false;
    }

    const auto distSq2D = CVector2D::DistSqr(ped->GetPosition(), m_SlideToPos);
    if (distSq2D < sq(0.05f)) { // 0x66C66E
        ped->m_vecAnimMovingShiftLocal = CVector2D{};
    } else {
        const CVector toSlidePos = (m_SlideToPos - ped->GetPosition()) * m_Speed;
        const auto&   pedMat     = ped->GetMatrix();
        ped->m_vecAnimMovingShiftLocal = CVector2D{
            toSlidePos.Dot(pedMat.GetRight()),
            toSlidePos.Dot(pedMat.GetForward())
        };
    }

    if ((uint32)(m_EndTime) < CTimer::GetTimeInMS()) { // 0x66C68B
        return true;
    }

    return isAnimDone
        && distSq2D < sq(0.05f)
        && std::abs(CGeneral::LimitRadianAngle(ped->m_fCurrentRotation - ped->m_fAimingRotation)) < 0.1f;
}

void CTaskSimpleSlideToCoord::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleSlideToCoord, 0x86ffec, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x66C3E0, "1", {}, const CVector&, float, float);
    RH_ScopedConstructorInstall(0x66C450, "2", {}, const CVector&, float, float, const char*, const char*, uint32, float, bool, uint32);
    RH_ScopedDestructorInstall(0x66C4C0);

    RH_ScopedVMTInstall(Clone, 0x66D300);
    RH_ScopedVMTInstall(GetTaskType, 0x66C440);
    RH_ScopedVMTInstall(MakeAbortable, 0x66C4D0);
    RH_ScopedVMTInstall(ProcessPed, 0x66C4E0);
}
