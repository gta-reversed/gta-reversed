#pragma once

#include "TaskSimpleRunAnim.h"

class CTaskSimpleHitFromBehind;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleHitFromBehind : public CTaskSimpleRunAnim {
public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_SIMPLE_HIT_BEHIND;

    CTaskSimpleHitFromBehind();
    CTaskSimpleHitFromBehind(const CTaskSimpleHitFromBehind&);

    virtual CTask* Clone() const { return new CTaskSimpleHitFromBehind{ *this }; }
    virtual bool IsInterruptable(CPed const* ped) { return false; }

private: // Wrappers for hooks
    // 0x637250
    CTaskSimpleHitFromBehind* Constructor() {
        this->CTaskSimpleHitFromBehind::CTaskSimpleHitFromBehind();
        return this;
    }
};
