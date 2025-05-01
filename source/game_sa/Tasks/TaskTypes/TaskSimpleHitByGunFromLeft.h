#pragma once

#include "TaskSimpleRunAnim.h"

class CPed;
class CTaskSimpleHitByGunFromLeft;

class NOTSA_EXPORT_VTABLE CTaskSimpleHitByGunFromLeft : public CTaskSimpleRunAnim {

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_SIMPLE_HIT_BY_GUN_LEFT;

    CTaskSimpleHitByGunFromLeft();
    CTaskSimpleHitByGunFromLeft(const CTaskSimpleHitByGunFromLeft&);

    virtual CTask* Clone() const { return new CTaskSimpleHitByGunFromLeft{ *this }; }
    virtual bool IsInterruptable(CPed const* ped) { return false; }

private: // Wrappers for hooks
    // 0x62F080
    CTaskSimpleHitByGunFromLeft* Constructor() {
        this->CTaskSimpleHitByGunFromLeft::CTaskSimpleHitByGunFromLeft();
        return this;
    }
};
