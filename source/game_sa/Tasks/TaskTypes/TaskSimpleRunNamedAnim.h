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
        uint32 endTime, bool dontInterrupt, bool runInSequence, bool offsetPed, bool holdLastFrame);
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
    char                 m_AnimName[24]{};
    char                 m_AnimGroupName[16]{};
    float                m_BlendDelta{};
    CAnimBlendHierarchy* m_AnimHierarchy{};
    uint32               m_Time{};
    CTaskTimer           m_Timer{};
    CVector              m_OffsetAtEnd{};
    uint32               m_AnimFlags{};
    int16                m_AnimId{};
};
VALIDATE_SIZE(CTaskSimpleRunNamedAnim, 0x64);
