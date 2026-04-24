#include "StdInc.h"
#include "BoneNode_c.h"
#include "BoneNodeManager_c.h"

#include "rtslerp.h"

void BoneNode_c::InjectHooks() {
    RH_ScopedClass(BoneNode_c);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x616B30);
    RH_ScopedInstall(Destructor, 0x616B80);

    RH_ScopedInstall(Init, 0x6177B0);
    RH_ScopedInstall(InitLimits, 0x617490);
    RH_ScopedGlobalInstall(EulerToQuat, 0x6171F0);
    RH_ScopedGlobalInstall(QuatToEuler, 0x617080);
    RH_ScopedGlobalInstall(GetIdFromBoneTag, 0x617050);
    RH_ScopedInstall(ClampLimitsCurrent, 0x6175D0);
    RH_ScopedInstall(ClampLimitsDefault, 0x617530);
    RH_ScopedInstall(Limit, 0x617650);
    RH_ScopedInstall(BlendKeyframe, 0x616E30);
    RH_ScopedInstall(GetSpeed, 0x616CB0);
    RH_ScopedInstall(SetSpeed, 0x616CC0);
    RH_ScopedInstall(SetLimits, 0x616C50);
    RH_ScopedInstall(GetLimits, 0x616BF0);
    RH_ScopedInstall(AddChild, 0x616BD0);
    RH_ScopedInstall(CalcWldMat, 0x616CD0);
}

// 0x6177B0
bool BoneNode_c::Init(int32 boneTag, RpHAnimBlendInterpFrame* interpFrame) {
    m_BoneTag     = static_cast<eBoneTag>(boneTag);
    m_InterpFrame = interpFrame;
    m_Orientation = interpFrame->q;
    m_Pos         = interpFrame->t;
    m_Parent      = nullptr;

    m_Childs.RemoveAll();
    InitLimits();

    m_Speed = 1.0f;

    return true;
}

// 0x617490
void BoneNode_c::InitLimits() {
    const auto id = GetIdFromBoneTag(m_BoneTag);
    assert(id != -1);
    const auto& boneInfo = BoneNodeManager_c::ms_boneInfos[id]; // Possible out of bounds when `id` = -1 (they don't check for -1) | unnecessary copying

    m_LimitMin.x = boneInfo.m_Max.x - boneInfo.m_Min.x;
    m_LimitMin.y = boneInfo.m_Max.y - boneInfo.m_Min.z;
    m_LimitMin.z = boneInfo.m_Max.z - boneInfo.m_ABC.y;

    m_LimitMax.x = boneInfo.m_Min.y + boneInfo.m_Max.x;
    m_LimitMax.y = boneInfo.m_ABC.x + boneInfo.m_Max.y;
    m_LimitMax.z = boneInfo.m_ABC.z + boneInfo.m_Max.z;
}

// 0x6171F0
void BoneNode_c::EulerToQuat(const CVector& angles, RtQuat& quat) {
    const CVector halfRadAngles = {
        DegreesToRadians(angles.x) / 2.f,
        DegreesToRadians(angles.y) / 2.f,
        DegreesToRadians(angles.z) / 2.f
    };

    float cr = std::cos(halfRadAngles.x);
    float sr = std::sin(halfRadAngles.x);
    float cp = std::cos(halfRadAngles.y);
    float sp = std::sin(halfRadAngles.y);
    float cy = std::cos(halfRadAngles.z);
    float sy = std::sin(halfRadAngles.z);

    quat.real = cr * cp * cy + sr * sp * sy;
    quat.imag.x = sr * cp * cy - cr * sp * sy;
    quat.imag.y = cr * sp * cy + sr * cp * sy;
    quat.imag.z = cr * cp * sy - sr * sp * cy;
}

// 0x617080
void BoneNode_c::QuatToEuler(const RtQuat& quat, CVector& angles) {
    // refactor this fuck
    const auto v9 = 2.0f * (quat.imag.x * quat.imag.z - quat.imag.y * quat.real);
    const auto v10 = std::sqrt(1.0f - sq(v9));

    angles.y = RadiansToDegrees(std::atan2(2.0f * (quat.imag.y * quat.real - quat.imag.x * quat.imag.z), v10));
    if (std::abs(v9) == 1.0f) {
        angles.x = RadiansToDegrees(std::atan2(-2.0f * (quat.imag.y * quat.imag.z - quat.imag.x * quat.real), 1.0f - 2.0f * (sq(quat.imag.x) + sq(quat.imag.z))));
        angles.z = RadiansToDegrees(0.0f);
    } else {
        angles.x = RadiansToDegrees(std::atan2(2.0f * (quat.imag.x * quat.real + quat.imag.y * quat.imag.z) / v10, (1.0f - 2.0f * (sq(quat.imag.x) + sq(quat.imag.y))) / v10));
        angles.z = RadiansToDegrees(std::atan2(2.0f * (quat.imag.z * quat.real + quat.imag.x * quat.imag.y) / v10, (1.0f - 2.0f * (sq(quat.imag.y) + sq(quat.imag.z))) / v10));
    }
}

