#include "StdInc.h"

#include "TaskComplexWalkRoundObject.h"
// #include "PointRoute.h"

void CTaskComplexWalkRoundObject::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWalkRoundObject, 0x86f364, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x655020);
    RH_ScopedInstall(Destructor, 0x6550D0);

    RH_ScopedInstall(CreateRouteTask, 0x655140, { .reversed = false });

    RH_ScopedVMTInstall(Clone, 0x655C10);
    RH_ScopedVMTInstall(GetTaskType, 0x6550C0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x657220, { .reversed = false });
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x657380, { .reversed = false });
    RH_ScopedVMTInstall(ControlSubTask, 0x6575F0, { .reversed = false });
}

// 0x655020
CTaskComplexWalkRoundObject::CTaskComplexWalkRoundObject(int32 moveState, const CVector& targetPoint, CEntity* object) : CTaskComplex() {
    m_moveState   = moveState;
    m_targetPoint = targetPoint;
    m_object      = object;
    field_24      = 0;
    field_28      = 0;
    field_2C      = 0;
    field_2D      = 0;

    CEntity::SafeRegisterRef(m_object);

    m_pointRoute = new CPointRoute();
}

CTaskComplexWalkRoundObject::~CTaskComplexWalkRoundObject() {
    CEntity::SafeCleanUpRef(m_object);

    delete m_pointRoute;
    // todo: m_pointRoute = nullptr;
}

CTaskComplexWalkRoundObject* CTaskComplexWalkRoundObject::Constructor(int32 moveState, const CVector& targetPoint, CEntity* object) {
    this->CTaskComplexWalkRoundObject::CTaskComplexWalkRoundObject(moveState, targetPoint, object);
    return this;
}

CTask* CTaskComplexWalkRoundObject::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x6575F0, CTaskComplexWalkRoundObject*, CPed*>(this, ped);
}

CTask* CTaskComplexWalkRoundObject::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x657220, CTaskComplexWalkRoundObject*, CPed*>(this, ped);
}

CTask* CTaskComplexWalkRoundObject::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x657380, CTaskComplexWalkRoundObject*, CPed*>(this, ped);
}

CTask* CTaskComplexWalkRoundObject::CreateRouteTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x655140, CTaskComplexWalkRoundObject*, CPed*>(this, ped);
}
