/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <Base.h>
#include "eLevelName.h"
#include "ePedRace.h"
#include "ePedStats.h"
#include <extensions/utility.hpp>

class CLoadedCarGroup;
class CPed;
class CDummy;
class CDummyObject;
class CZone;

enum eUpdatePedCounterState {
    UPDATE_PED_COUNTER_INCREASE = 0,
    UPDATE_PED_COUNTER_DECREASE = 1
};

// for the most part, pedGroupIds[0] is used, but in some cases
// it's used like this: pedGroupIds[CPopulation::CurrentWorldZone]
struct tPedGroupTranslationData {
    ePopcyclePedGroup pedGroupIds[3]{};
};
VALIDATE_SIZE(tPedGroupTranslationData, 0xC);

class CPopulation {
public:
    static float&  PedDensityMultiplier;
    static int32&  m_AllRandomPedsThisType;
    static uint32& MaxNumberOfPedsInUse;
    static uint32& NumberOfPedsInUseInterior;
    static inline tPedGroupTranslationData (&m_TranslationArray)[POPCYCLE_TOTAL_GROUPS] = *(tPedGroupTranslationData(*)[33])0x8D2540;
    static CLoadedCarGroup&          m_LoadedBoats;
    static CLoadedCarGroup&          m_InAppropriateLoadedCars;
    static CLoadedCarGroup&          m_AppropriateLoadedCars;
    static CLoadedCarGroup           (&m_LoadedGangCars)[10];
    static bool&                     bZoneChangeHasHappened;
    static uint8&                    m_CountDownToPedsAtStart;
    static uint32&                   ms_nTotalMissionPeds;
    static uint32&                   ms_nTotalPeds;
    static uint32&                   ms_nTotalGangPeds;
    static uint32&                   ms_nTotalCivPeds;
    static uint32&                   ms_nTotalCarPassengerPeds;
    static uint32&                   ms_nNumDealers;
    static std::array<uint32, TOTAL_GANGS>& ms_nNumGang;
    static uint32& ms_nNumEmergency;
    static uint32& ms_nNumCop;
    static uint32& ms_nNumCivFemale;
    static uint32& ms_nNumCivMale;
    static const uint16 m_DefaultModelIDForUnusedSlot = 2000; // means not loaded
    static bool&   m_bDontCreateRandomGangMembers;
    static bool&   m_bOnlyCreateRandomGangMembers;
    static bool&   m_bDontCreateRandomCops;
    static bool&   m_bMoreCarsAndFewerPeds;
    static bool&   bInPoliceStation;
    static uint32& NumMiamiViceCops;
    static uint32& CurrentWorldZone;

    //! Number of model IDs in each corresponding car group
    static inline auto& m_nNumCarsInGroup = StaticRef<std::array<uint16, (size_t)(POPCYCLE_TOTAL_CARGROUPS)>, 0xC0EC78>();

    //! Car model IDs in each corresponding car group
    static inline auto& m_CarGroups       = StaticRef<notsa::mdarray<int16, (size_t)(POPCYCLE_TOTAL_CARGROUPS), 23>, 0xC0ED38>();

    //! Number of model IDs in each corresponding ped group
    static inline auto& m_nNumPedsInGroup = StaticRef<std::array<uint16, (size_t)(POPCYCLE_TOTAL_PEDGROUPS)>, 0xC0ECC0>();

    //! Ped model IDs in each corresponding ped group
    static inline auto& m_PedGroups       = StaticRef<notsa::mdarray<int16, (size_t)(POPCYCLE_TOTAL_PEDGROUPS), 21>, 0xC0F358>();

public:
    static void InjectHooks();
    // static functions

