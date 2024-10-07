#include "StdInc.h"

void CBmx::InjectHooks() {
    RH_ScopedVirtualClass(CBmx, 0x871528, 67);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(Constructor, 0x6BF820);
    RH_ScopedVMTInstall(SetUpWheelColModel, 0x6BF9B0);
    RH_ScopedVMTInstall(BurstTyre, 0x6BF9C0);
    RH_ScopedVMTInstall(FindWheelWidth, 0x6C0550);
}

// 0x6BF820
CBmx::CBmx(int32 modelIndex, eVehicleCreatedBy createdBy) : CBike(modelIndex, createdBy) {
    auto mi = CModelInfo::GetModelInfo(modelIndex);
    m_nVehicleSubType = VEHICLE_TYPE_BMX;
    m_RideAnimData.m_nAnimGroup = CAnimManager::GetAnimBlocks()[mi->GetAnimFileIndex()].GroupId;
    if (m_RideAnimData.m_nAnimGroup < ANIM_GROUP_BMX || m_RideAnimData.m_nAnimGroup > ANIM_GROUP_CHOPPA) {
        m_RideAnimData.m_nAnimGroup = ANIM_GROUP_BMX;
    }

    m_fControlJump = 0.0f;
    m_fControlPedaling = 0.0f;
    m_fSprintLeanAngle = 0.0f;
    m_fCrankAngle = 0.0f;
    m_fPedalAngleL = 0.0f;
    m_fPedalAngleR = 0.0f;
    m_nFixLeftHand = false;
    m_nFixRightHand = false;
    m_bIsFreewheeling = false;

    const auto Calc = [&](eBmxNodes node) -> float {
        RwMatrix matrix;
        RwFrame* wheelFront = m_aBikeNodes[node];
        matrix = *RwFrameGetMatrix(wheelFront);

        auto parent = RwFrameGetParent(wheelFront);
        if (parent) {
            do {
                RwMatrixTransform(&matrix, RwFrameGetMatrix(parent), rwCOMBINEPOSTCONCAT);
                parent = RwFrameGetParent(parent);
            } while (parent != wheelFront && parent);
        }
        return matrix.pos.y;
    };
    auto wheelFrontPosY = Calc(BMX_WHEEL_FRONT);
    auto wheelRearPosY  = Calc(BMX_WHEEL_REAR);

    m_fMidWheelDistY = wheelFrontPosY - wheelRearPosY;
    m_fMidWheelFracY = wheelFrontPosY / m_fMidWheelDistY;
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
bool CBmx::BurstTyre(uint8 tyreComponentId, bool bPhysicalEffect) {
    return false;
}

// 0x6BFA30
void CBmx::ProcessControl() {
    plugin::CallMethod<0x6BFA30, CBmx*>(this);
}

// 0x6BFB50
void CBmx::ProcessDrivingAnims(CPed* driver, bool blend) {
    plugin::CallMethod<0x6BFB50, CBmx*, CPed*, uint8>(this, driver, blend);
}

// data is a ptr to CBmx
// 0x6C0390
void CBmx::LaunchBunnyHopCB(CAnimBlendAssociation* assoc, void* data) {
    return plugin::Call<0x6C0390, CAnimBlendAssociation*, void*>(assoc, data);
    auto bmx = static_cast<CBmx*>(data);
    if ((bmx->m_aWheelCounts[0] > 0.0f || bmx->m_aWheelCounts[1] > 0.0f) &&
        (bmx->m_aWheelCounts[2] > 0.0f || bmx->m_aWheelCounts[3] > 0.0f)
    ) {
        auto power = std::min(bmx->m_fControlJump / 25.0f, 1.0f) + 1.0f;
        if (bmx->m_nStatus != STATUS_PLAYER) {
            power *= CStats::GetFatAndMuscleModifier(STAT_MOD_6);
        }
        if (CCheat::IsActive(CHEAT_HUGE_BUNNY_HOP)) {
            power *= 5.0f;
        }
        bmx->ApplyMoveForce(0.06f * bmx->m_fMass     * power * bmx->m_matrix->GetUp());
        bmx->ApplyTurnForce(0.01f * bmx->m_fTurnMass * power * bmx->m_matrix->GetUp(), bmx->m_matrix->GetForward());
    }
}

// 0x6C0500 | inlined | see 0x6C11F3
void CBmx::GetFrameOffset(float& fZOffset, float& fAngleOffset) {
    const auto d1 = m_aWheelSuspensionHeights[0] - m_aWheelOrigHeights[0];
    const auto d2 = m_aWheelSuspensionHeights[1] - m_aWheelOrigHeights[1];

    fZOffset = (1.0f - m_fMidWheelFracY) * d1 + d2 * m_fMidWheelFracY;
    fAngleOffset = std::atan2(d1 - d2, m_fMidWheelDistY);
}

// 0x6C0550
float CBmx::FindWheelWidth(bool bRear) {
    return 0.07f;
}

// 0x6C0560
void CBmx::BlowUpCar(CEntity* damager, bool bHideExplosion) {
    // NOP
}

// 0x6C0590
void CBmx::ProcessBunnyHop() {
    plugin::CallMethod<0x6C0590, CBmx*>(this);
}

// 0x6C0810
void CBmx::PreRender() {
    plugin::CallMethod<0x6C0810, CBmx*>(this);
}

// 0x6C1470
bool CBmx::ProcessAI(uint32& extraHandlingFlags) {
    return plugin::CallMethodAndReturn<bool, 0x6C1470, CBmx*, uint32&>(this, extraHandlingFlags);
}
