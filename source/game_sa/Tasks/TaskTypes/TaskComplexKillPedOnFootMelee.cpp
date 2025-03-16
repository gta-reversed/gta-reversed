#include "StdInc.h"
#include "TaskComplexKillPedOnFootMelee.h"
#include <InterestingEvents.h>
#include "./TaskSimpleFight.h"
#include "./SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.h"
#include "./SeekEntity/TaskComplexSeekEntity.h"
#include "./TaskSimpleFightingControl.h"
#include "./TaskSimplePause.h"
#include "./TaskSimpleStandStill.h"
#include "./TaskComplexSequence.h"
#include "./TaskComplexTurnToFaceEntityOrCoord.h"
#include "./TaskSimpleShakeFist.h"

void CTaskComplexKillPedOnFootMelee::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexKillPedOnFootMelee, 0x86d8ec, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x621000);
    RH_ScopedInstall(Destructor, 0x621090);

    RH_ScopedInstall(CalculateSearchPositionAndRanges, 0x621100);
    RH_ScopedInstall(CreateSubTask, 0x626AC0);

    RH_ScopedVMTInstall(Clone, 0x623460);
    RH_ScopedVMTInstall(GetTaskType, 0x621080);
    RH_ScopedVMTInstall(MakeAbortable, 0x6210F0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x62BC10);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x62BE30);
    RH_ScopedVMTInstall(ControlSubTask, 0x626D90);
}

// 0x621000
CTaskComplexKillPedOnFootMelee::CTaskComplexKillPedOnFootMelee(CPed* target) :
    m_target{target}
{
    CEntity::SafeRegisterRef(m_target);
}

// NOTSA (For 0x623460)
CTaskComplexKillPedOnFootMelee::CTaskComplexKillPedOnFootMelee(const CTaskComplexKillPedOnFootMelee& o) :
    CTaskComplexKillPedOnFootMelee{o.m_target}
{
}

// 0x621090
CTaskComplexKillPedOnFootMelee::~CTaskComplexKillPedOnFootMelee() {
    CEntity::SafeCleanUpRef(m_target);
}

// 0x621100
void CTaskComplexKillPedOnFootMelee::CalculateSearchPositionAndRanges(CPed* ped) {
    m_attackRange = 8.f;
    const auto fight = ped->GetIntelligence()->GetTaskFighting();
    m_arriveRange = CTaskSimpleFight::m_aComboData[fight ? std::max(0, fight->m_nComboSet - 4) : 0].m_fRanges;
    if (m_target) {
        m_targetPos = m_target->GetPosition();
    }
}

// 0x626AC0
CTask* CTaskComplexKillPedOnFootMelee::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_FIGHT_CTRL: {
        m_pauseTimer = -1;
        return new CTaskSimpleFightingControl{
            m_target,
            m_attackAngle,
            m_attackRange
        };
    }
    case TASK_COMPLEX_SEEK_ENTITY:
        return new CTaskComplexSeekEntity<CEntitySeekPosCalculatorStandard>{
            m_target,
            50'000,
            1'000,
            1.f,
            2.f,
            2.f,
            true,
            true
        };
    case TASK_SIMPLE_PAUSE: {
        VERIFY(CTaskSimpleStandStill{}.ProcessPed(ped));
        if (m_pauseTimer == -1) {
            m_pauseTimer = (int32)CTimer::GetTimeInMS();
        }
        return new CTaskSimplePause{};
    }
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill{2000};
    case TASK_COMPLEX_SEQUENCE:
        return new CTaskComplexSequence{
            new CTaskComplexTurnToFaceEntityOrCoord{m_target},
            new CTaskSimpleShakeFist{}
        };
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE();
    }
}

bool CTaskComplexKillPedOnFootMelee::IsTargetInRange(CPed* ped, float range) {
    CalculateSearchPositionAndRanges(ped);
    return sq(range) <= (ped->GetPosition() - m_targetPos).SquaredMagnitude();
}

