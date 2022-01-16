#include "TaskSimple.h"

class CAnimBlendHierarchy;
class CAnimBlendAssociation;

enum class SimpleDieFlags : uint8 {
    INITIAL           = 0x0000,
    DIE_ANIM_FINISHED = 1 << 0,
    ALREADY_DEAD      = 1 << 1,
};

class CTaskSimpleDie : public CTaskSimple {
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
    CTaskSimpleDie(AssocGroupId animGroupId, AnimationId animId, float blendDelta, float animSpeed);
    CTaskSimpleDie(const char* animName, const char* animBlock, eAnimationFlags animFlags, float blendDelta, float animSpeed);
    CTaskSimpleDie(CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags, float blendDelta, float animSpeed);
    ~CTaskSimpleDie() override;

    CTask*    Clone() override;
    eTaskType GetTaskType() override { return TASK_SIMPLE_DIE; } // 0x62FA50
    bool      MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    bool      ProcessPed(CPed* ped) override;

    void        StartAnim(CPed* ped);
    static void FinishAnimDieCB(CAnimBlendAssociation* association, void* data);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimpleDie* Constructor(AssocGroupId animGroupId, AnimationId animId, float blendDelta, float animSpeed);
    CTaskSimpleDie* Constructor(const char* animName, const char* animBlock, eAnimationFlags animFlags, float blendDelta, float animSpeed);
    CTaskSimpleDie* Constructor(CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags, float blendDelta, float animSpeed);
    CTaskSimpleDie* Destructor();

    CTask*    Clone_Reversed();
    eTaskType GetTaskType_Reversed();
    bool      MakeAbortable_Reversed(CPed* ped, eAbortPriority priority, const CEvent* event);
    bool      ProcessPed_Reversed(CPed* ped);
};

VALIDATE_SIZE(CTaskSimpleDie, 0x28);
