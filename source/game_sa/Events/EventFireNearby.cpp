#include "StdInc.h"
#include "EventFireNearby.h"

CEventFireNearby::CEventFireNearby(const CVector& position) :
    m_position{ position } {
}

bool CEventFireNearby::AffectsPed(CPed* ped) {
    return !ped->GetTaskManager().Has<TASK_COMPLEX_EXTINGUISH_FIRES>() && ped->IsAlive();
}

void CEventFireNearby::InjectHooks() {
    RH_ScopedVirtualClass(CEventFireNearby, 0x85b6e8, 17);
    RH_ScopedCategory("Events");

    RH_ScopedConstructorInstall(0x4B1F10, "1", {}, const CVector&);
    RH_ScopedDestructorInstall(0x4B1F80);

    RH_ScopedVMTInstall(GetEventType, 0x4B1F50, { .Reversed = false });
    RH_ScopedVMTInstall(GetEventPriority, 0x4B1F70, { .Reversed = false });
    RH_ScopedVMTInstall(GetLifeTime, 0x4B1F60, { .Reversed = false });
    RH_ScopedVMTInstall(AffectsPed, 0x4B1F90, { .Reversed = false });
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4B1FC0, { .Reversed = false });
    RH_ScopedVMTInstall(CloneEditable, 0x4B7A70, { .Reversed = false });
}
