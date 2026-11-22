#pragma once

#include "TaskComplex.h"

class CTaskComplexMoveBackAndJump;
class CTask;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexMoveBackAndJump : public CTaskComplex {

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_MOVE_BACK_AND_JUMP;

    CTaskComplexMoveBackAndJump() = default;
    CTaskComplexMoveBackAndJump(const CTaskComplexMoveBackAndJump&) : CTaskComplexMoveBackAndJump{} {}
    ~CTaskComplexMoveBackAndJump() = default;
    static CTask* CreateSubTask(eTaskType taskType);

    CTask*    Clone() const override { return new CTaskComplexMoveBackAndJump{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }

private: // Wrappers for hooks
    // 0x655340
    CTaskComplexMoveBackAndJump* Constructor() {
        this->CTaskComplexMoveBackAndJump::CTaskComplexMoveBackAndJump();
        return this;
    }

    // 0x655370
    CTaskComplexMoveBackAndJump* Destructor() {
        this->CTaskComplexMoveBackAndJump::~CTaskComplexMoveBackAndJump();
        return this;
    }
};
