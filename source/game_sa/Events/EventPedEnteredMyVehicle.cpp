#include "StdInc.h"
#include "EventPedEnteredMyVehicle.h"

CEventPedEnteredMyVehicle::CEventPedEnteredMyVehicle(CPed* pedThatEntered, CVehicle* vehicle, eTargetDoor targetDoor) :
    m_Vehicle{ vehicle },
    m_PedThatEntered{ pedThatEntered },
    m_TargetDoor{ targetDoor } {
}

bool CEventPedEnteredMyVehicle::AffectsPed(CPed* ped) {
    return ped->IsAlive()
        && ped->m_pVehicle
        && ped->bInVehicle
        && ped->m_pVehicle == m_Vehicle
        && m_PedThatEntered;
}

void CEventPedEnteredMyVehicle::InjectHooks() {
    RH_ScopedVirtualClass(CEventPedEnteredMyVehicle, 0x85afe8, 17);
    RH_ScopedCategory("Events");

    RH_ScopedConstructorInstall(0x4AEAC0, "1", {}, CPed*, CVehicle*, eTargetDoor);
    RH_ScopedDestructorInstall(0x4AEB90);

    RH_ScopedVMTInstall(GetEventType, 0x4AEB40);
    RH_ScopedVMTInstall(GetEventPriority, 0x4AEB80);
    RH_ScopedVMTInstall(GetLifeTime, 0x4AEB50);
    RH_ScopedVMTInstall(AffectsPed, 0x4AEC00);
    RH_ScopedVMTInstall(GetSourceEntity, 0x4AEB60);
    RH_ScopedVMTInstall(GetLocalSoundLevel, 0x4AEB70);
    RH_ScopedVMTInstall(CloneEditable, 0x4B7170);
}
