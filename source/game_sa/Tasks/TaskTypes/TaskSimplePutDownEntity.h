#pragma once

#include "TaskSimpleHoldEntity.h"

class NOTSA_EXPORT_VTABLE CTaskSimplePutDownEntity : public CTaskSimpleHoldEntity {
public:
    float m_fPutDownHeightZ;

public:
    CTaskSimplePutDownEntity();
    CTaskSimplePutDownEntity(CEntity* entityToHold, const CVector* posn, uint8 boneFrameId, uint8 boneFlags, AnimationId animId, AssocGroupId groupId, bool bDisAllowDroppingOnAnimEnd, float fPutDownHeightZ);
    CTaskSimplePutDownEntity(CEntity* entityToHold, const CVector* posn, uint8 boneFrameId, uint8 boneFlags, const char* animName, const char* animBlockName, eAnimationFlags animFlags, float fPutDownHeightZ);
    CTaskSimplePutDownEntity(CEntity* entityToHold, const CVector* posn, uint8 boneFrameId, uint8 boneFlags, CAnimBlock* pAnimBlock, CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags, float fPutDownHeightZ);
    ~CTaskSimplePutDownEntity() override = default;

public:
    static constexpr auto Type = TASK_SIMPLE_PUTDOWN_ENTITY;

    CTask*    Clone() const override;
    eTaskType GetTaskType() const override { return TASK_SIMPLE_PUTDOWN_ENTITY; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

private: // Wrappers for hooks
    // 0x691990
    CTaskSimplePutDownEntity* Constructor() {
        this->CTaskSimplePutDownEntity::CTaskSimplePutDownEntity();
        return this;
    }
};

VALIDATE_SIZE(CTaskSimplePutDownEntity, 0x40);
