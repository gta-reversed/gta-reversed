/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Bike.h"

#include "Buoyancy.h"



void CBike::InjectHooks() {
    RH_ScopedVirtualClass(CBike, 0x871360, 67);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(Constructor, 0x6BF430);
    RH_ScopedInstall(Destructor, 0x6B57A0);
    RH_ScopedInstall(dmgDrawCarCollidingParticles, 0x6B5A00);
    RH_ScopedInstall(DamageKnockOffRider, 0x6B5A10);
    RH_ScopedInstall(KnockOffRider, 0x6B5F40);
    RH_ScopedInstall(SetRemoveAnimFlags, 0x6B5F50, { .Reversed = false });
    RH_ScopedInstall(ReduceHornCounter, 0x6B5F90);
    RH_ScopedVMTInstall(ProcessAI, 0x6BC930, { .Reversed = false });
    RH_ScopedInstall(ProcessBuoyancy, 0x6B5FB0);
    RH_ScopedInstall(ResetSuspension, 0x6B6740, { .Reversed = false });
    RH_ScopedInstall(GetAllWheelsOffGround, 0x6B6790);
    RH_ScopedInstall(DebugCode, 0x6B67A0);
    RH_ScopedInstall(DoSoftGroundResistance, 0x6B6D40, { .Reversed = false });
    RH_ScopedInstall(PlayHornIfNecessary, 0x6B7130);
    RH_ScopedInstall(CalculateLeanMatrix, 0x6B7150);
    RH_ScopedInstall(ProcessRiderAnims, 0x6B7280, { .Reversed = false });
    RH_ScopedInstall(FixHandsToBars, 0x6B7F90, { .Reversed = false });
    RH_ScopedInstall(PlaceOnRoadProperly, 0x6BEEB0, { .Reversed = false });
    RH_ScopedInstall(GetCorrectedWorldDoorPosition, 0x6BF230, { .Reversed = false });
    RH_ScopedVMTInstall(Fix, 0x6B7050);
    RH_ScopedVMTInstall(BlowUpCar, 0x6BEA10, { .Reversed = false });
    RH_ScopedVMTInstall(ProcessDrivingAnims, 0x6BF400);
    RH_ScopedVMTInstall(BurstTyre, 0x6BEB20, { .Reversed = false });
    RH_ScopedVMTInstall(ProcessControlInputs, 0x6BE310, { .Reversed = false });
    RH_ScopedVMTInstall(ProcessEntityCollision, 0x6BDEA0);
    RH_ScopedVMTInstall(Render, 0x6BDE20);
    RH_ScopedVMTInstall(PreRender, 0x6BD090, { .Reversed = false });
    RH_ScopedVMTInstall(Teleport, 0x6BCFC0);
    RH_ScopedVMTInstall(ProcessControl, 0x6B9250);
    RH_ScopedVMTInstall(VehicleDamage, 0x6B8EC0, { .Reversed = false });
    RH_ScopedVMTInstall(SetupSuspensionLines, 0x6B89B0, { .Reversed = false });
    RH_ScopedVMTInstall(SetModelIndex, 0x6B8970);
    RH_ScopedVMTInstall(PlayCarHorn, 0x6B7080, { .Reversed = false });
    RH_ScopedVMTInstall(SetupDamageAfterLoad, 0x6B7070);
    RH_ScopedVMTInstall(DoBurstAndSoftGroundRatios, 0x6B6950, { .Reversed = false });
    RH_ScopedVMTInstall(SetUpWheelColModel, 0x6B67E0, { .Reversed = false });
    RH_ScopedVMTInstall(RemoveRefsToVehicle, 0x6B67B0);
    RH_ScopedVMTInstall(ProcessControlCollisionCheck, 0x6B6620);
    RH_ScopedVMTInstall(GetComponentWorldPosition, 0x6B5990);
    RH_ScopedVMTInstall(ProcessOpenDoor, 0x6B58D0);
}

// 0x6BF430
CBike::CBike(int32 modelIndex, eVehicleCreatedBy createdBy) : CVehicle(createdBy) {
    auto mi = CModelInfo::GetModelInfo(modelIndex)->AsVehicleModelInfoPtr();
    if (mi->m_nVehicleType == VEHICLE_TYPE_BIKE) {
        const auto& animationStyle = CAnimManager::GetAnimBlocks()[mi->GetAnimFileIndex()].GroupId;
        m_RideAnimData.AnimGroup = animationStyle;
        if (animationStyle < ANIM_GROUP_BIKES || animationStyle > ANIM_GROUP_WAYFARER) {
            m_RideAnimData.AnimGroup = ANIM_GROUP_BIKES;
        }
    }

    m_nVehicleSubType = VEHICLE_TYPE_BIKE;
    m_nVehicleType = VEHICLE_TYPE_BIKE;

    m_BlowUpTimer = 0.0f;
    m_nBrakesOn = false;
    nBikeFlags = 0;
    SetModelIndex(modelIndex);

    m_pHandlingData = gHandlingDataMgr.GetVehiclePointer(mi->m_nHandlingId);
    m_BikeHandling = gHandlingDataMgr.GetBikeHandlingPointer(mi->m_nHandlingId);
    m_nHandlingFlagsIntValue = m_pHandlingData->m_nHandlingFlags;
    m_pFlyingHandlingData = gHandlingDataMgr.GetFlyingPointer(static_cast<uint8>(mi->m_nHandlingId));
    m_fBrakeCount = 20.0f;
    mi->ChooseVehicleColour(m_nPrimaryColor, m_nSecondaryColor, m_nTertiaryColor, m_nQuaternaryColor, 1);
    m_fSwingArmLength = 0.0f;
    m_fForkYOffset = 0.0f;
    m_fForkZOffset = 0.0f;
    m_nFixLeftHand = false;
    m_nFixRightHand = false;
    m_fSteerAngleTan = std::tan(DegreesToRadians(mi->m_fBikeSteerAngle));
    m_fMass = m_pHandlingData->m_fMass;
    m_fTurnMass = m_pHandlingData->m_fTurnMass;
    m_vecCentreOfMass = m_pHandlingData->m_vecCentreOfMass;
    m_vecCentreOfMass.z = 0.1f;
    m_fAirResistance = GetDefaultAirResistance();
    m_fElasticity = 0.05f;
    m_fBuoyancyConstant = m_pHandlingData->m_fBuoyancyConstant;
    m_fSteerAngle = 0.0f;
    m_GasPedal = 0.0f;
    m_BrakePedal = 0.0f;
    m_Damager = nullptr;
    m_pWhoInstalledBombOnMe = nullptr;
    m_GasPedalAudioRevs = 0.0f;
    m_fTyreTemp = 1.0f;
    m_fBrakingSlide = 0.0f;
    m_PrevSpeed = 0.0f;

    for (auto i = 0; i < 2; ++i) {
        m_nWheelStatus[i] = 0;
        m_aWheelSkidmarkType[i] = eSkidmarkType::DEFAULT;
        m_bWheelBloody[i] = false;
        m_bMoreSkidMarks[i] = false;
        m_aWheelPitchAngles[i] = 0.0f;
        m_aWheelAngularVelocity[i] = 0.0f;
        m_aWheelSuspensionHeights[i] = 0.0f;
        m_aWheelOrigHeights[i] = 0.0f;
        m_WheelStates[i] = WHEEL_STATE_NORMAL;
    }

    for (auto i = 0; i < 4; ++i) {
        m_aWheelColPoints[i] = {};
        m_aWheelRatios[i] = 1.0f;
        m_aRatioHistory[i] = 0.0f;
        m_WheelCounts[i] = 0.0f;
        m_fSuspensionLength[i] = 0.0f;
        m_fLineLength[i] = 0.0f;
        m_aGroundPhysicalPtrs[i] = nullptr;
        m_aGroundOffsets[i] = CVector{};
    }

    m_nNoOfContactWheels = 0;
    m_NumDriveWheelsOnGround = 0;
    m_NumDriveWheelsOnGroundLastFrame = 0;
    m_fHeightAboveRoad = 0.0f;
    m_fExtraTractionMult = 1.0f;

    if (!mi->m_pColModel->m_pColData->m_pLines) {
        mi->m_pColModel->m_pColData->m_nNumLines = 4;
        mi->m_pColModel->m_pColData->m_pLines = static_cast<CColLine*>(CMemoryMgr::Malloc(4 * sizeof(CColLine)));
        mi->m_pColModel->m_pColData->m_pLines[1].m_vecStart.x = 99'999.99f; // todo: explain this
    }
    mi->m_pColModel->m_pColData->m_pLines[0].m_vecStart.z = 99'999.99f;
    CBike::SetupSuspensionLines();

    m_autoPilot.m_nTempAction = TEMPACT_NONE;
    m_autoPilot.SetCarMission(MISSION_NONE, 0);
    m_autoPilot.carCtrlFlags.bAvoidLevelTransitions = false;

    SetStatus(STATUS_SIMPLE);
    m_nNumPassengers = 0;
    vehicleFlags.bLowVehicle = false;
    vehicleFlags.bIsBig = false;
    vehicleFlags.bIsVan = false;

    m_bLeanMatrixCalculated = false;
    m_mLeanMatrix = *m_matrix;
    m_vecOldSpeedForPlayback = CVector{};
    m_vehicleAudio.Initialise(this);
}

// 0x6B57A0
CBike::~CBike() {
    m_vehicleAudio.Terminate();
}

// 0x6B5A00
void CBike::dmgDrawCarCollidingParticles(const CVector& position, float power, eWeaponType weaponType) {
    // NOP
}

