#include "StdInc.h"

#include <numbers>

#include "Bmx.h"
#include "CarAI.h"
#include "CarCtrl.h"
#include "Shadows.h"

// NOTE: The game's own values - `PI` / `TWO_PI` / `FRAC_PI_2` from `common.h` are a few ulp off
constexpr float BMX_PI                 = std::numbers::pi_v<float>;            // 0x871504
constexpr float BMX_HALF_PI            = std::numbers::pi_v<float> / 2.0f;     // 0x87150C
constexpr float BMX_TWO_PI             = std::numbers::pi_v<float> * 2.0f;     // 0x858CBC
constexpr float BMX_INV_TWO_PI         = std::numbers::inv_pi_v<float> * 0.5f; // 0x8594F0

constexpr float BMX_CRANK_ANGLE_OFFSET = 0.0f;  // 0x871500
constexpr float BMX_CRANK_DAMPING      = 0.97f; // 0x871510
constexpr float BMX_ANIM_LEAN_DAMPING  = 0.95f; // 0x8714E8
constexpr float BMX_SPRINT_PEDALING    = 5.0f;  // 0x858C80 - `m_ControlPedaling` above this is a sprint
constexpr float BMX_MAX_CONTROL_JUMP   = 25.0f; // 0x871514
constexpr float BMX_BUNNY_HOP_HOLD_END = 0.2f;  // 0x871518 - The bunny hop anim is held in its first 0.2s while the hop is charged

//! When set the wheel frames follow the suspension, otherwise the whole frame does (See `GetFrameOffset`). Only read by `PreRender`, never written
static inline auto& s_SuspensionMovesWheels = StaticRef<bool>(0xC1C83C);

void CBmx::InjectHooks() {
    RH_ScopedVirtualClass(CBmx, 0x871528, 67);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(Constructor, 0x6BF820);
    RH_ScopedVMTInstall(SetUpWheelColModel, 0x6BF9B0);
    RH_ScopedVMTInstall(BurstTyre, 0x6BF9C0);
    RH_ScopedVMTInstall(FindWheelWidth, 0x6C0550);
    RH_ScopedVMTInstall(ProcessControl, 0x6BFA30);
    RH_ScopedVMTInstall(ProcessDrivingAnims, 0x6BFB50);
    RH_ScopedVMTInstall(PreRender, 0x6C0810);
    RH_ScopedVMTInstall(ProcessAI, 0x6C1470);
    RH_ScopedInstall(ProcessBunnyHop, 0x6C0590);
    RH_ScopedInstall(LaunchBunnyHopCB, 0x6C0390);
}

// 0x6BF820
CBmx::CBmx(int32 modelIndex, eVehicleCreatedBy createdBy) :
    CBike{ modelIndex, createdBy } {
    m_nVehicleSubType        = VEHICLE_TYPE_BMX;

    const auto animGroup     = CAnimManager::GetAnimBlocks()[CModelInfo::GetModelInfo(modelIndex)->GetAnimFileIndex()].GroupId;
    m_RideAnimData.AnimGroup = animGroup < ANIM_GROUP_BMX || animGroup > ANIM_GROUP_CHOPPA
        ? ANIM_GROUP_BMX
        : animGroup;

    m_nFixLeftHand           = false;
    m_nFixRightHand          = false;

    const auto GetFramePosY  = [&](eBmxNodes node) {
        auto* const frame  = m_aBikeNodes[node];
        RwMatrix    matrix = *RwFrameGetMatrix(frame);
        for (auto* parent = RwFrameGetParent(frame); parent && parent != frame; parent = RwFrameGetParent(parent)) {
            RwMatrixTransform(&matrix, RwFrameGetMatrix(parent), rwCOMBINEPOSTCONCAT);
        }
        return matrix.pos.y;
    };
    const float frontWheelY = GetFramePosY(BMX_WHEEL_FRONT);
    m_MidWheelDistY         = frontWheelY - GetFramePosY(BMX_WHEEL_REAR);
    m_MidWheelFracY         = frontWheelY / m_MidWheelDistY;
}

// 0x6BF9D0
CBmx::~CBmx() {
    m_vehicleAudio.Terminate();
}

// 0x6BF9B0
bool CBmx::SetUpWheelColModel(CColModel* wheelCol) {
    return false;
}

// 0x6BF9C0
bool CBmx::BurstTyre(uint8 tyreComponentId, bool physicalEffect) {
    return false;
}

// 0x6BFA30
void CBmx::ProcessControl() {
    constexpr float BMX_SPRINT_LEANSTART = BMX_HALF_PI; // 0x8D328C
    constexpr float BMX_PEDAL_LEANSTART  = 0.0f;        // 0xC1C820
    constexpr float BMX_SPRINT_LEANMULT  = 0.3f;        // 0x8D3290
    constexpr float MTB_SPRINT_LEANMULT  = 0.087f;      // 0x8D3298
    constexpr float BMX_PEDAL_LEANMULT   = 0.07f;       // 0x8D3294
    constexpr float MTB_PEDAL_LEANMULT   = 0.02f;       // 0x8D329C

    CBike::ProcessControl();

    if (GetWasPostponed() || GetStatus() != STATUS_PLAYER || !m_pDriver) {
        return;
    }

    const bool isMountainBike = GetModelId() == MODEL_MTBIKE;
    const auto GetAnimSway    = [](const CAnimBlendAssociation* a, float phase, float mult) {
        return std::sin(a->GetCurrentTime() / a->GetHier()->GetTotalTime() * BMX_TWO_PI + phase) * a->GetBlendAmount() * mult;
    };

    const auto* const aSprint = RpAnimBlendClumpGetAssociation(m_pDriver->GetRpClump(), ANIM_ID_BIKE_SPRINT);
    if (aSprint && aSprint->GetBlendAmount() > 0.01f) {
        m_SprintLeanAngle = GetAnimSway(aSprint, BMX_SPRINT_LEANSTART, isMountainBike ? MTB_SPRINT_LEANMULT : BMX_SPRINT_LEANMULT);
        return;
    }

    const auto* const aPedal = RpAnimBlendClumpGetAssociation(m_pDriver->GetRpClump(), ANIM_ID_BIKE_PEDAL);
    if (aPedal && aPedal->GetBlendAmount() > 0.01f) {
        m_RideAnimData.LeanAngle += GetAnimSway(aPedal, BMX_PEDAL_LEANSTART, isMountainBike ? MTB_PEDAL_LEANMULT : BMX_PEDAL_LEANMULT);
    }
    m_SprintLeanAngle *= 0.95f; // NOTE: Framerate dependent
}

