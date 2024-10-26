#include "StdInc.h"

#include "InformFriendsEventQueue.h"

void CInformFriendsEventQueue::InjectHooks() {
    RH_ScopedClass(CInformFriendsEventQueue);
    RH_ScopedCategoryGlobal();

    //RH_ScopedInstall(Constructor, 0x0, { .reversed = false }); <-- nop
    //RH_ScopedInstall(Destructor, 0x0, { .reversed = false }); <-- nop
    RH_ScopedInstall(Init, 0x4B2990);
    RH_ScopedInstall(Flush, 0x4AC2A0);
    RH_ScopedInstall(Add, 0x4AC1E0, { .reversed = false });
    RH_ScopedInstall(Process, 0x4AC2E0, { .reversed = false });
}

// 0x4B2990
void CInformFriendsEventQueue::Init() {
    CInformFriendsEventQueue::Flush();
}

// 0x4AC2A0
void CInformFriendsEventQueue::Flush() {
    for (auto& e : ms_informFriendsEvents) {
        CEntity::SafeCleanUpRef(e.m_Ped);
        e.m_Ped = nullptr;
        delete std::exchange(e.m_Event, nullptr);
        e.m_Time = -1;
    }
}

// 0x4AC1E0
bool CInformFriendsEventQueue::Add(CEntity* entity, CEvent* event) {
    return plugin::CallAndReturn<bool, 0x4AC1E0, CEntity*, CEvent*>(entity, event);
}

// 0x4AC2E0
void CInformFriendsEventQueue::Process() {
    plugin::Call<0x4AC2E0>();
}