// 0x6B5A10
bool CBike::DamageKnockOffRider(CVehicle* vehicle, float damageIntensity, uint16 pieceType, CEntity* damager, const CVector& collisionPos, const CVector& collisionImpactVelocity) {
    const auto driver    = vehicle->m_pDriver;
    const auto passenger = vehicle->m_apPassengers[0];

    // Impact force relative to the bike's mass
    auto force = damageIntensity / vehicle->m_fMass * 800.0f;

    // A skilled rider resists being knocked off (unless flagged to always come off)
    if (vehicle->GetStatus() != STATUS_PLAYER) {
        if (driver && driver->CantBeKnockedOffBike != CANT_BE_KNOCKED_OFF_ALWAYS_NORMAL) {
            force *= 1.0f - driver->GetBikeRidingSkill() * 0.6f;
        }
    } else {
        force *= 0.75f;
        if (driver) {
            force *= 1.0f - driver->GetBikeRidingSkill() * 0.5f;
        }
    }

    // Only an actual driver gets knocked off
    if (!driver || !driver->IsStateDriving() || force <= 10.0f) {
        return false;
    }

    // A ped already reacting to a hit isn't also knocked off (cops are exempt)
    if (const auto task = driver->GetIntelligence()->GetTaskManager().GetActiveTask()) {
        if (task->GetTaskType() == TASK_SIMPLE_BE_HIT && !driver->IsCop()) {
            return false;
        }
    }

    const auto impactFwdMag   = vehicle->GetForward().Dot(collisionImpactVelocity);
    const auto impactUpMag    = vehicle->GetUp().Dot(collisionImpactVelocity);
    const auto impactRightMag = vehicle->GetRight().Dot(collisionImpactVelocity);

    // Per-axis weighting of the impact
    auto fwdWeight = 0.6f;
    if (std::abs(impactFwdMag) > 0.85f) {
        const auto vertical = collisionImpactVelocity.z < 0.85f ? 0.0f : collisionImpactVelocity.z;
        fwdWeight = 7.0f * sq(vertical) + 0.6f;
    }
    if (vehicle->GetUp().z < 0.0f) { // bike lying on its side / upside down
        fwdWeight = 5.0f;
    }

    auto backWeight = 1.5f;
    auto upWeight   = 0.05f;
    if (vehicle->m_nModelIndex == MODEL_SANCHEZ) {
        fwdWeight *= 0.65f;
        upWeight  *= 0.75f;
    } else if (vehicle->IsSubQuad()) {
        backWeight = 3.0f;
        fwdWeight *= 0.65f;
        upWeight  *= 0.75f;
    }

    if (impactFwdMag > 0.0f) {
        fwdWeight *= 1.0f - driver->GetBikeRidingSkill() * 0.6f;
    }

    force *= std::abs(impactFwdMag) * fwdWeight
           + std::max(impactUpMag, 0.0f) * upWeight
           + std::abs(impactRightMag) * 0.45f
           - std::min(impactUpMag, 0.0f) * backWeight;

    // Don't knock the player off while they're on stairs
    if (driver->IsPlayer() && CCullZones::CamStairsForPlayer() && CCullZones::FindZoneWithStairsAttributeForPlayer()) {
        force = 0.0f;
    }

    // ALWAYS_HARD peds come off at a much lower force threshold
    if (force <= (driver->CantBeKnockedOffBike == CANT_BE_KNOCKED_OFF_ALWAYS_HARD ? 20.0f : 75.0f)) {
        return false;
    }

    // NEVER peds are never knocked off
    if (driver->CantBeKnockedOffBike == CANT_BE_KNOCKED_OFF_NEVER) {
        return false;
    }
    if (passenger && passenger->CantBeKnockedOffBike == CANT_BE_KNOCKED_OFF_NEVER) {
        return false;
    }

    // The driver (guaranteed present here) is thrown off, and so is the passenger, both reacting with the driver's facing
    const auto knockOffDir = (uint8)driver->GetLocalDirection(-CVector2D{ collisionImpactVelocity });

    driver->GetEventGroup().Add(CEventKnockOffBike{
        vehicle, vehicle->m_vecMoveSpeed, collisionImpactVelocity, damageIntensity, 0.05f * force, KNOCK_OFF_TYPE_SKIDBACKFRONT, knockOffDir, 0, nullptr, true, false
    });
    if (passenger) {
        passenger->GetEventGroup().Add(CEventKnockOffBike{
            vehicle, vehicle->m_vecMoveSpeed, collisionImpactVelocity, damageIntensity, 0.05f * force, KNOCK_OFF_TYPE_SKIDBACKFRONT, knockOffDir, 0, nullptr, false, false
        });
    }
    return true;
}

// dummy function
// 0x6B5F40
CPed* CBike::KnockOffRider(eWeaponType arg0, uint8 arg1, CPed* ped, bool arg3) {
    return ped;
}

// 0x6B5F50
void CBike::SetRemoveAnimFlags(CPed* ped) {
    ((void(__thiscall*)(CBike*, CPed*))0x6B5F50)(this, ped);
}

// 0x6B5F90
void CBike::ReduceHornCounter() {
    if (m_HornCounter)
        m_HornCounter -= 1;
}

// 0x6B5FB0
void CBike::ProcessBuoyancy() {
    CVector vecBuoyancyTurnPoint;
    CVector vecBuoyancyForce;
    if (!mod_Buoyancy.ProcessBuoyancy(this, m_fBuoyancyConstant, &vecBuoyancyTurnPoint, &vecBuoyancyForce)) {
        vehicleFlags.bIsDrowning = false;
        physicalFlags.bSubmergedInWater = false;
        physicalFlags.bTouchingWater = false;
        return;
    }

    physicalFlags.bTouchingWater = true;
    ApplyMoveForce(vecBuoyancyForce);
    ApplyTurnForce(vecBuoyancyForce, vecBuoyancyTurnPoint);

    auto fTimeStep = std::max(0.01F, CTimer::GetTimeStep());
    auto fUsedMass = m_fMass / 125.0F;
    auto fBuoyancyForceZ = vecBuoyancyForce.z / (fTimeStep * fUsedMass);

    if (fUsedMass > m_fBuoyancyConstant)
        fBuoyancyForceZ *= 1.05F * fUsedMass / m_fBuoyancyConstant;

    if (physicalFlags.bMakeMassTwiceAsBig)
        fBuoyancyForceZ *= 1.5F;

    auto fBuoyancyForceMult = std::max(0.5F, 1.0F - fBuoyancyForceZ / 20.0F);
    auto fSpeedMult = std::pow(fBuoyancyForceMult, CTimer::GetTimeStep());
    m_vecMoveSpeed *= fSpeedMult;
    m_vecTurnSpeed *= fSpeedMult;

    // 0x6B6443
    if (fBuoyancyForceZ > 0.8F || (fBuoyancyForceZ > 0.4F && IsAnyWheelNotMakingContactWithGround())) {
        vehicleFlags.bIsDrowning = true;
        physicalFlags.bSubmergedInWater = true;

        m_vecMoveSpeed.z = std::max(-0.1F, m_vecMoveSpeed.z);

        if (m_pDriver) {
            ProcessPedInVehicleBuoyancy(m_pDriver->AsPed(), true);
        }
        else {
            vehicleFlags.bEngineOn = false;
        }

        for (const auto passenger : GetPassengers()) {
            ProcessPedInVehicleBuoyancy(passenger, false);
        }
    }
    else {
        vehicleFlags.bIsDrowning = false;
        physicalFlags.bSubmergedInWater = false;
    }
}

inline void CBike::ProcessPedInVehicleBuoyancy(CPed* ped, bool bIsDriver) {
    if (!ped)
        return;

    ped->physicalFlags.bTouchingWater = true;
    if (!ped->IsPlayer() && bikeFlags.bWaterTight)
        return;

    if (ped->IsPlayer())
        ped->AsPlayer()->HandlePlayerBreath(true, 1.0F);

    if (IsAnyWheelMakingContactWithGround()) {
        if (!ped->IsPlayer()) {
            auto pedDamageResponseCalc = CPedDamageResponseCalculator(this, CTimer::GetTimeStep(), eWeaponType::WEAPON_DROWNING, PED_PIECE_TORSO, false);
            auto damageEvent = CEventDamage(this, CTimer::GetTimeInMS(), eWeaponType::WEAPON_DROWNING, PED_PIECE_TORSO, 0, false, true);
            if (damageEvent.AffectsPed(ped))
                pedDamageResponseCalc.ComputeDamageResponse(ped, damageEvent.m_damageResponse, true);
            else
                damageEvent.m_damageResponse.m_bDamageCalculated = true;

            ped->GetEventGroup().Add(&damageEvent, false);
        }
    } else {
        auto knockOffBikeEvent = CEventKnockOffBike(this, m_vecMoveSpeed, m_vecLastCollisionImpactVelocity, m_fDamageIntensity, 0.0F, KNOCK_OFF_TYPE_FALL, 0, 0, nullptr, bIsDriver, false);
        ped->GetEventGroup().Add(&knockOffBikeEvent);
        if (bIsDriver) {
            vehicleFlags.bEngineOn = false;
        }
    }
}

// 0x6BC930
bool CBike::ProcessAI(uint32& extraHandlingFlags) {
    return plugin::CallMethodAndReturn<bool, 0x6BC930, CBike*, uint32&>(this, extraHandlingFlags);
}

// 0x6BF400
void CBike::ProcessDrivingAnims(CPed* driver, bool blend) {
    if (m_bOffscreen && GetStatus() == STATUS_PLAYER)
        return;

    ProcessRiderAnims(driver, this, &m_RideAnimData, m_BikeHandling, 0);
}

