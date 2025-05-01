#pragma once

#include "TaskSimpleRunTimedAnim.h"

class CPed;

class NOTSA_EXPORT_VTABLE  CTaskSimpleChat : public CTaskSimpleRunTimedAnim {
public:
    static constexpr auto Type = TASK_SIMPLE_CHAT;

    static void InjectHooks();

    CTaskSimpleChat(uint32 duration);
    CTaskSimpleChat(const CTaskSimpleChat& o);
    ~CTaskSimpleChat();

    virtual CTask* Clone() const { return new CTaskSimpleChat{ *this }; }
    virtual bool   IsInterruptable(CPed const* ped) { return false; }
 
private:
    // 0x5F7FD0
    CTaskSimpleChat* Destructor() {
        this->CTaskSimpleChat::~CTaskSimpleChat();
        return this;
    }
};
