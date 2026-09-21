/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Bike.h"

#include "Buoyancy.h"
#include "VehicleRecording.h"

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
    RH_ScopedInstall(ProcessAI, 0x6BC930, { .reversed = false });
    RH_ScopedInstall(ProcessBuoyancy, 0x6B5FB0);
    RH_ScopedInstall(ResetSuspension, 0x6B6740);
    RH_ScopedInstall(GetAllWheelsOffGround, 0x6B6790);
    RH_ScopedInstall(DebugCode, 0x6B67A0);
    RH_ScopedInstall(DoSoftGroundResistance, 0x6B6D40);
    RH_ScopedInstall(PlayHornIfNecessary, 0x6B7130);
    RH_ScopedInstall(CalculateLeanMatrix, 0x6B7150);
    RH_ScopedInstall(ProcessRiderAnims, 0x6B7280, { .reversed = false });
    RH_ScopedInstall(FixHandsToBars, 0x6B7F90, { .reversed = false });
    RH_ScopedInstall(PlaceOnRoadProperly, 0x6BEEB0);
    RH_ScopedInstall(GetCorrectedWorldDoorPosition, 0x6BF230);
    RH_ScopedVMTInstall(Fix, 0x6B7050);
    RH_ScopedVMTInstall(BlowUpCar, 0x6BEA10);
    RH_ScopedVMTInstall(ProcessDrivingAnims, 0x6BF400);
    RH_ScopedVMTInstall(ProcessControlInputs, 0x6BE310);
    RH_ScopedVMTInstall(ProcessEntityCollision, 0x6BDEA0);
    RH_ScopedVMTInstall(Render, 0x6BDE20);
    RH_ScopedVMTInstall(PreRender, 0x6BD090, { .reversed = false });
    RH_ScopedVMTInstall(Teleport, 0x6BCFC0);
    RH_ScopedVMTInstall(ProcessControl, 0x6B9250, { .reversed = false });
    RH_ScopedVMTInstall(VehicleDamage, 0x6B8EC0);
    RH_ScopedVMTInstall(SetupSuspensionLines, 0x6B89B0);
    RH_ScopedVMTInstall(SetModelIndex, 0x6B8970);
    RH_ScopedVMTInstall(PlayCarHorn, 0x6B7080);
    RH_ScopedVMTInstall(SetupDamageAfterLoad, 0x6B7070);
    RH_ScopedVMTInstall(DoBurstAndSoftGroundRatios, 0x6B6950);
    RH_ScopedVMTInstall(SetUpWheelColModel, 0x6B67E0);
    RH_ScopedVMTInstall(RemoveRefsToVehicle, 0x6B67B0);
    RH_ScopedVMTInstall(ProcessControlCollisionCheck, 0x6B6620);
    RH_ScopedVMTInstall(GetComponentWorldPosition, 0x6B5990);
    RH_ScopedVMTInstall(ProcessOpenDoor, 0x6B58D0);
}

