#pragma once

#include "TaskSimpleRunAnim.h"

class CTaskSimpleHitFromFront;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleHitFromFront : public CTaskSimpleRunAnim {
public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_SIMPLE_HIT_FRONT;

    CTaskSimpleHitFromFront();
    CTaskSimpleHitFromFront(const CTaskSimpleHitFromFront&);

    virtual CTask* Clone() const { return new CTaskSimpleHitFromFront{ *this }; }
    virtual bool IsInterruptable(CPed const* ped) { return false; }

private: // Wrappers for hooks
    // 0x637250
    CTaskSimpleHitFromFront* Constructor() {
        this->CTaskSimpleHitFromFront::CTaskSimpleHitFromFront();
        return this;
    }
};
