#include "StdInc.h"
#include "TaskComplexSmartFleePoint.h"

void CTaskComplexSmartFleePoint::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSmartFleePoint, 0x86f744, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65BD20);
    RH_ScopedInstall(Destructor, 0x65BDB0);

    RH_ScopedInstall(SetDefaultTaskWanderDir, 0x65BE00, {.reversed = false});
    RH_ScopedInstall(ComputeFleeDir, 0x65BE40, {.reversed = false});
    RH_ScopedInstall(CreateSubTask, 0x65BE80, {.reversed = false});
    RH_ScopedInstall(SetFleePosition, 0x65C3C0, {.reversed = false});

    RH_ScopedVMTInstall(Clone, 0x65CED0);
    RH_ScopedVMTInstall(GetTaskType, 0x65BDA0);
    RH_ScopedVMTInstall(MakeAbortable, 0x65BDC0, {.reversed = false});
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65C0C0, {.reversed = false});
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65C140, {.reversed = false});
    RH_ScopedVMTInstall(ControlSubTask, 0x65C1E0, {.reversed = false});
}

// 0x65BD20
CTaskComplexSmartFleePoint::CTaskComplexSmartFleePoint(CVector const& fleePos, bool doScream, float safeDist, int32 fleeTimeMs) :
    m_fleePoint{fleePos},
    m_doScream{doScream},
    m_safeDist{safeDist},
    m_fleeTimeMs{fleeTimeMs}
{
    m_timer.Start(m_fleeTimeMs);
}

CTaskComplexSmartFleePoint::CTaskComplexSmartFleePoint(const CTaskComplexSmartFleePoint& o)
    : CTaskComplexSmartFleePoint{o.m_fleePoint, o.m_doScream, o.m_safeDist, o.m_fleeTimeMs}
{
}

// 0x65BE00
int8 CTaskComplexSmartFleePoint::SetDefaultTaskWanderDir(CPed* ped) {
    return plugin::CallMethodAndReturn<int8, 0x65BE00, CTaskComplexSmartFleePoint*, CPed*>(this, ped);
}

// 0x65BE40
uint32 CTaskComplexSmartFleePoint::ComputeFleeDir(CPed* ped) {
    return plugin::CallMethodAndReturn<uint32, 0x65BE40, CTaskComplexSmartFleePoint*, CPed*>(this, ped);
}

// 0x65BE80
CTask* CTaskComplexSmartFleePoint::CreateSubTask(eTaskType taskType, CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x65BE80, CTaskComplexSmartFleePoint*, eTaskType, CPed*>(this, taskType, ped);
}

// 0x65C3C0
int8 CTaskComplexSmartFleePoint::SetFleePosition(CVector const& a2, float a3, bool a4) {
    return plugin::CallMethodAndReturn<int8, 0x65C3C0, CTaskComplexSmartFleePoint*, CVector const&, float, bool>(this, a2, a3, a4);
}

// 0x65BDC0
bool CTaskComplexSmartFleePoint::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    return plugin::CallMethodAndReturn<bool, 0x65BDC0, CTaskComplexSmartFleePoint*, CPed*, eAbortPriority, CEvent const*>(this, ped, priority, event);
}

// 0x65C0C0
CTask* CTaskComplexSmartFleePoint::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x65C0C0, CTaskComplexSmartFleePoint*, CPed*>(this, ped);
}

// 0x65C140
CTask* CTaskComplexSmartFleePoint::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x65C140, CTaskComplexSmartFleePoint*, CPed*>(this, ped);
}

// 0x65C1E0
CTask* CTaskComplexSmartFleePoint::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x65C1E0, CTaskComplexSmartFleePoint*, CPed*>(this, ped);
}
