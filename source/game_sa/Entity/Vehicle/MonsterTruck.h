/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Automobile.h"

enum eMonsterTruckNodes {
    MONSTER_NODE_NONE      = 0,
    MONSTER_CHASSIS        = 1,
    MONSTER_WHEEL_RF       = 2,
    MONSTER_WHEEL_RM       = 3,
    MONSTER_WHEEL_RB       = 4,
    MONSTER_WHEEL_LF       = 5,
    MONSTER_WHEEL_LM       = 6,
    MONSTER_WHEEL_LB       = 7,
    MONSTER_DOOR_RF        = 8,
    MONSTER_DOOR_RR        = 9,
    MONSTER_DOOR_LF        = 10,
    MONSTER_DOOR_LR        = 11,
    MONSTER_BUMP_FRONT     = 12,
    MONSTER_BUMP_REAR      = 13,
    MONSTER_WING_RF        = 14,
    MONSTER_WING_LF        = 15,
    MONSTER_BONNET         = 16,
    MONSTER_BOOT           = 17,
    MONSTER_WINDSCREEN     = 18,
    MONSTER_TRANSMISSION_F = 19,
    MONSTER_TRANSMISSION_R = 20,
    MONSTER_LOADBAY        = 21,
    MONSTER_MISC_A         = 22,

    MONSTER_NUM_NODES
};

class NOTSA_EXPORT_VTABLE CMonsterTruck : public CAutomobile {
public:
    float field_988[4]; // unused
    float m_fSuspensionRadius;

    static float& DUMPER_COL_ANGLEMULT; // 0.0002f

    static constexpr auto Type = VEHICLE_TYPE_MTRUCK;

public:
    CMonsterTruck(int32 modelIndex, eVehicleCreatedBy createdBy);
    ~CMonsterTruck() override = default; // 0x6C7D10, 0x6C7F90

    int32 ProcessEntityCollision(CEntity* entity, CColPoint* colPoint) override;
    void ProcessSuspension() override;
    void ProcessControlCollisionCheck(bool applySpeed) override;
    void ProcessControl() override;
    void SetupSuspensionLines() override;
    void PreRender() override;
    bool BurstTyre(uint8 tyreComponentId, bool bPhysicalEffect) override;
    bool SetUpWheelColModel(CColModel* colModel) override;
    void ResetSuspension() override;
    void ExtendSuspension();

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CMonsterTruck* Constructor(int32 modelIndex, eVehicleCreatedBy createdBy) { this->CMonsterTruck::CMonsterTruck(modelIndex, createdBy); return this; };
    CMonsterTruck* Destructor() { this->CMonsterTruck::~CMonsterTruck(); return this; };

};

VALIDATE_SIZE(CMonsterTruck, 0x99C);

extern float& fWheelExtensionRate; // 0.1
