#include "StdInc.h"
#include "IKChain_c.h"
#include "BoneNodeManager_c.h"
#include "Enums/eIKChainSlot.h"


void IKChain_c::InjectHooks() {
    RH_ScopedClass(IKChain_c);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x618370);
    RH_ScopedInstall(Exit, 0x617870);
    RH_ScopedInstall(Update, 0x6184B0);
    RH_ScopedInstall(IsAtTarget, 0x617F30);
    RH_ScopedInstall(IsFacingTarget, 0x617E60);
    RH_ScopedInstall(UpdateTarget, 0x617E50);
    RH_ScopedInstall(UpdateOffset, 0x617E20);
    RH_ScopedInstall(ClampLimits, 0x618520);
    RH_ScopedInstall(UpdateEntity, 0x617E00);
    RH_ScopedInstall(GetBoneNodeFromTag, 0x617C60);
    RH_ScopedInstall(GetPriority, 0x617C50);
    RH_ScopedInstall(SetOffsetPos, 0x617C30);
    RH_ScopedInstall(SetOffsetBoneTag, 0x617C20);
    RH_ScopedInstall(SetBlend, 0x617C10);
    RH_ScopedInstall(MoveBonesToTarget, 0x6178B0);
    RH_ScopedInstall(SetupBones, 0x617CA0);
    RH_ScopedInstall(GetLimits, 0x618590);
}

// 0x618370
bool IKChain_c::Init(
    const char*  name,
    eIKChainSlot ikSlot,
    CPed*        ped,
    eBoneTag32   effectorBone,
    RwV3d        effectorPos,
    eBoneTag32   pivotBone,
    CEntity*     entity,
    eBoneTag32   offsetBone,
    RwV3d        offsetPos,
    float        speed,
    int8         priority
) {
    m_Ped = ped;

    const auto frames = m_Ped->GetAnimBlendData().GetFrames();

    if (frames[0].NeedsKeyFrameUpdate || !frames[0].IsUpdatingFrame) {
        return false;
    }

    if (!m_Ped->bHasBeenRendered) { // Bone data isn't present yet in this case
        return false;
    }

    // Check if frame of this bone has non-zero translation
    {
        const auto& boneFrame = frames[RpHAnimIDGetIndex(&m_Ped->GetAnimHierarchy(), (RwInt32)effectorBone)].KeyFrame;
        if (boneFrame->t.IsZero()) {
            return false;
        }
    }

    SetupBones(effectorBone, effectorPos, pivotBone, frames);
    UpdateEntity(entity);

    m_EffectorPos  = effectorPos;
    m_EffectorBone = effectorBone;
    m_OffsetBone   = offsetBone;
    m_OffsetPos    = offsetPos;
    m_Speed        = speed;
    m_UpdateTarget = true;
    m_IKSlot       = ikSlot;
    m_Priority     = priority;

    return true;
}

// 0x617870
void IKChain_c::Exit() {
    for (auto&& bone : GetBones()) {
        g_boneNodeMan.ReturnBoneNode(bone);
    }
    delete[] m_Bones;
    m_Bones = nullptr;
}

// 0x6184B0
void IKChain_c::Update(float timeStep) {
    UNUSED(timeStep);

    m_PivotBoneMatrix = m_Ped->GetBoneMatrix(m_PivotBone);
    GetBones()[m_BonesCount - 1]->CalcWldMat(m_PivotBoneMatrix);
    MoveBonesToTarget();
    for (auto& bone : GetBones()) {
        bone->BlendKeyframe(m_Blend);
    }
}

// 0x617F30
bool IKChain_c::IsAtTarget(float maxDist, float* outDist) const {
    const auto dist = (m_OffsetPosWS - GetBones().front()->GetPosition()).Magnitude();
    if (outDist) {
        *outDist = dist;
    }
    return dist <= maxDist && m_Blend > 0.98f;
}

// 0x617E60
bool IKChain_c::IsFacingTarget() const {
    // Genuine cancer :D

    RwV3d targetPos;
    RwV3dTransformVector(&targetPos, &m_EffectorPos, &GetBones().front()->GetMatrix());
    RwV3dNormalize(&targetPos, &targetPos);

    RwV3d dir;
    RwV3dSub(&dir, &m_OffsetPosWS, &targetPos);
    RwV3dNormalize(&dir, &dir);

    return RwV3dDotProduct(&dir, &targetPos) >= 0.95f && m_Blend > 0.98f;
}

// 0x617E50
void IKChain_c::UpdateTarget(bool bUpdate) {
    m_UpdateTarget = bUpdate;
}

// 0x617E20
void IKChain_c::UpdateOffset(eBoneTag32 offsetBone, CVector offsetPosn) {
    m_OffsetBone = offsetBone;
    m_OffsetPos  = offsetPosn;
}

// 0x618520
void IKChain_c::ClampLimits(eBoneTag32 boneTag, bool LimitX, bool LimitY, bool LimitZ, bool UseCurrentLimits) {
    auto& bone = *GetBoneNodeFromTag(boneTag);
    if (UseCurrentLimits) {
        bone.ClampLimitsCurrent(LimitX, LimitY, LimitZ);
    } else {
        bone.ClampLimitsDefault(LimitX, LimitY, LimitZ);
    }
}

// 0x617E00
void IKChain_c::UpdateEntity(CEntity* entity) {
    // NOTE: Original code didn't un-register the ref, but we're going to do that here (Or well, the Ref wrapper does it for us)
    m_TargetEntity = entity;
}

// 0x617C60
BoneNode_c* IKChain_c::GetBoneNodeFromTag(eBoneTag32 tag) {
    const auto it = rng::find_if(GetBones(), [tag](auto&& b) {
        return b->GetBoneTag() == tag;
    });
    return it != GetBones().end()
        ? *it
        : nullptr;
}