// 0x6BFB50
void CBmx::ProcessDrivingAnims(CPed* driver, bool blend) {
    if (m_bOffscreen && (!driver || !driver->IsPlayer())) {
        return;
    }

    m_nFixLeftHand    = true;
    m_nFixRightHand   = true;

    auto* const clump = driver->GetRpClump();
    if (const auto* const aBunnyHop = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_BUNNYHOP)) { // 0x6BFB9E
        m_CrankAngle  = (1.0f - aBunnyHop->GetBlendAmount()) * m_CrankAngle + BMX_HALF_PI * aBunnyHop->GetBlendAmount();
        m_PedalAngleL = -m_CrankAngle;
        m_PedalAngleR = -m_CrankAngle;
        return;
    }

    if (driver->GetPlayerData()) {
        driver->SetMoveState(PEDMOVE_NONE);
    }

    const float fwdSpeed       = m_vecMoveSpeed.Dot(GetForward());

    auto*             aPedal   = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_PEDAL);
    auto*             aSprint  = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_SPRINT);
    const auto* const aLeft    = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_LEFT);
    const auto* const aRight   = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_RIGHT);
    const auto* const aFwd     = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_FWD);
    const auto*       aDriveBy = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_DRIVEBYLHS);
    if (!aDriveBy) {
        aDriveBy = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_DRIVEBYRHS);
        if (!aDriveBy) {
            aDriveBy = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_DRIVEBYFT);
        }
    }

    const auto DampCrankAngle = [this] {
        m_CrankAngle *= std::pow(BMX_CRANK_DAMPING, CTimer::GetTimeStep());
    };
    const auto GetCrankAngleOfAnim = [](const CAnimBlendAssociation* a) {
        return BMX_CRANK_ANGLE_OFFSET - a->GetCurrentTime() / a->GetHier()->GetTotalTime() * BMX_TWO_PI;
    };
    const auto DampAnimLean = [this] { // NOTE: Framerate dependent
        m_RideAnimData.AnimLeanLeft *= BMX_ANIM_LEAN_DAMPING;
        m_RideAnimData.AnimLeanFwd *= BMX_ANIM_LEAN_DAMPING;
    };

    const float maxLeanToPedal = m_ControlPedaling > BMX_SPRINT_PEDALING ? 0.7f : 0.4f;
    if (std::abs(m_RideAnimData.LeanAngle) < maxLeanToPedal && m_RideAnimData.LeanFwd < maxLeanToPedal && fwdSpeed > 0.01f && !aDriveBy) { // 0x6BFCFC
        const bool  isMountainBike = GetModelId() == MODEL_MTBIKE;
        const float maxSprintSpeed = isMountainBike ? 2.0f : 2.5f;
        const float animSpeed      = isMountainBike
            ? m_nCurrentGear
                ? (5.0f * fwdSpeed) / ((float)m_nCurrentGear * m_pHandlingData->GetTransmission().m_MaxFlatVelocity - 0.25f)
                : 0.0f
            : 3.0f * fwdSpeed;

        // Blends the anim in, unless it's already (being) blended in
        bool       justBlended    = false;
        const auto BlendCycleAnim = [&](CAnimBlendAssociation* a, AnimationId animId) {
            if (a && (a->GetBlendAmount() >= 1.0f || a->GetBlendDelta() > 0.0f)) {
                return a;
            }
            justBlended = true;
            return CAnimManager::BlendAnimation(clump, m_RideAnimData.AnimGroup, animId, 4.0f);
        };

        CAnimBlendAssociation* aCycle{};                                             // The anim the crank follows
        if (m_ControlPedaling > BMX_SPRINT_PEDALING && animSpeed < maxSprintSpeed) { // 0x6BFD8B - Sprint
            aSprint = BlendCycleAnim(aSprint, ANIM_ID_BIKE_SPRINT);
            aSprint->SetFlag(ANIMATION_IS_PLAYING, true);
            if (aPedal) {
                aPedal->SetFlag(ANIMATION_IS_PLAYING, true);
            }
            aCycle = aPedal ? aPedal : aSprint;
            aCycle->SetSpeed(animSpeed);
        } else if (m_GasPedal == 0.0f && m_ControlPedaling <= 0.0f && GetStatus() != STATUS_SIMPLE) { // 0x6BFE33 - Freewheel
            aCycle = BlendCycleAnim(aPedal, ANIM_ID_BIKE_PEDAL);
            aCycle->SetFlag(ANIMATION_IS_PLAYING, false);
            if (!vehicleFlags.bIsHandbrakeOn && rng::any_of(m_aRatioHistory, [](float r) {
                    return r < 1.0f;
                })) {
                m_IsFreewheeling = true;
            }
        } else { // 0x6BFEDE - Pedal
            aCycle = BlendCycleAnim(aPedal, ANIM_ID_BIKE_PEDAL);
            aCycle->SetFlag(ANIMATION_IS_PLAYING, true);
            aCycle->SetSpeed(animSpeed);
        }

        // 0x6BFF3E - Where the crank is while the rider leans (The anim just blended in continues from there)
        const auto GetCrankAngleOfLeanAnim = [&]() -> std::optional<float> {
            if (aLeft && aLeft->GetBlendAmount() > 0.5f) {
                return BMX_PI;
            }
            if (aRight && aRight->GetBlendAmount() > 0.5f) {
                return 0.0f; // 0x871508
            }
            if (aFwd && aFwd->GetBlendAmount() > 0.5f) {
                return BMX_HALF_PI;
            }
            return std::nullopt;
        };

        if (!aCycle) { // 0x6C0016
            DampCrankAngle();
        } else if (const auto startAngle = justBlended ? GetCrankAngleOfLeanAnim() : std::nullopt) { // 0x6BFFA9
            const float progress        = (BMX_CRANK_ANGLE_OFFSET - *startAngle) * BMX_INV_TWO_PI;
            const float wrappedProgress = progress < 0.0f ? progress + 1.0f : progress;
            aCycle->SetCurrentTime(aCycle->GetHier()->GetTotalTime() * wrappedProgress);
            m_CrankAngle = wrappedProgress; // NOTE: The original stores the anim's progress [0, 1) here, not an angle
        } else {                            // 0x6BFFFF
            m_CrankAngle = GetCrankAngleOfAnim(aCycle);
        }

        if (std::abs(m_RideAnimData.AnimLeanLeft) > 0.05f || std::abs(m_RideAnimData.AnimLeanFwd) > 0.05f) { // 0x6C0061
            DampAnimLean();
        }
    } else { // 0x6C008C - Leaning / stopped / doing a drive-by
        const auto IsBlendedIn = [](const CAnimBlendAssociation* a) {
            return a && a->GetBlendDelta() >= 0.0f && a->GetBlendAmount() > 0.0f;
        };
        if (IsBlendedIn(aPedal) || IsBlendedIn(aSprint)) { // 0x6C00D4
            for (auto* const a : { aPedal, aSprint }) {
                if (a) {
                    a->SetFlag(ANIMATION_IS_PLAYING, false);
                    a->SetBlendDelta(-8.0f);
                }
            }
            DampAnimLean();
        } else { // 0x6C011A
            ProcessRiderAnims(driver, this, &m_RideAnimData, m_BikeHandling, 0);
        }

        // 0x6C0134
        const auto* const aCycle = aPedal ? aPedal : aSprint;
        m_CrankAngle             = aCycle ? GetCrankAngleOfAnim(aCycle) : 0.0f;

        // 0x6C0161 - Move the crank to where the lean anim has it
        const auto BlendCrankAngleTo = [this](float angle, float blend) {
            m_CrankAngle = (1.0f - blend) * m_CrankAngle + angle * blend;
        };
        if (aLeft && aLeft->GetBlendAmount() > 0.1f) {
            BlendCrankAngleTo(BMX_PI, aLeft->GetBlendAmount());
            m_IsFreewheeling = true;
        } else if (aRight && aRight->GetBlendAmount() > 0.1f) {
            BlendCrankAngleTo(0.0f, aRight->GetBlendAmount());
            m_IsFreewheeling = true;
        } else if (aFwd && aFwd->GetBlendAmount() > 0.1f) {
            BlendCrankAngleTo(BMX_HALF_PI, aFwd->GetBlendAmount());
        } else {
            DampCrankAngle();
        }

        if (aDriveBy) { // 0x6C0233
            m_nFixRightHand  = false;
            m_IsFreewheeling = true;
        }
    }

    if (driver->IsPlayer()) { // 0x6C0252
        const auto& wheelCounts     = m_WheelCounts;
        const float fwdZ            = GetForward().z;

        bikeFlags.bWheelieForCamera = false;
        if (wheelCounts[0] <= 0.0f && wheelCounts[1] <= 0.0f && fwdZ > 0.0f && (wheelCounts[2] > 0.0f || wheelCounts[3] > 0.0f)) { // 0x6C02C1 - Wheelie
            if (m_BikeHandling->m_fWheelieAng * 0.5f > m_BikeHandling->m_fWheelieAng - fwdZ) {
                bikeFlags.bWheelieForCamera = true;
            }
        } else if (wheelCounts[2] <= 0.0f && wheelCounts[3] <= 0.0f && fwdZ < 0.0f && (wheelCounts[0] > 0.0f || wheelCounts[1] > 0.0f)) { // 0x6C0344 - Stoppie
            if (m_BikeHandling->m_fStoppieAng * 0.6f < m_BikeHandling->m_fStoppieAng - fwdZ) {
                bikeFlags.bWheelieForCamera = true;
            }
        }
    }

    // 0x6C036B
    m_PedalAngleL = -m_CrankAngle;
    m_PedalAngleR = -m_CrankAngle;
}

