#pragma once

#include "TaskSimple.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleGetUp : public CTaskSimple {
public:
    bool                   m_bHasPedGotUp;
    bool                   m_bIsFinished;
    CAnimBlendAssociation* m_Anim;

public:
    static constexpr auto Type = TASK_SIMPLE_GET_UP;

    CTaskSimpleGetUp();
    ~CTaskSimpleGetUp() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskSimpleGetUp(); }
    bool ProcessPed(class CPed* ped) override;
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimpleGetUp* Constructor();

    bool StartAnim(CPed* ped);

    static void FinishGetUpAnimCB(CAnimBlendAssociation* blendAssoc, void* data); //data is CTaskSimpleGetUp

    static inline auto& m_aColPoints = *(std::array<CColPoint, 32>*)0xC18F98;
};

VALIDATE_SIZE(CTaskSimpleGetUp, 0x10);
