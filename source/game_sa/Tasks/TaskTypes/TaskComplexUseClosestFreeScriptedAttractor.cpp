#include "StdInc.h"
#include "TaskComplexUseClosestFreeScriptedAttractor.h"
#include "TaskComplexUseEffect.h"
#include "TaskComplexUseEffectRunning.h"
#include "TaskComplexUseEffectSprinting.h"

void CTaskComplexUseClosestFreeScriptedAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseClosestFreeScriptedAttractor, 0x86e428, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6346F0);
    RH_ScopedInstall(Destructor, 0x634720);

    RH_ScopedGlobalInstall(ComputeClosestFreeScriptedEffect, 0x634740, {.reversed = false});

    RH_ScopedVMTInstall(Clone, 0x636F70);
    RH_ScopedVMTInstall(GetTaskType, 0x634710);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x634730);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x639530);
    RH_ScopedVMTInstall(ControlSubTask, 0x634890);
}

CTaskComplexUseClosestFreeScriptedAttractor::CTaskComplexUseClosestFreeScriptedAttractor(eMoveState ms) :
    m_MoveState{ms}
{
}

CTaskComplexUseClosestFreeScriptedAttractor::CTaskComplexUseClosestFreeScriptedAttractor(const CTaskComplexUseClosestFreeScriptedAttractor&) :
    CTaskComplexUseClosestFreeScriptedAttractor{}
{
}

// 0x634740
C2dEffect* CTaskComplexUseClosestFreeScriptedAttractor::ComputeClosestFreeScriptedEffect(CPed const& ped) {
    return plugin::CallAndReturn<C2dEffect*, 0x634740, CPed const&>(ped);
}

// 0x639530
CTask* CTaskComplexUseClosestFreeScriptedAttractor::CreateFirstSubTask(CPed* ped) {
    const auto fx = reinterpret_cast<C2dEffectPedAttractor*>(ComputeClosestFreeScriptedEffect(*ped));
    if (!fx) {
        return nullptr;
    }

    switch (m_MoveState) {
    case PEDMOVE_WALK:
        return new CTaskComplexUseEffect{ fx, nullptr };
    case PEDMOVE_RUN:
        return new CTaskComplexUseEffectRunning{ fx, nullptr };
    case PEDMOVE_SPRINT:
        return new CTaskComplexUseEffectSprinting{ fx, nullptr };
    default:
        return new CTaskComplexUseEffect{ fx, nullptr };
    }
}
