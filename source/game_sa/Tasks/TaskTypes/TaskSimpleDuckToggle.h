#pragma once

#include "TaskSimple.h"

class CPed;
class CEvent;
class CTaskSimpleDuckToggle;

class NOTSA_EXPORT_VTABLE CTaskSimpleDuckToggle : public CTaskSimple {
public:
    enum class eMode : int32 {
        AUTO = -1,
        OFF  = 0,
        ON   = 1,
    };

public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_DUCK_TOGGLE;

    static void InjectHooks();

    CTaskSimpleDuckToggle(eMode mode);
    ~CTaskSimpleDuckToggle() override = default; // 0x62F6D0

    CTask*    Clone() const override { return new CTaskSimpleDuckToggle{ *this }; } // 0x635C90
    eTaskType GetTaskType() const override { return Type; } // 0x62F6B0
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override { return true; } // 0x62F6C0
    bool      ProcessPed(CPed* ped) override;

private: // Wrappers for hooks
    // 0x62F690
    CTaskSimpleDuckToggle* Constructor(eMode a) {
        this->CTaskSimpleDuckToggle::CTaskSimpleDuckToggle(a);
        return this;
    }

    // 0x62F6D0
    CTaskSimpleDuckToggle* Destructor() {
        this->CTaskSimpleDuckToggle::~CTaskSimpleDuckToggle();
        return this;
    }

private:
    eMode m_ToggleMode{};
};