// 0x6B7280
void CBike::ProcessRiderAnims(CPed* rider, CVehicle* vehicle, CRideAnimData* rideData, tBikeHandlingData* handling, int16 a5) {
    plugin::Call<0x6B7280, CPed*, CVehicle*, CRideAnimData*, tBikeHandlingData*, int16>(rider, vehicle, rideData, handling, a5);
}

// 0x6BEB20
bool CBike::BurstTyre(uint8 tyreComponentId, bool bPhysicalEffect) {
    return plugin::CallMethodAndReturn<bool, 0x6BEB20, CBike*, uint8, bool>(this, tyreComponentId, bPhysicalEffect);
}

// 0x6BE310
void CBike::ProcessControlInputs(uint8 playerNum) {
    plugin::CallMethod<0x6BE310, CBike*, uint8>(this, playerNum);
}

// 0x6BDEA0
int32 CBike::ProcessEntityCollision(CEntity* entity, CColPoint* outColPoints) {
    if (GetStatus() != STATUS_SIMPLE) {
        vehicleFlags.bVehicleColProcessed = true;
    }

    const auto tcd = GetColData(),
               ocd = entity->GetColData();

#ifdef FIX_BUGS // Text search for `FIX_BUGS@CAutomobile::ProcessEntityCollision:1`
    if (!tcd || !ocd) {
        return 0;
    }
#endif

    if (physicalFlags.bSkipLineCol || physicalFlags.bProcessingShift || entity->GetIsTypePed()) {
        tcd->m_nNumLines = 0; // Later reset back to original value
    }

    const auto ogWheelRatios = m_aWheelRatios;

    auto numColPts = CCollision::ProcessColModels(
        GetMatrix(), *GetColModel(),
        entity->GetMatrix(), *entity->GetColModel(),
        *(std::array<CColPoint, 32>*)(outColPoints),
        m_aWheelColPoints.data(),
        m_aWheelRatios.data(),
        false
    );

    // Possibly add driver & entity collisions to `outColPoints`
    if (m_pDriver && m_nTestPedCollision) {
        const auto pcd = m_pDriver->GetColData();
        if (!pcd->m_nNumLines) {
            std::array<CColPoint, 32> pedCPs{};

            CMatrix driverMat = GetMatrix();
            driverMat.GetPosition() += GetDriverSeatDummyPositionWS();

            std::array<CColPoint, 32> pedEntityColPts{};
            const auto numPedEntityColPts = CCollision::ProcessColModels(
                driverMat, *m_pDriver->GetColModel(),
                entity->GetMatrix(), *entity->GetColModel(),
                pedEntityColPts,
                nullptr,
                nullptr,
                false
            );

            if (numPedEntityColPts) {
                if (m_nTestPedCollision == 1) {
                    m_nTestPedCollision = 0;
                } else {
                    for (auto i = 0; i < numPedEntityColPts && numColPts < 32; i++) {
                        const auto& pedEntityCP = pedCPs[i];
                        if (pedEntityCP.m_nPieceTypeA == PED_COL_SPHERE_LEG) {
                            continue;
                        }
                        outColPoints[numColPts++] = pedEntityCP;
                    }
                }
            }
        }
    }
    
    size_t numProcessedLines{};
    if (tcd->m_nNumLines) {
        // Process the real wheels
        for (auto i = 0; i < NUM_SUSP_LINES; i++) {
            const auto& cp = m_aWheelColPoints[i];

            const auto wheelColPtsTouchDist = m_aWheelRatios[i];
            if (wheelColPtsTouchDist >= 1.f || wheelColPtsTouchDist >= ogWheelRatios[i]) {
                continue;
            }

            numProcessedLines++;

            m_anCollisionLighting[i] = cp.m_nLightingB;
            m_nContactSurface = cp.m_nSurfaceTypeB;

            switch (entity->GetType()) {
            case ENTITY_TYPE_VEHICLE:
            case ENTITY_TYPE_OBJECT: {
                CEntity::ChangeEntityReference(m_aGroundPhysicalPtrs[i], entity->AsPhysical());

                m_aGroundOffsets[i] = cp.m_vecPoint - entity->GetPosition();
                if (entity->GetIsTypeVehicle()) {
                    m_anCollisionLighting[i] = entity->AsVehicle()->m_anCollisionLighting[i];
                }
                break;
            }
            case ENTITY_TYPE_BUILDING: {
                m_pEntityWeAreOn = entity;
                m_bTunnel = entity->m_bTunnel;
                m_bTunnelTransition = entity->m_bTunnelTransition;
                break;
            }
            }
        }
    } else {
        tcd->m_nNumLines = NUM_SUSP_LINES;
    }

    if (numColPts > 0 || numProcessedLines > 0) {
        AddCollisionRecord(entity);
        if (!entity->GetIsTypeBuilding()) {
            entity->AsPhysical()->AddCollisionRecord(this);
        }
        if (numColPts > 0) {
            if (   entity->GetIsTypeBuilding()
                || (entity->GetIsTypeObject() && entity->AsPhysical()->physicalFlags.bDisableCollisionForce)
            ) {
                SetHasHitWall(true);
            }
        }
    }

    return numColPts;
}

