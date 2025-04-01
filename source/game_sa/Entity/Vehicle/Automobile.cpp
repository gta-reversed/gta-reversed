/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include <bit> // popcnt

#include "Automobile.h"
#include "ModelIndices.h"
#include "WaterCannons.h"
#include "Buoyancy.h"
#include "Skidmarks.h"
#include "CarCtrl.h"
#include "Glass.h"
#include "ModelIndices.h"
#include "InterestingEvents.h"
#include "VehicleRecording.h"
#include "EventDanger.h"

#include <Tasks/TaskTypes/TaskSimpleGangDriveBy.h>

#include <FireManager.h>

bool& CAutomobile::m_sAllTaxiLights = *(bool*)0xC1BFD0;
CVector& CAutomobile::vecHunterGunPos = *(CVector*)0x8D3394;
CMatrix* CAutomobile::matW2B = (CMatrix*)0xC1C220;

constexpr size_t NUM_AUTOMOBILE_SUSP_LINES  = (size_t)MAX_CARWHEELS;
constexpr size_t NUM_RHINO_EXTRA_SUSP_LINES = (size_t)MAX_CARWHEELS * 2u; // 8
constexpr size_t NUM_RHINO_SUSP_LINES       = NUM_AUTOMOBILE_SUSP_LINES + NUM_RHINO_EXTRA_SUSP_LINES; // 8
constexpr size_t MAX_NUM_SUSP_LINES         = std::max(NUM_AUTOMOBILE_SUSP_LINES, NUM_RHINO_SUSP_LINES); // 12
auto& aAutomobileColPoints = *(std::array<CColPoint, MAX_NUM_SUSP_LINES>*)0xC1BFF8;

static constexpr CVector PACKER_COL_PIVOT = CVector(0.0f, 0.0f, 2.0f);
static constexpr float CAR_BALANCE_MULT = 0.08f;

static constexpr CVector TANK_SHOT_DOOM_POS(0.0f, -1.394f, 2.296f);
static constexpr CVector TANK_SHOT_DOOM_DEFAULT_TARGET(0.0f, 2.95f, 2.97f);
static constexpr CVector TANK_SHOT_DOOM_DISTANCE_TO_DEFAULT_TARGET = TANK_SHOT_DOOM_DEFAULT_TARGET - TANK_SHOT_DOOM_POS;

static constexpr uint32 TIGER_GUNFIRE_RATE = 60;
static constexpr CVector TIGER_GUN_POS(0.0f, 0.5f, 0.2f); // 0xC1C208

void CAutomobile::InjectHooks()
{
    RH_ScopedVirtualClass(CAutomobile, 0x871120, 71);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(Constructor, 0x6B0A90);
    RH_ScopedInstall(Destructor, 0x6A61E0);

    RH_ScopedInstall(SetupModelNodes, 0x6A0770);
    RH_ScopedInstall(HydraulicControl, 0x6A07A0);
    RH_ScopedInstall(UpdateMovingCollision, 0x6A1460);
    RH_ScopedInstall(ProcessBuoyancy, 0x6A8C00);
    RH_ScopedInstall(ProcessHarvester, 0x6A9680);
    RH_ScopedInstall(TankControl, 0x6AE850);
    RH_ScopedInstall(DoSoftGroundResistance, 0x6A4AF0);
    RH_ScopedInstall(ProcessCarWheelPair, 0x6A4EC0);
    RH_ScopedInstall(PlaceOnRoadProperly, 0x6AF420);
    RH_ScopedInstall(RcbanditCheck1CarWheels, 0x6B3F70);
    RH_ScopedInstall(RcbanditCheckHitWheels, 0x6B45E0);
    RH_ScopedInstall(FireTruckControl, 0x729B60);
    RH_ScopedInstall(SetHeliOrientation, 0x6A2450);
    RH_ScopedInstall(ClearHeliOrientation, 0x6A2460);
    RH_ScopedInstall(ReduceHornCounter, 0x6A29A0);
    RH_ScopedInstall(FixTyre, 0x6A3580);
    RH_ScopedInstall(SetTaxiLight, 0x6A3740);
    RH_ScopedInstall(SetAllTaxiLights, 0x6A3760);
    RH_ScopedInstall(GetMovingCollisionOffset, 0x6A2150);
    RH_ScopedInstall(TellHeliToGoToCoors, 0x6A2390);
    RH_ScopedInstall(TellPlaneToGoToCoors, 0x6A2470);
    RH_ScopedInstall(SetTotalDamage, 0x6A27F0);
    RH_ScopedInstall(CustomCarPlate_BeforeRenderingStart, 0x6A2F00);
    RH_ScopedInstall(CustomCarPlate_AfterRenderingStop, 0x6A2F30);
    RH_ScopedInstall(GetAllWheelsOffGround, 0x6A2F70);
    RH_ScopedInstall(FixDoor, 0x6A35A0);
    RH_ScopedInstall(FixPanel, 0x6A3670);
    RH_ScopedInstall(PlayHornIfNecessary, 0x6A3820);
    RH_ScopedInstall(SetBusDoorTimer, 0x6A3860);
    RH_ScopedInstall(ProcessAutoBusDoors, 0x6A38A0);
    RH_ScopedInstall(BoostJumpControl, 0x6A3A60);
    RH_ScopedInstall(StopNitroEffect, 0x6A3E60);
    RH_ScopedInstall(NitrousControl, 0x6A3EA0);
    RH_ScopedInstall(KnockPedOutCar, 0x6A44C0);
    RH_ScopedInstall(PopBootUsingPhysics, 0x6A44D0);
    RH_ScopedInstall(CloseAllDoors, 0x6A4520);
    RH_ScopedInstall(GetCarRoll, 0x6A6010);
    RH_ScopedInstall(GetCarPitch, 0x6A6050);
    RH_ScopedInstall(IsInAir, 0x6A6140);
    RH_ScopedInstall(dmgDrawCarCollidingParticles, 0x6A6DC0);
    RH_ScopedInstall(SpawnFlyingComponent, 0x6A8580);
    RH_ScopedInstall(ProcessSwingingDoor, 0x6A9D70);
    RH_ScopedInstall(RemoveBonnetInPedCollision, 0x6AA200);
    RH_ScopedInstall(PopDoor, 0x6ADEF0);
    RH_ScopedInstall(PopPanel, 0x6ADF80);
    RH_ScopedInstall(ScanForCrimes, 0x6ADFF0);
    RH_ScopedInstall(BlowUpCarsInPath, 0x6AF110);
    RH_ScopedInstall(PopBoot, 0x6AF910);
    RH_ScopedInstall(CloseBoot, 0x6AFA20);
    RH_ScopedInstall(SetBumperDamage, 0x6B1350);
    RH_ScopedInstall(SetPanelDamage, 0x6B1480);
    RH_ScopedInstall(SetDoorDamage, 0x6B1600);
    RH_ScopedInstall(TowTruckControl, 0x6A40F0);
    RH_ScopedInstall(ProcessCarOnFireAndExplode, 0x6A7090);

    RH_ScopedVMTInstall(Fix, 0x6A3440);
    RH_ScopedVMTInstall(DoBurstAndSoftGroundRatios, 0x6A47F0);
    RH_ScopedVMTInstall(PlayCarHorn, 0x6A3770);
    RH_ScopedVMTInstall(VehicleDamage, 0x6A7650);
    RH_ScopedVMTInstall(GetComponentWorldPosition, 0x6A2210);
    RH_ScopedVMTInstall(IsComponentPresent, 0x6A2250);
    RH_ScopedVMTInstall(GetDooorAngleOpenRatio, 0x6A2270);
    RH_ScopedVMTInstall(GetDooorAngleOpenRatioU32, 0x6A62C0);
    RH_ScopedVMTInstall(IsDoorReady, 0x6A2290);
    RH_ScopedVMTInstall(IsDoorReadyU32, 0x6A6350);
    RH_ScopedVMTInstall(IsDoorFullyOpen, 0x6A22D0);
    RH_ScopedVMTInstall(IsDoorFullyOpenU32, 0x6A63E0);
    RH_ScopedVMTInstall(IsDoorClosed, 0x6A2310);
    RH_ScopedVMTInstall(IsDoorClosedU32, 0x6A6470);
    RH_ScopedVMTInstall(IsDoorMissing, 0x6A2330);
    RH_ScopedVMTInstall(IsDoorMissingU32, 0x6A6500);
    RH_ScopedVMTInstall(IsOpenTopCar, 0x6A2350);
    RH_ScopedVMTInstall(IsRoomForPedToLeaveCar, 0x6A3850);
    RH_ScopedVMTInstall(SetupDamageAfterLoad, 0x6B3E90);
    RH_ScopedVMTInstall(GetHeightAboveRoad, 0x6A62B0);
    RH_ScopedVMTInstall(GetNumContactWheels, 0x6A62A0);
    RH_ScopedVMTInstall(Teleport, 0x6A9CA0);
    RH_ScopedVMTInstall(Save, 0x5D47E0, {.enabled = false });
    RH_ScopedVMTInstall(Load, 0x5D2980, {.enabled = false });
    RH_ScopedVMTInstall(ProcessControl, 0x6B1880);
    RH_ScopedVMTInstall(AddMovingCollisionSpeed, 0x6A1ED0);
    RH_ScopedVMTInstall(ProcessAI, 0x6B4800);
    RH_ScopedVMTInstall(ResetSuspension, 0x6A2AE0);
    RH_ScopedVMTInstall(ProcessFlyingCarStuff, 0x6A8500);
    RH_ScopedVMTInstall(DoHoverSuspensionRatios, 0x6A45C0);
    RH_ScopedVMTInstall(ProcessSuspension, 0x6AFB10);
    // todo: fix/review Tow funcs
    RH_ScopedVMTInstall(GetTowHitchPos, 0x6AF1D0);
    RH_ScopedVMTInstall(GetTowBarPos, 0x6AF250);
    RH_ScopedVMTInstall(SetTowLink, 0x6B4410);
    RH_ScopedVMTInstall(BreakTowLink, 0x6A4400);
    RH_ScopedVMTInstall(FindWheelWidth, 0x6A6090);

    RH_ScopedVMTInstall(SetupSuspensionLines, 0x6A65D0);
    RH_ScopedVMTInstall(ProcessEntityCollision, 0x6ACE70);
    RH_ScopedVMTInstall(ProcessControlCollisionCheck, 0x6A29C0);
    RH_ScopedVMTInstall(ProcessControlInputs, 0x6AD690);
    RH_ScopedVMTInstall(OpenDoor, 0x6A6AE0);
    RH_ScopedVMTInstall(BlowUpCar, 0x6B3780);
    RH_ScopedVMTInstall(BlowUpCarCutSceneNoExtras, 0x6B3BB0);
}

// 0x6B0A90
CAutomobile::CAutomobile(int32 modelIndex, eVehicleCreatedBy createdBy, bool setupSuspensionLines) :
    CVehicle(createdBy),
    m_damageManager{ 0.5f }
{
    m_nVehicleType    = VEHICLE_TYPE_AUTOMOBILE;
    m_nVehicleSubType = VEHICLE_TYPE_AUTOMOBILE;

    m_fBurnTimer    = 0.0f;
    m_bDoingBurnout = false;

    CVehicle::SetModelIndex(modelIndex);
    SetupModelNodes();

    auto& mi = *GetVehicleModelInfo(); // has to be called after `SetModelIndex` has been called.

    m_pHandlingData = &mi.GetHandlingData(); // For now this has to be here, because the model index is not set in the member init list
    m_pFlyingHandlingData = &mi.GetFlyingHandlingData(); // Moved this over here
    m_nHandlingFlagsIntValue = m_pHandlingData->m_nHandlingFlags;
    if (handlingFlags.bHydraulicGeom && CGeneral::GetRandomNumber() % 4 == 0) { // 0x6B0BD7
        AddVehicleUpgrade(ModelIndices::MI_HYDRAULICS);
    }

    field_804 = 20.0f;
    m_fGasPedalAudio = 0.0f;
    m_fIntertiaValue1 = 0.0f;
    m_fIntertiaValue2 = 0.0f;

    // 0x6B0C2E
    mi.ChooseVehicleColour(m_nPrimaryColor, m_nSecondaryColor, m_nTertiaryColor, m_nQuaternaryColor, 1);

    // 0x6B0C49
    if (m_pHandlingData->m_bIsVan) {
        vehicleFlags.bIsVan = true;
    }
    if (m_pHandlingData->m_bIsBig) {
        vehicleFlags.bIsBig = true;
    }
    if (m_pHandlingData->m_bIsBus) {
        vehicleFlags.bIsBus = true;
    }
    if (m_pHandlingData->m_bIsLow) {
        vehicleFlags.bLowVehicle = true;
    }

    // 0x6B0CA8
    // Deal with front doors
    {
        // Right
        auto& doorRF = m_doors[eDoors::DOOR_RIGHT_FRONT];
        doorRF.m_nAxis = 2;
        doorRF.m_fClosedAngle = 0.f;
        doorRF.m_fOpenAngle = vehicleFlags.bIsBus ? PI * 0.4f : PI * 0.5f; // `PI * 0.4f` or `PI * o.4f`, same thing, although they used the latter most likely.

        // Left
        auto& doorLF = m_doors[eDoors::DOOR_LEFT_FRONT];
        doorLF = doorRF;
        doorLF.m_fOpenAngle = -doorRF.m_fOpenAngle;
    }

    // 0x6B0CF0
    // Deal with rear doors
    if (modelIndex == MODEL_RHINO) { // For Rhino just hide it
        for (auto door : { DOOR_LEFT_REAR, DOOR_RIGHT_REAR }) {
            m_doors[(size_t)door] = {
                .m_fOpenAngle = 1.f, // todo: see line Automobile.cpp:1065
                .m_fClosedAngle = 1.f,
                .m_fAngle = 1.f,
                .m_fPrevAngle = 1.f
            };
        }

        // Hide both
        for (auto comp : { CAR_WHEEL_LM, CAR_WHEEL_RM }) {
            rwObjectSetFlags(GetFirstObject(m_aCarNodes[comp]), ATOMIC_IS_NOT_PRESENT);
        }
    } else { // 0x6B0D57
        auto& doorLR = m_doors[DOOR_LEFT_REAR];
        auto& doorRR = m_doors[DOOR_RIGHT_REAR];

        doorLR.m_nAxis = 2;
        doorLR.m_fClosedAngle = 0.f;

        doorRR.m_fClosedAngle = 0.f;
        doorRR.m_nAxis = 2;

        if (vehicleFlags.bIsVan) {
            doorLR.m_fOpenAngle = -PI * 0.4f;
            doorLR.m_nDirn = 20;

            doorRR.m_fOpenAngle = PI * 0.4f;
            doorRR.m_nDirn = 20;
        } else {
            doorLR.m_fOpenAngle = -PI * 0.5f;
            doorLR.m_nDirn = 16;

            doorRR.m_fOpenAngle = PI * 0.5f;
            doorRR.m_nDirn = 19;
        }
    }

    // 0x6B0DBE
    // Bonnet
    {
        auto& bonnet = m_doors[DOOR_BONNET];
        bonnet.m_nAxis = 0;
        bonnet.m_fClosedAngle = 0.f;
        bonnet.m_fOpenAngle = m_pHandlingData->m_bReverseBonnet ? -PI * 0.3f : PI * 0.3f;
        bonnet.m_nDirn = m_pHandlingData->m_bReverseBonnet ? 36 : 33;
    }

    // 0x6B0DF4
    // Boot(y)
    {
        auto& boot = m_doors[DOOR_BOOT];

        if (m_pHandlingData->m_bHangingBoot) {
            boot.m_fOpenAngle = PI * 0.4f;
            boot.m_nDirn = 21;
        } else if (m_pHandlingData->m_bTailgateBoot) {
            boot.m_fOpenAngle = PI * 0.5f;
            boot.m_nDirn = 18;
        } else {
            boot.m_fOpenAngle = -PI * 0.3f;
            boot.m_nDirn = 20;
        }

        boot.m_fClosedAngle = 0.f;
        boot.m_nAxis = 0;
    }

    // 0x6B0E4B
    // If model has no doors, hide them all
    if (m_pHandlingData->m_bNoDoors) {
        m_damageManager.SetDoorStatus({ DOOR_LEFT_FRONT, DOOR_RIGHT_FRONT, DOOR_LEFT_REAR, DOOR_RIGHT_REAR }, DAMSTATE_NOTPRESENT);
    }

    // Init this field which is never used
    for (auto& doorRot : DoorRotation) {
        doorRot = CGeneral::GetRandomNumberInRange(-0.15f, 0.15f);
    }

    // 0x6B0EE6
    // Do something with misc components? Not sure..
    if (m_nModelIndex == MODEL_TOWTRUCK && m_aCarNodes[CAR_MISC_B]) {
        if (auto& panelFL = m_panels[FRONT_LEFT_PANEL]; panelFL.m_nFrameId == (uint16)-1) {
            panelFL.SetPanel(CAR_MISC_B, 2, 1.f);
        }
    } else if (m_nModelIndex == MODEL_TRACTOR && m_aCarNodes[CAR_BOOT]) {
        if (auto& panelFL = m_panels[FRONT_LEFT_PANEL]; panelFL.m_nFrameId == (uint16)-1) {
            panelFL.SetPanel(CAR_BOOT, 1, 1.f);
        }
    }

    // 0x6B0F3B
    // Deal with swinging chassis
    if (m_pHandlingData->m_bSwingingChassis) {
        const auto GetAngleMult = [modelIndex] {
            switch (modelIndex) {
            case MODEL_COPCARVG:
            case MODEL_ESPERANT:
                return 0.03f;
            case MODEL_STRETCH:
                return 0.01f;
            }
            return 0.02f;
        };
        m_swingingChassis.m_fOpenAngle = PI * GetAngleMult();
        m_swingingChassis.m_fClosedAngle = -m_swingingChassis.m_fOpenAngle;
        m_swingingChassis.m_nAxis        = 2;
        m_swingingChassis.m_nDirn        = 196;
        m_swingingChassis.m_nDoorState = eDoorState::DOOR_HIT_MAX_END;
    } else if (modelIndex == MODEL_FIRELA) {
        m_swingingChassis.m_fOpenAngle = PI / 10.f;
        m_swingingChassis.m_fClosedAngle = -m_swingingChassis.m_fOpenAngle;
        m_swingingChassis.m_nAxis = 2;
        m_swingingChassis.m_nDirn = 388;
        m_swingingChassis.m_nDoorState = eDoorState::DOOR_HIT_MAX_END;
    }

    m_moveForce = CVector();
    m_turnForce = CVector();

    // 0x6B1048
    m_fMass = m_pHandlingData->m_fMass;
    m_fTurnMass = m_pHandlingData->m_fTurnMass;
    m_vecCentreOfMass = m_pHandlingData->m_vecCentreOfMass;
    m_fElasticity = 0.05f;
    m_fBuoyancyConstant = m_pHandlingData->m_fBuoyancyConstant;
    m_fAirResistance = GetDefaultAirResistance();

    // 0x6B10B5
    m_fRotationBalance        = 0.0f;
    m_fAircraftGoToHeading    = 0.0f;
    m_nBusDoorTimerEnd        = 0;
    m_nBusDoorTimerStart      = 0;
    m_fSteerAngle             = 0.0f;
    m_fGasPedal               = 0.0f;
    m_fBreakPedal             = 0.0f;
    m_pExplosionVictim        = nullptr;
    m_fGasPedalAudio          = 0.0f;
    m_fMoveDirection          = 0.0f;
    m_wMiscComponentAngle     = 0;
    m_wMiscComponentAnglePrev = 0;

    // 0x6B1111
    rng::fill(m_fWheelsSuspensionCompression, 1.f);
    rng::fill(m_fWheelsSuspensionCompressionPrev, 1.f);

    m_nNumContactWheels     = 0;
    m_nWheelsOnGround       = 0;
    m_wheelsOnGrounPrev     = 0;
    m_fFrontHeightAboveRoad = 0.0f;
    m_fRearHeightAboveRoad  = 0.0f;
    m_fCarTraction          = 1.0f;
    m_fTireTemperature      = 1.0f;

    // 0x6B119A
    if (setupSuspensionLines) {
        CAutomobile::SetupSuspensionLines();
    }

    m_nStatus = eEntityStatus::STATUS_SIMPLE;
    m_nNumPassengers = 0;

    if (m_nDoorLock == CARLOCK_UNLOCKED) {
        if (IsLawEnforcementVehicle()) {
            m_nDoorLock = CARLOCK_COP_CAR;
        }
    }

    m_fDoomVerticalRotation = 0.0f;
    m_fDoomHorizontalRotation = 0.05f;
    m_fUpDownLightAngle[0] = 0.0f;
    m_fForcedOrientation = -1.0f;
    m_harvesterParticleCounter = 0;
    RightDoorOpenForDriveBys = 0.f;
    LeftDoorOpenForDriveBys = 0.f;

    switch (m_nModelIndex) {
    case -2: { // 0x6B120F
        // I don't think this is reachable, as it would crash somewhere above.

        rwObjectSetFlags(GetFirstObject(m_aCarNodes[CAR_WHEEL_LF]), ATOMIC_IS_NOT_PRESENT);

        // Probably an inlined function here, as there was a redundant check here (model == -2)
        CMatrix wheelRFMat{ RwFrameGetMatrix(m_aCarNodes[CAR_WHEEL_RF]) };
        CMatrix wheelLFMat{ RwFrameGetMatrix(m_aCarNodes[CAR_WHEEL_LF]) };
        wheelRFMat.SetTranslate({ wheelLFMat.GetPosition().x + 0.1f, 0.f, wheelLFMat.GetPosition().z});
        wheelRFMat.UpdateRW();
        break;
    }
    case MODEL_RHINO: { // 0x6B12D6
        physicalFlags.bExplosionProof = true;
        physicalFlags.bBulletProof = true;
        break;
    }
    }

    std::ranges::fill(m_exhaustNitroFxSystem, nullptr);

    m_vehicleAudio.Initialise(this);
    m_heliDustFxTimeConst = 0.0f;
    m_fireParticleCounter = 0;
}

// 0x6A61E0
CAutomobile::~CAutomobile() {
    FxSystem_c::SafeKillAndClear(m_pFireParticle);
    StopNitroEffect();
    m_vehicleAudio.Terminate();
}

