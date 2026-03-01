#include "StdInc.h"

#include "TaskComplexGetOnBoatSeat.h"
#include "TaskSimpleRunAnim.h"

void CTaskComplexGetOnBoatSeat::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGetOnBoatSeat, 0x86f044, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x649210);
    RH_ScopedInstall(Destructor, 0x649280);

    RH_ScopedVMTInstall(Clone, 0x64A3B0);
    RH_ScopedVMTInstall(GetTaskType, 0x649270);
    RH_ScopedVMTInstall(MakeAbortable, 0x6492E0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x649300);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x649310);
    RH_ScopedVMTInstall(ControlSubTask, 0x6493D0);
}

// 0x649210
CTaskComplexGetOnBoatSeat::CTaskComplexGetOnBoatSeat(CVehicle* vehicle) :
    m_Vehicle{ vehicle }
{
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x64A3B0
CTaskComplexGetOnBoatSeat::CTaskComplexGetOnBoatSeat(const CTaskComplexGetOnBoatSeat& o) :
    CTaskComplexGetOnBoatSeat{ o.m_Vehicle }
{
}

// 0x649280
CTaskComplexGetOnBoatSeat::~CTaskComplexGetOnBoatSeat() {
    CEntity::SafeCleanUpRef(m_Vehicle);
}

// 0x6492E0
bool CTaskComplexGetOnBoatSeat::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return priority == ABORT_PRIORITY_IMMEDIATE && m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x649310
CTask* CTaskComplexGetOnBoatSeat::CreateFirstSubTask(CPed* ped) {
    return new CTaskSimpleRunAnim{ ANIM_GROUP_DEFAULT, m_Vehicle->m_pHandlingData->m_bSitInBoat ? ANIM_ID_CAR_SIT : ANIM_ID_DRIVE_BOAT, 1.f };
}

// 0x6493D0
CTask* CTaskComplexGetOnBoatSeat::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}
