#include "StdInc.h"

#include "TaskComplexChat.h"

#include "TaskSimpleChat.h"
#include "InterestingEvents.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleStandStill.h"

void CTaskComplexChat::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexChat, 0x8706A0, 14);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x682CB0);
    RH_ScopedInstall(Destructor, 0x682D40);
    RH_ScopedVMTInstall(Clone, 0x682DA0);
    RH_ScopedVMTInstall(GetTaskType, 0x682D30);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x682E10);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x682E20);
    RH_ScopedVMTInstall(ControlSubTask, 0x683060);
}

// 0x682CB0
CTaskComplexChat::CTaskComplexChat(bool isChatter, CPed* partner, int32 stage, int16 globalSpeechContext) :
    m_IsChatter{ isChatter },
    m_ChatPartner{partner},
    m_Stage{stage},
    m_GlobalSpeechContext{ globalSpeechContext }
{
    CEntity::SafeRegisterRef(m_ChatPartner);
}

CTaskComplexChat::CTaskComplexChat(const CTaskComplexChat& o) :
    CTaskComplexChat{
        o.m_IsChatter,
        o.m_ChatPartner,
        o.m_Stage,
        o.m_GlobalSpeechContext
}
{
    m_TaskFinished = o.m_TaskFinished;
}

// 0x682D40
CTaskComplexChat::~CTaskComplexChat() {
    CEntity::SafeCleanUpRef(m_ChatPartner);
}

// 0x682E10
CTask* CTaskComplexChat::CreateNextSubTask(CPed* ped) {
    return nullptr;
}

// 0x682E20
CTask* CTaskComplexChat::CreateFirstSubTask(CPed* ped) {
    if (!m_ChatPartner) {
        return nullptr;
    }

    if (!m_IsChatter) {
        return new CTaskSimpleStandStill(999'999);
    }

    if (m_GlobalSpeechContext != eGlobalSpeechContext::CTX_GLOBAL_UNK) {
        ped->Say(m_GlobalSpeechContext);
    }

    g_InterestingEvents.Add(CInterestingEvents::PEDS_CHATTING, ped);

    if (m_GlobalSpeechContext == CTX_GLOBAL_PCONV_DISMISS || m_GlobalSpeechContext >= CTX_GLOBAL_PCONV_PART_FEM && m_GlobalSpeechContext <= CTX_GLOBAL_PCONV_PART_MALE) {
        switch (CGeneral::GetRandomNumberInRange(0, notsa::IsFixBugs() ? 4 : 3)) {
        case 0: return new CTaskSimpleRunAnim(ANIM_GROUP_DEFAULT, ANIM_ID_ENDCHAT_01, 4.0f, 0);
        case 1: return new CTaskSimpleRunAnim(ANIM_GROUP_DEFAULT, ANIM_ID_ENDCHAT_02, 4.0f, 0);
        case 2: return new CTaskSimpleRunAnim(ANIM_GROUP_DEFAULT, ANIM_ID_ENDCHAT_03, 4.0f, 0);
        case 3: return new CTaskSimpleChat{ 4000 }; // NOTE: (FIXED-BUG) Originally unreachable
        }
    }

    return new CTaskSimpleChat{ 999'999 };
}

// 0x683060
CTask* CTaskComplexChat::ControlSubTask(CPed* ped) {
    if (!m_ChatPartner) {
        return nullptr;
    }

    if (m_TaskFinished && m_pSubTask->MakeAbortable(ped)) {
        return nullptr;
    }

    const auto partnerChatTask = m_ChatPartner->GetTaskManager().Find<CTaskComplexChat>();
    if (partnerChatTask) {
        if (partnerChatTask->m_Stage < m_Stage || !partnerChatTask->m_TaskFinished && m_IsChatter == partnerChatTask->m_IsChatter) {
            m_TaskFinished = true;
        }
    }

    if (m_IsChatter && !ped->GetPedTalking()) {
        if (partnerChatTask) {
            if (!partnerChatTask->m_TaskFinished && !partnerChatTask->m_IsChatter) {
                partnerChatTask->m_TaskFinished = m_TaskFinished = true;
            }
        }
        m_NoChatTimeout++;
        if (m_NoChatTimeout > 50) {
            m_TaskFinished = true;
            if (partnerChatTask) {
                partnerChatTask->m_TaskFinished = true;
            }
        }
    }

    return m_pSubTask;
}
