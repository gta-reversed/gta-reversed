#include "StdInc.h"

#include "TaskComplexBeInCouple.h"
#include "TaskComplexWanderStandard.h"
#include "IKChainManager_c.h"

// TODO: Inject Hooks

// 0x6836F0
CTaskComplexBeInCouple::CTaskComplexBeInCouple(
    CPed* ped,
    bool  isLeader,
    bool  holdHands,
    bool  lookAtEachOther,
    float giveUpDist
) :
    CTaskComplex(),
    m_partner{ped},
    m_isLeader{isLeader},
    m_holdHands{holdHands},
    m_lookAtEachOther{lookAtEachOther},
    m_giveUpDist{giveUpDist}
{
    CEntity::SafeRegisterRef(m_partner);
}

// 0x683780
CTaskComplexBeInCouple::~CTaskComplexBeInCouple() {
    CEntity::SafeCleanUpRef(m_partner);
}

// 0x6837E0
CTask* CTaskComplexBeInCouple::CreateFirstSubTask(CPed* ped) {
    return CreateNextSubTask(ped);
}

// 0x6837F0
void CTaskComplexBeInCouple::AbortArmIK(CPed* ped) {
    const auto DoAbortArmIK = [ped](eIKArm arm) {
        if (IKChainManager_c::IsArmPointing(arm, ped)) {
            IKChainManager_c::AbortPointArm(arm, ped, 250);
        }
    };
    DoAbortArmIK(eIKArm::IK_ARM_RIGHT);
    DoAbortArmIK(eIKArm::IK_ARM_LEFT);
}

// 0x6847C0
bool CTaskComplexBeInCouple::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (m_partner && event && event->HasEditableResponse()) {
        auto editable = static_cast<CEventEditableResponse*>(const_cast<CEvent*>(event)); // Okay let's go!

        bool bAddToEventGroup = editable->m_bAddToEventGroup;
        editable->m_bAddToEventGroup = false;
        switch (event->GetEventType()) {
        case EVENT_DAMAGE:
        case EVENT_GUN_AIMED_AT:
        case EVENT_SHOT_FIRED:
            m_partner->GetEventGroup().Add(editable, false);
            break;
        }
        editable->m_bAddToEventGroup = bAddToEventGroup;
    }
    AbortArmIK(ped);
    return true;
}

// 0x684840
CTask* CTaskComplexBeInCouple::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x684840, CTaskComplexBeInCouple*, CPed*>(this, ped);
    /*
    if (!m_otherPed) {
        AbortArmIK(ped);
        return nullptr;
    }

    if (this->m_isLeader) {
        AbortArmIK(ped);
        return new CTaskComplexWanderStandard(4, CGeneral::GetRandomNumberInRange(0, 8), true);
    } else {
        AbortArmIK(ped);
        // todo: return new CTaskComplexWalkAlongsidePed(m_otherPed, m_giveUpDist);
    }
    */
}

// 0x684930
CTask* CTaskComplexBeInCouple::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x684930, CTaskComplexBeInCouple*, CPed*>(this, ped);
}
