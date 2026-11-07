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
    RH_ScopedInstall(SetRemoveAnimFlags, 0x6B5F50);
    RH_ScopedInstall(ReduceHornCounter, 0x6B5F90);
    RH_ScopedVMTInstall(ProcessAI, 0x6BC930, { .Reversed = false });
    RH_ScopedInstall(ProcessBuoyancy, 0x6B5FB0);
    RH_ScopedInstall(ResetSuspension, 0x6B6740, { .Reversed = false });
    RH_ScopedInstall(GetAllWheelsOffGround, 0x6B6790);
    RH_ScopedInstall(DebugCode, 0x6B67A0);
    RH_ScopedInstall(DoSoftGroundResistance, 0x6B6D40, { .Reversed = false });
    RH_ScopedInstall(PlayHornIfNecessary, 0x6B7130);
    RH_ScopedInstall(CalculateLeanMatrix, 0x6B7150);
    RH_ScopedInstall(ProcessRiderAnims, 0x6B7280);
    RH_ScopedInstall(FixHandsToBars, 0x6B7F90);
    RH_ScopedInstall(PlaceOnRoadProperly, 0x6BEEB0);
    RH_ScopedInstall(GetCorrectedWorldDoorPosition, 0x6BF230);
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
    if (!ped->GetIsTypePed()) {
        return;
    }
    for (auto* anim = RpAnimBlendClumpGetFirstAssociation(ped->GetRpClump(), ANIMATION_SECONDARY_TASK_ANIM); anim; anim = RpAnimBlendGetNextAssociation(anim, ANIMATION_SECONDARY_TASK_ANIM)) {
        anim->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE);
    }
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
// Used by CBike, CBmx and CQuadBike (an automobile) - `vehicle` is the vehicle the rider drives, `handling` its bike handling.
// NOTE: The last parameter is never read. All 3 callers (0x6BF425 CBike, 0x6C012C CBmx, 0x6CE29F CQuadBike) push 0.
void CBike::ProcessRiderAnims(CPed* rider, CVehicle* vehicle, CRideAnimData* rideData, tBikeHandlingData* handling, int16 unused) {
    const auto isPlayer = rider->IsPlayer();

    CBike*       bike{};
    CAutomobile* automobile{};
    int16        numContactWheels{};
    if (vehicle->IsBike()) {
        bike             = vehicle->AsBike();
        numContactWheels = bike->m_nNoOfContactWheels;
    } else if (vehicle->IsAutomobile()) { // Quad bike
        automobile       = vehicle->AsAutomobile();
        numContactWheels = automobile->m_nNumContactWheels;
    }

    auto blendRemaining = 1.0f; // How much blend is left for the steer/lean anims
    auto leanFwdTarget  = 0.0f;

    auto* const clump = rider->GetRpClump();
    auto* leftAssoc   = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_LEFT);
    auto* rightAssoc  = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_RIGHT);
    auto* stillAssoc  = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_STILL);
    CAnimBlendAssociation* fwdAssoc{};
    CAnimBlendAssociation* backAssoc{};
    if (isPlayer) {
        fwdAssoc  = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_FWD);
        backAssoc = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_BACK);
    }
    auto* pushesAssoc  = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_PUSHES);
    auto* drivebyAssoc = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_DRIVEBYLHS);
    if (!drivebyAssoc) {
        drivebyAssoc = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_DRIVEBYRHS);
    }
    if (!drivebyAssoc) {
        drivebyAssoc = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_DRIVEBYFT);
    }

    const auto forwardSpeed = vehicle->GetMoveSpeed().Dot(vehicle->GetForward());

    // 0x6B73F0 - The right hand is off the bars while pushing or doing a drive-by
    if (vehicle->IsSubBMX() && (pushesAssoc || drivebyAssoc)) {
        vehicle->AsBike()->m_nFixRightHand = false;
    }

    const auto KnockOffRiders = [&] {
        CEventKnockOffBike riderEvent{ vehicle, vehicle->GetMoveSpeed(), CVector{ 0.0f, 0.0f, 1.0f }, 0.0f, 0.0f, KNOCK_OFF_TYPE_SKIDBACK_FALLR, 2, 0, nullptr, true, false };
        rider->GetEventGroup().Add(&riderEvent);

        if (vehicle->m_apPassengers[0] && vehicle->m_apPassengers[0] != rider) {
            CEventKnockOffBike passengerEvent{ vehicle, vehicle->GetMoveSpeed(), CVector{ 0.0f, 0.0f, 1.0f }, 0.0f, 0.0f, KNOCK_OFF_TYPE_SKIDBACK_FALLR, 2, 0, nullptr, false, false };
            vehicle->m_apPassengers[0]->GetEventGroup().Add(&passengerEvent);
        }
    };

    // 0x6B740A - Spinning too fast for the rider's skill
    if (sq((rider->GetBikeRidingSkill() + 1.0f) * 0.3f) < vehicle->GetTurnSpeed().SquaredMagnitude()) {
        KnockOffRiders();
    }

    // 0x6B754D
    if (!drivebyAssoc && std::fabs(forwardSpeed) < 0.02f) {
        if (!stillAssoc || stillAssoc->GetBlendAmount() < 1.0f && stillAssoc->GetBlendDelta() <= 0.0f) {
            stillAssoc = CAnimManager::BlendAnimation(clump, rideData->AnimGroup, ANIM_ID_BIKE_STILL, 2.0f);
        }
    } else {
        auto fadeOutStillAnims = true;
        if (forwardSpeed < 0.0f) { // 0x6B75D6 - Going backwards
            const auto maxReverseVelocity = vehicle->m_pHandlingData->m_transmissionData.m_MaxReverseVelocity;

            auto reverseSpeedMult = (rider->GetBikeRidingSkill() + 1.0f) * 3.5f;
            if (vehicle->IsSubQuad()) {
                reverseSpeedMult += reverseSpeedMult;
            }

            if (forwardSpeed < reverseSpeedMult * maxReverseVelocity
                && (numContactWheels > 0 || vehicle->GetUp().z < -0.5f && CTimer::GetTimeInMS() - vehicle->m_nLastCollisionTime < 100)
            ) {
                KnockOffRiders(); // 0x6B7642 - Rolling backwards way too fast
                fadeOutStillAnims = false;
            } else if (vehicle->m_GasPedal < 0.0f && forwardSpeed > maxReverseVelocity * 1.5f) { // 0x6B776E - Reversing slowly: push with the feet
                if (!pushesAssoc || pushesAssoc->GetBlendAmount() < 1.0f && pushesAssoc->GetBlendDelta() <= 0.0f) {
                    pushesAssoc = CAnimManager::BlendAnimation(clump, rideData->AnimGroup, ANIM_ID_BIKE_PUSHES, 4.0f);
                }
                fadeOutStillAnims = false;
            } else if (isPlayer && forwardSpeed < maxReverseVelocity * 1.5f) {
                leanFwdTarget = -1.0f; // Lean fully back. Also used as a marker below: no steer anim, no other lean logic
            }
        }

        // 0x6B77E6
        if (fadeOutStillAnims) {
            if (stillAssoc && stillAssoc->GetBlendDelta() >= 0.0f) {
                stillAssoc->SetBlendDelta(-4.0f);
            }
            if (pushesAssoc && pushesAssoc->GetBlendDelta() >= 0.0f) {
                pushesAssoc->SetBlendDelta(-4.0f);
            }
        }
    }

    // 0x6B7821 - Predict the blend amount of next frame (same step as the anim update, but non-clipped)
    const auto GetNextBlendAmount = [](const CAnimBlendAssociation* assoc) {
        return CTimer::GetTimeStepNonClipped() * 0.02f * assoc->GetBlendDelta() + assoc->GetBlendAmount();
    };
    if (stillAssoc) {
        blendRemaining = 1.0f - std::min(GetNextBlendAmount(stillAssoc), 1.0f);
    }
    if (drivebyAssoc) {
        blendRemaining -= std::min(GetNextBlendAmount(drivebyAssoc), blendRemaining);
    }
    if (pushesAssoc) {
        blendRemaining -= std::min(GetNextBlendAmount(pushesAssoc), blendRemaining);
    }

    // 0x6B78D2 - Steer (left/right) anim
    const auto leanLeftTarget = leanFwdTarget == -1.0f
        ? 0.0f
        : std::clamp(rideData->LeanAngle / handling->m_fFullAnimLean, -1.0f, 1.0f);

    const auto leanLeftMult = std::pow(0.86f, CTimer::GetTimeStep()); // 0x6B7932 loads the 0.86f (0x871350) - SilentPatch replaces that `fld` with a call
    rideData->AnimLeanLeft = leanLeftMult * rideData->AnimLeanLeft + (1.0f - leanLeftMult) * leanLeftTarget;

    // 0x6B7949 - Fwd/back lean anim target
    if (!isPlayer || vehicle->m_apPassengers[0]) {
        leanFwdTarget = 0.0f;
        if (bike) {
            bike->bikeFlags.bWheelieForCamera = false;
        }
    } else if (leanFwdTarget > -1.0f) {
        leanFwdTarget = rideData->LeanFwd;

        auto isWheelie = false;
        if (bike) {
            bike->bikeFlags.bWheelieForCamera = false;

            const auto& wc = bike->m_WheelCounts; // 0, 1 - front wheel lines, 2, 3 - rear wheel lines
            const auto  fwdZ = bike->GetForward().z;
            if (wc[0] <= 0.0f && wc[1] <= 0.0f && fwdZ > 0.0f && (wc[2] > 0.0f || wc[3] > 0.0f)) { // 0x6B79A1 - Wheelie
                const auto wheelieAngleLeft = handling->m_fWheelieAng - fwdZ;
                if (wheelieAngleLeft < handling->m_fWheelieAng * 0.5f) {
                    bike->bikeFlags.bWheelieForCamera = true;
                }
                isWheelie = wheelieAngleLeft < 0.15f;
            } else if (wc[2] <= 0.0f && wc[3] <= 0.0f && fwdZ < 0.0f && (wc[0] > 0.0f || wc[1] > 0.0f)) { // 0x6B7A3D - Stoppie
                if (handling->m_fStoppieAng * 0.6f < handling->m_fStoppieAng - fwdZ) {
                    bike->bikeFlags.bWheelieForCamera = true;
                }
            }
        } else if (automobile) { // 0x6B7B14
            // NOTE: Same indices as the bike's lines, but for an automobile 0, 1 are the left wheels (CAR_WHEEL_FRONT_LEFT, CAR_WHEEL_REAR_LEFT)
            //       and 2, 3 the right ones - so for the quad this checks "left side in the air" rather than a wheelie.
            const auto& wc = automobile->m_WheelCounts;
            const auto  fwdZ = automobile->GetForward().z;
            if (wc[0] <= 0.0f && wc[1] <= 0.0f && fwdZ > 0.0f && (wc[2] > 0.0f || wc[3] > 0.0f)) {
                isWheelie = handling->m_fWheelieAng - fwdZ < 0.15f;
            }
        }

        if (isWheelie) { // 0x6B7B90 - Almost at the wheelie angle: lean forward
            leanFwdTarget = std::max(leanFwdTarget, 0.25f);
        } else if (vehicle->m_BrakePedal > 0.5f && forwardSpeed > 0.01f) { // 0x6B7AC4
            leanFwdTarget = std::max(leanFwdTarget, 0.1f);
        } else if (vehicle->m_GasPedal > 0.5f
            && leanFwdTarget <= 0.0f
            && forwardSpeed < vehicle->m_pHandlingData->m_transmissionData.m_MaxFlatVelocity * 0.3f
            && leanFwdTarget >= -0.3f
        ) { // 0x6B7BBA
            leanFwdTarget = -0.3f;
        }

        // 0x6B7C16 - Less fwd/back lean while steering hard
        if (std::fabs(leanLeftTarget) > 0.3f) {
            leanFwdTarget *= std::max(1.0f - (std::fabs(leanLeftTarget) - 0.3f) / (0.56f - 0.3f), 0.0f);
        }
    }

    // 0x6B7C7F
    if (isPlayer) {
        const auto leanFwdMult = std::pow(0.89f, CTimer::GetTimeStep());
        rideData->AnimLeanFwd = leanFwdMult * rideData->AnimLeanFwd + (1.0f - leanFwdMult) * leanFwdTarget;
    } else {
        rideData->AnimLeanFwd = 0.0f;
    }

    // 0x6B7CB7 - Split the remaining blend between the steer and the fwd/back lean anims
    float leanLeftBlend, leanFwdBlend;
    if (std::fabs(rideData->AnimLeanLeft) > 0.56f || !isPlayer) {
        leanLeftBlend = 1.0f;
        leanFwdBlend  = 0.0f;
    } else if (std::fabs(rideData->AnimLeanFwd) > 0.56f) {
        leanLeftBlend = 0.0f;
        leanFwdBlend  = 1.0f;
    } else if (const auto leanMag = CVector2D{ rideData->AnimLeanLeft, rideData->AnimLeanFwd }.Magnitude(); leanMag > 0.01f) {
        const auto invLeanMag = 1.0f / leanMag;
        leanFwdBlend  = std::fabs(rideData->AnimLeanFwd * invLeanMag);
        leanLeftBlend = std::fabs(invLeanMag * rideData->AnimLeanLeft);
    } else {
        leanFwdBlend  = std::fabs(rideData->AnimLeanFwd);
        leanLeftBlend = std::fabs(rideData->AnimLeanLeft);
    }
    leanFwdBlend  *= blendRemaining;
    leanLeftBlend *= blendRemaining;

    // Blend in `active` at `progress` (0 to 1) of its length (it's not played, only posed), blend out `inactive`
    const auto SetLeanAnims = [](CAnimBlendAssociation* active, CAnimBlendAssociation* inactive, float blend, float progress) {
        active->SetBlendAmount(blend);
        active->SetCurrentTime(active->GetHier()->GetTotalTime() * progress);
        active->SetFlag(ANIMATION_IS_PLAYING, false);
        inactive->SetBlendAmount(0.0f);
    };

    // 0x6B7D6E
    if (isPlayer) {
        if (!fwdAssoc) {
            fwdAssoc = CAnimManager::AddAnimation(clump, rideData->AnimGroup, ANIM_ID_BIKE_FWD);
        }
        if (!backAssoc) {
            backAssoc = CAnimManager::AddAnimation(clump, rideData->AnimGroup, ANIM_ID_BIKE_BACK);
        }
        if (rideData->AnimLeanFwd < 0.0f) {
            SetLeanAnims(backAssoc, fwdAssoc, leanFwdBlend, -rideData->AnimLeanFwd);
        } else {
            SetLeanAnims(fwdAssoc, backAssoc, leanFwdBlend, rideData->AnimLeanFwd);
        }
    }

    // 0x6B7E0A
    if (!leftAssoc) {
        leftAssoc = CAnimManager::AddAnimation(clump, rideData->AnimGroup, ANIM_ID_BIKE_LEFT);
    }
    if (!rightAssoc) {
        rightAssoc = CAnimManager::AddAnimation(clump, rideData->AnimGroup, ANIM_ID_BIKE_RIGHT);
    }
    if (rideData->AnimLeanLeft < 0.0f) {
        SetLeanAnims(leftAssoc, rightAssoc, leanLeftBlend, -rideData->AnimLeanLeft);
    } else {
        SetLeanAnims(rightAssoc, leftAssoc, leanLeftBlend, rideData->AnimLeanLeft);
    }

    // 0x6B7EA6 - Shake the head at speed (random angles each frame)
    if (forwardSpeed > 0.3f) {
        auto* const headQuat = &rider->m_apBones[PED_NODE_HEAD]->KeyFrame->q;
        const auto  maxShake = forwardSpeed * 6.0f; // degrees
        RtQuatRotate(headQuat, &CPedIK::XaxisIK, CGeneral::GetRandomNumberInRange(-maxShake, maxShake), rwCOMBINEPOSTCONCAT);
        RtQuatRotate(headQuat, &CPedIK::YaxisIK, CGeneral::GetRandomNumberInRange(-maxShake, maxShake), rwCOMBINEPOSTCONCAT);
        rider->bUpdateMatricesRequired = true;
    }
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

