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
    RH_ScopedInstall(ResetSuspension, 0x6B6740);
    RH_ScopedInstall(GetAllWheelsOffGround, 0x6B6790);
    RH_ScopedInstall(DebugCode, 0x6B67A0);
    RH_ScopedInstall(DoSoftGroundResistance, 0x6B6D40);
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
    RH_ScopedVMTInstall(ProcessControl, 0x6B9250, { .Reversed = false });
    RH_ScopedVMTInstall(VehicleDamage, 0x6B8EC0, { .Reversed = false });
    RH_ScopedVMTInstall(SetupSuspensionLines, 0x6B89B0);
    RH_ScopedVMTInstall(SetModelIndex, 0x6B8970);
    RH_ScopedVMTInstall(PlayCarHorn, 0x6B7080, { .Reversed = false });
    RH_ScopedVMTInstall(SetupDamageAfterLoad, 0x6B7070);
    RH_ScopedVMTInstall(DoBurstAndSoftGroundRatios, 0x6B6950);
    RH_ScopedVMTInstall(SetUpWheelColModel, 0x6B67E0);
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
        mi->m_pColModel->m_pColData->m_pLines[1].m_vecStart.z = 99'999.99f; // First bike of this model: SetupSuspensionLines also fixes the col model's ground clearance
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
    plugin::CallMethod<0x6B9250, CBike*>(this);
}