// 0x6C0390
void CBmx::LaunchBunnyHopCB(CAnimBlendAssociation* assoc, void* data) {
    auto* const bmx = static_cast<CBmx*>(data);

    if (bmx->m_WheelCounts[0] <= 0.0f && bmx->m_WheelCounts[1] <= 0.0f) {
        return;
    }
    if (bmx->m_WheelCounts[2] <= 0.0f && bmx->m_WheelCounts[3] <= 0.0f) {
        return;
    }

    const float charge = std::min(bmx->m_ControlJump / BMX_MAX_CONTROL_JUMP, 1.0f) + 1.0f;
    const float skill  = bmx->GetStatus() == STATUS_PLAYER
        ? CStats::GetFatAndMuscleModifier(STAT_MOD_6) * charge
        : charge;
    const float power  = CCheat::IsActive(CHEAT_HUGE_BUNNY_HOP)
        ? skill * 5.0f
        : skill;
    bmx->ApplyMoveForce(0.06f * bmx->GetMass() * power * bmx->GetUp());
    bmx->ApplyTurnForce(0.01f * bmx->m_fTurnMass * power * bmx->GetUp(), bmx->GetForward());
}

// 0x6C0500 | inlined | see 0x6C11F3
void CBmx::GetFrameOffset(float& zOffset, float& angleOffset) {
    const float frontOffset = m_aWheelSuspensionHeights[0] - m_aWheelOrigHeights[0];
    const float rearOffset  = m_aWheelSuspensionHeights[1] - m_aWheelOrigHeights[1];

    zOffset                 = (1.0f - m_MidWheelFracY) * frontOffset + rearOffset * m_MidWheelFracY;
    angleOffset             = std::atan2(frontOffset - rearOffset, m_MidWheelDistY);
}

