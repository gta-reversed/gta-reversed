#include "StdInc.h"

#include "TaskSimpleCarDriveTimed.h"

CTaskSimpleCarDriveTimed* CTaskSimpleCarDriveTimed::Constructor(CVehicle* vehicle, int32 nTime) {
    this->CTaskSimpleCarDriveTimed::CTaskSimpleCarDriveTimed(vehicle, nTime);
    return this;
}

// 0x5FF940
CTaskSimpleCarDriveTimed::CTaskSimpleCarDriveTimed(CVehicle* vehicle, int32 nTime) : CTaskSimpleCarDrive(vehicle, nullptr, false), m_nTimer() {
    m_nTime = nTime;
}

bool CTaskSimpleCarDriveTimed::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x46F610, CTaskSimpleCarDriveTimed*, CPed*>(this, ped);
}

void CTaskSimpleCarDriveTimed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarDriveTimed, 0x859e50, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x5FF940);
    RH_ScopedInstall(Destructor, 0x46F690);

    RH_ScopedVMTInstall(Clone, 0x46F570);
    RH_ScopedVMTInstall(GetTaskType, 0x46F600);
    RH_ScopedVMTInstall(ProcessPed, 0x46F610, { .reversed = false });
}
