#include "StdInc.h"
#include "TaskComplexPlayHandSignalAnim.h"

void CTaskComplexPlayHandSignalAnim::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexPlayHandSignalAnim, 0x86d5dc, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x61B2B0);
    RH_ScopedInstall(Destructor, 0x61BDF0, { .reversed = false });

    RH_ScopedInstall(GetAnimIdForPed, 0x61B460, {.reversed = false});
    RH_ScopedInstall(CreateSubTask, 0x61B2F0, {.reversed = false});

    RH_ScopedVMTInstall(Clone, 0x61BA00, {.reversed = false});
    RH_ScopedVMTInstall(GetTaskType, 0x61B2E0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x61B570, {.reversed = false});
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x61B4F0, {.reversed = false});
    RH_ScopedVMTInstall(ControlSubTask, 0x61B580, {.reversed = false});
}

// 0x61B2B0
CTaskComplexPlayHandSignalAnim::CTaskComplexPlayHandSignalAnim(AnimationId animationId, float blendFactor) :
    m_animationId{animationId},
    m_fBlendFactor{blendFactor}
{
}

// 0x61BDF0
CTaskComplexPlayHandSignalAnim::~CTaskComplexPlayHandSignalAnim() {
    DebugBreak(); // Unhooked in InjectHooks, so never reached.
}

// 0x61B460
AnimationId CTaskComplexPlayHandSignalAnim::GetAnimIdForPed(CPed* ped) {
    return plugin::CallAndReturn<AnimationId, 0x61B460, CPed*>(ped);
}

// 0x61B2F0
CTask* CTaskComplexPlayHandSignalAnim::CreateSubTask(eTaskType taskType) {
    return plugin::CallMethodAndReturn<CTask*, 0x61B2F0>(this, taskType);
}

// 0x61BA00
CTask* CTaskComplexPlayHandSignalAnim::Clone() {
    return plugin::CallMethodAndReturn<CTask*, 0x61BA00, CTaskComplexPlayHandSignalAnim*>(this);
}

// 0x61B570
CTask* CTaskComplexPlayHandSignalAnim::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x61B570, CTaskComplexPlayHandSignalAnim*, CPed*>(this, ped);
}

// 0x61B4F0
CTask* CTaskComplexPlayHandSignalAnim::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x61B4F0, CTaskComplexPlayHandSignalAnim*, CPed*>(this, ped);
}

// 0x61B580
CTask* CTaskComplexPlayHandSignalAnim::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x61B580, CTaskComplexPlayHandSignalAnim*, CPed*>(this, ped);
}
