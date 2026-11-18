#include "StdInc.h"

#include "TaskComplexLeaveCar.h"
#include "TaskSimpleCarWaitToSlowDown.h"

void CTaskComplexLeaveCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexLeaveCar, 0x86e828, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x62F1A0, "1", {}, CVehicle*, int32, int32);
    RH_ScopedConstructorInstall(0x63B8C0, "2", {}, CVehicle*, int32, int32, bool, bool);
    RH_ScopedDestructorInstall(0x63B970);

    RH_ScopedInstall(ComputeTargetDoor, 0x63BAB0, { .Reversed = false });
    RH_ScopedInstall(CreateSubTask, 0x641530, { .Reversed = false });
    RH_ScopedVMTInstall(Clone, 0x63D9E0);
    RH_ScopedVMTInstall(GetTaskType, 0x63B960);
    RH_ScopedVMTInstall(MakeAbortable, 0x641100, { .Reversed = false });
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6419F0, { .Reversed = false });
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x641FC0, { .Reversed = false });
    RH_ScopedVMTInstall(ControlSubTask, 0x6421B0, { .Reversed = false });
}

// 0x62F1A0
CTaskComplexLeaveCar::CTaskComplexLeaveCar(CVehicle* targetVehicle, int32 nTargetDoor, int32 nDelayTime) :
    CTaskComplexLeaveCar(targetVehicle, nTargetDoor, nDelayTime, false, true) {
    m_bDie = true;
}

// 0x63B8C0
CTaskComplexLeaveCar::CTaskComplexLeaveCar(CVehicle* targetVehicle, int32 nTargetDoor, int32 nDelayTime, bool bSensibleLeaveCar, bool bForceGetOut) :
    m_nTargetDoor{ nTargetDoor },
    m_nDelayTime{ nDelayTime },
    m_bSensibleLeaveCar{ bSensibleLeaveCar },
    m_pTargetVehicle{ targetVehicle },
    m_bForceGetOut{ bForceGetOut } {
}

// 0x63D9E0
CTaskComplexLeaveCar::CTaskComplexLeaveCar(const CTaskComplexLeaveCar& o) :
    CTaskComplexLeaveCar{ o.m_pTargetVehicle, o.m_nTargetDoor, o.m_nDelayTime, o.m_bSensibleLeaveCar, o.m_bForceGetOut } {
}

// 0x63B970
CTaskComplexLeaveCar::~CTaskComplexLeaveCar() {
    if (m_pTargetVehicle) {
        m_pTargetVehicle->ClearGettingOutFlags(m_nDoorFlagsSet);
        m_pTargetVehicle->m_nNumGettingIn -= m_nNumGettingInSet;
    }
}

// 0x63BAB0
void CTaskComplexLeaveCar::ComputeTargetDoor(CPed* ped) {
    return plugin::CallMethodAndReturn<void, 0x63BAB0, CTaskComplexLeaveCar*, CPed*>(this, ped);
}

// 0x641100
bool CTaskComplexLeaveCar::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return plugin::CallMethodAndReturn<bool, 0x641100, CTask*, CPed*, int32, const CEvent*>(this, ped, priority, event);
}

// 0x6419F0
CTask* CTaskComplexLeaveCar::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x6419F0, CTask*, CPed*>(this, ped);
}

// 0x641FC0
CTask* CTaskComplexLeaveCar::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x641FC0, CTask*, CPed*>(this, ped);
}

// 0x6421B0
CTask* CTaskComplexLeaveCar::ControlSubTask(CPed* ped) {
    if (!m_pTargetVehicle) {
        return nullptr;
    }

    if (ped->bInVehicle) {
        switch (m_pSubTask->GetTaskType()) {
        case TASK_SIMPLE_CAR_SET_PED_OUT:
        case TASK_SIMPLE_CAR_JUMP_OUT:
        case TASK_COMPLEX_GET_UP_AND_STAND_STILL:
        case TASK_SIMPLE_DIE:                     {
            if (!m_bSensibleLeaveCar && notsa::isa<CTaskSimpleCarWaitToSlowDown>(m_pSubTask)) {
                m_pSubTask->MakeAbortable(ped);
            }
            return m_pSubTask;
        }
        }
    }

    return CreateSubTask(TASK_SIMPLE_CAR_SET_PED_OUT, ped);
}

// 0x641530
CTask* CTaskComplexLeaveCar::CreateSubTask(eTaskType taskType, CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x641530, CTaskComplexLeaveCar*, eTaskType, CPed*>(this, taskType, ped);
}
