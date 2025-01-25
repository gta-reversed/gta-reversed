#include "StdInc.h"

#include "TaskAllocator.h"

// 0x69D980
void* CTaskAllocator::operator new(size_t size) {
    return GetTaskAllocatorPool()->New();
}

// 0x69D990
void CTaskAllocator::operator delete(void* obj) {
    GetTaskAllocatorPool()->Delete(static_cast<CTaskAllocator*>(obj));
}

bool CTaskAllocator::IsFinished(CPedGroupIntelligence* intel) {
    return rng::all_of(
        intel->GetPedTaskPairs(),
        [](const CPedTaskPair& tp) { return !tp.m_Task; }
    );
}

void CTaskAllocator::InjectHooks() {
    RH_ScopedVirtualClass(CTaskAllocator, 0x870E30, 6);
    RH_ScopedCategory("Tasks/Allocators/TaskAllocator");

    RH_ScopedVMTInstall(ProcessGroup, 0x69BB50);
    RH_ScopedVMTInstall(IsFinished, 0x69C3C0);
    RH_ScopedVMTInstall(Abort, 0x5F68E0);
}

CTaskAllocator::CTaskAllocator(CPed* ped) :
    m_Ped0{ped}
{ }
