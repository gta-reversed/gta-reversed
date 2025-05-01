#pragma once

#include "Base.h"

#include "Enums/eBoneTag.h"
#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "List_c.h"
#include "ListItem_c.h"

#include "RpHAnimBlendInterpFrame.h"
#include <rwplcore.h>
#include <rtquat.h>

class BoneNode_c : public ListItem_c<BoneNode_c> {
public:
    static void InjectHooks();

    BoneNode_c() = default;  // 0x616B30
    ~BoneNode_c() = default; // 0x616B80

    bool Init(int32 boneTag, RpHAnimBlendInterpFrame* interpFrame);
    void InitLimits();

    static void EulerToQuat(const CVector& angles, RtQuat& quat);
    static void QuatToEuler(const RtQuat& quat, CVector& angles);
    static int32  GetIdFromBoneTag(eBoneTag32 bone);

    void ClampLimitsCurrent(bool LimitX, bool LimitY, bool LimitZ);
    void ClampLimitsDefault(bool LimitX, bool LimitY, bool LimitZ);

    void Limit(float blend);

    void BlendKeyframe(float blend);

    float GetSpeed() const;
    void SetSpeed(float);

    void SetLimits(eRotationAxis axis, float min, float max);
    void GetLimits(eRotationAxis axis, float& min, float& max) const;

    void AddChild(BoneNode_c * children);
    void CalcWldMat(const RwMatrix* boneMatrix);

    const auto& GetPosition() const { return *RwMatrixGetPos(&m_WorldMat); }
    const auto& GetMatrix() const { return m_WorldMat;  }
    auto GetBoneTag() const { return m_BoneTag; }
    auto GetParent() const { return m_Parent; }
    auto& GetOrientation() { return m_Orientation; }
private:
    BoneNode_c* Constructor() { this->BoneNode_c::BoneNode_c(); return this; }
    BoneNode_c* Destructor() { this->BoneNode_c::~BoneNode_c(); return this; }

private:
    eBoneTag                 m_BoneTag;
    RpHAnimBlendInterpFrame* m_InterpFrame;
    RtQuat                   m_Orientation; 
    CVector                  m_Pos;
    BoneNode_c*              m_Parent;
    TList_c<BoneNode_c>      m_Childs;
    RwMatrix                 m_WorldMat;
    CVector                  m_LimitMin;
    CVector                  m_LimitMax;
    float                    m_Speed;
};
VALIDATE_SIZE(BoneNode_c, 0x98);
