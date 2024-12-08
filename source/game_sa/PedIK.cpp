/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

static inline bool& bRotateWithNeck = *(bool*)0x8D2354;

void CPedIK::InjectHooks() {
    RH_ScopedClass(CPedIK);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x5FD8C0);
    RH_ScopedInstall(RotateTorso, 0x5FDDB0);
    RH_ScopedInstall(RotateTorsoForArm, 0x5FDF90);
    RH_ScopedOverloadedInstall(MoveLimb, "non-normalized", 0x5FDA60, MoveLimbResult(CPedIK::*)(LimbOrientation&, float, float, LimbMovementInfo&));
    RH_ScopedOverloadedInstall(MoveLimb, "normalized", 0x5FDB60, MoveLimbResult(CPedIK::*)(LimbOrientation&, float, float, LimbMovementInfo&, float));
    RH_ScopedInstall(PointGunAtPosition, 0x5FDE20);
    RH_ScopedInstall(PointGunInDirection, 0x5FDC00);
    RH_ScopedInstall(PitchForSlope, 0x5FE0E0);
    RH_ScopedInstall(GetWorldMatrix, 0x5FD8F0);
}

// 0x5FDDB0
void CPedIK::RotateTorso(AnimBlendFrameData* bone, LimbOrientation& orientation, bool changeRoll) {
    const auto q = &bone->KeyFrame->q;
    RtQuatRotate(q, &XaxisIK, RadiansToDegrees(orientation.m_fYaw), rwCOMBINEREPLACE);
    RtQuatRotate(q, &ZaxisIK, RadiansToDegrees(orientation.m_fPitch), rwCOMBINEPRECONCAT);
    m_pPed->bDontAcceptIKLookAts = true;
}

// 0x5FDF90
void CPedIK::RotateTorsoForArm(const CVector& direction) {
    auto destRotation = CVector2D{direction - m_pPed->GetPosition()}.Heading();

    // this might be inlined
    if (m_pPed->m_fCurrentRotation + DegreesToRadians(180.0f) < destRotation) {
        destRotation -= DegreesToRadians(360.0f);
    } else if (m_pPed->m_fCurrentRotation - DegreesToRadians(180.0f) > destRotation) {
        destRotation += DegreesToRadians(360.0f);
    }

    const auto difAngle = destRotation - m_pPed->m_fCurrentRotation;
    auto resultAngle = DegreesToRadians(45.0f);

    if (difAngle > DegreesToRadians(45.0f) && difAngle <= DegreesToRadians(90.0f)) {
        resultAngle = difAngle - DegreesToRadians(45.0f);
    } else {
        if (difAngle >= DegreesToRadians(-60.0f))
            return;

        resultAngle = difAngle + DegreesToRadians(60.0f);

        if (difAngle < DegreesToRadians(-80.0f)) {
            resultAngle = DegreesToRadians(-20.0f);
        }
    }

    if (resultAngle != DegreesToRadians(0.0f)) {
        const auto degreesHalf = RadiansToDegrees(resultAngle / 2.0f);
        if (bRotateWithNeck) { // android doesn't have this check
            RtQuatRotate(&m_pPed->m_apBones[PED_NODE_NECK]->KeyFrame->q, &XaxisIK, degreesHalf, rwCOMBINEPOSTCONCAT);
        }
        RtQuatRotate(&m_pPed->m_apBones[PED_NODE_UPPER_TORSO]->KeyFrame->q, &XaxisIK, degreesHalf, rwCOMBINEPOSTCONCAT);
    }
}

