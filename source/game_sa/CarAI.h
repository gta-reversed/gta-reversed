/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CVehicle;

class CCarAI {
public:
    static void InjectHooks();

    static void AddAmbulanceOccupants(CVehicle* veh);
    static void AddFiretruckOccupants(CVehicle* veh);
    static void AddPoliceCarOccupants(CVehicle* veh, bool bAlwaysCreatePassenger);

    static void BackToCruisingIfNoWantedLevel(CVehicle* veh);
    static void CarHasReasonToStop(CVehicle* veh);
    static bool EntitiesGoHeadOn(CEntity* entity1, CEntity* entity2);

    static eCarMission FindPoliceBikeMissionForWantedLevel();
    static eCarMission FindPoliceBoatMissionForWantedLevel();
    static eCarMission FindPoliceCarMissionForWantedLevel();
    static int32 FindPoliceCarSpeedForWantedLevel(CVehicle* veh);
    static float FindSwitchDistanceClose(CVehicle* veh);
    static float FindSwitchDistanceFar(CVehicle* veh);

    static float GetCarToGoToCoors(CVehicle* veh, const CVector& coors, eCarDrivingStyle drivingStyle, bool setCruiseSpeed);
    static float GetCarToGoToCoorsAccurate(CVehicle* veh, const CVector& coors, eCarDrivingStyle drivingStyle, bool setCruiseSpeed);
    static float GetCarToGoToCoorsRacing(CVehicle* veh, const CVector& coors, eCarDrivingStyle drivingStyle, bool setCruiseSpeed);
    static float GetCarToGoToCoorsStraightLine(CVehicle* veh, const CVector& coors, eCarDrivingStyle drivingStyle, bool setCruiseSpeed);
    static float GetCarToParkAtCoors(CVehicle* veh, const CVector& coors);

    static void MakeWayForCarWithSiren(CVehicle* veh);
    static void MellowOutChaseSpeed(CVehicle* veh);
    static void MellowOutChaseSpeedBoat(CVehicle* veh);

    static void TellCarToBlockOtherCar(CVehicle* veh1, CVehicle* veh2);
    static void TellCarToFollowOtherCar(CVehicle* veh1, CVehicle* veh2, float radius);
    static void TellCarToRamOtherCar(CVehicle* veh1, CVehicle* veh2);
    static void TellOccupantsToLeaveCar(CVehicle* veh);

    static void UpdateCarAI(CVehicle* veh);
};
