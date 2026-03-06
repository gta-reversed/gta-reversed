#pragma once

#include "Matrix.h"
#include "Vector.h"

class CEntity;
class CPhysical;
class CVehicle;

// original name, our previous name eBuoyancyPointState
enum class tWaterLevel : int32 {
    COMPLETELY_ABOVE_WATER = 0x0,
    COLLIDING_WITH_WATER   = 0x1,
    COMPLETELY_UNDER_WATER = 0x2,
};

struct CBuoyancyCalcStruct {
    float   fCurrentAverageContribution;
    float   fNewPointContribution;
    CVector vecCurOffsetTurnPoint;
    float   fAddedDistToWaterSurface;
    bool    bBuoyancyDataSummed;
};

VALIDATE_SIZE(CBuoyancyCalcStruct, 0x1C);

class cBuoyancy {
public:
    CVector m_vecPos;            // Position of the entity which buoyancy is being calculated
    CMatrix m_EntityMatrix;      // Matrix of the entity which buoyancy is being calculated
    uint32  pad;
    CVector m_vecInitialZPos;    // Holds some info about current entity, only ever used to retrieve its Z coordinate, which is already stored in m_vecPos field
    float   m_fWaterLevel;       // Z coordinate of water at entity position
    float   m_fUnkn2;            // 104
    float   m_fBuoyancy;         // 108
    CVector m_vecBoundingMax;    // Max bounding of the entity, doesn't necessarily match its collision as it's modified a bit
    CVector m_vecBoundingMin;    // Min bounding of the entity, doesn't necessarily match its collision as it's modified a bit
    float   m_fNumCheckedPoints; // How many points have been checked to calculate the buoyancy force, 9 points are checked at most for entity, used to keep track of incremental
                                 // average calculation
    uint32  field_8C;
    uint32  field_90;
    uint32  field_94;
    bool    m_bInWater;          // Is entity in water?
    uint8   field_99;
    uint8   field_9A;
    uint8   field_9B;
    CVector m_vecCenterOffset;           // Offset from min bounding to the center
    CVector m_vecNormalizedCenterOffset; // Same as m_vecCenterOffset, but scaled by longest component so it's 1.0F in the longest direction
    uint32  field_B4;
    bool    m_bFlipUnknVector;           // Should the currently calculated move force be inverted? (multiplied by -1.0F in the calculations)
    uint8   field_B9;
    bool    m_bProcessingBoat;           // Are we currently checking buoyancy of a boat?
    uint8   field_BB;                    // 187
    float   m_fEntityWaterImmersion;     // How much of the entity is immersed in water, [0:1] where 0 means entity is out of water, and 1 means that entity is completely submerged in
                                         // water
    CVector m_vecTurnPoint;              // Calculated buoyancy move force
    uint32  field_CC;                    // 204

    static inline auto& fPointVolMultiplier = StaticRef<float>(0x8D32C8);
    static inline auto& calcStruct = StaticRef<CBuoyancyCalcStruct>(0xC1C858);
    static inline auto (&afBoatVolumeDistribution)[3][3] = StaticRef<float[3][3]>(0x8D32CC); // 3x3 array of buoyancy modifiers for other boats
    static inline auto (&afBoatVolumeDistributionCat)[3][3] = StaticRef<float[3][3]>(0x8D32F0); // Catamaran volume distribution, unused in game, as there is no matching vehicle (Address fixed via DWARF)
    static inline auto (&afBoatVolumeDistributionSail)[3][3] = StaticRef<float[3][3]>(0x8D3314); // 3x3 array of buoyancy modifiers for sailboats
    static inline auto (&afBoatVolumeDistributionDinghy)[3][3] = StaticRef<float[3][3]>(0x8D3338); // 3x3 array of buoyancy modifiers for small boats
    static inline auto (&afBoatVolumeDistributionSpeed)[3][3] = StaticRef<float[3][3]>(0x8D335C); // 3x3 array of buoyancy modifiers for speedboats

public:
    static void InjectHooks();

    cBuoyancy() = default; // 0x6C2740
    ~cBuoyancy() = default; // 0x6C2B80

    bool ProcessBuoyancy(CPhysical* entity, float fBuoyancy, CVector* vecTurnSpeed, CVector* buoyancy);
    bool ProcessBuoyancyBoat(CVehicle* vehicle, float fBuoyancy, CVector* vecBuoyancyTurnPoint, CVector* vecBuoyancyForce, bool bUnderwater);
    bool CalcBuoyancyForce(CPhysical* entity, CVector* vecTurnSpeed, CVector* buoyancy);
    void PreCalcSetup(CPhysical* entity, float fBuoyancy);
    void AddSplashParticles(CPhysical* entity, CVector vecFrom, CVector vecTo, CVector vecSplashDir, uint8 bReduceParticleSize);
    void SimpleCalcBuoyancy(CPhysical* entity);
    float SimpleSumBuoyancyData(CVector* vecPointRelativeToSurface, tWaterLevel ePointState);
    void FindWaterLevel(const CVector& vecInitialZPos, CVector* outVecOffset, tWaterLevel* outInWaterState);
    void FindWaterLevelNorm(const CVector& vecInitialZPos, CVector* outVecOffset, tWaterLevel* outInWaterState, CVector* outVecNormal);
};

VALIDATE_SIZE(cBuoyancy, 0xD0);

extern cBuoyancy& mod_Buoyancy;