// 0x5FDC00
bool CPedIK::PointGunInDirection(float zAngle, float distance, bool flag, float normalize) {
    bGunReachedTarget = false;
    bTorsoUsed = true;

    const auto angle = CGeneral::LimitRadianAngle(zAngle - m_pPed->m_fCurrentRotation);
    const auto hier  = GetAnimHierarchyFromSkinClump(m_pPed->m_pRwClump);
    const auto index = RpHAnimIDGetIndex(hier, m_pPed->m_apBones[PED_NODE_RIGHT_CLAVICLE]->BoneTag);

    // unused code
    // auto* boneMatrix = RwMatrixCreate();
    // *boneMatrix = RpHAnimHierarchyGetMatrixArray(hier)[index];
    // RwMatrixDestroy(boneMatrix);

    const auto limbResult = [&]() -> MoveLimbResult {
        if (normalize < 0.0f) {
            return MoveLimb(m_TorsoOrient, angle, distance, ms_torsoInfo);
        } else {
            return MoveLimb(m_TorsoOrient, angle, distance, ms_torsoInfo, normalize);
        }
    }();

    bool canReach = true;
    switch (limbResult) {
    case CANT_REACH_TARGET:
        canReach = false;
        break;
    case REACHED_TARGET:
        bGunReachedTarget = true;
        break;
    }

    const auto head = &RpHAnimHierarchyGetMatrixArray(hier)[PED_NODE_HEAD];
    const auto headAngle = -CGeneral::LimitRadianAngle(atan2(-head->at.y, -head->at.x) - m_pPed->m_fCurrentRotation);

    const auto axis = CVector{
        0.0f,
        flag ? std::cos(headAngle) : -std::sin(headAngle),
        flag ? std::sin(headAngle) :  std::cos(headAngle)
    };

    const auto torsoQ = &m_pPed->m_apBones[PED_NODE_UPPER_TORSO]->KeyFrame->q;
    RtQuatRotate(torsoQ, &axis, RadiansToDegrees(m_TorsoOrient.m_fPitch), rwCOMBINEPOSTCONCAT);
    RtQuatRotate(torsoQ, &XaxisIK, RadiansToDegrees(m_TorsoOrient.m_fYaw), rwCOMBINEPOSTCONCAT);
    m_pPed->bUpdateMatricesRequired = true;

    return canReach;
}

// 0x5FDE20
void CPedIK::PointGunAtPosition(const CVector& posn, float normalize) {
    const auto weaponAnimOffsetIdx = CWeaponInfo::GetWeaponInfo(m_pPed)->m_nAimOffsetIndex;

    const CVector newRight = [this, weaponAnimOffsetIdx] {
        if (m_pPed->bIsDucking) {
            return g_GunAimingOffsets[weaponAnimOffsetIdx].DuckX * m_pPed->GetRight();
        } else {
            return g_GunAimingOffsets[weaponAnimOffsetIdx].AimX * m_pPed->GetRight();
        }
    }();

    const CVector newUp = [this, weaponAnimOffsetIdx] {
        if (m_pPed->bIsDucking) {
            return g_GunAimingOffsets[weaponAnimOffsetIdx].DuckZ * m_pPed->GetUp();
        } else {
            return g_GunAimingOffsets[weaponAnimOffsetIdx].AimZ * m_pPed->GetUp();
        }
    }();

    const CVector newPos = m_pPed->GetPosition() + newRight + newUp;

    const auto angle = CGeneral::GetRadianAngleBetweenPoints(posn, newPos);
    const auto dist = DistanceBetweenPoints2D(newPos, posn);
    PointGunInDirection(angle, CGeneral::GetRadianAngleBetweenPoints(posn.z, dist, newPos.z, 0.0f), false, normalize);
}

