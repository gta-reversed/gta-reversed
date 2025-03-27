#include "StdInc.h"

#include "TaskSimpleAnim.h"

void CTaskSimpleAnim::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleAnim, 0x86D504, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedVMTInstall(MakeAbortable, 0x61A790);
}

// 0x61A6C0
CTaskSimpleAnim::CTaskSimpleAnim(bool bHoldLastFrame) : CTaskSimple() {
    m_bIsFinished = false;
    m_bDontInterrupt = false;
    m_bHoldLastFrame = bHoldLastFrame;
    m_pAnim = nullptr;
}

// 0x61A6F0
CTaskSimpleAnim::~CTaskSimpleAnim() {
    if (!m_pAnim)
        return;

    m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    m_pAnim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
    if (!m_bHoldLastFrame &&
        m_pAnim->m_BlendAmount > 0.0f &&
        m_pAnim->m_BlendDelta >= 0.0f
    ) {
        m_pAnim->m_BlendDelta = -4.0f;
    }
    m_pAnim = nullptr;
}

// 0x61A790

// 0x0
bool CTaskSimpleAnim::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    bool bSkipBlend = false;
    auto fBlend = -4.0F;
    if (priority == eAbortPriority::ABORT_PRIORITY_IMMEDIATE) {
        fBlend = -1000.0F;
        m_bHoldLastFrame = false;
    } else if (m_bDontInterrupt) {
        if (event && event->GetEventType() != eEventType::EVENT_SCRIPT_COMMAND)
            return false;
    } else if (event) {
        if (event->GetEventType() == eEventType::EVENT_SCRIPT_COMMAND) {
            const auto* scriptCommand = static_cast<const CEventScriptCommand*>(event);
            if (scriptCommand->m_task) {
                if (scriptCommand->m_task->GetTaskType() == TASK_SIMPLE_NAMED_ANIM) {
                    if (m_pAnim)
                        m_pAnim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;

                    bSkipBlend = true;
                }
            }
        }
    }

    if (!bSkipBlend) {
        if (m_pAnim) {
            m_pAnim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
            if (!m_bHoldLastFrame) {
                if (m_pAnim->m_Flags & ANIMATION_IS_PARTIAL)
                    m_pAnim->m_BlendDelta = fBlend;
                else
                    CAnimManager::BlendAnimation(ped->m_pRwClump, ped->m_nAnimGroup, ANIM_ID_IDLE, -fBlend);
            }
        }
    }

    if (priority == ABORT_PRIORITY_IMMEDIATE || priority == ABORT_PRIORITY_URGENT || m_bHoldLastFrame) {
        if (m_pAnim) {
            m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_bIsFinished = true;
            m_pAnim = nullptr;
        }
        return true;
    }

    return false;
}

// data is CTaskSimpleAnim
// 0x61A8A0
void CTaskSimpleAnim::FinishRunAnimCB(CAnimBlendAssociation* blendAssoc, void* data) {
    static_cast<CTaskSimpleAnim*>(data)->m_bIsFinished = true;
    static_cast<CTaskSimpleAnim*>(data)->m_pAnim = nullptr;
}
