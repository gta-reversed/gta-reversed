#include "StdInc.h"
#include "TaskInteriorSitOnChair.h"

#include "Interior/InteriorInfo_t.h"
#include "Interior/Interior_c.h"
#include "Interior/InteriorManager_c.h"
#include "Ragdoll/IKChainManager.h"

void CTaskInteriorSitOnChair::InjectHooks() {
    RH_ScopedVirtualClass(CTaskInteriorSitOnChair, 0x870314, 9);
    RH_ScopedCategory("Tasks/TaskTypes/Interior");

    RH_ScopedInstall(Constructor, 0x675C30);
    RH_ScopedInstall(Destructor, 0x675C90);

    RH_ScopedInstall(FinishAnimCB, 0x675DD0, { .reversed = false });
    RH_ScopedVMTInstall(Clone, 0x675CF0, { .reversed = false });
    RH_ScopedVMTInstall(GetTaskType, 0x675C80);
    RH_ScopedVMTInstall(MakeAbortable, 0x675D60, { .reversed = false });
    RH_ScopedVMTInstall(ProcessPed, 0x676D30, { .reversed = false });
}

// 0x675C30
CTaskInteriorSitOnChair::CTaskInteriorSitOnChair(Interior_c* interior, InteriorInfo_t* interiorInfo, bool bDoInstantly) :
    m_Interior{interior},
    m_InteriorInfo{interiorInfo},
    m_bDoInstantly{bDoInstantly}
{
}

// 0x675CF0
CTaskInteriorSitOnChair::CTaskInteriorSitOnChair(const CTaskInteriorSitOnChair& o) :
    CTaskInteriorSitOnChair{o.m_Interior, o.m_InteriorInfo, o.m_bDoInstantly}
{
}

// 0x675C90
CTaskInteriorSitOnChair::~CTaskInteriorSitOnChair() {
    if (m_Anim) {
        m_Anim->SetDefaultFinishCallback();
    }
}

// 0x675DD0
void CTaskInteriorSitOnChair::FinishAnimCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = notsa::cast<CTaskInteriorSitOnChair>(static_cast<CTask*>(data));

    self->m_PrevAnimId = anim->GetAnimId();

    if (self->m_PrevAnimId == ANIM_ID_LOU_OUT // Last animation in the sequence
     || self->m_bTaskAborting && self->m_PrevAnimId == ANIM_ID_LOU_IN
    ) {
        anim->SetBlendDelta(-1000.f);
        self->m_bTaskFinished = true;
    }

    self->m_Anim = nullptr;
}


// 0x675D60
bool CTaskInteriorSitOnChair::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    return plugin::CallMethodAndReturn<bool, 0x675D60, CTaskInteriorSitOnChair*, CPed*, eAbortPriority, const CEvent*>(this, ped, priority, event);
}

// 0x676D30
bool CTaskInteriorSitOnChair::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x676D30, CTaskInteriorSitOnChair*, CPed*>(this, ped);
}