// Handlebar grip (right hand) position in chassis frame space, used if the model has no `DUMMY_HAND_REST`
static constexpr CVector BIKE_HAND_REST_DEFAULT{ 0.25f, 0.29f, 0.525f }; // 0x8D3244
static constexpr CVector MTBIKE_HAND_REST{ 0.25f, 0.29f, 0.525f };       // 0x8D3250
static constexpr CVector BIKE_HAND_REST{ 0.25f, 0.21f, 0.69f };          // 0x8D325C

// Offset from the right hand bone to the point that holds the grip, in hand bone space
static constexpr CVector RIGHT_HAND_GRIP_OFFSET{ -0.075f, -0.03f, 0.0f }; // 0x8D3274

// 0x6B7F90
void CBike::FixHandsToBars(CPed* rider) {
    if (!m_nFixRightHand && !m_nFixLeftHand) {
        return;
    }

    auto* const chassisFrame = m_aBikeNodes[BIKE_CHASSIS];
    if (!chassisFrame) {
        return; // NOTE: Unlike below, the fix hand flags are not cleared here
    }

    CMatrix chassisMat{ GetMatrix() };
    chassisMat *= CMatrix{ RwFrameGetMatrix(chassisFrame) }; // World matrix of the chassis frame

    auto* const hier = GetAnimHierarchyFromSkinClump(rider->GetRpClump());
    const auto GetBoneMatrix = [hier](eBoneTag bone) {
        return &RpHAnimHierarchyGetMatrixArray(hier)[RpHAnimIDGetIndex(hier, bone)];
    };
    const auto GetBonePos = [&](eBoneTag bone) -> CVector& {
        return *static_cast<CVector*>(RwMatrixGetPos(GetBoneMatrix(bone)));
    };

    // 0x6B8043 - Right grip position in chassis frame space
    CVector gripPos = GetVehicleModelInfo()->GetModelDummyPosition(DUMMY_HAND_REST);
    if (gripPos.IsZero()) {
        switch (GetModelId()) {
        case MODEL_MTBIKE: gripPos = MTBIKE_HAND_REST; break;
        case MODEL_BIKE:   gripPos = BIKE_HAND_REST; break;
        default:           gripPos = BIKE_HAND_REST_DEFAULT; break;
        }
    } else { // 0x6B811F - The dummy marks the grip, move it by the (current) hand -> grip offset, so the hand bone ends up there
        gripPos += chassisMat.InverseTransformVector(CMatrix{ GetBoneMatrix(BONE_R_HAND) }.TransformVector(RIGHT_HAND_GRIP_OFFSET));
    }

    // 0x6B8200 - Move the hand (and fingers) onto the grip, the arm (and breast) partially
    if (m_nFixRightHand) {
        const auto delta = chassisMat.TransformPoint(gripPos) - GetBonePos(BONE_R_HAND);
        GetBonePos(BONE_R_HAND)      += delta;
        GetBonePos(BONE_R_FINGER)    += delta;
        GetBonePos(BONE_R_FINGER_01) += delta;
        GetBonePos(BONE_R_FORE_ARM)  += delta * 0.667f;

        const auto upperArmDelta = delta * 0.333f;
        GetBonePos(BONE_R_UPPER_ARM) += upperArmDelta;
        if (rider->IsPlayer()) {
            GetBonePos(BONE_R_BREAST) += upperArmDelta;
        }
    }

    // 0x6B8551 - Same for the left hand, mirrored
    // NOTE: The arm factors differ from the right hand's (0.75/0.4 vs 0.667/0.333)
    if (m_nFixLeftHand) {
        gripPos.x *= -1.0f;

        const auto delta = chassisMat.TransformPoint(gripPos) - GetBonePos(BONE_L_HAND);
        GetBonePos(BONE_L_HAND)      += delta;
        GetBonePos(BONE_L_FINGER)    += delta;
        GetBonePos(BONE_L_FINGER_01) += delta;
        GetBonePos(BONE_L_FORE_ARM)  += delta * 0.75f;

        const auto upperArmDelta = delta * 0.4f;
        GetBonePos(BONE_L_UPPER_ARM) += upperArmDelta;
        if (rider->IsPlayer()) {
            GetBonePos(BONE_L_BREAST) += upperArmDelta;
        }
    }

    m_nFixLeftHand  = false;
    m_nFixRightHand = false;
}

