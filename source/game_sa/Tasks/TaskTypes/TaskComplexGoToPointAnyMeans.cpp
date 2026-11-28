#include "StdInc.h"

#include "TaskComplexGoToPointAnyMeans.h"
#include "TaskComplexDriveToPoint.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskComplexFollowNodeRoute.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleCreateCarAndGetIn.h"
#include "CarEnterExit.h"

constexpr auto CREATE_CAR_INTERVAL_MS = 3'000;

void CTaskComplexGoToPointAnyMeans::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToPointAnyMeans, 0x86FF68, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x66B720, "1", {}, int32, const CVector&, float, int32);
    RH_ScopedConstructorInstall(0x66B790, "2", {}, int32, const CVector&, CVehicle*, float, int32);
    RH_ScopedDestructorInstall(0x66B830);
    RH_ScopedInstall(CreateSubTask, 0x6705D0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6728A0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6729C0);
    RH_ScopedVMTInstall(ControlSubTask, 0x672A50);
}

// 0x66B720
CTaskComplexGoToPointAnyMeans::CTaskComplexGoToPointAnyMeans(int32 moveState, const CVector& posn, float radius, int32 modelId) :
    m_Pos{ posn },
    m_MoveState{ static_cast<eMoveState>(moveState) },
    m_Radius{ radius },
    m_ModelId{ modelId } {
}

// 0x66B790
CTaskComplexGoToPointAnyMeans::CTaskComplexGoToPointAnyMeans(int32 moveState, const CVector& posn, CVehicle* vehicle, float radius, int32 modelId) :
    CTaskComplexGoToPointAnyMeans{ moveState, posn, radius, modelId } {
    m_Vehicle = vehicle;
    CEntity::SafeRegisterRef(m_Vehicle);
}

// NOTSA
CTaskComplexGoToPointAnyMeans::CTaskComplexGoToPointAnyMeans(const CTaskComplexGoToPointAnyMeans& o) :
    CTaskComplexGoToPointAnyMeans{ (int32)o.m_MoveState, o.m_Pos, o.m_Vehicle, o.m_Radius, o.m_ModelId } {
}

// 0x66B830
CTaskComplexGoToPointAnyMeans::~CTaskComplexGoToPointAnyMeans() {
    CEntity::SafeCleanUpRef(m_Vehicle);
}

// 0x6705D0
CTask* CTaskComplexGoToPointAnyMeans::CreateSubTask(int32 taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_CAR_DRIVE_TO_POINT:
        return new CTaskComplexDriveToPoint(m_Vehicle, m_Pos, -1.0f, 0, MODEL_INVALID, -1.0f, DRIVING_STYLE_STOP_FOR_CARS);
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar(ped->m_pVehicle, 0, 0, true, false);
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER:
        return new CTaskComplexEnterCarAsDriver(m_Vehicle);
    case TASK_SIMPLE_CREATE_CAR_AND_GET_IN:
        return new CTaskSimpleCreateCarAndGetIn(ped->GetPosition(), m_ModelId);
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
        return new CTaskComplexGoToPointAndStandStill(m_MoveState, m_Pos, m_Radius, 2.0f, false, false);
    case TASK_COMPLEX_FOLLOW_NODE_ROUTE:
        return new CTaskComplexFollowNodeRoute(m_MoveState, m_Pos, m_Radius, 3.0f, 2.0f, false, -1, true);
    case TASK_FINISHED: // NOTE: The original returns null for it
        return nullptr;
    default:
        NOTSA_UNREACHABLE_CASE(taskType);
    }
}

// 0x6728A0
CTask* CTaskComplexGoToPointAnyMeans::CreateNextSubTask(CPed* ped) {
    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_CAR_DRIVE_TO_POINT: // 0x6728D4
        return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
    case TASK_COMPLEX_LEAVE_CAR: // 0x6728EC
        return CreateSubTask(TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL, ped);
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER: // 0x672904
        return CreateSubTask(ped->IsInVehicle() ? TASK_COMPLEX_CAR_DRIVE_TO_POINT : TASK_COMPLEX_FOLLOW_NODE_ROUTE, ped);
    case TASK_SIMPLE_CREATE_CAR_AND_GET_IN:                                                                            // 0x672945
        return CreateSubTask(ped->bInVehicle ? TASK_COMPLEX_CAR_DRIVE_TO_POINT : TASK_COMPLEX_FOLLOW_NODE_ROUTE, ped); // NOTE: Only `bInVehicle`, no `m_pVehicle` check
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:                                                                     // 0x67299C
    case TASK_COMPLEX_FOLLOW_NODE_ROUTE:                                                                               // 0x672995
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_FINISHED: // 0x67298C
        return nullptr;
    default:
        NOTSA_UNREACHABLE_CASE(tt);
    }
}

// 0x6729C0
CTask* CTaskComplexGoToPointAnyMeans::CreateFirstSubTask(CPed* ped) {
    if (m_Vehicle) {
        return CreateSubTask(ped->IsInVehicle() ? TASK_COMPLEX_CAR_DRIVE_TO_POINT : TASK_COMPLEX_ENTER_CAR_AS_DRIVER, ped);
    }

    if (ped->IsInVehicle() && ped->m_pVehicle->IsDriver(ped))
        return CreateSubTask(TASK_COMPLEX_CAR_DRIVE_TO_POINT, ped);
    else
        return CreateSubTask(TASK_COMPLEX_FOLLOW_NODE_ROUTE, ped);
}

// 0x672A50
CTask* CTaskComplexGoToPointAnyMeans::ControlSubTask(CPed* ped) {
    if (!notsa::isa<CTaskComplexFollowNodeRoute>(m_pSubTask)) {
        return m_pSubTask;
    }

    if (m_ModelId != -1) { // 0x672A7D
        m_Timer.StartIfNotAlready(CREATE_CAR_INTERVAL_MS);
    }
    if (m_Timer.IsOutOfTime()) { // 0x672ACA
        auto* const tCreateCar = CreateSubTask(TASK_SIMPLE_CREATE_CAR_AND_GET_IN, ped);
        m_Timer.Start(CREATE_CAR_INTERVAL_MS);
        return tCreateCar;
    }

    if (CVector::DistSqr(ped->GetPosition(), m_Pos) <= sq(50.f)) {
        return m_pSubTask;
    }
    auto* const veh = ped->GetIntelligence()->GetVehicleScanner().GetClosestVehicleInRange();
    if (!veh || veh == m_Vehicle || !CCarEnterExit::IsVehicleStealable(veh, ped)) { // 0x672B58
        return m_pSubTask;
    }
    if (notsa::IsFixBugs()) {
        CEntity::SafeCleanUpRef(m_Vehicle); // NOTE/BUG: The original doesn't clean up the old vehicle's reference: deleting that vehicle later writes into this (maybe freed) task
    }
    m_Vehicle = veh;
    m_Vehicle->RegisterReference(reinterpret_cast<CEntity**>(&m_Vehicle));
    return CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_DRIVER, ped);
}