    // returns ePedRace value
    static ePedRace FindPedRaceFromName(const char* modelName);
    // loads pedgrp.dat
    static void LoadPedGroups();
    // loads cargrp.dat
    static void LoadCarGroups();
    // init variables at game init and reinit
    static void Initialise();
    // empty function
    static void  Shutdown();
    static float FindDummyDistForModel(eModelID modelIndex);
    static float FindPedDensityMultiplierCullZone();
    // CWorld::Remove(ped); delete ped;
    static void RemovePed(CPed* ped);
    // returns 0
    static eModelID ChoosePolicePedOccupation();
    static bool  ArePedStatsCompatible(ePedStats statType1, ePedStats statType2);
    static bool  PedMICanBeCreatedAtAttractor(eModelID modelIndex);
    // checks if ped with specific model index can be created at script attractor
    static bool PedMICanBeCreatedAtThisAttractor(eModelID modelIndex, const char* attrName);
    static bool PedMICanBeCreatedInInterior(eModelID modelIndex);
    // is model male
    static bool IsMale(eModelID modelIndex);
    // is model female
    static bool IsFemale(eModelID modelIndex);
    // returns false
    static bool IsSecurityGuard(ePedType pedType);
    // checks if surface at this point is skateable
    static bool IsSkateable(const CVector& point);
    // calls CGangs::ChooseGangPedModel(int32); parameter - gang id?
    static eModelID ChooseGangOccupation(eGangID gangId);
    // empty function, returns ped
    static CPed* AddExistingPedInCar(CPed* ped, CVehicle* vehicle);

    /*!
    * @addr 0x611570
    * 
    * @brief updates ped counter, for updateState see eUpdatePedCounterState
    *
    * @param ped               The ped that was added/removed
    * @param pedAddedOrRemoved Should be `false` if the ped was added, or `true` if it was removed 
    */
    static void UpdatePedCount(CPed* ped, bool pedAddedOrRemoved);

    // empty function
    static void MoveCarsAndPedsOutOfAbandonedZones();
    // empty function
    static void DealWithZoneChange(eLevelName arg0, eLevelName arg1, bool arg2);
    // returns ped creation distance multiplier
    static float PedCreationDistMultiplier();
    static bool  IsSunbather(eModelID modelIndex);
    static bool  IsSunbather(CPed* ped); // NOTSA
    // returns false
    static bool CanSolicitPlayerOnFoot(eModelID modelIndex);
    // returns true if ped type is PEDTYPE_PROSTITUTE
    static bool CanSolicitPlayerInCar(eModelID modelIndex);
    // returns true if ped type is PEDTYPE_CIVMALE 
    static bool CanJeerAtStripper(eModelID modelIndex);
    static void PlaceGangMembers(ePedType pedType, uint32 numOfPeds, const CVector& posn);

    static void LoadSpecificDriverModelsForCar(eModelID carModelIndex);
    static void  RemoveSpecificDriverModelsForCar(eModelID carModelIndex);


