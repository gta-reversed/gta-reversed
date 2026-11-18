#pragma once

#include "TaskSimpleRunAnim.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleScratchHead : public CTaskSimpleRunAnim {
public:
    CTaskSimpleScratchHead();
    CTaskSimpleScratchHead(const CTaskSimpleScratchHead&);
    ~CTaskSimpleScratchHead() override = default;

    CTask*       Clone() const override { return new CTaskSimpleScratchHead(); } // 0x48DF60
    virtual bool IsInterruptable(const CPed* ped) { return false; }              // 0x48DFD0

public:
    static void InjectHooks();
};