// 0x617C50
int8 IKChain_c::GetPriority() const {
    return m_Priority;
}

// 0x617C30
void IKChain_c::SetOffsetPos(CVector value) {
    m_OffsetPos = value;
}

// 0x617C20
void IKChain_c::SetOffsetBoneTag(eBoneTag32 offsetBone) {
    m_OffsetBone = offsetBone;
}

// 0x617C10
void IKChain_c::SetBlend(float value) {
    m_Blend = value;
}

// 0x6178B0
void IKChain_c::MoveBonesToTarget() {
    if (m_UpdateTarget) {
        if (m_TargetEntity) {
            const auto mat = m_TargetEntity->GetModellingMatrix();
            if (m_OffsetBone == BONE_UNKNOWN) {
                if (mat) {
                    RwV3dTransformPoint(&m_OffsetPosWS, &m_OffsetPos, mat);
                }
            } else {
                m_OffsetPosWS = m_TargetEntity->AsPed()->GetBonePosition((eBoneTag)m_OffsetBone);
                if (mat) {
                    RwV3d transformed;
                    RwV3dTransformVector(&transformed, &m_OffsetPos, mat);
                    m_OffsetPosWS += transformed;
                }
            }
        } else {
            m_OffsetPosWS = m_OffsetPos;
        }
    }

    auto bones = GetBones();
    if (bones.empty()) {
        return;
    }

    constexpr auto DIST_TOLERANCE = 0.00001f;

    const auto rootBone = bones.front();
    for (auto&& bone : bones) {
        RwV3d transformedBonePos;
        RwV3dTransformVector(&transformedBonePos, &m_EffectorPos, &rootBone->GetMatrix()); // Not sure why it's transformed every iteration..

        CVector dirBoneToRoot = transformedBonePos + (rootBone->GetPosition() - bone->GetPosition());
        if (dirBoneToRoot.Magnitude() <= DIST_TOLERANCE) {
            continue;
        }
        RwV3dNormalize(&dirBoneToRoot, &dirBoneToRoot);

        CVector dirBoneToOffset = m_OffsetPosWS - bone->GetPosition();
        if (dirBoneToOffset.Magnitude() <= DIST_TOLERANCE) {
            continue;
        }
        RwV3dNormalize(&dirBoneToOffset, &dirBoneToOffset);

        const auto dot = DotProduct(&dirBoneToOffset, &dirBoneToRoot);
        if (dot >= 0.997f) { // Make sure they don't point in the same direction
            continue;
        }

        const auto boneParentMat = bone->GetParent()
            ? &bone->GetParent()->GetMatrix()
            : m_PivotBoneMatrix;

        RtQuat quat;
        RtQuatConvertFromMatrix(&quat, boneParentMat);

        // NOTE: Normalize Quat.. I wonder why there isn't a macro or something for this?
        {
            const auto mag = RwV3dDotProduct(&quat.imag, &quat.imag) + quat.real * quat.real;
            if (mag > 0.f) {
                const auto recp = 1.f / mag;
                quat.real *= recp;
                RwV3dScale(&quat.imag, &quat.imag, -recp);
            }
        }

        CVector cross = CrossProduct(dirBoneToRoot, dirBoneToOffset);
        RwV3d axis;
        RtQuatTransformVectors(&axis, &cross, 1, &quat);
        RtQuatRotate(reinterpret_cast<RtQuat*>(&bone->GetOrientation()), &axis, RadiansToDegrees(bone->GetSpeed() * std::acos(dot) * m_Speed), rwCOMBINEPOSTCONCAT);

        bone->Limit(m_Blend); // Originally this was in an `if`, but the variable that was checked was always `true`
        bone->CalcWldMat(boneParentMat);
    }
}

// 0x617CA0
void IKChain_c::SetupBones(eBoneTag32 effectorBone, CVector effectorPos, eBoneTag32 pivotBone, AnimBlendFrameData* frames) {
    m_PivotBoneMatrix = m_Ped->GetBoneMatrix(pivotBone);
    m_PivotBone       = pivotBone;

    const auto GetBoneLinkPrev = [](eBoneTag32 b) {
        return BoneNodeManager_c::ms_boneInfos[BoneNode_c::GetIdFromBoneTag(b)].m_prev;
    };

    // Allocate bone nodes we use (Going from effector -> pivot)
    BoneNode_c* bones[MAX_BONE_NUM];
    m_BonesCount = 0;
    for (auto boneIt = effectorBone; boneIt != pivotBone; boneIt = GetBoneLinkPrev(boneIt)) {
        const auto node = g_boneNodeMan.GetBoneNode();
        node->Init(boneIt, frames[RpHAnimIDGetIndex(&m_Ped->GetAnimHierarchy(), (RwInt32)boneIt)].KeyFrame);
        bones[m_BonesCount++] = node;
    }

    // Now we know the exact size to allocate, so do that
    m_Bones = new BoneNode_c*[m_BonesCount];
    for (auto i = 0; i < m_BonesCount; i++) {
        m_Bones[i] = bones[i];
    }

    // Link bones together
    for (auto& currBone : GetBones()) {
        const auto prevBoneId = GetBoneLinkPrev(currBone->GetBoneTag());
        if (prevBoneId > -1) {
            if (const auto prevBoneNode = GetBoneNodeFromTag(prevBoneId)) {
                prevBoneNode->AddChild(currBone);
            }
        }
    }
}

// 0x618590
void IKChain_c::GetLimits(eBoneTag32 boneTag, eRotationAxis axis, float& outMin, float& outMax) {
    GetBoneNodeFromTag(boneTag)->GetLimits(axis, outMin, outMax);
}
