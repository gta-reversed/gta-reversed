#include "StdInc.h"

#include "TaskSimpleCarDriveTimed.h"

// 0x5FF940
CTaskSimpleCarDriveTimed::CTaskSimpleCarDriveTimed(CVehicle* vehicle, int32 nTime) :
    CTaskSimpleCarDrive(vehicle, nullptr, false),
    m_nTimer() {
    m_nTime = nTime;
}

// 0x46F610
bool CTaskSimpleCarDriveTimed::ProcessPed(CPed* ped) {
    m_nTimer.StartIfNotAlready(m_nTime);
    if (m_nTimer.IsOutOfTime()) {
        return true;
    }
    return CTaskSimpleCarDrive::ProcessPed(ped);
}

void CTaskSimpleCarDriveTimed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarDriveTimed, 0x859e50, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x5FF940, "1", {}, CVehicle*, int32);
    RH_ScopedDestructorInstall(0x46F690);

    RH_ScopedVMTInstall(Clone, 0x46F570);
    RH_ScopedVMTInstall(GetTaskType, 0x46F600);
    RH_ScopedVMTInstall(ProcessPed, 0x46F610);
}
