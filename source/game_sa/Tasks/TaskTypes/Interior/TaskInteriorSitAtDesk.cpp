#include "StdInc.h"
#include "TaskInteriorSitAtDesk.h"

void CTaskInteriorSitAtDesk::InjectHooks() {
    RH_ScopedVirtualClass(CTaskInteriorSitAtDesk, 0x87035c, 9);
    RH_ScopedCategory("Tasks/TaskTypes/Interior");

    RH_ScopedInstall(Constructor, 0x676010);
    RH_ScopedInstall(Destructor, 0x676080);

    RH_ScopedInstall(FinishAnimCB, 0x676190);
    RH_ScopedInstall(StartRandomLoopAnim, 0x677780);
    RH_ScopedInstall(StartRandomOneOffAnim, 0x677880);
    RH_ScopedVMTInstall(Clone, 0x6760E0);
    RH_ScopedVMTInstall(GetTaskType, 0x676070);
    RH_ScopedVMTInstall(MakeAbortable, 0x676150);
    RH_ScopedVMTInstall(ProcessPed, 0x677920, { .reversed = false });
}

// 0x676010
CTaskInteriorSitAtDesk::CTaskInteriorSitAtDesk(int32 duration, InteriorInfo_t* interiorInfo, bool bDoInstantly) :
    m_bDoInstantly{bDoInstantly},
    m_InteriorInfo{interiorInfo},
    m_Duration{duration}
{
}

// 0x6760E0
CTaskInteriorSitAtDesk::CTaskInteriorSitAtDesk(const CTaskInteriorSitAtDesk& o) :
    CTaskInteriorSitAtDesk{o.m_Duration, o.m_InteriorInfo, o.m_bDoInstantly}
{
}

// 0x676080
CTaskInteriorSitAtDesk::~CTaskInteriorSitAtDesk() {
    if (m_Anim) {
        m_Anim->SetDefaultFinishCallback();
    }
}

// 0x676190
void CTaskInteriorSitAtDesk::FinishAnimCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = CTask::Cast<CTaskInteriorSitAtDesk>(static_cast<CTask*>(data));

    assert(self && anim);
    assert(anim == self->m_Anim);

    if (anim->m_AnimId == ANIM_ID_OFF_SIT_2IDLE_180 || self->m_bTaskAborting && self->m_PrevAnimId == ANIM_ID_OFF_SIT_IN) {
        anim->SetBlendDelta(-1000.f);
        self->m_bTaskFinished = true;
    }

    self->m_Anim = nullptr;
}

// 0x677780
void CTaskInteriorSitAtDesk::StartRandomLoopAnim(CPed* ped, float blendDelta) {
    using CGeneral::GetRandomNumberInRange;

    const auto chance = GetRandomNumberInRange(100u);
    StartAnim(
        ped,
        chance > 40
            ? ANIM_ID_OFF_SIT_TYPE_LOOP
            : chance > 10
                ? ANIM_ID_OFF_SIT_BORED_LOOP
                : ANIM_ID_OFF_SIT_IDLE_LOOP,
        blendDelta
    );
    m_AnimTimer.Start(chance > 40 ? GetRandomNumberInRange(2000, 5000) : GetRandomNumberInRange(7000, 12000));
}

// 0x677880
void CTaskInteriorSitAtDesk::StartRandomOneOffAnim(CPed* ped) {
    const auto chance = CGeneral::GetRandomNumberInRange(100u);
    StartAnim(
        ped,
        chance > 60
            ? ANIM_ID_OFF_SIT_READ
            : chance > 35
                ? ANIM_ID_OFF_SIT_DRINK
                : chance > 10
                    ? ANIM_ID_OFF_SIT_WATCH
                    : ANIM_ID_OFF_SIT_CRASH,
        4.f
    );
}

// 0x676150
bool CTaskInteriorSitAtDesk::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_Anim) {
            m_Anim->SetBlendDelta(-1000.f);
            m_Anim->SetDefaultFinishCallback();
            m_Anim = nullptr;
        }
        return true;
    }
    m_bTaskAborting = true;
    return false;
}

// 0x677920
bool CTaskInteriorSitAtDesk::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x677920, CTaskInteriorSitAtDesk*, CPed*>(this, ped);
}

void CTaskInteriorSitAtDesk::StartAnim(CPed* ped, AnimationId animId, float blendDelta) {
    if (m_Anim) {
        m_Anim->SetDefaultFinishCallback();
    }
    m_Anim = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_INT_OFFICE, animId, blendDelta);
    m_Anim->SetFinishCallback(FinishAnimCB, this);
}