// 0x6B9250
void CBike::ProcessControl() {
    // `extraHandlingFlags` bits set by `ProcessAI` and `DoSoftGroundResistance` (bike meaning, differs from `eExtraHandlingFlags`)
    constexpr uint32 EXTRA_HANDLING_BIKE_BALANCED    = 0x2; // A rider balances the bike (VC: bBalancedByRider)
    constexpr uint32 EXTRA_HANDLING_BIKE_SOFT_GROUND = 0x4; // Wheels on sand/soft ground (VC: bStuckInSand)

    static auto& s_TurnResistance         = StaticRef<CVector>(0x8D3238); // { 0.9995f, 0.9f, 0.95f } (VC: vecTestResistance)
    static auto& s_TractionScale          = StaticRef<float>(0xC1C818);   // 10.0f / (50.0f * 50.0f), set by the static initializer at 0x853600
    static auto& s_StoppieSideForceMult   = StaticRef<float>(0x8D3280);   // 0.05f
    static auto& s_WheelieLeanMult        = StaticRef<float>(0x8D3284);   // -0.1f
    static auto& s_WheelieSteerForceMult  = StaticRef<float>(0x8D3288);   // 0.01f
    static auto& s_WheelStates            = StaticRef<std::array<tWheelState, 2>>(0xC1C26C);
    static auto& s_Thrust                 = StaticRef<float>(0xC1C27C);

    auto* const colData = GetColModel()->m_pColData;
    auto* const mi      = CModelInfo::GetVehicleModelInfo(m_nModelIndex);
    uint32 extraHandlingFlags = 0;

    // 0x6B9298 - Per-frame vehicle audio update (engine, skids, ...). SA-MP NOPs this call.
    m_vehicleAudio.Service();

    vehicleFlags.bWarnedPeds        = false;
    m_bLeanMatrixCalculated         = false;
    m_nBrakesOn                     = 0; // Really "doing burnout", see the rear wheel code
    bikeFlags.bPlayerBoost          = false;
    vehicleFlags.bRestingOnPhysical = false;

    if (CReplay::Mode == MODE_PLAYBACK) {
        return;
    }

    ProcessCarAlarm();
    ActivateBombWhenEntered();
    UpdateClumpAlpha();

    // 0x6B92FA - Ped collision tests are only done for bikes ridden by a player (1 = first frame, 2 = active)
    // NOTE: A player passenger only counts if there is a driver
    if (m_pDriver && (m_pDriver->IsPlayer() || m_apPassengers[0] && m_apPassengers[0]->IsPlayer())) {
        if (m_nTestPedCollision == 1) {
            m_nTestPedCollision = 2;
        } else if (m_nTestPedCollision == 0) {
            m_nTestPedCollision = 1;
        }
    } else {
        m_nTestPedCollision = 0;
    }

    ProcessAI(extraHandlingFlags); // NOTE: Unlike `CAutomobile`, the return value is ignored
    if (GetStatus() == STATUS_SIMPLE) {
        return;
    }

    // 0x6B9363 - Fall off the side stand when tilted too far
    if (bikeFlags.bOnSideStand && (std::fabs(GetRight().z) > 0.35f || std::fabs(GetForward().z) > 0.5f)) {
        bikeFlags.bOnSideStand = false;
    }

    // 0x6B939E - Centre of mass and roll/pitch turn resistance
    if (!(extraHandlingFlags & EXTRA_HANDLING_BIKE_BALANCED) && !bikeFlags.bGettingPickedUp && !bikeFlags.bOnSideStand) {
        m_vecCentreOfMass.x = m_pHandlingData->m_vecCentreOfMass.x;
        m_vecCentreOfMass.y = m_pHandlingData->m_vecCentreOfMass.y;
        m_vecCentreOfMass.z = m_BikeHandling->m_fNoPlayerCOMz;
    } else {
        constexpr float TURN_DAMP_X       = 1.0f;    // 0x8712D8 (VC: fDAxisX)
        constexpr float TURN_DAMP_X_EXTRA = 100.0f;  // 0x8712DC (VC: fDAxisXExtra)
        constexpr float IN_AIR_X_RES      = 0.98f;   // 0x8712E0 (VC: fInAirXRes)
        constexpr float TURN_DAMP_Y       = 1000.0f; // 0x8712E4 (VC: fDAxisY)

        float   turnDampX = TURN_DAMP_X;
        CVector resistance = s_TurnResistance;
        const CVector localTurnSpeed = GetMatrix().InverseTransformVector(m_vecTurnSpeed);
        if (GetStatus() == STATUS_PLAYER) {
            if (m_aWheelRatios[0] < 1.0f || m_aWheelRatios[1] < 1.0f) {
                // 0x6B94F4 - Front wheel down, rear wheel off the ground for a while: stoppie
                if (m_WheelCounts[2] <= 0.0f && m_WheelCounts[3] <= 0.0f) {
                    turnDampX = TURN_DAMP_X_EXTRA;
                    const float maxStoppieRes  = CStats::GetFatAndMuscleModifier(STAT_MOD_13) * 0.075f;
                    const float stoppieResMult = CStats::GetFatAndMuscleModifier(STAT_MOD_13) * 0.25f;
                    if (GetForward().z < 0.0f) {
                        resistance.x *= std::min(std::fabs(m_BikeHandling->m_fStoppieAng - GetForward().z) * stoppieResMult, maxStoppieRes) + 0.9f;
                    }
                }
            } else {
                // 0x6B944F - Front wheel in the air: wheelie or airborne
                turnDampX = TURN_DAMP_X_EXTRA;
                const float wheelieResMult = CStats::GetFatAndMuscleModifier(STAT_MOD_13) * 0.2f;
                if ((m_aWheelRatios[2] < 1.0f || m_aWheelRatios[3] < 1.0f) && GetForward().z > 0.0f) {
                    resistance.x -= std::min(std::fabs(m_BikeHandling->m_fWheelieAng - GetForward().z) * wheelieResMult, 0.05f);
                } else {
                    resistance.x = IN_AIR_X_RES;
                }
            }
        }

        // 0x6B95AD
        resistance.x /= sq(localTurnSpeed.x) * turnDampX + 1.0f;
        resistance.y /= TURN_DAMP_Y * localTurnSpeed.y * localTurnSpeed.y + 1.0f;
        resistance.x = std::pow(resistance.x, CTimer::GetTimeStep());
        resistance.y = std::pow(resistance.y, CTimer::GetTimeStep());
        const float turnX = localTurnSpeed.x * resistance.x - localTurnSpeed.x;
        const float turnY = resistance.y * localTurnSpeed.y - localTurnSpeed.y;

        const CVector comOffset = GetMatrix().TransformVector(m_vecCentreOfMass);
        ApplyTurnForce(-GetUp() * turnY * m_fTurnMass, comOffset + GetRight());
        ApplyTurnForce(GetUp() * turnX * m_fTurnMass, comOffset + GetForward());

        if (GetStatus() != STATUS_PLAYER) {
            m_vecCentreOfMass = m_pHandlingData->m_vecCentreOfMass;
        }
    }

    // 0x6B97C9 - Abandoned/wrecked bikes that barely move stop being simulated
    bool skipPhysics = false;
    if (!GetIsStuck() && (GetStatus() == STATUS_ABANDONED || GetStatus() == STATUS_WRECKED) && !bikeFlags.bGettingPickedUp) {
        const bool isVehicleIdle = !vehicleFlags.bVehicleColProcessed
            && m_vecMoveSpeed == 0.0f
            && m_aRatioHistory[3] != 1.0f;

        float forceLimitRadius  = 0.003f;
        float torqueLimitRadius = 0.00090000004f;
        float movingSpeedLimit  = 0.005f;
        if (GetStatus() == STATUS_WRECKED) {
            forceLimitRadius  = 0.006f;
            torqueLimitRadius = 0.0015f;
            movingSpeedLimit  = 0.015f;
        }

        m_vecForce  = (m_vecForce + m_vecMoveSpeed) / 2.0f;
        m_vecTorque = (m_vecTorque + m_vecTurnSpeed) / 2.0f;

        forceLimitRadius  *= CTimer::GetTimeStep();
        torqueLimitRadius *= CTimer::GetTimeStep();
        if (m_vecForce.SquaredMagnitude() <= sq(forceLimitRadius)
            && m_vecTorque.SquaredMagnitude() <= sq(torqueLimitRadius)
            && m_fMovingSpeed < movingSpeedLimit
            || isVehicleIdle
        ) {
            m_nFakePhysics++;
            // NOTE: The original also tests `!CCarCtrl::MapCouldMoveInThisArea(GetPosition().x, GetPosition().y)` (0x424100), a stub that always returns false
            if (m_nFakePhysics > 10 || isVehicleIdle) {
                if (!isVehicleIdle || m_nFakePhysics > 10) {
                    m_nFakePhysics = 10;
                }
                ResetMoveSpeed();
                ResetTurnSpeed();
                skipPhysics = true;
            }
        } else {
            m_nFakePhysics = 0;
        }
    }

    // 0x6B9A44
    for (const auto* const groundPhysical : m_aGroundPhysicalPtrs) {
        if (groundPhysical) {
            vehicleFlags.bRestingOnPhysical = true;
            if (!CWorld::bForceProcessControl && groundPhysical->GetIsInSafePosition()) {
                SetWasPostponed(true);
                return;
            }
        }
    }

    if (vehicleFlags.bRestingOnPhysical) {
        skipPhysics    = false;
        m_nFakePhysics = 0;
    }

    VehicleDamage(0.0f, eVehicleCollisionComponent::DEFAULT, nullptr, nullptr, nullptr, WEAPON_RAMMEDBYCAR);

    // 0x6B9AAB - Hit sideways while (almost) standing, or being picked up: lateral wheel forces and the ground normal are kept level
    const bool bKeepUpright = m_fDamageIntensity > 0.0f
        && std::fabs(m_vecLastCollisionImpactVelocity.Dot(GetRight())) > 0.5f
        && m_vecMoveSpeed.SquaredMagnitude() < 0.1f
        || bikeFlags.bGettingPickedUp;

    if (skipPhysics) {
        // 0x6B9B39
        SkipPhysics();
        vehicleFlags.bVehicleColProcessed = false;
        vehicleFlags.bAudioChangingGear   = false;

        if (bikeFlags.bOnSideStand) {
            if (m_RideAnimData.BarSteerAngle < 0.34906587f) { // ~20 deg
                m_RideAnimData.BarSteerAngle += CTimer::GetTimeStep() * DegreesToRadians(1.0f);
            }

            // 0x6B9B90 - Settle on the side stand
            const float leanMult = std::pow(0.97f, CTimer::GetTimeStep());
            m_RideAnimData.DesiredLeanAngle = leanMult * m_RideAnimData.DesiredLeanAngle - (1.0f - leanMult) * (std::asin(std::clamp(GetRight().z, -1.0f, 1.0f)) + DegreesToRadians(15.0f));
            m_RideAnimData.LeanAngle        = m_RideAnimData.DesiredLeanAngle;
        }
    } else {
        // 0x6B9C3A
        if (!vehicleFlags.bVehicleColProcessed) {
            ProcessControlCollisionCheck(true);
        }

        // 0x6B9C4C - Riderless bikes turn the handlebars to the side they lean to
        if (!(extraHandlingFlags & EXTRA_HANDLING_BIKE_BALANCED) && !bikeFlags.bGettingPickedUp && !bikeFlags.bOnSideStand) {
            if (GetRight().z < 0.0f) {
                if (m_fSteerAngle > -DegreesToRadians(25.0f)) {
                    m_fSteerAngle -= CTimer::GetTimeStep() * DegreesToRadians(0.5f);
                }
            } else if (m_fSteerAngle < DegreesToRadians(25.0f)) {
                m_fSteerAngle += CTimer::GetTimeStep() * DegreesToRadians(0.5f);
            }
        }

        // 0x6B9CBB - Rider leaning forward (`bike_fwd` anim): less drag and a small push
        const float airResistance = m_fAirResistance;
        if (GetStatus() == STATUS_PLAYER && m_pDriver) {
            const auto* const fwdAnim = RpAnimBlendClumpGetAssociation(m_pDriver->GetRpClump(), ANIM_ID_BIKE_FWD);
            if (fwdAnim && fwdAnim->GetBlendAmount() > 0.5f && fwdAnim->GetCurrentTime() > 0.06f && fwdAnim->GetCurrentTime() < 0.14f) {
                m_fAirResistance *= CCullZones::DoExtraAirResistanceForPlayer() ? 0.85f : 0.6f;
                if (m_GasPedal > 0.5f && m_vecMoveSpeed.Dot(GetForward()) > 0.25f) {
                    ApplyMoveForce(CTimer::GetTimeStep() * m_fMass * 0.0016000001f * GetForward());
                    bikeFlags.bPlayerBoost = true;
                }
            }
        }

        // 0x6B9DCE
        const bool wasSubmergedInWater = physicalFlags.bSubmergedInWater;
        CPhysical::ProcessControl();
        m_fAirResistance = airResistance;
        ProcessBuoyancy();

        if (!wasSubmergedInWater && physicalFlags.bSubmergedInWater) {
            if (m_pDriver && m_pDriver->IsPlayer()) {
                m_pDriver->AsPlayer()->ResetPlayerBreath();
            } else {
                for (auto* const passenger : GetPassengers()) {
                    if (passenger && passenger->IsPlayer()) {
                        passenger->AsPlayer()->ResetPlayerBreath();
                    }
                }
            }
        }

        // 0x6B9E55 - Rescale the wheel ratios from the whole line to the spring part of it
        for (auto i = 0; i < NUM_SUSP_LINES; i++) {
            const float wheelRadius = 1.0f - m_fSuspensionLength[i] / m_fLineLength[i];
            m_aWheelRatios[i] = (m_aWheelRatios[i] - wheelRadius) / (1.0f - wheelRadius);
        }

        // Speed of a point of the bike relative to the ground physical (if any) under suspension line `line`
        const auto GetContactSpeed = [this](const CVector& point, int32 line) {
            CVector speed = GetSpeed(point);
            if (m_aGroundPhysicalPtrs[line]) {
                speed -= m_aGroundPhysicalPtrs[line]->GetSpeed(m_aGroundOffsets[line]);
            }
            return speed;
        };

        std::array<CVector, NUM_SUSP_LINES> contactPoints{};
        std::array<CVector, NUM_SUSP_LINES> springDirs{};
        std::array<CVector, NUM_SUSP_LINES> contactSpeeds{};
        std::array<float, NUM_SUSP_LINES>   springForceDampingLimits{};

        // 0x6B9E86
        for (auto i = 0; i < NUM_SUSP_LINES; i++) {
            if (m_aWheelRatios[i] < 1.0f) {
                const auto& line = colData->m_pLines[i];
                contactPoints[i] = m_aWheelColPoints[i].m_vecPoint - GetPosition();
                springDirs[i]    = GetMatrix().TransformVector(line.m_vecEnd - line.m_vecStart);
                springDirs[i].Normalise();
            }
        }

        // 0x6B9F78 - Suspension springs
        rng::fill(m_aWheelSkidmarkType, eSkidmarkType::DEFAULT);
        rng::fill(m_bMoreSkidMarks, false);
        for (auto i = 0; i < NUM_SUSP_LINES; i++) {
            if (m_aWheelRatios[i] < 1.0f) {
                const float bias = i == 2 || i == 3
                    ? 1.0f - m_pHandlingData->m_fSuspensionBiasBetweenFrontAndRear
                    : m_pHandlingData->m_fSuspensionBiasBetweenFrontAndRear;

                auto& colPoint = m_aWheelColPoints[i];
                if (colPoint.m_vecNormal.z > 0.35f) {
                    ApplySpringCollisionAlt(m_pHandlingData->m_fSuspensionForceLevel, springDirs[i], contactPoints[i], m_aWheelRatios[i], bias, colPoint.m_vecNormal, springForceDampingLimits[i]);
                } else {
                    ApplySpringCollision(m_pHandlingData->m_fSuspensionForceLevel, springDirs[i], contactPoints[i], m_aWheelRatios[i], bias, springForceDampingLimits[i]);
                }

                const auto wheel = i >= 2 ? 1 : 0;
                m_aWheelSkidmarkType[wheel] = static_cast<eSkidmarkType>(g_surfaceInfos.GetSkidmarkType(colPoint.m_nSurfaceTypeB));
                if (m_aWheelSkidmarkType[wheel] == eSkidmarkType::MUDDY) {
                    m_bMoreSkidMarks[wheel] = true; // Really "wheel is muddy"
                }
            } else {
                contactPoints[i] = GetMatrix().TransformVector(colData->m_pLines[i].m_vecEnd);
            }
        }

        for (auto i = 0; i < NUM_SUSP_LINES; i++) {
            contactSpeeds[i] = GetContactSpeed(contactPoints[i], i);
        }

        // 0x6BA1E0 - Dampen along the ground normal of a wheel's lines, unless it is steep
        const auto AlignSpringDirsToGround = [&](int32 lineA, int32 lineB) {
            if (m_aWheelRatios[lineA] >= 1.0f && m_aWheelRatios[lineB] >= 1.0f) {
                return;
            }
            const CVector normalA = m_aWheelRatios[lineA] < 1.0f ? m_aWheelColPoints[lineA].m_vecNormal : m_aWheelColPoints[lineB].m_vecNormal;
            if (normalA.z > 0.35f) {
                springDirs[lineA] = -normalA;
            }
            const CVector normalB = m_aWheelRatios[lineB] < 1.0f ? m_aWheelColPoints[lineB].m_vecNormal : m_aWheelColPoints[lineA].m_vecNormal;
            if (normalB.z > 0.35f) {
                springDirs[lineB] = -normalB;
            }
        };
        AlignSpringDirsToGround(0, 1);
        AlignSpringDirsToGround(2, 3);

        // 0x6BA49A
        for (auto i = 0; i < NUM_SUSP_LINES; i++) {
            if (m_aWheelRatios[i] < 1.0f) {
                ApplySpringDampening(m_pHandlingData->m_fSuspensionDampingLevel, springForceDampingLimits[i], springDirs[i], contactPoints[i], contactSpeeds[i]);
            }
        }

        for (auto i = 0; i < NUM_SUSP_LINES; i++) {
            contactSpeeds[i] = GetContactSpeed(contactPoints[i], i);
        }

        // 0x6BA5E1 - Engine and brakes
        float speedFwd = m_vecMoveSpeed.Dot(GetForward());
        const float acceleration = m_pHandlingData->GetTransmission().CalculateDriveAcceleration(
            m_GasPedal,
            m_nCurrentGear,
            m_fGearChangeCount,
            speedFwd,
            nullptr,
            nullptr,
            m_NumDriveWheelsOnGround,
            CCheat::IsActive(CHEAT_PERFECT_HANDLING) ? CHEAT_HANDLING_PERFECT : CHEAT_HANDLING_NONE
        ) / m_fVelocityFrequency;
        float brake = m_pHandlingData->m_fBrakeDeceleration * m_BrakePedal * CTimer::GetTimeStep();

        const bool useHandlingBias = GetStatus() == STATUS_PLAYER || GetStatus() == STATUS_REMOTE_CONTROLLED || !m_pHandlingData->m_bNpcNeutralHandl;
        const float frontBrakeBias    = useHandlingBias ? 2.0f * m_pHandlingData->m_fBrakeBias : 1.0f;
        const float rearBrakeBias     = useHandlingBias ? 2.0f * (1.0f - m_pHandlingData->m_fBrakeBias) : 1.0f;
        const float frontTractionBias = useHandlingBias ? 2.0f * m_pHandlingData->m_fTractionBias : 1.0f;
        const float rearTractionBias  = useHandlingBias ? 2.0f - frontTractionBias : 1.0f;

        // 0x6BA6E2 - Wheel contact timers and the average ground normal
        m_NumDriveWheelsOnGroundLastFrame = m_NumDriveWheelsOnGround;
        m_nNoOfContactWheels              = 0;
        m_NumDriveWheelsOnGround          = 0;
        for (auto i = 0; i < NUM_SUSP_LINES; i++) {
            if (m_aWheelRatios[i] < 1.0f) {
                m_WheelCounts[i] = 4.0f;
            } else {
                m_WheelCounts[i] = std::max(m_WheelCounts[i] - CTimer::GetTimeStep(), 0.0f);
                if (m_WheelCounts[i] <= 0.0f) {
                    continue;
                }
            }

            m_nNoOfContactWheels++;
            if (i == 2 || i == 3) {
                m_NumDriveWheelsOnGround = 1; // Rear wheel drive
            }
            if (m_nNoOfContactWheels == 1) {
                m_vecAveGroundNormal = m_aWheelColPoints[i].m_vecNormal;
            } else {
                m_vecAveGroundNormal += m_aWheelColPoints[i].m_vecNormal;
            }
        }

        if (m_nNoOfContactWheels > 0) {
            m_vecAveGroundNormal *= 1.0f / (float)m_nNoOfContactWheels;
            if (m_vecAveGroundNormal.Dot(GetUp()) < -0.5f) {
                m_vecAveGroundNormal *= -1.0f;
            }
        } else {
            m_vecAveGroundNormal = CVector{ 0.0f, 0.0f, 1.0f };
        }

        // 0x6BA8AE - Wheel contact points (relative to the bike), using the more compressed line of each wheel
        const auto frontLine = m_aWheelRatios[0] < m_aWheelRatios[1] ? 0 : 1;
        CVector frontWheelPos = GetMatrix().TransformVector({
            0.0f,
            colData->m_pLines[0].m_vecStart.y,
            colData->m_pLines[0].m_vecStart.z - m_aWheelRatios[frontLine] * m_fSuspensionLength[0] - mi->m_fWheelSizeFront * 0.5f
        });
        const auto rearLine = m_aWheelRatios[2] < m_aWheelRatios[3] ? 2 : 3;
        CVector rearWheelPos = GetMatrix().TransformVector({
            0.0f,
            colData->m_pLines[3].m_vecStart.y,
            colData->m_pLines[2].m_vecStart.z - m_aWheelRatios[rearLine] * m_fSuspensionLength[2] - mi->m_fWheelSizeRear * 0.5f
        });

        const float traction = m_pHandlingData->m_fTractionMultiplier * m_fExtraTractionMult * s_TractionScale * 0.25f;

        // 0x6BA9F7 - Handlebar steering angle
        if (GetStatus() != STATUS_PLAYER && bikeFlags.bOnSideStand && !bikeFlags.bGettingPickedUp) {
            if (m_RideAnimData.BarSteerAngle < 0.34906587f) { // ~20 deg
                m_RideAnimData.BarSteerAngle += CTimer::GetTimeStep() * DegreesToRadians(1.5f);
            }
        } else if (std::fabs(m_vecMoveSpeed.x) < 0.01f && std::fabs(m_vecMoveSpeed.y) < 0.01f && m_fSteerAngle == 0.0f) {
            m_RideAnimData.BarSteerAngle *= std::pow(0.96f, CTimer::GetTimeStep());
        } else {
            float steerMult = 1.0f;
            if (speedFwd > 0.01f && (m_WheelCounts[0] > 0.0f || m_WheelCounts[1] > 0.0f) && GetStatus() == STATUS_PLAYER) {
                // Limit the steering to what the tyre grip allows at this speed
                CColPoint colPoint{};
                colPoint.m_nSurfaceTypeA = SURFACE_WHEELBASE;
                colPoint.m_nSurfaceTypeB = SURFACE_TARMAC;
                float steer = g_surfaceInfos.GetAdhesiveLimit(&colPoint) * m_BikeHandling->m_fSpeedSteer * traction * 4.0f;
                const auto rearAdhesionGroup = g_surfaceInfos.GetAdhesionGroup(m_aWheelColPoints[rearLine].m_nSurfaceTypeB);
                if (rearAdhesionGroup == ADHESION_GROUP_LOOSE || rearAdhesionGroup == ADHESION_GROUP_SAND) {
                    steer *= m_BikeHandling->m_fSlipSteer;
                }
                steer = std::min(steer / sq(speedFwd), 1.0f);
                steer = std::asin(steer) / DegreesToRadians(m_pHandlingData->m_fSteeringLock);
                if (m_fSteerAngle < 0.0f && m_RideAnimData.LeanAngle < 0.0f || m_fSteerAngle > 0.0f && m_RideAnimData.LeanAngle > 0.0f) {
                    steer *= 2.0f; // Steering into the lean
                }
                steerMult = std::min(steer, 1.0f);
            }
            // NOTE: The original also resets `steerMult` to 1 here when `GetStatus() != STATUS_PLAYER` (redundant)
            m_RideAnimData.BarSteerAngle = steerMult * m_fSteerAngle;
        }

        // 0x6BAC22 - Wheel forces
        const CVector moveSpeedBeforeWheels = m_vecMoveSpeed;

        // Stable wheel axes on the ground plane
        const auto CalcWheelAxes = [](CVector& wheelFwd, CVector& wheelRight, const CVector& groundNormal) {
            wheelFwd -= groundNormal * wheelFwd.Dot(groundNormal);
            wheelFwd.Normalise();
            wheelRight = wheelFwd.Cross(groundNormal);
            wheelRight.Normalise();
        };

        // Tyre grip on the contact surface + `ProcessBikeWheel` (`wheel`: 0 = front, 1 = rear)
        const auto ProcessWheelForces = [&](int32 wheel, int32 line, CVector& wheelFwd, CVector& wheelRight, CVector& wheelPos, float wheelTraction, float wheelBrake, float tractionBias) {
            auto& colPoint = m_aWheelColPoints[line];
            colPoint.m_nSurfaceTypeA = SURFACE_WHEELBASE;
            float adhesion = g_surfaceInfos.GetAdhesiveLimit(&colPoint) * wheelTraction;

            float destabTraction = 1.0f;
            if (m_fBrakingSlide > 0.0f) {
                switch (g_surfaceInfos.GetAdhesionGroup(colPoint.m_nSurfaceTypeB)) {
                case ADHESION_GROUP_HARD:
                case ADHESION_GROUP_LOOSE:
                    destabTraction = 0.9f;
                    break;
                case ADHESION_GROUP_ROAD:
                    destabTraction = 0.7f;
                    break;
                }
            }
            if (GetStatus() == STATUS_PLAYER) {
                adhesion *= g_surfaceInfos.GetWetMultiplier(colPoint.m_nSurfaceTypeB);
            }
            if (m_nWheelStatus[wheel] == WHEEL_STATUS_BURST) {
                adhesion *= 0.4f;
            }

            s_WheelStates[wheel] = m_WheelStates[wheel];
            CVector contactSpeed = GetContactSpeed(wheelPos, line);
            ProcessBikeWheel(
                wheelFwd,
                wheelRight,
                contactSpeed,
                wheelPos,
                2,
                s_Thrust,
                wheelBrake,
                adhesion * tractionBias,
                destabTraction,
                (int8)wheel,
                m_aWheelAngularVelocity[wheel],
                s_WheelStates[wheel],
                wheel == 0 ? BIKE_WHEEL_F_STD : BIKE_WHEEL_R_STD,
                m_nWheelStatus[wheel]
            );
            if ((extraHandlingFlags & EXTRA_HANDLING_BIKE_SOFT_GROUND) && (s_WheelStates[wheel] == WHEEL_STATE_SPINNING || s_WheelStates[wheel] == WHEEL_STATE_SKIDDING)) {
                s_WheelStates[wheel] = WHEEL_STATE_NORMAL;
            }
        };

        // 0x6BAC4B / 0x6BB566 - `isFirst`: processed before the rear wheel (the usual case)
        const auto ProcessFrontWheel = [&](bool isFirst) {
            if (m_WheelCounts[0] > 0.0f || m_WheelCounts[1] > 0.0f) {
                CVector wheelFwd = GetMatrix().TransformVector({ -std::sin(m_RideAnimData.BarSteerAngle), std::cos(m_RideAnimData.BarSteerAngle), 0.0f });
                CVector wheelRight;
                CalcWheelAxes(wheelFwd, wheelRight, m_aWheelColPoints[frontLine].m_vecNormal);
                if (isFirst && bKeepUpright) { // NOTE: Not done when the front wheel is processed after the rear wheel
                    wheelRight.z = 0.0f;
                }
                s_Thrust = 0.0f;
                ProcessWheelForces(0, frontLine, wheelFwd, wheelRight, frontWheelPos, traction, brake * frontBrakeBias, frontTractionBias);
            } else {
                // Front wheel in the air: spin down
                m_aWheelAngularVelocity[0] *= 0.95f;
                if (isFirst) {
                    m_aWheelPitchAngles[0] += m_aWheelAngularVelocity[0]; // BUG: Missing timestep (the other path below has it)
                } else {
                    m_aWheelPitchAngles[0] += m_aWheelAngularVelocity[0] * CTimer::GetTimeStep();
                }
            }
        };

        const bool procRearWheelFirst = m_pHandlingData->m_bProcRearwheelFirst;
        if (!procRearWheelFirst) {
            ProcessFrontWheel(true);
        }

        // 0x6BAF62 - Rear wheel
        if (m_WheelCounts[2] > 0.0f || m_WheelCounts[3] > 0.0f) {
            float rearBrake    = brake;
            float rearTraction = traction;

            CVector wheelFwd = GetForward();
            CVector wheelRight;
            CalcWheelAxes(wheelFwd, wheelRight, m_aWheelColPoints[rearLine].m_vecNormal);
            if (bKeepUpright) {
                wheelRight.z = 0.0f;
            }

            if (vehicleFlags.bIsHandbrakeOn) {
                rearBrake   = 20000.0f;
                m_fTyreTemp = 1.0f;
            } else if (m_nBrakesOn) { // Burnout
                rearBrake    = 0.0f;
                rearTraction = 0.0f;
                // BUG: The force and point arguments are swapped
                ApplyTurnForce(contactPoints[2], GetRight() * (m_fSteerAngle * m_fTurnMass * -0.0007f) * CTimer::GetTimeStep());
            } else if (m_fTyreTemp < 1.0f && m_GasPedal > 0.75f) {
                rearTraction = traction * m_fTyreTemp;
                // BUG: The force and point arguments are swapped
                ApplyTurnForce(contactPoints[2], GetRight() * ((1.0f - m_fTyreTemp) * m_fSteerAngle * m_fTurnMass * -0.0007f) * CTimer::GetTimeStep());
            }

            // NOTE: `s_Thrust` is still the front wheel's 0, or - if the front wheel wasn't processed this frame - the last value set by any bike
            if (s_Thrust > 0.0f && brake > 0.0f) {
                brake = 0.0f; // Only affects the front wheel if it's processed after the rear one
            }
            s_Thrust = acceleration;
            ProcessWheelForces(1, rearLine, wheelFwd, wheelRight, rearWheelPos, rearTraction, rearBrake * rearBrakeBias, rearTractionBias);
        } else {
            // Rear wheel in the air: throttle spins it, handbrake locks it
            if (vehicleFlags.bIsHandbrakeOn) {
                m_aWheelAngularVelocity[1] = 0.0f;
            } else if (acceleration != 0.0f) {
                if (acceleration > 0.0f) {
                    if (m_aWheelAngularVelocity[1] < 1.0f) { // BUG: Wrong side test (velocity decreases), so the spin-up has no limit
                        m_aWheelAngularVelocity[1] -= 0.1f;
                    }
                } else if (m_aWheelAngularVelocity[1] > -1.0f) {
                    m_aWheelAngularVelocity[1] += 0.05f;
                }
            }
            m_aWheelPitchAngles[1] += CTimer::GetTimeStep() * m_aWheelAngularVelocity[1];
        }

        // 0x6BB4E6 - Burnouts heat the tyre (which then has less grip), it cools down otherwise
        if (m_nBrakesOn && m_WheelStates[1] == WHEEL_STATE_SPINNING) {
            m_fTyreTemp -= CTimer::GetTimeStep() * 0.002f;
            if (m_fTyreTemp < 0.0f) {
                m_fTyreTemp = 0.0f;
            }
        } else if (m_fTyreTemp < 1.0f) {
            m_fTyreTemp += CTimer::GetTimeStep() * 0.005f;
        }

        if (procRearWheelFirst) {
            ProcessFrontWheel(false);
        }

        // 0x6BB87A
        // NOTE: Plain clear (like CAutomobile), the references registered by `ProcessEntityCollision` stay in the ground entities' lists
        rng::fill(m_aGroundPhysicalPtrs, nullptr);

        // 0x6BB894 - Standing still (`bike_still` anim) the rider puts a foot down and the bike leans up to 10 deg
        float stillLean = 0.0f;
        if (m_pDriver) {
            if (const auto* const stillAnim = RpAnimBlendClumpGetAssociation(m_pDriver->GetRpClump(), ANIM_ID_BIKE_STILL)) {
                stillLean = stillAnim->GetBlendAmount() * 0.17453294f; // ~10 deg
            }
        }

        // 0x6BB8C4
        if (bKeepUpright) {
            m_vecAveGroundNormal = CVector{ 0.0f, 0.0f, 1.0f };
            CVector right = GetForward().Cross(m_vecAveGroundNormal);
            right.Normalise();
            m_vecAveGroundNormal = right.Cross(GetForward());
            m_vecAveGroundNormal.Normalise();
        }

        // 0x6BB955 - Lean angle
        if (!(extraHandlingFlags & EXTRA_HANDLING_BIKE_BALANCED) && !bikeFlags.bGettingPickedUp) {
            if (bikeFlags.bOnSideStand) {
                const float leanMult = std::pow(0.97f, CTimer::GetTimeStep());
                m_RideAnimData.DesiredLeanAngle = leanMult * m_RideAnimData.DesiredLeanAngle - (1.0f - leanMult) * (std::asin(GetRight().z) + stillLean + DegreesToRadians(15.0f));
            } else {
                m_RideAnimData.DesiredLeanAngle *= std::pow(0.95f, CTimer::GetTimeStep());
            }
        } else {
            // 0x6BB9E9 - Lean into the sideways acceleration the wheels gave the bike this frame
            m_vecGroundRight = GetForward().Cross(m_vecAveGroundNormal);
            m_vecGroundRight.Normalise();

            const float sideImpulse = [&] {
                if (m_pAttachedTo) {
                    return 0.0f;
                }
                if (!m_nNoOfContactWheels) {
                    return m_fSteerAngle / DegreesToRadians(m_pHandlingData->m_fSteeringLock) * CTimer::GetTimeStep() * -0.004f;
                }
                if (physicalFlags.bDisableCollisionForce) {
                    const CVector speedChange = moveSpeedBeforeWheels - m_vecOldSpeedForPlayback;
                    m_vecOldSpeedForPlayback  = moveSpeedBeforeWheels;
                    return speedChange.Dot(m_vecGroundRight);
                }
                return (m_vecMoveSpeed - moveSpeedBeforeWheels).Dot(m_vecGroundRight);
            }();

            const float maxLean = m_nWheelStatus[0] == WHEEL_STATUS_BURST
                ? 0.4f * m_BikeHandling->m_fMaxLean
                : m_BikeHandling->m_fMaxLean;
            const float lean     = std::clamp(sideImpulse / (std::max(0.01f, CTimer::GetTimeStep()) * 0.008f), -maxLean, maxLean);
            const float leanMult = std::pow(m_BikeHandling->m_fDesLean, CTimer::GetTimeStep());
            m_RideAnimData.DesiredLeanAngle = leanMult * m_RideAnimData.DesiredLeanAngle + (1.0f - leanMult) * (std::asin(lean) - stillLean);
        }
        m_RideAnimData.LeanAngle = m_RideAnimData.DesiredLeanAngle;

        // 0x6BBBB3
        m_WheelStates = s_WheelStates;
        if (m_GasPedal < 0.0f && m_WheelStates[1] == WHEEL_STATE_SPINNING) {
            m_WheelStates[1] = WHEEL_STATE_NORMAL;
        }

        if (GetStatus() == STATUS_PLAYER) {
            ProcessSirenAndHorn(true);
        } else {
            ReduceHornCounter();
        }
    }

    // 0x6BBC18 - Engine fire
    if (m_fHealth < 250.0f && GetStatus() != STATUS_WRECKED) {
        if (!IsSubBMX()) {
            auto* const modellingMatrix = GetModellingMatrix();
            if (!m_pFireParticle && modellingMatrix) {
                m_pFireParticle = g_fxMan.CreateFxSystem("fire_bike", GetDummyPositionObjSpace(DUMMY_ENGINE), modellingMatrix, false);
                if (m_pFireParticle) {
                    m_pFireParticle->Play();
                    CEventVehicleOnFire vehOnFireEvent(this);
                    GetEventGlobalGroup()->Add(&vehOnFireEvent, false);
                }
            }
        }

        m_BlowUpTimer += (float)(uint32)CTimer::GetTimeStepInMS();
        if (m_BlowUpTimer > 5000.0f) {
            BlowUpCar(m_Damager, false);
        }
    } else {
        m_BlowUpTimer = 0.0f;
        FxSystem_c::SafeKillAndClear(m_pFireParticle);
    }

    ProcessDelayedExplosion();

    // 0x6BBD7F - Pad shake from suspension hits and rough surfaces
    const float speedSq   = m_vecMoveSpeed.SquaredMagnitude();
    float suspensionShake = 0.0f;
    float roughnessShake  = 0.0f;
    for (auto i = 0; i < NUM_SUSP_LINES; i++) {
        const float suspensionChange = m_aRatioHistory[i] - m_aWheelRatios[i];
        if (suspensionChange > 0.3f
            && (i == 0 || i == 2)
            && speedSq > 0.040000003f // sq(0.2f)
            && (GetStatus() == STATUS_PLAYER || GetStatus() == STATUS_PHYSICS)
            && suspensionChange > suspensionShake
        ) {
            suspensionShake = suspensionChange;
        }

        if (m_aWheelRatios[i] < 1.0f && GetStatus() == STATUS_PLAYER) {
            roughnessShake = std::max(roughnessShake, (float)g_surfaceInfos.GetRoughness(m_aWheelColPoints[i].m_nSurfaceTypeB) * 0.1f);
        }

        m_aRatioHistory[i] = m_aWheelRatios[i];
        m_aWheelRatios[i]  = 1.0f;
    }

    if ((CTimer::GetTimeInMS() & 2047) > 800) {
        if (roughnessShake >= 0.29f) {
            suspensionShake = 0.0f;
        }
        roughnessShake = 0.0f;
    }

    if ((suspensionShake > 0.0f || roughnessShake > 0.0f) && GetStatus() == STATUS_PLAYER) {
        const float speedSqNow = m_vecMoveSpeed.SquaredMagnitude();
        if (speedSqNow > 0.010000001f) {
            const float speed = std::sqrt(speedSqNow);
            if (suspensionShake > 0.0f) {
                const auto frequency = (uint8)std::min(speed / m_fMass * suspensionShake * 400000.0f + 100.0f, 250.0f);
                CPad::GetPad(0)->StartShake((int16)(CTimer::GetTimeStep() * 20000.0f / (float)frequency), frequency, 0);
            } else {
                const auto frequency = (uint8)std::min(speed / m_fMass * roughnessShake * 400000.0f + 40.0f, 150.0f);
                CPad::GetPad(0)->StartShake((int16)(CTimer::GetTimeStep() * 5000.0f / (float)frequency), frequency, 0);
            }
        }
    }

    // 0x6BC006
    vehicleFlags.bVehicleColProcessed = false;
    vehicleFlags.bAudioChangingGear   = false;
    if (!vehicleFlags.bWarnedPeds) {
        CCarCtrl::ScanForPedDanger(this);
    }

    if (physicalFlags.bDisableCollisionForce && physicalFlags.bCollidable) {
        ResetMoveSpeed();
        ResetTurnSpeed();
        ResetFrictionMoveSpeed();
        ResetFrictionTurnSpeed();
    } else if (!skipPhysics
        && (m_GasPedal == 0.0f || GetStatus() == STATUS_WRECKED)
        && std::fabs(m_vecMoveSpeed.x) < 0.005f
        && std::fabs(m_vecMoveSpeed.y) < 0.005f
        && std::fabs(m_vecMoveSpeed.z) < 0.005f
        && (m_fDamageIntensity <= 0.0f || m_pDamageEntity != FindPlayerPed())
    ) {
        m_vecTurnSpeed.z = 0.0f;
        ResetMoveSpeed();
    }

    // 0x6BC192 - Balancing: keep the bike upright relative to the ground
    if (!(extraHandlingFlags & EXTRA_HANDLING_BIKE_BALANCED) && !bikeFlags.bGettingPickedUp && !bikeFlags.bOnSideStand) {
        return;
    }

    const float groundRightDot = std::clamp(m_vecAveGroundNormal.Dot(GetRight()), -1.0f, 1.0f);
    const CVector comOffset = GetMatrix().TransformVector(m_vecCentreOfMass);
    if (extraHandlingFlags & EXTRA_HANDLING_BIKE_BALANCED) {
        ApplyTurnForce(GetUp() * (groundRightDot * m_fTurnMass * -0.07f) * CTimer::GetTimeStep(), comOffset + GetRight());
        bikeFlags.bOnSideStand = false;
    } else {
        ApplyTurnForce(GetUp() * (groundRightDot * m_fTurnMass * -0.1f) * CTimer::GetTimeStep(), comOffset + GetRight());
    }

    if (GetStatus() != STATUS_PLAYER) {
        return;
    }

    if (m_WheelCounts[0] <= 0.0f && m_WheelCounts[1] <= 0.0f && GetForward().z > 0.0f && (m_WheelCounts[2] > 0.0f || m_WheelCounts[3] > 0.0f)) {
        // 0x6BC3C6 - Wheelie: pull towards the wheelie angle, steer with the handlebars
        float wheelieAngDiff = m_BikeHandling->m_fWheelieAng - GetForward().z;
        if (wheelieAngDiff > 0.15f) {
            wheelieAngDiff = std::max(0.3f - wheelieAngDiff, 0.0f);
        } else if (wheelieAngDiff < -0.08f) {
            wheelieAngDiff = std::min(-0.14f - wheelieAngDiff, 0.0f);
        }
        const float speed = std::min(m_vecMoveSpeed.Magnitude(), 0.1f);
        ApplyTurnForce(
            GetUp() * (CStats::GetFatAndMuscleModifier(STAT_MOD_12) * (wheelieAngDiff * m_BikeHandling->m_fWheelieStabMult * speed) * m_fTurnMass * CTimer::GetTimeStep() * 0.5f),
            comOffset + GetForward()
        );
        ApplyTurnForce(
            GetRight() * (m_BikeHandling->m_fWheelieSteer * m_RideAnimData.BarSteerAngle * m_fTurnMass * CTimer::GetTimeStep() * 0.5f),
            comOffset + GetForward()
        );
        ApplyMoveForce(GetRight() * (m_vecMoveSpeed.Magnitude() * m_fMass * m_BikeHandling->m_fWheelieSteer * m_RideAnimData.BarSteerAngle * CTimer::GetTimeStep() * s_WheelieSteerForceMult) * m_vecMoveSpeed.Magnitude());
        m_RideAnimData.LeanAngle += CTimer::GetTimeStep() * m_RideAnimData.BarSteerAngle * s_WheelieLeanMult;
    } else if (m_WheelCounts[2] <= 0.0f && m_WheelCounts[3] <= 0.0f && GetForward().z < 0.0f && (m_WheelCounts[0] > 0.0f || m_WheelCounts[1] > 0.0f)) {
        // 0x6BC6FB - Stoppie: pull towards the stoppie angle, damp the sideways speed
        float stoppieAngDiff = m_BikeHandling->m_fStoppieAng - GetForward().z;
        if (stoppieAngDiff > 0.15f) {
            stoppieAngDiff = std::max(0.3f - stoppieAngDiff, 0.0f);
        } else if (stoppieAngDiff < -0.15f) {
            stoppieAngDiff = std::min(-0.3f - stoppieAngDiff, 0.0f);
        }
        const float speed = std::min(m_vecMoveSpeed.Magnitude(), 0.1f);
        ApplyTurnForce(
            GetUp() * (CStats::GetFatAndMuscleModifier(STAT_MOD_12) * (stoppieAngDiff * m_BikeHandling->m_fStoppieStabMult * speed) * m_fTurnMass * CTimer::GetTimeStep() * 0.5f),
            comOffset + GetForward()
        );

        const float sideForce = m_vecMoveSpeed.Dot(GetRight()) * m_fTurnMass * CTimer::GetTimeStep() * s_StoppieSideForceMult;
        CVector sideForcePoint = CVector{ 0.0f, 0.0f, 1.0f }.Cross(GetRight());
        sideForcePoint.Normalise();
        ApplyTurnForce(GetRight() * -sideForce, -sideForcePoint);
    }
}

