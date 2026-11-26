/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once


#include "TaskComplex.h"

class NOTSA_EXPORT_VTABLE CTaskComplexPlayHandSignalAnim : public CTaskComplex {
public:
    static inline auto& ms_animBlock = StaticRef<uint32>(0x8D2E38);

public:
    static constexpr auto Type = TASK_COMPLEX_HANDSIGNAL_ANIM;

    CTaskComplexPlayHandSignalAnim(AnimationId animationId = ANIM_ID_UNDEFINED, float blendFactor = 4.f);
    CTaskComplexPlayHandSignalAnim(const CTaskComplexPlayHandSignalAnim&);
    ~CTaskComplexPlayHandSignalAnim();

    CTask*    CreateSubTask(eTaskType taskType);

    CTask*    Clone() const override { return new CTaskComplexPlayHandSignalAnim{ *this }; } // 0x61BA00
    eTaskType GetTaskType() const override { return Type; } // 0x61B2E0
    CTask*    CreateNextSubTask(CPed* ped) override { return nullptr; } // 0x61B570
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private:
    static AnimationId GetAnimIdForPed(CPed* ped);

public:
    static void InjectHooks();

private:
    AnimationId m_AnimationId{};
    float       m_AnimBlenDelta{};
    bool        m_IsLeftHandLoaded : 1{};
    bool        m_IsRightHandLoaded : 1{};
    bool        m_IsAnimationLoaded : 1{};
    bool        m_UseFatHands : 1{};
};
VALIDATE_SIZE(CTaskComplexPlayHandSignalAnim, 0x18);
