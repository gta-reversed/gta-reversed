/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskComplex.h"
#include "PathFind.h"
#include "TaskUtilityLineUpPedWithCar.h"
#include "eTargetDoor.h"

// Note: This class is abstract, that is, it can't be directly constructed,
// rather, use one of the derived classes.
class NOTSA_EXPORT_VTABLE CTaskComplexEnterCar : public CTaskComplex {
public:
    static void InjectHooks();

    // Shouldn't be used directly, use `CTaskComplexEnterCarAsDriver` or `CTaskComplexEnterCarAsPassenger` instead
    CTaskComplexEnterCar(CVehicle* targetVehicle, bool bAsDriver, bool bQuitAfterOpeningDoor, bool bQuitAfterDraggingPedOut, bool bCarryOnAfterFallingOff = false);
    ~CTaskComplexEnterCar() override;

    bool           MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*         CreateNextSubTask(CPed* ped) override;
    CTask*         CreateFirstSubTask(CPed* ped) override;
    CTask*         ControlSubTask(CPed* ped) override;
    CTask*         CreateSubTask(eTaskType type, CPed* ped);
    CVector        GetTargetPos() const;
    CVehicle*      GetCameraAvoidVehicle();


    auto GetTargetCar() const { return m_Car; }
protected:
    virtual CTask* CreateNextSubTask_AfterSimpleCarAlign(CPed* ped);

    void   SetVehicleFlags(CPed* ped);
    bool   IsTargetDoorOnTheLeft() const { return m_TargetDoor == 10 || m_TargetDoor == 11; } // NOTSA
    int32  ComputeTargetDoorOppositeToFlag() const;

    void   PreparePedForVehicleEnter(CPed* ped);
    void   PrepareVehicleForPedEnter(CPed* ped);
    void   CreateTaskUtilityLineUpPedWithCar(CPed* ped);

    CTask* CreateDragPedOutSubTask(CPed* ped);
protected:
    CVehicle*                     m_Car{};                            //< Vehicle to get into/onto
    bool                          m_bAsDriver : 1{};                  //< Enter as driver
    bool                          m_bQuitAfterOpeningDoor : 1{};      //< Stop after opening the door
    bool                          m_bQuitAfterDraggingPedOut : 1{};   //< Stop after dragging the ped out from the target seat
    bool                          m_bCarryOnAfterFallingOff : 1{};    //< [For bikes, etc] Get back up and continue driving
    int32                         m_TargetDoor{};                     //< Door to enter on
    int32                         m_TargetDoorOppositeToFlag{};       //< Not sure
    int32                         m_TargetSeat{};                     //< Seat we want to be at
    int32                         m_DraggedPedDownTime{};             //< Not sure
    eMoveState                    m_MoveState{ PEDMOVE_RUN };         //< How to approach the vehicle
    uint8                         m_NumGettingInSet{};                //< Not sure
    uint8                         m_CamMovementChoice{};              //< Not sure
    CVector                       m_TargetDoorPos{};                  //< Target door's position (Persumeably calculated using `CCarEnterExit::GetPositionToOpenCarDoor`)
    CTaskUtilityLineUpPedWithCar* m_LineUpUtility{};                  //< Line up utility
    bool                          m_bAborting{};                      //< Whenever `MakeAbortable` was called (?)
    CPed*                         m_DraggedPed{};                     //< The ped we're dragging out rn (that was at targetSeat)
    uint8                         m_DoorFlagsSet{};                   //< used for CVehicle::SetGettingInFlags
    float                         m_CruiseSpeed{-1.f};                //< How fast to go after we've entered. Only used when `m_bAsDriver` is set
    int32                         m_EnterCarStartTime{};              //< When we started entering the vehicle (`CTimer::GetTimeMs()`)

private:
    // 0x63A220
    CTaskComplexEnterCar* Constructor(CVehicle* vehicle, bool bAsDriver, bool bQuitAfterOpeningDoor, bool bQuitAfterDraggingPedOut, bool bCarryOnAfterFallingOff) {
        this->CTaskComplexEnterCar::CTaskComplexEnterCar(vehicle, bAsDriver, bQuitAfterOpeningDoor, bQuitAfterDraggingPedOut, bCarryOnAfterFallingOff);
        return this;
    }

    // 0x63DFA0
    CTaskComplexEnterCar* Destructor() {
        this->CTaskComplexEnterCar::~CTaskComplexEnterCar();
        return this;
    }
};
VALIDATE_SIZE(CTaskComplexEnterCar, 0x50);
