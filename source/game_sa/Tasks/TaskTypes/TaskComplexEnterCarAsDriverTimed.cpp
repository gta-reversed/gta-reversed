#include "StdInc.h"
#include "TaskComplexEnterCarAsDriverTimed.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskSimpleCarSetPedInAsDriver.h"

void CTaskComplexEnterCarAsDriverTimed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexEnterCarAsDriverTimed, 0x86e720, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63AD70);
    RH_ScopedInstall(Destructor, 0x63AE00);

    RH_ScopedVMTInstall(Clone, 0x63D780);
    RH_ScopedVMTInstall(GetTaskType, 0x63ADF0);
    RH_ScopedVMTInstall(StopTimer, 0x63AFF0);
    RH_ScopedVMTInstall(MakeAbortable, 0x63AE60);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x63AED0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6405B0);
    RH_ScopedVMTInstall(ControlSubTask, 0x63AEE0);
}

// 0x63AD70
CTaskComplexEnterCarAsDriverTimed::CTaskComplexEnterCarAsDriverTimed(CVehicle* veh, int32 time) :
    m_veh{veh},
    m_enterMaxInterval{time}
{
    CEntity::SafeRegisterRef(m_veh);
}

CTaskComplexEnterCarAsDriverTimed::CTaskComplexEnterCarAsDriverTimed(const CTaskComplexEnterCarAsDriverTimed& o) :
    CTaskComplexEnterCarAsDriverTimed{ o.m_veh, o.m_enterMaxInterval }
{
    m_moveState = o.m_moveState;
}

// 0x63AE00
CTaskComplexEnterCarAsDriverTimed::~CTaskComplexEnterCarAsDriverTimed() {
    CEntity::SafeCleanUpRef(m_veh);
}

// 0x63AFF0
void CTaskComplexEnterCarAsDriverTimed::StopTimer(CEvent const* event) {
    if (!CEventHandler::IsTemporaryEvent(*event)) {
        m_enterBeganTimer.Pause();
    }
}

// 0x63AE60
bool CTaskComplexEnterCarAsDriverTimed::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (m_pSubTask->MakeAbortable(ped, priority, event)) {
        if (!event || !CEventHandler::IsTemporaryEvent(*event)) {
            switch (priority) {
            case ABORT_PRIORITY_URGENT:
            case ABORT_PRIORITY_IMMEDIATE:
                m_enterBeganTimer.Pause();
            }
        }
        return true;
    }
    return false;
}

// 0x6405B0
CTask* CTaskComplexEnterCarAsDriverTimed::CreateFirstSubTask(CPed* ped) {
    m_enterBeganTimer.Start(m_enterMaxInterval);
    return new CTaskComplexEnterCarAsDriver{ m_veh, m_moveState };
}

// 0x63AEE0
CTask* CTaskComplexEnterCarAsDriverTimed::ControlSubTask(CPed* ped) {
    if (!m_veh) {
        return nullptr;
    }

    if (!m_enterBeganTimer.IsOutOfTime()) {
        return m_pSubTask;
    }

    // Check if ped has got into the vehicle yet
    if (!m_pSubTask->MakeAbortable(ped) || ped->bInVehicle || m_veh->m_pDriver) {
        return m_pSubTask;
    }

    // Noep, still not in th evehicle, warp them into it
    return new CTaskSimpleCarSetPedInAsDriver{ m_veh, true };
}
