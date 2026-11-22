#pragma once

#include "TaskSimple.h"
#include "Base.h"

class CAnimBlendAssociation;

class NOTSA_EXPORT_VTABLE CTaskSimpleTired : public CTaskSimple {
public:
    uint32 m_TiredDurationMs;
    uint32 m_AnimStartMs;
    uint32 m_TiredDurationMs_Copy;
    bool   m_AnimHasStarted;
    bool   m_NeedsToStartAnim;
    CAnimBlendAssociation* m_TiredAnim;

public:
    constexpr static auto Type = TASK_SIMPLE_TIRED;

    explicit CTaskSimpleTired(uint32 tiredDurationMs);
    ~CTaskSimpleTired() override = default;

    eTaskType GetTaskType() const override { return Type; } // 0x630F50
    CTask* Clone() const override { return new CTaskSimpleTired(m_TiredDurationMs); } // 0x636180
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;

    bool ProcessPed(CPed* ped) override;
    void StartAnim(CPed* ped);

    static void InjectHooks();
    CTaskSimpleTired* Constructor(int32 tiredDurationMs) { this->CTaskSimpleTired::CTaskSimpleTired(tiredDurationMs); return this; }
    CTaskSimpleTired* Destructor() { this->CTaskSimpleTired::~CTaskSimpleTired(); return this; }
};
VALIDATE_SIZE(CTaskSimpleTired, 0x1C);