// 0x5FE0E0
void CPedIK::PitchForSlope() {
    const auto clumpData = RpAnimBlendClumpGetData(m_pPed->m_pRwClump);
    const auto hier = GetAnimHierarchyFromSkinClump(m_pPed->m_pRwClump);

    if (std::abs(m_fBodyRoll) > 0.01f) {
        m_fBodyRoll = std::clamp(m_fBodyRoll, DegreesToRadians(-30.0f), DegreesToRadians(30.0f));
    }

    if (!m_pPed->IsStateDying()) {
        if (std::abs(m_fSlopePitch) > 0.01f) {
            const auto offset = (m_pPed->m_nMoveState != PEDMOVE_WALK) ? 0.1f : 0.085f;
            m_fSlopePitchLimitMult = (m_fSlopePitchLimitMult * 0.9f) + offset;

            const auto lowBound  = m_fSlopePitchLimitMult * DegreesToRadians(-35.0f);
            const auto highBound = m_fSlopePitchLimitMult * DegreesToRadians(+30.0f);
            m_fSlopePitch = std::clamp(m_fSlopePitch, lowBound, highBound);
        }

        if (std::abs(m_fSlopeRoll) > 0.01f) {
            m_fSlopeRoll = std::clamp(m_fSlopeRoll, DegreesToRadians(-45.0f), DegreesToRadians(45.0f));
        }

        const auto GetAnimHierarchyMatrix = [&hier](eBoneTag bone) {
            return &RpHAnimHierarchyGetMatrixArray(hier)[RpHAnimIDGetIndex(hier, bone)];
        };

        const auto RotateBone = [clumpData, &hier](eBoneTag bone, float angle, const CVector& axis = ZaxisIK) {
            RtQuatRotate(&clumpData->m_FrameDatas[RpHAnimIDGetIndex(hier, bone)].KeyFrame->q, &axis, angle, rwCOMBINEPRECONCAT);
        };

        if (std::abs(m_fSlopePitch) > 0.01f) {
            if (&m_pPed->GetIntelligence()->m_TaskMgr && m_pPed->GetIntelligence()->m_TaskMgr.GetActiveTask()->GetTaskType() == TASK_SIMPLE_STEALTH_KILL) {
                RotateBone(BONE_SPINE, RadiansToDegrees(m_fSlopePitch));
                m_pPed->bUpdateMatricesRequired = true;
            } else {
                RotateBone(BONE_SPINE, RadiansToDegrees(-m_fSlopePitch) * 0.75f);
                m_pPed->bUpdateMatricesRequired = true;

                RotateBone(BONE_R_THIGH, std::asin(GetAnimHierarchyMatrix(BONE_R_THIGH)->up.z) * 30.0f * m_fSlopePitch);
                RotateBone(BONE_R_CALF, std::max(-std::asin(GetAnimHierarchyMatrix(BONE_R_THIGH)->up.z), 0.0f) * -30.0f * m_fSlopePitch);
                RotateBone(BONE_R_FOOT, std::max(std::asin(GetAnimHierarchyMatrix(BONE_R_FOOT)->up.z), 0.0f) * -30.0f * m_fSlopePitch);
                RotateBone(BONE_L_THIGH, std::asin(GetAnimHierarchyMatrix(BONE_L_THIGH)->up.z) * 30.0f * m_fSlopePitch);
                RotateBone(BONE_L_CALF, std::max(-std::asin(GetAnimHierarchyMatrix(BONE_L_THIGH)->up.z), 0.0f) * -30.0f * m_fSlopePitch);
                RotateBone(BONE_L_FOOT, std::max(-std::asin(GetAnimHierarchyMatrix(BONE_L_FOOT)->up.z), 0.0f) * -30.0f * m_fSlopePitch);

                CMatrix result(m_pPed->GetModellingMatrix(), false);
                CMatrix translate;
                translate.SetTranslate(CVector{0.0f, 0.0f, 0.0f});
                translate.RotateX(-m_fSlopePitch / 2.0f);
                result *= translate;

                result.GetPosition() += m_pPed->GetForward() * 0.75f * std::sin(m_fSlopePitch);
                result.GetPosition().z += 0.3f / std::cos(m_fSlopeRoll) - 0.3f;
                result.UpdateRW();
                m_pPed->UpdateRwFrame();
            }
        } else {
            m_fSlopePitch = 0.0f;
        }

        if (std::abs(m_fSlopeRoll) > 0.01f) {
            const auto RotateFoot = [&](eBoneTag bone) {
                const auto hierMatrix = GetAnimHierarchyMatrix(bone);
                const auto angle = CGeneral::LimitRadianAngle(atan2(hierMatrix->at.y, hierMatrix->at.x) - m_pPed->m_fCurrentRotation);

                RotateBone(bone, RadiansToDegrees(m_fSlopeRoll), CVector{0.0f, std::cos(angle), std::sin(angle)});
            };

            RotateFoot(BONE_L_FOOT);
            RotateFoot(BONE_R_FOOT);
            m_pPed->bUpdateMatricesRequired = true;
        } else {
            m_fSlopeRoll = 0.0f;
        }

    } else {
        CMatrix matResult(m_pPed->GetModellingMatrix());

        CMatrix matTransl;
        matTransl.SetTranslate(ZaxisIK);
        matTransl.RotateX(-m_fSlopePitch);
        matTransl.RotateY(m_fSlopeRoll);
        matResult *= matTransl;
        matResult.GetPosition().z -= 1.0f;
        matResult.UpdateRW();
        m_pPed->UpdateRwFrame();
    }

    bSlopePitch = false;
}

