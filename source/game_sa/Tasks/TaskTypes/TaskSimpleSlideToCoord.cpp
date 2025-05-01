#include "StdInc.h"

#include "TaskSimpleSlideToCoord.h"
#include "TaskSimpleStandStill.h"

void CTaskSimpleSlideToCoord::InjectHooks() {
    // + RH_ScopedVirtualClass(CTaskSimpleSlideToCoord, 0x86FFEC, 9); // todo (Pirulax): Make it working
    // + RH_ScopedCategory("Tasks/TaskTypes");

    // + RH_ScopedOverloadedInstall(Constructor, "NoAnim", 0x66C3E0, CTaskSimpleSlideToCoord*(CTaskSimpleSlideToCoord::*)(CVector const&, float, float));
    // + RH_ScopedOverloadedInstall(Constructor, "Anim", 0x66C450, CTaskSimpleSlideToCoord*(CTaskSimpleSlideToCoord::*)(CVector const&, float, float, char const*, char const*, int32, float, bool, int32));
    // + RH_ScopedVmtInstall(MakeAbortable, 0x66C4D0);
    // RH_ScopedVmtInstall(ProcessPed, 0x66C4E0);
}

// 0x66C3E0
CTaskSimpleSlideToCoord::CTaskSimpleSlideToCoord(const CVector& slideToPos, float aimingRotation, float speed) :
    CTaskSimpleRunNamedAnim(),
    m_SlideToPos{ slideToPos },
    m_fAimingRotation{ aimingRotation },
    m_fSpeed{ speed },
    m_bFirstTime{ true },
    m_bRunningAnim{ false }
{
    // m_Timer not initialized
}

// 0x66C450
CTaskSimpleSlideToCoord::CTaskSimpleSlideToCoord(const CVector& slideToPos, float aimingRotation, float speed, const char* animBlockName, const char* animGroupName, uint32 animFlags, float animBlendDelta, bool bRunInSequence, uint32 endTime) :
    CTaskSimpleRunNamedAnim{ animBlockName, animGroupName, animFlags, animBlendDelta, endTime, false, bRunInSequence, false, false },
    m_SlideToPos{ slideToPos },
    m_fAimingRotation{ aimingRotation },
    m_fSpeed{ speed },
    m_bFirstTime{ true },
    m_bRunningAnim{ false },
    m_Timer{ -1 }
{
}

// 0x66D300
CTask* CTaskSimpleSlideToCoord::Clone() const {
    return m_bRunningAnim
        ? new CTaskSimpleSlideToCoord(m_SlideToPos, m_fAimingRotation, m_fSpeed, m_animName, m_animGroupName, m_animFlags, m_fBlendDelta, !!m_bRunInSequence, m_Time)
        : new CTaskSimpleSlideToCoord(m_SlideToPos, m_fAimingRotation, m_fSpeed);
}

// 0x66C4D0
bool CTaskSimpleSlideToCoord::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    return m_bRunningAnim ? CTaskSimpleAnim::MakeAbortable(ped, priority, event) : true;
}

// 0x66C4E0
bool CTaskSimpleSlideToCoord::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x66C4E0, CTaskSimpleSlideToCoord*, CPed*>(this, ped); // untested/review

    const bool hasSuccessfullyProcessedRunAnim = m_bRunningAnim ? CTaskSimpleRunNamedAnim::ProcessPed(ped) : true; // Yeah could be simplified, but this is easier to understand.

    if (m_Time == (uint32)-1) {
        if (m_bRunningAnim) {
            if (hasSuccessfullyProcessedRunAnim) {
                m_Time = CTimer::GetTimeInMS() + 500;
            }
        } else {
            m_Time = CTimer::GetTimeInMS() + 2000;
        }
    }

    // The same standstill stuff has been used in `CTaskSimpleWaitUntilPedIsOutCar` as well.. weird.
    if (m_bFirstTime) {
        m_bFirstTime = false;

        CTaskSimpleStandStill standStillTask{ STAND_STILL_TIME };
        standStillTask.ProcessPed(ped);
        if (ped->IsPlayer()) {
            ped->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_DEFAULT)->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr);
        }
        ped->m_fAimingRotation = m_fAimingRotation;
    }

    const auto pedToSlidePosDir = (m_SlideToPos - ped->GetPosition()) * m_fSpeed; // Originally they calculated this twice, but thats a waste of performance, so.. :D
    const auto pedToSlidePosDistSq2D = pedToSlidePosDir.SquaredMagnitude2D();
    if (pedToSlidePosDir.SquaredMagnitude2D() < sq(0.05f)) {
        ped->m_vecAnimMovingShiftLocal = CVector2D{};
    } else {
        const auto& pedMat = ped->GetMatrix();
        ped->m_vecAnimMovingShiftLocal = {
            DotProduct(pedToSlidePosDir, pedMat.GetForward()), // Originally Y was calculated first, but I swapped it!
            DotProduct(pedToSlidePosDir, pedMat.GetRight())
        };
    }

    if (m_Time < CTimer::GetTimeInMS()) {
        return true;
    }

    if (hasSuccessfullyProcessedRunAnim && pedToSlidePosDistSq2D < sq(0.05f)) {
        if (std::abs(ped->m_fCurrentRotation - ped->m_fAimingRotation) < 0.1f) {
            return true;
        }
    }

    return false;
}
