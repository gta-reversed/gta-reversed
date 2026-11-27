#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"
#include "Enums/eMoveState.h"

class CVehicle;
class CPointRoute;

class NOTSA_EXPORT_VTABLE CTaskComplexWalkRoundCar : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_WALK_ROUND_CAR;

    CTaskComplexWalkRoundCar(eMoveState moveState, const CVector& targetPt, CVehicle* vehicle, bool isPedGoingForCarDoor, uint8 forceThisDirectionRoundCar = {});
    CTaskComplexWalkRoundCar(const CTaskComplexWalkRoundCar&);
    ~CTaskComplexWalkRoundCar() override;

    CTask*    Clone() const override { return new CTaskComplexWalkRoundCar{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    void   SetNewVehicle(CVehicle* vehicle, uint8 forceThisDirectionRoundCar);
    CTask* CreateRouteTask(CPed*) const;
    bool   GoingForDoor() const { return m_IsPedGoingForCarDoor && m_Veh && m_Veh->vehicleFlags.bIsBig; }
    float  ComputeRouteRoundSmallCar(CPed* ped);
    float  ComputeRouteRoundBigCar(CPed* ped);
    float  ComputeRoute(CPed* ped);

    CVehicle* GetVehicle() const { return m_Veh; } // NOTSA

    static void InjectHooks();

private:
    uint8                      m_MoveState{}; // eMoveState
    bool                       m_IsPedGoingForCarDoor : 1 {};
    bool                       m_FirstSubTaskNeedsToBeCreated : 1 {};
    uint8                      m_DirectionGoingRoundCar : 4 {};     //!< Direction of the route round the car (See `CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox`)
    uint8                      m_ForceThisDirectionRoundCar : 4 {}; //!< Direction the route round the car must take (0 => any)
    CVector                    m_TargetPt{};
    notsa::EntityRef<CVehicle> m_Veh{};
    CPointRoute*               m_Route{};
    CTaskTimer                 m_Timer{};
    CVector                    m_VehPos{};                //!< Position of the car when the route was computed
    CVector                    m_VehMatFwd{};             //!< Forward vector of the car when the route was computed
    CVector                    m_VehMatRight{};           //!< Right vector of the car when the route was computed
    int32                      m_EnterCarStartTime{ -1 }; //!< Start time of the player's enter car task (-1 => none)
};
VALIDATE_SIZE(CTaskComplexWalkRoundCar, 0x58);
