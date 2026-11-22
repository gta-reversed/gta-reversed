#pragma once

#include "TaskSimpleRunAnim.h"

class CTaskSimpleHitFromBack;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleHitFromBack : public CTaskSimpleRunAnim {
public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_SIMPLE_HIT_BACK;

    CTaskSimpleHitFromBack();
    CTaskSimpleHitFromBack(const CTaskSimpleHitFromBack&);

    virtual CTask* Clone() const { return new CTaskSimpleHitFromBack{ *this }; }
    virtual bool IsInterruptable(CPed const* ped) { return false; }

private: // Wrappers for hooks
    // 0x637250
    CTaskSimpleHitFromBack* Constructor() {
        this->CTaskSimpleHitFromBack::CTaskSimpleHitFromBack();
        return this;
    }
};
