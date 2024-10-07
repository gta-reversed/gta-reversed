#include "StdInc.h"

#include "TaskComplexGangFollower.h"

void CTaskComplexGangFollower::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGangFollower, 0x86F938, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65EAA0, { .reversed = false });
    RH_ScopedInstall(Destructor, 0x65EBB0, { .reversed = false });
    RH_ScopedInstall(CalculateOffsetPosition, 0x65ED40, { .reversed = false });
    RH_ScopedInstall(Clone, 0x65ECB0, { .reversed = false });
    RH_ScopedInstall(MakeAbortable, 0x65EC30, { .reversed = false });
    RH_ScopedInstall(CreateNextSubTask, 0x665E00, { .reversed = false });
    RH_ScopedInstall(CreateFirstSubTask, 0x666160, { .reversed = false });
    RH_ScopedInstall(ControlSubTask, 0x662A10, { .reversed = false });
}

// 0x65EAA0
CTaskComplexGangFollower::CTaskComplexGangFollower(CPedGroup* pedGroup, CPed* ped, uint8 a4, CVector pos, float a6) : CTaskComplex() {
    plugin::CallMethod<0x65EAA0, CTaskComplexGangFollower*, CPedGroup*, CPed*, uint8, CVector, float>(this, pedGroup, ped, a4, pos, a6);
}


// 0x65EBB0
CTaskComplexGangFollower::~CTaskComplexGangFollower() {
    plugin::CallMethod<0x65EBB0, CTaskComplexGangFollower*>(this);
}

// 0x65ED40
void CTaskComplexGangFollower::CalculateOffsetPosition(CVector& pos) {
    plugin::CallMethod<0x65ED40, CTaskComplexGangFollower*, CVector&>(this, pos);
}

// 0x65ECB0
CTask* CTaskComplexGangFollower::Clone() const {
    return plugin::CallMethodAndReturn<CTask*, 0x65ECB0, const CTaskComplexGangFollower*>(this);
}

// 0x65EC30
bool CTaskComplexGangFollower::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return plugin::CallMethodAndReturn<bool, 0x65EC30, CTaskComplexGangFollower*, CPed*, int32, CEvent const*>(this, ped, priority, event);
}

// 0x665E00
CTask* CTaskComplexGangFollower::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x665E00, CTaskComplexGangFollower*, CPed*>(this, ped);
}

// 0x666160
CTask* CTaskComplexGangFollower::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x666160, CTaskComplexGangFollower*, CPed*>(this, ped);
}

// 0x662A10
CTask* CTaskComplexGangFollower::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x662A10, CTaskComplexGangFollower*, CPed*>(this, ped);
}
