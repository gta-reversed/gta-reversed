#pragma once

#include "TaskSimpleRunAnim.h"

class CTaskSimpleHitFromRight;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleHitFromRight : public CTaskSimpleRunAnim {
public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_SIMPLE_HIT_RIGHT;

    CTaskSimpleHitFromRight();
    CTaskSimpleHitFromRight(const CTaskSimpleHitFromRight&);

    virtual CTask* Clone() const { return new CTaskSimpleHitFromRight{ *this }; }
    virtual bool IsInterruptable(CPed const* ped) { return false; }

private: // Wrappers for hooks
    // 0x637250
    CTaskSimpleHitFromRight* Constructor() {
        this->CTaskSimpleHitFromRight::CTaskSimpleHitFromRight();
        return this;
    }
};