// unused
// 0x5FD8F0
RwMatrixTag* CPedIK::GetWorldMatrix(RwFrame* frame, RwMatrixTag* transformMat) {
    *transformMat = *RwFrameGetMatrix(frame);
    for (frame = RwFrameGetParent(frame); frame; frame = RwFrameGetParent(frame)) {
        RwMatrixTransform(transformMat, RwFrameGetMatrix(frame), rwCOMBINEPOSTCONCAT);
    }
    return transformMat;
}

// 0x5FDA60
MoveLimbResult CPedIK::MoveLimb(LimbOrientation& limb, float targetYaw, float targetPitch, LimbMovementInfo& moveInfo) {
    auto result = HAVENT_REACHED_TARGET;

    // yaw
    if (std::abs(limb.m_fYaw - targetYaw) < moveInfo.yawD) {
        limb.m_fYaw = targetYaw;
        result = REACHED_TARGET;
    } else {
        if (limb.m_fYaw > targetYaw) {
            limb.m_fYaw -= moveInfo.yawD;
        } else if (limb.m_fYaw < targetYaw) {
            limb.m_fYaw += moveInfo.yawD;
        }
    }

    if (limb.m_fYaw > moveInfo.maxYaw || limb.m_fYaw < moveInfo.minYaw) {
        limb.m_fYaw = std::clamp(limb.m_fYaw, moveInfo.minYaw, moveInfo.maxYaw);
        result = CANT_REACH_TARGET;
    }

    // pitch
    if (std::abs(limb.m_fPitch - targetPitch) < moveInfo.pitchD) {
        limb.m_fPitch = targetPitch;
    } else {
        if (limb.m_fPitch > targetPitch) {
            limb.m_fPitch -= moveInfo.pitchD;
        } else if (limb.m_fPitch < targetPitch) {
            limb.m_fPitch += moveInfo.pitchD;
        }
        result = HAVENT_REACHED_TARGET;
    }

    if (limb.m_fPitch > moveInfo.maxPitch || limb.m_fPitch < moveInfo.minPitch) {
        limb.m_fPitch = std::clamp(limb.m_fPitch, moveInfo.minPitch, moveInfo.maxPitch);
        result = CANT_REACH_TARGET;
    }

    return result;
}

// 0x5FDB60
MoveLimbResult CPedIK::MoveLimb(LimbOrientation& limb, float targetYaw, float targetPitch, LimbMovementInfo& moveInfo, float normalize) {
    auto result = HAVENT_REACHED_TARGET;

    limb.m_fYaw = normalize * targetYaw;
    limb.m_fPitch = normalize * targetPitch;

    if (limb.m_fYaw > moveInfo.maxYaw || limb.m_fYaw < moveInfo.minYaw) {
        limb.m_fYaw = std::clamp(limb.m_fYaw, moveInfo.minYaw, moveInfo.maxYaw);
        result = CANT_REACH_TARGET;
    }

    if (limb.m_fPitch > moveInfo.maxPitch || limb.m_fPitch < moveInfo.minPitch) {
        limb.m_fPitch = std::clamp(limb.m_fPitch, moveInfo.minPitch, moveInfo.maxPitch);
        result = CANT_REACH_TARGET;
    } else if (normalize > 0.9f && result == HAVENT_REACHED_TARGET) {
        result = REACHED_TARGET;
    }

    return result;
}