// 0x6B6740
void CBike::ResetSuspension() {
    rng::fill(m_aWheelPitchAngles, 0.0f);
    rng::fill(m_WheelStates, WHEEL_STATE_NORMAL);
    rng::fill(m_aWheelRatios, 1.0f);
    rng::fill(m_WheelCounts, 0.0f);
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
void CBike::DoSoftGroundResistance(uint32& extraHandlingFlags) {
    const auto IsAnyWheelTouching = [this](auto&& Pred) {
        for (auto&& [ratio, colPoint] : rngv::zip(m_aWheelRatios, m_aWheelColPoints)) {
            if (ratio < 1.0f && Pred(colPoint)) {
                return true;
            }
        }
        return false;
    };
    const auto GetMoveSpeedAlongGround = [this] {
        return m_vecMoveSpeed - GetUp() * DotProduct(m_vecMoveSpeed, GetUp());
    };

    if (IsAnyWheelTouching([](const CColPoint& cp) { return g_surfaceInfos.GetAdhesionGroup(cp.m_nSurfaceTypeB) == ADHESION_GROUP_SAND; })) {
        auto speed = GetMoveSpeedAlongGround();
        if (m_GasPedal > 0.3f) {
            if (speed.SquaredMagnitude() < sq(0.3f)) {
                extraHandlingFlags += 4; // NOTE: Original code adds instead of OR-ing. TODO: Name this flag (bike bogged down in sand?)
            }
            speed -= GetForward() * DotProduct(speed, GetForward()); // Only drag sideways while accelerating
        }
        ApplyMoveForce(speed * -(CTimer::GetTimeStep() * m_fMass * 0.02f));
    } else if (IsAnyWheelTouching([](const CColPoint& cp) { return cp.m_nSurfaceTypeB == SURFACE_RAILTRACK; })) {
        ApplyMoveForce(GetMoveSpeedAlongGround() * -(CTimer::GetTimeStep() * m_fMass * CVehicle::ms_fRailTrackResistance));
    }
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

// Inlined in the original (also in SetUpWheelColModel): `frame`'s matrix concatenated with its parents' until `root` (exclusive) or the top of the hierarchy.
static RwMatrix GetFrameMatrixRelativeTo(RwFrame* frame, const RwFrame* root) {
    RwMatrix mat = *RwFrameGetMatrix(frame);
    if (auto* parent = RwFrameGetParent(frame)) {
        do {
            RwMatrixTransform(&mat, RwFrameGetMatrix(parent), rwCOMBINEPOSTCONCAT);
            parent = RwFrameGetParent(parent);
        } while (parent != root && parent);
    }
    return mat;
}

// 0x6B89B0
void CBike::SetupSuspensionLines() {
    // The constructor (0x6BF742) writes this to `m_pLines[0].m_vecStart.z` every time (= "lines must be recalculated from the frames"),
    // and to `m_pLines[1].m_vecStart.z` only when it had to allocate the lines (= first bike of this model).
    constexpr auto LINE_NOT_SET_UP = 99'999.99f; // 0x47C34FFF

    const auto& mi       = *GetVehicleModelInfo();
    auto&       cm       = *mi.GetColModel();
    auto&       cd       = *cm.m_pColData;
    const auto& handling = *m_pHandlingData;

    // 0x6B89CE
    const bool linesWereJustAllocated = cd.m_pLines[1].m_vecStart.z == LINE_NOT_SET_UP;
    const bool linesAreSetUp          = cd.m_pLines[0].m_vecStart.z != LINE_NOT_SET_UP;

    // 0x6B8A20
    for (auto i = 0; i < NUM_SUSP_LINES; i++) {
        auto& line = cd.m_pLines[i];
        const auto isFront = i < 2; // Lines 0, 1 are the front wheel's, 2, 3 the rear wheel's

        CVector wheelPos;
        if (linesAreSetUp) {
            wheelPos   = line.m_vecStart;
            wheelPos.z = m_aWheelOrigHeights[isFront ? 0 : 1];
        } else {
            // Each wheel has 2 lines: one a quarter wheel size in front of, the other behind the wheel's center
            const auto wheelSize = isFront ? mi.m_fWheelSizeFront : mi.m_fWheelSizeRear;
            const auto lineOffsetY = wheelSize * ((i % 2) == 0 ? 0.25f : -0.25f);

            wheelPos = GetFrameMatrixRelativeTo(m_aBikeNodes[isFront ? BIKE_WHEEL_FRONT : BIKE_WHEEL_REAR], RpClumpGetFrame(GetRpClump())).pos;
            if (i == 0) {
                m_aWheelOrigHeights[0] = wheelPos.z;
            } else if (i == 2) {
                m_aWheelOrigHeights[1] = wheelPos.z;

                // 0x6B8B3E
                if (auto* const forksRear = m_aBikeNodes[BIKE_FORKS_REAR]) {
                    const CVector forksRearPos = GetFrameMatrixRelativeTo(forksRear, RpClumpGetFrame(GetRpClump())).pos;
                    m_fSwingArmLength = CVector2D{ wheelPos.y - forksRearPos.y, wheelPos.z - forksRearPos.z }.Magnitude();
                } else {
                    m_fSwingArmLength = 0.0f;
                }
            }
            wheelPos.y += lineOffsetY;
        }

        // 0x6B8BC8
        wheelPos.z += handling.m_fSuspensionUpperLimit;

        line.m_vecStart = wheelPos;
        line.m_vecEnd   = CVector{
            wheelPos.x,
            wheelPos.y,
            handling.m_fSuspensionLowerLimit - handling.m_fSuspensionUpperLimit - (isFront ? mi.m_fWheelSizeFront : mi.m_fWheelSizeRear) * 0.5f + wheelPos.z
        };

        m_fSuspensionLength[i] = handling.m_fSuspensionUpperLimit - handling.m_fSuspensionLowerLimit;
        m_fLineLength[i]       = line.m_vecStart.z - line.m_vecEnd.z;
    }

    // 0x6B8C9E
    if (!linesAreSetUp) {
        // NOTE: Unlike above, this walk only stops at the top of the hierarchy (the stop frame is the fork frame itself), so the clump's root frame is included too.
        const CVector forksFrontPos = GetFrameMatrixRelativeTo(m_aBikeNodes[BIKE_FORKS_FRONT], m_aBikeNodes[BIKE_FORKS_FRONT]).pos;
        m_fForkYOffset = forksFrontPos.y;
        m_fForkZOffset = forksFrontPos.z;
    }

    // 0x6B8CF5
    m_fHeightAboveRoad = (1.0f - 1.0f / (handling.m_fSuspensionForceLevel * 4.0f)) * m_fSuspensionLength[0] + (mi.m_fWheelSizeFront * 0.5f - cd.m_pLines[0].m_vecStart.z);

    // 0x6B8D40
    for (auto i = 0u; i < m_aWheelSuspensionHeights.size(); i++) {
        m_aWheelSuspensionHeights[i] = (i == 0 ? mi.m_fWheelSizeFront : mi.m_fWheelSizeRear) * 0.5f - m_fHeightAboveRoad;
    }

    // 0x6B8D6A
    if (cd.m_pLines[0].m_vecEnd.z < cm.m_boundBox.m_vecMin.z) {
        cm.m_boundBox.m_vecMin.z = cd.m_pLines[0].m_vecEnd.z;
    }

    // 0x6B8D84
    cm.m_boundSphere.m_fRadius = std::max(cm.m_boundSphere.m_fRadius, std::max(cm.m_boundBox.m_vecMax.Magnitude(), cm.m_boundBox.m_vecMin.Magnitude()));

    // 0x6B8E04
    if (handling.m_bForceGroundClearance && linesWereJustAllocated) {
        const auto sphBottomMinZ = 0.25f - m_fHeightAboveRoad;
        for (auto& sph : cd.GetSpheres()) {
            if (sph.m_vecCenter.z - sph.m_fRadius < sphBottomMinZ) {
                if (sph.m_fRadius > 0.4f) {
                    sph.m_fRadius = std::max(sph.m_vecCenter.z - sphBottomMinZ, 0.4f);
                }
                sph.m_vecCenter.z = sphBottomMinZ + sph.m_fRadius;
            }
        }
    }
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
    const auto& mi          = *GetVehicleModelInfo();
    const auto  fwdSpeedAbs = std::fabs(m_vecMoveSpeed.Dot(GetForward()));

    // Lines whose ratio may still be changed by the soft ground (sand) code below
    std::array<bool, NUM_SUSP_LINES> canSink{ true, true, true, true };

    // 0x6B69B2
    for (auto wheel = 0u; wheel < m_nWheelStatus.size(); wheel++) {
        const auto lineA = wheel * 2, lineB = lineA + 1; // Each wheel has 2 suspension lines

        const auto GetRemainingCompression = [&] {
            return (m_fLineLength[lineA] - m_fSuspensionLength[lineA]) / m_fLineLength[lineA]; // NOTE: `lineA`'s lengths are used for both lines
        };

        switch ((eCarWheelStatus)m_nWheelStatus[wheel]) {
        case WHEEL_STATUS_MISSING: { // 0x6B69CC
            m_aWheelRatios[lineA] = 1.0f;
            m_aWheelRatios[lineB] = 1.0f;
            break;
        }
        case WHEEL_STATUS_BURST: { // 0x6B69F3
            // NOTE: Not `GetRandomNumberInRange`: the original scales by 1/32768 (not 1/32767) over the whole range
            const auto range = (float)((uint16)(fwdSpeedAbs * 40.0f) + 98);
            if ((int32)((float)CGeneral::GetRandomNumber() * (1.0f / 32768.0f) * range) < 100) {
                const auto compression = GetRemainingCompression() * 0.2f;
                m_aWheelRatios[lineA] = std::min(1.0f, compression + m_aWheelRatios[lineA]);
                m_aWheelRatios[lineB] = std::min(1.0f, compression + m_aWheelRatios[lineB]);
            }
            break;
        }
        default: { // 0x6B6A9E
            const auto IsOnRailTrack = [&](size_t line) {
                return m_aWheelRatios[line] < 1.0f && m_aWheelColPoints[line].m_nSurfaceTypeB == SURFACE_RAILTRACK;
            };
            if (!IsOnRailTrack(lineA) && !IsOnRailTrack(lineB)) {
                continue; // Leaves `canSink` set
            }

            // Bump the wheel up every time it rolls over a sleeper (one every 1.5 units of tyre travel, less often above 0.3 speed)
            auto anglePerSleeper = 1.5f / ((wheel == 0 ? mi.m_fWheelSizeFront : mi.m_fWheelSizeRear) * 0.5f);
            if (fwdSpeedAbs > 0.3f) {
                anglePerSleeper = fwdSpeedAbs / 0.3f * anglePerSleeper;
            }
            const auto sleepersPerAngle = 1.0f / anglePerSleeper;

            const float sleepers      = sleepersPerAngle * m_aWheelPitchAngles[wheel];
            const float sleepersFract = sleepers - std::floor(sleepers);
            const float nextSleepers  = (CTimer::GetTimeStep() * m_aWheelAngularVelocity[wheel] + m_aWheelPitchAngles[wheel]) * sleepersPerAngle; // 0x6B6B51
            const auto  nextSleepersFract = nextSleepers - std::floor(nextSleepers);

            const auto angVel = m_aWheelAngularVelocity[wheel];
            if ((angVel > 0.0f && nextSleepersFract < sleepersFract) || (angVel < 0.0f && nextSleepersFract > sleepersFract)) {
                const auto compression = GetRemainingCompression() * 0.3f;
                m_aWheelRatios[lineA] = std::max(0.2f, m_aWheelRatios[lineA] - compression);
                m_aWheelRatios[lineB] = std::max(0.2f, m_aWheelRatios[lineB] - compression);
            }
            break;
        }
        }

        // 0x6B6C25
        canSink[lineA] = false;
        canSink[lineB] = false;
    }

    // 0x6B6C39
    for (auto i = 0u; i < NUM_SUSP_LINES; i++) {
        auto& ratio = m_aWheelRatios[i];
        if (!canSink[i] || !(ratio < 1.0f)) {
            continue;
        }
        if (g_surfaceInfos.GetAdhesionGroup(m_aWheelColPoints[i].m_nSurfaceTypeB) != ADHESION_GROUP_SAND) {
            continue;
        }
        if (ModelIndices::IsRhino(m_nModelIndex)) { // NOTE: Copy-paste from CAutomobile, a bike is never a Rhino
            continue;
        }

        const auto offroadFactor = [&] {
            if (handlingFlags.bOffroadAbility2) {
                return 0.1f;
            }
            if (handlingFlags.bOffroadAbility) {
                return 0.15f;
            }
            return 0.25f;
        }();
        const auto adhesionFactor = std::max(1.0f - fwdSpeedAbs / 0.3f * 0.7f - CWeather::WetRoads * 0.7f, 0.4f);

        ratio += (m_fLineLength[i] - m_fSuspensionLength[i]) / m_fLineLength[i] * adhesionFactor * offroadFactor;
        ratio = std::min(ratio, 1.0f);
    }
}

// 0x6B67E0
bool CBike::SetUpWheelColModel(CColModel* wheelCol) {
    const auto* const mi  = GetVehicleModelInfo();
    const auto* const cm  = GetColModel();
    auto* const       wcd = wheelCol->m_pColData;

    wheelCol->m_boundSphere = cm->m_boundSphere;
    wheelCol->m_boundBox    = cm->m_boundBox;

    // Wheel positions are relative to the chassis frame
    wcd->m_pSpheres[0].Set(mi->m_fWheelSizeFront * 0.5f, GetFrameMatrixRelativeTo(m_aBikeNodes[BIKE_WHEEL_FRONT], m_aBikeNodes[BIKE_CHASSIS]).pos, SURFACE_RUBBER, 0xD);
    wcd->m_pSpheres[1].Set(mi->m_fWheelSizeRear * 0.5f, GetFrameMatrixRelativeTo(m_aBikeNodes[BIKE_WHEEL_REAR], m_aBikeNodes[BIKE_CHASSIS]).pos, SURFACE_RUBBER, 0xF);
    wcd->m_nNumSpheres = 2;

    return true;
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

