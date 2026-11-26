#include "StdInc.h"
#include "TaskInteriorSitInChair.h"

// 0x675C30
CTaskInteriorSitInChair::CTaskInteriorSitInChair(int32 getOutOfChairInterval, InteriorInfo_t* interiorInfo, bool sitInChairInstantly) :
    m_getOutOfChairInterval{ getOutOfChairInterval },
    m_pInteriorInfo{ interiorInfo },
    m_sitInChairInstantly{ sitInChairInstantly } {
}

// 0x675CF0
CTaskInteriorSitInChair::CTaskInteriorSitInChair(const CTaskInteriorSitInChair& o) :
    CTaskInteriorSitInChair{
        o.m_getOutOfChairInterval,
        o.m_pInteriorInfo,
        o.m_sitInChairInstantly
    } {
}

// 0x675C90
CTaskInteriorSitInChair::~CTaskInteriorSitInChair() {
    if (auto* const anim = std::exchange(m_pAnim, nullptr)) {
        anim->SetDefaultDeleteCallback();
    }
}

// 0x675DD0
CTaskInteriorSitInChair* CTaskInteriorSitInChair::FinishAnimCB(CAnimBlendAssociation* anim, void* self) {
    return plugin::CallAndReturn<CTaskInteriorSitInChair*, 0x675DD0, CAnimBlendAssociation*, void*>(anim, self);
}

// 0x675D60
bool CTaskInteriorSitInChair::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return plugin::CallMethodAndReturn<bool, 0x675D60, CTaskInteriorSitInChair*, CPed*, int32, const CEvent*>(this, ped, priority, event);
}

// 0x676D30
bool CTaskInteriorSitInChair::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x676D30, CTaskInteriorSitInChair*, CPed*>(this, ped);
}

void CTaskInteriorSitInChair::InjectHooks() {
    RH_ScopedVirtualClass(CTaskInteriorSitInChair, 0x870314, 9);
    RH_ScopedCategory("Tasks/TaskTypes/Interior");

    RH_ScopedConstructorInstall(0x675C30, "1", {}, int32, InteriorInfo_t*, bool);
    RH_ScopedDestructorInstall(0x675C90);

    RH_ScopedGlobalInstall(FinishAnimCB, 0x675DD0, { .Reversed = false });
    RH_ScopedVMTInstall(Clone, 0x675CF0);
    RH_ScopedVMTInstall(GetTaskType, 0x675C80);
    RH_ScopedVMTInstall(MakeAbortable, 0x675D60, { .Reversed = false });
    RH_ScopedVMTInstall(ProcessPed, 0x676D30, { .Reversed = false });
}