// 0x6B6740
void CBike::ResetSuspension() {
    ((void(__thiscall*)(CBike*))0x6B6740)(this);
}

// 0x6B6790
bool CBike::GetAllWheelsOffGround() const {
    return m_nNoOfContactWheels == 0;
}

// 0x6B67A0
void CBike::DebugCode() {
    // NOP
}

// 0x6B6D40
void CBike::DoSoftGroundResistance(uint32& arg0) {
    ((void(__thiscall*)(CBike*, uint32&))0x6B6D40)(this, arg0);
}

// 0x6B7130
void CBike::PlayHornIfNecessary() {
    if (m_autoPilot.carCtrlFlags.bHonkAtCar || m_autoPilot.carCtrlFlags.bHonkAtPed)
        PlayCarHorn();
}

// 0x6B7150
void CBike::CalculateLeanMatrix() {
    if (m_bLeanMatrixCalculated)
        return;

    CMatrix mat;
    mat.SetRotateX(fabs(m_RideAnimData.LeanAngle) * -0.05f);
    mat.RotateY(m_RideAnimData.LeanAngle);
    m_mLeanMatrix = GetMatrix();
    m_mLeanMatrix = m_mLeanMatrix * mat;
    // place wheel back on ground
    m_mLeanMatrix.GetPosition() += GetUp() * (1.0f - cos(m_RideAnimData.LeanAngle)) * GetColModel()->GetBoundingBox().m_vecMin.z;
    m_bLeanMatrixCalculated = true;
}

