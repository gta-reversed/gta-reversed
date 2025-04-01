/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PathFind.h"
#include "Vector.h"

class CPhysical;
class CVehicle;
class CAutomobile;
class CHeli;
class CPlane;

constexpr auto GAME_SPEED_TO_METERS_PER_SECOND = 50.0f;
constexpr auto METERS_PER_SECOND_TO_GAME_SPEED = 1.0f / GAME_SPEED_TO_METERS_PER_SECOND;
constexpr auto GAME_SPEED_TO_CAR_AI_SPEED = 60.0f;
constexpr auto TIME_COPS_WAIT_TO_EXIT_AFTER_STOPPING = 2500;

class CCarCtrl {
public:
    static inline bool& bCarIsBeingCreated = *(bool*)0x9690CC;
    static uint32& NumLawEnforcerCars;
    static uint32& NumParkedCars;
    static uint32& NumAmbulancesOnDuty;
    static uint32& NumFireTrucksOnDuty;
    static uint32& MaxNumberOfCarsInUse;
    static float& CarDensityMultiplier;
    static int32& NumRandomCars;
    static int32& NumMissionCars;
    static int32& NumPermanentVehicles;
    static int32& LastTimeAmbulanceCreated;
    static int32& LastTimeFireTruckCreated;
    static bool& bAllowEmergencyServicesToBeCreated;
    static bool& bCarsGeneratedAroundCamera;
    static int8& CountDownToCarsAtStart;
    static float& TimeNextMadDriverChaseCreated;
    static int32& SequenceElements;
    static int32& SequenceRandomOffset;
    static bool& bSequenceOtherWay;
    static int32& LastTimeLawEnforcerCreated;

public:
    static void InjectHooks();