// 0x6C0550
float CBmx::FindWheelWidth(bool rear) {
    return 0.07f;
}

// 0x6C0560
void CBmx::BlowUpCar(CEntity* damager, bool hideExplosion) {
    // NOP
}

// 0x6C0590
void CBmx::ProcessBunnyHop() {
    auto* aBunnyHop     = m_pDriver
        ? RpAnimBlendClumpGetAssociation(m_pDriver->GetRpClump(), ANIM_ID_BIKE_BUNNYHOP)
        : nullptr;

    const auto BlendOut = [&] {
        aBunnyHop->SetFlag(ANIMATION_IS_PLAYING, true);
        aBunnyHop->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE, true);
        aBunnyHop->SetBlendDelta(-8.0f);
    };

    if (GetStatus() != STATUS_PLAYER || !m_pDriver || !m_pDriver->IsPlayer()) { // 0x6C07F5
        if (aBunnyHop) {
            BlendOut();
        }
        return;
    }

    const auto* const pad = m_pDriver->AsPlayer()->GetPadFromPlayer();

    if (pad->IsLeftShoulder1Pressed() && !pad->DisablePlayerControls && m_ControlJump == 0.0f) { // 0x6C0611 - Start charging
        m_ControlJump += CTimer::GetTimeStep();
        aBunnyHop = CAnimManager::BlendAnimation(m_pDriver->GetRpClump(), m_RideAnimData.AnimGroup, ANIM_ID_BIKE_BUNNYHOP, 8.0f);
        aBunnyHop->SetCurrentTime(0.0f);
        aBunnyHop->SetFlag(ANIMATION_IS_PLAYING, false);
    }

    if (m_ControlJump > 0.0f && !pad->DisablePlayerControls) { // 0x6C0724
        if (!aBunnyHop) {
            m_ControlJump = 0.0f;
            return;
        }
        if (!aBunnyHop->IsPlaying()) {
            if (pad->IsLeftShoulder1()) { // 0x6C0744 - Charging
                m_ControlJump = notsa::step_up_to(m_ControlJump, BMX_MAX_CONTROL_JUMP, CTimer::GetTimeStep());
                aBunnyHop->SetCurrentTime(m_ControlJump / BMX_MAX_CONTROL_JUMP * BMX_BUNNY_HOP_HOLD_END);
            } else { // 0x6C0799 - Released: play the rest of the anim, the hop is launched at the end of it
                if (aBunnyHop->GetCurrentTime() < BMX_BUNNY_HOP_HOLD_END) {
                    aBunnyHop->SetCurrentTime(
                        (BMX_BUNNY_HOP_HOLD_END - aBunnyHop->GetCurrentTime()) / BMX_BUNNY_HOP_HOLD_END * (aBunnyHop->GetHier()->GetTotalTime() - BMX_BUNNY_HOP_HOLD_END) + BMX_BUNNY_HOP_HOLD_END
                    );
                }
                aBunnyHop->SetFlag(ANIMATION_IS_PLAYING, true);
                aBunnyHop->SetSpeed(1.5f);
                aBunnyHop->SetFinishCallback(CBmx::LaunchBunnyHopCB, this);
            }
        }
    } else { // 0x6C067A
        if (m_ControlJump > 0.0f) {
            m_ControlJump = 0.0f;
        }
        if (!aBunnyHop) {
            return;
        }
        BlendOut();
    }

    if (aBunnyHop->GetBlendAmount() > 0.5f) { // 0x6C06A1
        m_GasPedal                                     = 0.0f;
        FindPlayerPed()->GetPlayerData()->m_fMoveSpeed = 0.0f;
        if (!vehicleFlags.bIsHandbrakeOn && rng::any_of(m_aWheelRatios, [](float r) {
                return r < 1.0f;
            })) {
            m_IsFreewheeling = true;
        }
    }
}

