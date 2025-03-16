#pragma once

#include "TaskComplexJump.h"

class NOTSA_EXPORT_VTABLE CTaskComplexClimb : public CTaskComplexJump {
public:
    static constexpr auto Type = TASK_COMPLEX_CLIMB;

    CTaskComplexClimb() : CTaskComplexJump(COMPLEX_JUMP_TYPE_CLIMB) {}; // 0x46A630
    ~CTaskComplexClimb() override = default; // 0x46A6E0

    CTask* Clone() const override { return new CTaskComplexClimb(); } // 0x46A6B0
    eTaskType GetTaskType() const override { return Type; } // 0x46A6C0
};
