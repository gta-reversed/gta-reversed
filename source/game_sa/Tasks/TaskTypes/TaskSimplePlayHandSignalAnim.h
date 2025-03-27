#pragma once

#include "TaskSimpleAnim.h"
class CHandObject;

class NOTSA_EXPORT_VTABLE CTaskSimplePlayHandSignalAnim : public CTaskSimpleAnim {
public:
    static constexpr auto Type = TASK_SIMPLE_HANDSIGNAL_ANIM;

    CTaskSimplePlayHandSignalAnim(AnimationId animationId, float fBlendFactor, bool bFatHands, bool bHoldLastFrame);
    ~CTaskSimplePlayHandSignalAnim() override;

    eTaskType GetTaskType() const override { return Type; } // 0x61AEA0;
    CTask* Clone() const override { return new CTaskSimplePlayHandSignalAnim(m_PlayerIdlesAnimBlockId, m_AnimBlenDelta, m_DoUseFatHands, m_bHoldLastFrame); }
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return CTaskSimpleAnim::MakeAbortable(ped, priority, event); }
    bool ProcessPed(CPed* ped) override;

    void StartAnim(CPed* ped);

    static void InjectHooks();

private:
    AnimationId  m_PlayerIdlesAnimBlockId{};
    float        m_AnimBlenDelta{};
    bool         m_DoUseFatHands{};
    CHandObject* m_LeftHand{};
    CHandObject* m_RightHand{};
};
VALIDATE_SIZE(CTaskSimplePlayHandSignalAnim, 0x24);
