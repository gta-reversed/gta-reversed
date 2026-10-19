/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vehicle.h"
#include "Vector.h"

#define STUCK_CAR_CHECK_SIZE 16

struct tStuckCar {
    int32   CarHandle;
    CVector LastPos;
    int32   LastChecked;
    float   StuckRadius;
    int32   CheckTime;
    bool    CarStuck;
    bool    WarpCar;
    bool    WarpIfStuck;
    bool    WarpIfFlipped;
    bool    WarpIfInWater;
    int8    NumberOfNodesToCheck;
};

class CStuckCarCheck {
public:
    std::array<tStuckCar, STUCK_CAR_CHECK_SIZE> m_aStuckCars;

    static void InjectHooks();

    void Init();
    void AddCarToCheck(int32 carHandle, float stuckRadius, uint32 time, bool warpCar, bool stuck, bool flipped, bool inWater, int8 numberOfNodesToCheck);
    bool AttemptToWarpVehicle(CVehicle* vehicle, CVector* origin, float orientation);
    void ClearStuckFlagForCar(int32 carHandle);
    bool HasCarBeenStuckForAWhile(int32 carHandle);
    bool IsCarInStuckCarArray(int32 carHandle);
    void Process();
    void RemoveCarFromCheck(int32 carHandle);
    void ResetArrayElement(uint16 carHandle);
    void ResetArrayElement(tStuckCar& car);
};

VALIDATE_SIZE(CStuckCarCheck, 0x240);
