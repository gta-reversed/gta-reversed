#include "StdInc.h"

#include "TaskComplexHitPedWithCar.h"
#include "TaskComplexEvasiveStep.h"
#include "TaskComplexFallAndGetUp.h"
#include "TaskSimpleHitFromBehind.h"
#include "TaskSimpleHurtPedWithCar.h"
#include "TaskSimpleKillPedWithCar.h"
#include "TaskSimpleLeaveGroup.h"

void CTaskComplexHitPedWithCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexHitPedWithCar, 0x86f294, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6539A0);
    RH_ScopedInstall(Destructor, 0x653A30);

    RH_ScopedGlobalInstall(ComputeEvasiveStepMoveDir, 0x653B40);

    RH_ScopedInstall(HitHurtsPed, 0x653AE0);
    RH_ScopedInstall(CreateSubTask, 0x6560E0);

    RH_ScopedVMTInstall(Clone, 0x6559B0);
    RH_ScopedVMTInstall(GetTaskType, 0x653A20);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x657AF0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x656300);
    RH_ScopedVMTInstall(ControlSubTask, 0x653A90);
}

// 0x6539A0
CTaskComplexHitPedWithCar::CTaskComplexHitPedWithCar(CVehicle* veh, float impulseMagnitude) :
    m_Veh{veh},
    m_ImpulseMag{impulseMagnitude}
{
    CEntity::SafeRegisterRef(m_Veh);
}

CTaskComplexHitPedWithCar::CTaskComplexHitPedWithCar(const CTaskComplexHitPedWithCar& o) :
    CTaskComplexHitPedWithCar{o.m_Veh, o.m_ImpulseMag}
{
}

// 0x653A30
CTaskComplexHitPedWithCar::~CTaskComplexHitPedWithCar() {
    CEntity::SafeCleanUpRef(m_Veh);
}

// 0x653B40
CVector CTaskComplexHitPedWithCar::ComputeEvasiveStepMoveDir(const CPed* ped, CVehicle* veh) {
    const auto hitSide = (eDirection)CPedGeometryAnalyser::ComputeEntityHitSide(*ped, *veh);
    return CPedGeometryAnalyser::ComputeEntityDir(*veh, hitSide);
}

// 0x653AE0
bool CTaskComplexHitPedWithCar::HitHurtsPed(CPed* ped) {
    if (m_ImpulseMag > (ped->IsPlayer() ? 10.0f : 6.0f)) {
        return true;
    }
    if (ped->m_vecLastCollisionImpactVelocity.z >= -0.8f) {
        return false;
    }
    return m_ImpulseMag > 3.0f;
}

// 0x6560E0
CTask* CTaskComplexHitPedWithCar::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_COMPLEX_FALL_AND_GET_UP: {
        AnimationId animId;
        switch (m_PedHitSide) {
        case 0:
            animId = ANIM_ID_KO_SKID_BACK;
            break;
        case 1:
            animId = ANIM_ID_KO_SPIN_R;
            break;
        case 2:
            animId = ANIM_ID_KO_SKID_FRONT;
            break;
        case 3:
            animId = ANIM_ID_KO_SPIN_L;
            break;
        default:
            animId = static_cast<AnimationId>(taskType);
            break;
        }
        return new CTaskComplexFallAndGetUp(animId, ANIM_GROUP_DEFAULT, m_DownTime);
    }
    case TASK_NONE:
        return new CTaskSimpleLeaveGroup();
    case TASK_SIMPLE_HIT_BEHIND:
        return new CTaskSimpleHitFromBehind();
    case TASK_COMPLEX_EVASIVE_STEP:
        return new CTaskComplexEvasiveStep(m_Veh, m_MoveDir);
    case TASK_SIMPLE_KILL_PED_WITH_CAR:
        return new CTaskSimpleKillPedWithCar(m_Veh, m_ImpulseMag);
    case TASK_SIMPLE_HURT_PED_WITH_CAR:
        return new CTaskSimpleHurtPedWithCar(m_Veh, m_ImpulseMag);
    case TASK_FINISHED:
    default:
        return nullptr;
    }
}

// 0x657AF0
CTask* CTaskComplexHitPedWithCar::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_NONE:
    case TASK_COMPLEX_FALL_AND_GET_UP:
    case TASK_SIMPLE_HIT_BEHIND:
    case TASK_COMPLEX_EVASIVE_STEP:
    case TASK_SIMPLE_KILL_PED_WITH_CAR:
        return CreateSubTask(TASK_FINISHED);
    case TASK_SIMPLE_HURT_PED_WITH_CAR: {
        // N.B.: CTaskSimpleHurtPedWithCar::m_bWillKillPed sits at +0x10 of the sub-task
        // (see `mov al, [ecx+0x10]` @ 0x657B39); the field is private and its header is
        // owned by another ticket, so read it by offset here instead of naming it.
        // A follow-up should make it public (or add an accessor) and use that instead.
        const auto willKillPed = *reinterpret_cast<const bool*>(reinterpret_cast<const uint8*>(m_pSubTask) + 0x10);
        if (willKillPed) {
            return nullptr;
        }
        return CreateSubTask(TASK_COMPLEX_FALL_AND_GET_UP);
    }
    default:
        return nullptr;
    }
}

// 0x656300
CTask* CTaskComplexHitPedWithCar::CreateFirstSubTask(CPed* ped) {
    m_PedHitSide = CPedGeometryAnalyser::ComputePedHitSide(*ped, *m_Veh);
    if (m_ImpulseMag > (ped->IsPlayer() ? 20.0f : 12.0f)) {
        return CreateSubTask(TASK_SIMPLE_KILL_PED_WITH_CAR);
    }
    if (HitHurtsPed(ped)) {
        return CreateSubTask(TASK_SIMPLE_HURT_PED_WITH_CAR);
    }
    m_MoveDir = ComputeEvasiveStepMoveDir(ped, m_Veh);
    return CreateSubTask(TASK_COMPLEX_EVASIVE_STEP);
}

// 0x653A90
CTask* CTaskComplexHitPedWithCar::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}
