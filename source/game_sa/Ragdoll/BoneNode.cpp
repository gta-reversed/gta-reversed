#include "StdInc.h"

#include "BoneNode.h"
#include "BoneNodeManager.h"

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
bool BoneNode_c::Init(eBoneTag32 boneTag, RpHAnimBlendInterpFrame* interpFrame) {
    m_BoneTag     = boneTag;
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
    const auto& bone = BoneNodeManager_c::ms_boneInfos[GetIdFromBoneTag(m_BoneTag)];
    m_LimitMin = bone.PoseRots - CVector{
        bone.MinX,
        bone.MinY,
        bone.MinZ
    };
    m_LimitMax = bone.PoseRots + CVector{
        bone.MaxX,
        bone.MaxY,
        bone.MaxZ
    };
}

// degrees → radians → half-angles → quaternion
// Tait-Bryan XYZ convention
// 0x6171F0
void BoneNode_c::EulerToQuat(const CVector& angles, RtQuat& outQuat) {
    const CVector halfRadAngles = {
        DegreesToRadians(angles.x) / 2.f,
        DegreesToRadians(angles.y) / 2.f,
        DegreesToRadians(angles.z) / 2.f
    };
 
    const float cx = std::cos(halfRadAngles.x), sx = std::sin(halfRadAngles.x);
    const float cy = std::cos(halfRadAngles.y), sy = std::sin(halfRadAngles.y);
    const float cz = std::cos(halfRadAngles.z), sz = std::sin(halfRadAngles.z);
 
    // quaternion component products
    const float cc = cx * cz, cs = cx * sz;
    const float sc = sx * cz, ss = sx * sz;
 
    RtQuat q;
    q.imag.x = sc * cy - cs * sy;
    q.imag.y = cc * sy + ss * cy;
    q.imag.z = cs * cy - sc * sy;
    q.real   = cc * cy + ss * sy;
 
    // normalise and store back into quat
    const float magnitude = std::sqrt(q.imag.x * q.imag.x +
                                q.imag.y * q.imag.y +
                                q.imag.z * q.imag.z +
                                q.real   * q.real);
    outQuat.imag.x     = q.imag.x / magnitude;
    outQuat.imag.y     = q.imag.y / magnitude;
    outQuat.imag.z     = q.imag.z / magnitude;
    outQuat.real       = q.real / magnitude;
}

// 0x617080
void BoneNode_c::QuatToEuler(const RtQuat& quat, CVector& outAngles) {
    // original code:
    //const auto v9 = 2.0f * (quat.imag.x * quat.imag.z - quat.imag.y * quat.real);
    //const auto v10 = std::sqrt(1.0f - sq(v9));
    //
    //outAngles.y = RadiansToDegrees(std::atan2(2.0f * (quat.imag.y * quat.real - quat.imag.x * quat.imag.z), v10));
    //if (std::abs(v9) == 1.0f) {
    //    outAngles.x = RadiansToDegrees(std::atan2(-2.0f * (quat.imag.y * quat.imag.z - quat.imag.x * quat.real), 1.0f - 2.0f * (sq(quat.imag.x) + sq(quat.imag.z))));
    //    outAngles.z = RadiansToDegrees(0.0f);
    //} else {
    //    outAngles.x = RadiansToDegrees(std::atan2(2.0f * (quat.imag.x * quat.real + quat.imag.y * quat.imag.z) / v10, (1.0f - 2.0f * (sq(quat.imag.x) + sq(quat.imag.y))) / v10));
    //    outAngles.z = RadiansToDegrees(std::atan2(2.0f * (quat.imag.z * quat.real + quat.imag.x * quat.imag.y) / v10, (1.0f - 2.0f * (sq(quat.imag.y) + sq(quat.imag.z))) / v10));
    //}

    const float x = quat.imag.x;
    const float y = quat.imag.y;
    const float z = quat.imag.z;
    const float w = quat.real;

    const float sinPitch = 2.0f * (x * z - y * w);
    outAngles.y = RadiansToDegrees(std::asin(-sinPitch)); // Simplified from the original atan2 logic
    
    // Code simplified here, originally it was divided by some value, but it didn't change the result, so we can skip that step
    if (approxEqual(sinPitch, 1.f, 0.0001f)) { // Gimbal lock case: Yaw and Roll collapse into a single degree of freedom
        outAngles.x = RadiansToDegrees(std::atan2(-2.0f * (y * z - x * w), 1.0f - 2.0f * (x * x + z * z)));
        outAngles.z = 0.0f;
    } else { // General case:
        outAngles.x = RadiansToDegrees(std::atan2(2.0f * (x * w + y * z), 1.0f - 2.0f * (x * x + y * y)));
        outAngles.z = RadiansToDegrees(std::atan2(2.0f * (z * w + x * y), 1.0f - 2.0f * (y * y + z * z)));
    }
}

