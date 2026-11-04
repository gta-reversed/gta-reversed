#include "StdInc.h"
#include "TaskComplexSignalAtPed.h"

void CTaskComplexSignalAtPed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSignalAtPed, 0x86fa8c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x660A30);
    RH_ScopedInstall(Destructor, 0x660AB0);

    RH_ScopedVMTInstall(Clone, 0x662140);
    RH_ScopedVMTInstall(GetTaskType, 0x660AA0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x660B30, {.Reversed = false});
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x660CC0, {.Reversed = false});
    RH_ScopedVMTInstall(ControlSubTask, 0x660D80, {.Reversed = false});
}

// 0x660A30
CTaskComplexSignalAtPed::CTaskComplexSignalAtPed(CPed* pedToSignalAt, int32 unused1, bool playAnimAtEnd) :
    m_pedToSignalAt{pedToSignalAt},
    m_playAnimAtEnd{playAnimAtEnd}
{
    CEntity::SafeRegisterRef(m_pedToSignalAt);
}

CTaskComplexSignalAtPed::CTaskComplexSignalAtPed(const CTaskComplexSignalAtPed& o) :
    CTaskComplexSignalAtPed{o.m_pedToSignalAt, o.m_initialPause, o.m_playAnimAtEnd}
{
}

// 0x660AB0
CTaskComplexSignalAtPed::~CTaskComplexSignalAtPed() {
    CEntity::SafeCleanUpRef(m_pedToSignalAt);
}

// 0x660B30
CTask* CTaskComplexSignalAtPed::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x660B30, CTaskComplexSignalAtPed*, CPed*>(this, ped);
}

// 0x660CC0
CTask* CTaskComplexSignalAtPed::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x660CC0, CTaskComplexSignalAtPed*, CPed*>(this, ped);
}

// 0x660D80
CTask* CTaskComplexSignalAtPed::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x660D80, CTaskComplexSignalAtPed*, CPed*>(this, ped);
}