// 0x6B1880
void CAutomobile::ProcessControl()
{
    uint32 extraHandlingFlags = 0;
    if (vehicleFlags.bUseCarCheats) {
        extraHandlingFlags |= EXTRA_HANDLING_PERFECT;
    }
    CCollisionData* colData = GetColModel()->m_pColData;
    m_bDoingBurnout = false;
    autoFlags.bIsBoggedDownInSand = false;
    vehicleFlags.bWarnedPeds = false;
    vehicleFlags.bRestingOnPhysical = false;
    m_vehicleAudio.Service();
    bool bExplodeImmediately = false;
    if (IsSubPlane() || IsSubHeli()) {
        eCarMission carMission = m_autoPilot.m_nCarMission;
        if ((carMission == MISSION_PLANE_CRASH_AND_BURN
            || carMission == MISSION_HELI_CRASH_AND_BURN
            || m_nStatus == STATUS_PLAYER && m_fHealth < 250.0 && m_fireParticleCounter == 2)
            && m_nStatus != STATUS_WRECKED
            && (m_fDamageIntensity > 0.0f && m_vecLastCollisionImpactVelocity.z > 0.0f || !IsInAir())
            && (m_vecMoveSpeed.z >= 0.0f || physicalFlags.bSubmergedInWater))
        {
            bExplodeImmediately = true;
        }
    }

    if (CCheat::IsActive(CHEAT_ALL_TAXIS_NITRO)) {
        if (m_nStatus == STATUS_PLAYER && IsTransportVehicle())
            extraHandlingFlags |= EXTRA_HANDLING_TAXI_BOOST;
    }

    bool bExtraHandlingTaxiBoost = !!(extraHandlingFlags & EXTRA_HANDLING_TAXI_BOOST);
    if (CCheat::IsActive(CHEAT_ALL_CARS_HAVE_NITRO) || bExtraHandlingTaxiBoost) {
        handlingFlags.bNosInst = true;
        m_nNitroBoosts = 101;
    }
    if (vehicleFlags.bIsBus)
        ProcessAutoBusDoors();
    ProcessCarAlarm();
    UpdateClumpAlpha();
    UpdatePassengerList();
    if (m_pDriver) {
        if (!vehicleFlags.bDriverLastFrame && m_nBombOnBoard == BOMB_IGNITION_ACTIVATED) {
            m_wBombTimer = 1000;
            m_pWhoDetonatedMe = m_pWhoInstalledBombOnMe;
            CEntity::SafeRegisterRef(m_pWhoDetonatedMe);
        }
        vehicleFlags.bDriverLastFrame = true;
    }
    else {
        vehicleFlags.bDriverLastFrame = false;
    }

    if (m_nNumPassengers && IsUpsideDown())
        CanPedEnterCar();

    if (ProcessAI(extraHandlingFlags))
        return;

    bool extraPerfectHandling = !!(extraHandlingFlags & EXTRA_HANDLING_PERFECT);
    bool extraHandlingNitros = !!(extraHandlingFlags & EXTRA_HANDLING_NITROS);
    bool extraHandlingWheelsTouchingSand = !!(extraHandlingFlags & EXTRA_HANDLING_WHEELS_TOUCHING_SAND);
    bool extraHandlingTaxiBoost = !!(extraHandlingFlags & EXTRA_HANDLING_TAXI_BOOST);

    bool skipPhysics = false;
    if (m_nStatus == STATUS_ABANDONED || m_nStatus == STATUS_WRECKED) {
        bool isVehicleIdle = false;
        if (!vehicleFlags.bVehicleColProcessed
            && m_vecMoveSpeed == 0.0f
            && m_fWheelsSuspensionCompressionPrev[CAR_WHEEL_REAR_RIGHT] != 1.0f)
        {
            isVehicleIdle = true;
        }

        float forceLimitRadius = 0.003f;
        float torqueLimitRadius = 0.0009f;
        float movingSpeedLimit = 0.005f;
        if (IsSubPlane()) {
            if (m_fDamageIntensity > 0.0f && m_pDamageEntity && m_pDamageEntity->IsVehicle())
                forceLimitRadius = 0.003f;
            else
                forceLimitRadius = 0.009f;
            torqueLimitRadius = 0.003f;
            movingSpeedLimit = 0.1f;
        }
        else if (m_nStatus == STATUS_WRECKED) {
            forceLimitRadius = 0.006f;
            torqueLimitRadius = 0.0015f;
            movingSpeedLimit = 0.015f;
        }

        m_vecForce  = (m_vecForce + m_vecMoveSpeed)  / 2.0f;
        m_vecTorque = (m_vecTorque + m_vecTurnSpeed) / 2.0f;

        bool resetSpeed = true;

        forceLimitRadius *= CTimer::GetTimeStep();
        torqueLimitRadius *= CTimer::GetTimeStep();
        if (forceLimitRadius * forceLimitRadius < m_vecForce.SquaredMagnitude()
            || sq(torqueLimitRadius) < m_vecTorque.SquaredMagnitude()
            || movingSpeedLimit <= m_fMovingSpeed
            || m_fDamageIntensity > 0.0f && m_pDamageEntity && m_pDamageEntity->IsPed())
        {
            resetSpeed = false;
        }
        else if (physicalFlags.bSubmergedInWater) {
            if (DistanceBetweenPointsSquared(TheCamera.GetPosition(), GetPosition()) < sq(50.0f)) {
                resetSpeed = false;
            }
        }
        if (!resetSpeed && !isVehicleIdle) {
            m_nFakePhysics = 0;
        }
        else {
            m_nFakePhysics++;
            if (m_nFakePhysics > 10 || isVehicleIdle) {
                if (!isVehicleIdle || m_nFakePhysics > 10)
                    m_nFakePhysics = 10;
                ResetMoveSpeed();
                ResetTurnSpeed();
                skipPhysics = true;
            }
        }

        if (IsRealHeli()
            && m_wheelSpeed[CAR_WHEEL_REAR_LEFT] > 0.0f || IsAmphibiousHeli()
            && !m_nNumContactWheels
            && m_fDamageIntensity <= 0.0f
            || m_nModelIndex == MODEL_VORTEX
            || CCheat::IsActive(CHEAT_CARS_ON_WATER)
            && IsAnyWheelTouchingShallowWaterGround())
        {
            skipPhysics = false;
            m_nFakePhysics = 0;
        }
    }

    for (auto& collisionEntity : m_apWheelCollisionEntity) {
        if (collisionEntity) {
            vehicleFlags.bRestingOnPhysical = true;
            if (!CWorld::bForceProcessControl && collisionEntity->m_bIsInSafePosition) {
                m_bWasPostponed = true;
                return;
            }
        }
    }

    if (vehicleFlags.bRestingOnPhysical) {
        skipPhysics = false;
        m_nFakePhysics = 0;
    }

    VehicleDamage(0.0f, eVehicleCollisionComponent::DEFAULT, nullptr, nullptr, nullptr, WEAPON_RAMMEDBYCAR);
    if (m_nStatus == STATUS_PLAYER && ModelIndices::HasWaterCannon(m_nModelIndex)) {
        FireTruckControl(nullptr);
    }
    else {
        switch (m_nModelIndex)
        {
        case MODEL_RHINO:
        case MODEL_RCTIGER:
            TankControl();
            break;
        case MODEL_PACKER:
        case MODEL_DOZER:
        case MODEL_DUMPER:
        case MODEL_CEMENT:
        case MODEL_ANDROM:
        case MODEL_FORKLIFT:
            if (UpdateMovingCollision(-1.0f))
                skipPhysics = false;
            break;
        case MODEL_TOWTRUCK:
        case MODEL_TRACTOR:
            TowTruckControl();
            break;
        default:
            if (handlingFlags.bHydraulicInst)
                HydraulicControl();
            else if ((CCheat::IsActive(CHEAT_PERFECT_HANDLING) || (extraHandlingFlags & EXTRA_HANDLING_TAXI_BOOST))
                && m_nStatus == STATUS_PLAYER
                && m_vecMoveSpeed.SquaredMagnitude() > 0.04f)
            {
                BoostJumpControl();
            }
            break;
        }
    }
    if (handlingFlags.bNosInst) {
        NitrousControl(0);
    } else {
        StopNitroEffect();
    }

    if (FindPlayerVehicle() == this && CPad::GetPad()->CarGunJustDown())
        ActivateBomb();

    if (skipPhysics) {
        SkipPhysics();
        vehicleFlags.bAudioChangingGear = false;
        vehicleFlags.bVehicleColProcessed = false;
        m_fTireTemperature = 1.0f;
    }
    else {
        if (!vehicleFlags.bVehicleColProcessed) {
            ProcessControlCollisionCheck(true);
        }
        bool oldSubmergedInWater = physicalFlags.bSubmergedInWater;
        bool postCollision = false;
        if (m_fDamageIntensity > 0.0f && m_vecLastCollisionImpactVelocity.z > 0.1f)
            postCollision = true;
        CPhysical::ProcessControl();

        if (m_nModelIndex == MODEL_SKIMMER) {
            auto skimmerModelInfo = CModelInfo::GetModelInfo(MODEL_SKIMMER)->AsVehicleModelInfoPtr();
            auto skimmerHandling = gHandlingDataMgr.GetBoatPointer(static_cast<uint8>(skimmerModelInfo->m_nHandlingId));
            ProcessBoatControl(skimmerHandling, &m_fDoomVerticalRotation, m_bHasHitWall, postCollision);
            physicalFlags.bTouchingWater = physicalFlags.bSubmergedInWater;
        }
        else {
            ProcessBuoyancy();
        }

        if (!oldSubmergedInWater && physicalFlags.bSubmergedInWater) {
            if (m_pDriver && m_pDriver->IsPlayer()) {
                m_pDriver->AsPlayer()->ResetPlayerBreath();
            }
            else {
                for (const auto passenger : GetPassengers()) {
                    if (passenger && passenger->IsPlayer()) {
                        passenger->AsPlayer()->ResetPlayerBreath();
                    }
                }
            }
        }
        if (m_nModelIndex == MODEL_RCBARON)
            ProcessFlyingCarStuff();
        if (!autoFlags.bIsMonsterTruck) {
            for (int32 i = 0; i < 4; i++) {
                float wheelRadius = 1.0f - m_aSuspensionSpringLength[i] / m_aSuspensionLineLength[i];
                m_fWheelsSuspensionCompression[i] = (m_fWheelsSuspensionCompression[i] - wheelRadius) / (1.0f - wheelRadius);
            }
        }

        ProcessSuspension();

        CVector contactPoints[4]{};
        CVector contactSpeeds[4]{};

        for (int32 i = 0; i < 4; i++) {
            if (m_fWheelsSuspensionCompression[i] >= 1.0f) {
                CColLine& colLine = colData->m_pLines[i];
                CColDisk& colDisk = colData->m_pDisks[i];
                if (colData->bUsesDisks) {
                    CVector& point = contactPoints[i];
                    point = colDisk.m_vecCenter;
                    point.z -= colDisk.m_fRadius;
                    point = GetMatrix().TransformVector(contactPoints[i]);
                }
                else {
                    contactPoints[i] = GetMatrix().TransformVector(colLine.m_vecEnd);
                }
            }
            else {
                m_wheelSkidmarkMuddy[i] = false;
                m_wheelSkidmarkType[i] = static_cast<eSkidmarkType>(g_surfaceInfos.GetSkidmarkType(m_wheelColPoint[i].m_nSurfaceTypeB));
                if (m_wheelSkidmarkType[i] == eSkidmarkType::MUDDY)
                    m_wheelSkidmarkMuddy[i] = true;
                contactPoints[i] = m_wheelColPoint[i].m_vecPoint - GetPosition();
            }
        }

        for (int32 i = 0; i < 4; i++) {
            CPhysical* collisionEntity = m_apWheelCollisionEntity[i];
            contactSpeeds[i] = GetSpeed(contactPoints[i]);
            if (collisionEntity) {
                contactSpeeds[i] -= collisionEntity->GetSpeed(m_vWheelCollisionPos[i]);
                m_apWheelCollisionEntity[i] = nullptr;
            }
        }

        float speedForward = DotProduct(m_vecMoveSpeed, GetForward());

        if (vehicleFlags.bAudioChangingGear
            && m_fGasPedal > 0.4f
            && m_fBreakPedal < 0.1f
            && speedForward  > 0.15f
            && this == FindPlayerVehicle()
            && CCamera::GetActiveCamera().m_nMode != MODE_1STPERSON)
        {
            if (m_nStatus == STATUS_PLAYER && !m_pHandlingData->m_bIsBus)
            {
                if (m_nBusDoorTimerEnd) {
                    uint32 timeStep = (uint32)CTimer::GetTimeStepInMS();
                    if (m_nBusDoorTimerEnd <= timeStep)
                        m_nBusDoorTimerEnd = 0;
                    else
                        m_nBusDoorTimerEnd -= timeStep;
                }
                else {
                    m_nBusDoorTimerEnd = 1000;
                }
            }

            if (IsAnyFrontAndRearWheelTouchingGround()) {
                CVector point = GetForward() * -1.0f;
                CVector force = std::min(m_fTurnMass, 2500.0f) * -0.008f * GetUp();
                ApplyTurnForce(force, point);
            }
        }

        float brake = m_pHandlingData->m_fBrakeDeceleration * m_fBreakPedal * CTimer::GetTimeStep();

        m_wheelsOnGrounPrev = m_nWheelsOnGround;
        m_nNumContactWheels = 0;
        m_nWheelsOnGround = 0;

        for (int32 i = 0; i < 4; i++) {
            if (m_fWheelsSuspensionCompression[i] >= 1.0f) {
                m_aWheelTimer[i] = std::max(m_aWheelTimer[i] - CTimer::GetTimeStep(), 0.0f);
                if (m_aWheelTimer[i] <= 0.0f)
                    continue;
            }
            else {
                m_aWheelTimer[i] = 4.0f;
            }

            m_nNumContactWheels++;

            switch (m_pHandlingData->GetTransmission().m_nDriveType)
            {
            case '4':
                m_nWheelsOnGround++;
                break;
            case 'F':
                if (i == CAR_WHEEL_FRONT_LEFT || i == CAR_WHEEL_FRONT_RIGHT) {
                    m_nWheelsOnGround++;
                    break;
                }
                break;
            case 'R':
                if (i == CAR_WHEEL_REAR_LEFT || i == CAR_WHEEL_REAR_RIGHT) {
                    m_nWheelsOnGround++;
                    break;
                }
            }
        }

        uint8 cheatType = CHEAT_HANDLING_NONE;
        if (handlingFlags.bNosInst && m_fTireTemperature < 0.0f)
            cheatType = CHEAT_HANDLING_NITROS;
        else if (extraPerfectHandling || CCheat::IsActive(CHEAT_PERFECT_HANDLING))
            cheatType = CHEAT_HANDLING_PERFECT;

        float acceleration = 0.0f;
        if (vehicleFlags.bEngineOn && !m_pHandlingData->m_bIsPlane && !m_pHandlingData->m_bIsHeli) {
            acceleration = m_pHandlingData->GetTransmission().CalculateDriveAcceleration(
                m_fGasPedal,
                m_nCurrentGear,
                m_fGearChangeCount,
                speedForward,
                &m_fIntertiaValue1,
                &m_fIntertiaValue2,
                m_nWheelsOnGround,
                cheatType
            );
            acceleration /= m_fVelocityFrequency;
        }

        float traction = m_pHandlingData->m_fTractionMultiplier / 250.0f; // (0.004f * mult)
        if (m_nStatus == STATUS_PHYSICS)
            traction = 250.0f / m_fCarTraction * m_pHandlingData->m_fTractionMultiplier;
        traction *= 0.25f / m_fVelocityFrequency;
        CPlane* vortex = AsPlane();
        if (CCheat::IsActive(CHEAT_PERFECT_HANDLING) || m_nModelIndex == MODEL_VORTEX && vortex->m_fAccelerationBreakStatus == 0.0f)
            traction *= 4.0f;

        if (this != FindPlayerVehicle() && (extraPerfectHandling || CCheat::IsActive(CHEAT_PERFECT_HANDLING))) {
            traction *= 1.2f;
            acceleration *= 1.4f;
            if (extraHandlingNitros || CCheat::IsActive(CHEAT_PERFECT_HANDLING)) {
                traction *= 1.3f;
                acceleration *= 1.4f;
            }
        }

        float steerAngle = 1.0f; // todo: Repeated branch in conditional chain
        if (speedForward <= 0.01f || m_aWheelTimer[CAR_WHEEL_FRONT_LEFT] <= 0.0f && m_aWheelTimer[CAR_WHEEL_REAR_LEFT] <= 0.0f)
            steerAngle = 1.0f;
        else if (m_nStatus != STATUS_PLAYER)
            steerAngle = 1.0f;
        else {
            CColPoint colPoint{};
            colPoint.m_nSurfaceTypeA = SURFACE_WHEELBASE;
            colPoint.m_nSurfaceTypeB = SURFACE_TARMAC;
            float speedRight = DotProduct(m_vecMoveSpeed, GetRight());
            float adhesive = g_surfaceInfos.GetAdhesiveLimit(&colPoint);
            steerAngle = adhesive * traction * 4.0f * 4.0f / (speedForward * speedForward);
            steerAngle = std::min(steerAngle, 1.0f);
            steerAngle = std::asin(steerAngle) / DegreesToRadians(m_pHandlingData->m_fSteeringLock);
            if (m_fSteerAngle < 0.0f && speedRight > 0.05f
                || m_fSteerAngle > 0.0f && speedRight < -0.05f
                || vehicleFlags.bIsHandbrakeOn
                || steerAngle > 1.0f)
            {
                steerAngle = 1.0f;
            }
        }
        m_fSteerAngle *= steerAngle;

        if (!handlingFlags.bProcRearwheelFirst) {
            if (handlingFlags.bSteerRearwheels)
                CAutomobile::ProcessCarWheelPair(
                    CAR_WHEEL_FRONT_LEFT, CAR_WHEEL_FRONT_RIGHT,
                    -999.0f,
                    contactSpeeds,
                    contactPoints,
                    traction,
                    acceleration,
                    brake,
                    true
                );
            else
                CAutomobile::ProcessCarWheelPair(
                    CAR_WHEEL_FRONT_LEFT, CAR_WHEEL_FRONT_RIGHT,
                    m_fSteerAngle,
                    contactSpeeds,
                    contactPoints,
                    traction,
                    acceleration,
                    brake,
                    true
                );
        }

        float rearWheelsSteerAngle = -999.0f;
        if (handlingFlags.bSteerRearwheels)
            rearWheelsSteerAngle = -m_fSteerAngle;
        else if (handlingFlags.bHbRearwheelSteer)
            rearWheelsSteerAngle = m_f2ndSteerAngle;

        CAutomobile::ProcessCarWheelPair(CAR_WHEEL_REAR_LEFT, CAR_WHEEL_REAR_RIGHT,
            rearWheelsSteerAngle,
            contactSpeeds,
            contactPoints,
            traction,
            acceleration,
            brake,
            false
        );

        if (handlingFlags.bProcRearwheelFirst) {
            if (handlingFlags.bSteerRearwheels)
                CAutomobile::ProcessCarWheelPair(
                    CAR_WHEEL_FRONT_LEFT, CAR_WHEEL_FRONT_RIGHT,
                    -999.0f,
                    contactSpeeds,
                    contactPoints,
                    traction,
                    acceleration,
                    brake,
                    true
                );
            else
                CAutomobile::ProcessCarWheelPair(
                    CAR_WHEEL_FRONT_LEFT, CAR_WHEEL_FRONT_RIGHT,
                    m_fSteerAngle,
                    contactSpeeds,
                    contactPoints,
                    traction,
                    acceleration,
                    brake,
                    true
                );
        }

        if (m_nStatus != STATUS_PLAYER) {
            if (CanUpdateHornCounter())
                ReduceHornCounter();
        }
        else if (handlingFlags.bHydraulicInst || CCheat::IsActive(CHEAT_PERFECT_HANDLING) || extraHandlingTaxiBoost)
            ProcessSirenAndHorn(false);
        else
            ProcessSirenAndHorn(true);

        if (m_nModelIndex != MODEL_RCBARON)
            ProcessFlyingCarStuff();
    }
    ProcessCarOnFireAndExplode(bExplodeImmediately);
    if (vehicleFlags.bSirenOrAlarm
        && (CTimer::GetFrameCounter() & 7) == 5
        && UsesSiren()
        && m_nModelIndex != MODEL_MRWHOOP
        && FindPlayerVehicle() == this)
    {
        CCarAI::MakeWayForCarWithSiren(this);
    }

    float suspensionShake = 0.0f;
    float roughnessShake = 0.0f;
    float wheelSpinRate = 0.0f;
    for (int32 i = 0; i < 4; i++) {
        float suspensionChange = m_fWheelsSuspensionCompressionPrev[i] - m_fWheelsSuspensionCompression[i];
        if (suspensionChange > 0.3f && !extraHandlingWheelsTouchingSand && m_vecMoveSpeed.SquaredMagnitude() > 0.04f) {
            //m_damageManager.GetWheelStatus(i); // return value is ignored
            if (suspensionChange > suspensionShake)
                suspensionShake = suspensionChange;
        }

        if (m_fWheelsSuspensionCompression[i] < 1.0f && m_nStatus == STATUS_PLAYER) {
            float roughness = (float)g_surfaceInfos.GetRoughness(m_wheelColPoint[i].m_nSurfaceTypeB) * 0.1f;
            roughnessShake = std::max(roughnessShake, roughness);
        }
        uint32 handlingId = m_pHandlingData->m_nVehicleId;
        cTransmission& transmission = gHandlingDataMgr.GetVehiclePointer(handlingId)->GetTransmission();
        if (m_aWheelTimer[i] <= 0.0f
            && (m_fGasPedal > 0.5f || m_fGasPedal < -0.5f)
            && ((i == CAR_WHEEL_FRONT_LEFT || i == CAR_WHEEL_FRONT_RIGHT) && transmission.m_nDriveType != 'R'
                || (i == CAR_WHEEL_REAR_LEFT || i == CAR_WHEEL_REAR_RIGHT) && transmission.m_nDriveType != 'F'))
        {
            wheelSpinRate += CVehicle::WHEELSPIN_INAIR_TARGET_RATE;
        }
        else if (m_aWheelState[i] == WHEEL_STATE_SPINNING) {
            wheelSpinRate += CVehicle::WHEELSPIN_TARGET_RATE;
        }

        m_fWheelsSuspensionCompressionPrev[i] = m_fWheelsSuspensionCompression[i];
        m_fWheelsSuspensionCompression[i] = 1.0f;
    }

    if (m_pHandlingData->GetTransmission().m_nDriveType == '4')
        wheelSpinRate *= 0.25f;
    else
        wheelSpinRate *= 0.5f;

    float riseFallWheelSpinRate = CVehicle::WHEELSPIN_FALL_RATE;
    if (wheelSpinRate >= m_fWheelSpinForAudio)
        riseFallWheelSpinRate = CVehicle::WHEELSPIN_RISE_RATE;

    riseFallWheelSpinRate = std::pow(riseFallWheelSpinRate, CTimer::GetTimeStep());
    m_fWheelSpinForAudio *= riseFallWheelSpinRate;
    m_fWheelSpinForAudio += (1.0f - riseFallWheelSpinRate) * wheelSpinRate;

    if ((CTimer::GetTimeInMS() & 2047) > 800) { // % 2046
        if (roughnessShake >= 0.29f)
            suspensionShake = 0.0f;
        roughnessShake = 0.0f;
    }

    if (!extraHandlingWheelsTouchingSand
        && (suspensionShake > 0.0f || roughnessShake > 0.0f)
        && m_nStatus == STATUS_PLAYER)
    {
        float speed = m_vecMoveSpeed.SquaredMagnitude();
        if (speed > 0.01f) {
            speed = sqrt(speed);
            if (suspensionShake > 0.0f) {
                float frequency = (200.0f * suspensionShake * speed * 2000.0f) / m_fMass + 100.0f;
                frequency = std::min(frequency, 250.0f);
                auto time = static_cast<int16>(CTimer::GetTimeStep() * 20000.0f / frequency);
                CPad::GetPad()->StartShake(time, static_cast<uint8>(frequency), 0);
            }
            else {
                float frequency = (200.0f * roughnessShake * speed * 2000.0f) / m_fMass + 40.0f;
                frequency = std::min(frequency, 150.0f);
                auto time = static_cast<int16>(CTimer::GetTimeStep() * 5000.0f / frequency);
                CPad::GetPad()->StartShake(time, static_cast<uint8>(frequency), 0);
            }
        }
    }

    vehicleFlags.bAudioChangingGear = false;
    vehicleFlags.bVehicleColProcessed = false;

    // 0x6B2F94
    if (!vehicleFlags.bWarnedPeds)
        CCarCtrl::ScanForPedDanger(this);

    if (handlingFlags.bHydraulicInst && m_vecMoveSpeed.Magnitude() < 0.2f) {
        auto& hydraulicData = CVehicle::m_aSpecialHydraulicData[m_vehicleSpecialColIndex];
        if (m_nStatus == STATUS_PHYSICS
            && (hydraulicData.m_aWheelSuspension[CAR_WHEEL_FRONT_LEFT] > 0.5f && hydraulicData.m_aWheelSuspension[CAR_WHEEL_REAR_LEFT] > 0.5f
            || hydraulicData.m_aWheelSuspension[CAR_WHEEL_FRONT_RIGHT] > 0.5f && hydraulicData.m_aWheelSuspension[CAR_WHEEL_REAR_RIGHT] > 0.5f
            )
            || m_nStatus == STATUS_PLAYER
            && m_pDriver
            && m_pDriver->IsPlayer()
            && std::fabs((float)m_pDriver->AsPlayer()->GetPadFromPlayer()->GetCarGunLeftRight()) > 50.0f
            && std::fabs((float)m_pDriver->AsPlayer()->GetPadFromPlayer()->GetCarGunUpDown()) < 50.0f
        ) {
            float turnSpeedForward = DotProduct(m_vecTurnSpeed, GetForward());
            const float speedSquared = turnSpeedForward * turnSpeedForward;
            float speedTimeStep = std::pow(0.985f, CTimer::GetTimeStep()) / (speedSquared * 5.0f + 1.0f);
            float speed = std::pow(speedTimeStep, CTimer::GetTimeStep());
            speed *= turnSpeedForward;
            speed -= turnSpeedForward;

            CVector force = speed * GetUp() * -1.0f * m_fTurnMass;
            CVector point = GetRight() + GetMatrix().TransformVector(m_vecCentreOfMass);
            ApplyTurnForce(force, point);
        }
    }

    if (m_pTowingVehicle) {
        if (m_nStatus == STATUS_IS_TOWED) {
            bool updateTractorLink = false; // TODO: rename this variable later
            if (m_pTowingVehicle->m_vecMoveSpeed != 0.0f || m_vecMoveSpeed.SquaredMagnitude() > 0.01f) {
                updateTractorLink = true;
                m_pTowingVehicle->UpdateTractorLink(false, false);
            }
            CVehicle::UpdateTrailerLink(false, false);
            if (m_pTowingVehicle && m_nStatus == STATUS_IS_TOWED) {
                if (updateTractorLink)
                    m_pTowingVehicle->UpdateTractorLink(false, true);
                UpdateTrailerLink(false, true);
            }
        }
        else {
            BreakTowLink();
        }
    }
    else if (m_pVehicleBeingTowed) {
        if (m_pVehicleBeingTowed->m_nStatus == STATUS_IS_TOWED) {
            if (m_pVehicleBeingTowed->m_pTowingVehicle == this) {
                RemoveFromMovingList();
                AddToMovingList();
            }
        }
        else {
            CEntity::ClearReference(m_pVehicleBeingTowed);
        }
    }

    // 0x6B32EF
    if (physicalFlags.bDisableCollisionForce && physicalFlags.bCollidable) {
        ResetMoveSpeed();
        ResetTurnSpeed();
        ResetFrictionMoveSpeed();
        ResetFrictionTurnSpeed();
    }
    else if (!skipPhysics
        && (m_fGasPedal == 0.0f || m_nStatus == STATUS_WRECKED)
        && std::fabs(m_vecMoveSpeed.x) < 0.0045f
        && std::fabs(m_vecMoveSpeed.y) < 0.0045f
        && std::fabs(m_vecMoveSpeed.z) < 0.0045f)
    {
        if (m_fDamageIntensity <= 0.0f || m_pDamageEntity != FindPlayerPed()) {
            if ((!IsSubPlane() || AsPlane()->m_fAccelerationBreakStatus == 0.0f) &&
                (!IsSubHeli() || AsHeli()->m_fAccelerationBreakStatus == 0.0f))
            {
                if (((float)m_wMiscComponentAngle == 0.0f || m_wMiscComponentAngle == m_wMiscComponentAnglePrev)
                    && !physicalFlags.bSubmergedInWater)
                {
                    if ((!m_pTowingVehicle || m_pTowingVehicle->m_vecMoveSpeed == 0.0f) && DidAnyWheelTouchGroundPrev()) {
                        if (m_nModelIndex == MODEL_VORTEX || CCheat::IsActive(CHEAT_CARS_ON_WATER)
                            && DidAnyWheelTouchShallowWaterGroundPrev())
                        {
                            m_nFakePhysics = 0;
                        }
                        else {
                            ResetMoveSpeed();
                            m_vecTurnSpeed.z = 0.0f;
                        }
                    }
                }
            }
        }
    }

    if (CWeather::Earthquake > 0.0f) {
        CVector& position = GetPosition();
        position.z = CGeneral::GetRandomNumberInRange(0.0f, CWeather::Earthquake * 0.5f);
    }

    if (m_nModelIndex == MODEL_COMBINE)
        ProcessHarvester();

    if (m_nModelIndex == MODEL_RHINO && (m_vecMoveSpeed != 0.0f || m_vecTurnSpeed != 0.0f)) {
        m_doors[DOOR_LEFT_REAR].m_fOpenAngle = 1.0f;
        m_doors[DOOR_LEFT_REAR].m_fClosedAngle = 1.0f;
        m_doors[DOOR_LEFT_REAR].m_fAngle = 1.0f;
        m_doors[DOOR_LEFT_REAR].m_fPrevAngle = 1.0f;
        m_doors[DOOR_RIGHT_REAR].m_fOpenAngle = 1.0f;
        m_doors[DOOR_RIGHT_REAR].m_fClosedAngle = 1.0f;
        m_doors[DOOR_RIGHT_REAR].m_fAngle = 1.0f;
        m_doors[DOOR_RIGHT_REAR].m_fPrevAngle = 1.0f;
    }
}

// 0x6A1ED0
CVector CAutomobile::AddMovingCollisionSpeed(CVector& point) {
    if (m_nStatus != STATUS_PLAYER && m_nStatus != STATUS_FORCED_STOP) {
        if (m_nCreatedBy != MISSION_VEHICLE || !m_wMiscComponentAngle && !m_wMiscComponentAnglePrev)
            return {};
    }

    uint16 angleDiff = m_wMiscComponentAngle - m_wMiscComponentAnglePrev;
    if (angleDiff > 100 || angleDiff < -100)
        return {};

    float colAngleMult = 0.0f;
    CVector colPivot{};
    RwFrame* carNodeMisc = nullptr;
    if (ModelIndices::IsDumper(m_nModelIndex))
    {
        carNodeMisc = m_aCarNodes[CAR_MISC_C];
        colAngleMult = CMonsterTruck::DUMPER_COL_ANGLEMULT;
    }
    else if (ModelIndices::IsPacker(m_nModelIndex)) {
        colAngleMult = PACKER_COL_ANGLE_MULT;
        colPivot = PACKER_COL_PIVOT;
    }
    else if (ModelIndices::IsDozer(m_nModelIndex)) {
        carNodeMisc = m_aCarNodes[CAR_MISC_A];
        if (carNodeMisc) {
            colAngleMult = DOZER_COL_ANGLE_MULT;
        }
    }
    else if (ModelIndices::IsAndromada(m_nModelIndex)) {
        carNodeMisc = m_aCarNodes[CAR_MISC_E];
        if (carNodeMisc) {
            colAngleMult = CPlane::ANDROM_COL_ANGLE_MULT;
        }
    }
    else if (ModelIndices::IsForklift(m_nModelIndex)) {
        float rot = float(angleDiff) / CTimer::GetTimeStep() * FORKLIFT_COL_ANGLE_MULT * 2.0f;
        return rot * GetUp();
    }

    if (colAngleMult == 0.0f)
        return {};

    if (carNodeMisc) {
        colPivot = RwFrameGetMatrix(carNodeMisc)->pos;
    }
    CVector rotation(float(angleDiff) / CTimer::GetTimeStep() * colAngleMult, 0.0f, 0.0f);
    CVector pos = GetMatrix().TransformVector(rotation);
    CVector pivotPos = GetMatrix().TransformVector(colPivot);
    CVector distance = point - pivotPos;
    return CrossProduct(pos, distance);
}

// 0x6B4800
bool CAutomobile::ProcessAI(uint32& extraHandlingFlags) {
    CColModel* colModel = GetColModel();
    CCollisionData* colData = colModel->m_pColData;
    int8 recordingId = this->m_autoPilot.m_vehicleRecordingId;
    m_autoPilot.carCtrlFlags.bHonkAtCar = false;
    m_autoPilot.carCtrlFlags.bHonkAtPed = false;
    if (recordingId >= 0 && !CVehicleRecording::bUseCarAI[recordingId])
        return false;

    eCarMission carMission = m_autoPilot.m_nCarMission;
    CVehicle* playerVehicle = FindPlayerVehicle();
    if (playerVehicle && playerVehicle != this && FindPlayerWanted()->m_nWantedLevel > 3
        && (carMission == MISSION_RAMPLAYER_FARAWAY
            || carMission == MISSION_BLOCKPLAYER_FARAWAY
            || carMission == MISSION_RAMPLAYER_CLOSE
            || carMission == MISSION_BLOCKPLAYER_CLOSE)
        && FindPlayerSpeed().Magnitude() > 0.3f
    ) {
        extraHandlingFlags |= EXTRA_HANDLING_PERFECT;
        if (FindPlayerSpeed().Magnitude() > 0.4f && m_vecMoveSpeed.Magnitude() < 0.3f) {
            extraHandlingFlags |= EXTRA_HANDLING_NITROS;
        } else {
            if (DistanceBetweenPoints(FindPlayerCoors(), GetPosition()) > 50.0f)
                extraHandlingFlags |= EXTRA_HANDLING_NITROS;
        }
    } else if (m_nModelIndex == MODEL_RCBANDIT && m_nStatus != STATUS_REMOTE_CONTROLLED) {
        extraHandlingFlags |= EXTRA_HANDLING_PERFECT;
    }

    bool extraPerfectHandling = !!(extraHandlingFlags & EXTRA_HANDLING_PERFECT);
    if (extraPerfectHandling || CCheat::IsActive(CHEAT_PERFECT_HANDLING)) {
        m_vecCentreOfMass.z = m_aSuspensionSpringLength[CAR_WHEEL_FRONT_LEFT] * 0.3f - m_fFrontHeightAboveRoad;
    }
    else if (m_nStatus == STATUS_PHYSICS) {
        if (handlingFlags.bHydraulicGeom) {
            if (m_autoPilot.m_nCarMission != MISSION_NONE && colData && colData->m_nNumLines > 0) {
                m_vecCentreOfMass.y = (colData->m_pLines[0].m_vecStart.y + colData->m_pLines[1].m_vecStart.y)  * 0.5f;
                handlingFlags.bNpcNeutralHandl = true;
            }
            else {
                m_vecCentreOfMass = m_pHandlingData->m_vecCentreOfMass;
                handlingFlags.bNpcNeutralHandl = false;
            }
        }

        if (handlingFlags.bNpcAntiRoll) {
            m_pHandlingData->m_vecCentreOfMass.z += (colModel->m_boundBox.m_vecMin.z - m_pHandlingData->m_vecCentreOfMass.z) * 0.4f;
        }
    }
    else {
        m_vecCentreOfMass = m_pHandlingData->m_vecCentreOfMass;
    }

    if (m_nStatus != STATUS_ABANDONED && m_nStatus != STATUS_WRECKED
        && m_nStatus != STATUS_PLAYER && m_nStatus != STATUS_REMOTE_CONTROLLED
        && m_nStatus != STATUS_FORCED_STOP && vehicleFlags.bIsLawEnforcer)
    {
        ScanForCrimes();
    }

    if (vehicleFlags.bCanPark
        && !vehicleFlags.bParking
        && m_nCreatedBy != MISSION_VEHICLE
        && m_autoPilot.m_nCarMission == MISSION_CRUISE
        && ((CTimer::GetFrameCounter() + static_cast<uint8>(m_nRandomSeed)) & 0xF) == 0)
    {
        if (m_nModelIndex != MODEL_TAXI && m_nModelIndex != MODEL_CABBIE) {
            CVector target = GetPosition();
            target += GetRight() * 3.0f + GetForward() * 10.0f;
            CColPoint cp{};
            CEntity* hit;
            if (!CWorld::ProcessLineOfSight(GetPosition(), target, cp, hit, true, true, true, false, false, false, false, false) || hit == this) {
                CCarAI::GetCarToParkAtCoors(this, target);
            }
        }
    }

    bool isRemotelyControlledByPlayer = false;
    switch (m_nStatus) {
    case STATUS_SIMPLE:
    {
        CCarAI::UpdateCarAI(this);
        CPhysical::ProcessControl();
        CCarCtrl::UpdateCarOnRails(this);
        m_nNumContactWheels = 4;
        m_wheelsOnGrounPrev = m_nWheelsOnGround;
        m_nWheelsOnGround = 4;
        float speed = m_autoPilot.m_speed / 50.0f;
        m_pHandlingData->GetTransmission().CalculateGearForSimpleCar(speed, m_nCurrentGear);
        float wheelRot = CVehicle::ProcessWheelRotation(WHEEL_STATE_NORMAL, GetForward(), m_vecMoveSpeed, 0.35f);
        for (float& rotation : m_wheelRotation) {
            rotation += wheelRot;
        }
        PlayHornIfNecessary();
        ReduceHornCounter();
        vehicleFlags.bAudioChangingGear = false;
        vehicleFlags.bVehicleColProcessed = false;
        return true;
    }
    case STATUS_PHYSICS:
    case STATUS_GHOST:
    {
        CCarAI::UpdateCarAI(this);
        CCarCtrl::SteerAICarWithPhysics(this);
        CCarCtrl::ReconsiderRoute(this);
        PlayHornIfNecessary();
        if (vehicleFlags.bIsBeingCarJacked) {
            m_fGasPedal = 0.0f;
            m_fBreakPedal = 1.0f;
            vehicleFlags.bIsHandbrakeOn = true;
        }
        if (!IsAnyWheelTouchingSand())
            return false;
        if (m_nModelIndex == MODEL_RCBANDIT || m_nModelIndex == MODEL_SANDKING || m_nModelIndex == MODEL_BFINJECT)
            return false;
        autoFlags.bIsBoggedDownInSand = true;
        float force = m_fMass * CTimer::GetTimeStep() * 0.005f;
        if (CWeather::WetRoads > 0.0f)
            force *= 1.0f - CWeather::WetRoads;
        ApplyMoveForce(-force * m_vecMoveSpeed);
        return false;
    }
    case STATUS_ABANDONED:
        for (auto& collisionEntity : m_apWheelCollisionEntity) {
            if (collisionEntity) {
                vehicleFlags.bRestingOnPhysical = true;
                break;
            }
        }

        if (IsSubHeli() || IsSubPlane() && m_vecMoveSpeed.SquaredMagnitude() < 0.1f)
            m_fBreakPedal = 1.0f;

        if (vehicleFlags.bRestingOnPhysical)
            m_fBreakPedal = 0.5f;
        else if (m_vecMoveSpeed.SquaredMagnitude() < 0.01f)
            m_fBreakPedal = 0.2f;
        else
            m_fBreakPedal = 0.0f;

        vehicleFlags.bIsHandbrakeOn = false;
        m_fSteerAngle = 0.0f;
        m_fGasPedal = 0.0f;

        if (CanUpdateHornCounter())
            m_nHornCounter = 0;

        if (!vehicleFlags.bIsBeingCarJacked)
            return false;

        vehicleFlags.bIsHandbrakeOn = true;
        m_fGasPedal = 0.0f;
        m_fBreakPedal = 1.0f;
        return false;
    case STATUS_WRECKED:
        m_fBreakPedal = 0.05f;
        vehicleFlags.bIsHandbrakeOn = true;
        m_fSteerAngle = 0.0f;
        m_fGasPedal = 0.0f;
        if (!CanUpdateHornCounter())
            return false;
        m_nHornCounter = 0;
        return false;
    case STATUS_REMOTE_CONTROLLED:
        if (CPad::GetPad()->CarGunJustDown() && !CVehicle::bDisableRemoteDetonation) {
            BlowUpCar(FindPlayerPed(), false);
            CRemote::TakeRemoteControlledCarFromPlayer(true);
        }

        if (m_nModelIndex == MODEL_RCBANDIT && !CVehicle::bDisableRemoteDetonationOnContact) {
            //FindPlayerCoors(); // return value is ignored
            if (RcbanditCheckHitWheels()|| physicalFlags.bSubmergedInWater) {
                CRemote::TakeRemoteControlledCarFromPlayer(true);
                BlowUpCar(FindPlayerPed(), false);
            }
        }
        if (FindPlayerInfo().m_pRemoteVehicle == this)
            isRemotelyControlledByPlayer = true;
        break;
    case STATUS_FORCED_STOP:
        if (m_vecMoveSpeed.SquaredMagnitude() < 0.01f
            || m_pDriver
            && m_pDriver->IsPlayer()
            && (m_pDriver->m_nPedState == PEDSTATE_ARRESTED || m_pDriver->GetTaskManager().HasAnyOf<TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT, TASK_COMPLEX_CAR_QUICK_BE_DRAGGED_OUT, TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN>()))
        {
            m_fBreakPedal = 1.0f;
            m_fGasPedal = 0.0f;
            vehicleFlags.bIsHandbrakeOn = true;
        }
        else
        {
            m_fBreakPedal = 0.0f;
            vehicleFlags.bIsHandbrakeOn = false;
        }

        m_fSteerAngle = 0.0f;
        m_fGasPedal = 0.0f;
        if (!CanUpdateHornCounter())
            return false;
        m_nHornCounter = 0;
        return false;
    case STATUS_IS_TOWED:
        vehicleFlags.bIsHandbrakeOn = false;
        m_fBreakPedal = 0.0f;
        m_fSteerAngle = 0.0f;
        m_fGasPedal = 0.0f;
        if (!m_pTowingVehicle)
            BreakTowLink();
        return false;
    }

    if (m_nStatus != STATUS_PLAYER && m_nStatus != STATUS_REMOTE_CONTROLLED)
        return false;

    bool processControlInput = isRemotelyControlledByPlayer;
    if (!processControlInput && m_pDriver)
    {
        ePedState pedState = m_pDriver->m_nPedState;
        if (pedState != PEDSTATE_EXIT_CAR && pedState != PEDSTATE_DRAGGED_FROM_CAR && pedState != PEDSTATE_ARRESTED)
            processControlInput = true;
    }
    if (!processControlInput)
        return false;

    CPad* pad = nullptr;
    if (m_pDriver) {
        pad = m_pDriver->AsPlayer()->GetPadFromPlayer();
    }

    PruneReferences();
    if (isRemotelyControlledByPlayer) {
        int32 playerSlot = CWorld::FindPlayerSlotWithRemoteVehiclePointer(this);
        if (playerSlot >= 0)
            ProcessControlInputs(playerSlot);
    }
    else if (m_pDriver->IsPlayer()) { // 0x6B4CD4
        ProcessControlInputs(static_cast<uint8>(m_pDriver->m_nPedType));
    }

    if (m_nStatus == STATUS_PLAYER && !IsSubHeli()) {
        if (m_nModelIndex == MODEL_VORTEX || !IsSubPlane()) {
            if (m_nModelIndex != MODEL_SWATVAN && m_nModelIndex != MODEL_RHINO) {
                DoDriveByShootings();
            }
        }
    }

    auto carLess3WheelCounter = FindPlayerInfo().m_nCarLess3WheelCounter;
    m_vecCentreOfMass.z = m_pHandlingData->m_vecCentreOfMass.z;
    if (carLess3WheelCounter > 500 && !IsSubHeli() && !IsSubPlane()) {
        carLess3WheelCounter = std::min(carLess3WheelCounter - 500u, 1000u);
        float fCarLess3WheelCounter = static_cast<float>(carLess3WheelCounter) * 0.002f;
        const float boundMaxZ = colModel->m_boundBox.m_vecMax.z;
        if (GetUp().z > -0.4f && pad) {
            if (GetRight().z <= 0.0f)
                fCarLess3WheelCounter *= -1.0f;

            float steerLeftRight = (float)pad->GetSteeringLeftRight() / 128.0f;
            m_vecCentreOfMass.z += steerLeftRight * CAR_BALANCE_MULT * boundMaxZ * fCarLess3WheelCounter;
        }
        else {
            m_vecCentreOfMass.z -= CAR_BALANCE_MULT * boundMaxZ * 0.5f;
        }
    }

    if (!pad || m_nNumContactWheels != 0 || IsSubHeli() || IsSubPlane()) {
        DoSoftGroundResistance(extraHandlingFlags);
        return false;
    }

    float carWeightMult = 0.0007f;
    if (IsSubMonsterTruck())
        carWeightMult = 0.0025f;
    else
        carWeightMult *= CStats::GetFatAndMuscleModifier(STAT_MOD_DRIVING_SKILL);
    carWeightMult *= std::min(1.0f, 3000.0f / m_fTurnMass) * m_fTurnMass;

    float steerLeftRight = (float)pad->GetSteeringLeftRight() / 128.0f;
    float steerUpDown    = (float)pad->GetSteeringUpDown() / 128.0f;
    if (CCamera::m_bUseMouse3rdPerson && std::fabs(steerLeftRight) < 0.05f && std::fabs(steerUpDown) < 0.05f) {
        steerLeftRight = std::clamp<float>(CPad::NewMouseControllerState.X / 50.0f, -1.5f, 1.5f);
        steerUpDown    = std::clamp<float>(CPad::NewMouseControllerState.Y / 50.0f, -1.5f, 1.5f);
    }

    // 0x6B4F69
    if (pad->GetHandBrake()) {
        float dotTurnUp = DotProduct(m_vecTurnSpeed, GetUp());
        if (dotTurnUp < 0.02f && steerLeftRight < 0.0f || dotTurnUp > -0.02f && steerLeftRight > 0.0f) {
            CVector point = m_vecCentreOfMass + GetForward();
            CVector force = GetRight() * steerLeftRight * carWeightMult * CTimer::GetTimeStep();
            ApplyTurnForce(force, point);
        }
    }
    else if (!pad->GetAccelerate()) {
        float dotTurnForward = DotProduct(m_vecTurnSpeed, GetForward());
        if (dotTurnForward < 0.02f && steerLeftRight < 0.0f
            || dotTurnForward > -0.02f && steerLeftRight > 0.0f)
        {
            CVector point = m_vecCentreOfMass + GetUp();
            CVector force = GetRight() * steerLeftRight * carWeightMult * CTimer::GetTimeStep();
            ApplyTurnForce(force, point);
        }
    }

    if (!pad->GetAccelerate()) {
        float dotTurnRight = DotProduct(m_vecTurnSpeed, GetRight());
        if (dotTurnRight < 0.02f && steerUpDown < 0.0f
            || dotTurnRight > -0.02f && steerUpDown > 0.0f)
        {
            CVector point = m_vecCentreOfMass + GetForward();
            CVector force = GetUp() * steerUpDown * carWeightMult * CTimer::GetTimeStep();
            ApplyTurnForce(force, point);
        }
    }
    DoSoftGroundResistance(extraHandlingFlags);
    return false;
}

// 0x6A2AE0
void CAutomobile::ResetSuspension()
{
    for (int32 i = 0; i < 4; i++) {
        m_fWheelsSuspensionCompression[i] = 1.0f;
        m_aWheelTimer[i] = 0.0f;
        m_wheelRotation[i] = 0.0f;
        m_aWheelState[i] = WHEEL_STATE_NORMAL;
    }
}

// 0x6A8500
void CAutomobile::ProcessFlyingCarStuff()
{
    if (m_nStatus == STATUS_PLAYER || m_nStatus == STATUS_REMOTE_CONTROLLED || m_nStatus == STATUS_PHYSICS) {
        if (CCheat::IsActive(CHEAT_CARS_FLY)
            && m_vecMoveSpeed.Magnitude() > 0.0f
            && CTimer::GetTimeStep() > 0.0f
        ) {
            FlyingControl(FLIGHT_MODEL_PLANE, -9999.9902F, -9999.9902F, -9999.9902F, -9999.9902F);
        }
    }
}

// 0x6A45C0
void CAutomobile::DoHoverSuspensionRatios()
{
    if (GetUp().z >= 0.3f && !vehicleFlags.bIsDrowning) {
        auto colData = GetColModel()->m_pColData;
        for (int32 i = 0; i < 4; i++)  {
            CColLine& line = colData->m_pLines[i];
            CVector start = m_matrix->TransformPoint(line.m_vecStart);
            CVector end = m_matrix->TransformPoint(line.m_vecEnd);
            float colPointZ = MAP_Z_LOW_LIMIT;
            if (m_fWheelsSuspensionCompression[i] < 1.0f) {
                colPointZ = m_wheelColPoint[i].m_vecPoint.z;
            }
            float waterLevel = 0.0f;
            if (CWaterLevel::GetWaterLevel(end.x, end.y, end.z, waterLevel, false, nullptr)
                && waterLevel > colPointZ && end.z - 1.0f < waterLevel)
            {
                if (waterLevel <= end.z)
                    m_fWheelsSuspensionCompression[i] = 1.0f;
                else if (waterLevel <= start.z)
                    m_fWheelsSuspensionCompression[i] = (start.z - waterLevel) / (start.z - end.z);
                else
                    m_fWheelsSuspensionCompression[i] = 0.0f;

                CVector point = (end - start) * m_fWheelsSuspensionCompression[i];
                point += start;
                point.z = waterLevel;
                m_wheelColPoint[i].m_vecPoint = point;
                m_wheelColPoint[i].m_vecNormal = CVector(0.0f, 0.0f, 1.0f);
            }
        }
    }
}

