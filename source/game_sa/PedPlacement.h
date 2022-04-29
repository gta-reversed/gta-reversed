/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CVector;
class CEntity;
class CVehicle;
class CPed;

class CPedPlacement {
public:
    static void InjectHooks();

    static bool FindZCoorForPed(CVector& inoutPos);
    static bool IsPositionClearForPed(const CVector& pos, float radius, int32 maxHitEntities, CEntity** outHitEntities, bool bCheckVehicles, bool bCheckPeds, bool bCheckObjects);
    static CVehicle* IsPositionClearOfCars(const CVector* pos);
    static CVehicle* IsPositionClearOfCars(const CPed* ped);
};
