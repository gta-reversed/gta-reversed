#pragma once

#include "TaskSimpleRunAnim.h"

class CTaskSimpleHitFromLeft;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleHitFromLeft : public CTaskSimpleRunAnim {
public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_SIMPLE_HIT_LEFT;

    CTaskSimpleHitFromLeft();
    CTaskSimpleHitFromLeft(const CTaskSimpleHitFromLeft&);

    virtual CTask* Clone() const { return new CTaskSimpleHitFromLeft{ *this }; }
    virtual bool IsInterruptable(CPed const* ped) { return false; }

private: // Wrappers for hooks
    // 0x637250
    CTaskSimpleHitFromLeft* Constructor() {
        this->CTaskSimpleHitFromLeft::CTaskSimpleHitFromLeft();
        return this;
    }
};
