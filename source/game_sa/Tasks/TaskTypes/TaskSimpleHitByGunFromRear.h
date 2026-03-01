#pragma once

#include "TaskSimpleRunAnim.h"

class CPed;
class CTaskSimpleHitByGunFromRear;

class NOTSA_EXPORT_VTABLE CTaskSimpleHitByGunFromRear : public CTaskSimpleRunAnim {
public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_SIMPLE_HIT_BY_GUN_BACK;

    CTaskSimpleHitByGunFromRear();
    CTaskSimpleHitByGunFromRear(const CTaskSimpleHitByGunFromRear&);

    virtual CTask* Clone() const { return new CTaskSimpleHitByGunFromRear{ *this }; }
    virtual bool IsInterruptable(CPed const* ped) { return false; }

private: // Wrappers for hooks
    // 0x62F080
    CTaskSimpleHitByGunFromRear* Constructor() {
        this->CTaskSimpleHitByGunFromRear::CTaskSimpleHitByGunFromRear();
        return this;
    }
};
