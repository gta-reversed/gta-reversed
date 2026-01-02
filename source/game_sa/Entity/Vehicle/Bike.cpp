/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Bike.h"

#include "Buoyancy.h"

const float BRAKE_TO_REVERSE_DURATION = 20.0f; // 0x871288

float BURST_GRIP_MULTIPLIER = 0.4f; // 0x87128C
float BIKE_LEAN_TO_PITCH_SCALE = -0.05f; // 0x871290
float BIKE_TUCK_TIME_START = 0.06f; // 0x871294
float BIKE_TUCK_TIME_END = 0.14f; // 0x871298

float ABS_MINFALLTHRESHOLD = 10.0f; // 0x87129C
float FALLTHRESHOLD = 75.0f; // 0x8712A0
float FALLTHRESHOLD_EASY = 20.0f; // 0x8712A4

float BIKE_FRONT_SCALE_MULT = 7.0f; // 0x8712A8
float BIKE_FRONT_MULT = 0.6f; // 0x8712AC
float BIKE_SIDE_MULT = 0.45f; // 0x8712B0
float BIKE_BOTTOM_MULT = 0.05f; // 0x8712B4
float BIKE_TOP_MULT = 1.5f; // 0x8712B8
float BIKE_TOP_UPSIDEDOWN_MULT = 5.0f; // 0x8712BC
float BIKE_FALL_DAMAGE_MULT = 0.05f; // 0x8712C0

float DIRTBIKE_FRONT_MULT = 0.65f; // 0x8712C4
float DIRTBIKE_BOTTOM_MULT = 0.75f; // 0x8712C8
float QUADBIKE_FRONT_MULT = 0.65f; // 0x8712CC
float QUADBIKE_BOTTOM_MULT = 0.75f; // 0x8712D0
float QUADBIKE_TOP_MULT = 2.0f; // 0x8712D4

float fDAxisX = 1.0f; // 0x8712D8
float fDAxisXExtra = 100.0f; // 0x8712DC
float fInAirXRes = 0.98f; // 0x8712E0
float fDAxisY = 1000.0f; // 0x8712E4

float fRiderStabiliseForce = -0.07f; // 0x8712E8
float fPickUpStabiliseForce = -0.1f; // 0x8712EC
float BIKE_BURNOUT_STEER_MULT = -0.0007f; // 0x8712F0

float fTestSkidMult1 = 0.7f; // 0x8712F4
float fTestSkidMult2 = 0.9f; // 0x8712F8

float BIKE_BALANCE_MOVESPEED_CAP = 0.1f; // 0x8712FC

float BIKE_STEER_SMOOTH_RATE = 0.2f; // 0x871300

float fBikeBurstForceMult = -0.0035f; // 0x871304
float fBikeBurstFallSpeed = 0.35f; // 0x871308
float fBikeBurstFallSpeedPlayer = 0.98f; // 0x87130C

float BIKE_RATIO_BURST_MULT = 0.2f; // 0x871310
float BIKE_RATIO_SINK_IN_SAND = 0.25f; // 0x871314
float BIKE_RATIO_SINK_IN_SAND_OA1 = 0.15f; // 0x871318
float BIKE_RATIO_SINK_IN_SAND_OA2 = 0.1f; // 0x87131C
float BIKE_RATIO_SAND_BOGDOWN_LIMIT = 0.3f; // 0x871320

float BIKE_RATIO_RAILTRACK_BUMP_MULT = 0.3f; // 0x871324
float BIKE_RATIO_RAILTRACK_BUMP_DIST = 1.5f; // 0x871328
float BIKE_RATIO_RAILTRACK_BUMP_DIST_SPEED = 0.3f; // 0x87132C

float BIKE_SAND_MOVERES_MULT = 0.02f; // 0x871330

float PED_BIKE_FOOTDOWN_SPEED = 0.02f; // 0x871334
float PED_BIKE_HEAD_WOBBLE = 6.0f; // 0x871338
float PED_BIKE_BRAKELEANFWD = 0.1f; // 0x87133C
float PED_BIKE_GASLEANBACK = -0.3f; // 0x871340
float PED_BIKE_WHEELIELEANFWD = 0.25f; // 0x871344

float PED_BIKE_SPINFALL_SPEED = 0.3f; // 0x871348

float PED_BIKE_LEANFWD_RATE = 0.89f; // 0x87134C
float PED_BIKE_LEANLEFT_RATE = 0.86f; // 0x871350

float PED_BIKE_LEANBLENDLIMIT = 0.56f; // 0x871354
float PED_BIKE_FWDBLENDOUTSTART = 0.3f; // 0x871358
float PED_BIKE_MINBLENDSCALE = 0.01f; // 0x87135C

