#pragma once
#include "TaskSimpleRunAnim.h"
#include "AnimationEnums.h"

class CTaskSimpleAbseil : public CTaskSimpleRunAnim
{
public:
    CTaskSimpleAbseil() : CTaskSimpleRunAnim(ANIM_GROUP_DEFAULT, ANIM_ID_ABSEIL, 4.0F, TASK_SIMPLE_ABSEIL, "Abseil", false) {}
    ~CTaskSimpleAbseil() override {}
    CTask* Clone() const override { return new CTaskSimpleAbseil(); }

    virtual bool IsInterruptable(CPed* ped) { return false; }
};

VALIDATE_SIZE(CTaskSimpleAbseil, 0x20);
