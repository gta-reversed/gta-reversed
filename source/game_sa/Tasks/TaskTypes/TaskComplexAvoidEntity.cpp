#include "StdInc.h"

#include "TaskComplexAvoidEntity.h"
#include "IKChainManager_c.h"
#include "TaskSimpleGoToPoint.h"

// !!!!!!!!!!!!!!!!
// UNUSED TASK
// !!!!!!!!!!!!!!!!

void CTaskComplexAvoidEntity::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexAvoidEntity, 0x86FF00, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x66AA20, { .reversed = false });
    //RH_ScopedInstall(ComputeAvoidSphere, 0x66FD30, { .reversed = false });
    RH_ScopedInstall(ComputeSphere, 0x66F6C0, { .reversed = false });
    RH_ScopedVMTInstall(ControlSubTask, 0x66ADE0, { .reversed = false });
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x672530, { .reversed = false });
    RH_ScopedVMTInstall(CreateNextSubTask, 0x66AD70, { .reversed = false });
    RH_ScopedInstall(MakeAbortable, 0x66AD40, { .reversed = false });
    RH_ScopedInstall(QuitIK, 0x66AD10, { .reversed = false });
    RH_ScopedInstall(SetUpIK, 0x66AB40, { .reversed = false });
    RH_ScopedInstall(NearbyEntitiesInSphere, 0x66ADA0, { .reversed = false });

    RH_ScopedOverloadedInstall(ComputeDetourTarget, "0", 0x66FD30, bool(CTaskComplexAvoidEntity::*)(const CPed&), { .reversed = false });
    RH_ScopedOverloadedInstall(ComputeDetourTarget, "1", 0x66ADB0, bool(CTaskComplexAvoidEntity::*)(const CPed&, const CColSphere&), { .reversed = false });
}

// 0x66AA20
CTaskComplexAvoidEntity::CTaskComplexAvoidEntity(eMoveState moveState, CEntity* entity, const CVector& pos) : CTaskComplex() {
    m_moveState = moveState;
    m_Entity = entity;
    f20 = pos;
    f2C = pos;
    field_44 = 0;
    field_48 = 0;
    field_4C = 0;
    field_4D = 0;
    m_b1 = false;
    CEntity::SafeRegisterRef(m_Entity);
}

// 0x66AAE0
CTaskComplexAvoidEntity::~CTaskComplexAvoidEntity() {
    CEntity::SafeCleanUpRef(m_Entity);
}

// 0x66FD30
bool CTaskComplexAvoidEntity::ComputeDetourTarget(const CPed& ped) {
    CColSphere sphere;
    ComputeAvoidSphere(ped, sphere);
    return ComputeDetourTarget(ped, sphere);
}

// 0x66ADB0
bool CTaskComplexAvoidEntity::ComputeDetourTarget(const CPed& ped, const CColSphere& sphere) {
    return CPedGeometryAnalyser::ComputeRouteRoundSphere(ped, sphere, m_PedPos, f20, f38, f2C);
}

// 0x672080
void CTaskComplexAvoidEntity::ComputeAvoidSphere(const CPed& ped, CColSphere& sphere) {
    plugin::CallMethod<0x672080, CTaskComplexAvoidEntity*, const CPed&, CColSphere&>(this, ped, sphere);
}

// 0x66F6C0
void CTaskComplexAvoidEntity::ComputeSphere(CColSphere& sphere, CEntity** entities) {
    plugin::Call<0x66F6C0, CColSphere&, CEntity**>(sphere, entities);
}

// 0x66ADE0


CTask* CTaskComplexAvoidEntity::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x66ADE0, CTaskComplexAvoidEntity*, CPed*>(this, ped);
}

// 0x672530


CTask* CTaskComplexAvoidEntity::CreateFirstSubTask(CPed* ped) {
    m_PedPos = ped->GetPosition();
    if (!ComputeDetourTarget(*ped)) {
        return CreateNextSubTask(ped);
    }

    return new CTaskSimpleGoToPoint(m_moveState, f2C, 0.5f, false, false);
}

// 0x66AD70

// 0x0
CTask* CTaskComplexAvoidEntity::CreateNextSubTask(CPed* ped) {
    QuitIK(ped);
    return nullptr;
}

// 0x66AD40
bool CTaskComplexAvoidEntity::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    QuitIK(ped);
    return true;
}

// 0x66AD10
void CTaskComplexAvoidEntity::QuitIK(CPed* ped) const {
    if (m_b1 && g_ikChainMan.IsLooking(ped)) {
        g_ikChainMan.AbortLookAt(ped, 250);
    }
}

// 0x66AB40
void CTaskComplexAvoidEntity::SetUpIK(CPed *ped) {
    return plugin::CallMethod<0x66AB40, CTaskComplexAvoidEntity*, CPed*>(this, ped);

    if (ped == FindPlayerPed() && !CPad::GetPad(0)->DisablePlayerControls)
        return;

    if (!ped->GetIsOnScreen() || m_b1 || g_ikChainMan.GetLookAtEntity(ped) || ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_IK))
        return;

    auto taskType = m_Parent->GetTaskType();
    if (m_Parent && (taskType == TASK_COMPLEX_AVOID_OTHER_PED_WHILE_WANDERING || taskType == TASK_COMPLEX_AVOID_ENTITY))
        return;

    CVector out = f20 - ped->GetPosition();
    if (out.SquaredMagnitude() <= 9.0f)
        return;

    out.Normalise();
    auto dot = DotProduct(out, ped->m_matrix->GetForward());
    if (dot < CTaskSimpleGoTo::ms_fLookAtThresholdDotProduct) {
        out += out + CVector{ f20.x, f20.y, ped->GetPosition().z + 0.61f };
        g_ikChainMan.LookAt("TaskAvoidEntity", ped, nullptr, 5000, BONE_UNKNOWN, &out, false, 0.25f, 500, 3, false);
        m_b1 = true;
    }
}

// 0x66ADA0
uint8 CTaskComplexAvoidEntity::NearbyEntitiesInSphere(const CPed& ped, const CColSphere& sphere, CEntity**, CEntity **) {
    return 0;
}