// 0x6AFB10
void CAutomobile::ProcessSuspension() {
    float springLength[4]{};
    float wheelSpringForceDampingLimits[4]{};
    for (int32 i = 0; i < 4; i++) {
        springLength[i] = m_fWheelsSuspensionCompression[i];
        wheelSpringForceDampingLimits[i] = 0.0f;
    }

    float forwardSpeed = std::fabs(DotProduct(m_vecMoveSpeed, GetForward()));

    int32 numWheelLoops = 1;
    if (ModelIndices::IsRhino(m_nModelIndex) && CReplay::Mode != MODE_PLAYBACK) {
        numWheelLoops = 3;
    }

    CVector contactPoints[4]{};
    CVector directions[4]{};
    for (int32 wheelLoopIndex = 0; wheelLoopIndex < numWheelLoops; wheelLoopIndex++) {
        for (int32 i = 0; i < 4; i++) {
            directions[i] = GetUp() * -1.0f;
            if (springLength[i] < 1.0f) {
                contactPoints[i] = m_wheelColPoint[i].m_vecPoint - GetPosition();
            } else {
                contactPoints[i].Set(0.f, 0.f, 0.f);
            }
        }

        for (int32 i = 0; i < 4; i++) {
            if (springLength[i] >= 1.0f)
                continue;

            float suspensionBias = m_pHandlingData->m_fSuspensionBiasBetweenFrontAndRear;
            if (i == CAR_WHEEL_REAR_LEFT || i == CAR_WHEEL_REAR_RIGHT)
                suspensionBias = 1.0f - suspensionBias;

            float fSuspensionForceLevel = m_pHandlingData->m_fSuspensionForceLevel;
            if (handlingFlags.bHydraulicGeom && handlingFlags.bHydraulicInst) {
                if (handlingFlags.bNpcNeutralHandl && m_nStatus == STATUS_PHYSICS) {
                    suspensionBias = 0.5f;
                }
                if (std::fabs(forwardSpeed) < 0.15f) {
                    fSuspensionForceLevel *= 1.5f;
                }
            }

            if (ModelIndices::IsVortex(m_nModelIndex)) {
                fSuspensionForceLevel *= std::fabs(AsPlane()->m_fAccelerationBreakStatus) * 0.25f + 1.0f;
            }

            if (CCheat::IsActive(CHEAT_CARS_ON_WATER) || ModelIndices::IsVortex(m_nModelIndex))
                ApplySpringCollision(
                    fSuspensionForceLevel,
                    directions[i],
                    contactPoints[i],
                    springLength[i],
                    suspensionBias,
                    wheelSpringForceDampingLimits[i]
                );
            else
                ApplySpringCollisionAlt(
                    fSuspensionForceLevel,
                    directions[i],
                    contactPoints[i],
                    springLength[i],
                    suspensionBias,
                    m_wheelColPoint[i].m_vecNormal,
                    wheelSpringForceDampingLimits[i]
                );
        }

        CVector contactSpeeds[4]{};
        for (int32 i = 0; i < 4; ++i) {
            contactSpeeds[i] = CPhysical::GetSpeed(contactPoints[i]);
            if (m_apWheelCollisionEntity[i])
                contactSpeeds[i] -= m_apWheelCollisionEntity[i]->GetSpeed(m_vWheelCollisionPos[i]);

            if (m_fWheelsSuspensionCompression[i] < 1.0f && m_wheelColPoint[i].m_vecNormal.z > 0.35f) {
                directions[i] = -m_wheelColPoint[i].m_vecNormal;
            }
        }

        for (int32 i = 0; i < 4; ++i) {
            float dampingForce = m_pHandlingData->m_fSuspensionDampingLevel;
            if (handlingFlags.bHydraulicInst && dampingForce > 0.1f && fabs(forwardSpeed) < 0.15f)
                dampingForce = 0.1f;

            if (springLength[i] < 1.0f && !autoFlags.bIsMonsterTruck)
                ApplySpringDampening(
                    dampingForce,
                    wheelSpringForceDampingLimits[i],
                    directions[i],
                    contactPoints[i],
                    contactSpeeds[i]
                );
        }

        if (!ModelIndices::IsRhino(m_nModelIndex))
            continue;

        if (numWheelLoops <= 1 || wheelLoopIndex > 1)
            break;

        int32 wheelLineIndices[4]{};
        if (wheelLoopIndex == 0) {
            wheelLineIndices[CAR_WHEEL_FRONT_LEFT] = 4;
            wheelLineIndices[CAR_WHEEL_REAR_LEFT] = 7;
            wheelLineIndices[CAR_WHEEL_FRONT_RIGHT] = 8;
            wheelLineIndices[CAR_WHEEL_REAR_RIGHT] = 11;

            springLength[CAR_WHEEL_FRONT_LEFT]  = m_doors[DOOR_LEFT_REAR].m_fOpenAngle;
            springLength[CAR_WHEEL_REAR_LEFT]   = m_doors[DOOR_LEFT_REAR].m_fPrevAngle;
            springLength[CAR_WHEEL_FRONT_RIGHT] = m_doors[DOOR_RIGHT_REAR].m_fOpenAngle;
            springLength[CAR_WHEEL_REAR_RIGHT]  = m_doors[DOOR_RIGHT_REAR].m_fPrevAngle;
        }
        else if (wheelLoopIndex == 1) {
            springLength[CAR_WHEEL_FRONT_LEFT]  = m_doors[DOOR_LEFT_REAR].m_fClosedAngle;
            springLength[CAR_WHEEL_REAR_LEFT]   = m_doors[DOOR_LEFT_REAR].m_fAngle;
            springLength[CAR_WHEEL_FRONT_RIGHT] = m_doors[DOOR_RIGHT_REAR].m_fClosedAngle;
            springLength[CAR_WHEEL_REAR_RIGHT]  = m_doors[DOOR_RIGHT_REAR].m_fAngle;

            wheelLineIndices[CAR_WHEEL_FRONT_LEFT]  = 5;
            wheelLineIndices[CAR_WHEEL_REAR_LEFT]   = 6;
            wheelLineIndices[CAR_WHEEL_FRONT_RIGHT] = 9;
            wheelLineIndices[CAR_WHEEL_REAR_RIGHT]  = 10;
        }

        CCollisionData* colData = GetColModel()->m_pColData;
        float wheelsSuspensionCompressionPrev[4]{};
        for (int32 i = 0; i < 4; i++) {
            CColLine& colLine = colData->m_pLines[wheelLineIndices[i]];
            contactPoints[i]  = colLine.m_vecStart;
            contactPoints[i].z -= m_aSuspensionLineLength[i] * springLength[i];
            contactPoints[i] = GetMatrix().TransformVector(contactPoints[i]);

            auto springTension = 1.f - (m_aSuspensionSpringLength[i] / m_aSuspensionLineLength[i]);
            springLength[i]    = (springLength[i] - springTension) / (1.f - springTension);
        }
    }

    float speedThreshold = 0.02f;
    float rollOnToWheelsForce = ROLL_ONTO_WHEELS_FORCE;
    if (m_nStatus != STATUS_PLAYER && m_nStatus != STATUS_REMOTE_CONTROLLED) {
        speedThreshold *= 2.0f;
        rollOnToWheelsForce *= 2.0f;
    }

    if (forwardSpeed < speedThreshold) {
        float fDir = 0.0f;
        if (m_fWheelsSuspensionCompression[CAR_WHEEL_FRONT_LEFT] == 1.0f
            && m_fWheelsSuspensionCompression[CAR_WHEEL_REAR_LEFT] == 1.0f
            && (m_fWheelsSuspensionCompression[CAR_WHEEL_FRONT_RIGHT] < 1.0f
                || m_fWheelsSuspensionCompression[CAR_WHEEL_REAR_RIGHT] < 1.0f))
        {
            fDir = 1.0f;
        }
        else if (forwardSpeed < speedThreshold
            && m_fWheelsSuspensionCompression[CAR_WHEEL_FRONT_RIGHT] == 1.0f
            && m_fWheelsSuspensionCompression[CAR_WHEEL_REAR_RIGHT] == 1.0f
            && (m_fWheelsSuspensionCompression[CAR_WHEEL_FRONT_LEFT] < 1.0f
                || m_fWheelsSuspensionCompression[CAR_WHEEL_REAR_LEFT] < 1.0f))
        {
            fDir = -1.0f;
        }

        if (fDir != 0.0f) {
            CVector right = CrossProduct(GetForward(), CVector(0.0f, 0.0f, 1.0f));
            if (fabs(DotProduct(right, GetRight())) < 0.6f) {
                auto colModel = GetVehicleModelInfo()->GetColModel();
                CVector force = GetUp() * fDir * rollOnToWheelsForce * m_fTurnMass;
                CVector point = GetRight() * colModel->GetBoundingBox().m_vecMax.x;
                ApplyTurnForce(force, point);
                ApplyMoveForce(-right * rollOnToWheelsForce * m_fMass * fDir);
            }
        }
    }
}