// 0x6BEEB0
void CBike::PlaceOnRoadProperly() {
    const auto& bb        = GetColModel()->GetBoundingBox();
    const auto  frontDist = bb.m_vecMax.y;
    const auto  rearDist  = -bb.m_vecMin.y;

    const CVector2D fwd2D = GetForward();
    const auto      rear  = CVector2D{ GetPosition() } - fwd2D * rearDist;
    const auto      front = fwd2D * frontDist + CVector2D{ GetPosition() };

    // Returns the ground's Z below (or at most 5 units above) `pos2D`, or our Z if there's none
    const auto GetGroundZ = [this](CVector2D pos2D, CStoredCollPoly& collPoly) {
        CColPoint colPoint{};
        CEntity*  colEntity{};
        const auto posZ = GetPosition().z;
        if (!CWorld::ProcessVerticalLine({ pos2D.x, pos2D.y, posZ + 5.0f }, posZ - 5.0f, colPoint, colEntity, true)) {
            return GetPosition().z;
        }
        collPoly.ligthing   = colPoint.m_nLightingB; // NOTE: Original writes this as a zero-extended `uint32`, clearing the 3 padding bytes after it
        m_pEntityWeAreOn    = colEntity;
        m_bTunnel           = colEntity->m_bTunnel;
        m_bTunnelTransition = colEntity->m_bTunnelTransition;
        return colPoint.m_vecPoint.z;
    };
    const auto frontZ = GetGroundZ(front, m_FrontCollPoly); // 0x6BEFB4
    const auto rearZ  = GetGroundZ(rear, m_RearCollPoly);   // 0x6BF087

    // 0x6BF0F1 - Pitch the bike to the slope between the 2 points, no roll
    const auto length = rearDist + frontDist;
    const auto pitch  = std::atan((frontZ - rearZ) / length);

    auto& right = GetRight();
    right = CVector{ (front.y - rear.y) / length, -((front.x - rear.x) / length), 0.0f };

    auto& forward = GetForward();
    forward = CVector{ -(std::cos(pitch) * right.y), std::cos(pitch) * right.x, std::sin(pitch) };

    GetUp() = right.Cross(forward);

    SetPosn({ (front.x + rear.x) * 0.5f, (front.y + rear.y) * 0.5f, GetHeightAboveRoad() + (rearZ + frontZ) * 0.5f });
}

