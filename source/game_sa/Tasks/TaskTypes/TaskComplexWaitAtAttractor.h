#pragma once

#include "TaskComplex.h"

class CPedAttractor;
class CTaskComplexWaitAtAttractor;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexWaitAtAttractor : public CTaskComplex {

public:
    CPedAttractor* m_attractor{};
    int32 m_slot{};

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_WAIT_AT_ATTRACTOR;

    CTaskComplexWaitAtAttractor(CPedAttractor* attractor, int32 slot);
    CTaskComplexWaitAtAttractor(const CTaskComplexWaitAtAttractor&);
    ~CTaskComplexWaitAtAttractor() = default;

    CTask*    Clone() const override { return new CTaskComplexWaitAtAttractor{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override { return nullptr; }
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }

private: // Wrappers for hooks
    // 0x6325F0
    CTaskComplexWaitAtAttractor* Constructor(CPedAttractor* attractor, int32 slot) {
        this->CTaskComplexWaitAtAttractor::CTaskComplexWaitAtAttractor(attractor, slot);
        return this;
    }
    // 0x632630
    CTaskComplexWaitAtAttractor* Destructor() {
        this->CTaskComplexWaitAtAttractor::~CTaskComplexWaitAtAttractor();
        return this;
    }
};
