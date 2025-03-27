#pragma once

#include "TaskSimpleRunAnim.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleScratchHead : public CTaskSimpleRunAnim {
public:
    CTaskSimpleScratchHead() : CTaskSimpleRunAnim(ANIM_GROUP_DEFAULT, ANIM_ID_XPRESSSCRATCH, 4.0, TASK_SIMPLE_SCRATCH_HEAD, "ScratchHead", false) { }
    ~CTaskSimpleScratchHead() override = default;

    CTask* Clone() const override { return new CTaskSimpleScratchHead(); } // 0x48DF60
    virtual bool IsInterruptable(const CPed* ped) { return false; }  // 0x48DFD0
};