/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "cTransmission.h"
#include "Vector.h"
#include "eVehicleHandlingModelFlags.h"
#include "eVehicleHandlingFlags.h"

enum eVehicleLightsSize : uint8 {
    LIGHTS_LONG,
    LIGHTS_SMALL,
    LIGHTS_BIG,
    LIGHTS_TALL
};

struct tHandlingData {
public:
    int32 InitFromData(int32 id, const char* line);

    cTransmission& GetTransmission()       { return m_transmissionData; }
    auto           GetAnimGroupId()    const { return static_cast<AssocGroupId>((uint32)ANIM_GROUP_STDCARAMIMS + m_nAnimGroup); }

public:
    int32         m_nVehicleId;
    float         m_fMass; // 1.0 to 50000.0
    float         m_fMassRecpr; // = 1.f / m_fMass
    float         m_fTurnMass;
    float         m_fDragMult;
    CVector       m_vecCentreOfMass;   // x, y, z - 1.0 to 50000.0
    uint8         m_nPercentSubmerged; // 10 to 120 (> 100% vehicle sinks)
    float         m_fBuoyancyConstant;
    float         m_fTractionMultiplier; // 0.5 to 2.0
    cTransmission m_transmissionData;
    float         m_fBrakeDeceleration; // 0.1 to 10.0
    float         m_fBrakeBias;         // 0.0 > x > 1.0
    char          m_bABS;               // 0/1
    char          field_9D;
    char          field_9E;
    char          field_9F;
    float         m_fSteeringLock; // 10.0 to 40.0
    float         m_fTractionLoss;
    float         m_fTractionBias;
    float         m_fSuspensionForceLevel;     // not [L/M/H]
    float         m_fSuspensionDampingLevel;   // not [L/M/H]
    float         m_fSuspensionHighSpdComDamp; // often zero - 200.0 or more for bouncy vehicles
    float         m_fSuspensionUpperLimit;
    float         m_fSuspensionLowerLimit;
    float         m_fSuspensionBiasBetweenFrontAndRear;
    float         m_fSuspensionAntiDiveMultiplier;
    float         m_fCollisionDamageMultiplier; // 0.2 to 5.0
    union {
        eVehicleHandlingModelFlags m_nModelFlags;
        struct {
            uint32 m_bIsVan : 1;
            uint32 m_bIsBus : 1;
            uint32 m_bIsLow : 1;
            uint32 m_bIsBig : 1;
            uint32 m_bReverseBonnet : 1;
            uint32 m_bHangingBoot : 1;
            uint32 m_bTailgateBoot : 1;
            uint32 m_bNoswingBoot : 1;

            uint32 m_bNoDoors : 1;
            uint32 m_bTandemSeats : 1;
            uint32 m_bSitInBoat : 1;
            uint32 m_bConvertible : 1;
            uint32 m_bNoExhaust : 1;
            uint32 m_bDoubleExhaust : 1;
            uint32 m_bNo1fpsLookBehind : 1;
            uint32 m_bForceDoorCheck : 1;

            uint32 m_bAxleFNotlit : 1;
            uint32 m_bAxleFSolid : 1;
            uint32 m_bAxleFMcpherson : 1;
            uint32 m_bAxleFReverse : 1;
            uint32 m_bAxleRNotlit : 1;
            uint32 m_bAxleRSolid : 1;
            uint32 m_bAxleRMcpherson : 1;
            uint32 m_bAxleRReverse : 1;

            uint32 m_bIsBike : 1;
            uint32 m_bIsHeli : 1;
            uint32 m_bIsPlane : 1;
            uint32 m_bIsBoat : 1;
            uint32 m_bBouncePanels : 1;
            uint32 m_bDoubleRwheels : 1;
            uint32 m_bForceGroundClearance : 1;
            uint32 m_bIsHatchback : 1;
        };
    };
    union {
        eVehicleHandlingFlags m_nHandlingFlags;
        struct {
            uint32 m_b1gBoost : 1;
            uint32 m_b2gBoost : 1;
            uint32 m_bNpcAntiRoll : 1;
            uint32 m_bNpcNeutralHandl : 1;
            uint32 m_bNoHandbrake : 1;
            uint32 m_bSteerRearwheels : 1;
            uint32 m_bHbRearwheelSteer : 1;
            uint32 m_bAltSteerOpt : 1;

            uint32 m_bWheelFNarrow2 : 1;
            uint32 m_bWheelFNarrow : 1;
            uint32 m_bWheelFWide : 1;
            uint32 m_bWheelFWide2 : 1;
            uint32 m_bWheelRNarrow2 : 1;
            uint32 m_bWheelRNarrow : 1;
            uint32 m_bWheelRWide : 1;
            uint32 m_bWheelRWide2 : 1;

            uint32 m_bHydraulicGeom : 1;
            uint32 m_bHydraulicInst : 1;
            uint32 m_bHydraulicNone : 1;
            uint32 m_bNosInst : 1;
            uint32 m_bOffroadAbility : 1;
            uint32 m_bOffroadAbility2 : 1;
            uint32 m_bHalogenLights : 1;
            uint32 m_bProcRearwheelFirst : 1;
            uint32 m_bUseMaxspLimit : 1;

            uint32 m_bLowRider : 1;
            uint32 m_bStreetRacer : 1;
            uint32 m_bSwingingChassis : 1;
        };
    };
    float              m_fSeatOffsetDistance; // // ped seat position offset towards centre of car
    uint32             m_nMonetaryValue;      // 1 to 100000
    eVehicleLightsSize m_nFrontLights;
    eVehicleLightsSize m_nRearLights;
    uint8              m_nAnimGroup;          // TODO(Pirulax): to get AssocGroupId: `ANIM_GROUP_STDCARAMIMS + m_nAnimGroup`
};

VALIDATE_SIZE(tHandlingData, 0xE0);
