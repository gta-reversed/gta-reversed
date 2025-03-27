/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once


#include "TaskComplex.h"

//typedef int32 AnimationId;

class NOTSA_EXPORT_VTABLE CTaskComplexPlayHandSignalAnim : public CTaskComplex {
public:
    static inline uint32& ms_animBlock = *(uint32*)0x8D2E38;

public:
    AnimationId m_animationId{};
    float       m_AnimBlenDelta{};
    bool        m_bLeftHandLoaded : 1{};
    bool        m_bRightHandLoaded : 1{};
    bool        m_bAnimationLoaded : 1{};
    bool        m_DoUseFatHands : 1{};

public:
    static constexpr auto Type = TASK_COMPLEX_HANDSIGNAL_ANIM;

    static void InjectHooks();

    explicit CTaskComplexPlayHandSignalAnim(AnimationId animationId = ANIM_ID_UNDEFINED, float blendFactor = 4.f);
    CTaskComplexPlayHandSignalAnim(const CTaskComplexPlayHandSignalAnim&);
    ~CTaskComplexPlayHandSignalAnim();

    static AnimationId GetAnimIdForPed(CPed* ped);

    CTask* CreateSubTask(eTaskType taskType);

    CTask*    Clone() const override;
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;
private:
    // 0x61B2B0
    CTaskComplexPlayHandSignalAnim* Constructor(AnimationId animationId, float blendFactor) {
        this->CTaskComplexPlayHandSignalAnim::CTaskComplexPlayHandSignalAnim(animationId, blendFactor);
        return this;
    }

    // 0x61BDF0
    CTaskComplexPlayHandSignalAnim* Destructor() {
        this->CTaskComplexPlayHandSignalAnim::~CTaskComplexPlayHandSignalAnim();
        return this;
    }
};

VALIDATE_SIZE(CTaskComplexPlayHandSignalAnim, 0x18);
