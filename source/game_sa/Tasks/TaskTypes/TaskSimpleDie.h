#pragma once

#include "TaskSimple.h"

class CAnimBlendHierarchy;
class CAnimBlendAssociation;

enum class SimpleDieFlags : uint8 {
    INITIAL           = 0x0000,
    DIE_ANIM_FINISHED = 1 << 0,
    ALREADY_DEAD      = 1 << 1,
};

class NOTSA_EXPORT_VTABLE CTaskSimpleDie : public CTaskSimple {
public:
    AssocGroupId           m_animGroupId;
    AnimationId            m_animId;
    CAnimBlendHierarchy*   m_animHierarchy;
    eAnimationFlags        m_animFlags;
    float                  m_blendDelta;
    float                  m_animSpeed;
    union {
        struct {
            bool m_dieAnimFinished : 1;
            bool m_alreadyDead : 1;
        };
        uint8 m_nFlags;
    };
    CAnimBlendAssociation* m_animAssociation;

public:
    static constexpr auto Type = TASK_SIMPLE_DIE;

    CTaskSimpleDie(AssocGroupId animGroupId, AnimationId animId, float blendDelta, float animSpeed);
    CTaskSimpleDie(const char* animName, const char* animBlock, eAnimationFlags animFlags, float blendDelta, float animSpeed);
    CTaskSimpleDie(CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags, float blendDelta, float animSpeed);
    ~CTaskSimpleDie() override;

    eTaskType GetTaskType() const override { return Type; } // 0x62FA50
    CTask*    Clone() const override;
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;

    void        StartAnim(CPed* ped);
    static void FinishAnimDieCB(CAnimBlendAssociation* association, void* data);

    CTaskSimpleDie* Constructor(AssocGroupId animGroupId, AnimationId animId, float blendDelta, float animSpeed);
    CTaskSimpleDie* Constructor(const char* animName, const char* animBlock, eAnimationFlags animFlags, float blendDelta, float animSpeed);
    CTaskSimpleDie* Constructor(CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags, float blendDelta, float animSpeed);
    CTaskSimpleDie* Destructor();

};
VALIDATE_SIZE(CTaskSimpleDie, 0x28);
extern void CTaskSimpleDie__InjectHooks();
