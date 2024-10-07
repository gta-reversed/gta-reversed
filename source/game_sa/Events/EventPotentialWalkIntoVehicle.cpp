#include "StdInc.h"
#include "EventPotentialWalkIntoVehicle.h"


void CEventPotentialWalkIntoVehicle::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPotentialWalkIntoVehicle, 0x85AED0, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4AE320);
    RH_ScopedVMTInstall(AffectsPed, 0x4AE420);
}

// 0x4AE320
CEventPotentialWalkIntoVehicle::CEventPotentialWalkIntoVehicle(CVehicle* vehicle, int32 moveState) {
    m_vehicle = vehicle;
    m_moveState = moveState;
    CEntity::SafeRegisterRef(m_vehicle);
}

CEventPotentialWalkIntoVehicle::~CEventPotentialWalkIntoVehicle() {
    CEntity::SafeCleanUpRef(m_vehicle);
}

// 0x4AE320
CEventPotentialWalkIntoVehicle* CEventPotentialWalkIntoVehicle::Constructor(CVehicle* vehicle, int32 moveState) {
    this->CEventPotentialWalkIntoVehicle::CEventPotentialWalkIntoVehicle(vehicle, moveState);
    return this;
}

// 0x4AE420
bool CEventPotentialWalkIntoVehicle::AffectsPed(CPed* ped) {
    auto taskEnterCarAsDriver = ped->GetTaskManager().Find<CTaskComplexEnterCarAsDriver>();
    auto goToTask = reinterpret_cast<CTaskSimpleGoTo*>(ped->GetTaskManager().GetSimplestActiveTask());
    if (ped->IsPlayer() && !taskEnterCarAsDriver && !CTask::IsGoToTask(goToTask))
        return false;

    if (!ped->IsAlive() || ped->bInVehicle || m_moveState == PEDMOVE_STILL)
        return false;

    if (const auto task = ped->GetTaskManager().Find<CTaskComplexWalkRoundCar>()) {
        if (task->m_Veh != m_vehicle) {
            if ((m_vehicle->m_pTrailer && m_vehicle->m_pTrailer == task->m_Veh) ||
                (m_vehicle->m_pTractor && m_vehicle->m_pTractor == task->m_Veh)
            ) {
                task->SetNewVehicle(m_vehicle, 0);
            }
            return false;
        }
    }

    if (!ped->m_pAttachedTo && m_vehicle && m_vehicle->IsSubVehicleTypeValid() && !m_vehicle->IsSubFakeAircraft()) {
        bool isGoToPointTask = false;
        CVector targetPos;
        if (goToTask->GetTaskType() == TASK_SIMPLE_GO_TO_POINT) {
            targetPos = goToTask->m_vecTargetPoint;
            isGoToPointTask = true;
        }
        if (taskEnterCarAsDriver) {
            if (m_vehicle == taskEnterCarAsDriver->GetTargetCar() && m_vehicle->IsSubPlane())
                return false;

            targetPos = taskEnterCarAsDriver->GetTargetPos();
        }
        if (isGoToPointTask || taskEnterCarAsDriver) {
            CVector surfacePoint;
            CPedGeometryAnalyser::ComputeClosestSurfacePoint(*ped, *m_vehicle, surfacePoint);
            int32 hitSide = CPedGeometryAnalyser::ComputeEntityHitSide(surfacePoint, *m_vehicle);
            if (hitSide != CPedGeometryAnalyser::ComputeEntityHitSide(targetPos, *m_vehicle))
                return true;

            return false;
        }
        return true;
    }
    return false;
}