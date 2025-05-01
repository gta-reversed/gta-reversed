#include "StdInc.h"
#include "EventInAir.h"


void CEventInAir::InjectHooks()
{
    RH_ScopedVirtualClass(CEventInAir, 0x85B3C0, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B0CB0);
    RH_ScopedVMTInstall(AffectsPed, 0x4B0C00);
}

// 0x4B0CB0
CEventInAir* CEventInAir::Constructor()
{
    this->CEventInAir::CEventInAir();
    return this;
}

// 0x4B0C00
bool CEventInAir::AffectsPed(CPed* ped)
{
    CTask* activeTask = ped->GetTaskManager().GetActiveTask();
    if (activeTask
        && (activeTask->GetTaskType() == TASK_COMPLEX_USE_SWAT_ROPE
            || activeTask->GetTaskType() == TASK_COMPLEX_DIVE_FROM_ATTACHED_ENTITY_AND_GET_UP
            ||
                       activeTask->GetTaskType() == TASK_COMPLEX_IN_AIR_AND_LAND))
    {
        return false;
    }
    CTask* simplestActiveTask = ped->GetTaskManager().GetSimplestActiveTask();
    if (simplestActiveTask && simplestActiveTask->GetTaskType() == TASK_SIMPLE_FALL ||
        ped->m_pDamageEntity ||
        !ped->m_bUsesCollision ||
        ped->m_pAttachedTo ||
        !CPedGeometryAnalyser::IsInAir(*ped)
    ) {
        return false;
    }
    return ped->IsAlive();
}