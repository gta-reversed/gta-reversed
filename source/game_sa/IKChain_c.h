#pragma once

#include "Base.h"
#include <rwplcore.h>
#include <span>

#include <extensions/WEnum.hpp>
#include "ListItem_c.h"
#include "BoneNode_c.h"
#include "Vector.h"
#include "Enums/eIKChainSlot.h"

class CEntity;
class CPed;
class AnimBlendFrameData;


class IKChain_c : public ListItem_c<IKChain_c> {
public:
    static void InjectHooks();

    IKChain_c() = default;
    ~IKChain_c() = default;

    bool Init(const char* name, eIKChainSlot slot, CPed* ped, eBoneTag32 effectorBone, RwV3d effectorPos, eBoneTag32 pivotBone, CEntity* entity, eBoneTag32 offsetBone, RwV3d offsetPos, float speed, int8 priority);
    void Exit();
    void Update(float timeStep);
    bool IsAtTarget(float maxDist, float* outDist) const;
    bool IsFacingTarget() const;
    void UpdateTarget(bool target);
    void UpdateOffset(eBoneTag32 offsetBoneTag, CVector offsetPosn);
    void ClampLimits(eBoneTag32 boneTag, bool LimitX, bool LimitY, bool LimitZ, bool UseCurrentLimits);
    void UpdateEntity(CEntity* entity);
    BoneNode_c* GetBoneNodeFromTag(eBoneTag32 tag);
    int8 GetPriority() const;
    void SetOffsetPos(CVector value);
    void SetOffsetBoneTag(eBoneTag32 value);
    void SetBlend(float value);
    void MoveBonesToTarget();
    void SetupBones(eBoneTag32 boneTag, CVector posn, eBoneTag32 bone, AnimBlendFrameData* frames);
    void GetLimits(eBoneTag32 boneTag, eRotationAxis axis, float& outMin, float& outMax);

    auto GetBones() const { return std::span{ m_Bones, (size_t)m_BonesCount }; }
    auto GetBones()       { return std::span{ m_Bones, (size_t)m_BonesCount }; }

    auto GetIKSlot() const { return m_IKSlot; }
    CPed* GetPed() const { return m_Ped; }

private:
    CPed::Ref                    m_Ped{};
    int32                        m_BonesCount{};
    BoneNode_c**                 m_Bones{}; //!< Array, size of `m_BonesCount`. Use `GetBones()`.
    RwMatrix*                    m_PivotBoneMatrix{};
    float                        m_Blend{};
    eBoneTag16                   m_EffectorBone{};
    RwV3d                        m_EffectorPos{};
    eBoneTag16                   m_PivotBone{};
    notsa::EntityRef<>           m_TargetEntity{};
    eBoneTag32                   m_OffsetBone{};   //!< Offset bone
    CVector                      m_OffsetPos{};    //!< - If `m_TargetEntity` is set:
                                                   //!<   - And `m_OffsetBone` is set too: This is a position relative to that bone
                                                   //!<   - Otherwise this is an object-space value, otherwise it's a world-space value.
                                                   //!< - Otherwise:
                                                   //!    - It's a word-space offset
    float                        m_Speed{};        //!< IK animation speed
    CVector                      m_OffsetPosWS{};  //!< World-space offset (Calculated from `m_OffsetPos`, see `MoveBonesToTarget`)
    bool                         m_UpdateTarget{}; //!< Whenever the target was updated, and we need to re-calculate stuff related to them
    notsa::WEnumS8<eIKChainSlot> m_IKSlot{};       //!< The IK slot we're in
    int8                         m_Priority{};
};
VALIDATE_SIZE(IKChain_c, 0x58);
