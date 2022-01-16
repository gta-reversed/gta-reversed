#pragma once

#include "TaskSimple.h"
#include "Accident.h"

class CTaskSimpleGiveCPR : public CTaskSimple {
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
    CTaskSimpleGiveCPR(CAccident* pAccident);
    ~CTaskSimpleGiveCPR() override;

    eTaskType GetTaskType() override { return TASK_SIMPLE_GIVE_CPR; }

    CTask* Clone() override;
    bool ProcessPed(CPed* ped) override;
    bool MakeAbortable(class CPed* ped, eAbortPriority priority, const CEvent* event) override;

    void ReviveDeadPed(CPed* ped);
    static void FinishGiveCPRAnimCB(CAnimBlendAssociation* pAnim, void* priv);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimpleGiveCPR* Constructor(CAccident* pAccident);

    CTask* Clone_Reversed();
    bool ProcessPed_Reversed(CPed* ped);
    bool MakeAbortable_Reversed(class CPed* ped, eAbortPriority priority, const CEvent* event);
};

VALIDATE_SIZE(CTaskSimpleGiveCPR, 0x18);
