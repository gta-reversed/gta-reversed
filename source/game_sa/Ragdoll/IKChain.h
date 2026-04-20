#pragma once

#include "Base.h"
#include <rwplcore.h>
#include <span>

#include <extensions/WEnum.hpp>
#include "ListItem_c.h"
#include "BoneNode.h"

#include "Enums/eIKChainSlot.h"

class CEntity;
class CPed;
class AnimBlendFrameData;


class IKChain_c : public ListItem_c<IKChain_c> {
public:
    IKChain_c() = default;
    ~IKChain_c() = default;

    bool Init(const char* name, eIKChainSlot ikSlot, CPed* ped, eBoneTag32 effectorBone, RwV3d effectorPos, eBoneTag32 pivotBone, CEntity* entity, eBoneTag32 offsetBone, RwV3d offsetPos, float speed, int32 priority);
    void Exit();

    void Update(float deltaTime);

    void SetBlend(float blend);
    void SetOffsetBoneTag(eBoneTag32 offsetBone);
    void SetOffsetPos(RwV3d offsetPos);

    void ClampLimits(eBoneTag32 boneTag, bool axisX, bool axisY, bool axisZ, bool current);
    void GetLimits(eBoneTag32 boneTag, eRotationAxis axis, float& min, float& max);
    void SetLimits(eBoneTag32 boneTag, eRotationAxis axis, float min, float max);

    float GetBoneSpeed(eBoneTag32 boneTag);
    void SetBoneSpeed(eBoneTag32 boneTag, float speed);

    int8 GetPriority() const;

    void UpdateEntity(CEntity* entity);
    void UpdateOffset(eBoneTag32 offsetBone, RwV3d offsetPos);
    void UpdateTarget(bool update);

    bool IsFacingTarget() const;
    bool IsAtTarget(float range, float* dist) const;
    CPed* GetParentPed() const { return m_Ped; }

    // notsa section

    auto GetBones() const { return std::span{ m_Bones, (size_t)m_BonesCount }; }
    auto GetBones()       { return std::span{ m_Bones, (size_t)m_BonesCount }; }

    auto GetIKSlot() const { return m_IKSlot; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    void SetupBones(eBoneTag32 effectorBone, RwV3d effectorPos, eBoneTag32 pivotBone, AnimBlendFrameData* frames);
    void MoveBonesToTarget();

    BoneNode_c* GetBoneNodeFromTag(eBoneTag32 boneTag);

private:
    // static inline auto& ms_boneInfos = StaticRef<std::array<tBoneInfo, MAX_BONE_NUM>>(0x0); // unused

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
    RwV3d                        m_OffsetPos{};    //!< - If `m_TargetEntity` is set:
                                                   //!<   - And `m_OffsetBone` is set too: This is a position relative to that bone
                                                   //!<   - Otherwise this is an object-space value, otherwise it's a world-space value.
                                                   //!< - Otherwise:
                                                   //!    - It's a word-space offset
    float                        m_Speed{};        //!< IK animation speed

    RwV3d                        m_OffsetPosWS{};  //!< World-space offset (Calculated from `m_OffsetPos`, see `MoveBonesToTarget`)
    bool                         m_UpdateTarget{}; //!< Whenever the target was updated, and we need to re-calculate stuff related to them
    notsa::WEnumS8<eIKChainSlot> m_IKSlot{};       //!< The IK slot we're in
    int8                         m_Priority{};
};
VALIDATE_SIZE(IKChain_c, 0x58);
