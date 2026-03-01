#pragma once

#include "TaskSimpleRunTimedAnim.h"
class CTaskSimpleLookAbout;
class CPed;


class NOTSA_EXPORT_VTABLE CTaskSimpleLookAbout : public CTaskSimpleRunTimedAnim {
 
public:
    static void InjectHooks();

    constexpr static auto Type = TASK_SIMPLE_LOOK_ABOUT;

    CTaskSimpleLookAbout(uint32 durationMs);
    CTaskSimpleLookAbout(const CTaskSimpleLookAbout& o);

    CTask*  Clone() const override { return new CTaskSimpleLookAbout{*this}; }
    virtual bool IsInterruptable(CPed const* ped) { return false; }
private: // Wrappers for hooks
    // 0x48E0A0
    CTaskSimpleLookAbout* Constructor(int32 durationMs) {
        this->CTaskSimpleLookAbout::CTaskSimpleLookAbout(durationMs);
        return this;
    }
};