// 0x6ACE70
int32 CAutomobile::ProcessEntityCollision(CEntity* entity, CColPoint* outColPoints) {
    if (m_nStatus != STATUS_SIMPLE) {
        vehicleFlags.bVehicleColProcessed = true;
    }

    const auto tcd = GetColData(),
               ocd = entity->GetColData();

#ifdef FIX_BUGS
    // FIX_BUGS@CAutomobile::ProcessEntityCollision:1
    // The original code handled this properly, because `ProcessColModels` returned `0` 
    // if either colmodel's data was missing
    // but there's a lot of no-op stuff done below that we can just avoid altogether
    // Though, in the original code there was an edge case if `m_pTractor == entity || m_pTrailer == entity` => crash
    // but since I moved the assignment to the outside of the function now it always crashes xD
    if (!tcd || !ocd) {
        return 0;
    }
#endif

    // The Rhinosaurus Rex has extra wheels (8 of them)
    std::array<float, MAX_NUM_SUSP_LINES> wheelColPtsTouchDists{};
    rng::copy(m_fWheelsSuspensionCompression, wheelColPtsTouchDists.begin());

    const auto originalWheelSuspCompr{m_fWheelsSuspensionCompression};

    // ...and their suspension compression values are stored inside doors (fml pls)
    const auto GetRhinosaurusRexExtraWheelSuspCompression = [this](size_t extraWheelIdx) -> float& { // 
        auto& door = m_doors[4 + extraWheelIdx / 4];
        switch (extraWheelIdx % 4) {
        case 0: return door.m_fOpenAngle;
        case 1: return door.m_fClosedAngle;
        case 2: return door.m_fAngle;
        case 3: return door.m_fPrevAngle;
        default: NOTSA_UNREACHABLE();
        }
    };

    // 0x6ACEF7
    if (m_nModelIndex == eModelID::MODEL_RHINO) {
        // Store shit for the rhinosaurus rex
        for (auto i = 0; i < NUM_RHINO_EXTRA_SUSP_LINES; i++) {
            wheelColPtsTouchDists[NUM_AUTOMOBILE_SUSP_LINES + i] = GetRhinosaurusRexExtraWheelSuspCompression(i);
        }
    }

    const auto tNumLines = tcd->m_nNumLines;
    if (   physicalFlags.bSkipLineCol
        || physicalFlags.bProcessingShift
        || entity->IsPed()
        || m_nModelIndex == (uint16)eModelID::UNLOAD_MODEL && entity->IsVehicle()
    ) {
        tcd->m_nNumLines = 0; // Later reset back to original value
    }

    // Hide triangles in some cases
    const auto didHideTriangles = m_pTowingVehicle == entity || m_pVehicleBeingTowed == entity;
    const auto tNumTri = tcd->m_nNumTriangles, // Saving my sanity here, and unconditionally assigning
               oNumTri = ocd->m_nNumTriangles;
    if (didHideTriangles) {
        tcd->m_nNumTriangles = ocd->m_nNumTriangles = 0;
    }

    // For ghosts we dont do shit (In case this garbage is a forklift this value is modified below)
    auto numColPts = m_nStatus == STATUS_GHOST
        ? 0
        : CCollision::ProcessColModels(
            GetMatrix(), *GetColModel(),
            entity->GetMatrix(), *entity->GetColModel(),
            *(std::array<CColPoint, 32>*)(outColPoints),
            aAutomobileColPoints.data(),
            wheelColPtsTouchDists.data(),
            false
        );

    // Restore hidden triangles
    if (didHideTriangles) {
        tcd->m_nNumTriangles = tNumTri;
        ocd->m_nNumTriangles = oNumTri;
    }

    size_t numProcessedWheels{};
    if (tcd->m_nNumLines) {
        // Process the real wheels
        for (auto i = 0; i < MAX_CARWHEELS; i++) {
            // 0x6AD0D4
            const auto& cp = aAutomobileColPoints[i];

            const auto wheelColPtsTouchDist = wheelColPtsTouchDists[i];
            if (wheelColPtsTouchDist >= 1.f || wheelColPtsTouchDist >= m_fWheelsSuspensionCompression[i]) {
                continue;
            }

            numProcessedWheels++;

            m_fWheelsSuspensionCompression[i] = wheelColPtsTouchDist;
            m_wheelColPoint[i] = cp;

            m_anCollisionLighting[i] = cp.m_nLightingB;
            m_nContactSurface = cp.m_nSurfaceTypeB;

            switch (entity->GetType()) {
            case ENTITY_TYPE_VEHICLE:
            case ENTITY_TYPE_OBJECT: {
                CEntity::ChangeEntityReference(m_apWheelCollisionEntity[i], entity->AsPhysical());

                m_vWheelCollisionPos[i] = cp.m_vecPoint - entity->GetPosition();
                if (entity->IsVehicle()) {
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

        // Deal with extra wheels for the Rhinosaurus Rex
        if (m_nModelIndex == eModelID::MODEL_RHINO) {
            for (auto i = 0; i < NUM_RHINO_EXTRA_SUSP_LINES; i++) {
                auto& compr = GetRhinosaurusRexExtraWheelSuspCompression(i);
                compr = std::min(wheelColPtsTouchDists[NUM_AUTOMOBILE_SUSP_LINES + i], compr);
            }
        }
    } else {
        tcd->m_nNumLines = tNumLines;
    }
    
    // 0x6AD38A - Remove a few specific cp's for forklifts
    if (m_nModelIndex == eModelID::MODEL_FORKLIFT && entity->IsObject() && !m_wMiscComponentAngle && !m_wMiscComponentAnglePrev) {
        auto pts = aAutomobileColPoints | rng::views::take(numColPts);
        auto newEndIt = rng::remove_if(pts, [](CColPoint& cp) {
            return cp.m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT;
        });
        numColPts = rng::distance(pts.begin(), newEndIt.begin()); // Cancer
    }

    if (numColPts <= 0 && numProcessedWheels <= 0) {
        goto rtn;
    }

    AddCollisionRecord(entity);
    if (!entity->IsBuilding()) {
        entity->AsPhysical()->AddCollisionRecord(this);
    }

    if (numColPts > 0) {
        if (   entity->IsBuilding()
            || (entity->IsObject() && entity->AsPhysical()->physicalFlags.bDisableCollisionForce)
        ) {
            m_bHasHitWall = true;
        }
    }

    // 0x6AD482 - Add additional output colpoints for wheels
    if (numProcessedWheels > 0 && entity->IsBuilding() && m_pHandlingData->m_fSuspensionHighSpdComDamp > 0.f) {
        for (auto i = 0; i < MAX_CARWHEELS; i++) {
            if (m_damageManager.GetWheelStatus((eCarWheel)i) != eCarWheelStatus::WHEEL_STATUS_OK) { // Move to top
                continue;
            }
            const auto compressionNow = std::min(
                originalWheelSuspCompr[i],
                lerp(m_fWheelsSuspensionCompressionPrev[i], 1.f, 1.f - m_aSuspensionSpringLength[i] / m_aSuspensionLineLength[i])
            );
            const auto suspComprDelta = (compressionNow - m_fWheelsSuspensionCompression[i]) * m_aSuspensionLineLength[i]; // Suspension compression delta (In meters)
            if (suspComprDelta <= 0.1f /* 0x8D3200 */) {
                continue;
            }
            const auto& wheelcp = m_wheelColPoint[i];
            if (g_surfaceInfos.GetAdhesionGroup(wheelcp.m_nSurfaceTypeB) == ADHESION_GROUP_SAND || wheelcp.m_nSurfaceTypeB == SURFACE_RAILTRACK) {
                continue;
            }
            auto& outcp = outColPoints[numColPts];
            outcp = wheelcp;
            outcp.m_fDepth = (
                  std::abs(wheelcp.m_vecNormal.Dot(GetUp()))
                * std::min(std::abs(GetMoveSpeed().Dot(GetForward())), 0.3f) // Forward speedsq
                * std::min(0.2f, suspComprDelta)
                * m_pHandlingData->m_fSuspensionHighSpdComDamp
            ) / 0.3f;
            outcp.m_nPieceTypeA = CarWheelToCarPiece((eCarWheel)i);
            outcp.m_nSurfaceTypeA = outcp.m_nSurfaceTypeB = SURFACE_WHEELBASE;
            if (numColPts + 1 < 32) { // TODO/NOTE: `32` is ProcessColModels output array size
#ifdef FIX_BUGS
                numColPts++;
                break;
#endif
            }
        }
    }

rtn:
    return numColPts;
}

// 0x6A29C0
void CAutomobile::ProcessControlCollisionCheck(bool applySpeed) {
    m_bIsStuck = false;

    const CMatrix ogmat = GetMatrix(); // Save original matrix (We need it later)

    SkipPhysics();

    physicalFlags.bSkipLineCol = physicalFlags.bProcessingShift = false;

    m_fMovingSpeed = 0.f;
    rng::fill(m_fWheelsSuspensionCompression, 1.f);
    
    if (applySpeed) {
        ApplyMoveSpeed();
        ApplyTurnSpeed();
        for (auto tr = 0; CheckCollision() && tr < 5; tr++) { // tr = tries
            GetMatrix() = ogmat; // reset matrix
            ApplyMoveSpeed();
            ApplyTurnSpeed();
        }
    } else {
        const auto ogUsesCollision = m_bUsesCollision;
        m_bUsesCollision = false;
        (void)CheckCollision();
        m_bUsesCollision = ogUsesCollision; // restore
    }

    m_bIsStuck          = false;
    m_bIsInSafePosition = true;
}

// 0x6AD690
void CAutomobile::ProcessControlInputs(uint8 playerNum) {
    const auto plyrpad = CPad::GetPad((int32)playerNum);
    const auto plyrdriver = m_pDriver; // I guess in case of the RC Bandit for example the player isn't sitting in the plyrveh, but can control it, so this might be null!

    //> 0x6AD6CB - Update handbrake state
    const auto automaticallyHandBrake = [&, this] {
        if (plyrdriver) {
            auto& tm = plyrdriver->GetTaskManager();
            if (!tm.Has<TASK_SIMPLE_CAR_JUMP_OUT>()) {
                if (plyrpad->GetExitVehicle()) {
                    return true;
                }
                if (plyrdriver->m_nPedState == PEDSTATE_ARRESTED) {
                    return true;
                }
                if (tm.HasAnyOf<TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT, TASK_COMPLEX_CAR_QUICK_BE_DRAGGED_OUT, TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN>()) {
                    return true;
                }
            }
        }
        return false;
    }();
    vehicleFlags.bIsHandbrakeOn = automaticallyHandBrake || plyrpad->GetHandBrake() != 0;

    //> 0x6AD7BD - Update steer angle (And  possible control input too)
    std::tie(m_fRawSteerAngle, m_nLastControlInput) = [&, this]() -> std::pair<float, eControllerType> {
        // Calculates steering delta for this frame
        const auto GetSteeringDeltaForFrame = [&, this] {
            return (-(float)plyrpad->GetSteeringLeftRight() / 128.f - m_fRawSteerAngle) / 5.f * CTimer::GetTimeStep();
        };
    
        if (!CCamera::m_bUseMouse3rdPerson || !m_bEnableMouseSteering) {
            return { m_fRawSteerAngle + GetSteeringDeltaForFrame(), CONTROLLER_KEYBOARD1 };
        }

        if (CPad::NewMouseControllerState.X == 0.f && (m_nLastControlInput != CONTROLLER_MOUSE || plyrpad->GetSteeringLeftRight() != 0)) { // Simplified `if` here
            return { m_fRawSteerAngle + GetSteeringDeltaForFrame(), CONTROLLER_KEYBOARD1 };
        }

        if (CPad::NewMouseControllerState.X != 0.f || m_fRawSteerAngle != 0.f) {
            if (!plyrpad->NewState.m_bVehicleMouseLook) {
                return { m_fRawSteerAngle - CPad::NewMouseControllerState.X * 0.0035f, CONTROLLER_MOUSE };
            }

            if (plyrpad->NewState.m_bVehicleMouseLook || std::abs(m_fRawSteerAngle) <= 0.7f) { // Slowly steer back to 0
                return { m_fRawSteerAngle * std::pow(0.975f, CTimer::GetTimeStep()), CONTROLLER_MOUSE };
            }

            return { m_fRawSteerAngle, CONTROLLER_MOUSE };
        }

        return { m_fRawSteerAngle, m_nLastControlInput }; // No change
    }();
    m_fRawSteerAngle = std::clamp(m_fRawSteerAngle, -1.f, 1.f);

    //> Calculate gas/brake pedal values
    std::tie(m_fGasPedal, m_fBreakPedal) = [&, this]() -> std::pair<float, float> {
        if (automaticallyHandBrake) { // 0x6ADA8D
            return { 0.f, 1.f };
        }

        const auto fwdvel = GetMoveSpeed().Dot(GetForward()); // From the beginning

        //> 0x6AD9CC [moved it here]
        const auto gasPedalInput = [&, this] {
            if (vehicleFlags.bEngineOn && vehicleFlags.bIsDrowning) { // Moved up as early out
                return 0.f;
            }
            auto value = plyrpad->GetAccelerate() - plyrpad->GetBrake(); // Rescaled later..
            if (   !value
                && notsa::dyn_cast_if_present<CTaskSimpleGangDriveBy>(plyrdriver->GetTaskManager().GetSimplestActiveTask())
                && TheCamera.m_aCams[0].m_nMode == MODE_AIMWEAPON_FROMCAR
            ) {
                value = plyrpad->NewState.RightShoulder2 - plyrpad->NewState.LeftShoulder2; // Rescaled later..
            }
            return (value < 0 && m_nModelIndex == (uint16)eModelID::UNLOAD_MODEL
                ? (float)(value * 0.3f)
                : (float)(value)
            ) / 255.f;
        }();

        if (std::abs(fwdvel) < 0.01f) { // 0x6ADAC6
            if (plyrpad->GetAccelerate() > 150 && plyrpad->GetBrake() > 150) {
                m_bDoingBurnout = true;
                return {
                    (float)plyrpad->GetAccelerate() / 255.f,
                    (float)plyrpad->GetBrake() / 255.f
                };
            } else {
                return { gasPedalInput, 0.f };
            }
        }

        if (fwdvel >= 0.f) { // 0x6ADBA6
            if (gasPedalInput < 0.f) { 
                return { 0.f, std::abs(gasPedalInput) }; // Braking by pressing the reverse button
            } else {
                return { gasPedalInput, 0.f };
            }
        }

        if (gasPedalInput >= 0.f && (m_fGasPedal <= 0.5f || fwdvel <= -0.15f)) { // 0x6ADBD9 - Simplified 
            return { 0.f, gasPedalInput }; // Apply braking
        }

        return { gasPedalInput, 0.f }; // Regular go forwards
    }();

    //> 0x6ADC18 and 0x6ADC64 - Check if we can go towards that direction
    if (m_fGasPedal != 0.f && plyrdriver) {
        if (!CGameLogic::IsPlayerAllowedToGoInThisDirection(
            plyrdriver,
            m_fGasPedal > 0.f ? GetForward() : -GetForward(),
            0.f
        )) {
            m_fGasPedal = 0.f;
        }
    }

    m_fSteerAngle = DegreesToRadians(m_pHandlingData->m_fSteeringLock * std::copysignf(std::powf(m_fRawSteerAngle, 2), m_fRawSteerAngle)); // sign preserving pow

    //> 0x6ADD28 - Rear wheel steer
    if (m_pHandlingData->m_bHbRearwheelSteer) {
        m_f2ndSteerAngle = vehicleFlags.bIsHandbrakeOn
            ? lerp(m_f2ndSteerAngle, -m_fSteerAngle, 0.9f) // TODO/NOTE: `lerp`'s `from` and `to` arguments must be constant, this is textbook bad practice!
            : m_f2ndSteerAngle * 0.9f; // TODO/NOTE: Seems incorrect, maybe they meant `m_fSteerAngle * 0.9f`?
    }

    //> 0x6ADD81 - Comedy controls stuff
    if (vehicleFlags.bComedyControls) {
        const auto rndnum = CGeneral::GetRandomNumberInRange(0, 10);

        using enum eComedyControlState;
        if (m_comedyControlState != INACTIVE) {
            m_fSteerAngle += m_comedyControlState == STEER_RIGHT ? 0.05f : -0.05f;
            if (rndnum < 2) {
                m_comedyControlState = INACTIVE;
            }
        } else {
            m_comedyControlState = [&] {
                switch (rndnum) { // Using this instead of `rand() % 10`
                case 0:
                case 1:
                    return STEER_RIGHT;
                case 2:
                case 3:
                    return STEER_LEFT;
                default:
                    return INACTIVE; // No change
                }
            }();
        }
    }

    //> 0x6ADDCC - Slow player down in some cases
    if (const auto pad0 = CPad::GetPad(0); (pad0->DisablePlayerControls && CGameLogic::SkipState != 2) || pad0->bApplyBrakes) {
        vehicleFlags.bIsHandbrakeOn = true;

        m_fGasPedal = 0.f;
        m_fBreakPedal = 1.f;
        FindPlayerPed()->KeepAreaAroundPlayerClear();
        if (const auto speedsq = GetMoveSpeed().SquaredMagnitude(); speedsq >= sq(0.28f)) {
            GetMoveSpeed() *= 0.28f / std::sqrt(speedsq);
        }
    }

    //> 0x6ADEB3
    if (vehicleFlags.bEngineBroken) {
        vehicleFlags.bIsHandbrakeOn = false;

        m_fBreakPedal = 0.05f;
        m_fGasPedal = 0.f;
    }
}

// 0x6A2210
void CAutomobile::GetComponentWorldPosition(int32 componentId, CVector& outPos)
{
    if (m_aCarNodes[componentId]) {
        outPos = RwFrameGetLTM(m_aCarNodes[componentId])->pos;
    }
}

// 0x6A2250
bool CAutomobile::IsComponentPresent(int32 componentId)
{
    return m_aCarNodes[componentId] != 0;
}

// 0x6A6AE0
void CAutomobile::OpenDoor(CPed* ped, int32 nodeIdx, eDoors doorIdx, float doorOpenRatio, bool playSound) {
    const auto frame = m_aCarNodes[nodeIdx];
    if (!frame) {
        return;
    }

    CMatrix frameMat{ RwFrameGetMatrix(frame) }; // Is it necessary to do this here?
    auto& door = m_doors[doorIdx];

    const auto wasClosed = door.IsClosed();
    if (wasClosed) {
        RwFrameForAllObjects( // TODO: Make function (See refs to `SetAtomicFlagCB` and `ClearAtomicFlagCB` and combine them somehow)
            frame,
            CVehicleModelInfo::ClearAtomicFlagCB,
            (void*)(ATOMIC_RENDER_ALWAYS)
        );
    }

    door.Open(doorOpenRatio);

    const auto ProcessSound = [&](eAudioEvents baseAE, float eventQuietVolume) {
        if (!playSound) {
            return;
        }
        m_vehicleAudio.AddAudioEvent((eAudioEvents)((size_t)baseAE + (size_t)doorIdx), 0.f);
        if (!ped) {
            return;
        }
        GetEventGlobalGroup()->Add(CEventSoundQuiet{ ped, eventQuietVolume, (uint32)-1, {} });
    };

    if (wasClosed && !door.IsClosed()) {
        RwFrameForAllObjects( // TODO: Make function (See refs to `SetAtomicFlagCB` below)
            frame,
            CVehicleModelInfo::SetAtomicFlagCB,
            (void*)(ATOMIC_RENDER_ALWAYS)
        );
        ProcessSound(AE_CAR_BONNET_OPEN, 60.f);
    } else if (!wasClosed && doorOpenRatio == 0.f) {
        m_damageManager.SetDoorClosed(doorIdx);
        ProcessSound(AE_CAR_BONNET_CLOSE, 120.f);
    }

    door.UpdateFrameMatrix(frameMat);
}

// 0x6A2270
float CAutomobile::GetDooorAngleOpenRatio(eDoors door)
{
    const auto doorId = (int32)door;
    return m_doors[doorId].GetAngleOpenRatio();
}

// 0x6A62C0
float CAutomobile::GetDooorAngleOpenRatioU32(uint32 door)
{
    switch (door) {
    case 8:
        return GetDooorAngleOpenRatio(DOOR_RIGHT_FRONT);
    case 9:
        return GetDooorAngleOpenRatio(DOOR_RIGHT_REAR);
    case 10:
        return GetDooorAngleOpenRatio(DOOR_LEFT_FRONT);
    case 11:
        return GetDooorAngleOpenRatio(DOOR_LEFT_REAR);
    case 18:
        return 0.0f;
    default:
        assert(0); // Prevent usage mistakes - There's is an eDoors overload, and if an int literal is used instead of the enum this function will be called incorrectly.
        return 0.0f;
    }
}

// 0x6A2290
bool CAutomobile::IsDoorReady(eDoors door)
{
    if (m_doors[door].IsClosed())
        return true;

    if (IsDoorMissing(door))
        return true;

    return false;
}

// 0x6A6350
bool CAutomobile::IsDoorReadyU32(uint32 door)
{
    switch (door) {
    case 8:
        return IsDoorReady(DOOR_RIGHT_FRONT);
    case 9:
        return IsDoorReady(DOOR_RIGHT_REAR);
    case 10:
        return IsDoorReady(DOOR_LEFT_FRONT);
    case 11:
        return IsDoorReady(DOOR_LEFT_REAR);
    case 18:
        return true;
    default:
        assert(0); // Prevent usage mistakes - There's is an eDoors overload, and if an int literal is used instead of the enum this function will be called incorrectly.
        return false;
    }
}

// 0x6A22D0
bool CAutomobile::IsDoorFullyOpen(eDoors door)
{
    if (m_doors[door].IsFullyOpen())
        return true;

    if (IsDoorMissing(door))
        return true;

    return false;
}

// 0x6A63E0
bool CAutomobile::IsDoorFullyOpenU32(uint32 door) {
    switch (door) {
    case 8:  return IsDoorFullyOpen(DOOR_RIGHT_FRONT);
    case 9:  return IsDoorFullyOpen(DOOR_RIGHT_REAR);
    case 10: return IsDoorFullyOpen(DOOR_LEFT_FRONT);
    case 11: return IsDoorFullyOpen(DOOR_LEFT_REAR);
    case 18: return false;
    default:
        assert(0); // Prevent usage mistakes - There's is an eDoors overload, and if an int literal is used instead of the enum this function will be called incorrectly.
        return false;
    }
}

// 0x6A2310
bool CAutomobile::IsDoorClosed(eDoors door) {
    return m_doors[door].IsClosed();
}

// 0x6A6470
bool CAutomobile::IsDoorClosedU32(uint32 door) {
    switch (door) {
    case 8:  return IsDoorClosed(DOOR_RIGHT_FRONT);
    case 9:  return IsDoorClosed(DOOR_RIGHT_REAR);
    case 10: return IsDoorClosed(DOOR_LEFT_FRONT);
    case 11: return IsDoorClosed(DOOR_LEFT_REAR);
    case 18: return false;
    default:
        assert(0); // Prevent usage mistakes - There's is an eDoors overload, and if an int literal is used instead of the enum this function will be called incorrectly.
        return false;
    }
}

// 0x6A2330
bool CAutomobile::IsDoorMissing(eDoors door) {
    return m_damageManager.GetDoorStatus(door) == DAMSTATE_NOTPRESENT;
}

// 0x6A6500
bool CAutomobile::IsDoorMissingU32(uint32 door) {
    switch (door) {
    case 8:  return IsDoorMissing(DOOR_RIGHT_FRONT);
    case 9:  return IsDoorMissing(DOOR_RIGHT_REAR);
    case 10: return IsDoorMissing(DOOR_LEFT_FRONT);
    case 11: return IsDoorMissing(DOOR_LEFT_REAR);
    case 18: return true;
    default:
        assert(0); // Prevent usage mistakes - There is an eDoors overload, and if an int literal is used instead of the enum this function will be called incorrectly.
        return false;
    }
}

// 0x6A2350
bool CAutomobile::IsOpenTopCar() {
    return m_nModelIndex == MODEL_COMET    && m_anExtras[0] && m_anExtras[1]
        || m_nModelIndex == MODEL_STALLION && m_anExtras[0] && m_anExtras[1];
}

// 0x6A3030
void CAutomobile::RemoveRefsToVehicle(CEntity* entity) {
    for (auto& ref : m_apWheelCollisionEntity) {
        if (ref == entity)
            ref = nullptr;
    }
}

// NOTSA - Combined implementation of `BlowUpCar` and `BlowUpCarCutSceneNoExtras`
void CAutomobile::BlowUpCar_Impl(CEntity* dmgr, bool bDontShakeCam, bool bDontSpawnStuff, bool bNoExplosion, bool bHideExplosionFx, bool bIsForCutScene, bool bMakeSound) {
    if (!vehicleFlags.bCanBeDamaged) {
        return;
    }

    const bool bFixBugs = // TODO: Make a function for this in `notsa::`
#ifdef FIX_BUGS
        true;
#else
        false;
#endif

    if (bIsForCutScene) {
        if (IsSubPlane() && GetStatus() != STATUS_PLAYER) {
            switch (m_autoPilot.m_nCarMission) {
            case MISSION_PLANE_FLYTOCOORS:
            case MISSION_PLANE_ATTACK_PLAYER:
            case MISSION_PLANE_FLYINDIRECTION:
            case MISSION_PLANE_FOLLOW_ENTITY:
            case MISSION_PLANE_ATTACK_PLAYER_POLICE:
            case MISSION_PLANE_DOG_FIGHT_ENTITY:
            case MISSION_PLANE_DOG_FIGHT_PLAYER:
                m_autoPilot.SetCarMission(MISSION_PLANE_CRASH_AND_BURN);
                return;
            }
        }
    } else {
        if (dmgr == FindPlayerPed() || dmgr == FindPlayerVehicle()) {
            auto& plyrinfo = FindPlayerInfo();
            plyrinfo.m_nHavocCaused += 20;
            plyrinfo.m_fCurrentChaseValue += 10.f;
            CStats::IncrementStat(STAT_COST_OF_PROPERTY_DAMAGED, CGeneral::GetRandomNumberInRange(4000.f, 10'000.f - 1.f));
        }
    }

    if (m_nModelIndex == eModelID::MODEL_VCNMAV) {
        CWanted::bUseNewsHeliInAdditionToPolice = false;
    }

    if (!bNoExplosion) {
        GetMoveSpeed().z += 0.13f;
    }

    SetStatus(STATUS_WRECKED);

    physicalFlags.bRenderScorched = true;
    m_nTimeWhenBlowedUp      = CTimer::GetTimeInMS();

    CVisibilityPlugins::SetClumpForAllAtomicsFlag(m_pRwClump, ATOMIC_IS_BLOWN_UP);
    m_damageManager.FuckCarCompletely(false);

    const auto isRcShit = (bFixBugs || !bIsForCutScene)
        ? notsa::contains({ MODEL_RCTIGER, MODEL_RCBANDIT }, GetModelId()) // I'm 99% sure they forgot to copy paste this too, but let it be.
        : GetModelId() == MODEL_RCBANDIT;
    if (!isRcShit) { // 0x6B3C61
        for (auto bumper : { FRONT_BUMPER, REAR_BUMPER }) {
            SetBumperDamage(bumper, bDontSpawnStuff);
        }
        for (auto door : { DOOR_BONNET, DOOR_BOOT, DOOR_LEFT_FRONT, DOOR_RIGHT_FRONT, DOOR_LEFT_REAR, DOOR_RIGHT_REAR }) {
            SetDoorDamage(door, bDontSpawnStuff);
        }
        if (!bDontSpawnStuff) {
            SpawnFlyingComponent(CAR_WHEEL_LF, 1);
        }
        if (!bNoExplosion) {
            if (const auto obj = GetCurrentAtomicObject(m_aCarNodes[CAR_WHEEL_LF])) {
                RpAtomicSetFlags(obj, 0); // TODO: Use appropriate enum (if any?)
            }
        }
    }

    m_nBombOnBoard = 0;
    m_fHealth      = 0.f;
    m_wBombTimer   = 0;

    if (!bDontShakeCam) {
        TheCamera.CamShake(0.4f, GetPosition());
    }

    KillPedsInVehicle();
    if (bFixBugs || !bIsForCutScene) { // Originally not in `BlowUpCarCutSceneNoExtras`, but they probably just forgot to copy paste it
        KillPedsGettingInVehicle();  
    }

    vehicleFlags.bLightsOn     = false;
    vehicleFlags.bEngineOn     = false;
    vehicleFlags.bSirenOrAlarm = false;
    m_nOverrideLights          = NO_CAR_LIGHT_OVERRIDE;
    autoFlags.bTaxiLight     = false;

    if (vehicleFlags.bIsAmbulanceOnDuty) { //> 0x6B3DAE
        assert(!vehicleFlags.bIsFireTruckOnDuty);
        vehicleFlags.bIsAmbulanceOnDuty = false;
        CCarCtrl::NumAmbulancesOnDuty--;
    }

    if (vehicleFlags.bIsFireTruckOnDuty) {
        assert(!vehicleFlags.bIsAmbulanceOnDuty);
        vehicleFlags.bIsFireTruckOnDuty = false;
        CCarCtrl::NumFireTrucksOnDuty--;
    }

    ChangeLawEnforcerState(false);

    if (!bNoExplosion) {
        CDarkel::RegisterCarBlownUpByPlayer(*this, 0); // Um.... Okay

        gFireManager.StartFire(this, dmgr);

        if (bIsForCutScene) { //> 0x6B3E0D
            CExplosion::AddExplosion(this, dmgr, EXPLOSION_MOLOTOV, GetPosition(), 0, bMakeSound, -1.f, bHideExplosionFx);
        } else { //> 0x6B3A17 - Add explosion fx (cancer inducing code)
            const auto tcm = GetColModel();
            const auto maxOffset = IsSubAutomobile() || IsSubQuad()
                ? 0.75f
                : 0.1f;
            const auto RandomOffset = [&]() { return CGeneral::GetRandomNumberInRange(-maxOffset, maxOffset); };
            const auto explOffset = CVector{
                RandomOffset(),
                RandomOffset(),
                maxOffset + 0.5f
            } * GetColModel()->GetBoundingBox().m_vecMax;
            const auto explPos = // Do a matrix transform... manually, because that's fun!
                  GetPosition()
                + GetRight() * explOffset.x
                + GetForward() * explOffset.y
                - CVector{0.f, 0.f, explOffset.z};
            CExplosion::AddExplosion(
                this,
                dmgr,
                isRcShit ? EXPLOSION_QUICK_CAR : EXPLOSION_CAR,
                explPos,
                0,
                bMakeSound,
                -1.f,
                bHideExplosionFx
            );
        }
    }
}

// 0x6B3780
void CAutomobile::BlowUpCar(CEntity* dmgr, bool bHideExplosionFx) {
    BlowUpCar_Impl(dmgr, false, false, false, bHideExplosionFx, false, true);
}

// 0x6B3BB0
void CAutomobile::BlowUpCarCutSceneNoExtras(bool bDontShakeCam, bool bDontSpawnStuff, bool bNoExplosion, bool bMakeSound) {
    BlowUpCar_Impl(nullptr, bDontShakeCam, bDontSpawnStuff, bNoExplosion, false, true, bMakeSound);
}

// 0x6A3060
bool CAutomobile::SetUpWheelColModel(CColModel* wheelCol) {
    const auto mi = GetVehicleModelInfo();
    const auto wcm = GetColModel();
    const auto wcd = wheelCol->m_pColData;

    if (m_nModelIndex == MODEL_TRACTOR || m_nModelIndex == MODEL_COMBINE || m_nModelIndex == MODEL_KART)
        return false;

    wheelCol->m_boundBox = wcm->m_boundBox;
    wheelCol->m_boundSphere = wcm->m_boundSphere;

    CMatrix mat;

    mat.Attach(RwFrameGetMatrix(m_aCarNodes[CAR_WHEEL_LF]), false);
    wcd->m_pSpheres[0].Set(mi->m_fWheelSizeFront / 2.0f, mat.GetPosition(), SURFACE_RUBBER, 0xD);

    mat.Attach(RwFrameGetMatrix(m_aCarNodes[CAR_WHEEL_LB]), false);
    wcd->m_pSpheres[1].Set(mi->m_fWheelSizeRear / 2.0f, mat.GetPosition(), SURFACE_RUBBER, 0xF);

    mat.Attach(RwFrameGetMatrix(m_aCarNodes[CAR_WHEEL_RF]), false);
    wcd->m_pSpheres[2].Set(mi->m_fWheelSizeFront / 2.0f, mat.GetPosition(), SURFACE_RUBBER, 0xE);

    mat.Attach(RwFrameGetMatrix(m_aCarNodes[CAR_WHEEL_RB]), false);
    wcd->m_pSpheres[3].Set(mi->m_fWheelSizeRear / 2.0f, mat.GetPosition(), SURFACE_RUBBER, 0x10);

    if (m_aCarNodes[CAR_WHEEL_LM] && m_aCarNodes[CAR_WHEEL_RM]) {
        mat.Attach(RwFrameGetMatrix(m_aCarNodes[CAR_WHEEL_LM]), false);
        wcd->m_pSpheres[4].Set(mi->m_fWheelSizeRear / 2.0f, mat.GetPosition(), SURFACE_RUBBER, 0xF);

        mat.Attach(RwFrameGetMatrix(m_aCarNodes[CAR_WHEEL_RM]), false);
        wcd->m_pSpheres[5].Set(mi->m_fWheelSizeRear / 2.0f, mat.GetPosition(), SURFACE_RUBBER, 0x10);

        wcd->m_nNumSpheres = 6;
    } else {
        wcd->m_nNumSpheres = 4;
    }
    return true;
}

// 0x6A32B0
bool CAutomobile::BurstTyre(uint8 tyreComponentId, bool bPhysicalEffect) {
    if (m_nModelIndex == MODEL_RHINO)
        return false;

    if (vehicleFlags.bTyresDontBurst)
        return false;

    if (physicalFlags.bRenderScorched)
        return false;

    const auto GetCarWheel = [&]() -> eCarWheel { // TODO: `CarPieceToCarWheel` (See `CarWheelToCarPiece`)
        switch (tyreComponentId) {
        case CAR_PIECE_WHEEL_LF: return CAR_WHEEL_FRONT_LEFT;
        case CAR_PIECE_WHEEL_RF: return CAR_WHEEL_FRONT_RIGHT;
        case CAR_PIECE_WHEEL_RL: return CAR_WHEEL_REAR_LEFT;
        case CAR_PIECE_WHEEL_RR: return CAR_WHEEL_REAR_RIGHT;
        default:
            return (eCarWheel)tyreComponentId; // TODO: NOTSA_UNREACHABLE()
        }
    };

    auto wheel = GetCarWheel();
    if (m_damageManager.GetWheelStatus(wheel))
        return false;

    m_damageManager.SetWheelStatus(wheel, WHEEL_STATUS_BURST);
    CStats::IncrementStat(STAT_TIRES_POPPED_WITH_GUNFIRE, 1.0f);
    m_vehicleAudio.AddAudioEvent(AE_TYRE_BURST, 0.0f);
    if (m_nStatus == STATUS_SIMPLE) {
        CCarCtrl::SwitchVehicleToRealPhysics(this);
    }

    constexpr auto force = 0.03f;
    if (bPhysicalEffect) {
        ApplyMoveForce(m_matrix->GetRight() * CGeneral::GetRandomNumberInRange(-force, force) * m_fMass);
        ApplyTurnForce(
            m_matrix->GetRight() * CGeneral::GetRandomNumberInRange(-force, force) * m_fTurnMass,
            m_matrix->GetForward()
        );

        m_nFakePhysics = 0;
    }
    return true;
}

// 0x6A3850
bool CAutomobile::IsRoomForPedToLeaveCar(uint32 arg0, CVector* arg1) {
    return true;
}

// 0x6A65D0
void CAutomobile::SetupSuspensionLines() {
    const auto& mi = *GetVehicleModelInfo();
          auto& cm = *mi.GetColModel();
          auto& cd = *cm.m_pColData;

    // 0x6A65FB
    const bool hadToAllocateLines = !cd.m_nNumLines;
    if (hadToAllocateLines) {
        cd.AllocateLines(ModelIndices::IsRhino(m_nModelIndex) ? NUM_RHINO_SUSP_LINES : NUM_AUTOMOBILE_SUSP_LINES);
    }

    // 0x6A6754
    // Calculate col line positions for wheels, set wheel explPos, spring and line lengths
    const auto& handling = *m_pHandlingData;
    for (auto i = 0; i < NUM_AUTOMOBILE_SUSP_LINES; i++) {
        auto& colLine = cd.m_pLines[i]; // Collision line for this wheel

        CVector wheelPos;
        mi.GetWheelPosn(i, wheelPos, false);

        if (IsSubQuad()) {
            if (wheelPos.x <= 0.f)
                wheelPos.x -= 0.15f;
            else
                wheelPos.x += 0.15f;
        }

        colLine.m_vecStart = CVector{ wheelPos.x, wheelPos.y, wheelPos.z + handling.m_fSuspensionUpperLimit };
        colLine.m_vecEnd   = CVector{ wheelPos.x, wheelPos.y, wheelPos.z + handling.m_fSuspensionLowerLimit - mi.GetSizeOfWheel((eCarWheel)i) / 2.f };

        m_wheelPosition[i]           = wheelPos.z; // Originally set earlier in the loop, but should be fine.
        m_aSuspensionSpringLength[i] = handling.m_fSuspensionUpperLimit - handling.m_fSuspensionLowerLimit;
        m_aSuspensionLineLength[i]   = colLine.m_vecStart.z - colLine.m_vecEnd.z;
    }

    /* NOP code - 0x6A6764
    if (GetVehicleAppearance() != eVehicleAppearance::VEHICLE_APPEARANCE_PLANE) {
        GetVehicleAppearance();
    }*/

    const auto CalculateHeightAboveRoad = [&](eCarWheel wheel) {
        return (1.f - 1.f / (handling.m_fSuspensionForceLevel * 4.f)) * m_aSuspensionSpringLength[(size_t)wheel] + mi.GetSizeOfWheel(wheel) / 2.f - cd.m_pLines[(size_t)wheel].m_vecStart.z;
    };

    // 0x6A65D0
    m_fFrontHeightAboveRoad = CalculateHeightAboveRoad(eCarWheel::CAR_WHEEL_FRONT_LEFT);
    m_fRearHeightAboveRoad = CalculateHeightAboveRoad(eCarWheel::CAR_WHEEL_REAR_LEFT);

    // 0x6A681A
    // Adjust wheel's position based on height above road
    for (auto i = 0u; i < std::size(m_wheelPosition); i++) {
        m_wheelPosition[i] = mi.GetSizeOfWheel((eCarWheel)i) / 2.f - m_fFrontHeightAboveRoad; // Not sure why it uses front height for all wheels?
    }

    // 0x6A681C
    // This was probably inlined? Not sure..
    if (cd.m_pLines[0].m_vecEnd.z < cm.m_boundBox.m_vecMin.z) {
        // V570 The 'wcd.m_pLines[0].m_vecEnd.z' variable is assigned to itself.
        // todo: figure out what they want
        // wcd.m_pLines[0].m_vecEnd.z = wcd.m_pLines[0].m_vecEnd.z; // Adjust bounding box

        // Adjust bounding sphere radius - Originally outside this `if`, but that's pointless - This was most likely inlined.
        cm.m_boundSphere.m_fRadius = std::max({ cm.m_boundSphere.m_fRadius, cm.m_boundBox.m_vecMin.Magnitude(), cm.m_boundBox.m_vecMax.Magnitude() });
    }

    // 0x6A68C1
    // Adjust sphere sizes for rcbandit
    if (ModelIndices::IsRCBandit(m_nModelIndex)) {
        cm.m_boundSphere.m_fRadius = 2.f;

        for (auto&& sph : cd.GetSpheres()) {
            sph.m_fRadius = 0.3f;
        }
    }

    // 0x6A68FD
    if (handling.m_bForceGroundClearance && hadToAllocateLines) {
        const auto sphBottomMinZ = (ModelIndices::IsKart(m_nModelIndex) ? 0.12f : 0.25f) - m_fFrontHeightAboveRoad;
        for (auto&& sph : cd.GetSpheres()) {
            const auto sphBottomZ = sph.m_vecCenter.z - sph.m_fRadius;
            if (sphBottomZ < sphBottomMinZ) {
                if (sph.m_fRadius > 0.4f) {
                    sph.m_fRadius = std::max(0.4f, sphBottomZ);
                    sph.m_vecCenter.z = sphBottomZ + sph.m_fRadius;
                }
            }
        }
    }

    // 0x6A6995
    // Rhino has 12 suspension lines, which are calculated based on the 4 main wheel's line's position
    if (ModelIndices::IsRhino(m_nModelIndex)) {
        size_t lineIndex{ NUM_AUTOMOBILE_SUSP_LINES }; // Skip first 4 lines as those are already in use for the real wheels

        for (auto i = 0u; i < NUM_AUTOMOBILE_SUSP_LINES; i += NUM_AUTOMOBILE_SUSP_LINES / 2) { // Do left, and right side - Wheels 0, 1 are on the left side, while 2, 3 are on the right
            for (auto j = 0u; j < NUM_AUTOMOBILE_SUSP_LINES; j++, lineIndex++) { // Add 4 extra lines on each side
                const auto wheelRelativePos = (float)(j + 1u) / (float)(NUM_AUTOMOBILE_SUSP_LINES + 1);

                // Calculate positions of this wheel's line by lerping between the 2 wheel's lines on this side
                auto& line      = cd.m_pLines[lineIndex];
                line.m_vecStart = lerp(cd.m_pLines[i].m_vecStart, cd.m_pLines[i + 1].m_vecStart, wheelRelativePos);
                line.m_vecEnd   = lerp(cd.m_pLines[i].m_vecEnd, cd.m_pLines[i + 1].m_vecEnd, wheelRelativePos);
            }
        }
    }
}

// 0x6A3440
void CAutomobile::Fix() {
    m_damageManager.ResetDamageStatus();

    // Reset actual doorIdx's status
    if (m_pHandlingData->m_bWheelFNarrow2) {
        m_damageManager.SetDoorStatus({ DOOR_LEFT_FRONT, DOOR_RIGHT_FRONT, DOOR_LEFT_REAR, DOOR_RIGHT_REAR }, eDoorStatus::DAMSTATE_NOTPRESENT);
    }

    vehicleFlags.bIsDamaged = false;

    // Hide all DAM state atomics
    RpClumpForAllAtomics(m_pRwClump, CVehicleModelInfo::HideAllComponentsAtomicCB, (void*)ATOMIC_IS_DAM_STATE);

    // Reset rotation of some nodes
    for (auto i = (size_t)CAR_DOOR_RF; i < (size_t)CAR_NUM_NODES; i++) {
        if (auto frame = m_aCarNodes[i]) {
            CMatrix mat{ RwFrameGetMatrix(frame), false };
            mat.SetTranslate(mat.GetPosition());
            mat.UpdateRW();
        }
    }

    // Set all wheels
    m_damageManager.SetAllWheelsState(eCarWheelStatus::WHEEL_STATUS_OK);

    // Reset all bouncing panels
    for (auto&& panel : m_panels) {
        panel.ResetPanel();
    }
}

// 0x6B3E90
void CAutomobile::SetupDamageAfterLoad() {
    // TODO: Moderinfy....

    if (m_aCarNodes[CAR_BUMP_FRONT]) SetBumperDamage(FRONT_BUMPER, false);
    if (m_aCarNodes[CAR_BUMP_REAR])  SetBumperDamage(REAR_BUMPER, false);

    if (m_aCarNodes[CAR_BONNET])  SetDoorDamage(DOOR_BONNET, false);
    if (m_aCarNodes[CAR_BOOT])    SetDoorDamage(DOOR_BOOT, false);
    if (m_aCarNodes[CAR_DOOR_LF]) SetDoorDamage(DOOR_LEFT_FRONT, false);
    if (m_aCarNodes[CAR_DOOR_RF]) SetDoorDamage(DOOR_RIGHT_FRONT, false);
    if (m_aCarNodes[CAR_DOOR_LR]) SetDoorDamage(DOOR_LEFT_REAR, false);
    if (m_aCarNodes[CAR_DOOR_RR]) SetDoorDamage(DOOR_RIGHT_REAR, false);

    if (m_aCarNodes[CAR_WING_LF]) SetPanelDamage(FRONT_LEFT_PANEL, false);
    if (m_aCarNodes[CAR_WING_RF]) SetPanelDamage(FRONT_RIGHT_PANEL, false);
}

// 0x6A47F0
void CAutomobile::DoBurstAndSoftGroundRatios()
{
    const auto& mi = *GetVehicleModelInfo();
    const auto speedToFwdRatio = std::fabs(DotProduct(m_vecMoveSpeed, m_matrix->GetForward())); // Remember: Dot product is 0 when the two vectors are perpendicular - TODO: Rename var to fwdvel (forward velocity)

    for (auto i = 0u; i < 4; i++) {
        const auto& wheelCP = m_wheelColPoint[i];
        const auto& springLen = m_aSuspensionSpringLength[i];
        const auto& lineLen = m_aSuspensionLineLength[i];
        auto& compression = m_fWheelsSuspensionCompression[i];
        auto& rotation = m_wheelRotation[i];

        const auto GetRemainingSuspensionCompression = [&] {
            return (lineLen - springLen) / lineLen;
        };

        switch (m_damageManager.GetWheelStatus((eCarWheel)i)) {
        case eCarWheelStatus::WHEEL_STATUS_MISSING:
            compression = 1.0f;
            break;
        case eCarWheelStatus::WHEEL_STATUS_BURST: {
            // The more opposite the speedsq is to the forward vector the bigger chance
            // The highest chance is when the speedsq is opposite to forward (ie.: It's backwards)
            const auto val = CGeneral::GetRandomNumberInRange(0, int32(speedToFwdRatio * 40.0f) + 98);
            if (val < 100) {
                compression += GetRemainingSuspensionCompression() / 4.f;
                compression = std::min(1.0f, compression);
            }
            break;
        }
        default: {
            if (   compression >= 1.0f
                || g_surfaceInfos.GetAdhesionGroup(wheelCP.m_nSurfaceTypeB) != ADHESION_GROUP_SAND
                || ModelIndices::IsRhino(m_nModelIndex)
            ) {
                if (compression < 1.0f && wheelCP.m_nSurfaceTypeB == SURFACE_RAILTRACK) {
                    auto wheelSizeFactor = 1.5f / (mi.GetSizeOfWheel((eCarWheel)i) / 2.0f);
                    if (speedToFwdRatio > 0.3f) {
                        wheelSizeFactor *= speedToFwdRatio / 0.3f;
                    }

                    auto wheelSizeInv = 1.0f / wheelSizeFactor;
                    auto wheelRotFactor = wheelSizeInv * rotation;
                    auto wheelRotFactorFract = wheelRotFactor - std::floor(wheelRotFactor);

                    auto timeSpeedRotFactor = (CTimer::GetTimeStep() * m_wheelSpeed[i] + rotation) * wheelSizeInv;
                    auto timeSpeedRotFactorFract = timeSpeedRotFactor - std::floor(timeSpeedRotFactor);

                    if (   m_wheelSpeed[i] > 0.0f && timeSpeedRotFactorFract < wheelRotFactorFract
                        || m_wheelSpeed[i] < 0.0f && timeSpeedRotFactorFract > wheelRotFactorFract
                    ) {
                        compression = std::max(0.2f, compression - GetRemainingSuspensionCompression() * 0.3f);
                    }
                }
            } else {
                float offroadFactor = 0.3f;
                if (handlingFlags.bOffroadAbility2) {
                    offroadFactor = 0.15f;
                } else if (handlingFlags.bOffroadAbility) {
                    offroadFactor = 0.2f;
                }

                auto adhesionFactor = 1.0f - speedToFwdRatio / 0.3f * 0.7f - CWeather::WetRoads * 0.7f;
                adhesionFactor = std::max(0.4f, adhesionFactor);

                compression += offroadFactor * GetRemainingSuspensionCompression() * adhesionFactor;
                compression = std::min(compression, 1.0f);
            }
            break;
        }
        }
    }
}

// 0x6A3770
void CAutomobile::PlayCarHorn()
{
    if (m_nAlarmState && m_nAlarmState != -1 && m_nStatus != STATUS_WRECKED || m_nHornCounter) {
        return;
    }

    if (m_nCarHornTimer) {
        m_nCarHornTimer -= 1;
        return;
    }

    m_nCarHornTimer = CGeneral::GetRandomNumber() % 128 - 106; // TODO: GetRandomNumberInRange
    if (const auto r = m_nCarHornTimer % 8; r < 4) {
        if (r >= 2) {
            if (m_pDriver && m_autoPilot.carCtrlFlags.bHonkAtCar) {
                m_pDriver->Say(CTX_GLOBAL_BLOCKED);
            }
        }
        m_nHornCounter = 45;
    } else {
        if (m_pDriver) {
            m_pDriver->Say(CTX_GLOBAL_BLOCKED);
        }
    }
}

// 0x6A62B0
float CAutomobile::GetHeightAboveRoad() {
    return m_fFrontHeightAboveRoad;
}

// 0x6A62A0
int32 CAutomobile::GetNumContactWheels() {
    return m_nNumContactWheels;
}

// 0x6A7650
void CAutomobile::VehicleDamage(float damageIntensity, eVehicleCollisionComponent collisionComponent, CEntity* damager, CVector* vecCollisionCoors, CVector* vecCollisionDirection, eWeaponType weapon) {
    assert(this);

    if (!vehicleFlags.bCanBeDamaged) {
        return;
    }

    assert(m_matrix);

    float minDmgIntensity{ 25.f };
    float collForceMult{ 1.f / 3.f };
    float calcDmgIntensity{ damageIntensity };

    if (damageIntensity == 0.f) {
        // Man this is so stupid..
        vecCollisionDirection = &m_vecLastCollisionImpactVelocity;
        vecCollisionCoors     = &m_vecLastCollisionPosn;
        damager               = m_pDamageEntity;
        collisionComponent    = (eVehicleCollisionComponent)m_nPieceType;

        collForceMult = 1.f;
        minDmgIntensity  = m_fMass / 1500.f * 25.f;
        calcDmgIntensity = m_fDamageIntensity;

        // 0x6A7705
        if (m_matrix->GetUp().z < 0.f/*is flipped*/ && this != FindPlayerVehicle()) {
            if (autoFlags.bDoesNotGetDamagedUpsideDown) {
                return;
            }
            switch (m_nStatus) {
            case eEntityStatus::STATUS_REMOTE_CONTROLLED: {
                if (physicalFlags.bSubmergedInWater)
                    return;
                break;
            }
            case eEntityStatus::STATUS_WRECKED:
                break;
            default: {
                m_fHealth = std::max(0.f, m_fHealth - CTimer::GetTimeStep() * 4.f);
                break;
            }
            }
        }

        // 0x6A7780
        if (m_fDamageIntensity == 0.f || physicalFlags.bCollisionProof) {
            return;
        }

        if (IsSubQuad()) {
            if (CBike::DamageKnockOffRider(this, m_fDamageIntensity, m_nPieceType, m_pDamageEntity, m_vecLastCollisionPosn, m_vecLastCollisionImpactVelocity)) {
                return;
            }
        }

        // 0x6A77F0
        const auto lastImpactVel_Dot_Fwd = DotProduct(m_matrix->GetForward(), m_vecLastCollisionImpactVelocity);

        // 0x6A77D6
        if (   m_nStatus == STATUS_PHYSICS
            && !IsMissionVehicle()
            && lastImpactVel_Dot_Fwd < -0.4f // Impact was from behind - (-66, 66) deg
            && m_fDamageIntensity / m_fMass > 0.1f
        ) {
            m_autoPilot.SetTempAction(TEMPACT_HEADON_COLLISION, 4'000);
        }

        if (m_pDamageEntity) {
            // Inverted the if's a little, and used switch case to make it nicer.

            switch (m_pDamageEntity->GetType()) {
            case eEntityType::ENTITY_TYPE_VEHICLE: {
                // 0x6A7856
                if (!CGame::CanSeeOutSideFromCurrArea()
                    && std::abs(lastImpactVel_Dot_Fwd) > 0.4 // Sideways impact - (142, 66) deg
                ) {
                    calcDmgIntensity /= 3.f;
                }
                break;
            }
            case eEntityType::ENTITY_TYPE_PED: {
                // 0x6A7883
                if (const auto& ped = *m_pDamageEntity->AsPed(); ped.bIsStanding) {
                    const auto impactDir = DotProduct(ped.m_matrix->GetForward() * ped.m_vecAnimMovingShift.y, m_vecLastCollisionImpactVelocity);
                    if (impactDir < 0.f) { // From behind - (-90, 90) deg
                        calcDmgIntensity = std::max(0.f, calcDmgIntensity + impactDir * ped.m_fMass);
                    }
                }
                break;
            }
            }
        }
    } else {
        // 0x6A7BE5
        if (bool unused{}; !CanVehicleBeDamaged(damager, weapon, unused)) {
            return;
        }

        collForceMult = 1.f;
    }

    // 0x6A78F0
    if (   damager && damager->IsBuilding()
        && DotProduct(*vecCollisionDirection, m_matrix->GetUp()) > 0.6f // In front - (-66, 66) deg
    ) {
        return;
    }

    // 0x6A792B
    if (m_nStatus == STATUS_PLAYER) {
        if (CStats::GetPercentageProgress() >= 100.f) {
            calcDmgIntensity /= 2.f;
        }
    } else {
        if (   physicalFlags.bInvulnerable
            && damager
            && damager != FindPlayerPed()
            && damager != FindPlayerVehicle()
        ) {
            return;
        }
    }

    // 0x6A7984
    if (damager && (damager == m_pTowingVehicle || damager == m_pVehicleBeingTowed)) {
        return;
    }

    // 0x6A79AB (Condition inverted)
    if (calcDmgIntensity > minDmgIntensity && m_nStatus != STATUS_WRECKED) {
        // DEV_LOG("calcDmgIntensity: {.2f}", calcDmgIntensity); // NOTSA
        // 0x6A79C7
        // If we're a law enforcer, and the dmgr is the
        // player's plyrveh increase their wanted level.
        if (vehicleFlags.bIsLawEnforcer) {
            const auto vehicle = FindPlayerVehicle();
            if (vehicle && damager == vehicle && m_nStatus != STATUS_ABANDONED) {
                // R* used magnitude, but squared magnitude is more suitable.
                const auto speedMag = vehicle->m_vecMoveSpeed.SquaredMagnitude();
                if (   speedMag >= m_vecMoveSpeed.SquaredMagnitude()
                    && speedMag > sq(0.1f)
                ) {
                    FindPlayerPed()->SetWantedLevelNoDrop(1);
                }
            }
        }

        // 0x6A7A6E
        // Some shaky-shaky
        if (m_nStatus == STATUS_PLAYER && calcDmgIntensity > 50.f) {
            const auto intensity = std::min(250.f, calcDmgIntensity / m_fMass * 800.f + 100.f);
            CPad::GetPad()->StartShake(40000 / (uint8)intensity, (uint8)intensity, 2000u);
        }

        // 0x6A7ACF
        if (damager && damager->IsVehicle()) {
            m_nLastWeaponDamageType = WEAPON_RAMMEDBYCAR;
            m_pLastDamageEntity = damager;
            damager->RegisterReference(&m_pLastDamageEntity);
        }

        // Store light states, so later we can determine if any was blown
        // and play the fx sound
        const auto prevLightStates = m_damageManager.GetAllLightsState();

        if (m_nStatus == STATUS_PLAYER) {
            FindPlayerInfo().m_nVehicleTimeCounter = CTimer::GetTimeInMS();
        }

        // 0x6A7B63
        const auto moveSpeedMagSq = m_vecMoveSpeed.SquaredMagnitude();
        if (moveSpeedMagSq > 0.02f / 50.0f) {
            dmgDrawCarCollidingParticles(*vecCollisionCoors, calcDmgIntensity * collForceMult, weapon);
        }

        if (m_matrix->GetUp().z > 0.f /*Not flipped on roof*/ || moveSpeedMagSq > 0.3f) {
            #define ApplyDamageToComponent(component) m_damageManager.ApplyDamage(this, component, calcDmgIntensity * 4.0f, m_pHandlingData->m_fCollisionDamageMultiplier)

            // Returned value:
            // [-oo, 0] - Left side (In bounding box if: [-1, 0] )
            // [0, +oo] - Right side (In bounding box if: [0, 1] )
            const auto GetCollisionPointLocalDirection = [&, this] {
                const auto collDirDotRight = DotProduct(*vecCollisionCoors - GetPosition() , m_matrix->GetRight());

                // Since the dot product is scaled by the distance of the collision point from us
                // If we divide it by the width of our bounding box we can determinate how far
                // the point is:
                // [-oo, 0] - Left side (In bounding box if: [-1, 0] )
                // [0, +oo] - Right side (In bounding box if: [0, 1] )
            #ifdef FIX_BUGS
                return collDirDotRight / (GetColModel()->m_boundBox.GetWidth() / 2.f);
            #else
                // Must be positive, otherwise calculation below might not work (Adding abs might help?)
                // Also, here they rely on the fact that all bounding boxes are symmetrical split by the Y axis
                // Which is true for all plyrveh models in the vanilla SA (AFAIK)
                // But in order to prevent weird bugs we gotta do it the right way.
                assert(GetColModel()->m_boundBox.m_vecMax.x > 0.f);
                return collDirDotRight / GetColModel()->m_boundBox.m_vecMax.x;
            #endif
            };

            switch (collisionComponent) {
            case eVehicleCollisionComponent::BONNET:
                ApplyDamageToComponent(tComponent::COMPONENT_BONNET);
                break;
            case eVehicleCollisionComponent::BOOT:
                ApplyDamageToComponent(tComponent::COMPONENT_BOOT);
                break;
            case eVehicleCollisionComponent::BUMP_FRONT: {
                ApplyDamageToComponent(tComponent::COMPONENT_BUMP_FRONT);

                const auto localDir       = GetCollisionPointLocalDirection();
                const auto colDirDotRight = DotProduct(*vecCollisionDirection, m_matrix->GetRight());

                // Possibly apply right/left wing damage based on direction
                if (   localDir > 0.7f // [0, 45] deg
                    || localDir > 0.5f && colDirDotRight < -0.5f && m_fMass * 0.35f < calcDmgIntensity
                ) {
                    ApplyDamageToComponent(tComponent::COMPONENT_WING_RF);
                } else if (
                       localDir > -0.7f // [90, 135] deg
                    || localDir > -0.5f && colDirDotRight > 0.5f && m_fMass * 0.35f < calcDmgIntensity
                ) {
                    ApplyDamageToComponent(tComponent::COMPONENT_WING_LF);
                }

                // Possibly apply bonnet damage if front bumper is not `ok`
                if (   m_damageManager.GetPanelStatus(ePanels::FRONT_BUMPER) != DAMSTATE_OK && m_fMass * 0.2f < calcDmgIntensity
                    || weapon < WEAPON_LAST_WEAPON && (CGeneral::GetRandomNumber() % 3 == 0) // If it was a weapon, apply damage by randomly
                ) {
                    ApplyDamageToComponent(tComponent::COMPONENT_BONNET);
                }

                // Possibly apply windscreen damage if front bumper is `damaged`
                if (m_damageManager.GetPanelStatus(ePanels::FRONT_BUMPER) >= DAMSTATE_DAMAGED && m_fMass * 0.2f < calcDmgIntensity) {
                    ApplyDamageToComponent(tComponent::COMPONENT_WINDSCREEN);
                }
                break;
            }
            case eVehicleCollisionComponent::BUMP_REAR: {
                ApplyDamageToComponent(tComponent::COMPONENT_BUMP_REAR);

                const auto localDir = GetCollisionPointLocalDirection();

                // If this is a van possibly apply damage to the rear doors
                if (vehicleFlags.bIsVan && m_fMass * 0.35f < calcDmgIntensity) {
                    if (localDir > 0.1f) { // [-90, -85] deg
                        ApplyDamageToComponent(tComponent::COMPONENT_DOOR_RR);
                    } else if (localDir < -0.1f) { // [-95, -90]
                        ApplyDamageToComponent(tComponent::COMPONENT_DOOR_LR);
                    }
                }

                // Possibly apply damage to boot if not already damaged
                if (m_damageManager.GetPanelStatus(ePanels::REAR_BUMPER) < DAMSTATE_DAMAGED) {
                    ApplyDamageToComponent(tComponent::COMPONENT_BOOT);
                }


                break;
            }
            case eVehicleCollisionComponent::DOOR_LF:
                ApplyDamageToComponent(tComponent::COMPONENT_DOOR_LF);
                break;
            case eVehicleCollisionComponent::DOOR_RF:
                ApplyDamageToComponent(tComponent::COMPONENT_DOOR_RF);
                break;
            case eVehicleCollisionComponent::DOOR_LR:
                ApplyDamageToComponent(tComponent::COMPONENT_DOOR_LR);
                break;
            case eVehicleCollisionComponent::DOOR_RR:
                ApplyDamageToComponent(tComponent::COMPONENT_DOOR_RR);
                break;
            case eVehicleCollisionComponent::WING_LF:
                ApplyDamageToComponent(tComponent::COMPONENT_WING_LF);
                break;
            case eVehicleCollisionComponent::WING_RF:
                ApplyDamageToComponent(tComponent::COMPONENT_WING_RF);
                break;
            case eVehicleCollisionComponent::WINDSCREEN:
                ApplyDamageToComponent(tComponent::COMPONENT_WINDSCREEN);
                break;
            case eVehicleCollisionComponent::DEFAULT:
            case eVehicleCollisionComponent::WING_LR:
            case eVehicleCollisionComponent::WING_RR:
                break; // NOTSA
            }
        }
        #undef ApplyDamageToComponent

        // 0x6A8052
        auto calcCollHealthLoss = (calcDmgIntensity - minDmgIntensity) * m_pHandlingData->m_fCollisionDamageMultiplier * 0.6f * collForceMult;

        switch (m_nModelIndex) {
        case eModelID::MODEL_SECURICA: {
            if (m_pDamageEntity && m_pDamageEntity->GetStatus() == STATUS_PLAYER) {
                calcCollHealthLoss *= 7.f;
            }
            break;
        }
        case eModelID::MODEL_RCRAIDER:
        case eModelID::MODEL_RCGOBLIN:
        case eModelID::MODEL_RCTIGER: {
            calcCollHealthLoss *= 30.f;
            break;
        }
        }

        if (m_pDamageEntity && ModelIndices::IsRhino(m_pDamageEntity->m_nModelIndex)) {
            calcCollHealthLoss *= 15.f;
        }

        // 0x6A80E1
        if (calcCollHealthLoss > 0.f) {
            if (calcCollHealthLoss > 5.f) {
                if (m_pDamageEntity) {
                    if (m_pDamageEntity->IsVehicle()) {
                        const auto& damagedVeh = *m_pDamageEntity->AsVehicle();

                        if (damagedVeh.m_pDriver && m_pDriver) {
                            // Add collision event
                            {
                                const auto dot = DotProduct(
                                    m_pDamageEntity->AsPhysical()->m_vecMoveSpeed - m_vecMoveSpeed,
                                    m_vecLastCollisionImpactVelocity
                                );
                                if (dot < 0.f) { // Forces are opposite
                                    CEventVehicleDamageCollision event{ this, m_pDamageEntity->AsVehicle(), WEAPON_RAMMEDBYCAR };
                                    m_pDriver->GetEventGroup().Add(&event);
                                } else {
                                    CEventVehicleDamageCollision event{ m_pDamageEntity->AsVehicle(), this, WEAPON_RAMMEDBYCAR };
                                    m_pDamageEntity->AsVehicle()->m_pDriver->GetEventGroup().Add(&event);
                                }
                            }

                            // Make our driver say something reacting to the collision..
                            if (&damagedVeh != FindPlayerVehicle() || damagedVeh.IsMissionVehicle()) {
                                switch (m_vehicleAudio.GetVehicleTypeForAudio()) {
                                case 1:
                                    m_pDriver->Say(CTX_GLOBAL_CRASH_BIKE);
                                    break;
                                case 0:
                                    m_pDriver->Say(CTX_GLOBAL_CRASH_CAR);
                                    break;
                                default:
                                    m_pDriver->Say(CTX_GLOBAL_CRASH_GENERIC);
                                    break;
                                }
                            }
                        }
                    }
                    if (this == FindPlayerVehicle()) {
                        if (const auto p = PickRandomPassenger()) {
                            p->Say(m_pDamageEntity->IsPed() ? CTX_GLOBAL_CAR_HIT_PED : CTX_GLOBAL_CAR_CRASH);
                        }
                    }
                }
                g_InterestingEvents.Add(CInterestingEvents::EType::VEHICLE_DAMAGE, this);
            }

            // Had to re-order all this mess.

            // Decrease m_fHealth
            const auto prevHealth = m_fHealth;

            // 0x6A82CB
            if (this == FindPlayerVehicle()->AsAutomobile()) {
                m_fHealth -= vehicleFlags.bTakeLessDamage ? calcCollHealthLoss / 6.f : calcCollHealthLoss / 2.f;
            } else if (vehicleFlags.bTakeLessDamage) {
                m_fHealth -= calcCollHealthLoss / 12.f;
            } else {
                if (m_pDamageEntity && m_pDamageEntity == FindPlayerVehicle()) {
                    m_fHealth -= calcCollHealthLoss / 1.5f;
                }
                else {
                    m_fHealth -= calcCollHealthLoss / 4.f;
                }
            }
            // DEV_LOG("Health: {.2f} (Loss: {.2f})", m_fHealth, prevHealth - m_fHealth); // NOTSA
            // 0x6A8338
            if (CCheat::IsActive(CHEAT_SMASH_N_BOOM) && m_pDamageEntity && m_pDamageEntity == FindPlayerVehicle()) {
                BlowUpCar(m_pDamageEntity, false);
            }
            else if (m_fHealth <= 0.f && (uint16)prevHealth > 0) { // 0x6A8354
                m_fHealth = 1.f; // 0x6A8374
                if (m_pDamageEntity && ModelIndices::IsRhino(m_pDamageEntity->m_nModelIndex) && FindPlayerVehicle()->AsAutomobile() != this) {
                    BlowUpCar(m_pDamageEntity, false);
                }
            }
        }

        if (!std::ranges::equal(prevLightStates, m_damageManager.GetAllLightsState())) { // Check if a light was smashed
            m_vehicleAudio.AddAudioEvent(AE_LIGHT_SMASH, 0.f); // Yeah, play sound
        }
    }

    // 0x6A83DF
    if (m_fHealth >= 250.f) {
        if (ModelIndices::IsBFInjection(m_nModelIndex)) {
            if (m_fHealth >= 400.f) {
                if (m_fHealth < 600.f) {
                    m_damageManager.SetEngineStatus(100u);
                }
            } else {
                m_damageManager.SetEngineStatus(200u);
            }
        }
    } else if (m_damageManager.GetEngineStatus() < 225u) {
        m_damageManager.SetEngineStatus(225u);

        m_fBurnTimer = 0.f;

        m_pLastDamageEntity = m_pDamageEntity;
        CEntity::SafeRegisterRef(m_pLastDamageEntity);

        if (const auto passenger = PickRandomPassenger()) {
            passenger->Say(CTX_GLOBAL_CAR_FIRE, 1500);
        }
    }
}

//0x6AF1D0
bool CAutomobile::GetTowHitchPos(CVector& outPos, bool bCheckModelInfo, CVehicle* attachTo) {
    if (!bCheckModelInfo) {
        return false;
    }
    outPos.x = 0.0f;
    outPos.y = CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->m_boundBox.m_vecMax.y - 0.5f;
    outPos.z = 0.5f - m_fFrontHeightAboveRoad;
    outPos = m_matrix->TransformPoint(outPos);
    return true;
}

// 0x6AF250
bool CAutomobile::GetTowBarPos(CVector& outPos, bool ignoreModelType, CVehicle* attachTo) {
    switch (m_nModelIndex) {
    case eModelID::MODEL_TOWTRUCK:
    case eModelID::MODEL_TRACTOR: {
        float baseY = -1.05f;
        if (m_nModelIndex == MODEL_TRACTOR) {
            if (attachTo && attachTo->IsSubTrailer() && attachTo->m_nModelIndex != MODEL_FARMTR1) {
                return false;
            }
            baseY = -0.6f;
        } else if (attachTo && attachTo->IsSubTrailer()) {
            return false;
        }

        outPos.x = 0.0f;
        outPos.y = baseY + CModelInfo::GetModelInfo(m_nModelIndex)->GetColModel()->m_boundBox.m_vecMin.y;
        outPos.z = ((1.0f - (float)m_wMiscComponentAngle / (float)TOWTRUCK_HOIST_DOWN_LIMIT) / 2.0f + 0.5f) - m_fFrontHeightAboveRoad;
        outPos = m_matrix->TransformPoint(outPos);
        return true;
    }
    default: { // TODO: Move out to after switch (deindentate)
        if (!m_aCarNodes[CAR_MISC_A]) {
            break;
        }

        switch (m_nModelIndex) {
        case eModelID::MODEL_PETRO:
        case eModelID::MODEL_RDTRAIN:
        case eModelID::MODEL_LINERUN:
        case eModelID::MODEL_ARTICT3: {
            outPos = *RwMatrixGetPos(RwFrameGetLTM(m_aCarNodes[CAR_MISC_A]));
            return true;
        }
        case eModelID::MODEL_UTILITY: {
            if (attachTo && attachTo->m_nModelIndex == MODEL_UTILTR1) {
                outPos = *RwMatrixGetPos(RwFrameGetLTM(m_aCarNodes[CAR_MISC_A]));
                return true;
            }
            break;
        }
        case eModelID::MODEL_BAGGAGE:
        case eModelID::MODEL_TUG:
        case eModelID::MODEL_BAGBOXA:
        case eModelID::MODEL_BAGBOXB: {
            if (attachTo) {
                switch (attachTo->m_nModelIndex) {
                case eModelID::MODEL_BAGBOXA:
                case eModelID::MODEL_BAGBOXB:
                case eModelID::MODEL_TUGSTAIR:
                    outPos = *RwMatrixGetPos(RwFrameGetLTM(m_aCarNodes[CAR_MISC_A]));
                    return true;
                }
            }
            break;
        }
        }
        break;
    }
    }

    return GetTowHitchPos(outPos, ignoreModelType, attachTo);
}

// 0x6B4410
bool CAutomobile::SetTowLink(CVehicle* tractor, bool placeMeOnRoadProperly) {
    if (!tractor) {
        return false;
    }

    if (m_pTowingVehicle) {
        return false;
    }

    switch (m_nStatus) {
    case eEntityStatus::STATUS_PHYSICS:
    case eEntityStatus::STATUS_IS_TOWED:
    case eEntityStatus::STATUS_ABANDONED:
        break;
    case eEntityStatus::STATUS_SIMPLE:
        return false;
    default: {
        CCarCtrl::SwitchVehicleToRealPhysics(this);
        break;
    }
    }

    m_nStatus = STATUS_IS_TOWED;

    m_pTowingVehicle = tractor;
    tractor->RegisterReference(m_pTowingVehicle);

    m_pTowingVehicle->m_pVehicleBeingTowed = this;
    RegisterReference(m_pTowingVehicle->m_pVehicleBeingTowed);

    for (auto&& entity : { AsVehicle(), tractor }) {
        entity->RemoveFromMovingList();
        entity->AddToMovingList();
    }

    if (placeMeOnRoadProperly) {
        switch (tractor->m_nModelIndex) {
        case eModelID::MODEL_TOWTRUCK:
        case eModelID::MODEL_TRACTOR: {
            tractor->AsAutomobile()->m_wMiscComponentAngle = TOWTRUCK_HOIST_UP_LIMIT;
            break;
        }
        }

        SetHeading(tractor->GetHeading());

        CVector towHitchPos{}, towBarPos{};
        if (GetTowHitchPos(towHitchPos, true, this) && tractor->GetTowBarPos(towBarPos, true, this)) {
            SetPosn(towBarPos - (towHitchPos - GetPosition()));
            PlaceOnRoadProperly();
            return true;
        }
    } else {
        UpdateTrailerLink(true, false);
        return true;
    }
    return false;
}

// 0x6A4400
bool CAutomobile::BreakTowLink() {
    if (m_pTowingVehicle) {
        CEntity::ClearReference(m_pTowingVehicle->m_pVehicleBeingTowed);
        CEntity::ClearReference(m_pTowingVehicle);
    }

    switch (m_nStatus) {
    case eEntityStatus::STATUS_IS_TOWED:
    case eEntityStatus::STATUS_IS_SIMPLE_TOWED: {
        if (m_pDriver) { // TODO: Use ternary
            m_nStatus = m_pDriver->IsPlayer() ? eEntityStatus::STATUS_PLAYER : eEntityStatus::STATUS_PHYSICS;
        } else {
            m_nStatus = m_fHealth >= 1.f ? eEntityStatus::STATUS_ABANDONED : eEntityStatus::STATUS_WRECKED;
        }
        return true;
    }
    }
    return false;
}

// 0x6A6090
float CAutomobile::FindWheelWidth(bool bRear) {
    // TODO: Maybe replace this with `bool flagValaue` and use bitfields directly (instead of bitmasks?)
    static constexpr struct { eVehicleHandlingFlags flag; float mult; } mapping[2][4]{
        { // Rear wheel
            { VEHICLE_HANDLING_WHEEL_R_NARROW2, 0.65f },
            { VEHICLE_HANDLING_WHEEL_R_NARROW,  0.8f  },
            { VEHICLE_HANDLING_WHEEL_R_WIDE,    1.1f  },
            { VEHICLE_HANDLING_WHEEL_R_WIDE2,   1.25f }
        },
        { // Front wheel
            { VEHICLE_HANDLING_WHEEL_F_NARROW2, 0.65f },
            { VEHICLE_HANDLING_WHEEL_F_NARROW,  0.8f  },
            { VEHICLE_HANDLING_WHEEL_F_WIDE,    1.1f  },
            { VEHICLE_HANDLING_WHEEL_F_WIDE2,   1.25f }
        }
    };

    const auto& mi = *GetVehicleModelInfo();

    auto wheelWidth = (bRear ? mi.m_fWheelSizeRear : mi.m_fWheelSizeFront) / 2.8f;
    if (m_nModelIndex == eModelID::MODEL_KART) {
        wheelWidth *= 1.5f;
    }

    // Find first flag and apply it, and return that as the result
    for (auto&& [flag, mult] : mapping[bRear ? 1 : 0]) {
        if (m_nHandlingFlagsIntValue & flag) {
            return wheelWidth * mult;
        }
    }

    return wheelWidth;
}

// 0x5D47E0
bool CAutomobile::Save() {
    CVehicle::Save();
    CGenericGameStorage::SaveDataToWorkBuffer(&m_damageManager, sizeof(m_damageManager));
    return true;
}

// 0x5D2980
bool CAutomobile::Load() {
    CVehicle::Load();
    CGenericGameStorage::LoadDataFromWorkBuffer(&m_damageManager, sizeof(m_damageManager));
    return true;
}

// 0x6A0770
void CAutomobile::SetupModelNodes() {
    std::ranges::fill(m_aCarNodes, nullptr);
    CClumpModelInfo::FillFrameArray(m_pRwClump, m_aCarNodes.data());
}

// 0x6A07A0
void CAutomobile::HydraulicControl() {
    if (m_nStatus == STATUS_PHYSICS) { // TODO: switch()
        if (!IsCreatedBy(MISSION_VEHICLE))
            return;

        if (m_vehicleSpecialColIndex < 0)
            return;
    } else if (m_nStatus != STATUS_PLAYER) {
        return;
    }

    if (handlingFlags.bHydraulicNone) {
        return;
    }

    CPlayerPed* driver = nullptr;
    if (m_nStatus == STATUS_PLAYER) {
        if (m_pDriver && m_pDriver->IsPlayer())
            driver = m_pDriver->AsPlayer();
        if (CGameLogic::GameState != GAME_STATE_INITIAL)
            return;
    }
    if (!CVehicle::GetSpecialColModel()) {
        return;
    }

    auto modelInfo = GetVehicleModelInfo();
    float wheelRadius = modelInfo->m_fWheelSizeFront / 2.0f;
    CCollisionData* colData = CVehicle::m_aSpecialColModel[m_vehicleSpecialColIndex].m_pColData;
    auto& hydraulicData = CVehicle::m_aSpecialHydraulicData[m_vehicleSpecialColIndex];
    float normalUpperLimit   = hydraulicData.m_fSuspensionNormalUpperLimit;
    float normalLowerLimit   = hydraulicData.m_fSuspensionNormalLowerLimit;
    float extendedUpperLimit = hydraulicData.m_fSuspensionExtendedUpperLimit;
    float extendedLowerLimit = hydraulicData.m_fSuspensionExtendedLowerLimit;
    float normalSpringLength = normalUpperLimit - normalLowerLimit;
    float extendedSpringLength = extendedUpperLimit - extendedLowerLimit;
    if (hydraulicData.m_fSuspensionExtendedUpperLimit >= 100.0f) {
        float minz = 0.0f;
        for (int32 i = 0; i < colData->m_nNumSpheres; i++) {
            float z = colData->m_pSpheres[i].m_vecCenter.z - colData->m_pSpheres[i].m_fRadius;
            if (z < minz)
                minz = z;
        }

        CVector pos;
        modelInfo->GetWheelPosn(CAR_WHEEL_FRONT_LEFT, pos, false);
        float limitExtend = minz - pos.z;
        normalLowerLimit = m_pHandlingData->m_fSuspensionLowerLimit;
        normalUpperLimit = m_pHandlingData->m_fSuspensionUpperLimit;
        normalSpringLength = normalUpperLimit - normalLowerLimit;
        if (handlingFlags.bHydraulicGeom)
            normalSpringLength *= 1.5f;
        normalUpperLimit = normalSpringLength + normalLowerLimit;
        hydraulicData.m_fSuspensionNormalUpperLimit = normalUpperLimit;
        hydraulicData.m_fSuspensionNormalLowerLimit = normalLowerLimit;
        if (handlingFlags.bHydraulicGeom) {
            extendedUpperLimit = normalUpperLimit - 0.4f;
            extendedLowerLimit = normalLowerLimit - 0.4f;
        } else {
            extendedUpperLimit = normalUpperLimit - 0.2f;
            extendedLowerLimit = normalLowerLimit - 0.2f;
        }
        extendedUpperLimit = std::max(extendedUpperLimit, limitExtend);
        extendedSpringLength = extendedUpperLimit - extendedLowerLimit;
        hydraulicData.m_fSuspensionExtendedUpperLimit = extendedUpperLimit;
        hydraulicData.m_fSuspensionExtendedLowerLimit = extendedLowerLimit;

        // or 0.15f * normalSpringLength
        float normalSpringLength_15_Percent = (1.0f - 0.7f) * 0.5f * normalSpringLength;
        hydraulicData.m_fSuspensionNormalIdleUpperLimit = normalUpperLimit - normalSpringLength_15_Percent;
        hydraulicData.m_fSuspensionNormalIdleLowerLimit = normalLowerLimit + normalSpringLength_15_Percent;
        float wheelHeight = limitExtend + wheelRadius - 0.1f;
        if (wheelHeight < hydraulicData.m_fSuspensionNormalIdleLowerLimit) {
            float height = hydraulicData.m_fSuspensionNormalIdleLowerLimit - wheelHeight;
            hydraulicData.m_fSuspensionNormalIdleUpperLimit -= height;
            hydraulicData.m_fSuspensionNormalIdleLowerLimit -= height;
        }
    }

    bool setPrevRatio = false;
    if (m_wMiscComponentAngle >= 20 || m_fMoveDirection <= 0.02f || m_fGasPedal == 0.0f) {
        if (m_wMiscComponentAngle && m_wMiscComponentAngle < 61 && m_fMoveDirection < 0.01f) {
            m_wMiscComponentAngle--;
            if (m_wMiscComponentAngle == 1)
                m_vehicleAudio.AddAudioEvent(AE_SUSPENSION_OFF, 0.0f);
        }
    } else if (m_wMiscComponentAngle) {
        m_wMiscComponentAngle++;
    } else {
        m_wMiscComponentAngle = 20;
        m_vehicleAudio.AddAudioEvent(AE_SUSPENSION_ON, 0.0f);
        setPrevRatio = true;
    }

    bool suspensionTriggered = false;
    float wheelPositions[4];
    float suspensionChange[4];

    CVector pos;
    if (driver && driver->GetPadFromPlayer()->HornJustDown()) {
        float lineLength = extendedSpringLength + wheelRadius;
        m_wMiscComponentAngle = m_wMiscComponentAngle < 500 ? 500 : 60;
        if (m_wMiscComponentAngle >= 500) {
            for (int32 i = 0; i < 4; i++) {
                CColLine& line = colData->m_pLines[i];
                wheelPositions[i] = line.m_vecStart.z - m_aSuspensionLineLength[i] * m_fWheelsSuspensionCompression[i];
                modelInfo->GetWheelPosn(i, pos, false);
                pos.z += extendedUpperLimit;
                line.m_vecStart = pos;
                pos.z -= lineLength;
                line.m_vecEnd = pos;
                m_aSuspensionSpringLength[i] = extendedSpringLength;
                m_aSuspensionLineLength[i] = lineLength;
                if (m_fWheelsSuspensionCompression[i] < 1.0f) {
                    float compressed = (line.m_vecStart.z - wheelPositions[i]) / lineLength;
                    m_fWheelsSuspensionCompression[i] = std::min(compressed, 1.0f);
                }
            }
            m_vehicleAudio.AddAudioEvent(AE_SUSPENSION_TRIGGER, 0.0f);
            suspensionTriggered = true;
        } else {
            lineLength = normalSpringLength + wheelRadius;
            for (int32 i = 0; i < 4; i++) {
                CColLine& line = colData->m_pLines[i];
                wheelPositions[i] = line.m_vecStart.z - m_fWheelsSuspensionCompression[i] * m_aSuspensionLineLength[i];
                modelInfo->GetWheelPosn(i, pos, false);
                pos.z += normalUpperLimit;
                line.m_vecStart = pos;
                pos.z -= lineLength;
                line.m_vecEnd = pos;
                m_aSuspensionSpringLength[i] = normalSpringLength;
                m_aSuspensionLineLength[i] = lineLength;
                if (m_fWheelsSuspensionCompression[i] < 1.0f) {
                    float compressed = (line.m_vecStart.z - wheelPositions[i]) / lineLength;
                    m_fWheelsSuspensionCompression[i] = std::min(compressed, 1.0f);
                }
            }
            m_vehicleAudio.AddAudioEvent(AE_SUSPENSION_HYDRAULIC, 0.0f);
        }
    }

    if (!suspensionTriggered) {
        std::ranges::fill(suspensionChange, 1.0f);

        if (m_nStatus == STATUS_PHYSICS) {
            for (int32 i = 0; i < 4; i++) {
                suspensionChange[i] = hydraulicData.m_aWheelSuspension[i];
            }
        } else if (driver) {
            if (!driver->GetPadFromPlayer()->GetHydraulicJump()) { // 0x6A0E3D
                float gunLeftRight = driver->GetPadFromPlayer()->GetCarGunLeftRight();
                float gunUpDown = driver->GetPadFromPlayer()->GetCarGunUpDown();
                float jumpFactor = (std::sqrt(gunUpDown * gunUpDown + gunLeftRight * gunLeftRight) * 1.5f) / 128.0f;
                float angle = std::atan2(gunLeftRight, gunUpDown) - DegreesToRadians(45.0f);
                float change_rear_right = std::cos(angle) * jumpFactor;
                float change_front_right = std::sin(angle) * jumpFactor;
                suspensionChange[CAR_WHEEL_FRONT_LEFT]  = std::max(0.0f, -change_rear_right);
                suspensionChange[CAR_WHEEL_REAR_LEFT]   = std::max(0.0f, -change_front_right);
                suspensionChange[CAR_WHEEL_FRONT_RIGHT] = std::max(0.0f, +change_front_right);
                suspensionChange[CAR_WHEEL_REAR_RIGHT]  = std::max(0.0f, +change_rear_right);
            }
        }

        float maxDelta = 0.0f;
        if (m_wMiscComponentAngle < 500u) {
            if (suspensionChange[CAR_WHEEL_FRONT_LEFT] == 0.0f &&
                suspensionChange[CAR_WHEEL_REAR_LEFT] == 0.0f &&
                suspensionChange[CAR_WHEEL_FRONT_RIGHT] == 0.0f &&
                suspensionChange[CAR_WHEEL_REAR_RIGHT] == 0.0f
            ) {
                if (m_wMiscComponentAngle == 0) {
                    normalUpperLimit = hydraulicData.m_fSuspensionNormalIdleUpperLimit;
                    normalLowerLimit = hydraulicData.m_fSuspensionNormalIdleLowerLimit;
                    normalSpringLength = normalUpperLimit - normalLowerLimit;
                }
            } else {
                if (m_wMiscComponentAngle == 0) {
                    std::ranges::fill(suspensionChange, 0.0f);
                }
                m_wMiscComponentAngle = 60;
            }

            for (int32 i = 0; i < 4; i++) {
                CColLine& line = colData->m_pLines[i];
                suspensionChange[i] = std::min(suspensionChange[i], 1.0f);
                float springLength = (extendedSpringLength - normalSpringLength) * suspensionChange[i] + normalSpringLength;
                float lineLength = springLength + wheelRadius;
                wheelPositions[i] = line.m_vecStart.z - m_aSuspensionLineLength[i] * m_fWheelsSuspensionCompression[i];
                modelInfo->GetWheelPosn(i, pos, false);
                pos.z += (extendedUpperLimit - normalUpperLimit) * suspensionChange[i] + normalUpperLimit;
                line.m_vecStart = pos;
                pos.z -= lineLength;
                maxDelta = std::max(fabs(maxDelta), fabs(pos.z - line.m_vecEnd.z));
                line.m_vecEnd = pos;
                m_aSuspensionLineLength[i] = lineLength;
                m_aSuspensionSpringLength[i] = springLength;
                if (m_fWheelsSuspensionCompression[i] < 1.0f) {
                    float compressed = (line.m_vecStart.z - wheelPositions[i]) / lineLength;
                    m_fWheelsSuspensionCompression[i] = std::min(compressed, 1.0f);
                }
            }
        } else {
            if (m_wMiscComponentAngle < 504u) {
                m_wMiscComponentAngle++;
            }

            for (int32 i = 0; i < 4; i++) {
                CColLine& line = colData->m_pLines[i];
                suspensionChange[i] = std::min(suspensionChange[i], 1.0f);
                float springLength = (normalSpringLength - extendedSpringLength) * suspensionChange[i] + extendedSpringLength;
                wheelPositions[i] = line.m_vecStart.z - m_aSuspensionLineLength[i] * m_fWheelsSuspensionCompression[i];
                modelInfo->GetWheelPosn(i, pos, false);
                pos.z += (normalUpperLimit - extendedUpperLimit) * suspensionChange[i] + extendedUpperLimit;
                float lineLength = springLength + wheelRadius;
                line.m_vecStart = pos;
                pos.z -= lineLength;
                maxDelta = std::max(std::fabs(maxDelta), std::fabs(pos.z - line.m_vecEnd.z));
                line.m_vecEnd = pos;
                m_aSuspensionLineLength[i] = lineLength;
                m_aSuspensionSpringLength[i] = springLength;
                if (m_fWheelsSuspensionCompression[i] < 1.0f) {
                    float compressed = (line.m_vecStart.z - wheelPositions[i]) / lineLength;
                    m_fWheelsSuspensionCompression[i] = std::min(compressed, 1.0f);
                }
            }
        }
        float limitDiff = extendedLowerLimit - normalLowerLimit;
        if (limitDiff != 0.0f && std::fabs(maxDelta / limitDiff) > 0.01f) {
            float f = (limitDiff + maxDelta) * 0.5f / limitDiff;
            f = std::clamp(f, 0.0f, 1.0f);
            if (f < 0.4f || f > 0.6f)
                setPrevRatio = true;
            if (f < 0.5f - CTimer::GetTimeStep() * 0.05f)
                m_vehicleAudio.AddAudioEvent(AE_SUSPENSION_HYDRAULIC, 0.0f);
            else if (f > CTimer::GetTimeStep() * 0.05f + 0.5f)
                m_vehicleAudio.AddAudioEvent(AE_SUSPENSION_TRIGGER, 0.0f);
            else if (f > CTimer::GetTimeStep() * 0.025f + 0.5f)
                m_vehicleAudio.AddAudioEvent(AE_SUSPENSION_HYDRAULIC, 0.0f);
        }
    }

    if (setPrevRatio || suspensionTriggered) {
        for (int32 i = 0; i < MAX_CARWHEELS; i++) {
            const float t = 1.f - m_aSuspensionSpringLength[i] / m_aSuspensionLineLength[i];
            // same as `(m_fWheelsSuspensionCompression[i] - 1.f + k) / k` where `k = m_aSuspensionSpringLength[i] / m_aSuspensionLineLength[i]`
            m_fWheelsSuspensionCompressionPrev[i] = (m_fWheelsSuspensionCompression[i] - t) / (1.0f - t); 
        }
    }
    std::ranges::fill(hydraulicData.m_aWheelSuspension, 0.0f);
}

static constexpr uint16 DEFAULT_COLLISION_EXTENDLIMIT = 2500; // 0x8D314C

// 0x6A1460
bool CAutomobile::UpdateMovingCollision(float angle) {
    if (ModelIndices::HasMiscComponent(m_nModelIndex)) {
        m_wMiscComponentAnglePrev = m_wMiscComponentAngle;
    }

    CPad* pad = nullptr;
    if (m_nStatus == STATUS_PLAYER) {
        CPlayerPed* driver = m_pDriver->AsPlayer();
        if (!driver || !driver->IsPlayer() || CGameLogic::GameState)
            return false;

        pad = driver->GetPadFromPlayer();
    }
    else if (!IsCreatedBy(MISSION_VEHICLE) || angle < 0.0f
        || !ModelIndices::HasMiscComponent(m_nModelIndex) && !ModelIndices::IsFireTruckLadder(m_nModelIndex))
    {
        return false;
    }

    if (!ModelIndices::IsCementTruck(m_nModelIndex) && !ModelIndices::IsFireTruckLadder(m_nModelIndex)) {
        CColModel* colModel = GetVehicleModelInfo()->GetColModel();
        CCollisionData* colData = colModel->m_pColData;
        if (!GetSpecialColModel()) {
            return false;
        }
        CColModel& specialColModel = m_aSpecialColModel[m_vehicleSpecialColIndex];
        CCollisionData* specialColData = specialColModel.m_pColData;
        m_wMiscComponentAnglePrev = m_wMiscComponentAngle;
        if (angle < 0.0f) {
            if (!pad || std::fabs((float)pad->GetCarGunUpDown()) <= 10.0f)
                return false;
        }
        if (angle >= 0.0f) {
            m_wMiscComponentAngle = static_cast<uint16>(angle * DEFAULT_COLLISION_EXTENDLIMIT);
        }
        else {
            float colAngleMult = 10.0f;
            if (ModelIndices::IsForklift(m_nModelIndex) ) {
                if ((float)pad->GetCarGunUpDown() >= 0.0f)
                    colAngleMult *= -2;
                else
                    colAngleMult *= -1;
            }

            m_wMiscComponentAngle += static_cast<uint16>(((float)pad->GetCarGunUpDown() / 128.0f) * colAngleMult * CTimer::GetTimeStep());
            m_wMiscComponentAngle = std::clamp<uint16>(m_wMiscComponentAngle, 0, DEFAULT_COLLISION_EXTENDLIMIT);
        }

        CMatrix rotMatrix;

        // 0x6A174A
        RwFrame* carNodeMisc = nullptr;
        if (ModelIndices::IsDozer(m_nModelIndex)) {
            carNodeMisc = m_aCarNodes[CAR_MISC_A];
            if (carNodeMisc)
                rotMatrix.SetRotateX((float)m_wMiscComponentAngle * CAutomobile::DOZER_COL_ANGLE_MULT);
        }
        else if (ModelIndices::IsDumper(m_nModelIndex) && IsSubMonsterTruck()) {
            carNodeMisc = m_aCarNodes[CAR_MISC_C];
            if (carNodeMisc)
                rotMatrix.SetRotateX((float)m_wMiscComponentAngle * CMonsterTruck::DUMPER_COL_ANGLEMULT);
        }
        else if (ModelIndices::IsAndromada(m_nModelIndex)) {
            carNodeMisc = m_aCarNodes[CAR_MISC_E];
            if (carNodeMisc)
                rotMatrix.SetRotateX((float)m_wMiscComponentAngle * CPlane::ANDROM_COL_ANGLE_MULT);
        }
        else if (ModelIndices::IsForklift(m_nModelIndex)) {
            carNodeMisc = m_aCarNodes[CAR_MISC_A];
            if (carNodeMisc)
                rotMatrix.SetTranslate(CVector(0.0f, 0.0f, (float)m_wMiscComponentAngle * CAutomobile::FORKLIFT_COL_ANGLE_MULT));
        }
        else if (ModelIndices::IsPacker(m_nModelIndex)) {
            carNodeMisc = m_aCarNodes[CAR_MISC_A];
            if (carNodeMisc)
                rotMatrix.SetRotateX((float)m_wMiscComponentAngle * CAutomobile::PACKER_COL_ANGLE_MULT);
        }

        CVector componentPos;
        if (carNodeMisc && !ModelIndices::IsForklift(m_nModelIndex))
            componentPos = RwFrameGetMatrix(carNodeMisc)->pos;

        float minZ = 1000.0f;
        float maxZ = -1000.0f;
        for (uint16 triIndx = 0; triIndx < specialColData->m_nNumTriangles; triIndx++) {
            CColTriangle& specialColTriangle = specialColData->m_pTriangles[triIndx];
            if (specialColTriangle.m_nMaterial == SURFACE_CAR_MOVINGCOMPONENT) {
                const CColTriangle& colTriangle = colData->m_pTriangles[triIndx];
                for (int32 i = 0; i < 3; i++) {
                    CVector vertexPos = UncompressVector(colData->m_pVertices[colTriangle.m_vertIndices[i]]);
                    CVector distance  = vertexPos - componentPos;
                    vertexPos = rotMatrix.TransformPoint(distance) + componentPos;
                    specialColData->m_pVertices[specialColTriangle.m_vertIndices[i]] = CompressVector(vertexPos);
                    if (maxZ < vertexPos.z)
                        maxZ = vertexPos.z;
                    else if (minZ > vertexPos.z)
                        minZ = vertexPos.z;
                }
            }
        }

        if (specialColData->m_pTrianglePlanes) {
            for (int32 i = 0; i < specialColData->m_nNumTriangles; i++) {
                CColTrianglePlane& trianglePlane = specialColData->m_pTrianglePlanes[i];
                trianglePlane.Set(specialColData->m_pVertices, specialColData->m_pTriangles[i]);
            }
        }

        for (uint16 i = 0; i < specialColData->m_nNumSpheres; i++) {
            CColSphere& specialColSphere = specialColData->m_pSpheres[i];
            if (specialColSphere.m_Surface.m_nMaterial == SURFACE_CAR_MOVINGCOMPONENT) {
                CColSphere& colSphere = colData->m_pSpheres[i];
                CVector distance = colSphere.m_vecCenter - componentPos;
                specialColSphere.m_vecCenter = (rotMatrix.TransformPoint(distance)) + componentPos;
                const float newMaxZ = specialColSphere.m_fRadius + specialColSphere.m_vecCenter.z;
                const float newMinZ = specialColSphere.m_vecCenter.z - specialColSphere.m_fRadius;
                if (maxZ < newMaxZ)
                    maxZ = newMaxZ;
                else if (minZ > newMinZ)
                    minZ = newMinZ;
            }
        }
        if (colModel->GetBoundingBox().m_vecMax.z < maxZ)
            specialColModel.GetBoundingBox().m_vecMax.z = maxZ;
        if (colModel->GetBoundingBox().m_vecMin.z > minZ)
            specialColModel.GetBoundingBox().m_vecMin.z = minZ;
        return true;
    }

    m_wMiscComponentAnglePrev = m_wMiscComponentAngle;
    if (angle >= 0.0f) {
        m_wMiscComponentAngle = static_cast<uint16>(angle * DEFAULT_COLLISION_EXTENDLIMIT);
        return false;
    }

    if (!pad)
        return false;

    if ((float)pad->GetCarGunUpDown() < -10.0f) {
        m_wMiscComponentAngle -= static_cast<uint16>(2 * ((float)pad->GetCarGunUpDown() / 128.0f) * 10.0f * CTimer::GetTimeStep());
        m_wMiscComponentAngle = std::min(m_wMiscComponentAngle, DEFAULT_COLLISION_EXTENDLIMIT);
        return false;
    }

    if (!m_wMiscComponentAngle)
        return false;

    m_wMiscComponentAngle -= static_cast<uint16>((float(pad->GetCarGunUpDown() + 100) * 10.0f * CTimer::GetTimeStep()) / 128.0f);
    if (static_cast<int16>(m_wMiscComponentAngle) < 0)
        m_wMiscComponentAngle = 0;

    return false;
}

// 0x6A2150
float CAutomobile::GetMovingCollisionOffset() {
    if (m_wMiscComponentAngle) { // TODO: Invert
        switch (m_nModelIndex) {
        case eModelID::MODEL_DUMPER:
            return m_aCarNodes[CAR_MISC_C] ? (float)m_wMiscComponentAngle * CMonsterTruck::DUMPER_COL_ANGLEMULT : 0.0f;

        case eModelID::MODEL_PACKER:
            return (float)m_wMiscComponentAngle / -10000.f;

        case eModelID::MODEL_DOZER:
            return m_aCarNodes[CAR_MISC_A] ? (float)m_wMiscComponentAngle / 5000.f : 0.0f;

        case eModelID::MODEL_ANDROM:
            return m_aCarNodes[CAR_MISC_E] ? (float)m_wMiscComponentAngle * CPlane::ANDROM_COL_ANGLE_MULT : 0.0f;

        case eModelID::MODEL_FORKLIFT:
            return (float)m_wMiscComponentAngle * 0.0006f; // / 1666.66?
        }
    }
    return 0.f;
}

// 0x6A2390
void CAutomobile::TellHeliToGoToCoors(float x, float y, float z, float altitudeMin, float altitudeMax) {
    m_autoPilot.m_vecDestinationCoors = CVector{ x, y, z };
    m_autoPilot.SetCarMission(MISSION_HELI_FLYTOCOORS);
    m_autoPilot.SetCruiseSpeed(100);

    AsHeli()->m_fMinAltitude = altitudeMin;
    AsHeli()->m_fMaxAltitude = altitudeMax;

    m_nStatus = STATUS_PHYSICS;

    if (m_fAircraftGoToHeading == 0.f) {
        m_fAircraftGoToHeading = CGeneral::GetATanOfXY(m_matrix->GetForward().x, m_matrix->GetForward().y) + PI;
        if (m_fAircraftGoToHeading > TWO_PI) {
            auto heading = m_fAircraftGoToHeading;
            do
                heading -= TWO_PI;
            while (heading > TWO_PI);
            m_fAircraftGoToHeading = heading;
        }
        m_fAircraftGoToHeading -= TWO_PI; // TODO: Is dis some inlined function? CGeneral::LimitRadianAngle ?
    }
}

// 0x6A2450
void CAutomobile::SetHeliOrientation(float angle) {
    m_fForcedOrientation = angle;
}

// 0x6A2460
void CAutomobile::ClearHeliOrientation() {
    m_fForcedOrientation = 0.0f;
}

// 0x6A2470
void CAutomobile::TellPlaneToGoToCoors(float x, float y, float z, float altitudeMin, float altitudeMax) {
    m_autoPilot.m_vecDestinationCoors = CVector{ x, y, z };
    m_autoPilot.SetCarMission(MISSION_PLANE_FLYTOCOORS);
    m_autoPilot.SetCruiseSpeed(0);

    AsPlane()->m_minAltitude = std::max(altitudeMin, z);
    AsPlane()->m_maxAltitude = altitudeMax;

    m_nStatus = STATUS_PHYSICS;

    if (m_fAircraftGoToHeading == 0.f) {
        m_fAircraftGoToHeading = CGeneral::GetATanOfXY(m_matrix->GetForward().x, m_matrix->GetForward().y);
    }

    vehicleFlags.bEngineOn = true;
}

// unused
// 0x6A2510
void CAutomobile::HideAllComps() {
    // NOP
}

// unused
// 0x6A2520
void CAutomobile::ShowAllComps() {
    // NOP
}

// 0x6A2530
void CAutomobile::SetRandomDamage(bool bRemoveStuff) {
    //> Bugfix shit
    const auto FixedRandom = [&](int32 x, int32 y) {
        // I'm very sure somebody fell into the trap of the shit naming of `GetRandomNumberInRange`
        // (The int version of it is exclusive, while the float one is inclusive)
        // Reason being that at `0x6A260B` the possible interval is [0, 1), that is, in integer terms a fat `0` :D
        // Obviously, they intended it to be `[0, 1]`
#ifdef FIX_BUGS
        return (size_t)CGeneral::GetRandomNumberInRange(x, y + 1);
#else
        return (size_t)CGeneral::GetRandomNumberInRange(x, y);
#endif
    };

    //> Processes a group of components (panels/doors)
    const auto Process = [
        this,
        IsCompDamageable = [vs = GetVehicleModelInfo()->GetVehicleStruct()](auto nodeIdx) {
            return vs->IsComponentDamageable(nodeIdx);
        }
    ](
        size_t pickedbs, 
        size_t numToDmg,
        size_t numTotal,
        auto   GetNodeIdx,
        auto   IsDamStateOk,
        auto   SetAsDamaged
    ) {
        /**
        * Get the `n`-th component that wasn't yet picked
        * @param n 0-based number
        * @param pickedbs Bitset of components that were picked previously
        */
        const auto GetNthUnpicked = [&](size_t n) {
            if (pickedbs == 0) {
                return n; // No pickedbs components so we can directly return
            }
            for (size_t i = 0; i < numTotal; i++) {
                if ((pickedbs & (1 << i)) == 0) {
                    if (n-- == 0) {
                        return i;
                    }
                }
            }
            NOTSA_UNREACHABLE(); // This is reached if the first `ncomps` bits are all set - Should never happen
        };

        for (size_t i = 0u; i < numToDmg; i++) {
            const auto pick = GetNthUnpicked((size_t)CGeneral::GetRandomNumberInRange(0, (int32)(numToDmg - i)));
            pickedbs |= 1 << pick; // We mark here for simplicity
            if (!IsDamStateOk(pick)) {
                continue;
            }
            const auto nodeIdx = GetNodeIdx(pick);
            if (!IsCompDamageable(nodeIdx)) {
                continue;
            }
            SetAsDamaged(pick);
            SetComponentVisibility(m_aCarNodes[nodeIdx], ATOMIC_IS_DAM_STATE);
        }
    };

    //> Doors
    const auto numDoorsToDmg = FixedRandom(0, bRemoveStuff ? MAX_DOORS : 1);
    Process(
        0,
        numDoorsToDmg,
        MAX_DOORS,
        [](auto i)     { return CDamageManager::GetCarNodeIndexFromDoor((eDoors)i); },
        [this](auto i) { return m_damageManager.GetDoorStatus((eDoors)i) == DAMSTATE_OK; },
        [this](auto i) { return m_damageManager.SetDoorStatus((eDoors)i, DAMSTATE_DAMAGED); }
    );

    //> Panels (With a few exceptions)
    const auto numPanelsToDmg = numDoorsToDmg == 0
        ? bRemoveStuff ? FixedRandom(1, 4) : 1 // The 4 here and below comes from MAX_PANELS - 3 (As 3 panels are never damaged, see below)
        : FixedRandom(0, bRemoveStuff ? 4 : 1);
    Process(
        1 << REAR_LEFT_PANEL | 1 << REAR_RIGHT_PANEL | 1 << WINDSCREEN_PANEL, // These are never damaged
        numPanelsToDmg,
        MAX_PANELS,
        [](auto i)     { return CDamageManager::GetCarNodeIndexFromPanel((ePanels)i); },
        [this](auto i) { return m_damageManager.GetPanelStatus((ePanels)i) == DAMSTATE_OK; },
        [this](auto i) { return m_damageManager.SetPanelStatus((ePanels)i, DAMSTATE_DAMAGED); }
    );
}

// 0x6A27F0
void CAutomobile::SetTotalDamage(bool randomness) {
    const auto IsComponentDamageable = [vehStruct = GetVehicleModelInfo()->m_pVehicleStruct](int32 component) {
        return vehStruct->IsComponentDamageable(component);
    };

    // Doors
    for (auto i = 0u; i < MAX_DOORS; i++) {
        const auto nodeIdx{ CDamageManager::GetCarNodeIndexFromDoor((eDoors)i) };
        if (const auto doorFrame{ m_aCarNodes[nodeIdx] }) {
            if (!randomness || CGeneral::GetRandomNumberInRange(0, 3)) {
                if (m_damageManager.GetDoorStatus((eDoors)i) == DAMSTATE_OK && IsComponentDamageable(nodeIdx)) {
                    m_damageManager.SetDoorStatus((eDoors)i, DAMSTATE_DAMAGED);
                    SetComponentVisibility(doorFrame, ATOMIC_IS_DAM_STATE);
                }
            } else {
                m_damageManager.SetDoorStatus((eDoors)i, DAMSTATE_NOTPRESENT);
                SetComponentVisibility(doorFrame, ATOMIC_IS_NOT_PRESENT);
            }
        }
    }

    // Panels
    for (auto i = 0u; i < MAX_PANELS; i++) { // TODO: Maybe use array of panel enums instead?
        switch ((ePanels)i) {
        case ePanels::REAR_LEFT_PANEL:
        case ePanels::REAR_RIGHT_PANEL:
            break;

        default: {
            const auto nodeIdx{ CDamageManager::GetCarNodeIndexFromPanel((ePanels)i) };
            if (const auto panelFrame{ m_aCarNodes[nodeIdx] }) {
                if (!randomness || CGeneral::GetRandomNumberInRange(0, 3)) {
                    if (m_damageManager.GetPanelStatus((ePanels)i) == DAMSTATE_OK && IsComponentDamageable(nodeIdx)) {
                        m_damageManager.SetPanelStatus((ePanels)i, DAMSTATE_OPENED);
                        SetComponentVisibility(panelFrame, ATOMIC_IS_DAM_STATE);
                    }
                } else {
                    m_damageManager.SetPanelStatus((ePanels)i, DAMSTATE_OPENED_DAMAGED);
                    SetComponentVisibility(panelFrame, ATOMIC_IS_NOT_PRESENT);
                }
            }
            break;
        }
        }
    }
}

// 0x6A2980
RpMaterial* DisableMatFx(RpMaterial* material, void* data) {
    RpMatFXMaterialSetEffects(material, rpMATFXEFFECTNULL);
    return material;
}

// 0x6A29A0
void CAutomobile::ReduceHornCounter() {
    if (m_nHornCounter) {
        m_nHornCounter--;
    }
}

static RwTexture*& renderLicensePlateTexture{ *(RwTexture**)0xC1BFD8 };

// 0x6A2F00
void CAutomobile::CustomCarPlate_BeforeRenderingStart(const CVehicleModelInfo& mi)
{
    if (mi.m_pPlateMaterial) {
        renderLicensePlateTexture = RpMaterialGetTexture(mi.m_pPlateMaterial);
        RwTextureAddRef(renderLicensePlateTexture);
        RpMaterialSetTexture(mi.m_pPlateMaterial, m_pCustomCarPlate);
    }
}

// 0x6A2F30
void CAutomobile::CustomCarPlate_AfterRenderingStop(CVehicleModelInfo* model) {
    if (model->m_pPlateMaterial) {
        RpMaterialSetTexture(model->m_pPlateMaterial, renderLicensePlateTexture);
        RwTextureDestroy(renderLicensePlateTexture);
        renderLicensePlateTexture = nullptr;
    }
}

/*
 * Check if plyrveh is in air
 * @addr 0x6A2F70
 */
bool CAutomobile::GetAllWheelsOffGround() const {
    return m_nWheelsOnGround == 0;
}

// 0x6A2F80
void CAutomobile::DebugCode() {
    auto* mi = GetVehicleModelInfo();
    auto* vehicle = FindPlayerVehicle();
    auto* remoteVehicle = FindPlayerInfo().m_pRemoteVehicle;
    if (vehicle == this && !remoteVehicle || remoteVehicle == this) {
        auto* handling      = gHandlingDataMgr.GetVehiclePointer(mi->m_nHandlingId);
        m_fMass             = handling->m_fMass;
        m_fTurnMass         = handling->m_fTurnMass;
        m_fBuoyancyConstant = handling->m_fBuoyancyConstant;
        m_fAirResistance    = GetDefaultAirResistance(); // todo: check handling. does handling in f() equal handling above
    }
}

// 0x6A3580
void CAutomobile::FixTyre(eWheels wheel) {
    m_damageManager.SetWheelStatus(wheel, WHEEL_STATUS_OK);
}

// 0x6A35A0
void CAutomobile::FixDoor(int32 nodeIndex, eDoors door) {
    if (!m_pHandlingData->m_bNoDoors) { // TODO: Invert
        m_doors[door].Open(0.f);
        m_damageManager.SetDoorStatus(door, DAMSTATE_OK);
        if (const auto frame = m_aCarNodes[nodeIndex]) {
            SetComponentVisibility(frame, ATOMIC_IS_OK_STATE);

            // Reset it's matrix
            CMatrix mat{ RwFrameGetLTM(frame), false };
            mat.SetTranslate(GetPosition());
            mat.UpdateRW();
        }
    }
}

// 0x6A3670
void CAutomobile::FixPanel(eCarNodes nodeIndex, ePanels panel) {
    m_damageManager.SetPanelStatus(panel, DAMSTATE_OK);

    // Remove any bouncing panels belonging to this frame
    for (auto&& panelx : m_panels) {
        if (panelx.m_nFrameId == nodeIndex) {
            panelx.ResetPanel();
        }
    }

    if (const auto frame = m_aCarNodes[nodeIndex]) { // Same code as in FixDoor.. Maybe this was a standalone function, like "FixNode"?
        SetComponentVisibility(frame, ATOMIC_IS_OK_STATE);

        CMatrix mat{ RwFrameGetLTM(frame), false };
        mat.SetTranslate(GetPosition());
        mat.UpdateRW();
    }
}

// 0x6A3740
void CAutomobile::SetTaxiLight(bool enable) {
    autoFlags.bTaxiLight = enable;
}

// 0x6A3760
void CAutomobile::SetAllTaxiLights(bool enable) {
    m_sAllTaxiLights = enable;
}

// 0x6A3820
void CAutomobile::PlayHornIfNecessary() {
    if ((m_autoPilot.carCtrlFlags.bHonkAtPed || m_autoPilot.carCtrlFlags.bHonkAtCar) && !HasCarStoppedBecauseOfLight()) {
        PlayCarHorn();
    }
}

// 0x6A3860
void CAutomobile::SetBusDoorTimer(uint32 timerEndDelta, bool setAsStartedInPast) {
    const auto time = setAsStartedInPast ? CTimer::GetTimeInMS() - 500u : CTimer::GetTimeInMS();
    m_nBusDoorTimerStart = time;
    m_nBusDoorTimerEnd   = time + std::max(1000u, timerEndDelta);
}

// 0x6A38A0
void CAutomobile::ProcessAutoBusDoors() {
    if (m_nBusDoorTimerEnd <= CTimer::GetTimeInMS()) {
        if (m_nBusDoorTimerStart) {
            static constexpr struct { eDoors door; tComponent comp; uint32 flagMask; } doors[]{
                { eDoors::DOOR_LEFT_FRONT,  tComponent::COMPONENT_DOOR_RR, 1 }, // TODO: `COMPONENT_DOOR_RR` doesn't match up with `DOOR_LEFT_FRONT`
                { eDoors::DOOR_RIGHT_FRONT, tComponent::COMPONENT_DOOR_RF, 4 }
            };

            for (auto&& [door, comp, flagMask] : doors) {
                if (!IsDoorMissing(door) && (m_nGettingInFlags & flagMask)) {
                    OpenDoor(nullptr, comp, door, 0.f, true);
                }
            }

            m_nBusDoorTimerEnd = 0;
            m_nBusDoorTimerStart = 0;
        }
    } else if (m_nBusDoorTimerEnd && CTimer::GetTimeInMS() > m_nBusDoorTimerEnd - 500) {
        if (!IsDoorMissing(eDoors::DOOR_LEFT_FRONT) && (m_nGettingInFlags & 1)) {
            const auto OpenThisDoor = [this](float ratio) {
                OpenDoor(nullptr, COMPONENT_DOOR_RR, DOOR_LEFT_FRONT, ratio, true); // TODO: `COMPONENT_DOOR_RR` doesn't match up with `DOOR_LEFT_FRONT`
            };

            if (IsDoorClosed(eDoors::DOOR_LEFT_FRONT)) {
                m_nBusDoorTimerStart = 0;
                OpenThisDoor(0.f);
            } else {
                OpenThisDoor(1.f - (float)(CTimer::GetTimeInMS() - m_nBusDoorTimerEnd + 500) / 500.f);
            }
        }

        if (!IsDoorMissing(eDoors::DOOR_RIGHT_FRONT) && (m_nGettingInFlags & 4)) {
            if (IsDoorClosed(eDoors::DOOR_RIGHT_FRONT)) {
                m_nBusDoorTimerEnd = CTimer::GetTimeInMS();
            }
            OpenDoor(nullptr, COMPONENT_DOOR_RF, DOOR_RIGHT_FRONT, 0.f, true);
        }
    }
}

// 0x6A3A60
void CAutomobile::BoostJumpControl() {
    if (m_pDriver && m_pDriver->IsPlayer()) { // TODO: Invert
        if (const auto playerPad = m_pDriver->AsPlayer()->GetPadFromPlayer()) {
            if (playerPad->HornJustDown() && m_fWheelsSuspensionCompression[0] < 1.f) {
                ApplyMoveForce(CVector{ 0.f, 0.f, 1.f } * (m_fMass * 0.15f));
                ApplyTurnForce(m_matrix->GetUp() * (m_fTurnMass / 100.f), m_matrix->GetForward());
            }
        }
    }
}

// 0x6A3BD0
void CAutomobile::DoNitroEffect(float power) {
    // TODO: Refactor

    const auto& mi = *GetVehicleModelInfo();
    CVector exhaustPosition = mi.GetModelDummyPosition(DUMMY_EXHAUST);

    bool firstExhaustSubmergedInWater = false;
    bool secondExhaustSubmergedInWater = false;
    float level = 0.0f;
    if (physicalFlags.bTouchingWater) {
        CVector point = m_matrix->TransformPoint(exhaustPosition);
        if (CWaterLevel::GetWaterLevel(point.x, point.y, point.z, level, true, nullptr)) {
            if (level >= point.z)
                firstExhaustSubmergedInWater = true;
        }
    }
    CVector secondExhaustPosition;
    if (m_pHandlingData->m_bDoubleExhaust) {
        secondExhaustPosition = exhaustPosition;
        secondExhaustPosition.x *= -1.0f;
        if (!physicalFlags.bTouchingWater) {
            CVector point = m_matrix->TransformPoint(secondExhaustPosition);
            if (CWaterLevel::GetWaterLevel(point, level, true)) {
                if (level >= point.z)
                    secondExhaustSubmergedInWater = true;
            }
        }
    }

    RwMatrix* rwMatrix = GetModellingMatrix();

    FxSystem_c* firstExhaustFxSystem = m_exhaustNitroFxSystem[0];
    if (firstExhaustFxSystem) {
        firstExhaustFxSystem->SetConstTime(1, std::fabs(power));
        if (firstExhaustFxSystem->GetPlayStatus() == eFxSystemPlayStatus::FX_PLAYING && firstExhaustSubmergedInWater)
            firstExhaustFxSystem->Stop();
        else if (firstExhaustFxSystem->GetPlayStatus() == eFxSystemPlayStatus::FX_STOPPED && !firstExhaustSubmergedInWater)
            firstExhaustFxSystem->Play();
    } else if (!firstExhaustSubmergedInWater && rwMatrix) {
        firstExhaustFxSystem = g_fxMan.CreateFxSystem("nitro", exhaustPosition, rwMatrix, true);
        m_exhaustNitroFxSystem[0] = firstExhaustFxSystem;
        if (firstExhaustFxSystem) {
            firstExhaustFxSystem->SetLocalParticles(true);
            firstExhaustFxSystem->Play();
        }
    }

    if (m_pHandlingData->m_bDoubleExhaust) {
        FxSystem_c* secondExhaustFxSystem = m_exhaustNitroFxSystem[1];
        if (secondExhaustFxSystem) {
            secondExhaustFxSystem->SetConstTime(1, std::fabs(power));
            if (secondExhaustFxSystem->GetPlayStatus() == eFxSystemPlayStatus::FX_PLAYING && secondExhaustSubmergedInWater)
                secondExhaustFxSystem->Stop();
            else if (secondExhaustFxSystem->GetPlayStatus() == eFxSystemPlayStatus::FX_STOPPED && !secondExhaustSubmergedInWater)
                secondExhaustFxSystem->Play();
        } else if (!firstExhaustSubmergedInWater && rwMatrix) {
            secondExhaustFxSystem = g_fxMan.CreateFxSystem("nitro", secondExhaustPosition, rwMatrix, true);
            m_exhaustNitroFxSystem[1] = secondExhaustFxSystem;
            if (secondExhaustFxSystem) {
                secondExhaustFxSystem->SetLocalParticles(1);
                secondExhaustFxSystem->Play();
            }
        }
    }
}

// 0x6A3E60
void CAutomobile::StopNitroEffect() {
    for (auto&& fx : m_exhaustNitroFxSystem) {
        FxSystem_c::SafeKillAndClear(fx);
    }
}

// 0x6A3EA0
void CAutomobile::NitrousControl(int8 boost) {
    CPad* pad = nullptr;
    if (m_nStatus == STATUS_PLAYER && m_pDriver->IsPlayer()) {
        pad = m_pDriver->AsPlayer()->GetPadFromPlayer();
    }

    if (boost > 0) {
        handlingFlags.bNosInst = true;
        m_fTireTemperature = 1.0f;
        m_nNitroBoosts = boost;
        StopNitroEffect();
        return;
    }

    if (boost < 0) {
        handlingFlags.bNosInst = false;
        m_fTireTemperature = 1.0f; // todo: FIX_BUGS: 0.0f?
        m_nNitroBoosts = 0;
        StopNitroEffect();
        return;
    }

    if (m_fTireTemperature == 1.0f && m_nNitroBoosts > 0) {
        if (m_nStatus != STATUS_PHYSICS) {
            if (   !pad
                || !pad->GetCarGunFired()
                || pad->GetLookLeft()
                || pad->GetLookRight()
                || pad->GetLookBehindForCar()
            ) {
                StopNitroEffect();
                return;
            }
        }

        m_fTireTemperature = -0.000001f; // Just set some small negative value

        if (m_nNitroBoosts >= 101) { // todo: magic number
            StopNitroEffect();
            return;
        }

        m_nNitroBoosts -= 1;
        StopNitroEffect();
        return;
    }

    if (m_fTireTemperature >= 0.f) {
        const auto a = std::max(0.25f, 1.f - m_fGasPedal);
        const auto b = a * (CTimer::GetTimeStep() / 100.f) + m_fTireTemperature;
        m_fTireTemperature = std::min(1.0f, b);
        const auto power = (1.0f - m_fTireTemperature) / 2.f;
        DoNitroEffect(power);
        return;
    }

    m_fTireTemperature -= CTimer ::GetTimeStep() / 1000.f;
    if (m_fTireTemperature < -1.f) {
        m_fTireTemperature = 0.000001f; // Just set some small positive value.
        if (!m_nNitroBoosts) {
            handlingFlags.bNosInst = false;
            RemoveUpgrade(eVehicleUpgradePosn::UPGRADE_NITRO);
            m_fTireTemperature = 1.0f;
        }
    }
    DoNitroEffect(
        m_fGasPedal <= 0.0f
            ? 0.5f
            : std::abs(m_fGasPedal) / 2.0f + 0.5f
    );
}

template<typename T>
auto Get(CEntity** entities, size_t numEntities)
    requires std::is_base_of_v<CEntity, T>
{
    return (
        std::span{ entities, numEntities }
      | std::views::transform([](auto&& entity) { return reinterpret_cast<T*>(entity); })
    );
};

// 0x6A40F0
void CAutomobile::TowTruckControl() {
    const auto UP_SPEED   = 6.0f;
    const auto DOWN_SPEED = 2.0f;

    if (m_nStatus != STATUS_PLAYER) {
        return;
    }

    if (!m_pDriver || !m_pDriver->IsPlayer()) {
        return;
    }

    const auto driversPad = m_pDriver->AsPlayer()->GetPadFromPlayer();
    if (!driversPad) {
        return;
    }

    // Update misc door. angle
    const auto carUpDown = driversPad->GetCarGunUpDown();
    if (std::fabs(carUpDown) > 10.f) {
        const auto speed = carUpDown > 0 ? DOWN_SPEED : UP_SPEED;
        const auto step = (uint16)(float(carUpDown) * speed * CTimer::GetTimeStep());

        while (true) {
            // up
            if (carUpDown > 0) {
                m_wMiscComponentAngle = std::max(
                    m_pVehicleBeingTowed ? TOWTRUCK_HOIST_UP_LIMIT : 0,
                    m_wMiscComponentAngle - step
                );
            } else if (m_wMiscComponentAngle < TOWTRUCK_HOIST_DOWN_LIMIT) { // down
                m_wMiscComponentAngle = std::min<uint16>(
                    TOWTRUCK_HOIST_DOWN_LIMIT,
                    m_wMiscComponentAngle - step
                );
            }
            break;
        }
    }

    // Attach a suitable plyrveh in range if we don't already have a trailer
    if (m_wMiscComponentAngle != TOWTRUCK_HOIST_DOWN_LIMIT || m_pVehicleBeingTowed)
        return;

    CVector towBarPos{};
    if (!GetTowBarPos(towBarPos, false, this))
        return;

    CEntity* entitiesInRange[16]{};
    int16 numEntitiesInRange{};
    CVector hitchPos{};
    CWorld::FindObjectsInRange(towBarPos, 10.f, true, &numEntitiesInRange, (int16)std::size(entitiesInRange), entitiesInRange, false, true, false, false, false);

    for (auto* vehicle : Get<CVehicle>(entitiesInRange, numEntitiesInRange)) {
        if (vehicle == this)
            continue;

        if (!vehicle->GetTowHitchPos(hitchPos, true, this))
            continue;

        if (vehicle->vehicleFlags.bIsLocked != 0u)
            continue;

        if (std::fabs(hitchPos.z - towBarPos.z) < 1.0f && DistanceBetweenPointsSquared2D(towBarPos, hitchPos) < sq(0.5f)) {
            vehicle->SetTowLink(this, false);
            m_wMiscComponentAngle -= 100; // "hide" hoist
            break;
        }
    }
}

// 0x6A44C0
CPed* CAutomobile::KnockPedOutCar(eWeaponType type, uint16 a2, CPed* ped) {
    return ped;
}

// 0x6A44D0
// UNUSED - Likely inlined?
void CAutomobile::PopBootUsingPhysics() {
    switch (m_damageManager.GetDoorStatus(eDoors::DOOR_BOOT)) {
    case DAMSTATE_DAMAGED:
        return;
    case DAMSTATE_OK:
        // Original code:
        // m_damageManager.SetDoorStatus(eDoors::DOOR_BOOT, DAMSTATE_OPENED);
        // [[fallthrough]]; Probably a mistake from their part. (TODO: Use break here, and uncomment code and see what happens)
    default:
        m_damageManager.SetDoorStatus(eDoors::DOOR_BOOT, DAMSTATE_OPENED_DAMAGED);
        break;
    }
    m_doors[eDoors::DOOR_BOOT].m_fAngVel = -2.f;
}

// 0x6A4520
void CAutomobile::CloseAllDoors() {
    const auto& mi = *GetVehicleModelInfo();

    const auto CloseDoor = [this](tComponent comp, eDoors door) {
        if (!IsDoorMissing(door)) {
            OpenDoor(nullptr, comp, door, 0.f, true);
        }
    };

    assert(mi.m_nNumDoors > 0);
    CloseDoor(tComponent::COMPONENT_DOOR_RR, eDoors::DOOR_LEFT_FRONT); // TODO: Again, enums dont seem to match up..
    if (mi.m_nNumDoors > 1) {
        CloseDoor(tComponent::COMPONENT_DOOR_RF, eDoors::DOOR_RIGHT_FRONT);
        if (mi.m_nNumDoors > 2) {
            assert(mi.m_nNumDoors == 4); // Pirulax: May assert, I'm not sure. Let me know if it did.

            CloseDoor(tComponent::COMPONENT_WING_LF, eDoors::DOOR_LEFT_REAR); // TODO: Enums dont match at all..
            CloseDoor(tComponent::COMPONENT_DOOR_LR, eDoors::DOOR_RIGHT_REAR);
        }
    }
}

void CAutomobile::DoSoftGroundResistance(uint32& extraHandlingFlags) {
    if (IsAnyWheelTouchingSand() && m_nModelIndex != MODEL_RCBANDIT && m_nModelIndex != MODEL_RHINO) {
        CVector speedUp = m_vecMoveSpeed - DotProduct(m_vecMoveSpeed, GetUp()) * GetUp();
        float offroadAbility = 0.005f;
        if (speedUp.SquaredMagnitude() <= sq(0.3f)) {
            autoFlags.bIsBoggedDownInSand = true;
        }
        else {
            float magnitude = speedUp.NormaliseAndMag();
            speedUp *= 0.3f;
            offroadAbility *= std::max(0.2f, 1.0f - (magnitude + magnitude));
        }

        if (handlingFlags.bOffroadAbility2)
            offroadAbility *= 0.3f;
        else if (handlingFlags.bOffroadAbility)
            offroadAbility *= 0.6f;

        if (CWeather::WetRoads > 0.2f)
            offroadAbility *= (1.2f - CWeather::WetRoads);

        float direction = -(offroadAbility * (CTimer::GetTimeStep() * m_fMass));
        ApplyMoveForce(speedUp * direction);
        extraHandlingFlags |= EXTRA_HANDLING_WHEELS_TOUCHING_SAND;
    } else if (IsAnyWheelTouchingRailTrack()) {
        if (m_nModelIndex != MODEL_RCBANDIT && m_nModelIndex != MODEL_RHINO) {
            CVector speedUp = m_vecMoveSpeed - DotProduct(m_vecMoveSpeed, GetUp()) * GetUp();
            float direction = -(CTimer::GetTimeStep() * m_fMass * CVehicle::ms_fRailTrackResistance);
            ApplyMoveForce(speedUp * direction);
        }
    }
}

// 0x6A4EC0
void CAutomobile::ProcessCarWheelPair(eCarWheel leftWheel, eCarWheel rightWheel, float steerAngle, CVector* contactSpeeds, CVector* contactPoints, float traction, float acceleration, float brake, bool bFront)
{
    bool driveWheels = false; // TODO: Refactor please
    if (bFront && gHandlingDataMgr.HasFrontWheelDrive(m_pHandlingData->m_nVehicleId))
        driveWheels = true;
    else if (!bFront && gHandlingDataMgr.HasRearWheelDrive(m_pHandlingData->m_nVehicleId))
        driveWheels = true;

    float suspensionBias = 2.0f * m_pHandlingData->m_fSuspensionBiasBetweenFrontAndRear; // TODO: Use lambda 
    if (!bFront) {
        suspensionBias = 2.0f - suspensionBias;

        if (!vehicleFlags.bIsHandbrakeOn || handlingFlags.bHbRearwheelSteer) {
            if (driveWheels && m_bDoingBurnout) {
                brake = 0.0f;
                traction = 0.0f;
                CVector point = std::min(1.0f, 3000.0f / m_fTurnMass) * -0.002f * m_fTurnMass * m_fSteerAngle * GetRight();
                ApplyTurnForce(contactPoints[leftWheel], point);
            } else if (!handlingFlags.bNosInst && gHandlingDataMgr.HasRearWheelDrive(m_pHandlingData->m_nVehicleId)) {
                traction *= m_fTireTemperature;
            }
        } else {
            brake = 20'000.0f;
        }
    }

    if (m_aWheelTimer[leftWheel] > 0.0f || m_aWheelTimer[rightWheel] > 0.0f) {
        // TOOD: Refactor
        float sinSteerAngle = 0.0f;
        float cosSteerAngle = 0.0f;
        bool canSteer = steerAngle > -100.0f;
        if (canSteer) {
            sinSteerAngle = std::sin(steerAngle);
            cosSteerAngle = std::cos(steerAngle);
        }

        const auto neutralHandling = m_nStatus != STATUS_PLAYER && m_nStatus != STATUS_REMOTE_CONTROLLED && handlingFlags.bNpcNeutralHandl;

        // TOOD: Refactor
        float brakeBias = 0.0f;
        float tractionBias = 0.0f;

        if (bFront) {
            brakeBias = neutralHandling ? 1.0f : 2.0f * m_pHandlingData->m_fBrakeBias;
            tractionBias = neutralHandling ? 1.0f : 2.0f * m_pHandlingData->m_fTractionBias;
        } else {
            brakeBias = neutralHandling ? 1.0f : 2.0f - (2.0f * m_pHandlingData->m_fBrakeBias);
            tractionBias = neutralHandling ? 1.0f : 2.0f - (2.0f * m_pHandlingData->m_fTractionBias);
        }

        // TOOD: Below seems like a lot of copy-paste code, maybe refactor?

        CVector wheelFwd{};
        CVector wheelRight{};
        if (m_aWheelTimer[leftWheel] > 0.0f) {
            float thrust = driveWheels ? acceleration : 0.0f;

            wheelFwd = GetForward();
            wheelFwd -= DotProduct(wheelFwd, m_wheelColPoint[leftWheel].m_vecNormal) * m_wheelColPoint[leftWheel].m_vecNormal;
            wheelFwd.Normalise();
            wheelRight = CrossProduct(wheelFwd, m_wheelColPoint[leftWheel].m_vecNormal);
            wheelRight.Normalise();
            if (canSteer && m_nModelIndex != MODEL_HYDRA)
            {
                CVector tmp = cosSteerAngle * wheelFwd - sinSteerAngle * wheelRight;
                wheelRight = sinSteerAngle * wheelFwd + cosSteerAngle * wheelRight;
                wheelFwd = tmp;
            }
            m_wheelColPoint[leftWheel].m_nSurfaceTypeA = SURFACE_WHEELBASE;
            float adhesion = g_surfaceInfos.GetAdhesiveLimit(&m_wheelColPoint[leftWheel]) * traction;
            if (m_nStatus == STATUS_PLAYER)
            {
                adhesion *= g_surfaceInfos.GetWetMultiplier(m_wheelColPoint[leftWheel].m_nSurfaceTypeB);
                adhesion *= std::min(suspensionBias * m_pHandlingData->m_fSuspensionForceLevel * 4.0f * (1.0f - m_fWheelsSuspensionCompression[leftWheel]), 2.0f);
                if (handlingFlags.bOffroadAbility2 && g_surfaceInfos.GetAdhesionGroup(m_wheelColPoint[leftWheel].m_nSurfaceTypeB) > ADHESION_GROUP_ROAD)
                {
                    adhesion *= 1.4f;
                }
                else if (handlingFlags.bOffroadAbility)
                {
                    if (g_surfaceInfos.GetAdhesionGroup(m_wheelColPoint[leftWheel].m_nSurfaceTypeB) > ADHESION_GROUP_ROAD)
                        adhesion *= 1.15f;
                }
            }
            tWheelState wheelState = m_aWheelState[leftWheel];
            if (m_damageManager.GetWheelStatus(leftWheel) == WHEEL_STATUS_BURST) {
                CVehicle::ProcessWheel(wheelFwd, wheelRight, contactSpeeds[leftWheel], contactPoints[leftWheel], m_nNumContactWheels,
                    thrust,
                    brake * brakeBias,
                    adhesion * m_damageManager.m_fWheelDamageEffect * tractionBias,
                    leftWheel, &m_fWheelBurnoutSpeed[leftWheel], &wheelState, WHEEL_STATUS_BURST);
            }
            else {
                CVehicle::ProcessWheel(wheelFwd, wheelRight, contactSpeeds[leftWheel], contactPoints[leftWheel], m_nNumContactWheels,
                    thrust,
                    brake * brakeBias,
                    adhesion * tractionBias,
                    leftWheel, &m_fWheelBurnoutSpeed[leftWheel], &wheelState, WHEEL_STATUS_OK);
            }
            if (driveWheels && m_fGasPedal < 0.0f && wheelState == WHEEL_STATE_SPINNING)
                m_aWheelState[leftWheel] = WHEEL_STATE_NORMAL;
            else
                m_aWheelState[leftWheel] = wheelState;
        }

        if (m_aWheelTimer[rightWheel] > 0.0f) {
            float thrust = driveWheels ? acceleration : 0.0f;
            wheelFwd = GetForward();
            wheelFwd -= DotProduct(wheelFwd, m_wheelColPoint[rightWheel].m_vecNormal) * m_wheelColPoint[rightWheel].m_vecNormal;
            wheelFwd.Normalise();
            wheelRight = CrossProduct(wheelFwd, m_wheelColPoint[rightWheel].m_vecNormal);
            wheelRight.Normalise();
            if (canSteer)
            {
                CVector tmp = cosSteerAngle * wheelFwd - sinSteerAngle * wheelRight;
                wheelRight = sinSteerAngle * wheelFwd + cosSteerAngle * wheelRight;
                wheelFwd = tmp;
            }
            m_wheelColPoint[rightWheel].m_nSurfaceTypeA = SURFACE_WHEELBASE;
            float adhesion = g_surfaceInfos.GetAdhesiveLimit(&m_wheelColPoint[rightWheel]) * traction;
            if (m_nStatus == STATUS_PLAYER)
            {
                adhesion *= g_surfaceInfos.GetWetMultiplier(m_wheelColPoint[rightWheel].m_nSurfaceTypeB);
                adhesion *= std::min(suspensionBias * m_pHandlingData->m_fSuspensionForceLevel * 4.0f * (1.0f - m_fWheelsSuspensionCompression[rightWheel]), 2.0f);
                if (handlingFlags.bOffroadAbility2 && g_surfaceInfos.GetAdhesionGroup(m_wheelColPoint[rightWheel].m_nSurfaceTypeB) > ADHESION_GROUP_ROAD)
                {
                    adhesion *= 1.4f;
                }
                else if (handlingFlags.bOffroadAbility)
                {
                    if (g_surfaceInfos.GetAdhesionGroup(m_wheelColPoint[rightWheel].m_nSurfaceTypeB) > ADHESION_GROUP_ROAD)
                        adhesion *= 1.15f;
                }
            }
            tWheelState wheelState = m_aWheelState[rightWheel];
            if (m_damageManager.GetWheelStatus(rightWheel) == WHEEL_STATUS_BURST) {
                CVehicle::ProcessWheel(wheelFwd, wheelRight, contactSpeeds[rightWheel], contactPoints[rightWheel], m_nNumContactWheels,
                    thrust,
                    brake * brakeBias,
                    adhesion * m_damageManager.m_fWheelDamageEffect * tractionBias,
                    rightWheel, &m_fWheelBurnoutSpeed[rightWheel], &wheelState, WHEEL_STATUS_BURST
               );
            }
            else {
                CVehicle::ProcessWheel(wheelFwd, wheelRight, contactSpeeds[rightWheel], contactPoints[rightWheel], m_nNumContactWheels,
                    thrust,
                    brake * brakeBias,
                    adhesion * tractionBias,
                    rightWheel, &m_fWheelBurnoutSpeed[rightWheel], &wheelState, WHEEL_STATUS_OK
               );
            }
            if (driveWheels && m_fGasPedal < 0.0f && wheelState == WHEEL_STATE_SPINNING)
                m_aWheelState[rightWheel] = WHEEL_STATE_NORMAL;
            else
                m_aWheelState[rightWheel] = wheelState;
        }
    }

    if (!bFront && !handlingFlags.bNosInst) {
        if (m_bDoingBurnout && driveWheels &&
            (m_aWheelState[CAR_WHEEL_REAR_LEFT] == WHEEL_STATE_SPINNING ||
            m_aWheelState[CAR_WHEEL_REAR_RIGHT] == WHEEL_STATE_SPINNING))
        {
            m_fTireTemperature += CTimer::GetTimeStep() / 1000.0f;
            m_fTireTemperature = std::min(m_fTireTemperature, 3.0f);
        }
        else if (m_fTireTemperature > 1.0f) {
            m_fTireTemperature = std::powf(0.995f, CTimer::GetTimeStep()) * (m_fTireTemperature - 1.0f) + 1.0f;
        }
    }

    if (IsRealHeli()) {
        return;
    }

    if (m_aWheelTimer[leftWheel] <= 0.0f) {
        if (driveWheels && acceleration != 0.0f) {
            if (acceleration > 0.0f) {
                if (m_wheelSpeed[leftWheel] < 1.0f) {
                    m_wheelSpeed[leftWheel] -= 0.1f;
                }
            } else {
                if (m_wheelSpeed[leftWheel] > -1.0f) {
                    m_wheelSpeed[leftWheel] += 0.05f;
                }
            }
        } else {
            m_wheelSpeed[leftWheel] *= 0.95f;
        }
        m_wheelRotation[leftWheel] += CTimer::GetTimeStep() * m_wheelSpeed[leftWheel];
    }

    if (m_aWheelTimer[rightWheel] <= 0.0f) {
        if (driveWheels && acceleration != 0.0f) {
            if (acceleration > 0.0f) {
                if (m_wheelSpeed[rightWheel] < 1.0f) {
                    m_wheelSpeed[rightWheel] -= 0.1f;
                }
            } else {
                if (m_wheelSpeed[rightWheel] > -1.0f) {
                    m_wheelSpeed[rightWheel] += 0.05f;
                }
            }
        } else {
            m_wheelSpeed[rightWheel] *= 0.95f;
        }
        m_wheelRotation[rightWheel] += CTimer::GetTimeStep() * m_wheelSpeed[rightWheel];
    }
}

// TODO: Could add these to CMatrix as well, might be useful.. Like: CMatrix::GetRoll()

/*!
* @addr 0x6A6010
* @brief Returns `roll` in DEGREES
*/
float CAutomobile::GetCarRoll() {
    const auto& right = m_matrix->GetRight();
    const auto rightMag2D = right.Magnitude2D();

    // If up.z < 0.f we're flipped, in which case `right` is more like `left` so we have to negate it.
    return RadiansToDegrees(std::atan2(right.z, m_matrix->GetUp().z < 0.f ? -rightMag2D : rightMag2D));
}

/*!
* @addr 0x6A6050
* @brief Returns `pitch` in RADIANS
*/
float CAutomobile::GetCarPitch() {
    const auto& fwd = m_matrix->GetForward();
    const auto  fwdMag2D = fwd.Magnitude2D();

    // `up.z` < 0 means we're flipped on the roof, which also means `forward` is more like `backward`, so we have to negate it.
    return std::atan2(fwd.z, m_matrix->GetUp().z < 0.f ? -fwdMag2D : fwdMag2D);
}

// 0x6A6140
bool CAutomobile::IsInAir() {
    if (physicalFlags.bDontApplySpeed) {
        return true;
    }

    if (!physicalFlags.bSubmergedInWater) {
        return AreAllWheelsNotTouchingGround() && m_vecMoveSpeed.IsZero();
    }

    return false;
}

// 0x6A6DC0
void CAutomobile::dmgDrawCarCollidingParticles(const CVector& position, float force, eWeaponType weapon) {
    if (!GetIsOnScreen()) {
        return;
    }

    switch (weapon) {
    case eWeaponType::WEAPON_UNARMED:
    case eWeaponType::WEAPON_FLOWERS:
        break;
    default: {
        auto fxDirection = m_vecMoveSpeed;
        const auto speedMag = fxDirection.NormaliseAndMag();

        g_fx.AddSparks(
            const_cast<CVector&>(position),
            fxDirection,
            speedMag * -10.f,
            ((int8)force / 10 + 4) % 64,
            m_vecMoveSpeed,
            eSparkType::SPARK_PARTICLE_SPARK2,
            0.3f,
            1.f
        );
        break;
    }
    }


    CVector fxPos{ lerp(GetPosition(), position, 0.7f) }; // Actually const, just that fx functions dont take consts :D

    // Add smoke
    {
        FxPrtMult_c prtMult{ 0.4f, 0.4f , 0.4f, 0.6f, 0.4f, 1.f, 1.f };
        CVector velocity{};

        // The higher our speedsq the more particles we create
        const auto numSmokes = std::max(1u, (uint32)((m_vecMoveSpeed * CTimer::GetTimeStep()).Magnitude() * 4.f));
        for (auto i = 0u; i < numSmokes; i++) {
            g_fx.m_SmokeHuge->AddParticle(&fxPos, &velocity, 0.f, &prtMult, -1.f, 1.2f, 0.6f, 0);
        }
    }

    // Add debris
    if (m_vecMoveSpeed.SquaredMagnitude() <= sq(0.25f)) {
        auto color = CVehicleModelInfo::ms_vehicleColourTable[m_nPrimaryColor] * m_fContactSurfaceBrightness;
        auto rwcolor = color.ToRwRGBA();
        g_fx.AddDebris(fxPos, rwcolor, 0.06f, (int32)force / 100 + 1);
    }
}

// 0x6A7090
void CAutomobile::ProcessCarOnFireAndExplode(bool bExplodeImmediately) {
    // TODO: Re-order the code to get rid of lambdas here (and of the really deep nesting)

    const auto DecreaseHealthAndProcess = [this, estatus = m_damageManager.GetEngineStatus()] {
        if (estatus > 225 && m_fHealth > 250.f) {
            m_fHealth -= 2.f;
        }
        ProcessDelayedExplosion();
    };

    const auto SetFxVelocity = [&, this] {
        if (m_pFireParticle) {
            auto bullshit = GetMoveSpeed() * 50.f;
            m_pFireParticle->SetVelAdd(&bullshit);
        }
        DecreaseHealthAndProcess();
    };

    const auto CreateFx = [&, this](size_t typ) {
        auto pos = GetDummyPosition(DUMMY_ENGINE);
        if (const auto mat = GetModellingMatrix()) {
            m_pFireParticle = g_fxMan.CreateFxSystem(
                typ == 1
                    ? "fire_car"
                    : "fire_large",
                pos,
                mat
            );
        }
        if (m_pFireParticle) {
            m_pFireParticle->Play();
            GetEventGlobalGroup()->Add(CEventVehicleOnFire{ this });
        }
    };

    if (m_fHealth < 250.f && GetStatus() != STATUS_WRECKED && !vehicleFlags.bIsDrowning) {
        const auto isSubPlaneOrHeli = notsa::contains({ VEHICLE_TYPE_PLANE, VEHICLE_TYPE_HELI }, m_nVehicleSubType);

        auto partFxToUse = !m_pFireParticle && !isSubPlaneOrHeli ? 1 : 0;

        const auto floorTsMS = std::floor(CTimer::GetTimeStepInMS());
        if (isSubPlaneOrHeli) { // 0x6A7117
            if (!m_fireParticleCounter) {
                m_fireParticleCounter = IsInAir() ? 2 : 1;
            }

            if (!bExplodeImmediately) { // 0x6A720B
                const auto isRcShit = notsa::contains({
                    MODEL_RCBARON,
                    MODEL_RCRAIDER,
                    MODEL_RCGOBLIN,
                    MODEL_RCBANDIT,
                    MODEL_RCTIGER
                }, GetModelId());

                m_fBurnTimer += m_fireParticleCounter || isRcShit
                    ? floorTsMS
                    : floorTsMS / 5.f;
                if (m_fBurnTimer > 5000.f) { // 0x6A72A4
                    BlowUpCar(m_pExplosionVictim, false);
                } else { //> 0x6A72D9 - Create smoke particle fx
                    auto fxPrtMult = [&, this]() -> FxPrtMult_c {
                        if (!isRcShit) {
                            return { 0.f, 0.f, 0.f, 0.4f, 1.f, 1.f, 0.3f };
                        }
                        if (GetStatus() == STATUS_REMOTE_CONTROLLED) {
                            return { 0.f, 0.f, 0.5f, 0.4f, 0.1f, 1.f, 0.1f };
                        }
                        return { 0.f, 0.f, 0.15f, 0.4f, 0.3f, 1.f, 0.3f };
                    }();
                    if (CTimer::GetFrameCounter() % 2 == 0) { // TODO: Don't use frame counter
                        auto vel = isRcShit
                            ? CVector::Random({ -0.5f, -0.5f, 0.f }, { 0.5f, 0.5f, 0.4f })
                            : CVector::Random({ -1.5f, -1.5f, 0.f }, { 1.5f, 1.5f, 1.0f });
                        auto pos = GetPosition() + (isRcShit
                            ? CVector::Random({ -0.7f, -0.7f, 0.f }, { 0.7f, 0.7f, 0.f })
                            : CVector::Random({ -2.0f, -2.0f, 0.f }, { 2.0f, 2.0f, 0.f })
                        );
                        g_fx.m_SmokeHuge->AddParticle(
                            &pos,
                            &vel,
                            0.f,
                            &fxPrtMult,
                            -1.f,
                            1.2f,
                            0.6f,
                            false
                        );
                    }
                }

                if (notsa::contains({ STATUS_PLAYER, STATUS_REMOTE_CONTROLLED }, GetStatus())) { // 0x6A745D
                    if (!m_pFireParticle && m_fBurnTimer > 2500.f) {
                        CreateFx(2);
                        SetFxVelocity();
                        return;
                    }
                } else if (!isRcShit && CGeneral::RandomBool(1.2f)) {
                    CExplosion::AddExplosion(this, m_pExplosionVictim, EXPLOSION_ROCKET, GetPosition(), 0, true, -1.f, false);
                }
            
                if (partFxToUse == 0) {
                    SetFxVelocity();
                    return;
                }
                CreateFx(partFxToUse);
                SetFxVelocity();
            }
        } else {
            m_fBurnTimer += floorTsMS;
            if (m_fBurnTimer > 5000.f) {
                BlowUpCar(m_pExplosionVictim, false);
            }
            if (partFxToUse == 0) {
                SetFxVelocity();
                return;
            }
        }
    }

    m_fBurnTimer = 0.f;
    if (m_pFireParticle) {
        m_pFireParticle->Kill();
        m_pFireParticle = nullptr;
    }

    DecreaseHealthAndProcess();
}

// todo: nodeIndex one of { eBikeNodes eBmxNodes eBoatNodes eHeliNodes eMonsterTruckNodes ePlaneNodes eQuadBikeNodes eTrailerNodes eTrainNodes eCarNodes }
// 0x6A8580
CObject* CAutomobile::SpawnFlyingComponent(eCarNodes nodeIndex, uint32 collisionType)
{
    if (CObject::nNoTempObjects >= 150) {
        return nullptr;
    }

    auto compFrame = m_aCarNodes[(size_t)nodeIndex];
    if (!compFrame) {
        return nullptr;
    }

    // Grab frame's atomic (so we can clone it later)
    RpAtomic* compAtomic{};
    RwFrameForAllObjects(compFrame, GetCurrentAtomicObjectCB, &compAtomic);
    if (!compAtomic) {
        return nullptr; // No atomic means there's nothing to render :D
    }

    GetObjectPool()->SetDealWithNoMemory(true);
    auto obj = new CObject;
    GetObjectPool()->SetDealWithNoMemory(false);

    if (!obj) {
        return nullptr;
    }

    const auto& frameLTM = RwFrameGetLTM(compFrame);

    if (nodeIndex == eCarNodes::CAR_WINDSCREEN) {
        obj->SetModelIndexNoCreate(MODEL_TEMPCOL_BONNET1);
    } else {
        switch (collisionType) {
        case 0u: {
            obj->SetModelIndexNoCreate(MODEL_TEMPCOL_BUMPER1);

            CMatrix frameMat{ frameLTM, false };
            obj->m_vecCentreOfMass = CVector{
                -DotProduct(frameMat.GetPosition() - GetPosition(), m_matrix->GetRight()), // Set it's X centre on mass depending on where it is on our local X axis
                0.f,
                0.f
            };
            break;
        }
        case 1u: {
            obj->SetModelIndexNoCreate(MODEL_TEMPCOL_WHEEL1);
            break;
        }
        case 2u: {
            obj->SetModelIndexNoCreate(MODEL_TEMPCOL_DOOR1);
            obj->m_vecCentreOfMass = CVector{ 0.f, -0.5f, 0.f };
            obj->m_bDrawLast = true;
            break;
        }
        case 3u: {
            obj->SetModelIndexNoCreate(MODEL_TEMPCOL_BONNET1);
            obj->m_vecCentreOfMass = CVector{ 0.f, 0.4f, 0.f };
            break;
        }
        case 4u: {
            obj->SetModelIndexNoCreate(MODEL_TEMPCOL_BOOT1);
            obj->m_vecCentreOfMass = CVector{ 0.f, -0.3f, 0.f };
            break;
        }
        default: {
            obj->SetModelIndexNoCreate(MODEL_TEMPCOL_PANEL1);
            break;
        }
        }
    }

    obj->RefModelInfo(m_nModelIndex);

    // Create a new frame using the flying object's atomic
    auto flyingObjFrame = RwFrameCreate();
    auto clonedFlyingObjAtomic = RpAtomicClone(compAtomic);  // Clone this component's atomic
    RpAtomicSetFrame(clonedFlyingObjAtomic, flyingObjFrame); // Associate the cloned atomic with a the new frame
    *RwFrameGetMatrix(flyingObjFrame) = *frameLTM; // Set this frame's matrix to be the same as the component's - TODO: This most likely isn't the correct way to do this..
    CVisibilityPlugins::SetAtomicRenderCallback(clonedFlyingObjAtomic, nullptr);
    obj->AttachToRwObject((RwObject*)clonedFlyingObjAtomic, true);
    obj->m_bDontStream = true;
    obj->m_fMass = 10.f;
    obj->m_fTurnMass = 25.f;
    obj->m_fAirResistance = 0.97f;
    obj->m_fElasticity = 0.1f;
    obj->m_fBuoyancyConstant = 0.106666f; // 8.0f / 75.0f ?
    obj->m_nObjectType = OBJECT_TEMPORARY;

    obj->SetIsStatic(false);

    CObject::nNoTempObjects++;
    if (CObject::nNoTempObjects <= 20u) {
        if (CObject::nNoTempObjects <= 10u) {
            obj->m_nRemovalTime = CTimer::GetTimeInMS() + 20'000; // todo 0x6A88A4 int/float add
        } else {
            obj->m_nRemovalTime = CTimer::GetTimeInMS() + 10'000;
        }
    } else {
        obj->m_nRemovalTime = CTimer::GetTimeInMS() + 4'000;
    }

    const auto GetMoveSpeedZ = [&, this] {
        if (m_vecMoveSpeed.z <= 0.f) {
            if (m_matrix->GetUp().z < 0.f /*flipped*/) {
                switch (collisionType) {
                case 3:
                case 4:
                case 18:
                    return 0.04f - m_vecMoveSpeed.z * 1.5f;
                }
            }
            return m_vecMoveSpeed.z / 4.f;
        } else {
            return m_vecMoveSpeed.z * 1.5f;
        }
    };
    obj->m_vecMoveSpeed = CVector{ m_vecMoveSpeed.x * 0.75f, m_vecMoveSpeed.y * 0.75f, GetMoveSpeedZ() };
    obj->m_vecTurnSpeed = m_vecTurnSpeed * 2.f;

    auto objDir = Normalized(obj->GetPosition() - GetPosition());
    switch (collisionType) {
    case 3:
    case 4:
    case 18: {
        objDir += m_matrix->GetUp();
        if (m_matrix->GetUp().z > 0.f/*not flipped*/) {
            obj->m_matrix->GetPosition() += m_matrix->GetUp() * m_vecMoveSpeed.Magnitude2D();
        }
        break;
    }
    }

    obj->ApplyMoveForce(objDir);

    if (collisionType == 1) {
        obj->m_fTurnMass = 5.f;
        obj->m_vecTurnSpeed.x = 0.5f;
        obj->m_fAirResistance = 0.99f;
    }

    if (m_nStatus == STATUS_WRECKED && IsVisible()) {
        const auto camDirUnnorm = TheCamera.GetPosition() - GetPosition();
        if (DotProduct(objDir, camDirUnnorm) > -0.5f) { // Object's direction is opposite to that of the camera's
            auto camDir = Normalized(camDirUnnorm);
            camDir.z += 0.3f;
            obj->ApplyMoveForce(camDir * 5.f); // Apply some move force toward the camera's direction
        }
    }

    if (CCollision::ProcessColModels(*obj->m_matrix, *obj->GetColModel(), *m_matrix, *GetColModel(), CWorld::m_aTempColPts, nullptr, nullptr, false) > 0)
        obj->m_pEntityIgnoredCollision = this;

    if (physicalFlags.bRenderScorched)
        obj->physicalFlags.bRenderScorched = true;

    CWorld::Add(obj);

    return obj;
}

// 0x6A8C00
void CAutomobile::ProcessBuoyancy()
{
    /* Unused code in sa function
    CTimeCycle::GetAmbientRed_Obj();
    CTimeCycle::GetAmbientGreen_Obj();
    CTimeCycle::GetAmbientBlue_Obj();
    CGeneral::GetRandomNumber();
    */

    CVector vecBuoyancyTurnPoint;
    CVector vecBuoyancyForce;
    if (!mod_Buoyancy.ProcessBuoyancy(this, m_fBuoyancyConstant, &vecBuoyancyTurnPoint, &vecBuoyancyForce)) {
        vehicleFlags.bIsDrowning = false;
        physicalFlags.bSubmergedInWater = false;
        physicalFlags.bTouchingWater = false;

        m_fBuoyancyConstant = m_pHandlingData->m_fBuoyancyConstant;
        for (int32 i = 0; i < 4; ++i) {
            auto& colPoint = m_wheelColPoint[i];
            if (m_fWheelsSuspensionCompression[i] < 1.0F && g_surfaceInfos.IsWater(colPoint.m_nSurfaceTypeB)) {
                auto vecWaterImpactVelocity = (colPoint.m_vecPoint + GetUp() * 0.3F) - GetPosition();
                CVector vecSpeed = GetSpeed(vecWaterImpactVelocity);
            }
        }
        return; // The loop above is pretty much an overcomplicated NOP.
    }

    if (IsSubPlane() && m_vecMoveSpeed.z < -1.0F) {
        BlowUpCar(this, false);
    }

    physicalFlags.bTouchingWater = true;
    auto fTimeStep = std::max(0.01F, CTimer::GetTimeStep());
    auto fUsedMass = m_fMass / 125.0F;
    auto fBuoyancyForceZ = vecBuoyancyForce.z / (fTimeStep * fUsedMass);

    if (fUsedMass > m_fBuoyancyConstant)
        fBuoyancyForceZ *= 1.05F * fUsedMass / m_fBuoyancyConstant;

    if (physicalFlags.bMakeMassTwiceAsBig)
        fBuoyancyForceZ *= 1.5F;

    auto fBuoyancyForceMult = std::max(0.5F, 1.0F - fBuoyancyForceZ / 20.0F);
    auto fSpeedMult = std::pow(fBuoyancyForceMult, CTimer::GetTimeStep()); // todo:: shadow var

    if (m_nModelIndex != MODEL_VORTEX
        || GetUp().z <= 0.3F
        || vehicleFlags.bIsDrowning) {

        m_vecMoveSpeed *= fSpeedMult;
        m_vecTurnSpeed *= fSpeedMult;
    }

    bool bHeliRotorKilled = false;
    if (m_pHandlingData->m_bIsHeli && m_fHeliRotorSpeed > 0.15F) {
        bool bForceKillRotor = false;
        if (m_nModelIndex != MODEL_SEASPAR && m_nModelIndex != MODEL_LEVIATHN) {
            auto fUsedForce = std::max(1.0F, fBuoyancyForceZ * 8.0F);
            auto vecMoveForceHeli = (vecBuoyancyForce * -2.0F) / fUsedForce;
            ApplyMoveForce(vecMoveForceHeli);
            m_vecTurnSpeed *= fSpeedMult;
            if (fBuoyancyForceZ <= 0.9F)
                return;

            bForceKillRotor = true;
        }

        if (fBuoyancyForceZ > 3.0F || bForceKillRotor) {
            m_fHeliRotorSpeed = 0.0F;
            bHeliRotorKilled = true;
        }
    }

    physicalFlags.bTouchingWater = true;
    ApplyMoveForce(vecBuoyancyForce);
    ApplyTurnForce(vecBuoyancyForce, vecBuoyancyTurnPoint);

    if (ModelIndices::IsAmphibiousHeli(m_nModelIndex)
        && fBuoyancyForceZ < 3.0F
        && (GetUp().z > -0.5F || fBuoyancyForceZ < 0.6F)) {

        vehicleFlags.bIsDrowning = false;
        physicalFlags.bSubmergedInWater = false;
        return;
    }

    // 0x6A8F67
    if ((CCheat::IsActive(CHEAT_CARS_ON_WATER) || m_nModelIndex == MODEL_VORTEX)
        && m_nStatus == eEntityStatus::STATUS_PLAYER
        && GetUp().z > 0.3F
    ) {
        if (!vehicleFlags.bIsDrowning) {
            vehicleFlags.bIsDrowning = false; // todo: FIX_BUGS?
            physicalFlags.bSubmergedInWater = false;
            return;
        }
    }

    if (bHeliRotorKilled || fBuoyancyForceZ >= 1.0F
        || (fBuoyancyForceZ > 0.6F && IsAnyWheelNotMakingContactWithGround())) {

        vehicleFlags.bIsDrowning = true;
        physicalFlags.bSubmergedInWater = true;

        m_vecMoveSpeed.z = std::max(-0.1F, m_vecMoveSpeed.z);

        if (m_fMass * 8.0F / 1250.0F < m_fBuoyancyConstant)
            m_fBuoyancyConstant -= (m_fMass * 0.01F / 1250.0F);

        if (m_fBuoyancyConstant < m_fMass / 125.0F)
            vehicleFlags.bEngineOn = false;

        auto driver = m_pDriver->AsPed();
        ProcessPedInVehicleBuoyancy(driver, true);

        for (const auto passenger : GetPassengers()) {
            ProcessPedInVehicleBuoyancy(passenger, false);
        }
    }
    else {
        vehicleFlags.bIsDrowning = false;
        physicalFlags.bSubmergedInWater = false;
    }
}

inline void CAutomobile::ProcessPedInVehicleBuoyancy(CPed* ped, bool bIsDriver) {
    if (!ped)
        return;

    ped->physicalFlags.bTouchingWater = true;
    if (!ped->IsPlayer() && autoFlags.bWaterTight)
        return;

    if (!IsSubQuad() || IsAnyWheelMakingContactWithGround()) {
        if (ped->IsPlayer())
            ped->AsPlayer()->HandlePlayerBreath(true, 1.0F);
        else {
            auto pedDamageResponseCalc = CPedDamageResponseCalculator(this, CTimer::GetTimeStep(), eWeaponType::WEAPON_DROWNING, PED_PIECE_TORSO, false);
            auto damageEvent = CEventDamage(this, CTimer::GetTimeInMS(), eWeaponType::WEAPON_DROWNING, PED_PIECE_TORSO, 0, false, true);
            if (damageEvent.AffectsPed(ped))
                pedDamageResponseCalc.ComputeDamageResponse(ped, damageEvent.m_damageResponse, true);
            else
                damageEvent.m_damageResponse.m_bDamageCalculated = true;

            ped->GetEventGroup().Add(&damageEvent, false);
        }
    } else {
        auto fDamageIntensity = m_vecMoveSpeed.Magnitude() * m_fMass; // TODO: It's "force"

        auto knockOffBikeEvent = CEventKnockOffBike(this, m_vecMoveSpeed, (-m_vecMoveSpeed).Normalized(), fDamageIntensity,
            0.0F, KNOCK_OFF_TYPE_FALL, 0, 0, nullptr, false, false);

        ped->GetEventGroup().Add(&knockOffBikeEvent, false);

        if (ped->IsPlayer())
            ped->AsPlayer()->HandlePlayerBreath(true, 1.0F);

        if (bIsDriver)
            vehicleFlags.bEngineOn = false;
    }
}

// 0x6A9680
void CAutomobile::ProcessHarvester()
{
    if (m_nStatus != STATUS_PLAYER)
        return;

    CStreaming::m_bStreamHarvesterModelsThisFrame = true;
    if (m_vecMoveSpeed.Magnitude2D() > 0.01f) {
        if ((CTimer::GetFrameCounter() & 1) != 0)
        {
            for (int32 i = GetPedPool()->GetSize() - 1; i >= 0; i--) {
                CPed* ped = GetPedPool()->GetAt(i);
                if (!ped)
                    continue;

                if (ped->IsPlayer())
                    continue;

                CVector distance = ped->GetPosition() - GetPosition();
                float distanceForward = DotProduct(distance, GetForward());
                if (distanceForward > 4.0f && distanceForward < 5.0f) {
                    float distanceRight = std::abs(DotProduct(distance, GetRight()));
                    if (distanceRight < 4.0f) {
                        float distanceUp = std::abs(DotProduct(distance, GetUp()));
                        if (distanceUp < 4.0f) {
                            ped->FlagToDestroyWhenNextProcessed();
                            m_harvesterParticleCounter = 20;
                            m_vehicleAudio.AddAudioEvent(AE_BODY_HARVEST, 0.0f);
                        }
                    }
                }
            }
        }
        else
        {
            for (int32 i = GetObjectPool()->GetSize() - 1; i >= 0; i--) {
                CObject* object = GetObjectPool()->GetAt(i);
                if (!object)
                    continue;

                if (m_nModelIndex != ModelIndices::MI_GRASSHOUSE && m_nModelIndex != ModelIndices::MI_GRASSPLANT)
                    continue;

                CVector distance1 = object->GetPosition() - GetPosition();
                float distanceForward1 = DotProduct(distance1, GetForward());
                if (distanceForward1 > 4.0f && distanceForward1 < 5.0f) {
                    float distanceRight1 = std::abs(DotProduct(distance1, GetRight()));
                    if (distanceRight1 < 4.0f) {
                        float distanceUp1 = std::abs(DotProduct(distance1, GetUp()));
                        if (distanceUp1 < 4.0f)
                            object->ObjectDamage(99999.0f, nullptr, nullptr, this, WEAPON_RUNOVERBYCAR);
                    }
                }
            }
        }
    }

    if (!m_harvesterParticleCounter)
        return;

    CVector pos = m_matrix->TransformPoint(CVector(-1.2f, -3.8f, 1.5f));
    CVector velocity = GetForward() * -0.1f;
    velocity.x += CGeneral::GetRandomNumberInRange(0.05f, -0.05f);
    velocity.y += CGeneral::GetRandomNumberInRange(0.05f, -0.05f);
    int32 bodyPartModelId = -1;
    switch (m_harvesterParticleCounter - 1) { // todo: remove -1 and refactor?
    case 0:
    case 6:
        bodyPartModelId = ModelIndices::MI_HARVESTERBODYPART1;
        break;
    case 1:
    case 3:
        bodyPartModelId = ModelIndices::MI_HARVESTERBODYPART2;
        break;
    case 2:
        bodyPartModelId = ModelIndices::MI_HARVESTERBODYPART3;
        break;
    case 5:
        bodyPartModelId = ModelIndices::MI_HARVESTERBODYPART4;
        break;
    }

    if (CLocalisation::ShootLimbs() && bodyPartModelId >= 0) {
        auto* limb = new CObject(bodyPartModelId, true);
        CPlaceable::SetMatrix(*m_matrix);
        limb->SetPosn(pos);
        limb->m_vecMoveSpeed = velocity;
        limb->m_vecTurnSpeed = CVector{
            CGeneral::GetRandomNumberInRange(0.12f, -0.04f),
            CGeneral::GetRandomNumberInRange(0.12f, -0.04f),
            CGeneral::GetRandomNumberInRange(0.12f, -0.04f)
        };
        limb->m_nObjectType = OBJECT_TEMPORARY;
        limb->UpdateRW();
        limb->UpdateRwFrame();
        limb->SetIsStatic(false);
        CObject::nNoTempObjects++;
        CWorld::Add(limb);
    }
    m_harvesterParticleCounter--;
    if (CLocalisation::Blood() && m_harvesterParticleCounter % 3 == 0) {
        FxPrtMult_c fxPrtMult;
        fxPrtMult.SetUp(0.15f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);
        g_fx.m_SmokeII3expand->AddParticle(&pos, &velocity, 0.0f, &fxPrtMult, -1.0f, 1.2f, 0.6f, 0);
    }
}

// 0x6A9D70
void CAutomobile::ProcessSwingingDoor(eCarNodes nodeIdx, eDoors doorIdx)
{
    constexpr auto BONNET_SWING_RADIUS = 0.05f;

    auto frame = m_aCarNodes[(size_t)nodeIdx];
    if (!frame) {
        return;
    }

    if (!m_damageManager.IsDoorPresent(doorIdx)) {
        return;
    }

    if (m_damageManager.IsDoorClosed(doorIdx)) {
        if (m_fDamageIntensity <= 100.f || doorIdx == DOOR_BONNET) {
            return;
        }

        switch (m_nStatus) {
        case eEntityStatus::STATUS_PLAYER:
        case eEntityStatus::STATUS_PHYSICS:
            break;
        default:
            return;
        }
    }

    CMatrix frameMatrix{ RwFrameGetMatrix(frame) };

    auto& door = m_doors[doorIdx];

    if (m_damageManager.IsDoorClosed(doorIdx) && CanDoorsBeDamaged()) {
        if (door.ProcessImpact(
            this,
            m_moveForce,
            m_turnForce,
            m_matrix->TransformVector(frameMatrix.GetPosition())
        )) {
            m_damageManager.SetDoorOpen(doorIdx);
        }
    }

    // Try opening the doorIdx (If it's not open already)
    if (!m_damageManager.IsDoorOpen(doorIdx)) {
        return;
    }

    // 0x6A9F02
    // If it's the bonnet, we possibly apply some angle velocity based on our current speedsq
    if (doorIdx == eDoors::DOOR_BONNET) {
        auto& bonnet = m_doors[eDoors::DOOR_BONNET];
        if ((bonnet.m_nDirn & 15) == 1) { // == 1 necessary
            bonnet.m_fAngVel += ((std::sin(bonnet.m_fAngle + 0.1f) * BONNET_SWING_RADIUS) * m_matrix->GetForward() * m_vecMoveSpeed).ComponentwiseSum();
        }
    }

    // 0x6A9F91
    // Now process it, if successful close it and do AudioFX
    if (door.Process(
        this,
        m_moveForce,
        m_turnForce,
        m_matrix->TransformVector(frameMatrix.GetPosition())
    )) {
        m_damageManager.SetDoorClosed(doorIdx);
        m_vehicleAudio.AddAudioEvent((eAudioEvents)((int32)AE_CAR_BONNET_CLOSE + (int32)doorIdx), 0.f);
    }

    // 0x6A9FCE
    door.UpdateFrameMatrix(frameMatrix);
   
    // Possibly detach bonnet and let it fly
    if (doorIdx == eDoors::DOOR_BONNET) {
        if (   door.m_nDoorState == DAMSTATE_OPENED                      // Still open (couldn't close it) ; todo: Comparison of different enumeration types ('eDoorState' and 'ePanelDamageState') is deprecated
            && DotProduct(m_vecMoveSpeed, m_matrix->GetForward()) > 0.4f // Speed's direction is kinda forwards
        ) {
            auto* flyingObj = SpawnFlyingComponent(CAR_BONNET, 2)->AsVehicle();

            m_vehicleAudio.AddAudioEvent(AE_BONNET_FLUBBER_FLUBBER, flyingObj);
            SetComponentVisibility(m_aCarNodes[CAR_BONNET], ATOMIC_IS_NOT_PRESENT);
            m_damageManager.SetDoorStatus(DOOR_BONNET, DAMSTATE_NOTPRESENT);

            // Apply some additional forces to the flying component
            if (flyingObj) {
                // Apply move speedsq (with some randomness in up/down direction)
                flyingObj->m_vecMoveSpeed = m_vecMoveSpeed * 0.4f + m_matrix->GetRight() * 0.1f;
                if (CGeneral::GetRandomNumber() % 2) {
                    flyingObj->m_vecMoveSpeed += m_matrix->GetUp() / 2.f;
                } else {
                    flyingObj->m_vecMoveSpeed -= m_matrix->GetUp() / 2.f;
                }
                flyingObj->ApplyTurnForce(m_matrix->GetUp() * 10.f, m_matrix->GetForward());
            }
        }
    }
}

/*!
* @addr 0x6AA200
* @brief Remove bonnet if it's present and open
* @returns Flying component object (May be null even in case the bonnet was removed - See `SpawnFlyingComponent` as to when this happens)
*/
CObject* CAutomobile::RemoveBonnetInPedCollision() {
    if (!m_aCarNodes[eCarNodes::CAR_BONNET]) {
        return nullptr; // Not present
    }

    if (!m_damageManager.IsDoorPresent(eDoors::DOOR_BONNET) || !m_damageManager.IsDoorOpen(eDoors::DOOR_BONNET)) {
        return nullptr; // Not open/missing
    }

    if (const auto& bonnet = m_doors[eDoors::DOOR_BONNET]; bonnet.m_fOpenAngle * 0.4f >= bonnet.m_fAngle) { // TODO: CDoor - Probably inlined (IsDoorHalf(ish)Open?)
        return nullptr; // Not open enough
    }

    auto* const flyingComp = SpawnFlyingComponent(eCarNodes::CAR_BONNET, 2u);
    m_vehicleAudio.AddAudioEvent(eAudioEvents::AE_BONNET_FLUBBER_FLUBBER, flyingComp);
    SetComponentVisibility(m_aCarNodes[eCarNodes::CAR_BONNET], ATOMIC_IS_NOT_PRESENT);
    m_damageManager.SetDoorStatus(eDoors::DOOR_BONNET, eDoorStatus::DAMSTATE_NOTPRESENT);
    return flyingComp;
}

// 0x6AA290
void CAutomobile::UpdateWheelMatrix(int32 nodeIndex, int32 flags) {
    ((void(__thiscall*)(CAutomobile*, int32, int32))0x6AA290)(this, nodeIndex, flags); // TODO: Reverse
}

// 0x6ADEF0
void CAutomobile::PopDoor(eCarNodes nodeIdx, eDoors doorIdx, bool showVisualEffect) {
    if (!m_damageManager.IsDoorPresent(doorIdx)) {
        return;
    }

    if (showVisualEffect) {
        if (nodeIdx != eCarNodes::CAR_NODE_NONE) {
            SpawnFlyingComponent(nodeIdx, nodeIdx == eCarNodes::CAR_CHASSIS ? 4u : 2u);
        } else {
            const auto obj = SpawnFlyingComponent(CAR_NODE_NONE, 3u)->AsVehicle();
            m_vehicleAudio.AddAudioEvent(eAudioEvents::AE_BONNET_FLUBBER_FLUBBER, obj);
        }
    }

    m_damageManager.SetDoorStatus(doorIdx, DAMSTATE_NOTPRESENT);
    SetComponentVisibility(m_aCarNodes[nodeIdx], ATOMIC_IS_NOT_PRESENT);
}

// 0x6ADF80
void CAutomobile::PopPanel(eCarNodes nodeIdx, ePanels panel, bool showVisualEffect) {
    if (m_damageManager.GetPanelStatus(panel) == ePanelDamageState::DAMSTATE_OPENED_DAMAGED) {
        return;
    }

    if (showVisualEffect) {
        switch (nodeIdx) {
        case eCarNodes::CAR_WHEEL_LF:
        case eCarNodes::CAR_WHEEL_LM:
            SpawnFlyingComponent(nodeIdx, 0u);
            break;
        default:
            SpawnFlyingComponent(nodeIdx, 5u);
            break;
        }
    }

    m_damageManager.SetPanelStatus(panel, ePanelDamageState::DAMSTATE_OPENED_DAMAGED);
    SetComponentVisibility(m_aCarNodes[nodeIdx], ATOMIC_IS_NOT_PRESENT);
}

/*!
* @addr 0x6ADFF0
*/
void CAutomobile::ScanForCrimes() {
    const auto plyrveh = FindPlayerVehicle();
    if (   plyrveh
        && plyrveh->IsAutomobile()
        && plyrveh->m_nStatus != eEntityStatus::STATUS_WRECKED
        && plyrveh->m_nAlarmState
        && plyrveh->m_nAlarmState != -1
        && DistanceBetweenPointsSquared(GetPosition(), plyrveh->GetPosition()) < sq(20.f)
    ) {
        FindPlayerPed()->SetWantedLevelNoDrop(1);
    }
}

// 0x6AE850
void CAutomobile::TankControl()
{
    if (m_nModelIndex == MODEL_RCTIGER && m_nStatus == STATUS_REMOTE_CONTROLLED) {
        if (CPad::GetPad()->CarGunJustDown()) {
            if (CTimer::GetTimeInMS() > m_nGunFiringTime + TIGER_GUNFIRE_RATE) {
                CWeapon minigun(WEAPON_MINIGUN, 5000);
                CVector point = m_matrix->TransformPoint(TIGER_GUN_POS + CTimer::GetTimeStep() * m_vecMoveSpeed);
                minigun.FireInstantHit(this, &point, &point, nullptr, nullptr, nullptr, false, true);
                CVector2D direction(0.0f, 0.1f);
                minigun.AddGunshell(this, point, direction, 0.025f);
                AudioEngine.ReportWeaponEvent(AE_WEAPON_FIRE, WEAPON_MINIGUN, this);
                m_nGunFiringTime = CTimer::GetTimeInMS();
            }
        }
        return;
    }

    if (m_nStatus != STATUS_PLAYER || m_nModelIndex != MODEL_RHINO)
        return;

    if (CGameLogic::GameState != GAME_STATE_INITIAL || !m_pDriver || !m_pDriver->IsPlayer())
        return;

    CPad* pad = m_pDriver->AsPlayer()->GetPadFromPlayer();
    if (!pad)
        return;

    CCam& activeCam = CCamera::GetActiveCamera();
    if (activeCam.m_nMode != MODE_CAM_ON_A_STRING) {
        m_fDoomVerticalRotation   -= ((float)pad->GetCarGunLeftRight() * CTimer::GetTimeStep() * 0.015f) / 128.0f;
        m_fDoomHorizontalRotation += ((float)pad->GetCarGunUpDown() * CTimer::GetTimeStep() * 0.005f) / 128.0f;
    } else {
        CVector frontDot = GetMatrix().InverseTransformVector(activeCam.m_vecFront);
        float doomVerticalRotation   = std::atan2(-frontDot.x, frontDot.y);
        float doomHorizontalRotation = std::atan2(frontDot.z, frontDot.Magnitude2D()) + DegreesToRadians(15);

        if (doomVerticalRotation < m_fDoomVerticalRotation - PI)
            doomVerticalRotation = doomVerticalRotation + TWO_PI;
        else if (doomVerticalRotation > m_fDoomVerticalRotation + PI)
            doomVerticalRotation = doomVerticalRotation - TWO_PI;

        float doomVerticalRotDiff = doomVerticalRotation - m_fDoomVerticalRotation;
        float timeStep = CTimer::GetTimeStep() * 0.015f;
        if (doomVerticalRotDiff < -timeStep)
            m_fDoomVerticalRotation -= timeStep;
        else if (doomVerticalRotDiff > timeStep)
            m_fDoomVerticalRotation += timeStep;

        float doomHorizontalRotDiff = doomHorizontalRotation - m_fDoomHorizontalRotation;
        timeStep = CTimer::GetTimeStep() * 0.005f;
        if (doomHorizontalRotDiff > timeStep)
            m_fDoomHorizontalRotation += timeStep;
        else if (doomHorizontalRotDiff >= -timeStep)
            m_fDoomHorizontalRotation = doomHorizontalRotation;
        else
            m_fDoomHorizontalRotation -= timeStep;
    }

    if (m_fDoomVerticalRotation > PI)
        m_fDoomVerticalRotation -= TWO_PI;
    else if (m_fDoomVerticalRotation < -PI)
        m_fDoomVerticalRotation += TWO_PI;

    constexpr float DOOM_HORIZONTAL_ROT_MAX = DegreesToRadians(35);
    constexpr float DOOM_HORIZONTAL_ROT_MIN = -DegreesToRadians(7);

    if (m_fDoomHorizontalRotation > DOOM_HORIZONTAL_ROT_MAX) {
        m_fDoomHorizontalRotation = DOOM_HORIZONTAL_ROT_MAX;
    }
    else if (m_fDoomVerticalRotation > DegreesToRadians(90) || m_fDoomVerticalRotation < -DegreesToRadians(90)) {
        float cosDoomVerticalRot = cosf(m_fDoomVerticalRotation) * 1.3f;
        float doomHorizontalRot = -DegreesToRadians(3);
        if (cosDoomVerticalRot >= -1.0f)
            doomHorizontalRot = DOOM_HORIZONTAL_ROT_MIN - cosDoomVerticalRot * (doomHorizontalRot - DOOM_HORIZONTAL_ROT_MIN);
        if (doomHorizontalRot > m_fDoomHorizontalRotation)
            m_fDoomHorizontalRotation = doomHorizontalRot;
    }
    else if (m_fDoomHorizontalRotation < DOOM_HORIZONTAL_ROT_MIN) {
        m_fDoomHorizontalRotation = DOOM_HORIZONTAL_ROT_MIN;
    }

    if (pad->CarGunJustDown()) { // TODO: Denest
        CPlayerInfo& playerInfo = FindPlayerInfo();
        if (CTimer::GetTimeInMS() > playerInfo.m_nLastTimeBigGunFired + 800) {
            playerInfo.m_nLastTimeBigGunFired = CTimer::GetTimeInMS();
            CVector point;
            point.x = std::sin(-m_fDoomVerticalRotation);
            point.y = std::cos(m_fDoomVerticalRotation);
            point.z = std::sin(m_fDoomHorizontalRotation);
            point = GetMatrix().TransformVector(point);

            CVector newTurretPosition;
            if (m_aCarNodes[CAR_MISC_C]) { // 0x6AED5D
                RwMatrix* carNodeMiscMatrix = RwFrameGetLTM(m_aCarNodes[CAR_MISC_C]);
                newTurretPosition = *RwMatrixGetPos(carNodeMiscMatrix);
                newTurretPosition += GetSpeed(newTurretPosition - GetPosition()) * CTimer::GetTimeStep();
            }
            else {
                float sinVerticalDoomRot = std::sin(m_fDoomVerticalRotation);
                float cosVerticalDoomRot = std::cos(m_fDoomVerticalRotation);

                CVector doomOffset;

                doomOffset.x = TANK_SHOT_DOOM_POS.x;
                doomOffset.x += cosVerticalDoomRot * TANK_SHOT_DOOM_DISTANCE_TO_DEFAULT_TARGET.x;
                doomOffset.x -= sinVerticalDoomRot * TANK_SHOT_DOOM_DISTANCE_TO_DEFAULT_TARGET.y;

                doomOffset.y = TANK_SHOT_DOOM_POS.y;
                doomOffset.y += cosVerticalDoomRot * TANK_SHOT_DOOM_DISTANCE_TO_DEFAULT_TARGET.y;
                doomOffset.y += sinVerticalDoomRot * TANK_SHOT_DOOM_DISTANCE_TO_DEFAULT_TARGET.x;

                doomOffset.z = TANK_SHOT_DOOM_POS.z;
                doomOffset.z += TANK_SHOT_DOOM_DISTANCE_TO_DEFAULT_TARGET.z - 1.0f;

                // BUG?: This statement is not assigned to any variable.
                // It should be `newTurretPosition =  *m_matrix * doomOffset`
                // Izzotop: Fixed, equal to OG code; untested -> remove ^ after tests
                newTurretPosition = m_matrix->TransformPoint(doomOffset);
            }

            CVector distance = newTurretPosition - GetPosition();
            CVector force = point * -0.1f * m_fMass;
            ApplyForce(force, distance, true);
            CVector endPoint = newTurretPosition + point * 60.0f;
            CWeapon::DoTankDoomAiming(this, m_pDriver, &newTurretPosition, &endPoint);

            CColPoint colPoint{};
            CEntity* entity = nullptr;
            CWorld::pIgnoreEntity = this;
            CWorld::ProcessLineOfSight(
                newTurretPosition,
                endPoint,
                colPoint,
                entity,
                true,
                true,
                true,
                true,
                true,
                true,
                false,
                false
            );
            CWorld::pIgnoreEntity = nullptr;
            CVector explosionPos = endPoint;
            if (entity)
                explosionPos = colPoint.m_vecPoint - (colPoint.m_vecPoint - newTurretPosition) * 0.04f;
            CExplosion::AddExplosion(nullptr, FindPlayerPed(), EXPLOSION_TANK_FIRE, explosionPos, 0, true, -1.0f, false);
            CVector target = explosionPos - newTurretPosition;
            g_fx.TriggerTankFire(newTurretPosition, target);
        }
    }
}

// 0x6AF110
void CAutomobile::BlowUpCarsInPath() {
    if (m_vecMoveSpeed.SquaredMagnitude() < sq(0.1f)) { // Originally `Magnitude() < 0.1f`
        return;
    }

    if (autoFlags.bTankExplodesCars) {
        return;
    }

    for (auto&& entity : GetCollidingEntities()) {
        if (!entity)
            continue; // I don't think should ever happen? But original code checks...
        if (!entity->IsVehicle())
            continue;

        auto& veh = *entity->AsVehicle();
        if (ModelIndices::IsRhino(veh.m_nModelIndex))
            continue;
        if (veh.physicalFlags.bRenderScorched)
            continue;

        if (this == FindPlayerVehicle()) {
            CCrime::ReportCrime(eCrimeType::CRIME_EXPLOSION, &veh, FindPlayerPed());
        }
        veh.BlowUpCar(this, false);
    }
}

// 0x6AF420
void CAutomobile::PlaceOnRoadProperly()
{
    auto* cm = GetColModel();
    auto fStartY =  cm->m_pColData->m_pLines[0].m_vecStart.y;
    auto fEndY   = -cm->m_pColData->m_pLines[3].m_vecStart.y;

    const auto& vecPos = GetPosition();

    auto vecRearCheck = vecPos - GetForward() * fEndY;
    vecRearCheck.z = vecPos.z;

    auto vecFrontCheck = vecPos + GetForward() * fStartY;
    vecFrontCheck.z = vecPos.z;

    bool bColFoundFront = false;
    CColPoint colPoint{};
    CEntity* colEntity;
    float fColZ;
    if (CWorld::ProcessVerticalLine(vecFrontCheck, vecFrontCheck.z + 5.0F, colPoint, colEntity, true)) {
        m_bTunnel = colEntity->m_bTunnel;
        m_bTunnelTransition = colEntity->m_bTunnelTransition;

        fColZ = colPoint.m_vecPoint.z;
        m_pEntityWeAreOn = colEntity;
        bColFoundFront = true;
    }
    if (CWorld::ProcessVerticalLine(vecFrontCheck, vecFrontCheck.z - 5.0F, colPoint, colEntity, true)) {
        if (!bColFoundFront || std::fabs(vecFrontCheck.z - colPoint.m_vecPoint.z) < std::fabs(vecFrontCheck.z - fColZ)) {
            m_bTunnel = colEntity->m_bTunnel;
            m_bTunnelTransition = colEntity->m_bTunnelTransition;

            fColZ = colPoint.m_vecPoint.z;
            m_pEntityWeAreOn = colEntity;

            m_FrontCollPoly.ligthing = colPoint.m_nLightingB;
            vecFrontCheck.z = fColZ;
        }
    }
    else if (bColFoundFront) {
        m_FrontCollPoly.ligthing = colPoint.m_nLightingB;
        vecFrontCheck.z = fColZ;
    }

    bool bColFoundRear = false;
    colEntity = nullptr;
    if (CWorld::ProcessVerticalLine(vecRearCheck, vecRearCheck.z + 5.0F, colPoint, colEntity, true)) {
        m_bTunnel = colEntity->m_bTunnel;
        m_bTunnelTransition = colEntity->m_bTunnelTransition;

        fColZ = colPoint.m_vecPoint.z;
        m_pEntityWeAreOn = colEntity;
        bColFoundRear = true;
    }
    if (CWorld::ProcessVerticalLine(vecRearCheck, vecRearCheck.z - 5.0F, colPoint, colEntity, true)) {
        if (!bColFoundRear || std::fabs(vecRearCheck.z - colPoint.m_vecPoint.z) < std::fabs(vecRearCheck.z - fColZ)) {
            m_bTunnel = colEntity->m_bTunnel;
            m_bTunnelTransition = colEntity->m_bTunnelTransition;

            fColZ = colPoint.m_vecPoint.z;
            m_pEntityWeAreOn = colEntity;

            m_RearCollPoly.ligthing = colPoint.m_nLightingB;
            vecRearCheck.z = fColZ;
        }
    }
    else if (bColFoundRear) {
        m_RearCollPoly.ligthing = colPoint.m_nLightingB;
        vecRearCheck.z = fColZ;
    }

    //FIXME: Not originally in this function, we can't spawn skimmer from debug menu cause those 2 values aren't initialized,
    //       resulting in garbage results further down, either we have a bug somewhere, or it's like that in original SA too
    if (m_nModelIndex == MODEL_SKIMMER) {
        m_fFrontHeightAboveRoad = 0.0F;
        m_fRearHeightAboveRoad = 0.0F;
        vecFrontCheck.z += 4.0F;
        vecRearCheck.z += 4.0F;
    }
    else {
        auto fHeightAboveRoad = GetHeightAboveRoad();
        vecFrontCheck.z += fHeightAboveRoad;
        vecRearCheck.z += m_fRearHeightAboveRoad;
    }

    auto fLength = fEndY + fStartY;
    GetRight().Set((vecFrontCheck.y - vecRearCheck.y) / fLength, -((vecFrontCheck.x - vecRearCheck.x) / fLength), 0.0F);

    auto vecDiff = vecFrontCheck - vecRearCheck;
    vecDiff.Normalise();
    GetForward() = vecDiff;

    auto vecCross = CrossProduct(GetRight(), GetForward());
    GetUp() = vecCross;

    CVector vecNewPos = (vecFrontCheck * fEndY + vecRearCheck * fStartY) / fLength;
    SetPosn(vecNewPos);

    if (IsSubPlane())
        AsPlane()->m_planeCreationHeading = CGeneral::GetATanOfXY(GetForward().x, GetForward().y);
}

// 0x6AF910
void CAutomobile::PopBoot() {
    if (m_damageManager.IsDoorClosed(eDoors::DOOR_BOOT)) {
        auto& door = m_doors[eDoors::DOOR_BOOT];

        door.OpenFully();

        CMatrix frameMat{ RwFrameGetMatrix(m_aCarNodes[eCarNodes::CAR_BOOT]) };
        CVector rot{ 0.f, 0.f, 0.f };
        rot[door.m_nAxis] = door.m_fAngle;
        frameMat.SetRotateKeepPos(rot);
        frameMat.UpdateRW();
    }
}

// 0x6AFA20
void CAutomobile::CloseBoot() {
    auto& door = m_doors[eDoors::DOOR_BOOT];

    door.CloseFully();

    // Code copy pasted from `PopBoot`:
    CMatrix frameMat{ RwFrameGetMatrix(m_aCarNodes[eCarNodes::CAR_BOOT]) };
    CVector rot{ 0.f, 0.f, 0.f };
    rot[door.m_nAxis] = door.m_fAngle;
    frameMat.SetRotateKeepPos(rot);
    frameMat.UpdateRW();
}

// 0x6B0690
void CAutomobile::DoHeliDustEffect(float timeConstMult, float fxMaxZMult) {
    const auto KillDustFx = [this] {
        if (m_pDustParticle) {
            m_pDustParticle->Kill();
            m_pDustParticle = nullptr;
            m_heliDustFxTimeConst = 0.f;
        }
    };

    const auto& myPos = GetPosition();

    // 0x6B07E9
    // Moved early out here instead
    if (DistanceBetweenPointsSquared(TheCamera.GetPosition(), myPos) >= sq(50.f)) {
        KillDustFx();
        return;
    }

    switch (m_nVehicleSubType) {
    case eVehicleType::VEHICLE_TYPE_HELI: {
        if (m_fHeliRotorSpeed < 0.1125f) {
            KillDustFx();
            return;
        }
        break;
    }
    case eVehicleType::VEHICLE_TYPE_PLANE: {
        break;
    }
    default: {
        KillDustFx();
        return;
    }
    }

    // 0x6B0705

    // Figure out ground position, and if it's underwater

    CColPoint groundCP{};
    CEntity* hitEntity{}; // Unused
    CWorld::ProcessVerticalLine(myPos, -1000.f, groundCP, hitEntity, true); // TODO: Check if it returned true :D
    const bool isGroundSand = g_surfaceInfos.IsSand(groundCP.m_nSurfaceTypeB);

    auto waterLevel{-1000.f};
    const bool isThereWaterUnderUs = CWaterLevel::GetWaterLevel(myPos, waterLevel, false);
    const auto isHitPosUnderWater = isThereWaterUnderUs && waterLevel > groundCP.m_vecPoint.z;

    const auto groundZ = isHitPosUnderWater ? waterLevel : groundCP.m_vecPoint.z;
    const auto distToGroundZ = myPos.z - groundZ;

    // Calculate max height for fx to be played
    const auto GetBaseMaxZForFx = [this]{
        switch (m_nModelIndex) {
        case eModelID::MODEL_RCGOBLIN:
        case eModelID::MODEL_RCRAIDER:
            return 3.f;
        }
        return 30.f;
    };
    const auto maxZForFx = GetBaseMaxZForFx() * fxMaxZMult;

    // Check if we're flying low enough for the fx to be played. If not, kill it, and return.
    if (distToGroundZ >= maxZForFx) {
        KillDustFx();
        return;
    }

    // Early out moved to top

    // Add danger event if we're flying too low to the ground
    if (CTimer::GetFrameCounter() % 20 == 0) {
        const auto dangerRadius = m_nModelIndex == MODEL_HYDRA ? 30.f : 20.f;
        if (distToGroundZ < dangerRadius) {
            CEventDanger danger{this, dangerRadius};
            GetEventGlobalGroup()->Add(&danger);
        }
    }

    // 0x6B08B1
    // Create and play fx if it doesn't exist.
    if (!m_pDustParticle) {
        CVector fxPos{0.f, 0.f, 0.f};
        m_pDustParticle = g_fxMan.CreateFxSystem("heli_dust", CVector{0.f, 0.f, 0.f}, nullptr, true);
        if (!m_pDustParticle) { // Failed, return
            return;
        }
        m_pDustParticle->SetLocalParticles(true);
        m_pDustParticle->Play();
    }

    // Update time constant of fx
    {
        const auto heightProgress = (maxZForFx - distToGroundZ) / maxZForFx * timeConstMult;
        const auto heightProgressSq = heightProgress * heightProgress;

        // TODO: Figure out a better way to do this... Very ugly.
        // Basically it's stepping `m_heliDustFxTimeConst` by 0.04 towards `heightProgressSq`.
        if (m_heliDustFxTimeConst < heightProgressSq) {
            m_heliDustFxTimeConst = std::min(m_heliDustFxTimeConst + 0.04f, heightProgressSq);
        } else if (m_heliDustFxTimeConst > heightProgressSq) { // Must check if less, because they might be equal (tbh, the chance for that to happen are... small)
            m_heliDustFxTimeConst = std::max(m_heliDustFxTimeConst - 0.04f, heightProgressSq);
        }

        m_pDustParticle->SetConstTime(true, m_heliDustFxTimeConst);
    }

    // Update max of fx
    m_pDustParticle->SetOffsetPos({ myPos.x, myPos.y, groundZ });

    // Finally enable/disable prims (whatever that is)
    m_pDustParticle->EnablePrim(0, !isHitPosUnderWater && !isGroundSand); // Dust or similar particles?
    m_pDustParticle->EnablePrim(1, !isHitPosUnderWater && isGroundSand);  // Sand particles?
    m_pDustParticle->EnablePrim(2, isHitPosUnderWater);                   // Water particles?

    // Above code originally (kinda):
    //if (isHitPosUnderWater) {
    //    m_pDustParticle->EnablePrim(0, false);
    //    m_pDustParticle->EnablePrim(1, false);
    //    m_pDustParticle->EnablePrim(2, true);
    //} else {
    //    m_pDustParticle->EnablePrim(0, !isGroundSand);
    //    m_pDustParticle->EnablePrim(1, isGroundSand);
    //    m_pDustParticle->EnablePrim(2, false);
    //}
}

// 0x6B1350
void CAutomobile::SetBumperDamage(ePanels panelIdx, bool withoutVisualEffect) {
    auto nodeIdx = CDamageManager::GetCarNodeIndexFromPanel(panelIdx);
    auto frame = m_aCarNodes[nodeIdx];
    if (!frame) {
        return;
    }

    if (!GetVehicleModelInfo()->m_pVehicleStruct->IsComponentDamageable(nodeIdx)) {
        return;
    }

    switch (m_damageManager.GetPanelStatus(panelIdx)) {
    case ePanelDamageState::DAMSTATE_DAMAGED: {
        if (!m_pHandlingData->m_bBouncePanels) { // TODO: Weird... The flag name might be incorrect, because here we actually set the bouncing panelIdx.
            if (auto* panelFrame = CheckIfExistsGetFree(nodeIdx)) {
                panelFrame->SetPanel(nodeIdx, 0, CGeneral::GetRandomNumberInRange(-0.5f, -0.2f));
            }
        }
        break;
    }
    case ePanelDamageState::DAMSTATE_OPENED: {
        SetComponentVisibility(frame, ATOMIC_IS_DAM_STATE);
        break;
    }
    case ePanelDamageState::DAMSTATE_OPENED_DAMAGED: {
        if (!withoutVisualEffect) {
            SpawnFlyingComponent(nodeIdx, 0u);
        }
        SetComponentVisibility(frame, ATOMIC_IS_NOT_PRESENT);
        break;
    }
    }
}

// 0x6B1480
void CAutomobile::SetPanelDamage(ePanels panel, bool createWindowGlass)
{
    auto nodeIdx = CDamageManager::GetCarNodeIndexFromPanel(panel);
    auto frame = m_aCarNodes[nodeIdx];
    if (!frame) {
        return;
    }

    if (!GetVehicleModelInfo()->m_pVehicleStruct->IsComponentDamageable(nodeIdx)) {
        return;
    }

    switch (m_damageManager.GetPanelStatus(panel)) {
    case ePanelDamageState::DAMSTATE_DAMAGED: {
        if (m_pHandlingData->m_bBouncePanels) { // TODO: Weird... The flag name might be incorrect, because here we actually set the bouncing panelIdx.
            return;
        }
        if (auto* panel_ = CheckIfExistsGetFree(nodeIdx)) {
            switch (nodeIdx) {
            case eCarNodes::CAR_WINDSCREEN:
            case eCarNodes::CAR_WING_LF:
            case eCarNodes::CAR_WING_RF:
                break;
            default: {
                panel_->SetPanel(nodeIdx, 1, CGeneral::GetRandomNumberInRange(-0.2f, -0.5f));
                break;
            }
            }
        }
        SetComponentVisibility(frame, ATOMIC_IS_DAM_STATE);
        break;
    }
    case ePanelDamageState::DAMSTATE_OPENED: {
        if (panel == ePanels::WINDSCREEN_PANEL) {
            m_vehicleAudio.AddAudioEvent(eAudioEvents::AE_WINDSCREEN_SHATTER, 0.f);
        }
        SetComponentVisibility(frame, ATOMIC_IS_DAM_STATE);
        break;
    }
    case ePanelDamageState::DAMSTATE_OPENED_DAMAGED: {
        if (createWindowGlass) {
            if (panel == ePanels::WINDSCREEN_PANEL) {
                CGlass::CarWindscreenShatters(this);
            }
        } else {
            SpawnFlyingComponent(nodeIdx, 5u);
        }
        SetComponentVisibility(frame, ATOMIC_IS_NOT_PRESENT);
        break;
    }
    }
}

// 0x6B1600
void CAutomobile::SetDoorDamage(eDoors doorIdx, bool withoutVisualEffect)
{
    auto nodeIdx = CDamageManager::GetCarNodeIndexFromDoor(doorIdx);
    auto frame = m_aCarNodes[nodeIdx];
    if (!frame) {
        return;
    }

    const auto isDamageable = GetVehicleModelInfo()->m_pVehicleStruct->IsComponentDamageable(nodeIdx);

    auto& door{ m_doors[doorIdx] };

    // Leaving this here for further reference...
    // The `if` @ `0x6B1650` is inverted here a little, because
    // `0x6B169C` is only reachable if `eDoors::DOOR_BOOT` in which all other if's are ignored (that is the one at `0x6B1650` and `0x6B1673`)
    // If the doorIdx isn't BOOT, but is BONNET just ignore it, because of `0x6B1660`
    // Now, if it's neither, then we go on and check the logical invert of the 2 other conditions at `0x6B1650`
    // If those are all true we will rtn up @ `0x6B1673`

    switch (doorIdx) {
    case eDoors::DOOR_BOOT: {
        // 0x6B169C
        if (m_pHandlingData->m_bAltSteerOpt) {
            switch (m_damageManager.GetDoorStatus(eDoors::DOOR_BOOT)) {
            case eDoorStatus::DAMSTATE_OPENED: {
                m_damageManager.SetDoorStatus(eDoors::DOOR_BOOT, isDamageable ? eDoorStatus::DAMSTATE_DAMAGED : eDoorStatus::DAMSTATE_NOTPRESENT);
                break;
            }
            case eDoorStatus::DAMSTATE_OPENED_DAMAGED: {
                m_damageManager.SetDoorStatus(eDoors::DOOR_BOOT, eDoorStatus::DAMSTATE_NOTPRESENT);
                break;
            }
            }
        }
        break;
    }
    case eDoors::DOOR_BONNET: // Check from 0x6B1660
        break;

    default: {
        // Inverted `if` from `0x6B1650`, this way we get to the `else` branch at `0x6B1667`
        if (!CanDoorsBeDamaged()) {
            switch (m_damageManager.GetDoorStatus(doorIdx)) { // 0x6B1659
            case eDoorStatus::DAMSTATE_OPENED_DAMAGED:
            case eDoorStatus::DAMSTATE_NOTPRESENT: {
                // 0x6B1667
                door.Open(0.f);
                m_damageManager.SetDoorStatus(doorIdx, eDoorStatus::DAMSTATE_DAMAGED);
                return;
            }
            }
        }
        break;
    }
    }

    // 0x6B16E5
    switch (m_damageManager.GetDoorStatus(doorIdx)) {
    case DAMSTATE_DAMAGED: { // 0x6B16E5
        if (isDamageable) {
            SetComponentVisibility(frame, ATOMIC_IS_DAM_STATE);
        }

        if (door.m_fPrevAngle != 0.f) {
            door.m_fAngle = 0.f;
            door.m_fPrevAngle = 0.f;
            door.m_fAngVel = 0.f;

            // Reset component rotation
            {
                CMatrix frameMatrix{RwFrameGetMatrix(frame)};
                frameMatrix.SetRotateKeepPos({});
                frameMatrix.UpdateRW();
            }

            m_vehicleAudio.AddAudioEvent((eAudioEvents)((int32)AE_CAR_BONNET_CLOSE + (int32)doorIdx), 0.f);
        }
        break;
    }
    case DAMSTATE_NOTPRESENT: { // 0x6B17F0
        if (!withoutVisualEffect) {
            if (doorIdx == eDoors::DOOR_BONNET) { // Inverted
                const auto obj = SpawnFlyingComponent(nodeIdx, 3u)->AsVehicle();
                m_vehicleAudio.AddAudioEvent(AE_BONNET_FLUBBER_FLUBBER, obj);
            } else {
                SpawnFlyingComponent(nodeIdx, doorIdx == eDoors::DOOR_BOOT ? 4u : 2u);
            }
        }
        SetComponentVisibility(frame, ATOMIC_IS_NOT_PRESENT);
        break;
    }
    case DAMSTATE_OPENED: // 0x6B16E5
    case DAMSTATE_OPENED_DAMAGED: {
        RwFrameForAllObjects(frame, CVehicleModelInfo::SetAtomicFlagCB, (void*)ATOMIC_RENDER_ALWAYS);
        if (doorIdx == eDoors::DOOR_BONNET) {
            door.m_fAngVel = 0.2f;
        }
        break;
    }
    }
}

// 0x6B3F70
bool CAutomobile::RcbanditCheck1CarWheels(CPtrList& ptrList)
{
    CColModel* colModel = GetVehicleModelInfo()->GetColModel();

    CPtrNode* next = nullptr;
    for (CPtrNode* node = ptrList.m_node; node; node = next) {
        next = node->m_next;
        auto* vehicle = (CAutomobile*)node->m_item;
        if (node->m_item == this || !vehicle->IsAutomobile())
            continue;

        if (!ModelIndices::IsRCBandit(vehicle->m_nModelIndex) && vehicle->m_nScanCode != GetCurrentScanCode())
            continue;

        CVector distance = GetPosition() - vehicle->GetPosition();
        if (distance.x < 10.0f && distance.y < 10.0f) {
            auto modelInfo = vehicle->GetVehicleModelInfo();
            for (int32 i = 0; i < 4; i++) {
                if (m_fWheelsSuspensionCompressionPrev[i] >= 1.0f && m_nStatus != STATUS_SIMPLE)
                    continue;

                static CMatrix wheelMatrix;
                CVector wheelPos;
                modelInfo->GetWheelPosn(i, wheelPos, false);
                wheelMatrix = Invert(*m_matrix, wheelMatrix);

                CColSphere sphere;
                sphere.m_vecCenter = wheelMatrix.TransformPoint(vehicle->m_matrix->TransformPoint(wheelPos));
                sphere.m_fRadius = modelInfo->m_fWheelSizeFront * 0.25f;
                if (CCollision::TestSphereBox(sphere, colModel->m_boundBox))
                    return true;
            }
        }
    }
    return false;
}

// 0x6B45E0
bool CAutomobile::RcbanditCheckHitWheels() {
    const auto& point = GetPosition();
    float fRadius = 2.0f;
    const float minX = point.x - fRadius;
    const float maxX = point.x + fRadius;
    const float minY = point.y - fRadius;
    const float maxY = point.y + fRadius;
    int32 startSectorX = std::max(CWorld::GetSectorX(minX), 0);
    int32 startSectorY = std::max(CWorld::GetSectorY(minY), 0);
    int32 endSectorX = std::min(CWorld::GetSectorX(maxX), MAX_SECTORS_X - 1);
    int32 endSectorY = std::min(CWorld::GetSectorY(maxY), MAX_SECTORS_Y - 1);
    CWorld::IncrementCurrentScanCode();
    for (int32 sectorY = startSectorY; sectorY <= endSectorY; ++sectorY) {
        for (int32 sectorX = startSectorX; sectorX <= endSectorX; ++sectorX) {
            CRepeatSector* repeatSector = GetRepeatSector(sectorX, sectorY);
            if (RcbanditCheck1CarWheels(repeatSector->GetList(REPEATSECTOR_VEHICLES)))
                break;
        }
    }
    return true;
}

// 0x729B60
void CAutomobile::FireTruckControl(CFire* fire) {
    if (this != FindPlayerVehicle()) {
        if (m_nStatus != STATUS_PHYSICS || !fire)
            return;

        CVector2D distance = fire->m_vecPosition - GetPosition();
        float shootDir = std::atan2(-distance.x, distance.y);
        float heading = GetHeading();
        if (shootDir > heading + PI)
            shootDir -= TWO_PI;
        else if (shootDir < heading - PI)
            shootDir += TWO_PI;

        float doomVerticalRotation = shootDir - heading;
        if (doomVerticalRotation > m_fDoomVerticalRotation + PI)
            doomVerticalRotation -= TWO_PI;
        else if (doomVerticalRotation < m_fDoomVerticalRotation - PI)
            doomVerticalRotation += TWO_PI;

        float doomVerticalRotDiff = doomVerticalRotation - m_fDoomVerticalRotation;
        float timeStep = CTimer::GetTimeStep() / 100.0f;
        if (std::fabs(doomVerticalRotDiff) >= timeStep) {
            if (doomVerticalRotDiff <= 0.0f)
                m_fDoomVerticalRotation -= timeStep;
            else
                m_fDoomVerticalRotation += timeStep;
        }
        m_fDoomHorizontalRotation = std::sin(float(CTimer::GetTimeInMS() % 4096) * TWO_PI * 1.f / 4096.f) * 0.15f;
    }
    else {
        CCam& activeCam = CCamera::GetActiveCamera();
        if (activeCam.m_nMode != MODE_CAM_ON_A_STRING)
        {
            CPad* pad = CPad::GetPad();
            m_fDoomVerticalRotation   -= ((float)pad->GetCarGunLeftRight() * CTimer::GetTimeStep() / 20.0f) / 128.0f;
            m_fDoomHorizontalRotation += ((float)pad->GetCarGunUpDown()    * CTimer::GetTimeStepInSeconds()) / 128.0f;
        }
        else {
            CVector frontDot = GetMatrix().InverseTransformVector(activeCam.m_vecFront);
            float doomVerticalRotation   = std::atan2(-frontDot.x, frontDot.y);
            float doomHorizontalRotation = std::atan2(+frontDot.z, frontDot.Magnitude2D());

            if (ModelIndices::IsSwatVan(m_nModelIndex))
                doomHorizontalRotation += DegreesToRadians(22);
            else
                doomHorizontalRotation += DegreesToRadians(15);

            if (doomVerticalRotation > m_fDoomVerticalRotation + PI)
                doomVerticalRotation -= TWO_PI;
            else if (doomVerticalRotation < m_fDoomVerticalRotation - PI)
                doomVerticalRotation += TWO_PI;

            float doomVerticalRotDiff = doomVerticalRotation - m_fDoomVerticalRotation;
            float timeStep = CTimer::GetTimeStep() / 20.0f;
            if (doomVerticalRotDiff > timeStep)
                m_fDoomVerticalRotation += timeStep;
            else if (doomVerticalRotDiff < -timeStep)
                m_fDoomVerticalRotation -= timeStep;

            float doomHorizontalRotDiff = doomHorizontalRotation - m_fDoomHorizontalRotation;
            timeStep = CTimer::GetTimeStepInSeconds();
            if (doomHorizontalRotDiff > timeStep)
                m_fDoomHorizontalRotation += timeStep;
            else if (doomHorizontalRotDiff < -timeStep)
                m_fDoomHorizontalRotation -= timeStep;
        }

        if (m_fDoomVerticalRotation > PI)
            m_fDoomVerticalRotation -= TWO_PI;
        else if (m_fDoomVerticalRotation < -PI)
            m_fDoomVerticalRotation += TWO_PI;

        if (ModelIndices::IsSwatVan(m_nModelIndex))
            m_fDoomHorizontalRotation = std::clamp<float>(m_fDoomHorizontalRotation, -DegreesToRadians(10), DegreesToRadians(35));
        else
            m_fDoomHorizontalRotation = std::clamp<float>(m_fDoomHorizontalRotation, -DegreesToRadians(20), DegreesToRadians(20));

        if (!CPad::GetPad()->GetCarGunFired())
            return;
    }

    CVector newTurretPosition;
    if (m_aCarNodes[CAR_MISC_A]) {
        RwMatrix* carNodeMiscMatrix = RwFrameGetLTM(m_aCarNodes[CAR_MISC_A]);
        newTurretPosition = *RwMatrixGetPos(carNodeMiscMatrix);
    } else {
        newTurretPosition = m_matrix->TransformPoint(CVector(0.0f, 1.5f, 1.8f));
    }

    CVector point{
        -(std::sin(m_fDoomVerticalRotation) * std::cos(m_fDoomHorizontalRotation)),
        std::cos(m_fDoomVerticalRotation)   * std::cos(m_fDoomHorizontalRotation),
        std::sin(m_fDoomHorizontalRotation),
    };
    point = GetMatrix().TransformVector(point); // 0x72A062

    if (m_aCarNodes[CAR_MISC_A]) {
        if (ModelIndices::IsSwatVan(m_nModelIndex))
            newTurretPosition += point * 2.0f;
        else
            newTurretPosition += point * 1.2f;

        newTurretPosition += GetSpeed(newTurretPosition - GetPosition()) * CTimer::GetTimeStep();
    }

    point.z += float(CGeneral::GetRandomNumber() % 16) / 1000.0f;
    CVector endPoint = m_vecMoveSpeed * CVector(1.0f, 1.0f, 0.3f);
    if (ModelIndices::IsSwatVan(m_nModelIndex))
        endPoint += point * 0.4f;
    else
        endPoint += point * 0.5f;
    CWaterCannons::UpdateOne((uint32)this, &newTurretPosition, &endPoint);
}

// 0x44D520
bool CAutomobile::HasCarStoppedBecauseOfLight() {
    return ((bool(__thiscall*)(CAutomobile*))0x44D520)(this); // TODO: Reverse
}

/*!
* @notsa
*/
CBouncingPanel* CAutomobile::CheckIfExistsGetFree(eCarNodes nodeIdx) {
    // NOTE/TODO/BUG:
    // This is the original code.. I'm not sure how it works..
    // It checks panel's frame == `nodeIdx`, but only up until the first one whose frame is == -1... Weird, maybe a bug?
    for (auto&& panel : m_panels) {
        if (panel.m_nFrameId == (uint16)-1) {
            return &panel;
        } else if (panel.m_nFrameId == nodeIdx) {
            return nullptr;
        }
    }
    return nullptr;
}

// 0x6AAB50
void CAutomobile::PreRender() {
    plugin::CallMethod<0x6AAB50, CAutomobile*>(this);
}

// 0x6A2B10
void CAutomobile::Render() {
    plugin::CallMethod<0x6A2B10, CAutomobile*>(this);
}

// 0x6A9CA0
void CAutomobile::Teleport(CVector destination, bool resetRotation) {
    CWorld::Remove(this);
    GetPosition() = destination;

    if (resetRotation)
        SetOrientation(0.0f, 0.0f, 0.0f);

    ResetMoveSpeed();
    ResetTurnSpeed();
    ResetSuspension();
    CWorld::Add(this);
}

// 0x6A0750
RwObject* GetCurrentAtomicObjectCB(RwObject* object, void* data) {
    if (RpAtomicGetFlags(object) & rpATOMICRENDER)
        *reinterpret_cast<RpAtomic**>(data) = reinterpret_cast<RpAtomic*>(object);

    return object;
}

// NOTSA
RwObject* GetCurrentAtomicObject(RwFrame* frame) {
    RwObject* out = nullptr;
    RwFrameForAllObjects(frame, GetCurrentAtomicObjectCB, &out);
    return out;
}
