#include "StdInc.h"

#include "TaskSimpleHitHead.h"

void CTaskSimpleHitHead::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHitHead, 0x86F1D0, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x653060);
    RH_ScopedInstall(FinishAnimCB, 0x653150);
    RH_ScopedVMTInstall(MakeAbortable, 0x6530F0);
    RH_ScopedVMTInstall(ProcessPed, 0x657A10);
}
CTaskSimpleHitHead* CTaskSimpleHitHead::Constructor() { this->CTaskSimpleHitHead::CTaskSimpleHitHead(); return this; }

// 0x653060
CTaskSimpleHitHead::CTaskSimpleHitHead() : CTaskSimple() {
    m_bIsFinished = false;
    m_pAnim = nullptr;
}

// 0x653090
CTaskSimpleHitHead::~CTaskSimpleHitHead() {
    if (m_pAnim) {
        m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
}

// 0x653150
void CTaskSimpleHitHead::FinishAnimCB(CAnimBlendAssociation* anim, void* data) {
    auto task = reinterpret_cast<CTaskSimpleHitHead*>(data);
    task->m_pAnim = nullptr;
    task->m_bIsFinished = true;
}

// 0x6530F0


bool CTaskSimpleHitHead::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_URGENT || priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_pAnim) {
            m_pAnim->m_BlendDelta = -4.0F;
            m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_pAnim = nullptr;
        }

        m_bIsFinished = true;
        return true;
    }

    if (m_pAnim) {
        m_pAnim->m_BlendDelta = -4.0F;
    }
    return false;
}

// 0x657A10

// 0x0
bool CTaskSimpleHitHead::ProcessPed(CPed* ped) {
    if (m_bIsFinished)
        return true;

    if (!m_pAnim) {
        m_pAnim = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_HIT_WALL, 8.0F);
        m_pAnim->SetFinishCallback(FinishAnimCB, this);
    }

    return m_bIsFinished;
}