//
float flt_C1C818 = 0.004f * 10.0f; // 0xC1C818
float flt_C1C81C = 0.277778f / 50.0f;  // 0xC1C81C

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
    RH_ScopedInstall(GetCorrectedWorldDoorPosition, 0x6BF230, { .reversed = false });

    RH_ScopedVMTInstall(Fix, 0x6B7050);
    RH_ScopedVMTInstall(BlowUpCar, 0x6BEA10, { .reversed = false });
    RH_ScopedVMTInstall(ProcessDrivingAnims, 0x6BF400);
    RH_ScopedVMTInstall(BurstTyre, 0x6BEB20, { .reversed = false });
    RH_ScopedVMTInstall(ProcessControlInputs, 0x6BE310, { .reversed = false });
    RH_ScopedVMTInstall(ProcessEntityCollision, 0x6BDEA0);
    RH_ScopedVMTInstall(Render, 0x6BDE20);
    RH_ScopedVMTInstall(PreRender, 0x6BD090, { .reversed = false });
    RH_ScopedVMTInstall(Teleport, 0x6BCFC0);
    RH_ScopedVMTInstall(ProcessControl, 0x6B9250, { .reversed = false });
    RH_ScopedVMTInstall(VehicleDamage, 0x6B8EC0, { .reversed = false });
    RH_ScopedVMTInstall(SetupSuspensionLines, 0x6B89B0, { .reversed = false });
    RH_ScopedVMTInstall(SetModelIndex, 0x6B8970);
    RH_ScopedVMTInstall(PlayCarHorn, 0x6B7080);
    RH_ScopedVMTInstall(SetupDamageAfterLoad, 0x6B7070);
    RH_ScopedVMTInstall(DoBurstAndSoftGroundRatios, 0x6B6950, { .reversed = false });
    RH_ScopedVMTInstall(SetUpWheelColModel, 0x6B67E0);
    RH_ScopedVMTInstall(RemoveRefsToVehicle, 0x6B67B0);
    RH_ScopedVMTInstall(ProcessControlCollisionCheck, 0x6B6620);
    RH_ScopedVMTInstall(GetComponentWorldPosition, 0x6B5990);
    RH_ScopedVMTInstall(ProcessOpenDoor, 0x6B58D0);
}

