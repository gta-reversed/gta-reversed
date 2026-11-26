#include "StdInc.h"
#include "TaskComplexLeaveBoat.h"
#include "TaskSimpleCarSetPedOut.h"

void CTaskComplexLeaveBoat::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexLeaveBoat, 0x86e854, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63BB60);
    RH_ScopedInstall(Destructor, 0x63BBD0);

    RH_ScopedInstall(CreateSubTask, 0x63BC50);

    RH_ScopedVMTInstall(Clone, 0x63DA60);
    RH_ScopedVMTInstall(GetTaskType, 0x63BBC0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x63BC30);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x642270);
    RH_ScopedVMTInstall(ControlSubTask, 0x63BC40);
}

// 0x63BB60
CTaskComplexLeaveBoat::CTaskComplexLeaveBoat(CVehicle* vehicle, uint32 delay) :
    m_veh{vehicle},
    m_delayTimeMs{delay}
{
    CEntity::SafeRegisterRef(m_veh);
}

CTaskComplexLeaveBoat::CTaskComplexLeaveBoat(const CTaskComplexLeaveBoat& o) :
    CTaskComplexLeaveBoat{o.m_veh, o.m_delayTimeMs}
{
}

// 0x63BBD0
CTaskComplexLeaveBoat::~CTaskComplexLeaveBoat() {
    CEntity::SafeCleanUpRef(m_veh);
}

// 0x63BC50
CTask* CTaskComplexLeaveBoat::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_SIMPLE_CAR_SET_PED_OUT: {
        return new CTaskSimpleCarSetPedOut{ m_veh, TARGET_DOOR_FRONT_LEFT, true };
    }
    }
    return nullptr;
}

// 0x63BC30
CTask* CTaskComplexLeaveBoat::CreateNextSubTask(CPed* ped) {
    return nullptr;
}

// 0x642270
CTask* CTaskComplexLeaveBoat::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(TASK_SIMPLE_CAR_SET_PED_OUT);
}

// 0x63BC40
CTask* CTaskComplexLeaveBoat::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}
