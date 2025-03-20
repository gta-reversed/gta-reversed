/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <initializer_list>

class CVector;
class CEntity;
class CVehicle;
class CPed;

class CPedPlacement {
public:
    static void InjectHooks();

    static std::pair<CVector, bool> FindZCoorForPed(CVector pos);
    static bool IsPositionClearForPed(
        const CVector& pos,
        float          radius,
        int32          maxHitEntities = -1,
        CEntity**      outHitEntities = nullptr,
        bool           bCheckVehicles = true,
        bool           bCheckPeds = true,
        bool           bCheckObjects = true
    );
    static bool IsPositionClearForPed(
        const CVector&                  pos,
        float                           radius,
        std::initializer_list<CEntity*> ignoreEntities,
        bool                            bCheckVehicles = true,
        bool                            bCheckPeds = true,
        bool                            bCheckObjects = true
    );
    static CVehicle* IsPositionClearOfCars(const CVector* pos);
    static CVehicle* IsPositionClearOfCars(const CPed* ped);
};
