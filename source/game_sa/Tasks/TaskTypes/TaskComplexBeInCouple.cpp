#include "StdInc.h"

#include "TaskComplexBeInCouple.h"
#include "TaskComplexWanderStandard.h"
#include "TaskComplexWalkAlongsidePed.h"
#include "TaskComplexWaitForPed.h"
#include "Ragdoll/IKChainManager.h"

// 0x6836F0
CTaskComplexBeInCouple::CTaskComplexBeInCouple(
    CPed* partner,
    bool  isLeader,
    bool  holdHands,
    bool  lookAtEachOther,
    float giveUpDist
) :
    m_Partner{ partner },
    m_IsLeader{ isLeader },
    m_HoldHands{ holdHands },
    m_LookAtEachOther{ lookAtEachOther },
    m_GiveUpDist{ giveUpDist } {
}

// NOTSA
CTaskComplexBeInCouple::CTaskComplexBeInCouple(const CTaskComplexBeInCouple& o) :
    CTaskComplexBeInCouple{ o.m_Partner, o.m_IsLeader, o.m_HoldHands, o.m_LookAtEachOther, o.m_GiveUpDist } {
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

// 0x683840
CTaskComplexBeInCouple::ePartnerType CTaskComplexBeInCouple::GetPartnerSide(CPed* ped) const {
    const CVector& partnerRight = m_Partner->GetRight();
    return partnerRight.Dot(ped->GetPosition()) - partnerRight.Dot(m_Partner->GetPosition()) < 0.0f
        ? LEFT
        : RIGHT;
}

// 0x6847C0
bool CTaskComplexBeInCouple::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (m_Partner && event && event->HasEditableResponse()) {
        auto editable                = static_cast<CEventEditableResponse*>(const_cast<CEvent*>(event)); // Okay let's go!

        bool bAddToEventGroup        = editable->m_bAddToEventGroup;
        editable->m_bAddToEventGroup = false;
        switch (event->GetEventType()) {
        case EVENT_DAMAGE:
        case EVENT_GUN_AIMED_AT:
        case EVENT_SHOT_FIRED:
            m_Partner->GetEventGroup().Add(editable, false);
            break;
        }
        editable->m_bAddToEventGroup = bAddToEventGroup;
    }
    AbortArmIK(ped);
    return true;
}

// 0x684840
CTask* CTaskComplexBeInCouple::CreateNextSubTask(CPed* ped) {
    if (!m_Partner) { // 0x68485F
        AbortArmIK(ped);
        return nullptr;
    }
    if (m_IsLeader) { // 0x684884
        AbortArmIK(ped);
        return new CTaskComplexWanderStandard{ PEDMOVE_WALK, (uint8)(CGeneral::GetRandomNumberInRange(0, 8)), true };
    }
    AbortArmIK(ped); // 0x6848D6
    return new CTaskComplexWalkAlongsidePed{ m_Partner, m_GiveUpDist };
}

// 0x684930
CTask* CTaskComplexBeInCouple::ControlSubTask(CPed* ped) {
    constexpr float LEADER_WAIT_DIST_SQ = 4.f; // Leader waits for the partner if they're further than this
    constexpr float HOLD_HANDS_DIST_SQ  = 2.25f;

    if (!m_Partner || !m_Partner->IsAlive()) {
        AbortArmIK(ped);
        return nullptr;
    }

    auto* const tPartnerCouple = m_Partner->GetTaskManager().Find<CTaskComplexBeInCouple>(false);
    if (!tPartnerCouple || tPartnerCouple->m_IsLeader == m_IsLeader || tPartnerCouple->m_Partner != ped) { // 0x68498F
        AbortArmIK(ped);
        return nullptr;
    }

    auto* const tCouple        = ped->GetTaskManager().Find<CTaskComplexBeInCouple>();
    auto* const tPartnerActive = m_Partner->GetTaskManager().Find<CTaskComplexBeInCouple>();
    const auto  side           = GetPartnerSide(ped);
    if (!m_IsLeader) {
        if (auto* const tWalkAlongside = ped->GetTaskManager().Find<CTaskComplexWalkAlongsidePed>(false)) { // 0x684A07
            tWalkAlongside->SetOffset({ side == LEFT ? -1.05f : 1.05f, 0.f, 0.f });
        }
    }

    const CVector2D usToPartner = m_Partner->GetPosition2D() - ped->GetPosition2D();
    const float     distSq      = usToPartner.SquaredMagnitude();
    if (distSq > sq(m_GiveUpDist) || !tCouple || !tPartnerActive) { // 0x684AB4
        AbortArmIK(ped);
        return nullptr;
    }

    if (distSq > LEADER_WAIT_DIST_SQ && m_IsLeader) { // 0x684AE2
        AbortArmIK(ped);
        if (!ped->GetTaskManager().Find<CTaskComplexWaitForPed>(false)) {
            return new CTaskComplexWaitForPed{ m_Partner, 0.75f, 20'000, false };
        }
    }

    if (m_HoldHands) { // 0x684B43
        CVector handPos = ped->GetPosition() + CVector{ usToPartner.x * 0.5f, usToPartner.y * 0.5f, 0.f };
        if (distSq < HOLD_HANDS_DIST_SQ) {
            if (side != m_PrevSide) {
                AbortArmIK(ped);
            }
            switch (side) {
            case LEFT: // 0x684BB3
                g_ikChainMan.PointArm("CTaskComplexBeInCouple", IK_ARM_RIGHT, ped, nullptr, BONE_UNKNOWN, &handPos, 0.5f, 250, 30.f);
                m_PrevSide = (int8)(side);
                break;
            case RIGHT: // 0x684BE7
                g_ikChainMan.PointArm("CTaskComplexBeInCouple", IK_ARM_LEFT, ped, nullptr, BONE_UNKNOWN, &handPos, 0.5f, 250, 30.f);
                m_PrevSide = (int8)(side);
                break;
            default:
                NOTSA_UNREACHABLE_CASE(side);
            }
        } else { // 0x684C15
            AbortArmIK(ped);
        }
    }

    if (m_LookAtEachOther) { // 0x684C28
        const auto LookAtIfNotLooking = [](CPed* looker, CPed* target) {
            if (!g_ikChainMan.IsLooking(looker) && CGeneral::GetRandomNumberInRange(0, 100) > 80) {
                g_ikChainMan.LookAt("TaskBeInCouple", looker, target, CGeneral::GetRandomNumberInRange(2'000, 4'000), BONE_HEAD, nullptr, false, 0.25f, 500, 3, false);
            }
        };
        LookAtIfNotLooking(ped, m_Partner);
        LookAtIfNotLooking(m_Partner, ped);
    }

    return m_pSubTask;
}

void CTaskComplexBeInCouple::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexBeInCouple, 0x870724, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x6836F0, "1", {}, CPed*, bool, bool, bool, float);
    RH_ScopedDestructorInstall(0x683780);

    RH_ScopedInstall(AbortArmIK, 0x6837F0);
    RH_ScopedInstall(GetPartnerSide, 0x683840);

    RH_ScopedVMTInstall(Clone, 0x6839C0);
    RH_ScopedVMTInstall(GetTaskType, 0x683770);
    RH_ScopedVMTInstall(MakeAbortable, 0x6847C0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x684840);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6837E0);
    RH_ScopedVMTInstall(ControlSubTask, 0x684930);
}
