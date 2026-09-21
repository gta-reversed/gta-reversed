#include "StdInc.h"

#include "TaskComplexFollowPointRoute.h"
#include "TaskComplexWalkRoundCar.h"
#include "CarEnterExit.h"

void CTaskComplexWalkRoundCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWalkRoundCar, 0x86f308, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6541B0);
    RH_ScopedInstall(Destructor, 0x656B00);

    RH_ScopedInstall(SetNewVehicle, 0x654290);
    RH_ScopedInstall(CreateRouteTask, 0x6542E0);
    RH_ScopedInstall(ComputeRouteRoundSmallCar, 0x6544F0, { .reversed = false });
    RH_ScopedInstall(GoingForDoor, 0x654720);
    RH_ScopedInstall(ComputeRouteRoundBigCar, 0x656BB0, { .reversed = false });
    RH_ScopedInstall(ComputeRoute, 0x657B80);

    RH_ScopedVMTInstall(Clone, 0x655B00);
    RH_ScopedVMTInstall(GetTaskType, 0x654280);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x656B70);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x658200, { .reversed = false });
    RH_ScopedVMTInstall(ControlSubTask, 0x654370);
}

// 0x6541B0
CTaskComplexWalkRoundCar::CTaskComplexWalkRoundCar(eMoveState moveState, CVector const& targetPt, CVehicle* vehicle, bool isPedGoingForCarDoor, uint8 forceThisDirectionRoundCar) :
    m_MoveState{(uint8)moveState},
    m_bIsPedGoingForCarDoor{isPedGoingForCarDoor},
    m_ForceThisDirectionRoundCar{forceThisDirectionRoundCar},
    m_TargetPt{targetPt},
    m_Veh{vehicle},
    m_Route{new CPointRoute{}}
{
    CEntity::SafeRegisterRef(m_Veh);
}

CTaskComplexWalkRoundCar::CTaskComplexWalkRoundCar(const CTaskComplexWalkRoundCar& o) :
    CTaskComplexWalkRoundCar{
        (eMoveState)o.m_MoveState,
        o.m_TargetPt,
        o.m_Veh,
        o.m_bIsPedGoingForCarDoor,
        o.m_ForceThisDirectionRoundCar
    }
{
}

// 0x656B00
CTaskComplexWalkRoundCar::~CTaskComplexWalkRoundCar() {
    CEntity::SafeCleanUpRef(m_Veh);
    delete m_Route;
}


// 0x654290
void CTaskComplexWalkRoundCar::SetNewVehicle(CVehicle * vehicle, uint8 forceThisDirectionRoundCar) {
    if (notsa::IsFixBugs()) {
        if (vehicle == m_Veh) {
            return;
        }
    }
    CEntity::ChangeEntityReference(m_Veh, vehicle);
    m_ForceThisDirectionRoundCar    = forceThisDirectionRoundCar;
    m_bFirstSubTaskNeedsToBeCreated = true;
    m_Route->Clear();
}

// 0x6542E0
CTask* CTaskComplexWalkRoundCar::CreateRouteTask(CPed*) const {
    if (m_Route->IsEmpty()) {
        return nullptr;
    }
    return new CTaskComplexFollowPointRoute{ (eMoveState)m_MoveState, *m_Route };
}

// 0x6544F0
float CTaskComplexWalkRoundCar::ComputeRouteRoundSmallCar(CPed* ped) {
    return plugin::CallMethodAndReturn<float, 0x6544F0, CTaskComplexWalkRoundCar*, CPed*>(this, ped);
}

// 0x656BB0
float CTaskComplexWalkRoundCar::ComputeRouteRoundBigCar(CPed* ped) {
    return plugin::CallMethodAndReturn<float, 0x656BB0, CTaskComplexWalkRoundCar*, CPed*>(this, ped);
}

// 0x657B80
float CTaskComplexWalkRoundCar::ComputeRoute(CPed* ped) {
    return m_Veh->vehicleFlags.bIsBig
        ? ComputeRouteRoundBigCar(ped)
        : ComputeRouteRoundSmallCar(ped);
}

// 0x656B70
CTask* CTaskComplexWalkRoundCar::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return CreateRouteTask(ped);
    }
    return nullptr;
}

// 0x658200
CTask* CTaskComplexWalkRoundCar::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x658200, CTaskComplexWalkRoundCar*, CPed*>(this, ped);
}

// 0x654370
CTask* CTaskComplexWalkRoundCar::ControlSubTask(CPed* ped) {
    if (m_bFirstSubTaskNeedsToBeCreated) {
        m_bFirstSubTaskNeedsToBeCreated = false;
        return CreateFirstSubTask(ped);
    }
    int32 waitTime = 200;
    bool  quitEntering = false;
    if (ped->IsPlayer() && m_Veh && m_EnterCarStartTime != -1
        && CCarEnterExit::IsPlayerToQuitCarEnter(ped, m_Veh, m_EnterCarStartTime, m_pSubTask)
        && m_pSubTask->GetTaskType() == TASK_COMPLEX_FOLLOW_POINT_ROUTE) {
        quitEntering = true;
        waitTime = 1302; // 0x516
    } else if (m_pSubTask->GetTaskType() == TASK_COMPLEX_FOLLOW_POINT_ROUTE && m_Timer.IsOutOfTime()) {
        waitTime = 1302; // 0x516 (quitEntering stays false here)
    }
    if (m_Veh) {
        const CVector& vehPos = m_Veh->GetPosition();
        if ((m_VehPos - vehPos).SquaredMagnitude() <= 0.0625f
            && DotProduct(m_Veh->GetMatrix().GetForward(), m_VehMatFwd) >= 0.9f
            && DotProduct(m_Veh->GetMatrix().GetRight(), m_VehMatRight) >= 0.9f
            && waitTime == 200) {
            return m_pSubTask;
        }
    }
    if (!m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
        return m_pSubTask;
    }
    if (quitEntering) {
        if (CTask* primary = ped->GetTaskManager().m_aPrimaryTasks[TASK_PRIMARY_PRIMARY]) {
            if (primary->GetTaskType() == TASK_COMPLEX_ENTER_CAR_AS_DRIVER) {
                primary->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
            }
        }
    }
    return nullptr;
}