// 0x6BF430
CBike::CBike(int32 modelIndex, eVehicleCreatedBy createdBy) :
    CVehicle(createdBy) {
    auto mi = CModelInfo::GetModelInfo(modelIndex)->AsVehicleModelInfoPtr();
    if (mi->m_nVehicleType == VEHICLE_TYPE_BIKE) {
        const auto& animationStyle = CAnimManager::GetAnimBlocks()[mi->GetAnimFileIndex()].GroupId;
        m_RideAnimData.AnimGroup   = animationStyle;
        if (animationStyle < ANIM_GROUP_BIKES || animationStyle > ANIM_GROUP_WAYFARER) {
            m_RideAnimData.AnimGroup = ANIM_GROUP_BIKES;
        }
    }

    m_nVehicleSubType = VEHICLE_TYPE_BIKE;
    m_nVehicleType    = VEHICLE_TYPE_BIKE;

    m_BlowUpTimer     = 0.0f;
    m_nBrakesOn       = false;
    nBikeFlags        = 0;
    SetModelIndex(modelIndex);

    m_pHandlingData          = gHandlingDataMgr.GetVehiclePointer(mi->m_nHandlingId);
    m_BikeHandling           = gHandlingDataMgr.GetBikeHandlingPointer(mi->m_nHandlingId);
    m_nHandlingFlagsIntValue = m_pHandlingData->m_nHandlingFlags;
    m_pFlyingHandlingData    = gHandlingDataMgr.GetFlyingPointer(static_cast<uint8>(mi->m_nHandlingId));
    m_fBrakeCount            = 20.0f;
    mi->ChooseVehicleColour(m_nPrimaryColor, m_nSecondaryColor, m_nTertiaryColor, m_nQuaternaryColor, 1);
    m_fSwingArmLength       = 0.0f;
    m_fForkYOffset          = 0.0f;
    m_fForkZOffset          = 0.0f;
    m_nFixLeftHand          = false;
    m_nFixRightHand         = false;
    m_fSteerAngleTan        = std::tan(DegreesToRadians(mi->m_fBikeSteerAngle));
    m_fMass                 = m_pHandlingData->m_fMass;
    m_fTurnMass             = m_pHandlingData->m_fTurnMass;
    m_vecCentreOfMass       = m_pHandlingData->m_vecCentreOfMass;
    m_vecCentreOfMass.z     = 0.1f;
    m_fAirResistance        = GetDefaultAirResistance();
    m_fElasticity           = 0.05f;
    m_fBuoyancyConstant     = m_pHandlingData->m_fBuoyancyConstant;
    m_fSteerAngle           = 0.0f;
    m_GasPedal              = 0.0f;
    m_BrakePedal            = 0.0f;
    m_Damager               = nullptr;
    m_pWhoInstalledBombOnMe = nullptr;
    m_GasPedalAudioRevs     = 0.0f;
    m_fTyreTemp             = 1.0f;
    m_fBrakingSlide         = 0.0f;
    m_PrevSpeed             = 0.0f;

    for (auto i = 0; i < 2; ++i) {
        m_nWheelStatus[i]            = 0;
        m_aWheelSkidmarkType[i]      = eSkidmarkType::DEFAULT;
        m_bWheelBloody[i]            = false;
        m_bMoreSkidMarks[i]          = false;
        m_aWheelPitchAngles[i]       = 0.0f;
        m_aWheelAngularVelocity[i]   = 0.0f;
        m_aWheelSuspensionHeights[i] = 0.0f;
        m_aWheelOrigHeights[i]       = 0.0f;
        m_WheelStates[i]             = WHEEL_STATE_NORMAL;
    }

    for (auto i = 0; i < 4; ++i) {
        m_aWheelColPoints[i]     = {};
        m_aWheelRatios[i]        = 1.0f;
        m_aRatioHistory[i]       = 0.0f;
        m_WheelCounts[i]         = 0.0f;
        m_fSuspensionLength[i]   = 0.0f;
        m_fLineLength[i]         = 0.0f;
        m_aGroundPhysicalPtrs[i] = nullptr;
        m_aGroundOffsets[i]      = CVector{};
    }

    m_nNoOfContactWheels              = 0;
    m_NumDriveWheelsOnGround          = 0;
    m_NumDriveWheelsOnGroundLastFrame = 0;
    m_fHeightAboveRoad                = 0.0f;
    m_fExtraTractionMult              = 1.0f;

    if (!mi->m_pColModel->m_pColData->m_pLines) {
        mi->m_pColModel->m_pColData->m_nNumLines              = 4;
        mi->m_pColModel->m_pColData->m_pLines                 = static_cast<CColLine*>(CMemoryMgr::Malloc(4 * sizeof(CColLine)));
        mi->m_pColModel->m_pColData->m_pLines[1].m_vecStart.x = 99'999.99f; // todo: explain this
    }
    mi->m_pColModel->m_pColData->m_pLines[0].m_vecStart.z = 99'999.99f;
    CBike::SetupSuspensionLines();

    m_autoPilot.m_nTempAction = TEMPACT_NONE;
    m_autoPilot.SetCarMission(MISSION_NONE, 0);
    m_autoPilot.carCtrlFlags.bAvoidLevelTransitions = false;

    SetStatus(STATUS_SIMPLE);
    m_nNumPassengers         = 0;
    vehicleFlags.bLowVehicle = false;
    vehicleFlags.bIsBig      = false;
    vehicleFlags.bIsVan      = false;

    m_bLeanMatrixCalculated  = false;
    m_mLeanMatrix            = *m_matrix;
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
        fwdWeight           = 7.0f * sq(vertical) + 0.6f;
    }
    if (vehicle->GetUp().z < 0.0f) { // bike lying on its side / upside down
        fwdWeight = 5.0f;
    }

    auto backWeight = 1.5f;
    auto upWeight   = 0.05f;
    if (vehicle->m_nModelIndex == MODEL_SANCHEZ) {
        fwdWeight *= 0.65f;
        upWeight *= 0.75f;
    } else if (vehicle->IsSubQuad()) {
        backWeight = 3.0f;
        fwdWeight *= 0.65f;
        upWeight *= 0.75f;
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

    driver->GetEventGroup().Add(CEventKnockOffBike{ vehicle, vehicle->m_vecMoveSpeed, collisionImpactVelocity, damageIntensity, 0.05f * force, KNOCK_OFF_TYPE_SKIDBACKFRONT, knockOffDir, 0, nullptr, true, false });
    if (passenger) {
        passenger->GetEventGroup().Add(CEventKnockOffBike{ vehicle, vehicle->m_vecMoveSpeed, collisionImpactVelocity, damageIntensity, 0.05f * force, KNOCK_OFF_TYPE_SKIDBACKFRONT, knockOffDir, 0, nullptr, false, false });
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
    for (auto assoc = RpAnimBlendClumpGetFirstAssociation(ped->GetRpClump(), ANIMATION_SECONDARY_TASK_ANIM); assoc; assoc = RpAnimBlendGetNextAssociation(assoc, ANIMATION_SECONDARY_TASK_ANIM)) {
        assoc->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
    }
}

// 0x6B5F90
void CBike::ReduceHornCounter() {
    if (m_HornCounter) {
        m_HornCounter -= 1;
    }
}

// 0x6B5FB0
void CBike::ProcessBuoyancy() {
    CVector vecBuoyancyTurnPoint;
    CVector vecBuoyancyForce;
    if (!mod_Buoyancy.ProcessBuoyancy(this, m_fBuoyancyConstant, &vecBuoyancyTurnPoint, &vecBuoyancyForce)) {
        vehicleFlags.bIsDrowning        = false;
        physicalFlags.bSubmergedInWater = false;
        physicalFlags.bTouchingWater    = false;
        return;
    }

    physicalFlags.bTouchingWater = true;
    ApplyMoveForce(vecBuoyancyForce);
    ApplyTurnForce(vecBuoyancyForce, vecBuoyancyTurnPoint);

    auto fTimeStep       = std::max(0.01F, CTimer::GetTimeStep());
    auto fUsedMass       = m_fMass / 125.0F;
    auto fBuoyancyForceZ = vecBuoyancyForce.z / (fTimeStep * fUsedMass);

    if (fUsedMass > m_fBuoyancyConstant) {
        fBuoyancyForceZ *= 1.05F * fUsedMass / m_fBuoyancyConstant;
    }

    if (physicalFlags.bMakeMassTwiceAsBig) {
        fBuoyancyForceZ *= 1.5F;
    }

    auto fBuoyancyForceMult = std::max(0.5F, 1.0F - fBuoyancyForceZ / 20.0F);
    auto fSpeedMult         = std::pow(fBuoyancyForceMult, CTimer::GetTimeStep());
    m_vecMoveSpeed *= fSpeedMult;
    m_vecTurnSpeed *= fSpeedMult;

    // 0x6B6443
    if (fBuoyancyForceZ > 0.8F || (fBuoyancyForceZ > 0.4F && IsAnyWheelNotMakingContactWithGround())) {
        vehicleFlags.bIsDrowning        = true;
        physicalFlags.bSubmergedInWater = true;

        m_vecMoveSpeed.z                = std::max(-0.1F, m_vecMoveSpeed.z);

        if (m_pDriver) {
            ProcessPedInVehicleBuoyancy(m_pDriver->AsPed(), true);
        } else {
            vehicleFlags.bEngineOn = false;
        }

        for (const auto passenger : GetPassengers()) {
            ProcessPedInVehicleBuoyancy(passenger, false);
        }
    } else {
        vehicleFlags.bIsDrowning        = false;
        physicalFlags.bSubmergedInWater = false;
    }
}

inline void CBike::ProcessPedInVehicleBuoyancy(CPed* ped, bool bIsDriver) {
    if (!ped) {
        return;
    }

    ped->physicalFlags.bTouchingWater = true;
    if (!ped->IsPlayer() && bikeFlags.bWaterTight) {
        return;
    }

    if (ped->IsPlayer()) {
        ped->AsPlayer()->HandlePlayerBreath(true, 1.0F);
    }

    if (IsAnyWheelMakingContactWithGround()) {
        if (!ped->IsPlayer()) {
            auto pedDamageResponseCalc = CPedDamageResponseCalculator(this, CTimer::GetTimeStep(), eWeaponType::WEAPON_DROWNING, PED_PIECE_TORSO, false);
            auto damageEvent           = CEventDamage(this, CTimer::GetTimeInMS(), eWeaponType::WEAPON_DROWNING, PED_PIECE_TORSO, 0, false, true);
            if (damageEvent.AffectsPed(ped)) {
                pedDamageResponseCalc.ComputeDamageResponse(ped, damageEvent.m_damageResponse, true);
            } else {
                damageEvent.m_damageResponse.m_bDamageCalculated = true;
            }

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
    if (m_bOffscreen && GetStatus() == STATUS_PLAYER) {
        return;
    }

    ProcessRiderAnims(driver, this, &m_RideAnimData, m_BikeHandling, 0);
}

// 0x6B7280
void CBike::ProcessRiderAnims(CPed* rider, CVehicle* vehicle, CRideAnimData* rideData, tBikeHandlingData* handling, int16 a5) {
    plugin::Call<0x6B7280, CPed*, CVehicle*, CRideAnimData*, tBikeHandlingData*, int16>(rider, vehicle, rideData, handling, a5);
}

// 0x6BEB20
bool CBike::BurstTyre(uint8 tyreComponentId, bool bPhysicalEffect) {
    if (vehicleFlags.bTyresDontBurst) {
        return false;
    }
    if (physicalFlags.bRenderScorched) {
        return false;
    }

    auto wheel = tyreComponentId;
    if (wheel == 0xD) {
        wheel = 0;
    } else if (wheel == 0xF) {
        wheel = 1;
    }

    bool burst = false;
    if (m_nWheelStatus[wheel] == WHEEL_STATUS_OK) {
        m_nWheelStatus[wheel] = WHEEL_STATUS_BURST;
        m_vehicleAudio.AddAudioEvent(AE_TYRE_BURST, 0.0f);
        if (GetStatus() == STATUS_SIMPLE) {
            CCarCtrl::SwitchVehicleToRealPhysics(this);
        }
        if (bPhysicalEffect) {
            constexpr auto force = 0.02f;
            ApplyMoveForce(m_matrix->GetRight() * CGeneral::GetRandomNumberInRange(-force, force) * m_fMass);
            ApplyTurnForce(
                m_matrix->GetRight() * CGeneral::GetRandomNumberInRange(-force, force) * m_fTurnMass,
                m_matrix->GetForward()
            );
        }
        burst = true;
    }

    if (!m_pDriver) {
        return burst;
    }

    // NOTE: `wheel` is compared against the raw component ids below, exactly as the original does,
    // even though the remap above means the 0xD case can never be taken.
    if (wheel == 0xD) {
        if (m_aRatioHistory[0] >= 1.0f && m_aRatioHistory[1] >= 1.0f) {
            return burst;
        }
    } else {
        if (wheel != 0xE) {
            return burst;
        }
        if (m_aRatioHistory[2] >= 1.0f && m_aRatioHistory[3] >= 1.0f) {
            return burst;
        }
    }

    const auto speed = m_vecMoveSpeed.Magnitude();
    if (speed <= 0.3f || (GetStatus() == STATUS_PLAYER && speed <= 0.55f)) {
        return burst;
    }

    if (wheel == 0xD) {
        auto knockOff = CEventKnockOffBike{ this, m_vecMoveSpeed, m_vecLastCollisionImpactVelocity, 0.0f, 0.0f, KNOCK_OFF_TYPE_SKIDBACKFRONT, 0, 0, nullptr, true, false };
        m_pDriver->GetEventGroup().Add(&knockOff, false);
        if (m_apPassengers[0]) {
            auto knockOffPassenger = CEventKnockOffBike{ this, m_vecMoveSpeed, m_vecLastCollisionImpactVelocity, 0.0f, 0.0f, KNOCK_OFF_TYPE_SKIDBACKFRONT, 0, 0, nullptr, false, false };
            m_apPassengers[0]->GetEventGroup().Add(&knockOffPassenger, false);
        }
    } else {
        ApplyTurnForce(m_matrix->GetRight() * (0.02f * 2.0f * m_fTurnMass), m_matrix->GetForward());
    }
    return burst;
}

// 0x6BE310
void CBike::ProcessControlInputs(uint8 playerNum) {
    const float fwdSpeed = m_vecMoveSpeed.Dot(GetForward());
    CPad* pad = CPad::GetPad(playerNum);
    if (pad->GetExitVehicle()) {
        vehicleFlags.bIsHandbrakeOn = true;
    } else {
        vehicleFlags.bIsHandbrakeOn = pad->GetHandBrake() != 0;
    }
    const float timeStep = CTimer::GetTimeStep();
    if (!CCamera::m_bUseMouse3rdPerson || !CVehicle::m_bEnableMouseSteering) {
        m_fRawSteerAngle += (float)(-(float)pad->GetSteeringLeftRight() * 0.0078125f - m_fRawSteerAngle) * timeStep * 0.2f;
        m_RideAnimData.LeanFwd += (float)(-(float)pad->GetSteeringUpDown() * 0.0078125f - m_RideAnimData.LeanFwd) * timeStep * 0.2f;
    } else if (CPad::NewMouseControllerState.m_AmountMoved.x == 0.0f && CPad::NewMouseControllerState.m_AmountMoved.y == 0.0f) {
        if (std::fabs(m_fRawSteerAngle) > 0.0f && CVehicle::m_nLastControlInput == eControllerType::MOUSE
            && pad->GetSteeringLeftRight() == 0 && pad->GetSteeringUpDown() == 0) {
            goto mouseDrift;
        }
        if (pad->GetSteeringLeftRight() == 0 && pad->GetSteeringUpDown() == 0 && CVehicle::m_nLastControlInput == eControllerType::MOUSE) {
            goto clampSteer;
        }
        CVehicle::m_nLastControlInput = eControllerType::KEYBOARD;
        m_fRawSteerAngle += (float)(-(float)pad->GetSteeringLeftRight() * 0.0078125f - m_fRawSteerAngle) * timeStep * 0.2f;
        m_RideAnimData.LeanFwd += (float)(-(float)pad->GetSteeringUpDown() * 0.0078125f - m_RideAnimData.LeanFwd) * timeStep * 0.2f;
    } else {
mouseDrift:
        CVehicle::m_nLastControlInput = eControllerType::MOUSE;
        if (pad->NewState.m_bVehicleMouseLook == 0) {
            m_fRawSteerAngle += CPad::NewMouseControllerState.m_AmountMoved.x * -0.0035f;
            m_RideAnimData.LeanFwd += CPad::NewMouseControllerState.m_AmountMoved.y * -0.0035f;
        }
        if (std::fabs(m_fRawSteerAngle) < 0.35f || pad->NewState.m_bVehicleMouseLook != 0) {
            m_fRawSteerAngle *= std::pow(0.98f, CTimer::GetTimeStep());
        }
        if (std::fabs(m_RideAnimData.LeanFwd) < 0.35f || pad->NewState.m_bVehicleMouseLook != 0) {
            m_RideAnimData.LeanFwd *= std::pow(0.98f, CTimer::GetTimeStep());
        }
    }
clampSteer:
    m_fRawSteerAngle = std::clamp(m_fRawSteerAngle, -1.0f, 1.0f);
    m_RideAnimData.LeanFwd = std::clamp(m_RideAnimData.LeanFwd, -1.0f, 1.0f);
    const float input = (float)(pad->GetAccelerate() - pad->GetBrake()) * 0.0039215689f;
    if (std::fabs(fwdSpeed) >= 0.01f) {
        if (fwdSpeed < 0.0f) {
            if (input >= 0.0f) {
                m_BrakePedal = input;
                m_GasPedal = 0.0f;
                goto updateSteer;
            }
            m_GasPedal = input;
        } else {
            if (input < 0.0f) {
                m_GasPedal = 0.0f;
                m_BrakePedal = -input;
                goto updateSteer;
            }
            m_GasPedal = input;
        }
    } else if (pad->GetAccelerate() > 150 && pad->GetBrake() > 150 && m_nVehicleSubType != VEHICLE_TYPE_BMX) {
        m_GasPedal = (float)pad->GetAccelerate() * 0.0039215689f;
        m_nBrakesOn = 1;
        m_BrakePedal = (float)pad->GetBrake() * 0.0039215689f;
        goto updateSteer;
    } else {
        m_GasPedal = input;
    }
    m_BrakePedal = 0.0f;
updateSteer: {
        float signedSquare = m_fRawSteerAngle * m_fRawSteerAngle;
        if (m_fRawSteerAngle < 0.0f) {
            signedSquare = -signedSquare;
        }
        StaticRef<float>(0xC1C804) = signedSquare; // 0xC1C804: unknown static kept for fidelity
        if (m_autoPilot.m_vehicleRecordingId < 0 || CVehicleRecording::bUseCarAI[m_autoPilot.m_vehicleRecordingId]) {
            m_fSteerAngle = m_pHandlingData->m_fSteeringLock * 0.017453292f * signedSquare;
        }
    }
    if (vehicleFlags.bComedyControls) {
        if ((CTimer::GetTimeInMS() & 0x3C00) < 0x3000) {
            m_GasPedal = 1.0f;
        }
        if (((CTimer::GetTimeInMS() >> 10) + 6 & 0xF) < 0xC) {
            m_BrakePedal = 0.0f;
        }
        vehicleFlags.bIsHandbrakeOn = false;
        if ((CTimer::GetTimeInMS() & 0x800) == 0) {
            m_fSteerAngle -= 0.03f;
        } else {
            m_fSteerAngle += 0.08f;
        }
    }
    if (CPad::GetPad(0)->DisablePlayerControls != 0 && CGameLogic::SkipState != SKIP_IN_PROGRESS) {
        m_BrakePedal = 1.0f;
        vehicleFlags.bIsHandbrakeOn = true;
        m_GasPedal = 0.0f;
        FindPlayerPed()->KeepAreaAroundPlayerClear();
        const float speed = m_vecMoveSpeed.Magnitude();
        if (speed > 0.28f) {
            m_vecMoveSpeed *= 0.28f / speed;
        }
    }
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

    auto numColPts           = CCollision::ProcessColModels(
        GetMatrix(), *GetColModel(), entity->GetMatrix(), *entity->GetColModel(), *(std::array<CColPoint, 32>*)(outColPoints), m_aWheelColPoints.data(), m_aWheelRatios.data(), false
    );

    // Possibly add driver & entity collisions to `outColPoints`
    if (m_pDriver && m_nTestPedCollision) {
        const auto pcd = m_pDriver->GetColData();
        if (!pcd->m_nNumLines) {
            std::array<CColPoint, 32> pedCPs{};

            CMatrix driverMat = GetMatrix();
            driverMat.GetPosition() += GetDriverSeatDummyPositionWS();

            std::array<CColPoint, 32> pedEntityColPts{};
            const auto                numPedEntityColPts = CCollision::ProcessColModels(
                driverMat, *m_pDriver->GetColModel(), entity->GetMatrix(), *entity->GetColModel(), pedEntityColPts, nullptr, nullptr, false
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
            const auto& cp                  = m_aWheelColPoints[i];

            const auto wheelColPtsTouchDist = m_aWheelRatios[i];
            if (wheelColPtsTouchDist >= 1.f || wheelColPtsTouchDist >= ogWheelRatios[i]) {
                continue;
            }

            numProcessedLines++;

            m_anCollisionLighting[i] = cp.m_nLightingB;
            m_nContactSurface        = cp.m_nSurfaceTypeB;

            switch (entity->GetType()) {
            case ENTITY_TYPE_VEHICLE:
            case ENTITY_TYPE_OBJECT:  {
                CEntity::ChangeEntityReference(m_aGroundPhysicalPtrs[i], entity->AsPhysical());

                m_aGroundOffsets[i] = cp.m_vecPoint - entity->GetPosition();
                if (entity->GetIsTypeVehicle()) {
                    m_anCollisionLighting[i] = entity->AsVehicle()->m_anCollisionLighting[i];
                }
                break;
            }
            case ENTITY_TYPE_BUILDING: {
                m_pEntityWeAreOn    = entity;
                m_bTunnel           = entity->m_bTunnel;
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
            if (entity->GetIsTypeBuilding()
                || (entity->GetIsTypeObject() && entity->AsPhysical()->physicalFlags.bDisableCollisionForce)) {
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
    for (auto i = 0; i < 2; i++) {
        m_aWheelPitchAngles[i] = 0.0f;
        m_WheelStates[i]       = WHEEL_STATE_NORMAL;
    }
    for (auto i = 0u; i < NUM_SUSP_LINES; i++) {
        m_aWheelRatios[i] = 1.0f;
        m_WheelCounts[i]  = 0.0f;
    }
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
    // Any wheel on sand loses its grip
    for (auto i = 0u; i < NUM_SUSP_LINES; i++) {
        if (m_aWheelRatios[i] < 1.0f && g_surfaceInfos.GetAdhesionGroup(m_aWheelColPoints[i].m_nSurfaceTypeB) == ADHESION_GROUP_SAND) {
            const auto up  = GetUp();
            const auto fwd = GetForward();

            auto vel       = m_vecMoveSpeed;
            vel -= up * vel.Dot(up); // Only the horizontal velocity is affected

            if (m_GasPedal > 0.3f) {
                if (vel.SquaredMagnitude() < sq(0.3f)) {
                    extraHandlingFlags += 4;
                }
                vel -= fwd * vel.Dot(fwd); // The wheels can't grip, so no forward push either
            }
            ApplyMoveForce(vel * -(CTimer::ms_fTimeStep * m_fMass * 0.02f));
            return;
        }
    }

    // Any wheel on rails
    for (auto i = 0u; i < NUM_SUSP_LINES; i++) {
        if (m_aWheelRatios[i] < 1.0f && m_aWheelColPoints[i].m_nSurfaceTypeB == SURFACE_RAILTRACK) {
            const auto up  = GetUp();

            const auto vel = m_vecMoveSpeed - up * m_vecMoveSpeed.Dot(up);
            ApplyMoveForce(vel * -(CTimer::ms_fTimeStep * m_fMass * ms_fRailTrackResistance));
            return;
        }
    }
}

// 0x6B7130
void CBike::PlayHornIfNecessary() {
    if (m_autoPilot.carCtrlFlags.bHonkAtCar || m_autoPilot.carCtrlFlags.bHonkAtPed) {
        PlayCarHorn();
    }
}

// 0x6B7150
void CBike::CalculateLeanMatrix() {
    if (m_bLeanMatrixCalculated) {
        return;
    }

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


// 0x6BF230
void CBike::GetCorrectedWorldDoorPosition(CVector& out, CVector arg1, CVector arg2) {
    const auto forward = GetForward();

    // Rebuild the bike's orientation from its forward vector
    const auto right       = forward.Cross(CVector{ 0.0f, 0.0f, 1.0f });
    const auto up          = right.Cross(forward);

    const auto rightUpSkew = right.Dot(GetUp());

    // The bounding box may stick out past the bounding sphere - Take that into account
    const auto cm       = GetColModel();
    const auto overhang = cm->m_boundSphere.m_fRadius < cm->m_boundBox.m_vecMin.x
        ? cm->m_boundBox.m_vecMin.x - cm->m_boundSphere.m_fRadius
        : 0.0f;

    out.Set(0.0f, 0.0f, 0.0f);
    out += forward * (arg2.y - arg1.y);
    out += right * (overhang * rightUpSkew + (arg2.x - arg1.x));
    out += up * (arg2.z - arg1.z);
    out += GetPosition();
}

// 0x6BEA10
void CBike::BlowUpCar(CEntity* damager, bool bHideExplosion) {
    if (!vehicleFlags.bCanBeDamaged) {
        return;
    }

    m_vecMoveSpeed.z += 0.13f;
    SetStatus(STATUS_WRECKED);
    physicalFlags.bRenderScorched = true;

    CVisibilityPlugins::SetClumpForAllAtomicsFlag(GetRpClump(), eAtomicComponentFlag::ATOMIC_PIPE_NO_EXTRA_PASSES);

    m_fHealth    = 0.0f;
    m_wBombTimer = 0;

    TheCamera.CamShake(0.4f, GetPosition());
    KillPedsInVehicle();

    m_nOverrideLights      = NO_CAR_LIGHT_OVERRIDE;
    vehicleFlags.bLightsOn = false;
    vehicleFlags.bEngineOn = false;
    ChangeLawEnforcerState(false);

    CExplosion::AddExplosion(this, damager, EXPLOSION_CAR, GetPosition(), 0, true, -1.0f, bHideExplosion);
    CDarkel::RegisterCarBlownUpByPlayer(*this, 0);
}

// 0x6B7050
void CBike::Fix() {
    vehicleFlags.bIsDamaged = false;
    bikeFlags.bEngineOnFire = false;
    m_nWheelStatus[0]       = 0;
    m_nWheelStatus[1]       = 0;
}

// 0x6BD090
void CBike::PreRender() {
    plugin::CallMethod<0x6BD090, CBike*>(this);
}

// 0x6BEEB0
void CBike::PlaceOnRoadProperly() {
    const auto cm = GetColModel();
    const auto startX = cm->m_pColData->m_pLines[0].m_vecStart.x;
    const auto endX   = -cm->m_pColData->m_pLines[0].m_vecStart.y;

    const auto& pos = GetPosition();

    auto frontCheck = pos + GetForward() * startX;
    frontCheck.z    = pos.z;

    auto rearCheck = pos - GetForward() * endX;
    rearCheck.z    = pos.z;

    CColPoint colPoint{};
    CEntity* colEntity = nullptr;

    bool foundFront = false;
    float frontZ;
    if (CWorld::ProcessVerticalLine(frontCheck, frontCheck.z + 5.0f, colPoint, colEntity, true)) {
        foundFront = true;
        frontZ     = colPoint.m_vecPoint.z;
        m_pEntityWeAreOn = colEntity;
        m_bTunnel = colEntity->m_bTunnel;
        m_bTunnelTransition = colEntity->m_bTunnelTransition;
    }
    if (CWorld::ProcessVerticalLine(frontCheck, frontCheck.z - 5.0f, colPoint, colEntity, true)) {
        if (!foundFront || std::fabs(frontCheck.z - colPoint.m_vecPoint.z) < std::fabs(frontCheck.z - frontZ)) {
            m_pEntityWeAreOn = colEntity;
            m_bTunnel = colEntity->m_bTunnel;
            m_bTunnelTransition = colEntity->m_bTunnelTransition;
            frontZ    = colPoint.m_vecPoint.z;
            foundFront = true;
        }
        m_FrontCollPoly.ligthing = colPoint.m_nLightingB;
        frontCheck.z = frontZ;
    } else if (foundFront) {
        m_FrontCollPoly.ligthing = colPoint.m_nLightingB;
        frontCheck.z = frontZ;
    }

    bool foundRear = false;
    float rearZ;
    if (CWorld::ProcessVerticalLine(rearCheck, rearCheck.z + 5.0f, colPoint, colEntity, true)) {
        foundRear = true;
        rearZ     = colPoint.m_vecPoint.z;
        m_pEntityWeAreOn = colEntity;
        m_bTunnel = colEntity->m_bTunnel;
        m_bTunnelTransition = colEntity->m_bTunnelTransition;
    }
    if (CWorld::ProcessVerticalLine(rearCheck, rearCheck.z - 5.0f, colPoint, colEntity, true)) {
        if (!foundRear || std::fabs(rearCheck.z - colPoint.m_vecPoint.z) < std::fabs(rearCheck.z - rearZ)) {
            m_pEntityWeAreOn = colEntity;
            m_bTunnel = colEntity->m_bTunnel;
            m_bTunnelTransition = colEntity->m_bTunnelTransition;
            rearZ     = colPoint.m_vecPoint.z;
            foundRear = true;
        }
        m_RearCollPoly.ligthing = colPoint.m_nLightingB;
        rearCheck.z = rearZ;
    } else if (foundRear) {
        m_RearCollPoly.ligthing = colPoint.m_nLightingB;
        rearCheck.z = rearZ;
    }

    const auto length = endX + startX;
    const auto pitch  = std::atan2(frontZ - rearZ, length);
    const auto cosPitch = std::cos(pitch);

    GetRight().Set((frontCheck.y - rearCheck.y) / length, -((frontCheck.x - rearCheck.x) / length), 0.0f);
    GetForward().Set(-cosPitch * GetRight().y, cosPitch * GetRight().x, 0.0f);
    GetForward().z = std::sin(pitch);
    GetUp() = CrossProduct(GetRight(), GetForward());

    const CVector newPos = (frontCheck * endX + rearCheck * startX) * (1.0f / length);
    const auto height = GetHeightAboveRoad() + newPos.z;
    if (m_matrix) {
        m_matrix->GetPosition().Set(newPos.x, newPos.y, height);
    } else {
        m_placement.m_vPosn.Set(newPos.x, newPos.y, height);
    }
}

// 0x6BDE20
void CBike::Render() {
    auto savedRef = 0;
    RwRenderStateGet(rwRENDERSTATEALPHATESTFUNCTIONREF, &savedRef);
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(1));

    m_nTimeTillWeNeedThisCar = CTimer::GetTimeInMS() + 3'000;
    CVehicle::Render();

    if (m_renderLights.m_bRightFront) {
        CalculateLeanMatrix();
        CVehicle::DoHeadLightBeam(DUMMY_LIGHT_FRONT_MAIN, m_mLeanMatrix, true);
    }

    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(savedRef));
}

// 0x6BCFC0
void CBike::Teleport(CVector destination, bool resetRotation) {
    CWorld::Remove(this);

    GetPosition() = destination;
    if (resetRotation) {
        SetOrientation(0.0f, 0.0f, 0.0f);
    }

    ResetMoveSpeed();
    ResetTurnSpeed();
    ResetSuspension();

    CWorld::Add(this);
}

// 0x6B8EC0
void CBike::VehicleDamage(float damageIntensity, eVehicleCollisionComponent component, CEntity* damager, CVector* vecCollisionCoors, CVector* vecCollisionDirection, eWeaponType weapon) {
    // Only called with `damageIntensity == 0` in which case the damage stored in `m_fDamageIntensity` is processed
    if (damageIntensity > 0.0f || m_fDamageIntensity < 1.0f || !vehicleFlags.bCanBeDamaged) {
        return;
    }

    damageIntensity = m_fDamageIntensity;

    // The player takes less damage once they've finished the game
    if (GetStatus() == STATUS_PLAYER && CStats::GetPercentageProgress() >= 100.0f) {
        damageIntensity *= 0.5f;
    }

    if (bikeFlags.bOnSideStand && damageIntensity > 20.0f) {
        bikeFlags.bOnSideStand = false;
    }

    DamageKnockOffRider(this, m_fDamageIntensity, m_nPieceType, m_pDamageEntity, m_vecLastCollisionPosn, m_vecLastCollisionImpactVelocity);

    if (m_pDamageEntity && m_pDamageEntity->GetIsTypeVehicle()) {
        m_nLastWeaponDamageType = WEAPON_RAMMEDBYCAR;
        m_pLastDamageEntity     = m_pDamageEntity;
        m_pDamageEntity->RegisterReference(&m_pLastDamageEntity);
    }

    if (physicalFlags.bCollisionProof) {
        return;
    }
    // Being rammed from below by a building (I.e.: A wall that was moved up) isn't considered a collision
    if (m_pDamageEntity && m_pDamageEntity->GetIsTypeBuilding() && m_vecLastCollisionImpactVelocity.Dot(GetUp()) <= 0.6f) {
        return;
    }

    if (damageIntensity > 25.0f && GetStatus() != STATUS_WRECKED) {
        // Rams by a police bike (Only if both are moving fast enough) raise the wanted level
        if (vehicleFlags.bIsLawEnforcer) {
            const auto playerVeh = FindPlayerVehicle();
            if (playerVeh && m_pDamageEntity == playerVeh && GetStatus() != STATUS_ABANDONED) {
                if (GetMoveSpeed().Magnitude() <= playerVeh->GetMoveSpeed().Magnitude() && playerVeh->GetMoveSpeed().Magnitude() > 0.1f) {
                    FindPlayerPed()->SetWantedLevelNoDrop(eWantedLevel::WANTED_LEVEL_1);
                }
            }
        }

        damageIntensity = (damageIntensity - 25.0f) * m_pHandlingData->m_fCollisionDamageMultiplier;
        if (damageIntensity > 0.0f) {
            // The other vehicle's driver complains about the crash
            if (damageIntensity > 5.0f
                && m_pDriver
                && m_pDamageEntity
                && m_pDamageEntity->GetIsTypeVehicle()
                && (m_pDamageEntity != FindPlayerVehicle() || m_pDamageEntity->AsVehicle()->m_nCreatedBy != MISSION_VEHICLE)
                && m_pDamageEntity->AsVehicle()->m_pDriver) {
                m_pDriver->Say(CTX_GLOBAL_CRASH_BIKE);
            }

            const auto prevHealth = m_fHealth;
            if (this == FindPlayerVehicle()) {
                damageIntensity *= vehicleFlags.bTakeLessDamage ? 1.0f / 6.0f : 0.5f;
            } else if (vehicleFlags.bTakeLessDamage) {
                damageIntensity *= 1.0f / 12.0f;
            } else if (m_pDamageEntity && m_pDamageEntity == FindPlayerVehicle()) {
                damageIntensity *= 2.0f / 3.0f;
            } else {
                damageIntensity *= 0.25f;
            }

            m_fHealth -= damageIntensity;
            if (m_fHealth <= 1.0f && prevHealth > 1.0f) { // Don't let it die from a single hit
                m_fHealth = 1.0f;
            }
        }
    }

    if (m_fHealth < 250.0f && !bikeFlags.bEngineOnFire) {
        bikeFlags.bEngineOnFire = true;
        m_BlowUpTimer           = 0;
        m_Damager               = m_pDamageEntity;
        if (m_Damager) {
            m_Damager->RegisterReference(&m_Damager);
        }
    }
}

// 0x6B89B0
void CBike::SetupSuspensionLines() {
    const auto mi = GetVehicleModelInfo();
    auto& cm      = *mi->GetColModel();
    auto& cd      = *cm.m_pColData;

    const auto& handling = *m_pHandlingData;

    const auto GetNodeWorldPos = [&](RwFrame* node, CVector& out) {
        RwMatrix matrix = *RwFrameGetMatrix(node);
        for (auto parent = RwFrameGetParent(node); parent && parent != m_aBikeNodes[BIKE_CHASSIS]; parent = RwFrameGetParent(parent)) {
            RwMatrixTransform(&matrix, RwFrameGetMatrix(parent), rwCOMBINEPOSTCONCAT);
        }
        out = *RwMatrixGetPos(&matrix);
    };

    const bool hasValidLines = cd.m_pLines[0].m_vecStart.x != 99999.99f && cd.m_pLines[0].m_vecStart.y != 99999.99f;
    for (auto i = 0; i < NUM_SUSP_LINES; i++) {
        auto& line = cd.m_pLines[i];

        CVector wheelPos;
        float height;
        if (!hasValidLines) {
            GetNodeWorldPos(i < 2 ? m_aBikeNodes[BIKE_WHEEL_FRONT] : m_aBikeNodes[BIKE_WHEEL_REAR], wheelPos);
            if (i == 0) {
                height = mi->m_fWheelSizeFront * 0.25f;
            } else if (i == 1) {
                height = mi->m_fWheelSizeFront * -0.25f;
            } else if (i == 2) {
                height = mi->m_fWheelSizeRear * 0.25f;
            } else {
                height = mi->m_fWheelSizeRear * -0.25f;
            }
        } else {
            wheelPos = i < 2 ? line.m_vecStart : line.m_vecEnd;
            height   = i < 2 ? m_fForkYOffset : m_fForkZOffset;
        }

        wheelPos.z += height;
        if (i == 0) {
            m_fForkYOffset = wheelPos.z;
        } else if (i == 2) {
            m_fForkZOffset = wheelPos.z;
            if (!m_aBikeNodes[BIKE_MISC_A]) {
                m_fSwingArmLength = 0.0f;
            } else {
                CVector miscPos;
                GetNodeWorldPos(m_aBikeNodes[BIKE_MISC_A], miscPos);
                const auto dx = wheelPos.x - miscPos.x;
                const auto dy = wheelPos.y - miscPos.y;
                m_fSwingArmLength = std::sqrt(dx * dx + dy * dy);
            }
        }
        wheelPos.z += handling.m_fSuspensionUpperLimit;

        line.m_vecStart = wheelPos;
        line.m_vecEnd   = CVector{ wheelPos.x, wheelPos.y, wheelPos.z + handling.m_fSuspensionLowerLimit - (i < 2 ? mi->m_fWheelSizeFront : mi->m_fWheelSizeRear) * 0.5f };

        m_fSuspensionLength[i] = handling.m_fSuspensionUpperLimit - handling.m_fSuspensionLowerLimit;
        m_fLineLength[i]       = line.m_vecStart.z - line.m_vecEnd.z;
    }

    if (!m_aBikeNodes[BIKE_MISC_A]) {
        CVector chassisPos;
        GetNodeWorldPos(m_aBikeNodes[BIKE_CHASSIS], chassisPos);
        m_fHeightAboveRoad   = chassisPos.z;
        m_fExtraTractionMult = chassisPos.z;
    }

    m_fHeightAboveRoad = mi->m_fWheelSizeFront * 0.5f - cd.m_pLines[0].m_vecStart.z
        + (1.0f - 1.0f / (handling.m_fSuspensionForceLevel * 4.0f)) * m_fSuspensionLength[0];

    for (auto i = 0; i < 2; i++) {
        m_aWheelSuspensionHeights[i] = (i == 0 ? mi->m_fWheelSizeFront : mi->m_fWheelSizeRear) * 0.5f - m_fHeightAboveRoad;
    }

    if (cd.m_pLines[0].m_vecEnd.z < cm.m_boundBox.m_vecMin.z) {
        cm.m_boundBox.m_vecMin.z = cd.m_pLines[0].m_vecEnd.z;
    }
    cm.m_boundSphere.m_fRadius = std::max({ cm.m_boundSphere.m_fRadius, cm.m_boundBox.m_vecMin.Magnitude(), cm.m_boundBox.m_vecMax.Magnitude() });

    if ((m_nHandlingFlagsIntValue & VEHICLE_HANDLING_STREET_RACER) && cd.m_pLines[0].m_vecStart.x == 99999.99f) {
        const auto clearance = 0.25f - m_fHeightAboveRoad;
        const auto numVerts  = *reinterpret_cast<const uint16*>(cd.m_pVertices);
        const auto verts     = reinterpret_cast<CVector*>(reinterpret_cast<uint16*>(cd.m_pVertices) + 8);
        for (auto i = 0; i < numVerts; i++) {
            if (verts[i].y - verts[i].z < clearance) {
                if (verts[i].z > 0.4f) {
                    verts[i].z = std::max(0.4f, verts[i].y - clearance);
                }
                verts[i].y = clearance + verts[i].z;
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
    if ((m_nAlarmState && m_nAlarmState != -1 && GetStatus() != STATUS_WRECKED) || m_HornCounter) {
        return;
    }

    if (m_nCarHornTimer) {
        m_nCarHornTimer--;
        return;
    }

    m_nCarHornTimer = (CGeneral::GetRandomNumber() & 0x7F) + 150;
    switch (m_nCarHornTimer & 7) {
    case 0:
    case 1:
        break;
    case 2:
    case 3:
        if (m_pDriver && m_autoPilot.carCtrlFlags.bHonkAtCar) {
            m_pDriver->Say(CTX_GLOBAL_BLOCKED);
        }
        break;
    default: // `4..7`
        if (!m_pDriver) {
            return;
        }
        m_pDriver->Say(CTX_GLOBAL_BLOCKED);
        return;
    }
    m_HornCounter = 45;
}

// 0x6B7070
void CBike::SetupDamageAfterLoad() {
    // NOP
}

// 0x6B6950
void CBike::DoBurstAndSoftGroundRatios() {
    const auto mi = GetVehicleModelInfo();

    // Wheels that aren't burst/aren't on rails (Only these can sink into soft ground)
    std::array<bool, NUM_SUSP_LINES> wheelIntact;
    rng::fill(wheelIntact, true);

    const auto fwdSpeed = std::abs(m_vecMoveSpeed.Dot(GetForward()));

    for (auto i = 0u; i < 2u; i++) {
        const auto wheelA = 2 * i; // Wheels come in pairs (Front, Rear) that share a `m_nWheelStatus`
        const auto wheelB = 2 * i + 1;

        switch (m_nWheelStatus[i]) {
        case WHEEL_STATUS_MISSING:
            m_aWheelRatios[wheelA] = 1.0f;
            m_aWheelRatios[wheelB] = 1.0f;
            break;
        case WHEEL_STATUS_BURST: {
            // NOTE: The original generates these two values, but never uses them
            (void)(CGeneral::GetRandomNumber());
            (void)((float)CGeneral::GetRandomNumber() + 0x62);
            if (CGeneral::GetRandomNumber() < 100) { // The rim burrows itself into the ground a bit
                const auto sinkAmount  = (m_fLineLength[wheelA] - m_fSuspensionLength[wheelA]) / m_fLineLength[wheelA] * 0.2f;
                m_aWheelRatios[wheelA] = std::min(1.0f, m_aWheelRatios[wheelA] + sinkAmount);
                m_aWheelRatios[wheelB] = std::min(1.0f, m_aWheelRatios[wheelB] + sinkAmount);
            }
            break;
        }
        default:
            if ((m_aWheelRatios[wheelA] < 1.0f && m_aWheelColPoints[wheelA].m_nSurfaceTypeB == SURFACE_RAILTRACK)
                || (m_aWheelRatios[wheelB] < 1.0f && m_aWheelColPoints[wheelB].m_nSurfaceTypeB == SURFACE_RAILTRACK)) {
                // Stepping over a rail compresses the suspension
                const auto wheelSize       = 1.5f / (mi->m_fWheelSizeFront * 0.5f); // NOTE: Uses the front size for both pairs, same as the original
                const auto pitchAngleScale = [&] {
                    auto scale = wheelSize;
                    if (fwdSpeed > 0.3f) {
                        scale *= fwdSpeed / 0.3f;
                    }
                    return 1.0f / scale;
                }();
                const auto prevAngle = [&] {
                    const auto angle = m_aWheelPitchAngles[i] * pitchAngleScale;
                    return angle - std::floor(angle);
                }();
                const auto currAngle = [&] {
                    const auto angle = (CTimer::ms_fTimeStep * m_aWheelAngularVelocity[i] + m_aWheelPitchAngles[i]) * pitchAngleScale;
                    return angle - std::floor(angle);
                }();
                if (m_aWheelAngularVelocity[i] > 0.0f ? currAngle < prevAngle : prevAngle < currAngle) {
                    const auto compression = (m_fLineLength[wheelA] - m_fSuspensionLength[wheelA]) / m_fLineLength[wheelA] * 0.3f;
                    m_aWheelRatios[wheelA] = std::max(m_aWheelRatios[wheelA] - compression, 0.2f);
                    m_aWheelRatios[wheelB] = std::max(m_aWheelRatios[wheelB] - compression, 0.2f);
                }
            } else {
                continue;
            }
        }
        wheelIntact[wheelA] = false;
        wheelIntact[wheelB] = false;
    }

    // Sink the intact wheels into soft ground
    for (auto i = 0u; i < NUM_SUSP_LINES; i++) {
        if (!wheelIntact[i]
            || m_aWheelRatios[i] >= 1.0f
            || g_surfaceInfos.GetAdhesionGroup(m_aWheelColPoints[i].m_nSurfaceTypeB) != ADHESION_GROUP_SAND
            || GetModelId() == MODEL_RHINO) {
            continue;
        }
        const auto sinkMult = m_nHandlingFlagsIntValue & VEHICLE_HANDLING_OFFROAD_ABILITY2 ? 0.1f
            : m_nHandlingFlagsIntValue & VEHICLE_HANDLING_OFFROAD_ABILITY                  ? 0.15f
                                                                                           : 0.25f;
        const auto sinkage  = std::max(0.4f, (1.0f - (fwdSpeed / 0.3f) * 0.7f) - CWeather::WetRoads * 0.7f);
        m_aWheelRatios[i]   = std::min(1.0f, ((m_fLineLength[i] - m_fSuspensionLength[i]) / m_fLineLength[i]) * sinkage * sinkMult + m_aWheelRatios[i]);
    }
}

// 0x6B67E0
bool CBike::SetUpWheelColModel(CColModel* wheelCol) {
    const auto mi               = GetVehicleModelInfo();
    const auto wcm              = GetColModel();
    const auto wcd              = wheelCol->m_pColData;

    wheelCol->m_boundBox        = wcm->m_boundBox;
    wheelCol->m_boundSphere     = wcm->m_boundSphere;

    const auto SetupWheelSphere = [&](CColSphere& sphere, eBikeNodes wheelNode, float wheelSize, uint8 pieceType) {
        // Get the wheel's position in world space by walking up the frame hierarchy (Up to the chassis)
        RwMatrix matrix = *RwFrameGetMatrix(m_aBikeNodes[wheelNode]);
        for (auto parent = RwFrameGetParent(m_aBikeNodes[wheelNode]); parent && parent != m_aBikeNodes[BIKE_CHASSIS]; parent = RwFrameGetParent(parent)) {
            RwMatrixTransform(&matrix, RwFrameGetMatrix(parent), rwCOMBINEPOSTCONCAT);
        }
        sphere.Set(wheelSize * 0.5f, CVector{ matrix.pos.x, matrix.pos.y, matrix.pos.z }, SURFACE_RUBBER, pieceType, tColLighting(0xFF));
    };

    SetupWheelSphere(wcd->m_pSpheres[0], BIKE_WHEEL_FRONT, mi->m_fWheelSizeFront, 0xD);
    SetupWheelSphere(wcd->m_pSpheres[1], BIKE_WHEEL_REAR, mi->m_fWheelSizeRear, 0xF);

    wcd->m_nNumSpheres = 2;
    return true;
}

// 0x6B67B0
void CBike::RemoveRefsToVehicle(CEntity* entityToRemove) {
    for (auto& entity : m_aGroundPhysicalPtrs) {
        if (entity == entityToRemove) {
            entity = nullptr;
        }
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
    if (IsComponentPresent(componentId)) {
        outPos = RwFrameGetLTM(m_aBikeNodes[componentId])->pos;
    } else {
        NOTSA_LOG_DEBUG("BikeNode missing: model={}, nodeIdx={}", m_nModelIndex, componentId);
    }
}

// 0x6B58D0
void CBike::ProcessOpenDoor(CPed* ped, uint32 doorComponentId, uint32 animGroup, uint32 animId, float fTime) {
    // NOP
}
