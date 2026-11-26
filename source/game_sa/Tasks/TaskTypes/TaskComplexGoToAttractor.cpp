#include "StdInc.h"

#include "TaskComplexGoToAttractor.h"
#include "PedAtmAttractor.h"
#include "PedPlacement.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleSlideToCoord.h"
#include "TaskSimpleStandStill.h"

// 0x66B640
CTaskComplexGoToAttractor::CTaskComplexGoToAttractor(CPedAttractor* attractor, const CVector& pos, float heading, float attrTime, int32 slot, eMoveState ms) :
    m_Attractor{ attractor },
    m_AttractPos{ pos },
    m_AttractHeading{ heading },
    m_MoveState{ ms },
    m_AttractTime{ attrTime },
    m_Slot{ slot } {
}

// For 0x66D130
CTaskComplexGoToAttractor::CTaskComplexGoToAttractor(const CTaskComplexGoToAttractor& o) :
    CTaskComplexGoToAttractor(
        o.m_Attractor,
        o.m_AttractPos,
        o.m_AttractHeading,
        o.m_AttractTime,
        o.m_Slot,
        o.m_MoveState
    ) {
}

// 0x66B6C0
CTask* CTaskComplexGoToAttractor::CreateNextSubTask(CPed* ped) {
    GetPedAttractorManager()->BroadcastArrival(ped, m_Attractor);
    if (ped->bUseAttractorInstantly && m_Attractor->GetHeadOfQueue() != ped) {
        ped->bUseAttractorInstantly = false;
    }
    return nullptr;
}

// 0x670420
CTask* CTaskComplexGoToAttractor::CreateFirstSubTask(CPed* ped) {
    if (ped->bUseAttractorInstantly) {
        m_AttractPos = std::get<CVector>(CPedPlacement::FindZCoorForPed(m_AttractPos));
        ped->SetPosn(m_AttractPos);
        ped->m_fAimingRotation = ped->m_fCurrentRotation = m_AttractHeading;
        return new CTaskSimpleStandStill{};
    }
    return new CTaskComplexSequence{
        new CTaskComplexGoToPointAndStandStill{ m_Attractor->GetType() == PED_ATTRACTOR_SHELTER ? PEDMOVE_RUN : m_MoveState, m_AttractPos, 0.5f, 2.f },
        new CTaskSimpleSlideToCoord{ m_AttractPos, m_AttractHeading, 0.5f }
    };
}

void CTaskComplexGoToAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToAttractor, 0x86FF3C, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x66B640, "1", {}, CPedAttractor*, const CVector&, float, float, int32, eMoveState);
    RH_ScopedDestructorInstall(0x66B6A0);

    RH_ScopedVMTInstall(Clone, 0x66D130);
    RH_ScopedVMTInstall(GetTaskType, 0x66B690);
    RH_ScopedVMTInstall(MakeAbortable, 0x66B6B0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x66B6C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x670420);
    RH_ScopedVMTInstall(ControlSubTask, 0x66B710);
}
