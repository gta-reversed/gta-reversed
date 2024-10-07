#pragma once

#include "TaskSimple.h"

enum eHoldEntityBoneFlags {
    HOLD_ENTITY_FLAG_1 = 0x1,
    HOLD_ENTITY_UPDATE_TRANSLATION_ONLY = 0x10
};

class CEntity;
class CAnimBlock;
class CAnimBlendHierarchy;
class CAnimBlendAssociation;

class NOTSA_EXPORT_VTABLE CTaskSimpleHoldEntity : public CTaskSimple {
public:
    CEntity*               m_pEntityToHold;
    CVector                m_vecPosition;
    uint8                  m_nBoneFrameId; // see ePedNode
    uint8                  m_bBoneFlags;   // See eHoldEntityBoneFlags
    bool                   field_1A[2];
    float                  m_fRotation;
    AnimationId            m_nAnimId;
    AssocGroupId           m_nAnimGroupId;
    int32                  m_animFlags; // see eAnimationFlags
    CAnimBlock*            m_pAnimBlock;
    CAnimBlendHierarchy*   m_pAnimBlendHierarchy; // If set, m_animID and m_groupID are ignored in StartAnim method
    bool                   m_bEntityDropped;
    bool                   m_bEntityRequiresProcessing;
    bool                   m_bDisallowDroppingOnAnimEnd;
    bool                   field_37;
    CAnimBlendAssociation* m_pAnimBlendAssociation;

public:
    static constexpr auto Type = TASK_SIMPLE_HOLD_ENTITY;

    CTaskSimpleHoldEntity(
        CEntity* entityToHold,
        const CVector* posn,
        uint8 boneFrameId,
        uint8 boneFlags = HOLD_ENTITY_UPDATE_TRANSLATION_ONLY,
        AnimationId animId = AnimationId::ANIM_ID_NO_ANIMATION_SET,
        AssocGroupId groupId = AssocGroupId::ANIM_GROUP_DEFAULT,
        bool bDisAllowDroppingOnAnimEnd = true
    );
    CTaskSimpleHoldEntity(  // NOTSA
        CEntity* entityToHold,
        const CVector& posn,
        uint8 boneFrameId,
        uint8 boneFlags = HOLD_ENTITY_UPDATE_TRANSLATION_ONLY,
        AnimationId animId = AnimationId::ANIM_ID_NO_ANIMATION_SET,
        AssocGroupId groupId = AssocGroupId::ANIM_GROUP_DEFAULT,
        bool bDisAllowDroppingOnAnimEnd = true
    );
    CTaskSimpleHoldEntity(CEntity* entityToHold, const CVector* posn, uint8 boneFrameId, uint8 boneFlags, const char* animName, const char* animBlockName, eAnimationFlags animFlags);
    CTaskSimpleHoldEntity(CEntity* entityToHold, const CVector* posn, uint8 boneFrameId, uint8 boneFlags, CAnimBlock* animBlock, CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags);
    ~CTaskSimpleHoldEntity() override;

    eTaskType GetTaskType() const override { return Type; }; // 0x691460
    CTask* Clone() const override;
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;
    bool SetPedPosition(CPed* ped) override;

    void ReleaseEntity();
    bool CanThrowEntity() const;
    void PlayAnim(AnimationId animId, AssocGroupId grpId);
    static void FinishAnimHoldEntityCB(CAnimBlendAssociation* pAnimAssoc, void* data);
    void StartAnim(CPed* ped);
    void DropEntity(CPed* ped, bool bAddEventSoundQuiet);
    void ChoosePutDownHeight(CPed* ped);
    auto GetHeldEntity() const { return m_pEntityToHold; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimpleHoldEntity* Constructor(CEntity* entityToHold, CVector* posn, uint8 boneFrameId, uint8 boneFlags, AnimationId animId, AssocGroupId groupId, bool bDisAllowDroppingOnAnimEnd);
    CTaskSimpleHoldEntity* Constructor(CEntity* entityToHold, CVector* posn, uint8 boneFrameId, uint8 boneFlags, const char* animName, const char* animBlockName, eAnimationFlags animFlags);
    CTaskSimpleHoldEntity* Constructor(CEntity* entityToHold, CVector* posn, uint8 boneFrameId, uint8 boneFlags, CAnimBlock* animBlock, CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags);

};

VALIDATE_SIZE(CTaskSimpleHoldEntity, 0x3C);

