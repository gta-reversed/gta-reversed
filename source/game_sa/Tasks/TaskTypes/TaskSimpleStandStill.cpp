#include "StdInc.h"

#include "TaskSimpleStandStill.h"
#include "TaskSimpleDuck.h"

void CTaskSimpleStandStill::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleStandStill, 0x86DD2C, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x62F310);
    RH_ScopedVMTInstall(MakeAbortable, 0x4B8690);
    RH_ScopedVMTInstall(ProcessPed, 0x62F370);
}

// 0x62F310
CTaskSimpleStandStill::CTaskSimpleStandStill(int32 nTime, bool Looped, bool bUseAnimIdleStance, float fBlendData) {
    m_nTime = nTime;
    m_fBlendData = fBlendData;
    m_bLooped = Looped;
    m_bUseAnimIdleStance = bUseAnimIdleStance;
}

// 0x4B8690
bool CTaskSimpleStandStill::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority)
        return true;
    m_timer.m_nStartTime = CTimer::GetTimeInMS();
    m_timer.m_nInterval = -1;
    m_timer.m_bStarted = true;
    return true;
}

// 0x62F370
bool CTaskSimpleStandStill::ProcessPed(CPed* ped) {
    if (!m_timer.m_bStarted && m_timer.Start(m_nTime)) {
        if (!ped->bInVehicle) {
            ped->SetMoveState(PEDMOVE_STILL);
            ped->m_nSwimmingMoveState = PEDMOVE_STILL;
            if (!ped->bIsDucking || !ped->m_pIntelligence->GetTaskDuck(false)) {
                CAnimManager::BlendAnimation(ped->m_pRwClump, ped->m_nAnimGroup, ANIM_ID_IDLE, m_fBlendData);
            } else {
                CTaskSimpleDuck* pDuckTask = ped->m_pIntelligence->GetTaskDuck(false);
                pDuckTask->ControlDuckMove();
            }
            if (ped->m_pPlayerData)
                ped->m_pPlayerData->m_fMoveBlendRatio = 0.0f;
        }
    }

    if (ped->bIsDucking && ped->m_pIntelligence->GetTaskDuck(false)) {
        CTaskSimpleDuck* pDuckTask = ped->m_pIntelligence->GetTaskDuck(false);
        pDuckTask->ControlDuckMove();
    } else {
        ped->SetMoveState(PEDMOVE_STILL);
    }

    if (m_bUseAnimIdleStance) {
        auto pIdleAnimAssoc = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_IDLE);
        if (pIdleAnimAssoc && pIdleAnimAssoc->m_BlendAmount > 0.99f)
            return true;
    }

    if (m_bLooped || !m_timer.Reset())
        return false;

    return m_timer.IsOutOfTime();
}