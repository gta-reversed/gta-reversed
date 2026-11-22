#pragma once

#include "TaskSimpleRunAnim.h"

class CTaskSimpleCower;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleCower : public CTaskSimpleRunAnim {

public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_COWER;

    CTaskSimpleCower() : CTaskSimpleRunAnim{ ANIM_GROUP_DEFAULT, ANIM_ID_HANDSCOWER, 4.0, Type, "Cower", false } {}
    ~CTaskSimpleCower() = default;

    CTask* Clone() const override { return new CTaskSimpleCower{}; }
    virtual bool IsInterruptable(CPed const* ped) { return false; } // NOTE: Possible leftover from `CTaskSimpleRunAnim`?

private: // Wrappers for hooks
    // 0x48DE70
    CTaskSimpleCower* Constructor() {
        this->CTaskSimpleCower::CTaskSimpleCower();
        return this;
    }
    // 0x48E900
    CTaskSimpleCower* Destructor() {
        this->CTaskSimpleCower::~CTaskSimpleCower();
        return this;
    }
};
