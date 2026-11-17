#include "StdInc.h"

#include "TaskComplexOpenDriverDoor.h"

CTaskComplexOpenDriverDoor::CTaskComplexOpenDriverDoor(CVehicle* vehicle) :
    CTaskComplexEnterCar(vehicle, true, true, false, false) {
    m_TargetSeat = 10;
}

CTaskComplexOpenDriverDoor::CTaskComplexOpenDriverDoor(const CTaskComplexOpenDriverDoor& o) :
    CTaskComplexOpenDriverDoor{ o.m_Car } {
    m_MoveState = o.m_MoveState;
}

void CTaskComplexOpenDriverDoor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexOpenDriverDoor, 0x86eb0c, 12);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x640390, "1", {}, CVehicle*);
    RH_ScopedDestructorInstall(0x6403D0);

    RH_ScopedVMTInstall(Clone, 0x643870);
    RH_ScopedVMTInstall(GetTaskType, 0x6403C0);
}
