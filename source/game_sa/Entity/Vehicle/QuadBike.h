/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Automobile.h"
#include "RideAnimData.h"

enum eQuadBikeNodes {
    QUAD_NODE_NONE = 0,
    QUAD_CHASSIS = 1,
    QUAD_WHEEL_RF = 2,
    QUAD_WHEEL_RM = 3,
    QUAD_WHEEL_RB = 4,
    QUAD_WHEEL_LF = 5,
    QUAD_WHEEL_LM = 6,
    QUAD_WHEEL_LB = 7,
    QUAD_DOOR_RF = 8,
    QUAD_DOOR_RR = 9,
    QUAD_DOOR_LF = 10,
    QUAD_DOOR_LR = 11,
    QUAD_BODY_FRONT = 12,
    QUAD_BODY_REAR = 13,
    QUAD_SUSPENSION_RF = 14,
    QUAD_SUSPENSION_LF = 15,
    QUAD_REAR_AXLE = 16,
    QUAD_HANDLEBARS = 17,
    QUAD_MISC_A = 18,
    QUAD_MISC_B = 19,

    QUAD_NUM_NODES
};

class NOTSA_EXPORT_VTABLE CQuadBike : public CAutomobile {
public:
    tBikeHandlingData* m_pHandling;
    CRideAnimData      m_sRideAnimData{ ANIM_GROUP_QUAD };
    float              field_9A8[4]; // unused
    uint8              m_nQuadFlags;

    static constexpr auto Type = VEHICLE_TYPE_QUAD;

public:
    CQuadBike(int32 modelIndex, eVehicleCreatedBy createdBy);
    ~CQuadBike() override = default; // 0x6CDC30

    void Fix() override;
    CRideAnimData* GetRideAnimData() override;
    void PreRender() override;
    bool ProcessAI(uint32& extraHandlingFlags) override;
    void ProcessControl() override;
    void ProcessControlInputs(uint8 playerNum) override;
    void ProcessDrivingAnims(CPed* driver, bool blend) override;
    void ProcessSuspension() override;
    void ResetSuspension() override;
    void SetupDamageAfterLoad() override;
    void SetupSuspensionLines() override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CQuadBike* Constructor(int32 modelIndex, eVehicleCreatedBy createdBy);

};

VALIDATE_SIZE(CQuadBike, 0x9BC);

extern bool& bDoQuadDamping; // true 0x8D3450
extern float& QUAD_HBSTEER_ANIM_MULT; // -0.4f 0x8D3454
extern CVector& vecQuadResistance; // { 0.995f, 0.995f, 1.0f } // 0x8D3458