// 0x6BF230
// Callers: CCarEnterExit::GetPositionToOpenCarDoor (0x64EBB3), CTaskUtilityLineUpPedWithCar::GetLocalPositionToOpenCarDoor (0x65007C)
// @param out            World position the ped has to stand at to start the enter anim
// @param animDoorOffset The enter anim's root translation (`CVehicleAnimGroup::ComputeAnimDoorOffsets`, X mirrored for right side doors)
// @param seatPos        The seat dummy's position (`DUMMY_SEAT_FRONT` / `DUMMY_SEAT_REAR`) in model space
//
// Transforms `seatPos - animDoorOffset` into world space as if the bike stood upright (lean ignored, only heading and pitch are used),
// then shifts it sideways by how much the lean moved the top of the bike.
void CBike::GetCorrectedWorldDoorPosition(CVector& out, CVector animDoorOffset, CVector seatPos) {
    const auto right = GetForward().Cross(CVector{ 0.0f, 0.0f, 1.0f }); // Horizontal right
    const auto up    = right.Cross(GetForward());                       // Up without the lean
    const auto leanSideways = right.Dot(GetUp());                       // Sideways component of the real (leaned) up

    const auto& bbMax = GetColModel()->GetBoundingBox().m_vecMax;
    const auto heightOverHalfWidth = bbMax.z > bbMax.x ? bbMax.z - bbMax.x : 0.0f;

    out = CVector{};
    out += GetForward() * (seatPos.y - animDoorOffset.y);
    out += right * (heightOverHalfWidth * leanSideways + (seatPos.x - animDoorOffset.x));
    out += up * (seatPos.z - animDoorOffset.z);
    out += GetPosition();
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

