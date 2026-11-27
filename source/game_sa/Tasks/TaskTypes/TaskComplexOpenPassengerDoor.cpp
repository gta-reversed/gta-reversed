#include "StdInc.h"

#include "TaskComplexOpenPassengerDoor.h"

void CTaskComplexOpenPassengerDoor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexOpenPassengerDoor, 0x86eb3c, 12);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x6403E0, "1", {}, CVehicle*, int32);
    RH_ScopedDestructorInstall(0x640420);

    RH_ScopedVMTInstall(Clone, 0x6438E0);
    RH_ScopedVMTInstall(GetTaskType, 0x640410);
}

// NOTSA
CTaskComplexOpenPassengerDoor::CTaskComplexOpenPassengerDoor(const CTaskComplexOpenPassengerDoor& o) :
    CTaskComplexOpenPassengerDoor{ o.m_Car, o.m_TargetSeat } {
    m_MoveState = o.m_MoveState;
}
