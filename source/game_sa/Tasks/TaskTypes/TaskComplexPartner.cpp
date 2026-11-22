#include "StdInc.h"

#include "TaskComplexPartner.h"
#include "TaskComplexPartnerChat.h"
#include "TaskSimpleStandStill.h"
#include "Ragdoll/IKChainManager.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskComplexWanderStandard.h"

void CTaskComplexPartner::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexPartner, 0x870664, 14);
    RH_ScopedCategory("Tasks/TaskTypes");
  
    RH_ScopedConstructorInstall(0x681E70, "1", {}, char *, CPed *, uint8, float, uint8, int32, CVector);
    RH_ScopedDestructorInstall(0x683A40);

    RH_ScopedInstall(CalcTargetPositions, 0x681FE0);
    RH_ScopedInstall(GetPartnerState, 0x6822B0, { .Reversed = false });
    RH_ScopedVMTInstall(GetTaskType, 0x681F10);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x683AD0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x681F20);
    RH_ScopedVMTInstall(ControlSubTask, 0x6840D0);
    RH_ScopedVMTInstall(StreamRequiredAnims, 0x682310);
    RH_ScopedVMTInstall(RemoveStreamedAnims, 0x682370);
}

// 0x681E70
CTaskComplexPartner::CTaskComplexPartner(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, bool makePedAlwaysFacePartner, int8 updateDirectionCount, CVector point) :
    m_leadSpeaker{ leadSpeaker },
    m_makePedAlwaysFacePartner{ makePedAlwaysFacePartner },
    m_distanceMultiplier{ distanceMultiplier },
    m_updateDirectionCount{ updateDirectionCount },
    m_point{ point },
    m_partner{ partner } {
}

// 0x683A40
CTaskComplexPartner::~CTaskComplexPartner() {
    if (m_requiredAnimsStreamedIn) {
        if (strcmp(m_animBlockName, "") != 0) {
            CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex(m_animBlockName));
        }
        m_requiredAnimsStreamedIn = false;
    }
}


// 0x681FE0
void CTaskComplexPartner::CalcTargetPositions(CPed * ped, CVector * point, CVector * targetPoint) {
    const auto pedPos     = ped->GetPosition();
    const auto partnerPos = m_partner->GetPosition();

    auto dir              = partnerPos - pedPos;
    auto mid              = pedPos + dir * m_distanceMultiplier;
    dir.Normalise();

    if (GetTaskType() != TASK_COMPLEX_PARTNER_CHAT) {
        // Move the meeting point until it's far enough from both peds
        for (auto i = 0;; i++) {
            if (i > 10) {
                m_taskCompleted = true; // Couldn't find a good spot, `point` and `targetPoint` are left untouched
                return;
            }

            bool movedAwayFromPed = false;
            if (const auto d = CVector2D::Dist(mid, pedPos); d < 0.7f) {
                mid += dir * (0.75f - d);
                movedAwayFromPed = true;
            }
            if (const auto d = CVector2D::Dist(mid, partnerPos); d < 0.7f) {
                mid -= dir * (0.75f - d);
            } else if (!movedAwayFromPed) {
                break;
            }
        }
    }

    *point       = mid - dir * 0.5f;
    *targetPoint = mid + dir * 0.5f;
}

// 0x6822B0
int32 CTaskComplexPartner::GetPartnerState() {
    return plugin::CallMethodAndReturn<int32, 0x6822B0, CTaskComplexPartner*>(this);
}

