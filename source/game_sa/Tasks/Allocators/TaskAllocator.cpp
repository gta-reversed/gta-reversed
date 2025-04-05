#include "StdInc.h"

#include "TaskAllocator.h"

// 0x69D980
void* CTaskAllocator::operator new(size_t size) {
    return CPools::GetTaskAllocatorPool()->New();
}

// 0x69D990
void CTaskAllocator::operator delete(void* obj) {
    CPools::GetTaskAllocatorPool()->Delete(static_cast<CTaskAllocator*>(obj));
}

CTaskAllocator::CTaskAllocator(CPed* ped) {
    m_Ped0 = ped;
    m_GroupId = 0;
    m_Ped1 = nullptr;
    m_Time = 0; // bad
}

// 0x69BB50
void CTaskAllocator::ProcessGroup(CPedGroupIntelligence* intel) {
    // NOP
}

// 0x69C3C0
bool CTaskAllocator::IsFinished(CPedGroupIntelligence* intel) {
    return plugin::CallMethodAndReturn<bool, 0x69C3C0, CTaskAllocator*, CPedGroupIntelligence*>(this, intel);
}

// 0x5F68E0
void CTaskAllocator::Abort() {
    // NOP
}