// 0x6B7F90
void CBike::FixHandsToBars(CPed* rider) {
    ((void(__thiscall*)(CBike*, CPed*))0x6B7F90)(this, rider);
}

// 0x6BEEB0
void CBike::PlaceOnRoadProperly() {
    ((void(__thiscall*)(CBike*))0x6BEEB0)(this);
}

// 0x6BF230
void CBike::GetCorrectedWorldDoorPosition(CVector& out, CVector arg1, CVector arg2) {
    ((void(__thiscall*)(CBike*, CVector&, CVector, CVector))0x6BF230)(this, out, arg1, arg2);
}

// 0x6BEA10
void CBike::BlowUpCar(CEntity* damager, bool bHideExplosion) {
    plugin::CallMethod<0x6BEA10, CBike*, CEntity*, uint8>(this, damager, bHideExplosion);
}

// 0x6B7050
void CBike::Fix() {
    vehicleFlags.bIsDamaged = false;
    bikeFlags.bEngineOnFire = false;
    m_nWheelStatus[0] = 0;
    m_nWheelStatus[1] = 0;
}

// 0x6BD090
void CBike::PreRender() {
    plugin::CallMethod<0x6BD090, CBike*>(this);
}

// 0x6BDE20
void CBike::Render() {
    auto savedRef = 0;
    RwRenderStateGet(rwRENDERSTATEALPHATESTFUNCTIONREF, &savedRef);
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(1));

    m_nTimeTillWeNeedThisCar = CTimer::GetTimeInMS() + 3000;
    CVehicle::Render();

    if (m_renderLights.m_bRightFront) {
        CalculateLeanMatrix();
        CVehicle::DoHeadLightBeam(eVehicleLightId::MAIN, m_mLeanMatrix, true);
    }

    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(savedRef));
}