// 0x683AD0
CTask * CTaskComplexPartner::CreateNextSubTask(CPed * ped) {
    const auto partnerTask = m_partner
        ? static_cast<CTaskComplexPartner*>(m_partner->GetTaskManager().FindActiveTaskByType((eTaskType)m_taskId))
        : nullptr;
    if (!partnerTask || partnerTask->m_partner != ped) {
        return nullptr;
    }

    const auto CreateStandStill = [] {
        return new CTaskSimpleStandStill{ 50, false, false, 8.f };
    };

    switch (m_partnerState) {
    case 1: { // Wait for the stand still to finish, then pick the meeting points
        if (m_pSubTask->GetTaskType() != TASK_SIMPLE_STAND_STILL) {
            return nullptr;
        }
        if (m_leadSpeaker) {
            CalcTargetPositions(ped, &m_point, &m_targetPoint);
            static_cast<CTaskComplexPartner*>(m_partner->GetTaskManager().FindActiveTaskByType((eTaskType)m_taskId))->m_point = m_targetPoint;
        }
        m_partnerState = (ePartnerState)2;
        return CreateStandStill();
    }
    case 2: { // Go to our point (if we have one)
        if (m_pSubTask->GetTaskType() != TASK_SIMPLE_STAND_STILL) {
            return nullptr;
        }
        if (m_point.x == 0.f && m_point.y == 0.f) {
            return CreateStandStill();
        }
        m_partnerState          = (ePartnerState)3;
        m_goToPointFrameCounter = 0;
        return new CTaskComplexGoToPointAndStandStill{ PEDMOVE_WALK, m_point, 0.1f, 0.f, false, true };
    }
    case 3: { // Arrived, face the partner
        switch (m_pSubTask->GetTaskType()) {
        case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
            return new CTaskComplexTurnToFaceEntityOrCoord{ m_partner, 0.5f, 0.2f };
        case TASK_COMPLEX_TURN_TO_FACE_ENTITY:
            m_partnerState = (ePartnerState)4;
            return CreateStandStill();
        default:
            return nullptr;
        }
    }
    case 4: { // Wait for the partner to arrive too, the first one there gets `m_firstToTargetFlag = 0`
        const auto  partnerState = GetPartnerState();
        auto* const pt           = static_cast<CTaskComplexPartner*>(m_partner->GetTaskManager().FindActiveTaskByType((eTaskType)m_taskId));
        if (partnerState == 4 || partnerState == 5) {
            if (m_firstToTargetFlag == -1) {
                m_firstToTargetFlag = 0;
            }
            if (pt->m_firstToTargetFlag == -1) {
                pt->m_firstToTargetFlag = 1;
            }
            m_partnerState = (ePartnerState)5;
        } else {
            if (m_firstToTargetFlag == -1) {
                m_firstToTargetFlag = 1;
            }
            if (pt->m_firstToTargetFlag == -1) {
                pt->m_firstToTargetFlag = 0;
            }
            pt->m_point = m_targetPoint;
        }
        return CreateStandStill();
    }
    case 5: {
        if (!m_makePedAlwaysFacePartner) {
            if (GetPartnerState() >= 5) {
                // Make sure the ped wanders off the way it's facing after this
                auto* const defaultTask = ped->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_DEFAULT);
                if (defaultTask && defaultTask->GetTaskType() == TASK_COMPLEX_WANDER && static_cast<CTaskComplexWander*>(defaultTask)->GetWanderType() == WANDER_TYPE_STANDARD) {
                    const auto fwd    = ped->GetForward();
                    auto*      wander = new CTaskComplexWanderStandard{ PEDMOVE_WALK, (uint8)CGeneral::GetNodeHeadingFromVector(-fwd.x, -fwd.y), true };
                    wander->SetMinNextScanTime(CTimer::GetTimeInMS() + 100'000);
                    ped->GetTaskManager().SetTask(wander, TASK_PRIMARY_DEFAULT);
                }
                m_partnerState = (ePartnerState)6;
            }
        } else if (m_firstToTargetFlag == 1) {
            if (GetPartnerState() == 6) {
                m_partnerState = (ePartnerState)6;
            }
        } else {
            // Shuffle (max 0.02 per step) until we're 1m away from the partner
            const auto pedPos     = CVector2D{ ped->GetPosition() };
            const auto partnerPos = CVector2D{ m_partner->GetPosition() };
            auto       dir        = pedPos - partnerPos;
            if (const auto dist = dir.Magnitude(); dist > 0.99f && dist < 1.01f) {
                m_partnerState = (ePartnerState)6;
            } else {
                dir.Normalise();
                auto shift = partnerPos + dir - pedPos;
                if (shift.Magnitude() > 0.02f) {
                    shift.Normalise();
                    shift *= 0.02f;
                }
                const auto& mat                = ped->GetMatrix();
                ped->m_vecAnimMovingShiftLocal = CVector2D{
                    mat.GetRight().x * shift.x + mat.GetRight().y * shift.y,
                    mat.GetForward().x * shift.x + mat.GetForward().y * shift.y,
                };
            }
        }
        return CreateStandStill();
    }
    case 6: { // Both there, run the actual partner sequence (once per `m_updateDirectionCount`)
        if (m_requiredAnimsStreamedIn && m_updateDirectionCount) {
            m_updateDirectionCount--;
            return GetPartnerSequence();
        }
        return nullptr;
    }
    default:
        return nullptr;
    }
}

