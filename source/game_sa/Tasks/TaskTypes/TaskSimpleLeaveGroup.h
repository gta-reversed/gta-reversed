#pragma once

#include "TaskSimpleNone.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleLeaveGroup : public CTaskSimpleNone {
public:
    static void InjectHooks();

    CTaskSimpleLeaveGroup() = default;
    ~CTaskSimpleLeaveGroup() = default;

private: // Wrappers for hooks
    // 0x463540
    CTaskSimpleLeaveGroup* Constructor() {
        this->CTaskSimpleLeaveGroup::CTaskSimpleLeaveGroup();
        return this;
    }
    // 0x463560
    CTaskSimpleLeaveGroup* Destructor() {
        this->CTaskSimpleLeaveGroup::~CTaskSimpleLeaveGroup();
        return this;
    }
};
