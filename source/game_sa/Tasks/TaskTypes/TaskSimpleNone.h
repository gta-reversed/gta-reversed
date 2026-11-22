#pragma once

#include "TaskSimple.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleNone : public CTaskSimple {
public:
    static constexpr auto Type = TASK_NONE;

    CTaskSimpleNone() = default;
    ~CTaskSimpleNone() override = default;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskSimpleNone(); }
    bool MakeAbortable(class CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return true; }
    bool ProcessPed(CPed* ped) override { return true; }

public:
    static void InjectHooks();

private: // Wrappers for hooks
    // 0x4636E0
    CTaskSimpleNone* Destructor() {
        this->CTaskSimpleNone::~CTaskSimpleNone();
        return this;
    }
};
VALIDATE_SIZE(CTaskSimpleNone, 0x8);