// 0x6BCFC0
void CBike::Teleport(CVector destination, bool resetRotation) {
    CWorld::Remove(this);

    GetPosition() = destination;
    if (resetRotation)
        SetOrientation(0.0f, 0.0f, 0.0f);

    ResetMoveSpeed();
    ResetTurnSpeed();
    ResetSuspension();

    CWorld::Add(this);
}

// 0x6B8EC0
void CBike::VehicleDamage(float damageIntensity, eVehicleCollisionComponent component, CEntity* damager, CVector* vecCollisionCoors, CVector* vecCollisionDirection, eWeaponType weapon) {
    plugin::CallMethod<0x6B8EC0, CBike*, float, eVehicleCollisionComponent, CEntity*, CVector*, CVector*, eWeaponType>(this, damageIntensity, component, damager, vecCollisionCoors, vecCollisionDirection, weapon);
}

// 0x6B89B0
void CBike::SetupSuspensionLines() {
    plugin::CallMethod<0x6B89B0, CBike*>(this);
}

// 0x6B8970
void CBike::SetModelIndex(uint32 index) {
    CVehicle::SetModelIndex(index);
    SetupModelNodes();
}

// 0x6B5960
void CBike::SetupModelNodes() {
    std::ranges::fill(m_aBikeNodes, nullptr);
    CClumpModelInfo::FillFrameArray(GetRpClump(), m_aBikeNodes.data());
}