    // returns ped model index
    static eModelID FindSpecificDriverModelForCar_ToUse(eModelID carModelIndex);
    static bool  IsCorrectTimeOfDayForEffect(const C2dEffectPedAttractor& effect);
    // return CPopulation::m_bMoreCarsAndFewerPeds? 1.7f : 1.0f;
    static float FindCarMultiplierMotorway();
    // return 1.0f;
    static float FindPedMultiplierMotorway();
    static void  ManagePed(CPed* ped, const CVector& playerPosn);
    static int32 FindNumberOfPedsWeCanPlaceOnBenches();
    static void  RemoveAllRandomPeds();
    static bool  TestRoomForDummyObject(CObject* object);
    static bool  TestSafeForRealObject(CDummyObject* dummyObject);
    static CPed* AddPed(ePedType pedType, eModelID modelIndex, const CVector& posn, bool makeWander);
    // creates male01 ped in front of car?
    static CPed* AddDeadPedInFrontOfCar(const CVector& posn, CVehicle* vehicle);
    static eModelID ChooseCivilianOccupation(
        bool         mustBeMale = false,
        bool         mustBeFemale = false,
        AssocGroupId mustUseThisAnimGroup = ANIM_GROUP_NONE,
        eModelID     mustNotBeThisModel = MODEL_INVALID,
        ePedStats    mustBeCompatibleWithThisPedStat = ePedStats::NONE,
        bool         bOnlyOnFoots = false,
        bool         doTestForUsedOccupations = true,
        bool         isAtAttractor = false, 
        const char*  attractorScriptName = nullptr
    );
    static void     ChooseCivilianCoupleOccupations(eModelID& husbandOccupation, eModelID& wifeyOccupation);
    static eModelID ChooseCivilianOccupationForVehicle(bool mustBeMale, CVehicle* vehicle);
    static void  CreateWaitingCoppers(CVector createAt, float createaWithHeading);
    // Creates ped in a vehicle. `carRating` can be set to -1
    static CPed* AddPedInCar(
        CVehicle* veh,
        bool      pedAsDriver,
        int32     carRating,
        int32     seatNumber,
        bool      mustBeMale,
        bool      isCriminal
    );
    static void  PlaceMallPedsAsStationaryGroup(const CVector& posn);
    static void  PlaceCouple(ePedType pedType1, eModelID modelIndex1, ePedType pedType2, eModelID modelIndex2, CVector placeAt);
    // Creates ped at attractor. decisionMakerType can be set to -1
    static bool  AddPedAtAttractor(eModelID modelIndex, C2dEffectPedAttractor* attractor, CVector posn, CEntity* entity, int32 decisionMakerType); 
    static float FindDistanceToNearestPedOfType(ePedType pedType, CVector posn);
    static float FindDistanceToNearestPed(CVector pos);
    static eModelID PickGangCar(eGangID forGang);
    static eModelID PickRiotRoadBlockCar();
    static void  ConvertToRealObject(CDummyObject* dummyObject);
    static void  ConvertToDummyObject(CObject* object);
    static bool  AddToPopulation(float arg0, float arg1, float arg2, float arg3);
    // returns number of generated peds?
    static int32 GeneratePedsAtAttractors(
        CVector pos,
        float   minRadius,
        float   maxRadius,
        float   minRadiusClose,
        float   maxRadiusClose,
        int32   decisionMaker,
        int32   numPedsToCreate
    );
    static void GeneratePedsAtStartOfGame();
    static void ManageObject(CObject* object, const CVector& posn);
    static void ManageDummy(CDummy* dummy, const CVector& posn);
    static void ManageAllPopulation();
    static void ManagePopulation();
    static void RemovePedsIfThePoolGetsFull();
    static void ConvertAllObjectsToDummyObjects();
    static void PopulateInterior(int32 numPeds, CVector posn);
    static void Update(bool generatePeds);
    static bool DoesCarGroupHaveModelId(int32 carGroupId, int32 modelId);
    static uint32 CalculateTotalNumGangPeds();

    static const auto& GetPedGroupsOfGroup(ePopcycleGroup popcycleGroup) { return m_TranslationArray[popcycleGroup].pedGroupIds; }
    static ePopcyclePedGroup GetPedGroupId(ePopcycleGroup popcycleGroup, size_t worldZone = 0) { return static_cast<ePopcyclePedGroup>(m_TranslationArray[popcycleGroup].pedGroupIds[worldZone]); }
    static ePopcyclePedGroup GetGangGroupId(eGangID gang, int32 worldZone = 0) { return static_cast<ePopcyclePedGroup>(m_TranslationArray[gang + POPCYCLE_GROUP_BALLAS].pedGroupIds[worldZone]); }
    static int32 GetNumPedsInGroup(ePopcycleGroup popcycleGroup, int32 worldZone) { return m_nNumPedsInGroup[GetPedGroupId(popcycleGroup, worldZone)]; }
    static int32 GetNumPedsInGroup(ePopcyclePedGroup pedGroup) { return m_nNumPedsInGroup[pedGroup]; }

    static auto GetModelsInPedGroup(ePopcyclePedGroup pedGroup) { return m_PedGroups[pedGroup] | rng::views::take((size_t)GetNumPedsInGroup(pedGroup)) | rng::views::transform([](auto model) {return (eModelID)model; }); }
    static int32 GetPedGroupModelId(ePopcyclePedGroup pedGroup, int32 slot) { return GetModelsInPedGroup(pedGroup)[slot]; } // Doing it like this for (out-of-range) debug checks
    static auto GetModelsInCarGroup(int32 carGroupId) { return m_CarGroups[carGroupId] | rng::views::take(m_nNumCarsInGroup[carGroupId]); }
private:
    static void UpdatePedCounts();
};
