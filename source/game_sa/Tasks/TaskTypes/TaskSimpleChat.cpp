#include "StdInc.h"
#include "TaskSimpleChat.h"


void CTaskSimpleChat::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleChat, 0x86C78C, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Destructor, 0x5F7FD0);
    RH_ScopedVMTInstall(Clone, 0x5F65E0);
    RH_ScopedVMTInstall(IsInterruptable, 0x5F6650);
}

// 0x5F65A0
CTaskSimpleChat::CTaskSimpleChat(uint32 duration) :
    CTaskSimpleRunTimedAnim{
        ANIM_GROUP_DEFAULT,
        ANIM_ID_IDLE_CHAT,
        4.f,
        -4.f,
        duration,
        TASK_SIMPLE_CHAT,
        "Chat",
        0
    }
{
}

// 0x5F65E0
CTaskSimpleChat::CTaskSimpleChat(const CTaskSimpleChat& o) :
    CTaskSimpleChat{ o.m_durationMs }
{
}

// 0x5F7FD0
CTaskSimpleChat::~CTaskSimpleChat() {
    assert(false && "Destructor not reversed"); // TODO: Reverse destructor}
}
