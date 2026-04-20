#pragma once

#include "Base.h"

#include "Enums/eBoneTag.h"
#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "List_c.h"
#include "ListItem_c.h"
#include "Vehicle.h"

#include "RpHAnimBlendInterpFrame.h"
#include <rwplcore.h>
#include <rtquat.h>

class BoneNode_c : public ListItem_c<BoneNode_c> {
public:
    BoneNode_c() = default;  // 0x616B30
    ~BoneNode_c() = default; // 0x616B80

    bool Init(eBoneTag32 boneTag, RpHAnimBlendInterpFrame* interpFrame);

    void AddChild(BoneNode_c* children);

    void InitLimits();
    void ClampLimitsCurrent(bool limitX, bool limitY, bool limitZ);
    void ClampLimitsDefault(bool limitX, bool limitY, bool limitZ);
    void SetLimits(eRotationAxis axis, float min, float max);
    void GetLimits(eRotationAxis axis, float& min, float& max) const;
    void Limit(float blend);

    float GetSpeed() const;
    void SetSpeed(float speed);

    void CalcWldMat(const RwMatrix* boneMatrix);
    void BlendKeyframe(float blend);

private:
    static void QuatToEuler(const RtQuat& quat, RwV3d& angles);
    static void EulerToQuat(const RwV3d& angles, RtQuat& quat);

    static eBoneTag32 GetIdFromBoneTag(eBoneTag32 bone);

    // notsa section

    friend void InjectHooksMain();
    static void InjectHooks();

    friend class IKChain_c;

public:
    const auto& GetPosition() const { return *RwMatrixGetPos(&m_WorldMat); }
    const auto& GetMatrix() const { return m_WorldMat;  }
    auto GetBoneTag() const { return m_BoneTag; }
    auto GetParent() const { return m_Parent; }
    auto& GetOrientation() { return m_Orientation; }

    BoneNode_c* Constructor() { this->BoneNode_c::BoneNode_c(); return this; }
    BoneNode_c* Destructor() { this->BoneNode_c::~BoneNode_c(); return this; }

private:
    eBoneTag32               m_BoneTag;
    RpHAnimBlendInterpFrame* m_InterpFrame;
    RtQuat                   m_Orientation; 
    RwV3d                    m_Pos;

    BoneNode_c*              m_Parent;
    TList_c<BoneNode_c>      m_Childs;

    RwMatrix                 m_WorldMat;

    CVector                  m_LimitMin; // RwV3d
    CVector                  m_LimitMax; // RwV3d

    float                    m_Speed;
};
VALIDATE_SIZE(BoneNode_c, 0x98);