// 0x6B7080
void CBike::PlayCarHorn() {
    plugin::CallMethod<0x6B7080, CBike*>(this);
}

// 0x6B7070
void CBike::SetupDamageAfterLoad() {
    // NOP
}

// 0x6B6950
void CBike::DoBurstAndSoftGroundRatios() {
    plugin::CallMethod<0x6B6950, CBike*>(this);
}

// 0x6B67E0
bool CBike::SetUpWheelColModel(CColModel* wheelCol) {
    return plugin::CallMethodAndReturn<bool, 0x6B67E0, CBike*, CColModel*>(this, wheelCol);
}

// 0x6B67B0
void CBike::RemoveRefsToVehicle(CEntity* entityToRemove) {
    for (auto& entity: m_aGroundPhysicalPtrs) {
        if (entity == entityToRemove)
            entity = nullptr;
    }
}

// 0x6B6620
void CBike::ProcessControlCollisionCheck(bool applySpeed) {
    const CMatrix oldMat = GetMatrix();
    SetIsStuck(false);
    SkipPhysics();
    physicalFlags.bSkipLineCol     = false;
    physicalFlags.bProcessingShift = false;
    m_fMovingSpeed                 = 0.0f;
    rng::fill(m_aWheelRatios, 1.0f);

    if (applySpeed) {
        ApplyMoveSpeed();
        ApplyTurnSpeed();

        for (auto i = 0; CheckCollision() && i < 5; i++) {
            GetMatrix() = oldMat;
            ApplyMoveSpeed();
            ApplyTurnSpeed();
        }
    } else {
        const auto usesCollision = GetUsesCollision();
        SetUsesCollision(false);
        CheckCollision();
        SetUsesCollision(usesCollision);
    }

    SetIsStuck(false);
    SetIsInSafePosition(true);
}

// 0x6B5990
void CBike::GetComponentWorldPosition(int32 componentId, CVector& outPos) {
    if (IsComponentPresent(componentId))
        outPos = RwFrameGetLTM(m_aBikeNodes[componentId])->pos;
    else
        NOTSA_LOG_DEBUG("BikeNode missing: model={}, nodeIdx={}", m_nModelIndex, componentId);
}

// 0x6B58D0
void CBike::ProcessOpenDoor(CPed* ped, uint32 doorComponentId, uint32 animGroup, uint32 animId, float fTime) {
    // NOP
}

