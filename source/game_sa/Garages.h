/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Garage.h"

class CVehicle;
class CObject;
class CVector;

class CGarages {
public:
    static constexpr auto MAX_NUM_SAFEHOUSES{ 20 };
    static constexpr auto MAX_CARS_IN_SAFEHOUSE{ 4 };
    static constexpr auto MAX_NUM_GARAGES{ 50 };

    static inline auto (&aCarsInSafeHouse)[MAX_NUM_SAFEHOUSES][MAX_CARS_IN_SAFEHOUSE] = StaticRef<CStoredCar[MAX_NUM_SAFEHOUSES][MAX_CARS_IN_SAFEHOUSE]>(0x96ABD8);
    static inline auto (&aGarages)[MAX_NUM_GARAGES] = StaticRef<CGarage[MAX_NUM_GARAGES]>(0x96C048);
    static inline auto (&MessageIDString)[8] = StaticRef<char[8]>(0x96C014);
    static inline auto& LastGaragePlayerWasIn = StaticRef<int32>(0x96BFDC);
    static inline auto& LastTimeHelpMessage = StaticRef<int32>(0x96BFE0);
    static inline auto& bCamShouldBeOutside = StaticRef<bool>(0x96BFE4);
    static inline auto& CrushedCarId = StaticRef<int32>(0x96BFE8);
    static inline auto (&CarTypesCollected)[MAX_CARS_IN_SAFEHOUSE] = StaticRef<int32[MAX_CARS_IN_SAFEHOUSE]>(0x96BFEC);
    static inline auto& PoliceCarsCollected = StaticRef<int32>(0x96BFFC);
    static inline auto& BankVansCollected = StaticRef<int32>(0x96C000);
    static inline auto& CarsCollected = StaticRef<int32>(0x96C004);
    static inline auto& NoResprays = StaticRef<bool>(0x96C008);
    static inline auto& RespraysAreFree = StaticRef<int8>(0x96C009);
    static inline auto& BombsAreFree = StaticRef<int8>(0x96C00A);
    static inline auto& PlayerInGarage = StaticRef<int8>(0x96C00B);
    static inline auto& MessageNumberInString = StaticRef<int32>(0x96C00C);
    static inline auto& MessageNumberInString2 = StaticRef<int32>(0x96C010);
    static inline auto& MessageEndTime = StaticRef<uint32>(0x96C01C);
    static inline auto& MessageStartTime = StaticRef<uint32>(0x96C020);
    static inline auto& NumGarages = StaticRef<int32>(0x96C024);
    static inline auto& pOldToGarageWeAreIn = StaticRef<CGarage*>(0x96BFD8);

public:
    static void InjectHooks();

    static void Init();
    static void Init_AfterRestart();
    static void Shutdown();
    static void Update();

    static void AddOne(float x1, float y1, float z1, float frontX, float frontY, float x2, float y2, float z2, uint8 type, uint32 a10, char* name, uint32 argFlags);
    static void CloseHideOutGaragesBeforeSave();
    static void PlayerArrestedOrDied();
    static void AllRespraysCloseOrOpen(bool state);

    static int32 FindSafeHouseIndexForGarageType(eGarageType type);
    static int16 FindGarageForObject(CObject*);
    static int16 FindGarageIndex(char* name);
    static float FindDoorHeightForMI(uint32 modelIndex);

    static bool IsModelIndexADoor(int32 model);
    static bool IsPointWithinHideOutGarage(Const CVector& point);
    static bool IsGarageOpen(int16 garageId);
    static bool IsGarageClosed(int16 garageId);
    static bool IsCarSprayable(CVehicle* vehicle);
    static bool IsPointInAGarageCameraZone(CVector point);
    static bool IsThisCarWithinGarageArea(int16 garageId, CEntity* entity);
    static bool IsPointWithinAnyGarage(CVector& point);

    static void ActivateGarage(int16 a1);
    static void DeActivateGarage(int16 garageId);
    static void SetTargetCarForMissionGarage(int16 garageId, CVehicle* vehicle);

    static int16 GetGarageNumberByName(const char* name);
    static void StoreCarInNearestImpoundingGarage(CVehicle* vehicle);

    static void ChangeGarageType(int16 garageId, eGarageType type, uint32 unused);
    static bool HasCarBeenDroppedOffYet(int16 garageId) { return GetGarage(garageId).m_nDoorState == GARAGE_DOOR_CLOSED_DROPPED_CAR; } // 0x447C90
    static bool HasResprayHappened(int16 garageId);
    static bool CameraShouldBeOutside() { return bCamShouldBeOutside; } // 0x448650
    static void GivePlayerDetonator();
    static void StopCarFromBlowingUp(CAutomobile* vehicle);
    static int32 CountCarsInHideoutGarage(eGarageType type);

    static void TriggerMessage(const char* tagMsg, int16 msgMin = -1, uint16 time = 5000, int16 msgMax = -1);
    static void PrintMessages();

    static bool Load();
    static bool Save();

    static CGarage&    GetGarage(int32 iGarageInd) { return aGarages[iGarageInd]; }
    static CStoredCar* GetStoredCarsInSafehouse(int32 iSafehouseInd) { return aCarsInSafeHouse[iSafehouseInd]; }
    static CStoredCar& GetStoredCar(int32 iSafehouseInd, int32 iCarInd) { return aCarsInSafeHouse[iSafehouseInd][iCarInd]; }
};
