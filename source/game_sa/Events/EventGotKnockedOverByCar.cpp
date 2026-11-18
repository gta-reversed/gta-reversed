#include "StdInc.h"

#include "EventGotKnockedOverByCar.h"

// 0x4B1B60
CEventGotKnockedOverByCar::CEventGotKnockedOverByCar(CVehicle* vehicle) :
    m_KnockedOverByCar{ vehicle } {
}

// 0x4B1C70
bool CEventGotKnockedOverByCar::AffectsPed(CPed* ped) {
    if (m_KnockedOverByCar && !ped->IsPlayer()) {
        return ped->IsAlive();
    }
    return false;
}

void CEventGotKnockedOverByCar::InjectHooks() {
    RH_ScopedVirtualClass(CEventGotKnockedOverByCar, 0x85b618, 17);
    RH_ScopedCategory("Events");

    RH_ScopedConstructorInstall(0x4B1B60, "1", {}, CVehicle*);
    RH_ScopedDestructorInstall(0x4B1C10);

    RH_ScopedVMTInstall(GetEventType, 0x4B1BD0);
    RH_ScopedVMTInstall(GetEventPriority, 0x4B1C00);
    RH_ScopedVMTInstall(GetLifeTime, 0x4B1BE0);
    RH_ScopedVMTInstall(AffectsPed, 0x4B1C70);
    RH_ScopedVMTInstall(GetSourceEntity, 0x4B1CA0);
    RH_ScopedVMTInstall(GetLocalSoundLevel, 0x4B1BF0);
    RH_ScopedVMTInstall(CloneEditable, 0x4B7960);
}
