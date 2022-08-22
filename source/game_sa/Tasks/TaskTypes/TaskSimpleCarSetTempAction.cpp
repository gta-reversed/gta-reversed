#include "StdInc.h"

#include "TaskSimpleCarSetTempAction.h"

void CTaskSimpleCarSetTempAction::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarSetTempAction, 0x86ea6c, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63D6F0);
    RH_ScopedInstall(Destructor, 0x63D730);
    RH_ScopedVMTInstall(Clone, 0x63DF10);
    RH_ScopedVMTInstall(GetTaskType, 0x63D720);
    RH_ScopedVMTInstall(MakeAbortable, 0x63D740);
    RH_ScopedVMTInstall(ProcessPed, 0x645370);
}

// 0x63D6F0
CTaskSimpleCarSetTempAction::CTaskSimpleCarSetTempAction(CVehicle* vehicle, uint32 action, uint32 timeMs) :
    CTaskSimpleCarDrive{ vehicle },
    m_Action{ action },
    m_DurationMs{ timeMs }
{
}

// 0x63D740
bool CTaskSimpleCarSetTempAction::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (CTaskSimpleCarDrive::MakeAbortable(ped, priority, event)) {
        if (m_pVehicle) {
            m_pVehicle->m_autoPilot.ClearTempAction();
        }
        return true;
    }
    return false;
}

// 0x645370
bool CTaskSimpleCarSetTempAction::ProcessPed(CPed* ped) {
    if (!m_pVehicle) {
        m_pVehicle = ped->m_pVehicle;
        CEntity::SafeRegisterRef(m_pVehicle);
    }

    if (CTaskSimpleCarDrive::ProcessPed(ped)) {
        if (m_pVehicle) {
            m_pVehicle->m_autoPilot.SetTempAction(0, 0);
            return true;
        }
    }

    if (m_pVehicle) {
        if (m_Action != 0) {
            m_pVehicle->m_autoPilot.SetTempAction(m_Action, m_DurationMs);
            m_Action = 0; // eACTION_TYPE::FIRST_PERSON?
            return false;
        }
        if (m_pVehicle && m_pVehicle->m_autoPilot.m_nTempAction == 0) { // TODO: Inlined? Double null check of `m_pVehicle`
            return true;
        }
    }

    return false;
}