    static void Init();
    static void InitSequence(int32 SequenceElements);
    static void ReInit();
    static int32 ChooseBoatModel();
    static int32 ChooseCarModelToLoad(int32 arg1);
    static int32 ChooseModel(int32* arg1);
    static int32 ChoosePoliceCarModel(uint32 arg0);
    static eModelID ChooseGangCarModel(eGangID loadedCarGroupId);
    static void ClearInterestingVehicleList();
    static void ClitargetOrientationToLink(CVehicle* vehicle, CCarPathLinkAddress arg2, int8 arg3, float* arg4, float arg5, float arg6);
    static CVehicle* CreateCarForScript(int32 modelId, CVector posn, bool doMissionCleanup);
    static bool CreateConvoy(CVehicle* vehicle, int32 arg2);
    static bool CreatePoliceChase(CVehicle* vehicle, int32 arg2, CNodeAddress NodeAddress);
    static bool DealWithBend_Racing(CVehicle* vehicle, CCarPathLinkAddress LinkAddress1, CCarPathLinkAddress LinkAddress2, CCarPathLinkAddress LinkAddress3, CCarPathLinkAddress LinkAddress4, char arg6, char arg7, char arg8, char arg9, float arg10, float* arg11, float* arg12, float* arg13, float* arg14, CVector* pos);
    static void DragCarToPoint(CVehicle* vehicle, CVector* pos);
    static float FindAngleToWeaveThroughTraffic(CVehicle* vehicle, CPhysical* physical, float arg3, float arg4, float arg5);
    static void FindIntersection2Lines(float arg1, float arg2, float arg3, float arg4, float arg5, float arg6, float arg7, float arg8, float* arg9, float* arg10);
    static void FindLinksToGoWithTheseNodes(CVehicle* vehicle);
    static float FindMaximumSpeedForThisCarInTraffic(CVehicle* vehicle);
    static void FindNodesThisCarIsNearestTo(CVehicle* vehicle, CNodeAddress& nodeAddress1, CNodeAddress& nodeAddress2);
    static int8 FindPathDirection(CNodeAddress nodeAddress1, CNodeAddress nodeAddress2, CNodeAddress nodeAddress3, bool* arg4);
    static float FindPercDependingOnDistToLink(CVehicle* vehicle, CCarPathLinkAddress linkAddress);
    static int32 FindSequenceElement(int32 arg1);
    static float FindSpeedMultiplier(float arg1, float arg2, float arg3, float arg4);
    static float FindSpeedMultiplierWithSpeedFromNodes(int8 arg1);
    static float FindGhostRoadHeight(CVehicle* vehicle);
    static void FireHeliRocketsAtTarget(CAutomobile* entityLauncher, CEntity* entity);
    static void FlyAIHeliInCertainDirection(CHeli* heli, float arg2, float arg3, bool arg4);
    static void FlyAIHeliToTarget_FixedOrientation(CHeli* heli, float fOrientation, CVector posn);
    static void FlyAIPlaneInCertainDirection(CPlane* pPlane);
    static bool GenerateCarCreationCoors2(CVector posn, float radius, float arg3, float arg4, bool arg5, float arg6, float arg7, CVector* origin, CNodeAddress* nodeAddress1, CNodeAddress* nodeAddress2, float* arg11, bool arg12, bool arg13);
    static void GenerateEmergencyServicesCar();
    static CAutomobile* GenerateOneEmergencyServicesCar(uint32 modelID, CVector posn);
    static void GenerateOneRandomCar();
    static void GenerateRandomCars();
    static void GetAIHeliToAttackPlayer(CAutomobile* automobile);
    static void GetAIHeliToFlyInDirection(CAutomobile* automobile);
    static void GetAIPlaneToAttackPlayer(CAutomobile* automobile);
    static void GetAIPlaneToDoDogFight(CAutomobile* automobile);
    static void GetAIPlaneToDoDogFightAgainstPlayer(CAutomobile* automobile);
    static CVehicle* GetNewVehicleDependingOnCarModel(int32 modelID, uint8 createdBy);
    static bool IsAnyoneParking();
    static bool IsThisAnAppropriateNode(CVehicle* vehicle, CNodeAddress nodeAddress1, CNodeAddress nodeAddress2, CNodeAddress nodeAddress3, bool arg5);
    static bool IsThisVehicleInteresting(CVehicle* vehicle);
    static void JoinCarWithRoadAccordingToMission(CVehicle* vehicle);
    static void JoinCarWithRoadSystem(CVehicle* vehicle);
    static bool JoinCarWithRoadSystemGotoCoors(CVehicle* vehicle, const CVector& posn, bool unused = true, bool bIsBoat = false);
    static bool PickNextNodeAccordingStrategy(CVehicle* vehicle);
    static void PickNextNodeRandomly(CVehicle* vehicle);
    static bool PickNextNodeToChaseCar(CVehicle* vehicle, float destX, float destY, float destZ);
    static bool PickNextNodeToFollowPath(CVehicle* vehicle);
    static void PossiblyFireHSMissile(CVehicle* entityLauncher, CEntity* targetEntity);
    static void PossiblyRemoveVehicle(CVehicle* vehicle);
    static void PruneVehiclesOfInterest();
    static void ReconsiderRoute(CVehicle* vehicle);
    static void RegisterVehicleOfInterest(CVehicle* vehicle);
    static void RemoveCarsIfThePoolGetsFull();
    static void RemoveDistantCars();
    static void RemoveFromInterestingVehicleList(CVehicle* vehicle);
    static void ScanForPedDanger(CVehicle* vehicle);
    static bool ScriptGenerateOneEmergencyServicesCar(uint32 modelId, CVector posn);
    static void SetCoordsOfScriptCar(CVehicle* vehicle, float x, float y, float z, uint8 arg5, uint8 arg6);
    static void SetUpDriverAndPassengersForVehicle(CVehicle* vehicle, int32 arg2, int32 arg3, bool arg4, bool arg5, int32 passengersNum);
    template<typename PtrListType>
    static void SlowCarDownForCarsSectorList(PtrListType& ptrList, CVehicle* vehicle, float arg3, float arg4, float arg5, float arg6, float* arg7, float arg8);
    static void SlowCarDownForObject(CEntity* entity, CVehicle* vehicle, float* arg3, float arg4);
    template<typename PtrListType>
    static void SlowCarDownForObjectsSectorList(PtrListType& PtrList, CVehicle* vehicle, float arg3, float arg4, float arg5, float arg6, float* arg7, float arg8);
    static void SlowCarDownForOtherCar(CEntity* car1, CVehicle* car2, float* arg3, float arg4);
    template<typename PtrListType>
    static void SlowCarDownForPedsSectorList(PtrListType& PtrList, CVehicle* vehicle, float arg3, float arg4, float arg5, float arg6, float* arg7, float arg8);
    static void SlowCarOnRailsDownForTrafficAndLights(CVehicle* vehicle);
    static void SteerAIBoatWithPhysicsAttackingPlayer(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5);
    static void SteerAIBoatWithPhysicsCirclingPlayer(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5);
    static void SteerAIBoatWithPhysicsHeadingForTarget(CVehicle* vehicle, float arg2, float arg3, float* arg4, float* arg5, float* arg6);
    static void SteerAICarBlockingPlayerForwardAndBack(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5);
    static void SteerAICarParkParallel(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5);
    static void SteerAICarParkPerpendicular(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5);
    static void SteerAICarTowardsPointInEscort(CVehicle* vehicle1, CVehicle* vehicle2, float arg3, float arg4, float* arg5, float* arg6, float* arg7, bool* arg8);
    static void SteerAICarWithPhysics(CVehicle* vehicle);
    static void SteerAICarWithPhysicsFollowPath(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5);
    static void SteerAICarWithPhysicsFollowPath_Racing(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5);
    static void SteerAICarWithPhysicsFollowPreRecordedPath(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5);
    static void SteerAICarWithPhysicsHeadingForTarget(CVehicle* vehicle, CPhysical* target, float arg3, float arg4, float* arg5, float* arg6, float* arg7, bool* arg8);
    static void SteerAICarWithPhysicsTryingToBlockTarget(CVehicle* vehicle, CEntity* Unusued, float arg3, float arg4, float arg5, float arg6, float* arg7, float* arg8, float* arg9, bool* arg10);
    static void SteerAICarWithPhysicsTryingToBlockTarget_Stop(CVehicle* vehicle, float x, float y, float arg4, float arg5, float* arg6, float* arg7, float* arg8, bool* arg9);
    static void SteerAICarWithPhysics_OnlyMission(CVehicle* vehicle, float* arg2, float* arg3, float* arg4, bool* arg5);
    static void SteerAIHeliAsPoliceHeli(CAutomobile* automobile);
    static void SteerAIHeliFlyingAwayFromPlayer(CAutomobile* automobile);
    static void SteerAIHeliToCrashAndBurn(CAutomobile* automobile);
    static void SteerAIHeliToFollowEntity(CAutomobile* automobile);
    static void SteerAIHeliToKeepEntityInView(CAutomobile* automobile);
    static void SteerAIHeliToLand(CAutomobile* automobile);
    static void SteerAIHeliTowardsTargetCoors(CAutomobile* automobile);
    static void SteerAIPlaneToCrashAndBurn(CAutomobile* automobile);
    static void SteerAIPlaneToFollowEntity(CAutomobile* automobile);
    static void SteerAIPlaneTowardsTargetCoors(CAutomobile* automobile);
    static bool StopCarIfNodesAreInvalid(CVehicle* vehicle);
    static void SwitchBetweenPhysicsAndGhost(CVehicle* vehicle);
    static void SwitchVehicleToRealPhysics(CVehicle* vehicle);
    static float TestCollisionBetween2MovingRects(CVehicle* vehicle1, CVehicle* vehicle2, float arg3, float arg4, CVector* pos1, CVector* pos2);
    static float TestCollisionBetween2MovingRects_OnlyFrontBumper(CVehicle* vehicle1, CVehicle* vehicle2, float arg3, float arg4, CVector* pos1, CVector* pos2);
    static void TestWhetherToFirePlaneGuns(CVehicle* vehicle, CEntity* target);
    static bool ThisVehicleShouldTryNotToTurn(CVehicle* vehicle);
    static void TriggerDogFightMoves(CVehicle* vehicle1, CVehicle* vehicle2);
    static void UpdateCarCount(CVehicle* vehicle, uint8 bDecrease);
    static void UpdateCarOnRails(CVehicle* vehicle);
    static void WeaveForObject(CEntity* entity, CVehicle* vehicle, float* arg3, float* arg4);
    static void WeaveForOtherCar(CEntity* entity, CVehicle* vehicle, float* arg3, float* arg4);
    template<typename PtrListType>
    static void WeaveThroughCarsSectorList(PtrListType& ptrList, CVehicle* vehicle, CPhysical* physical, float arg4, float arg5, float arg6, float arg7, float* arg8, float* arg9);
    template<typename PtrListType>
    static void WeaveThroughObjectsSectorList(PtrListType& ptrList, CVehicle* vehicle, float arg3, float arg4, float arg5, float arg6, float* arg7, float* arg8);
    template<typename PtrListType>
    static void WeaveThroughPedsSectorList(PtrListType& ptrList, CVehicle* vehicle, CPhysical* physical, float arg4, float arg5, float arg6, float arg7, float* arg8, float* arg9);
    static float FindMaxSteerAngle(CVehicle* veh);
};
