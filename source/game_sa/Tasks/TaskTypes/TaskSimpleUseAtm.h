#pragma once

#include "TaskSimpleRunAnim.h"

class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleUseAtm : public CTaskSimpleRunAnim {

public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_CAR_SET_TEMP_ACTION;

    CTaskSimpleUseAtm();
    CTaskSimpleUseAtm(const CTaskSimpleUseAtm&); // NOTSA
    ~CTaskSimpleUseAtm() = default;

    CTask* Clone() const override { return new CTaskSimpleUseAtm{ *this }; }
    virtual bool IsInterruptable(CPed const* ped) { return false; }

private: // Wrappers for hooks
    // 0x48DFE0
    CTaskSimpleUseAtm* Constructor() {
        this->CTaskSimpleUseAtm::CTaskSimpleUseAtm();
        return this;
    }
    // 0x48E960
    CTaskSimpleUseAtm* Destructor() {
        this->CTaskSimpleUseAtm::~CTaskSimpleUseAtm();
        return this;
    }
};
