#include "StdInc.h"

#include "TaskComplexPartnerChat.h"

void CTaskComplexPartnerChat::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexPartnerChat, 0x8707c4, 14);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x684290, "1", {}, const char *, CPed *, bool, float, int8, bool, bool, CVector);
    RH_ScopedDestructorInstall(0x684320);

    RH_ScopedVMTInstall(Clone, 0x684E30);
    RH_ScopedVMTInstall(GetTaskType, 0x681E10);
    RH_ScopedVMTInstall(MakeAbortable, 0x682C60, { .Reversed = false });
    RH_ScopedVMTInstall(GetPartnerSequence, 0x684380, { .Reversed = false });
}

// 0x684290
CTaskComplexPartnerChat::CTaskComplexPartnerChat(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, int8 updateDirectionCount, bool conversationEnabled, bool a8, CVector point) :
    CTaskComplexPartner(commandName, partner, leadSpeaker, distanceMultiplier, false, updateDirectionCount, point) {
    m_taskId                = TASK_COMPLEX_PARTNER_CHAT;
    m_pedConversationLoaded = 0;
    m_conversationEnabled   = conversationEnabled;
    if (conversationEnabled) {
        m_updateDirectionCount = 4;
    }
    field_75 = a8;
    strcpy_s(m_commandName, commandName);
}

// 0x684320
CTaskComplexPartnerChat::~CTaskComplexPartnerChat() {
    if (m_conversationEnabled && m_pedConversationLoaded) {
        CAEPedSpeechAudioEntity::ReleasePedConversation();
    }
}

// 0x682C60
bool CTaskComplexPartnerChat::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return plugin::CallMethodAndReturn<bool, 0x682C60, CTask*, CPed*, int32, const CEvent*>(this, ped, priority, event);
}

// 0x684380
CTaskComplexSequence* CTaskComplexPartnerChat::GetPartnerSequence() {
    return plugin::CallMethodAndReturn<CTaskComplexSequence*, 0x684380, CTask*>(this);
}
