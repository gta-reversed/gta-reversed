#include "StdInc.h"

#include "InformGroupEvent.h"

void CInformGroupEvent::InjectHooks() {
    RH_ScopedClass(CInformGroupEvent);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x4AC350);
    RH_ScopedInstall(Destructor, 0x4B29A0);
    RH_ScopedInstall(Process, 0x4B29E0);
}

// 0x4B29A0
CInformGroupEvent::~CInformGroupEvent() {
    CEntity::SafeCleanUpRef(m_Ped);
    m_Ped = nullptr;
    delete std::exchange(m_Event, nullptr);
    m_Time = -1;
}

// 0x4B29E0
void CInformGroupEvent::Process() {
    if (m_Ped) {
        // FIXME: This cast isn't safe. Also in CInformFriendsEventQueue::Process.
        if (static_cast<uint32>(m_Time) >= CTimer::GetTimeInMS()) {
            return;
        }

        if (auto* group = CPedGroups::GetPedsGroup(m_Ped); group) {
            if (const auto id = group->GetId(); id >= 0 && CPedGroups::ms_activeGroups[id]) {
                group->m_groupIntelligence.AddEvent(CEventGroupEvent{ m_Ped, m_Event->Clone() });
            }
        }

        CEntity::SafeCleanUpRef(m_Ped);
        m_Ped = nullptr;
    }

    delete std::exchange(m_Event, nullptr);
    m_Time = -1;
}
