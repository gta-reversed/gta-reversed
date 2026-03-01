#pragma once

#include "TaskComplex.h"

class CPed;
class CPedAttractor;
class CTaskComplexUseAttractor;

class NOTSA_EXPORT_VTABLE CTaskComplexUseAttractor : public CTaskComplex {

public:
    CPedAttractor* m_attractor; // 0xC

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_USE_ATTRACTOR;

    CTaskComplexUseAttractor(CPedAttractor*);
    CTaskComplexUseAttractor(const CTaskComplexUseAttractor&);
    ~CTaskComplexUseAttractor() = default;

    CTask*    Clone() const override { return new CTaskComplexUseAttractor{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x6326F0
    auto Constructor(CPedAttractor* attractor) {
        this->CTaskComplexUseAttractor::CTaskComplexUseAttractor(attractor);
        return this;
    }

    // 0x632720
    auto Destructor() {
        this->CTaskComplexUseAttractor::~CTaskComplexUseAttractor();
        return this;
    }
};
