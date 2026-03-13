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
    static constexpr size_t MAX_NUM_SAFEHOUSES{ 20 };
    static constexpr size_t MAX_CARS_IN_SAFEHOUSE{ 4 };
    static constexpr size_t MAX_CARS_IN_IMPOUND{ 3 };
    static constexpr size_t MAX_CARS_IN_GANTON_SAFEHOUSE{ 2 };
    static constexpr size_t MAX_NUM_GARAGES{ 50 };

    static_assert(MAX_CARS_IN_SAFEHOUSE >= MAX_CARS_IN_IMPOUND); // Impound vehicles are stored in safehouses too

    static inline CStoredCar (&aCarsInSafeHouse)[MAX_NUM_SAFEHOUSES][MAX_CARS_IN_SAFEHOUSE] = *(CStoredCar(*)[20][4])0x96ABD8; // NOTE: This was originally (incorrectly) at 0x96ABD4, so watch out!
    static inline CGarage    (&aGarages)[MAX_NUM_GARAGES] = *(CGarage(*)[50])0x96C048;
    static inline char       (&MessageIDString)[8] = *(char (*)[8])0x96C014;
    static inline int32&     LastGaragePlayerWasIn = *(int32*)0x96BFDC; // garage id
    static inline int32&     LastTimeHelpMessage = *(int32*)0x96BFE0;
    static inline bool&      bCamShouldBeOutside = *(bool*)0x96BFE4;
    static inline int32&     CrushedCarId = *(int32*)0x96BFE8;
    static inline int32      (&CarTypesCollected)[MAX_CARS_IN_SAFEHOUSE] = *(int32(*)[4])0x96BFEC;
    static inline int32&     PoliceCarsCollected = *(int32*)0x96BFFC;
    static inline int32&     BankVansCollected = *(int32*)0x96C000;
    static inline int32&     CarsCollected = *(int32*)0x96C004;
    static inline bool&      NoResprays = *(bool*)0x96C008;
    static inline int8&      RespraysAreFree = *(int8*)0x96C009;
    static inline int8&      BombsAreFree = *(int8*)0x96C00A;
    static inline int8&      PlayerInGarage = *(int8*)0x96C00B;
    static inline int32&     MessageNumberInString = *(int32*)0x96C00C;
    static inline int32&     MessageNumberInString2 = *(int32*)0x96C010;
    static inline uint32&    MessageEndTime = *(uint32*)0x96C01C;
    static inline uint32&    MessageStartTime = *(uint32*)0x96C020;
    static inline uint32&    NumGarages = *(uint32*)0x96C024;
    static inline CGarage*&  pOldToGarageWeAreIn = *(CGarage**)0x96BFD8;

public:
    static void InjectHooks();

    static void Init();
    static void Init_AfterRestart();
    static void Shutdown();
    static void Update();

    static void AddOne(
        CVector base,
        CVector2D p1,
        CVector2D p2,
        float ceilingZ,
        eGarageType type,
        uint32,
        const char* name,
        uint32 flagsIPL
    );
    static void CloseHideOutGaragesBeforeSave();
    static void PlayerArrestedOrDied();
    static void AllRespraysCloseOrOpen(bool state);

    static int32 FindSafeHouseIndexForGarageType(eGarageType type);
    static int16 FindGarageForObject(CObject*);
    static float FindDoorHeightForMI(uint32 modelIndex);

    static bool IsModelIndexADoor(int32 model);

    //! Check is point within any hideout garage (See `CGarage::IsHideOut`)
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
    static CGarage* FindNearestImpountGarage(CVector pos);
    static void StoreCarInNearestImpoundingGarage(CVehicle* vehicle);

    static void ChangeGarageType(int16 garageId, eGarageType type, uint32 unused);
    static bool HasCarBeenDroppedOffYet(int16 garageId) { return GetGarage(garageId).m_DoorState == GARAGE_DOOR_CLOSED_DROPPED_CAR; } // 0x447C90
    static bool HasResprayHappened(int16 garageId);
    static bool CameraShouldBeOutside() { return bCamShouldBeOutside; } // 0x448650
    static void GivePlayerDetonator();
    static void StopCarFromBlowingUp(CAutomobile* vehicle);
    static size_t CountCarsInHideoutGarage(eGarageType type);

    static void TriggerMessage(const char* tagMsg, int16 msgMin = -1, uint16 time = 5000, int16 msgMax = -1);
    static void PrintMessages();

    static bool Load();
    static bool Save();

    static CGarage&    GetGarage(int32 iGarageInd) { return aGarages[iGarageInd]; }
    static CStoredCar* GetStoredCarsInSafehouse(int32 iSafehouseInd) { return aCarsInSafeHouse[iSafehouseInd]; }
    static CStoredCar& GetStoredCar(int32 iSafehouseInd, int32 iCarInd) { return aCarsInSafeHouse[iSafehouseInd][iCarInd]; }

    static auto GetAll() { return aGarages | rng::views::take(NumGarages); }
};