// 0x681F20
CTask * CTaskComplexPartner::CreateFirstSubTask(CPed * ped) {
    if (m_leadSpeaker && m_taskId == TASK_COMPLEX_PARTNER_CHAT) {
        // Only a chat task gets here, the fields are past the end of this class
        auto* const chat = static_cast<CTaskComplexPartnerChat*>(this);
        if (chat->m_conversationEnabled) {
            if (CAEPedSpeechAudioEntity::RequestPedConversation(ped, m_partner)) {
                chat->m_pedConversationLoaded = true;
            } else if (chat->field_75) {
                chat->m_conversationEnabled = false;
            } else {
                return nullptr;
            }
        }
    }
    ped->StopPlayingHandSignal();
    return new CTaskSimpleStandStill{ 50, false, false, 8.f };
}

// 0x6840D0
CTask * CTaskComplexPartner::ControlSubTask(CPed * ped) {
    const auto IsPartnerStillWithUs = [&] {
        if (!m_partner) {
            return false;
        }
        auto* const t = static_cast<CTaskComplexPartner*>(m_partner->GetTaskManager().FindActiveTaskByType((eTaskType)m_taskId));
        return t && t->m_partner == ped && !m_taskCompleted;
    };
    if ((m_partnerState > 1 || !m_partner || m_taskCompleted) && !IsPartnerStillWithUs()) {
        if (m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
            return nullptr;
        }
    }

    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL && ++m_goToPointFrameCounter > 150) {
        if (m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
            return nullptr;
        }
    }

    ped->DropEntityThatThisPedIsHolding(true);
    StreamRequiredAnims();
    if (g_ikChainMan.IsLooking(ped)) {
        g_ikChainMan.AbortLookAt(ped, 500);
    }
    return m_pSubTask;
}

// 0x682310
void CTaskComplexPartner::StreamRequiredAnims() {
    if (m_requiredAnimsStreamedIn) {
        return;
    }
    if (strcmp(m_animBlockName, "") != 0) {
        const auto idx = CAnimManager::GetAnimationBlockIndex(m_animBlockName);
        if (!CAnimManager::GetAnimBlocks()[idx].IsLoaded) {
            CStreaming::RequestModel(IFPToModelId(idx), STREAMING_KEEP_IN_MEMORY);
            return;
        }
        CAnimManager::AddAnimBlockRef(idx);
    }
    m_requiredAnimsStreamedIn = true;
}

// 0x682370
void CTaskComplexPartner::RemoveStreamedAnims() {
    if (!m_requiredAnimsStreamedIn) {
        return;
    }
    if (strcmp(m_animBlockName, "") != 0) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex(m_animBlockName));
    }
    m_requiredAnimsStreamedIn = false;
}