// inline
// 0x617050
inline int32 BoneNode_c::GetIdFromBoneTag(eBoneTag32 tag) {
    for (auto&& [i, info] : rngv::enumerate(BoneNodeManager_c::ms_boneInfos)) {
        if (info.BoneTag == tag) {
            return (eBoneTag)(i);
        }
    }
    return BONE_UNKNOWN;
}

// Empty in Android
// 0x6175D0
void BoneNode_c::ClampLimitsCurrent(bool limitX, bool limitY, bool limitZ) {
    static auto& s_TestSkipClampCurrent = StaticRef<bool>(0x8D2BD1); // true

    if (!s_TestSkipClampCurrent) {
        CVector current;
        QuatToEuler(m_Orientation, current);
        if (limitX) {
            m_LimitMin.x = m_LimitMax.x = current.x;
        }
        if (limitY) {
            m_LimitMin.y = m_LimitMax.y = current.y;
        }
        if (limitZ) {
            m_LimitMin.z = m_LimitMax.z = current.z;
        }
    }
}

// Empty in Android
// 0x617530
void BoneNode_c::ClampLimitsDefault(bool limitX, bool limitY, bool limitZ) {
    static auto& s_TestSkipClampDefault = StaticRef<bool>(0x8D2BD0); // true

    if (!s_TestSkipClampDefault) {
        if (const auto* info = GetBoneInfo()) { // BUGFIX: Check if info is available
            if (limitX) {
                m_LimitMin.x = m_LimitMax.x = info->PoseRots.x;
            }
            if (limitY) {
                m_LimitMin.y = m_LimitMax.y = info->PoseRots.y;
            }
            if (limitZ) {
                m_LimitMin.z = m_LimitMax.z = info->PoseRots.z;
            }
        }
    }
}

// argument (float blend) - ignored
// 0x617650
void BoneNode_c::Limit(float blend) {
    CVector angles{};
    QuatToEuler(m_Orientation, angles);

    angles.x = std::clamp(angles.x, m_LimitMin.x, m_LimitMax.x);
    angles.y = std::clamp(angles.y, m_LimitMin.y, m_LimitMax.y);

    float minZ = m_LimitMin.z;
    float maxZ = m_LimitMax.z;

    if (m_BoneTag == eBoneTag::BONE_HEAD) {
        if (const auto* info = GetBoneInfo()) { // BUGFIX: Check if info is available
            float pose = info->PoseRots.z;

            // Scale the Z range toward boneOffset as angleX approaches 45°
            const float factor = std::max(std::abs(angles.x) / -45.0f + 1.0f, 0.0f);
            minZ = (minZ - pose) * factor + pose;
            maxZ = (maxZ - pose) * factor + pose;
        }
    }

    angles.z = std::clamp(angles.z, minZ, maxZ);

    EulerToQuat(angles, m_Orientation);
}

// 0x616E30
void BoneNode_c::BlendKeyframe(float blend) {
    auto src = m_InterpFrame->q;
    auto dst = m_Orientation;
    RtQuatSlerpCache cache;
    RtQuatSetupSlerpCache(&src, &dst, &cache);
    RtQuatSlerp(&m_InterpFrame->q, &src, &dst, blend, &cache);
}

// notsa
const BoneInfo_t* BoneNode_c::GetBoneInfo() {
    return BoneNodeManager_c::GetBoneInfoFromTag(m_BoneTag);
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
    m_LimitMin[+axis] = min;
    m_LimitMax[+axis] = max;
}

// 0x616BF0
void BoneNode_c::GetLimits(eRotationAxis axis, float& outMin, float& outMax) const {
    outMin = m_LimitMin[+axis];
    outMax = m_LimitMax[+axis];
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
        mat.SetRotate(CQuaternion{ m_Orientation });
        mat.GetPosition() = m_Pos;
        return mat.ToRwMatrix();
    }();

    rwMatrixSetFlags(&rotMatrix, rwMATRIXTYPEORTHONORMAL);
    RwMatrixMultiply(&m_WorldMat, &rotMatrix, boneMatrix);

    for (auto bone = m_Childs.GetHead(); bone; bone = m_Childs.GetNext(bone)) {
        bone->CalcWldMat(&m_WorldMat);
    }
}