// 0x6C0810
void CBmx::PreRender() {
    CVehicle::PreRender();

    const auto* const cm    = GetColModel();
    const auto* const lines = GetColData()->m_pLines;
    auto* const       mi    = GetVehicleModelInfo();

    if (vehicleFlags.bVehicleColProcessed) { // 0x6C0858
        DoBurstAndSoftGroundRatios();

        const auto UpdateSuspensionHeight = [&](int32 wheel) {
            const int32 line       = wheel * 2;
            const float minRatio   = 1.0f - m_fSuspensionLength[line] / m_fLineLength[line];
            const float compressed = (std::min(m_aWheelRatios[line], m_aWheelRatios[line + 1]) - minRatio) / (1.0f - minRatio);
            const float height     = compressed > 0.0f
                ? lines[line].m_vecStart.z - compressed * m_fSuspensionLength[line]
                : lines[line].m_vecStart.z;
            m_aWheelSuspensionHeights[wheel] += (height - m_aWheelSuspensionHeights[wheel]) * 0.75f; // NOTE: Framerate dependent
        };
        UpdateSuspensionHeight(0);
        UpdateSuspensionHeight(1);
    }

    if (notsa::contains({ STATUS_PHYSICS, STATUS_PLAYER, STATUS_PLAYER_PLAYBACK_FROM_BUFFER, STATUS_SIMPLE }, GetStatus())) { // 0x6C097C
        const float speed = m_vecMoveSpeed.Magnitude();
        for (auto i = 0u; i < m_WheelStates.size(); i++) {
            const int32  wheel = i == 0
                ? (m_aRatioHistory[0] >= 1.0f && m_aRatioHistory[1] < 1.0f ? 1 : 0)
                : (m_aRatioHistory[3] >= 1.0f && m_aRatioHistory[2] < 1.0f ? 2 : 3);
            const uint32 flags =
                (i == 0 || m_WheelStates[1] == WHEEL_STATE_FIXED ? WHEEL_PARTICLE_NOSKIDSMOKE : 0)
                | (m_bWheelBloody[i] ? WHEEL_PARTICLE_BLOODY : 0)
                | (m_bMoreSkidMarks[i] ? WHEEL_PARTICLE_EXTRASKIDS : 0);

            auto&   cp  = m_aWheelColPoints[wheel];
            CVector pos = cp.m_vecPoint + GetRight() * (std::sin(m_RideAnimData.LeanAngle) * cm->GetBoundingBox().m_vecMin.z * 0.8f);
            AddSingleWheelParticles(
                m_WheelStates[i],
                m_nWheelStatus[i],
                m_aRatioHistory[wheel],
                speed,
                cp,
                pos,
                m_RideAnimData.LeanAngle > 0.0f ? -1.0f : 1.0f,
                (eCarWheel)i,
                m_aWheelSkidmarkType[i],
                m_bWheelBloody[i],
                (eWheelParticleFlags)flags
            );
        }
    }

    // 0x6C0B10
    m_bLeanMatrixCalculated = false;
    CalculateLeanMatrix();
    CShadows::StoreShadowForVehicle(this, VEH_SHD_BIKE);

    // 0x6C0B30 - Spin the wheels
    const CVector frontWheelFwd = m_matrix->TransformVector(CVector{ -std::sin(m_fSteerAngle), std::cos(m_fSteerAngle), 0.0f });
    const CVector rearWheelFwd  = GetForward();
    const auto    RotateWheel   = [&](int32 wheel, tWheelState state, const CVector& wheelFwd, float radius) {
        const int32 line = wheel * 2;
        if (m_WheelCounts[line] <= 0.0f && m_WheelCounts[line + 1] <= 0.0f) {
            return;
        }
        // NOTE: The original uses the front wheel's size for both contact points
        const CVector contactSpeed     = GetSpeed({
            0.0f,
            (lines[line + 1].m_vecStart.y + lines[line].m_vecStart.y) * 0.5f,
            lines[line].m_vecStart.z - std::min(m_aRatioHistory[line + 1], m_aRatioHistory[line]) * m_fSuspensionLength[line] - mi->GetWheelSize(true) * 0.5f,
        });
        m_aWheelAngularVelocity[wheel] = ProcessWheelRotation(state, wheelFwd, contactSpeed, radius);
        m_aWheelPitchAngles[wheel] += m_aWheelAngularVelocity[wheel] * CTimer::GetTimeStep();
    };
    RotateWheel(0, WHEEL_STATE_NORMAL, frontWheelFwd, mi->GetWheelSize(true) * 0.5f); // NOTE: The original doesn't use the front wheel's state
    RotateWheel(1, m_WheelStates[1], rearWheelFwd, mi->GetWheelSize(false) * 0.5f);

    // Each frame keeps its position, only its rotation is set
    CMatrix    mat;
    const auto AttachTo = [&](eBmxNodes node) {
        mat.Attach(RwFrameGetMatrix(m_aBikeNodes[node]), false);
        return CVector{ mat.GetPosition() };
    };
    const auto UpdateFrame = [&](const CVector& pos) {
        mat.GetPosition() += pos;
        mat.UpdateRW();
    };

    if (m_aBikeNodes[BMX_FORKS_FRONT]) { // 0x6C0DAF
        const CVector forkPos = AttachTo(BMX_FORKS_FRONT);

        RwMatrix steerRwMat;
        CMatrix  steerMat;
        steerMat.m_pAttachMatrix = &steerRwMat;
        steerMat.SetUnity();
        steerMat.UpdateRW();

        // Around the steering axis (That is tilted by the bike's steer angle)
        const float steerAxisAngle = mi->m_fBikeSteerAngle * (BMX_PI / 180.0f);
        CVector     steerAxis      = CVector{ 0.0f, std::sin(steerAxisAngle), -std::cos(steerAxisAngle) }.Normalized();
        CQuaternion steerQuat;
        steerQuat.Set(&steerAxis, -m_RideAnimData.BarSteerAngle);
        steerQuat.Get(steerMat.m_pAttachMatrix);
        steerMat.Update();

        mat.SetUnity();
        mat = mat * steerMat;
        UpdateFrame(forkPos);

        if (m_aBikeNodes[BMX_HANDLEBARS]) { // 0x6C0F09
            const CVector barsPos = AttachTo(BMX_HANDLEBARS);
            if (notsa::contains({ STATUS_ABANDONED, STATUS_WRECKED }, GetStatus())) { // 0x6C0F4B
                mat.SetUnity();
                mat *= steerMat;
                mat.GetPosition() += barsPos;
            } else { // 0x6C0F3D - The rider's hands turn the bars (See `CBike::FixHandsToBars`)
                mat.SetTranslate(barsPos);
            }
            mat.UpdateRW();
        }
    }

    if (m_aBikeNodes[BMX_FORKS_REAR]) { // 0x6C0FB4
        const float   swingArmAngle = -std::asin((m_aWheelSuspensionHeights[1] - m_aWheelOrigHeights[1]) / m_fSwingArmLength);
        const CVector pos           = AttachTo(BMX_FORKS_REAR);
        mat.SetRotate(swingArmAngle, 0.0f, 0.0f);
        UpdateFrame(pos);
    }

    const auto SetWheelRotation = [&](int32 wheel, float burstWobble) {
        const float pitch = m_aWheelPitchAngles[wheel];
        if (m_nWheelStatus[wheel] == WHEEL_STATUS_BURST) {
            mat.SetRotate(pitch, 0.0f, std::sin(pitch) * burstWobble);
        } else {
            mat.SetRotateX(pitch);
        }
    };

    { // 0x6C103C
        const CVector framePos = AttachTo(BMX_WHEEL_FRONT);
        const CVector pos      = s_SuspensionMovesWheels
            ? CVector{
                  framePos.x,
                  (lines[1].m_vecStart.y + lines[0].m_vecStart.y) * 0.5f - m_fForkYOffset - (m_aWheelSuspensionHeights[0] - m_aWheelOrigHeights[0]) * m_fSteerAngleTan,
                  m_aWheelSuspensionHeights[0] - m_fForkZOffset,
              }
            : framePos;
        SetWheelRotation(0, 0.02f);
        UpdateFrame(pos);
    }

    { // 0x6C1119
        const CVector framePos = AttachTo(BMX_WHEEL_REAR);
        const CVector pos      = s_SuspensionMovesWheels && !m_aBikeNodes[BMX_FORKS_REAR]
            ? CVector{ framePos.x, framePos.y, m_aWheelSuspensionHeights[1] }
            : framePos;
        SetWheelRotation(1, 0.04f);
        UpdateFrame(pos);
    }

    if (m_aBikeNodes[BMX_CHASSIS]) { // 0x6C11D9
        float zOffset{}, angleOffset{};
        if (!s_SuspensionMovesWheels) {
            GetFrameOffset(zOffset, angleOffset);
        }

        const CVector framePos = AttachTo(BMX_CHASSIS);
        const float   lean     = m_RideAnimData.LeanAngle;
        mat.SetRotateX(std::abs(lean) * -0.05f + angleOffset);
        mat.RotateY(m_SprintLeanAngle + lean);
        UpdateFrame({ framePos.x, framePos.y, (1.0f - std::cos(lean)) * cm->GetBoundingBox().m_vecMin.z * 0.9f + zOffset });
    }

    const auto SetFrameRotationX = [&](eBmxNodes node, float angle) {
        if (!m_aBikeNodes[node]) {
            return;
        }
        const CVector pos = AttachTo(node);
        mat.SetRotate(angle, 0.0f, 0.0f);
        UpdateFrame(pos);
    };
    SetFrameRotationX(BMX_CHAINSET, m_CrankAngle); // 0x6C12FC
    SetFrameRotationX(BMX_PEDAL_R, m_PedalAngleL); // 0x6C136B
    SetFrameRotationX(BMX_PEDAL_L, m_PedalAngleR); // 0x6C13DA
}

