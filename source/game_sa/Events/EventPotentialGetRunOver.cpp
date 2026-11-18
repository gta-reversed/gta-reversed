#include "StdInc.h"
#include "EventPotentialGetRunOver.h"

// 0x4AE1A0
CEventPotentialGetRunOver::CEventPotentialGetRunOver(CVehicle* runOverBy) :
    m_RunOverBy{ runOverBy } {
}

// 0x4AE2D0
bool CEventPotentialGetRunOver::AffectsPed(CPed* ped) {
    return ped->IsAlive()
        && !ped->m_pAttachedTo
        && !ped->bInVehicle
        && m_RunOverBy
        && !m_RunOverBy->IsBoat();
}

void CEventPotentialGetRunOver::InjectHooks() {
    RH_ScopedVirtualClass(CEventPotentialGetRunOver, 0x85ae88, 17);
    RH_ScopedCategory("Events");

    RH_ScopedConstructorInstall(0x4AE1A0, "1", {}, CVehicle*);
    RH_ScopedDestructorInstall(0x4AE270);

    RH_ScopedVMTInstall(GetEventType, 0x4AE210);
    RH_ScopedVMTInstall(GetEventPriority, 0x4AE260);
    RH_ScopedVMTInstall(GetLifeTime, 0x4AE220);
    RH_ScopedVMTInstall(AffectsPed, 0x4AE2D0);
    RH_ScopedVMTInstall(GetSourceEntity, 0x4AE230);
    RH_ScopedVMTInstall(GetLocalSoundLevel, 0x4AE250);
    RH_ScopedVMTInstall(CloneEditable, 0x4B6F40);
}
