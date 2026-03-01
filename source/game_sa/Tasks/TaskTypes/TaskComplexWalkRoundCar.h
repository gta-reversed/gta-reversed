#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"
#include "Enums/eMoveState.h"

class CVehicle;
class CPointRoute;

class NOTSA_EXPORT_VTABLE CTaskComplexWalkRoundCar final : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_WALK_ROUND_CAR;

    static void InjectHooks();

    CTaskComplexWalkRoundCar(eMoveState moveState, CVector const& targetPt, CVehicle* vehicle, bool isPedGoingForCarDoor, uint8 forceThisDirectionRoundCar = {});
    CTaskComplexWalkRoundCar(const CTaskComplexWalkRoundCar&);
    ~CTaskComplexWalkRoundCar();

    void   SetNewVehicle(CVehicle * vehicle, uint8 forceThisDirectionRoundCar);
    CTask* CreateRouteTask(CPed*) const;
    bool   GoingForDoor() const { return m_bIsPedGoingForCarDoor && m_Veh && m_Veh->vehicleFlags.bIsBig; }
    float  ComputeRouteRoundSmallCar(CPed* ped);
    float  ComputeRouteRoundBigCar(CPed* ped);
    float  ComputeRoute(CPed* ped);

    CTask*    Clone() const override { return new CTaskComplexWalkRoundCar{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x6541B0
    CTaskComplexWalkRoundCar* Constructor(eMoveState moveState, CVector const& targetPoint, CVehicle* vehicle, bool isPedGoingForCarDoor, int32 bUnknown) {
        this->CTaskComplexWalkRoundCar::CTaskComplexWalkRoundCar(moveState, targetPoint, vehicle, isPedGoingForCarDoor, bUnknown);
        return this;
    }

    // 0x656B00
    CTaskComplexWalkRoundCar* Destructor() {
        this->CTaskComplexWalkRoundCar::~CTaskComplexWalkRoundCar();
        return this;
    }

public:
    uint8        m_MoveState{}; // eMoveState
    // flags 1
    bool         m_bIsPedGoingForCarDoor : 1{};
    bool         m_bFirstSubTaskNeedsToBeCreated : 1{};
    // flags 2
    uint8        m_DirectionGoingRoundCar : 4{};
    uint8        m_ForceThisDirectionRoundCar : 4{};
    CVector      m_TargetPt{};
    CVehicle*    m_Veh{};
    CPointRoute* m_Route{};
    CTaskTimer   m_Timer{};
    CVector      m_VehPos{};
    CVector      m_VehMatFwd{};
    CVector      m_VehMatRight{};
    int32        m_EnterCarStartTime{};
};
VALIDATE_SIZE(CTaskComplexWalkRoundCar, 0x58);