// 0x6C1470
bool CBmx::ProcessAI(uint32& extraHandlingFlags) {
    constexpr float BMX_PEDALING_ON_ACCELERATE = 4.9f;

    auto* const mi                             = GetVehicleModelInfo();

    m_autoPilot.carCtrlFlags.bHonkAtCar        = false;
    m_autoPilot.carCtrlFlags.bHonkAtPed        = false;
    m_IsFreewheeling                           = false;

    const auto ApplyCarJackedBrake             = [this] {
        m_GasPedal                  = 0.0f;
        m_BrakePedal                = 1.0f;
        vehicleFlags.bIsHandbrakeOn = true;
    };

    switch (GetStatus()) {
    case STATUS_PLAYER: { // 0x6C14B2
        extraHandlingFlags += 2;
        bikeFlags.bGettingPickedUp = false;
        if (!m_pDriver || !m_pDriver->IsPlayer()) {
            break;
        }

        ProcessControlInputs((uint8)m_pDriver->m_nPedType);

        auto* const driver = m_pDriver->AsPlayer();
        auto* const pad    = driver->GetPadFromPlayer();

        // 0x6C14FC - Leaning fwd / back moves the centre of mass, and pitches the bike
        const float leanFwd        = m_RideAnimData.LeanFwd;
        const auto  ApplyLeanForce = [&](float force) {
            ApplyTurnForce(-(CTimer::GetTimeStep() * force) * GetUp(), m_vecCentreOfMass + GetForward());
        };
        if (leanFwd < 0.0f) { // 0x6C151D
            m_vecCentreOfMass.y = m_BikeHandling->m_fLeanBakCOM * leanFwd + m_pHandlingData->m_vecCentreOfMass.y;
            if ((m_BrakePedal == 0.0f && !vehicleFlags.bIsHandbrakeOn) || !m_nNoOfContactWheels) {
                const float speed   = std::min(m_vecMoveSpeed.Magnitude(), 0.1f);
                const float gasMult = GetModelId() == MODEL_SANCHEZ
                    ? m_GasPedal * 0.7f + 0.3f
                    : (m_GasPedal + 1.0f) * 0.5f;
                ApplyLeanForce(gasMult * m_BikeHandling->m_fLeanBakForce * m_fTurnMass * leanFwd * speed);
            }
        } else { // 0x6C161F
            m_vecCentreOfMass.y = m_BikeHandling->m_fLeanFwdCOM * leanFwd + m_pHandlingData->m_vecCentreOfMass.y;
            if (m_BrakePedal < 0.0f || !m_nNoOfContactWheels) {
                ApplyLeanForce(m_BikeHandling->m_fLeanFwdForce * m_fTurnMass * std::min(m_vecMoveSpeed.Magnitude(), 0.1f) * leanFwd);
            }
        }

        // 0x6C1704
        PruneReferences();
        if (GetStatus() == STATUS_PLAYER) {
            DoDriveByShootings();
        }
        DoSoftGroundResistance(extraHandlingFlags);

        if (!IsAnyWheelMakingContactWithGround()) { // 0x6C175B - Turn the bike in the air
            const float turnSpeed = m_vecTurnSpeed.Dot(GetUp());
            const float steer     = (float)pad->GetSteeringLeftRight();
            if ((turnSpeed < 0.04f && steer < 0.0f) || (turnSpeed > -0.04f && steer > 0.0f)) {
                ApplyTurnForce(steer * (1.0f / 128.0f) * m_fTurnMass * CTimer::GetTimeStep() * 0.002f * GetRight(), GetForward());
            }
        }

        ProcessBunnyHop();

        // 0x6C1855 - Pedaling
        const auto* const aSprint = RpAnimBlendClumpGetAssociation(m_pDriver->GetRpClump(), ANIM_ID_BIKE_SPRINT);
        if (driver->ControlButtonSprint(SPRINT_BMX) > 1.2f || (driver->GetButtonSprintResults(SPRINT_BMX) > 1.0f && aSprint && aSprint->GetBlendAmount() > 0.5f)) { // 0x6C19AD
            bikeFlags.bPlayerBoost = true;
            m_ControlPedaling      = driver->GetPlayerData()->m_fMoveSpeed;
        } else { // 0x6C18B6
            driver->HandleSprintEnergy(false, std::max(0.5f, 1.0f - (float)pad->GetAccelerate() * (1.0f / 255.0f) * 0.5f));
            if (driver->GetButtonSprintResults(SPRINT_BMX) > 0.0f) { // 0x6C1912
                m_ControlPedaling = BMX_PEDALING_ON_ACCELERATE;
                if (m_GasPedal == 0.0f && m_BrakePedal == 0.0f) {
                    m_GasPedal = 1.0f;
                }
            } else if (driver->GetPlayerData()->m_fTimeCanRun >= 0.0f) { // 0x6C19A1
                m_ControlPedaling = 0.0f;
            } else if (pad->GetAccelerateJustDown()) { // 0x6C1973 - Out of breath
                m_ControlPedaling = BMX_PEDALING_ON_ACCELERATE;
            } else {                                                                    // 0x6C197F
                m_ControlPedaling = notsa::step_down_to(m_ControlPedaling, 0.0f, 0.4f); // NOTE: Framerate dependent
            }
        }

        // 0x6C19CE
        CStats::UpdateStatsWhenCycling(bikeFlags.bPlayerBoost, this);
        if (pad->CarGunJustDown()) {
            ActivateBomb();
        }
        break;
    }
    case STATUS_PLAYER_PLAYBACK_FROM_BUFFER: { // 0x6C1B65
        extraHandlingFlags += 2;
        break;
    }
    case STATUS_SIMPLE: { // 0x6C1B71
        CCarAI::UpdateCarAI(this);
        CPhysical::ProcessControl();
        CCarCtrl::UpdateCarOnRails(this);

        m_nNoOfContactWheels              = 2;
        m_NumDriveWheelsOnGroundLastFrame = m_NumDriveWheelsOnGround;
        m_NumDriveWheelsOnGround          = 2;

        m_pHandlingData->GetTransmission().CalculateGearForSimpleCar(m_autoPilot.m_speed * 0.02f, m_nCurrentGear);

        // NOTE: Not scaled by the time step (Unlike in `PreRender`)
        m_aWheelPitchAngles[0] += ProcessWheelRotation(WHEEL_STATE_NORMAL, GetForward(), m_vecMoveSpeed, mi->GetWheelSize(true) * 0.5f);
        m_aWheelPitchAngles[1] += ProcessWheelRotation(WHEEL_STATE_NORMAL, GetForward(), m_vecMoveSpeed, mi->GetWheelSize(false) * 0.5f);

        PlayHornIfNecessary();
        ReduceHornCounter();

        vehicleFlags.bVehicleColProcessed = false;
        vehicleFlags.bAudioChangingGear   = false;
        bikeFlags.bWheelieForCamera       = false;
        m_ControlPedaling                 = 0.0f;
        break;
    }
    case STATUS_PHYSICS: { // 0x6C1C5A
        CCarAI::UpdateCarAI(this);
        CCarCtrl::SteerAICarWithPhysics(this);
        PlayHornIfNecessary();

        extraHandlingFlags += 2;
        bikeFlags.bWheelieForCamera = false;
        if (vehicleFlags.bIsBeingCarJacked) {
            ApplyCarJackedBrake();
        } else {
            bikeFlags.bGettingPickedUp = false;
        }

        if (m_ControlPedaling > 0.0f) { // 0x6C1CD2 - Let the pedaling the AI asked for run out
            const float step  = m_ControlPedaling > BMX_SPRINT_PEDALING
                ? CTimer::GetTimeStep() * 0.02f
                : CTimer::GetTimeStep() * 0.01f * (vehicleFlags.bUseCarCheats ? 2.0f : 1.0f);
            m_ControlPedaling = notsa::step_down_to(m_ControlPedaling, 0.0f, step);
        }
        break;
    }
    case STATUS_ABANDONED: { // 0x6C1D33
        m_BrakePedal                = 0.0f;
        vehicleFlags.bIsHandbrakeOn = m_vecMoveSpeed.SquaredMagnitude() < 0.01f || bikeFlags.bOnSideStand;
        m_GasPedal                  = 0.0f;
        m_HornCounter               = 0;
        if ((m_pDriver || m_apPassengers[0] || vehicleFlags.bIsBeingCarJacked) && !bikeFlags.bOnSideStand) { // NOTE: Not `GetPassengers()`: the original reads the seat whatever the seat count is
            extraHandlingFlags += 2;
        }
        m_RideAnimData.AnimLeanLeft = 0.0f;
        m_RideAnimData.AnimLeanFwd  = 0.0f;
        bikeFlags.bWheelieForCamera = false;
        m_ControlPedaling           = 0.0f;
        if (vehicleFlags.bIsBeingCarJacked) {
            ApplyCarJackedBrake();
        }
        break;
    }
    case STATUS_WRECKED: { // 0x6C1E87
        m_BrakePedal                = 0.05f;
        vehicleFlags.bIsHandbrakeOn = true;
        m_fSteerAngle               = 0.0f;
        m_GasPedal                  = 0.0f;
        m_HornCounter               = 0;
        bikeFlags.bWheelieForCamera = false;
        m_ControlPedaling           = 0.0f;
        m_RideAnimData.AnimLeanLeft = 0.0f;
        m_RideAnimData.AnimLeanFwd  = 0.0f;
        break;
    }
    case STATUS_FORCED_STOP: { // 0x6C1E0E
        const bool isStopped        = m_vecMoveSpeed.SquaredMagnitude() < 0.01f;
        m_BrakePedal                = isStopped ? 1.0f : 0.0f;
        vehicleFlags.bIsHandbrakeOn = isStopped;
        m_fSteerAngle               = 0.0f;
        m_GasPedal                  = 0.0f;
        m_HornCounter               = 0;
        extraHandlingFlags += 2;
        bikeFlags.bWheelieForCamera = false;
        m_ControlPedaling           = 0.0f;
        break;
    }
    default:
        break;
    }

    if (m_pDriver) { // 0x6C1A0C - No pedaling while the rider leans fwd / back or does a drive-by
        auto* const clump       = m_pDriver->GetRpClump();
        const auto  IsBlendedIn = [](const CAnimBlendAssociation* a) {
            return a && a->GetBlendAmount() >= 0.5f;
        };

        const auto* aLean = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_FWD);
        if (!IsBlendedIn(aLean)) {
            aLean = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_BACK);
        }

        const auto* aDriveBy = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_DRIVEBYLHS);
        if (!IsBlendedIn(aDriveBy)) {
            aDriveBy = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_DRIVEBYRHS);
        }
        if (!IsBlendedIn(aDriveBy)) {
            aDriveBy = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_DRIVEBYFT);
        }

        if ((aLean && aLean->GetBlendAmount() > 0.5f) || (aDriveBy && aDriveBy->GetBlendAmount() > 0.5f)) { // 0x6C1AEF
            m_GasPedal = 0.0f;
            if (!vehicleFlags.bIsHandbrakeOn && rng::any_of(m_aWheelRatios, [](float r) {
                    return r < 1.0f;
                })) {
                m_IsFreewheeling = true;
            }
            return true;
        }
    }

    if (m_ControlPedaling > BMX_SPRINT_PEDALING && (m_aWheelRatios[2] < 1.0f || m_aWheelRatios[3] < 1.0f)) { // 0x6C1F18 - Sprinting pushes the bike (The slower it is the harder)
        const float fwdSpeed = m_vecMoveSpeed.Dot(GetForward());
        const float push     = std::clamp(2.4f - fwdSpeed / m_pHandlingData->GetTransmission().m_MaxFlatVelocity * 1.5f, 0.0f, 2.0f);
        const float mult     = GetStatus() == STATUS_PLAYER
            ? CStats::GetFatAndMuscleModifier(STAT_MOD_5) * push
            : vehicleFlags.bUseCarCheats
            ? push * 1.25f
            : push;
        ApplyMoveForce(CTimer::GetTimeStep() * m_fMass * 0.3f * mult * GAME_GRAVITY * GetForward());
    }
    return true;
}
