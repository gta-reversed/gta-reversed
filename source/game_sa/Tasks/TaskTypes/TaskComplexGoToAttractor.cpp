#include "StdInc.h"

#include "TaskComplexGoToAttractor.h"
#include "PedAtmAttractor.h"
#include "PedPlacement.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleSlideToCoord.h"
#include "TaskSimpleStandStill.h"

void CTaskComplexGoToAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToAttractor, 0x86FF3C, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x66B640);
    RH_ScopedInstall(Destructor, 0x66B6A0);

    RH_ScopedVMTInstall(Clone, 0x66D130);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x66B6C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x670420);
}

// 0x66B640
CTaskComplexGoToAttractor::CTaskComplexGoToAttractor(CPedAttractor* attractor, const CVector& pos, float heading, float attrTime, int32 queueNumber, eMoveState ms) :
    CTaskComplex() {
    m_Attractor    = attractor;
    m_vecAttrPosn  = pos;
    m_fAttrHeading = heading;
    m_MoveState    = ms;
    m_fAttrTime    = attrTime;
    m_nQueueNumber = queueNumber;
}

// 0x66B6B0
bool CTaskComplexGoToAttractor::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return m_pSubTask->MakeAbortable(ped, priority, event);
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
    auto moveState = m_MoveState;
    if (m_Attractor->GetType() == PED_ATTRACTOR_SHELTER) {
        moveState = PEDMOVE_RUN;
    }

    if (!ped->bUseAttractorInstantly) {
        auto* const sequence = new CTaskComplexSequence{};
        sequence->AddTask(new CTaskComplexGoToPointAndStandStill{ moveState, m_vecAttrPosn, 0.02f, 0.04f, false, false });
        sequence->AddTask(new CTaskSimpleSlideToCoord{ m_vecAttrPosn, m_fAttrHeading, 0.5f });
        return sequence;
    }

    // 0x616920 (Refactored there to return the adjusted position instead of modifying it in-place)
    m_vecAttrPosn = std::get<CVector>(CPedPlacement::FindZCoorForPed(m_vecAttrPosn));
    ped->SetPosn(m_vecAttrPosn);
    ped->m_fAimingRotation  = m_fAttrHeading;
    ped->m_fCurrentRotation = m_fAttrHeading;
    return new CTaskSimpleStandStill{ 0, false, false, 8.0f };
}

// 0x66B710
CTask* CTaskComplexGoToAttractor::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}