// 0x6BF430
CBike::CBike(int32 modelIndex, eVehicleCreatedBy createdBy) : CVehicle(createdBy) {
    auto mi = CModelInfo::GetModelInfo(modelIndex)->AsVehicleModelInfoPtr();
    if (mi->IsBike()) {
        const auto& animationStyle = CAnimManager::GetAnimBlocks()[mi->GetAnimFileIndex()].GroupId;
        m_RideAnimData.AnimGroup = animationStyle;
        if (animationStyle < ANIM_GROUP_BIKES || animationStyle > ANIM_GROUP_WAYFARER) {
            m_RideAnimData.AnimGroup = ANIM_GROUP_BIKES;
        }
    }

    m_nVehicleSubType = m_nVehicleType = VEHICLE_TYPE_BIKE;
    m_BlowUpTimer = 0.0f;
    m_nBrakesOn = false;

    nBikeFlags = 0;

    SetModelIndex(modelIndex);

    m_pHandlingData = gHandlingDataMgr.GetVehiclePointer(mi->m_nHandlingId);
    m_BikeHandling = gHandlingDataMgr.GetBikeHandlingPointer(mi->m_nHandlingId);
    m_nHandlingFlagsIntValue = m_pHandlingData->m_nHandlingFlags;
    m_pFlyingHandlingData = gHandlingDataMgr.GetFlyingPointer(static_cast<uint8>(mi->m_nHandlingId));
    m_fBrakeCount = BRAKE_TO_REVERSE_DURATION;
    mi->ChooseVehicleColour(m_nPrimaryColor, m_nSecondaryColor, m_nTertiaryColor, m_nQuaternaryColor, 1);
    m_fSwingArmLength = 0.0f;

    m_fForkYOffset = 0.0f;
    m_fForkZOffset = 0.0f;

    m_fSteerAngleTan = std::tan(DegreesToRadians(mi->m_fBikeSteerAngle));

    m_nFixLeftHand = false;
    m_nFixRightHand = false;

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

    for (auto i = 0; i < NUM_SUSP_LINES; ++i) {
        m_aWheelColPoints[i] = {};
        m_aWheelRatios[i] = BILLS_EXTENSION_LIMIT;
        m_aRatioHistory[i] = 0.0f;
        m_WheelCounts[i] = 0.0f;
        m_fSuspensionLength[i] = 0.0f;
        m_fLineLength[i] = 0.0f;
        m_aGroundPhysicalPtrs[i] = nullptr;
        m_aGroundOffsets[i].Reset();
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
    m_vecOldSpeedForPlayback.Reset();
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
bool CBike::DamageKnockOffRider(CVehicle* pVehicle, float fImpulse, uint16 nPieceType, CEntity* pDamageEntity, CVector& vecDamagePos, CVector& vecDamageNormal) {
    const auto driver = pVehicle->m_pDriver;
    auto v32 = fImpulse / pVehicle->m_fMass * 800.0f;
    if (pVehicle->GetStatus()) {
        if (driver && (driver->bDoBloodyFootprints == true && driver->bGetUpAnimStarted == false)) {
            v32 = (1.0f - driver->GetBikeRidingSkill() * 0.6f) * v32;
        }
    } else {
        v32 = v32 * 0.75f;
        if (driver) {
            v32 = (1.0f - driver->GetBikeRidingSkill() * 0.5f) * v32;
        }
    }

    if (!driver) {
        return false;
    }

    if (driver->IsStateDriving()) {
        return false;
    }

    if (v32 <= 10.0f) {
        return false;
    }

    if (driver->m_pIntelligence->m_TaskMgr.GetActiveTask()) {
        const auto ActiveTask = pVehicle->m_pDriver->m_pIntelligence->m_TaskMgr.GetActiveTask();
        if (ActiveTask->GetTaskType() == TASK_SIMPLE_GANG_DRIVEBY
            && pVehicle->m_pDriver->IsCop()) {
            return false;
        }
    }

    const auto matrix = pVehicle->m_matrix;
    float v34, v11;
    if (fabs(matrix->GetForward().Dot(vecDamageNormal)) <= 0.85f) {
        v34 = 0.6f;
    } else {
        v11 = (vecDamageNormal.y + vecDamageNormal.x) * 0.0f + vecDamageNormal.z;
        if (v11 < 0.0f || v11 < 0.85f) {
            v11 = 0.0f;
        }
        v34 = 7.0f * v11 * v11 + 0.6f;
    }

    float a2a = 1.5f;
    if (matrix->GetUp().z < 0.0f) {
        v34 = 5.0f;
    }

    float v35 = 0.05f;
    float v13;

    if (pVehicle->m_nModelIndex == MODEL_SATCHEL) {
        v34 = 0.65f * v34;
        v13 = 0.75f;
    } else if (!pVehicle->IsSubQuad()) {
        a2a = 2.0f * 1.5f;
        v34 = 0.65f * v34;
        v13 = 0.75f;
    }

    v35 = v13 * 0.05f;
    if (driver) {
        if (matrix->GetForward().Dot(vecDamageNormal) > 0.0f) {
            v34 = (1.0f - driver->GetBikeRidingSkill() * 0.6f) * v34;
        }
    }

    float v16;
    if (matrix->GetUp().Dot(vecDamageNormal) >= 0.0f) {
        v16 = matrix->GetUp().Dot(vecDamageNormal);
    } else {
        v16 = 0.0f;
    }

    float v17;
    if (matrix->GetUp().Dot(vecDamageNormal) <= 0.0f) {
        v17 = matrix->GetUp().Dot(vecDamageNormal);
    } else {
        v17 = 0.0f;
    }

    const auto v18 = matrix->GetForward().Dot(vecDamageNormal);
    const auto v19 = matrix->GetRight().Dot(vecDamageNormal);
    const auto v21 = pVehicle->m_pDriver;
    const auto v37 = fabs(v19);
    auto v33 = (fabs(v18) * v34 + v16 * v35 + v37 * 0.45f - v17 * a2a) * v32;

    if (v21->IsPlayer() && CCullZones::CamStairsForPlayer()) {
        if (CCullZones::FindZoneWithStairsAttributeForPlayer()) {
            v33 = 0.0f;
        }
    }

    if (v33 <= 75.0f && (pVehicle->m_pDriver->bCanExitCar || v33 <= 20.0f)) {
        return false;
    }
    if (driver) {
        if (driver->bCanExitCar) {
            return false;
        }
    }

    const auto v23 = pVehicle->m_apPassengers[0];
    if (v23) {
        if (v23->bCanExitCar) {
            return false;
        }
    }

    auto LocalDirection = -10;
    if (driver) {
        LocalDirection = driver->GetLocalDirection(-vecDamageNormal);
        const auto a6 = 0.05f * v33;
        CEventKnockOffBike event(pVehicle, pVehicle->m_vecMoveSpeed, vecDamageNormal, fImpulse, a6, 0x31u, LocalDirection, 0, nullptr, true, false);
        pVehicle->m_pDriver->GetEventGroup().Add(event, false);
    }

    if (v23) {
        if (LocalDirection == -10) {
            LocalDirection = v23->GetLocalDirection(-vecDamageNormal);
        }
        const auto a6a = 0.05f * v33;
        CEventKnockOffBike event(pVehicle, pVehicle->m_vecMoveSpeed, vecDamageNormal, fImpulse, a6a, 0x31u, LocalDirection, 0, nullptr, false, false);
        pVehicle->m_apPassengers[0]->GetEventGroup().Add(event, false);
    }
    return true;
}

// dummy function
// 0x6B5F40
CPed* CBike::KnockOffRider(eWeaponType hitType, uint8 localDirn, CPed* ped, bool beingJacked) {
    return ped;
}

// 0x6B5F50
void CBike::SetRemoveAnimFlags(CPed* ped) {
    if (ped->GetIsTypePed()) {
        for (CAnimBlendAssociation* i = RpAnimBlendClumpGetFirstAssociation(ped->m_pRwClump, ANIMATION_SECONDARY_TASK_ANIM); i; i = RpAnimBlendGetNextAssociation(i, ANIMATION_SECONDARY_TASK_ANIM)) {
            i->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE);
        }
    }
}

// 0x6B5F90
void CBike::ReduceHornCounter() {
    if (m_HornCounter) {
        m_HornCounter--;
    }
}

// 0x6B5FB0
void CBike::ProcessBuoyancy() {
    CVector centreOfBuoyancy;
    CVector buoyancyForce;
    if (mod_Buoyancy.ProcessBuoyancy(this, m_fBuoyancyConstant, &centreOfBuoyancy, &buoyancyForce)) {
        physicalFlags.bTouchingWater = true;
        ApplyMoveForce(buoyancyForce);
        ApplyTurnForce(buoyancyForce, centreOfBuoyancy);

        auto fTimeStep = std::max(0.01F, CTimer::GetTimeStep());
        auto fUsedMass = m_fMass * GAME_GRAVITY;
        buoyancyForce.z /= fTimeStep * fUsedMass;

        if (fUsedMass > m_fBuoyancyConstant) {
            buoyancyForce.z *= 1.05F * fUsedMass / m_fBuoyancyConstant;
        }

        if (physicalFlags.bMakeMassTwiceAsBig) {
            buoyancyForce.z *= 1.5F;
        }

        auto fBuoyancyForceMult = std::max(0.5F, 1.0F - buoyancyForce.z * 0.05F);
        auto fSpeedMult = std::pow(fBuoyancyForceMult, CTimer::GetTimeStep());
        m_vecMoveSpeed *= fSpeedMult;
        m_vecTurnSpeed *= fSpeedMult;

        // 0x6B6443
        if (buoyancyForce.z > 0.8F || (buoyancyForce.z > 0.4F && IsAnyWheelNotMakingContactWithGround())) {
            vehicleFlags.bIsDrowning = true;
            physicalFlags.bSubmergedInWater = true;

            m_vecMoveSpeed.z = std::max(-0.1F, m_vecMoveSpeed.z);

            if (m_pDriver) {
                ProcessPedInVehicleBuoyancy(m_pDriver->AsPed(), true);
            } else {
                vehicleFlags.bEngineOn = false;
            }

            for (const auto passenger : GetPassengers()) {
                ProcessPedInVehicleBuoyancy(passenger, false);
            }
        } else {
            vehicleFlags.bIsDrowning = false;
            physicalFlags.bSubmergedInWater = false;
        }
    } else {
        vehicleFlags.bIsDrowning = false;
        physicalFlags.bSubmergedInWater = false;
        physicalFlags.bTouchingWater = false;
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
            auto damageEvent = CEventDamage(this, CTimer::GetTimeInMS(), eWeaponType::WEAPON_DROWNING, PED_PIECE_TORSO, 0, false, true);
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
bool CBike::ProcessAI(uint32& nProcContFlags) {
    /*
    v3 = CModelInfo::ms_modelInfoPtrs[m_nModelIndex];
    m_vehicleRecordingId = m_autoPilot.m_vehicleRecordingId;
    m_autoPilot.m_nCarCtrlFlags &= 0xFCu;
    if (m_vehicleRecordingId >= 0 && !CVehicleRecording::bUseCarAI[m_vehicleRecordingId]) {
        nProcContFlags += 2;
        return false;
    }

    switch (m_nType >> 3) {
    case 0:
        nProcContFlags += 2;
        bikeFlags &= ~8u;
        if (FindPlayerPed(-1)->m_nPedState == PED_EXIT_CAR || FindPlayerPed(-1)->m_nPedState == PED_DRAGGED_FROM_CAR) {
            goto LABEL_24;
        }
        m_Driver = m_pDriver;
        if (m_Driver) {
            if (CWorld::Players[0].m_pPed == m_Driver) {
                ProcessControlInputs(0);
            } else if (CWorld::Players[1].m_Ped == m_Driver) {
                ProcessControlInputs(1);
            }
        }
        m_Handling = m_Handling;
        m_BikeHandling = m_BikeHandling;
        if (m_RideAnimData.dword10 >= 0.0) {
            m_vecCOM.y = m_BikeHandling->m_fLeanFwdCOM * m_RideAnimData.dword10
                + m_Handling->m_vecCentreOfMass.y;
            if (m_fBreakPedal < 0.0 || !m_nNoOfContactWheels) {
                ba = CVector::Magnitude(&m_vecMoveSpeed);
                a2d = min(0.1, ba);
                a = a2d / 0.1;
                a2e = (max(a, m_fBreakPedal) + m_fBreakPedal)
                    * (m_BikeHandling->m_fLeanFwdForce
                       * m_fTurnMass
                       * m_RideAnimData.dword10
                       * a2d)
                    * 0.5;
                FatAndMuscleModifier = CStats::GetFatAndMuscleModifier(11);
                m_matrix = m_matrix;
                a2a = FatAndMuscleModifier * a2e;
                v12 = VectorAdd(&force, &m_vecCOM, &m_matrix->m_forward);
                p_m_up = &m_matrix->m_up;
                p_out = &v47;
                goto LABEL_20;
            }
        } else {
            m_vecCOM.y = m_BikeHandling->m_fLeanBakCOM * m_RideAnimData.dword10
                + m_Handling->m_vecCentreOfMass.y;
            if (m_fBreakPedal == 0.0 && (m_nFlags1 & 0x20) == 0 || !m_nNoOfContactWheels) {
                b = CVector::Magnitude(&m_vecMoveSpeed);
                a2b = min(0.1, b);
                v35 = a2b / 0.1;
                a2c = (max(v35, m_fGasPedal) + m_fGasPedal)
                    * (m_BikeHandling->m_fLeanBakForce
                       * m_fTurnMass
                       * m_RideAnimData.dword10
                       * a2b)
                    * 0.5;
                v10 = CStats::GetFatAndMuscleModifier(11);
                v11 = m_matrix;
                a2a = v10 * a2c;
                v12 = VectorAdd(&a1, &m_vecCOM, &v11->m_forward);
                p_m_up = &v11->m_up;
                p_out = &out;
LABEL_20:
                v16 = v12;
                aa = -(CTimer::ms_fTimeStep * a2a);
                v17 = CVector::Scale2(p_out, aa, p_m_up);
                CPhysical::ApplyTurnForce(this, *v17, *v16);
            }
        }
        CEntity::PruneReferences();
        if ((m_nType & 0xF8) == 0) {
            CVehicle::DoDriveByShootings();
        }
        CBike::DoSoftGroundResistance(nProcContFlags);
LABEL_24:
        Pad = CPad::GetPad(0);
        if (!CPad::CarGunJustDown(Pad)) {
            return 1;
        }
        CVehicle::ActivateBomb();
        return 1;
    case 1:
        *nProcContFlags += 2;
        return 1;
    case 2:
        CCarAI::UpdateCarAI(this);
        CPhysical::ProcessControl();
        CCarCtrl::UpdateCarOnRails(this);
        v19 = m_autoPilot.m_speed * 0.02;
        m_nDriveWheelsOnGroundLastFrame = m_nDriveWheelsOnGround;
        m_nNoOfContactWheels = 2;
        v20 = m_Handling;
        ab = v19;
        m_nDriveWheelsOnGround = 2;
        cTransmission::CalculateGearForSimpleCar(&v20->m_transmissionData, ab, &m_nCurrentGear);
        a2f = CTimer::ms_fTimeStep;
        bb = *&v3[2].__vftable * 0.5;
        v21 = CVehicle::ProcessWheelRotation(
            this,
            WHEEL_STATE_NORMAL,
            &m_matrix->m_forward,
            &m_vecMoveSpeed,
            bb
        );
        p_m_forward = &m_matrix->m_forward;
        m_aWheelPitchAngles[0] = v21 * a2f + m_aWheelPitchAngles[0];
        a2g = CTimer::ms_fTimeStep;
        bc = *&v3[2].m_nKey * 0.5;
        v23 = CVehicle::ProcessWheelRotation(this, WHEEL_STATE_NORMAL, p_m_forward, &m_vecMoveSpeed, bc);
        v24 = (m_autoPilot.m_nCarCtrlFlags & 3) == 0;
        m_aWheelPitchAngles[1] = v23 * a2g + m_aWheelPitchAngles[1];
        if (!v24) {
            PlayCarHorn(this);
        }
        m_nHornCounter = m_nHornCounter;
        if (m_nHornCounter) {
            m_nHornCounter = m_nHornCounter - 1;
        }
        v26 = m_nFlags4 & 0xDE;
        bikeFlags &= ~0x80u;
        m_nFlags4 = v26;
        return 1;
    case 3:
    case 0xC:
        CCarAI::UpdateCarAI(this);
        CCarCtrl::SteerAICarWithPhysics(v3, this);
        if ((m_autoPilot.m_nCarCtrlFlags & 3) != 0) {
            PlayCarHorn(this);
        }
        *nProcContFlags += 2;
        m_nFlags3 = m_nFlags3;
        v28 = bikeFlags & 0x7F;
        bikeFlags = v28;
        if ((m_nFlags3 & 8) != 0) {
            m_nFlags1 |= 0x20u;
            m_fGasPedal = 0.0;
            m_fBreakPedal = 1.0;
        } else {
            bikeFlags = v28 & 0xF7;
        }
        return 1;
    case 4:
        m_fBreakPedal = 0.0;
        if (CVector::SquaredMag(&m_vecMoveSpeed) < 0.0099999998 || (bikeFlags & 0x10) != 0) {
            v29 = m_nFlags1 | 0x20;
        } else {
            v29 = m_nFlags1 & 0xDF;
        }
        m_nFlags1 = v29;
        v24 = m_Driver == 0;
        m_fGasPedal = 0.0;
        m_nHornCounter = 0;
        if ((!v24 || m_Passengers[0] || (m_nFlags3 & 8) != 0) && (bikeFlags & 0x10) == 0) {
            *nProcContFlags += 2;
        }
        v30 = m_nFlags3;
        v31 = bikeFlags & 0x7F;
        m_RideAnimData.m_fHandlebarsAngle = 0.0;
        m_RideAnimData.m_fAnimPercentageState = 0.0;
        bikeFlags = v31;
        if ((v30 & 8) == 0) {
            return 1;
        }
        m_nFlags1 |= 0x20u;
        m_fGasPedal = 0.0;
        m_fBreakPedal = 1.0;
        return 1;
    case 5:
        v33 = m_nFlags1 | 0x20;
        v34 = bikeFlags & 0x7F;
        m_fBreakPedal = 0.050000001;
        m_nFlags1 = v33;
        m_fSteerAngle = 0.0;
        m_fGasPedal = 0.0;
        m_nHornCounter = 0;
        bikeFlags = v34;
        m_RideAnimData.m_fHandlebarsAngle = 0.0;
        m_RideAnimData.m_fAnimPercentageState = 0.0;
        return 1;
    case 9:
        if (CVector::SquaredMag(&m_vecMoveSpeed) >= 0.0099999998) {
            v32 = m_nFlags1 & 0xDF;
            m_fBreakPedal = 0.0;
            m_nFlags1 = v32;
        } else {
            m_nFlags1 |= 0x20u;
            m_fBreakPedal = 1.0;
        }
        m_fSteerAngle = 0.0;
        m_fGasPedal = 0.0;
        m_nHornCounter = 0;
        *nProcContFlags += 2;
        bikeFlags &= ~0x80u;
        return 1;
    default:
        return 1;
    }
    */
    return 0;
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
    return plugin::CallMethodAndReturn<bool, 0x6BEB20, CBike*, uint8, bool>(this, tyreComponentId, bPhysicalEffect);
}

// 0x6BE310
void CBike::ProcessControlInputs(uint8 playerNum) {
    static float flt_C1C804; // 0xC1C804, or not static
    plugin::CallMethod<0x6BE310, CBike*, uint8>(this, playerNum);
}

// 0x6BDEA0
int32 CBike::ProcessEntityCollision(CEntity* entity, CColPoint* outColPoints) {
    static std::array<CColPoint, 32> pedEntityColPts; // 0xC1C280
    static int32                     isInitColPtr;    // 0xC1C800

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
        GetMatrix(), *GetColModel(), entity->GetMatrix(), *entity->GetColModel(), *(std::array<CColPoint, 32>*)(outColPoints), m_aWheelColPoints, m_aWheelRatios, false
    );

    // Possibly add driver & entity collisions to `outColPoints`
    if (m_pDriver && m_nTestPedCollision) {
        if (!isInitColPtr) {
            pedEntityColPts = {};
            isInitColPtr ^= true;
        }

        const auto pcd = m_pDriver->GetColData();
        if (!pcd->m_nNumLines) {
            std::array<CColPoint, 32> pedCPs{};

            CMatrix driverMat = GetMatrix();
            driverMat.GetPosition() += GetDriverSeatDummyPositionWS();

            const auto numPedEntityColPts = CCollision::ProcessColModels(
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
            const auto& cp = m_aWheelColPoints[i];

            const auto wheelColPtsTouchDist = m_aWheelRatios[i];
            if (wheelColPtsTouchDist >= BILLS_EXTENSION_LIMIT || wheelColPtsTouchDist >= ogWheelRatios[i]) {
                continue;
            }

            numProcessedLines++;

            m_anCollisionLighting[i] = cp.m_nLightingB;
            m_nContactSurface = cp.m_nSurfaceTypeB;

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
    static float WheelState[4]; // 0xC1C26C + 0xC1C270
    static float fThrust;       // 0xC1C27C

    plugin::CallMethod<0x6B9250, CBike*>(this);
}

// 0x6B6740
void CBike::ResetSuspension() {
    for (auto i = 0; i < 2; ++i) {
        m_aWheelPitchAngles[i] = 0.0f;
        m_WheelStates[i] = WHEEL_STATE_NORMAL;
    }

    for (auto i = 0; i < NUM_SUSP_LINES; ++i) {
        m_aWheelRatios[i] = BILLS_EXTENSION_LIMIT;
        m_WheelCounts[i] = 0.0f;
    }
}

// 0x6B6790
bool CBike::GetAllWheelsOffGround() const {
    return m_nNoOfContactWheels == 0;
}

// 0x6B67A0
void CBike::DebugCode() {
    // CVehicleModelInfo* pModelInfo; // unused
    
    // NOP
}

// 0x6B6D40
void CBike::DoSoftGroundResistance(uint32& nProcContFlags) {
    const auto isSoftSurface = [this](size_t wheelIdx) {
        return m_aWheelRatios[wheelIdx] < BILLS_EXTENSION_LIMIT && g_surfaceInfos.GetAdhesionGroup(m_aWheelColPoints[wheelIdx].m_nSurfaceTypeB) == 4;
    };
    const auto isRailTrack = [this](size_t wheelIdx) {
        return m_aWheelRatios[wheelIdx] < BILLS_EXTENSION_LIMIT && m_aWheelColPoints[wheelIdx].m_nSurfaceTypeB == SURFACE_RAILTRACK;
    };

    // soft surface test
    if (std::ranges::any_of(std::views::iota(0, 4), isSoftSurface)) {
        const auto& up = m_matrix->GetUp();
        const auto& fwd = m_matrix->GetForward();

        const auto vUpProj = up * m_vecMoveSpeed.Dot(up);
        auto lateral = m_vecMoveSpeed - vUpProj;

        if (m_GasPedal > 0.3f) {
            if (sq(BIKE_RATIO_SAND_BOGDOWN_LIMIT) > lateral.SquaredMagnitude()) {
                nProcContFlags |= 4u;
            }
            const auto vFwdProj = fwd * lateral.Dot(fwd);
            lateral -= vFwdProj;
        }

        const float factor = -(CTimer::GetTimeStep() * m_fMass * BIKE_SAND_MOVERES_MULT);
        CPhysical::ApplyMoveForce(lateral * factor);

        // railway track inspection
    } else if (std::ranges::any_of(std::views::iota(0, 4), isRailTrack)) {
        const auto& up = m_matrix->GetUp();
        const auto vUpProj = up * m_vecMoveSpeed.Dot(up);
        const auto lateral = m_vecMoveSpeed - vUpProj;

        const float factor = -(CTimer::GetTimeStep() * m_fMass * CVehicle::ms_fRailTrackResistance);
        CPhysical::ApplyMoveForce(lateral * factor);
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
    if (!m_bLeanMatrixCalculated) {
        CMatrix mat;
        mat.SetRotateX(fabs(m_RideAnimData.LeanAngle) * -0.05f);
        mat.RotateY(m_RideAnimData.LeanAngle);
        m_mLeanMatrix = GetMatrix();
        m_mLeanMatrix = m_mLeanMatrix * mat;
        // place wheel back on ground
        m_mLeanMatrix.GetPosition() += GetUp() * (1.0f - cos(m_RideAnimData.LeanAngle)) * GetColModel()->GetBoundingBox().m_vecMin.z;
        m_bLeanMatrixCalculated = true;
    }
}

// 0x6B7F90
void CBike::FixHandsToBars(CPed* rider) {
    ((void(__thiscall*)(CBike*, CPed*))0x6B7F90)(this, rider);
}

// 0x6BEEB0
/**
 * Places the bike properly on the road surface by detecting collision points
 * and adjusting the bike's orientation and position accordingly.
 */
void CBike::PlaceOnRoadProperly() {
    CColModel* colModel = GetColModel();
    CMatrix*   matrix   = &GetMatrix();

    // Get bike bounding box dimensions
    float frontY     = colModel->m_boundBox.m_vecMax.y;
    float rearY      = colModel->m_boundBox.m_vecMin.y;
    float bikeLength = frontY - rearY;

    // Get current position
    CVector currentPos = GetPosition();

    // Calculate front and rear wheel positions
    CVector frontWheelPos, rearWheelPos;

    if (matrix) {
        // Front wheel position
        frontWheelPos.x = currentPos.x + GetForward().x * frontY;
        frontWheelPos.y = currentPos.y + GetForward().y * frontY;
        frontWheelPos.z = currentPos.z + 5.0f;

        // Rear wheel position
        rearWheelPos.x = currentPos.x + GetForward().x * rearY;
        rearWheelPos.y = currentPos.y + GetForward().y * rearY;
        rearWheelPos.z = currentPos.z + 5.0f;
    }

    // Raycast from front wheel down to find ground
    CColPoint frontCollision;
    CEntity*  frontEntity = nullptr;
    bool      frontHit    = CWorld::ProcessVerticalLine(
        frontWheelPos,
        currentPos.z - 5.0f,
        frontCollision,
        frontEntity,
        true,
        false,
        false,
        false,
        false,
        false,
        nullptr
    );

    /**
    * Helper function to copy relevant entity flags
    */
    const auto CopyEntityFlagsFrom = [&](CEntity* entity) {
        if (!entity) {
            return;
        }

        m_bTunnel = entity->m_bTunnel;
        m_bTunnelTransition = entity->m_bTunnelTransition;
    };

    float frontGroundZ;
    if (frontHit) {
        // Update collision info
        m_FrontCollPoly.lighting = frontCollision.m_nLightingB;
        m_pEntityWeAreOn              = frontEntity;

        // Copy entity flags
        CopyEntityFlagsFrom(frontEntity);

        frontGroundZ = frontCollision.m_vecPoint.z;
    } else {
        frontGroundZ = currentPos.z;
    }

    // Raycast from rear wheel down to find ground
    CColPoint rearCollision;
    CEntity*  rearEntity = nullptr;
    bool      rearHit    = CWorld::ProcessVerticalLine(
        rearWheelPos,
        currentPos.z - 5.0f,
        rearCollision,
        rearEntity,
        true,
        false,
        false,
        false,
        false,
        false,
        nullptr
    );

    float rearGroundZ;
    if (rearHit) {
        // Update collision info
        m_RearCollPoly.lighting = rearCollision.m_nLightingB;
        m_pEntityWeAreOn              = rearEntity;

        // Copy entity flags
        CopyEntityFlagsFrom(rearEntity);

        rearGroundZ = rearCollision.m_vecPoint.z;
    } else {
        rearGroundZ = currentPos.z;
    }

    // Calculate elevation angle and orientation
    float heightDiff     = frontGroundZ - rearGroundZ;
    float elevationAngle = atan2f(heightDiff, bikeLength);
    float cosElevation   = cosf(elevationAngle);
    float sinElevation   = sinf(elevationAngle);

    // Calculate new right vector (perpendicular to bike direction in XY plane)
    CVector right;
    right.x = (frontWheelPos.y - rearWheelPos.y) / bikeLength;
    right.y = -(frontWheelPos.x - rearWheelPos.x) / bikeLength;
    right.z = 0.0f;

    // Calculate forward vector accounting for elevation
    CVector forward;
    forward.x = -cosElevation * right.y;
    forward.y = cosElevation * right.x;
    forward.z = sinElevation;

    // Calculate up vector (cross product of forward and right)
    CVector up = right.Cross(forward);

    // Update bike's transformation matrix
    if (matrix) {
        // Set orientation vectors
        matrix->GetRight()   = right;
        matrix->GetForward() = forward;
        matrix->GetUp()      = up;

        // Calculate and set new position (midpoint between wheels)
        CVector newPos;
        newPos.x      = (frontWheelPos.x + rearWheelPos.x) * 0.5f;
        newPos.y      = (frontWheelPos.y + rearWheelPos.y) * 0.5f;
        newPos.z      = (frontGroundZ + rearGroundZ) * 0.5f;

        matrix->GetPosition() = newPos;
    } else {
        // Fallback if no matrix exists
        GetPosition().x = (frontWheelPos.x + rearWheelPos.x) * 0.5f;
        GetPosition().y = (frontWheelPos.y + rearWheelPos.y) * 0.5f;
        GetPosition().z = (frontGroundZ + rearGroundZ) * 0.5f;
    }

    // Update bounding sphere and related data
    UpdateRwFrame();
}


// 0x6BF230
void CBike::GetCorrectedWorldDoorPosition(CVector& out, CVector arg1, CVector arg2) {
    ((void(__thiscall*)(CBike*, CVector&, CVector, CVector))0x6BF230)(this, out, arg1, arg2);
}

// 0x6BEA10
void CBike::BlowUpCar(CEntity* pCulprit, bool bInACutscene) {
    plugin::CallMethod<0x6BEA10, CBike*, CEntity*, uint8>(this, pCulprit, bInACutscene);
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
void CBike::VehicleDamage(float impulse, eVehicleCollisionComponent pieceType, CEntity* damageEntity, CVector* damagePos, CVector* damageNormal, eWeaponType weaponType) {
    /*
    if (impulse > 0.0f || m_fDamageIntensity < 1.0f || !vehicleFlags.bCanBeDamaged) {
        return;
    }

    float damageIntensity = m_fDamageIntensity;
    const auto status = GetStatus();
    if (status == STATUS_PLAYER && CStats::GetPercentageProgress() >= 100.0f) {
        damageIntensity *= 0.5f;
    }

    if (bikeFlags.bOnSideStand && damageIntensity > 20.0f) {
        bikeFlags.bOnSideStand = false;
    }

    DamageKnockOffRider(this, m_fDamageIntensity, m_nPieceType, m_pDamageEntity, m_vecLastCollisionPosn, m_vecLastCollisionImpactVelocity);

    if (m_pDamageEntity && m_pDamageEntity->IsVehicle()) {
        m_nLastWeaponDamageType = 49;
        m_pLastDamageEntity = m_pDamageEntity;
        m_pDamageEntity->RegisterReference(m_pLastDamageEntity);
    }

    if (physicalFlags.bCollisionProof) {
        return;
    }

    CEntity* damageEntity = m_pDamageEntity;
    if (damageEntity && damageEntity->IsBuilding() && m_vecLastCollisionImpactVelocity.Dot(m_matrix->GetUp()) > 0.6f) {
        return;
    }

    if (damageIntensity > 25.0f && (m_nType & 0xF8) != 0x28) {
        auto* playerVehicle = FindPlayerVehicle();

        if ((m_nVehicleUpperFlags & 1) && playerVehicle) {
            if (damageEntity == playerVehicle && (m_nType & 0xF8) != 0x20) {
                float mySpeed = m_vecMoveSpeed.Magnitude();
                if (playerVehicle->m_vecMoveSpeed.Magnitude() >= mySpeed) {
                    if (playerVehicle->m_vecMoveSpeed.Magnitude() > 0.1f) {
                        FindPlayerPed()->SetWantedLevelNoDrop(1);
                    }
                }
            }
        }

        float collisionDamage = (damageIntensity - 25.0f) * m_pHandlingData->m_fCollisionDamageMultiplier;
        if (collisionDamage > 0.0f) {
            if (collisionDamage > 5.0f && m_pDriver) {
                if (damageEntity->IsPed()) {
                    const auto ped = damageEntity->AsPed();
                    if (ped->IsVehicle() && (playerVehicle != this || ped->m_apBones[7] != PED_NODE_LEFT_LEG)) {
                        if (ped->AsPed()->m_roadRageWith) {
                            m_pDriver->Say(CTX_GLOBAL_CRASH_BIKE, 0, 1.0f, 0, 0);
                        }
                    }
                }
            }

            float prevHealth = m_fHealth;
            float healthLoss = 0.0f;

            if (this == playerVehicle) {
                healthLoss = (m_nFlags2 < 0) ? (collisionDamage * 0.1667f) : (collisionDamage * 0.5f);
            } else if (m_nFlags2 < 0) {
                healthLoss = collisionDamage * 0.0833f;
            } else if (damageEntity && damageEntity == playerVehicle) {
                healthLoss = collisionDamage * 0.6667f;
            } else {
                healthLoss = collisionDamage * 0.25f;
            }

            m_fHealth -= healthLoss;
            if (m_fHealth <= 1.0f && prevHealth > 1.0f) {
                m_fHealth = 1.0f;
            }
        }
    }

    if (m_fHealth < 250.0f && !bikeFlags.bEngineOnFire) {
        bikeFlags.bEngineOnFire = true;
        m_BlowUpTimer = 0.0f;
        m_Damager = m_pDamageEntity;
        if (m_Damager) {
            m_Damager->RegisterReference(m_Damager);
        }
    }
    */
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
    rng::fill(m_aBikeNodes, nullptr);
    CClumpModelInfo::FillFrameArray(m_pRwClump, m_aBikeNodes);
}

// 0x6B7080
void CBike::PlayCarHorn() {
    if (CanUpdateHornCounter() || m_HornCounter != 0) {
        return;
    }

    if (m_nCarHornTimer) {
        m_nCarHornTimer--;
        return;
    }

    m_nCarHornTimer = (CGeneral::GetRandomNumber() & 0x7F) + 150;
    const auto r = m_nCarHornTimer & 7;
    if (r < 2) {
        m_nCarHornTimer = 45;
    } else if (r < 4) {
        if (m_pDriver) {
            m_pDriver->Say(CTX_GLOBAL_BLOCKED);
        }
        m_nCarHornTimer = 45;
    } else {
        if (m_pDriver) {
            m_pDriver->Say(CTX_GLOBAL_BLOCKED);
        }
    }
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
bool CBike::SetUpWheelColModel(CColModel* cl) {
    const auto colData = cl->m_pColData;
    CVehicleModelInfo* mi = CModelInfo::GetModelInfo(GetModelId())->AsVehicleModelInfoPtr();
    const auto colModel = GetColModel();
    cl->m_boundSphere = colModel->m_boundSphere;
    cl->m_boundBox = colModel->m_boundBox;

    const auto v6 = m_aBikeNodes[4];
    RwMatrix matrix = v6->modelling;
    RwFrame* pParentFrame = (RwFrame*)v6->object.parent;
    if (pParentFrame) {
        do {
            RwMatrixTransform(&matrix, &pParentFrame->modelling, rwCOMBINEPOSTCONCAT);
            pParentFrame = (RwFrame*)pParentFrame->object.parent;
        } while (pParentFrame != m_aBikeNodes[1] && pParentFrame);
    }

    colData->m_pSpheres->Set(mi->m_fWheelSizeFront * 0.5f, matrix.pos, SURFACE_RUBBER, 13u);

    const auto v10 = m_aBikeNodes[5];
    matrix = v10->modelling;
    RwFrame* parent = (RwFrame*)v10->object.parent;
    if (parent) {
        do {
            RwMatrixTransform(&matrix, &parent->modelling, rwCOMBINEPOSTCONCAT);
            parent = (RwFrame*)parent->object.parent;
        } while (parent != m_aBikeNodes[1] && parent);
    }

    colData->m_pSpheres[1].Set(mi->m_fWheelSizeRear * 0.5f, matrix.pos, SURFACE_RUBBER, 15u);
    colData->m_nNumSpheres = 2;
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
    physicalFlags.bSkipLineCol = false;
    physicalFlags.bProcessingShift = false;
    m_fMovingSpeed = 0.0f;
    rng::fill(m_aWheelRatios, BILLS_EXTENSION_LIMIT);

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
