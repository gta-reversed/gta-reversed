#pragma once

#include "TaskSimple.h"
class CAccident;

class NOTSA_EXPORT_VTABLE CTaskSimpleGiveCPR : public CTaskSimple {
public:
    union {
        struct {
            uint32 bNoHealthValue : 1;
            uint32 bAnimationFinished : 1;
        };
        uint32 m_nFlags;
    };
    CAccident* m_pAccident;
    CAnimBlendAssociation* m_pAnim;
    float m_fInitialHealth;

public:
    static constexpr auto Type = TASK_SIMPLE_GIVE_CPR;

    explicit CTaskSimpleGiveCPR(CAccident* accident);
    ~CTaskSimpleGiveCPR() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override;
    bool ProcessPed(CPed* ped) override;
    bool MakeAbortable(class CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;

    void ReviveDeadPed(CPed* ped);
    static void FinishGiveCPRAnimCB(CAnimBlendAssociation* anim, void* ptask);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimpleGiveCPR* Constructor(CAccident* pAccident);

};
VALIDATE_SIZE(CTaskSimpleGiveCPR, 0x18);
