/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"

class CPed;
class CVehicle;
class CTask;

class CCarEnterExit {
public:
    static const float& ms_fMaxSpeed_CanDragPedOut;
    static const float& ms_fMaxSpeed_PlayerCanDragPedOut;

    static inline bool& ms_bPedOffsetsCalculated = *(bool*)0xC18C20;
    static inline CVector& ms_vecPedGetUpAnimOffset = *(CVector*)0xC18C3C;
    static inline CVector& ms_vecPedBedLAnimOffset = *(CVector*)0xC18C54;
    static inline CVector& ms_vecPedBedRAnimOffset = *(CVector*)0xC18C60;
    static inline CVector& ms_vecPedDeskAnimOffset = *(CVector*)0xC18C6C;
    static inline CVector& ms_vecPedChairAnimOffset = *(CVector*)0xC18C78;
    static inline CVector& ms_vecPedQuickDraggedOutCarAnimOffset = *(CVector*)0xC18C48;

public:
    static void InjectHooks();

    static void AddInCarAnim(const CVehicle* vehicle, CPed* ped, bool bAsDriver);
    static bool CarHasDoorToClose(const CVehicle* vehicle, int32 doorId);
    static bool CarHasDoorToOpen(const CVehicle* vehicle, int32 doorId);
    static bool CarHasOpenableDoor(const CVehicle* vehicle, int32 doorId_UnusedArg, const CPed* ped);
    static bool CarHasPartiallyOpenDoor(const CVehicle* vehicle, int32 doorId);
    static int32 ComputeDoorFlag(const CVehicle* vehicle, int32 doorId, bool bCheckVehicleType);
    static int32 ComputeOppositeDoorFlag(const CVehicle* vehicle, int32 doorId, bool bCheckVehicleType);
    static int32 ComputePassengerIndexFromCarDoor(const CVehicle* vehicle, int32 doorId);
    static CPed* ComputeSlowJackedPed(const CVehicle* vehicle, int32 doorId);
    static int32 ComputeTargetDoorToEnterAsPassenger(const CVehicle* vehicle, int32 nPassengerNum);
    static int32 ComputeTargetDoorToExit(const CVehicle* vehicle, const CPed* ped);
    static bool GetNearestCarDoor(const CPed* ped, const CVehicle* vehicle, CVector& outPos, int32& doorId);
    static bool GetNearestCarPassengerDoor(const CPed* ped, const CVehicle* vehicle, CVector* outVec, int32* doorId, bool CheckIfOccupiedTandemSeat, bool CheckIfDoorIsEnterable, bool CheckIfRoomToGetIn);
    static CVector GetPositionToOpenCarDoor(const CVehicle* vehicle, int32 doorId);
    static bool IsCarDoorInUse(const CVehicle* vehicle, int32 firstDoorId, int32 secondDoorId);
    static bool IsCarDoorReady(const CVehicle* vehicle, int32 doorId);
    static bool IsCarQuickJackPossible(CVehicle* vehicle, int32 doorId, const CPed* ped);
    static bool IsCarSlowJackRequired(const CVehicle* vehicle, int32 doorId);
    static bool IsClearToDriveAway(const CVehicle* outVehicle);
    static bool IsPathToDoorBlockedByVehicleCollisionModel(const CPed* ped, const CVehicle* vehicle, const CVector& pos);
    static bool IsPedHealthy(CPed* vehicle);
    static bool IsPlayerToQuitCarEnter(const CPed* ped, const CVehicle* vehicle, int32 startTime, CTask* task);
    static bool IsRoomForPedToLeaveCar(const CVehicle* vehicle, int32 doorId, CVector* pos = nullptr);
    static bool IsVehicleHealthy(const CVehicle* vehicle);
    static bool IsVehicleStealable(const CVehicle* vehicle, const CPed* ped);
    static void MakeUndraggedDriverPedLeaveCar(const CVehicle* vehicle, const CPed* ped);
    static void MakeUndraggedPassengerPedsLeaveCar(const CVehicle* targetVehicle, const CPed* draggedPed, const CPed* ped);
    static void QuitEnteringCar(CPed* ped, CVehicle* vehicle, int32 doorId, bool bCarWasBeingJacked);
    static void RemoveCarSitAnim(const CPed* ped);
    static void RemoveGetInAnims(const CPed* ped);
    static void SetAnimOffsetForEnterOrExitVehicle();
    static bool SetPedInCarDirect(CPed* ped, CVehicle* vehicle, int32 seatNumber, bool bAsDriver);
};