// 0x62BC10
CTask* CTaskComplexKillPedOnFootMelee::CreateNextSubTask(CPed* ped) {
    if (!m_target) {
        return nullptr;
    }
    const auto Create = [&](eTaskType tt) { return CreateSubTask(tt, ped); };
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_SEEK_ENTITY:
        return Create(TASK_SIMPLE_FIGHT_CTRL);
    case TASK_SIMPLE_STAND_STILL:
    case TASK_COMPLEX_SEQUENCE: // 0x62BC4A
        return m_target->physicalFlags.bSubmergedInWater
            ? CreateFirstSubTask(ped)
            : Create(TASK_SIMPLE_STAND_STILL);
    case TASK_SIMPLE_PAUSE: { // 0x62BC98
        if (!IsTargetInRange(ped, m_arriveRange)) {
            return Create(TASK_SIMPLE_FIGHT_CTRL);
        }
        if (!ped->bStayInSamePlace) {
            return Create(TASK_COMPLEX_SEEK_ENTITY);
        }
        return Create(
            m_pauseTimer != -1 && CTimer::GetTimeInMS() - (uint32)m_pauseTimer > 3000
                ? TASK_FINISHED
                : TASK_SIMPLE_PAUSE
        );
    }
    case TASK_FINISHED: { // 0x62BD75
        return nullptr;
    }
    case TASK_SIMPLE_FIGHT_CTRL: { // 0x62BD8B
        if (ped->bStayInSamePlace) {
            return Create(TASK_SIMPLE_PAUSE);
        }
        return Create(
            IsTargetInRange(ped, m_arriveRange)
                ? TASK_COMPLEX_SEEK_ENTITY
                : TASK_SIMPLE_PAUSE
        );
    }
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x62BE30
CTask* CTaskComplexKillPedOnFootMelee::CreateFirstSubTask(CPed* ped) {
    g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_19, ped);
    m_bNewTarget = false;
    if (!m_target) {
        return nullptr;
    }
    return CreateSubTask([this, ped] {
        if (!IsTargetInRange(ped, m_arriveRange)) {
            return TASK_SIMPLE_FIGHT_CTRL;
        }
        return ped->bStayInSamePlace
            ? TASK_SIMPLE_PAUSE
            : TASK_COMPLEX_SEEK_ENTITY;
    }(), ped);
}

// 0x626D90
CTask* CTaskComplexKillPedOnFootMelee::ControlSubTask(CPed* ped) {
    ped->Say(CTX_GLOBAL_FIGHT);

    if (m_bNewTarget) {
        return CreateFirstSubTask(ped);
    }

    if (!m_target || m_target->m_fHealth <= 0.f) {
        return nullptr;
    }

    const auto TryAbortAndCreate = [&, this](eTaskType taskToCreate) {
        return m_pSubTask->MakeAbortable(ped)
            ? CreateSubTask(taskToCreate, ped)
            : m_pSubTask;
    };

    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_SEQUENCE: { // 0x626dfe
        return m_target->physicalFlags.bSubmergedInWater
            ? m_pSubTask
            : TryAbortAndCreate(TASK_SIMPLE_FIGHT_CTRL);
    }
    case TASK_SIMPLE_PAUSE: // 0x626e05
    case TASK_COMPLEX_SEEK_ENTITY: {  // 0x626f0e
        if (m_target->physicalFlags.bSubmergedInWater) {
            return TryAbortAndCreate(TASK_COMPLEX_SEQUENCE);
        }
        if (IsTargetInRange(ped, m_arriveRange)) {
            return m_pSubTask;
        }
        return TryAbortAndCreate(TASK_SIMPLE_FIGHT_CTRL);
    }
    case TASK_SIMPLE_FIGHT_CTRL: { // 0x626E5C
        if (m_target->physicalFlags.bSubmergedInWater) {
            return TryAbortAndCreate(TASK_COMPLEX_SEQUENCE);
        }
        if (!IsTargetInRange(ped, m_attackRange)) {
            return m_pSubTask;
        }
        return TryAbortAndCreate(
            ped->bStayInSamePlace
                ? TASK_SIMPLE_PAUSE
                : TASK_COMPLEX_SEEK_ENTITY
        );
    }
    default:
        return m_pSubTask;
    }
}
