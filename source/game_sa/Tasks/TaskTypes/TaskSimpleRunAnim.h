/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimpleAnim.h"
#include "AnimBlendAssociation.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleRunAnim : public CTaskSimpleAnim {
public:
    AssocGroupId m_nAnimGroup;
    AnimationId  m_nAnimId;
    float        m_fBlendDelta;
    uint16       m_nTaskType;

public:
    static constexpr auto Type = TASK_SIMPLE_NAMED_ANIM;

    CTaskSimpleRunAnim(AssocGroupId animGroup, AnimationId animId, float fBlendDelta = 4.f, bool bHoldLastFrame = false);
    CTaskSimpleRunAnim(AssocGroupId animGroup, AnimationId animId, float fBlendDelta, int32 nTaskType, const char* taskName _IGNORED_, bool bHoldLastFrame);

    CTask* Clone() const override;
    eTaskType GetTaskType() const override { return static_cast<eTaskType>(m_nTaskType); }
    bool ProcessPed(CPed* ped) override;

    void StartAnim(CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimpleRunAnim* Constructor(AssocGroupId animGroup, AnimationId animId, float fBlendDelta, bool bHoldLastFrame);
    CTaskSimpleRunAnim* Constructor2(AssocGroupId animGroup, AnimationId animId, float fBlendDelta, int32 nTaskType, const char* taskName _IGNORED_, bool bHoldLastFrame);

};

VALIDATE_SIZE(CTaskSimpleRunAnim, 0x20);
