#pragma once

#include "TaskSimpleRunAnim.h"

class CPed;
class CTaskSimpleHitByGunFromFront;

class NOTSA_EXPORT_VTABLE CTaskSimpleHitByGunFromFront : public CTaskSimpleRunAnim {
public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_SIMPLE_HIT_BY_GUN_FRONT;

    CTaskSimpleHitByGunFromFront();
    CTaskSimpleHitByGunFromFront(const CTaskSimpleHitByGunFromFront&);

    virtual CTask* Clone() const { return new CTaskSimpleHitByGunFromFront{ *this }; }
    virtual bool IsInterruptable(CPed const* ped) { return false; }

private: // Wrappers for hooks
    // 0x62F080
    CTaskSimpleHitByGunFromFront* Constructor() {
        this->CTaskSimpleHitByGunFromFront::CTaskSimpleHitByGunFromFront();
        return this;
    }
};
