/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimpleAnim.h"
#include "AnimBlendHierarchy.h"
#include "TaskTimer.h"
#include "Vector.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleRunNamedAnim : public CTaskSimpleAnim {
public:
    static constexpr auto Type = TASK_SIMPLE_NAMED_ANIM;

    CTaskSimpleRunNamedAnim();
    CTaskSimpleRunNamedAnim(const char* animName, const char* animGroupName, uint32 animFlags, float blendDelta,
        uint32 endTime, bool bDontInterrupt, bool bRunInSequence, bool bOffsetPed, bool bHoldLastFrame);
    CTaskSimpleRunNamedAnim(const CTaskSimpleRunNamedAnim&);

    CTask*    Clone() const override;
    eTaskType GetTaskType() const override { return Type; }
    bool      ProcessPed(CPed* ped) override;

    void StartAnim(CPed* ped);
    void OffsetPedPosition(CPed* ped);

private:
    static void FinishRunAnimMovePedCB(CAnimBlendAssociation* anim, void* data);

public:
    static void InjectHooks();

protected:
    char                 m_animName[24]{};
    char                 m_animGroupName[16]{};
    float                m_fBlendDelta{};
    CAnimBlendHierarchy* m_pAnimHierarchy{};
    uint32               m_Time{};
    CTaskTimer           m_Timer{};
    CVector              m_vecOffsetAtEnd{};
    uint32               m_animFlags{};
    int16                m_nAnimId{};
};
VALIDATE_SIZE(CTaskSimpleRunNamedAnim, 0x64);
