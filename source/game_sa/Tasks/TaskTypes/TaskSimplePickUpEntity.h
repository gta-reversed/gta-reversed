#pragma once

#include "TaskSimpleHoldEntity.h"

class NOTSA_EXPORT_VTABLE CTaskSimplePickUpEntity : public CTaskSimpleHoldEntity {
public:
    float m_fMovePedUntilAnimProgress; //< value can be 0.0 to 1.0. Ped Will stop moving when the current `m_fCurrentTime`
                                       //< of `m_pAnimBlendAssociation` is equal or greater than this.
    CVector m_vecPickuposn;

public:
    static constexpr auto Type = TASK_SIMPLE_PICKUP_ENTITY;

    CTaskSimplePickUpEntity(CEntity* entityToHold, const CVector* posn, uint8 boneFrameId, uint8 boneFlags, CAnimBlock* animBlock, CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags, float fMovePedUntilAnimProgress);
    CTaskSimplePickUpEntity(CEntity* entityToHold, const CVector* posn, uint8 boneFrameId, uint8 boneFlags, AnimationId animId, AssocGroupId groupId, float fMovePedUntilAnimProgress);
    ~CTaskSimplePickUpEntity() override = default;

    CTask* Clone() const override;
    eTaskType GetTaskType() const override { return TASK_SIMPLE_PICKUP_ENTITY; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

private: // Wrappers for hooks
    // 0x691870
    CTaskSimplePickUpEntity* Constructor(CEntity* entityToHold, CVector* posn, uint8 boneFrameId, uint8 boneFlags, CAnimBlock* pAnimBlock, CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags, float fMovePedUntilAnimProgress) {
        this->CTaskSimplePickUpEntity::CTaskSimplePickUpEntity(entityToHold, posn, boneFrameId, boneFlags, pAnimBlock, animHierarchy, animFlags, fMovePedUntilAnimProgress);
        return this;
    }

    // 0x6917B0
    CTaskSimplePickUpEntity* Constructor(CEntity* entityToHold, CVector* posn, uint8 boneFrameId, uint8 boneFlags, AnimationId animId, AssocGroupId groupId, float fMovePedUntilAnimProgress){
        this->CTaskSimplePickUpEntity::CTaskSimplePickUpEntity(entityToHold, posn, boneFrameId, boneFlags, animId, groupId, fMovePedUntilAnimProgress);
        return this;
    }

};

VALIDATE_SIZE(CTaskSimplePickUpEntity, 0x4C);