// 0x617050
int32 BoneNode_c::GetIdFromBoneTag(eBoneTag32 bone) {
    for (auto i = 0u; i < BoneNodeManager_c::ms_boneInfos.size(); i++) {
        if (BoneNodeManager_c::ms_boneInfos[i].m_current == bone) {
            return i;
        }
    }
    return -1;
}

// Empty in Android
// 0x6175D0
void BoneNode_c::ClampLimitsCurrent(bool LimitX, bool LimitY, bool LimitZ) {
    if (*(bool*)0x8D2BD1) // always true
        return;

    CVector angles;
    BoneNode_c::QuatToEuler(m_Orientation, angles);
    if (LimitX) {
        m_LimitMax.x = angles.x;
        m_LimitMin.x = angles.x;
    }
    if (LimitY) {
        m_LimitMax.y = angles.y;
        m_LimitMin.y = angles.y;
    }
    if (LimitZ) {
        m_LimitMax.z = angles.z;
        m_LimitMin.z = angles.z;
    }
}

// Empty in Android
// 0x617530
void BoneNode_c::ClampLimitsDefault(bool LimitX, bool LimitY, bool LimitZ) {
    if (*(bool*)0x8D2BD0) // always true
        return;

    const auto id = GetIdFromBoneTag(m_BoneTag);
    const auto& boneInfo = BoneNodeManager_c::ms_boneInfos[id];

    if (LimitX) {
        m_LimitMax.x = boneInfo.m_Min.x;
        m_LimitMin.x = boneInfo.m_Min.x;
    }
    if (LimitY) {
        m_LimitMax.y = boneInfo.m_Min.y;
        m_LimitMin.y = boneInfo.m_Min.y;
    }
    if (LimitZ) {
        m_LimitMax.z = boneInfo.m_Min.z;
        m_LimitMin.z = boneInfo.m_Min.z;
    }
}

// argument (float blend) - ignored
// 0x617650
void BoneNode_c::Limit(float blend) {
    CVector eulerOrientation{};

    BoneNode_c::QuatToEuler(m_Orientation, eulerOrientation);

    eulerOrientation.x = std::clamp(eulerOrientation.x, m_LimitMin.x, m_LimitMax.x);
    eulerOrientation.y = std::clamp(eulerOrientation.y, m_LimitMin.y, m_LimitMax.y);

    float clampZMin = m_LimitMin.z;
    float clampZMax = m_LimitMax.z;

    if (m_BoneTag == eBoneTag::BONE_HEAD) {
        float maxHeadZ = BoneNodeManager_c::ms_boneInfos[GetIdFromBoneTag(eBoneTag::BONE_HEAD)].m_Max.z;
        float multy = std::max(std::abs(eulerOrientation.x) / -45.0f + 1.0f, 0.0f);

        clampZMin = maxHeadZ + (m_LimitMin.z - maxHeadZ) * multy;
        clampZMax = maxHeadZ + (m_LimitMax.z - maxHeadZ) * multy;
    }

    eulerOrientation.z = std::clamp(eulerOrientation.z, clampZMin, clampZMax);

    BoneNode_c::EulerToQuat(eulerOrientation, m_Orientation);
}

// 0x616E30
void BoneNode_c::BlendKeyframe(float blend) {
    auto src = m_InterpFrame->q;
    auto dst = m_Orientation;
    RtQuatSlerpCache cache;
    RtQuatSetupSlerpCache(&src, &dst, &cache);
    RtQuatSlerp(&m_InterpFrame->q, &src, &dst, blend, &cache);
}

// 0x616CB0
float BoneNode_c::GetSpeed() const {
    return m_Speed;
}

// 0x616CC0
void BoneNode_c::SetSpeed(float speed) {
    m_Speed = speed;
}

// 0x616C50
void BoneNode_c::SetLimits(eRotationAxis axis, float min, float max) {
     m_LimitMin[axis] = min;
     m_LimitMax[axis] = max;
}

// 0x616BF0
void BoneNode_c::GetLimits(eRotationAxis axis, float& outMin, float& outMax) const {
    outMin = m_LimitMin[axis];
    outMax = m_LimitMax[axis];
}

// 0x616BD0
void BoneNode_c::AddChild(BoneNode_c* children) {
    children->m_Parent = this;
    m_Childs.AddItem(children);
}

// 0x616CD0
void BoneNode_c::CalcWldMat(const RwMatrix* boneMatrix) {
    RwMatrix rotMatrix = [this] {
        CMatrix mat{};
        mat.SetRotate(CQuaternion{m_Orientation});
        mat.GetPosition() = m_Pos;
        return mat.ToRwMatrix();
    }();

    rwMatrixSetFlags(&rotMatrix, rwMATRIXTYPEORTHONORMAL);
    RwMatrixMultiply(&m_WorldMat, &rotMatrix, boneMatrix);

    for (auto bone = m_Childs.GetHead(); bone; bone = m_Childs.GetNext(bone)) {
        bone->CalcWldMat(&m_WorldMat);
    }
}
