#include "StdInc.h"
#include "TaskComplexWaitForDryWeather.h"
#include "TaskSimpleStandStill.h"
#include "Weather.h"

void CTaskComplexWaitForDryWeather::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWaitForDryWeather, 0x86e160, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x632800);
    RH_ScopedInstall(Destructor, 0x632830);

    RH_ScopedVMTInstall(Clone, 0x636A10);
    RH_ScopedVMTInstall(GetTaskType, 0x632820);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x632840);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x632860);
    RH_ScopedVMTInstall(ControlSubTask, 0x6328F0);
}

CTaskComplexWaitForDryWeather::CTaskComplexWaitForDryWeather(const CTaskComplexWaitForDryWeather& o) :
    CTaskComplexWaitForDryWeather{}
{
}

// 0x632840
CTask* CTaskComplexWaitForDryWeather::CreateNextSubTask(CPed* ped) {
    return CWeather::Rain >= 0.2f
            ? CreateFirstSubTask(ped)
            : nullptr;
}   

// 0x632860
CTask* CTaskComplexWaitForDryWeather::CreateFirstSubTask(CPed* ped) {
    return new CTaskSimpleStandStill{ 1000 };
}
