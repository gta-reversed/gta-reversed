#include "StdInc.h"

#include "TaskComplexUseSwatRope.h"

#include "TaskSimpleAbseil.h"
#include "TaskSimplePause.h"
#include "TaskSimpleNone.h"
#include "PedPlacement.h"
#include "Rope.h"
#include "Ropes.h"

void CTaskComplexUseSwatRope::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseSwatRope, 0x86F59C, 13);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x659470);
    RH_ScopedInstall(CreateSubTask, 0x659620);
    RH_ScopedVMTInstall(Clone, 0x659C30);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65A440);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65A3E0);
    RH_ScopedVMTInstall(ControlSubTask, 0x65A460);
    RH_ScopedVMTInstall(MakeAbortable, 0x659530);
}

CTaskComplexUseSwatRope* CTaskComplexUseSwatRope::Constructor(uint32 ropeId, CHeli* heli) {
    this->CTaskComplexUseSwatRope::CTaskComplexUseSwatRope(ropeId, heli);
    return this;
}

// 0x659470
CTaskComplexUseSwatRope::CTaskComplexUseSwatRope(uint32 ropeId, CHeli* heli) : CTaskComplex() {
    m_nRopeId        = ropeId;
    m_pHeli          = heli;
    m_fCoorAlongRope = 0.0F;
    m_bIsOnHeli      = true;

    // todo: CEntity::SafeRegisterRef
    m_pHeli->RegisterReference(reinterpret_cast<CEntity**>(&m_pHeli));
}

CTaskComplexUseSwatRope::CTaskComplexUseSwatRope(uint32 ropeId) : CTaskComplex() {
    m_nRopeId        = ropeId;
    m_pHeli          = nullptr;
    m_fCoorAlongRope = 0.0F;
    m_bIsOnHeli      = false;
}

// 0x65A3C0
CTaskComplexUseSwatRope::~CTaskComplexUseSwatRope() {
    if (m_bIsOnHeli)
        m_pHeli->CleanUpOldReference(reinterpret_cast<CEntity**>(&m_pHeli));
}

// 0x659C30
CTask* CTaskComplexUseSwatRope::Clone() const {
    if (m_bIsOnHeli)
        return new CTaskComplexUseSwatRope(m_nRopeId, m_pHeli);
    else
        return new CTaskComplexUseSwatRope(m_nRopeId);
}

// 0x659530
bool CTaskComplexUseSwatRope::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    eEventType eventType = event->GetEventType();

    if ((priority == ABORT_PRIORITY_IMMEDIATE
        || eventType == EVENT_DEATH
        || eventType == EVENT_HEALTH_REALLY_LOW
        || eventType == EVENT_ON_FIRE
        || eventType == EVENT_IN_WATER
        || eventType == EVENT_WATER_CANNON
        || eventType == EVENT_VEHICLE_DAMAGE_WEAPON
        || eventType == EVENT_VEHICLE_DAMAGE_COLLISION
        || ped->m_fHealth <= 20.0F
        )
        && m_pSubTask->MakeAbortable(ped, priority, event)
    ) {
        ped->bIsStanding = false;
        ped->m_bUsesCollision = true;
        ped->ResetMoveSpeed();
        return true;
    }
    else
        return false;
}

// 0x65A3E0
CTask* CTaskComplexUseSwatRope::CreateNextSubTask(CPed* ped) {
    eTaskType subTaskType = m_pSubTask->GetTaskType();

    if (subTaskType == TASK_NONE || subTaskType == TASK_SIMPLE_PAUSE)
        return CreateSubTask(TASK_FINISHED, ped);

    if (subTaskType == TASK_SIMPLE_ABSEIL)
        return CreateSubTask(TASK_SIMPLE_PAUSE, ped);

    return nullptr;
}

// 0x65A440
CTask* CTaskComplexUseSwatRope::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(TASK_SIMPLE_ABSEIL, ped);
}

// 0x65A460
CTask* CTaskComplexUseSwatRope::ControlSubTask(CPed* ped) {
    if (   m_bIsOnHeli
        && (
               !m_pHeli
               || m_pHeli->m_autoPilot.m_nCarMission == MISSION_HELI_CRASH_AND_BURN
               || m_pHeli->m_fHealth <= 0.0F
           )
        && m_pSubTask->MakeAbortable(ped)
    ) {
        ped->bIsStanding = false;
        ped->m_bUsesCollision = true;
        ped->ResetMoveSpeed();
        return nullptr;
    }

    eTaskType subTaskType = m_pSubTask->GetTaskType();

    if (subTaskType == TASK_SIMPLE_PAUSE || subTaskType == TASK_SIMPLE_ABSEIL) {
        CVector groundCoord = ped->GetPosition();
        CPedPlacement::FindZCoorForPed(groundCoord);
        if (ped->GetPosition().z - 2.0F < groundCoord.z && m_pSubTask->MakeAbortable(ped))
            return CreateSubTask(TASK_NONE, ped);

        m_fCoorAlongRope += CTimer::GetTimeStep() * 0.003F;
        CVector posn;
        if (CRopes::FindCoorsAlongRope(m_nRopeId, m_fCoorAlongRope, &posn, nullptr)) {
            ped->SetPosn(posn);
            ped->m_fAimingRotation = ped->m_fCurrentRotation - CTimer::GetTimeStep() * 0.05F;
            ped->m_vecMoveSpeed.z = -0.03f;
            ped->Say(CTX_GLOBAL_ROPE);
        }
    }

    return m_pSubTask;
}

// 0x659620
CTask* CTaskComplexUseSwatRope::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_ABSEIL:
        return new CTaskSimpleAbseil();
    case TASK_SIMPLE_PAUSE:
        return new CTaskSimplePause(100'000);
    case TASK_NONE:
        return new CTaskSimpleNone();
    case TASK_FINISHED:
        ped->m_bUsesCollision = true;
        ped->ResetMoveSpeed();
        return nullptr;
    default:
        return nullptr;
    }
}
