#pragma once

#include "TaskSimple.h"
class CAnimBlendAssociation;

class NOTSA_EXPORT_VTABLE CTaskSimpleFall : public CTaskSimple {
public:
    bool                   m_bIsFinished;
    AnimationId            m_nAnimId;
    AssocGroupId           m_nAnimGroup;
    CAnimBlendAssociation* m_pAnim;
    int32                  m_nTotalDownTime; // TODO: uint32?
    uint32                 m_nCurrentDownTime;

    static uint32& m_nMaxPlayerDownTime;

public:
    static constexpr auto Type = TASK_SIMPLE_FALL;

    CTaskSimpleFall(AnimationId nAnimId, AssocGroupId nAnimGroup, int32 nDownTime);
    ~CTaskSimpleFall() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskSimpleFall(m_nAnimId, m_nAnimGroup, m_nTotalDownTime); }
    bool ProcessPed(CPed* ped) override;
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;


    bool StartAnim(CPed* ped);
    void ProcessFall(CPed* ped);
    static void FinishFallAnimCB(CAnimBlendAssociation* anim, void* data); // data is CTaskSimpleFall

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimpleFall* Constructor(AnimationId nAnimId, AssocGroupId nAnimGroup, int32 nDownTime);
};
VALIDATE_SIZE(CTaskSimpleFall, 0x20);
