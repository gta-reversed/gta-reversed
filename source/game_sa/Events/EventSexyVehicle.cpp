#include "StdInc.h"
#include "EventSexyVehicle.h"

CEventSexyVehicle::CEventSexyVehicle(CVehicle* vehicle) :
    m_SexyVehicle{ vehicle } {
}

bool CEventSexyVehicle::AffectsPed(CPed* ped) {
    return ped->IsAlive() && m_SexyVehicle;
}

void CEventSexyVehicle::InjectHooks() {
    RH_ScopedVirtualClass(CEventSexyVehicle, 0x85b0f8, 16);
    RH_ScopedCategory("Events");

    RH_ScopedConstructorInstall(0x4AF010, "1", {}, CVehicle*);
    RH_ScopedDestructorInstall(0x4AF0A0);

    RH_ScopedVMTInstall(GetEventType, 0x4AF070);
    RH_ScopedVMTInstall(GetEventPriority, 0x4AF090);
    RH_ScopedVMTInstall(GetLifeTime, 0x4AF080);
    RH_ScopedVMTInstall(Clone, 0x4B72F0);
    RH_ScopedVMTInstall(AffectsPed, 0x4AF100);
}
