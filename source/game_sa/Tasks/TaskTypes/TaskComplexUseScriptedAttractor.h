#pragma once

#include "TaskComplex.h"

class CPed;
class CTaskComplexUseScriptedAttractor;
class CPedAttractor;

class NOTSA_EXPORT_VTABLE CTaskComplexUseScriptedAttractor : public CTaskComplex {
public:
    const CPedAttractor& m_attractor;

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_USE_SCRIPTED_ATTRACTOR;

    CTaskComplexUseScriptedAttractor(const CPedAttractor&);
    CTaskComplexUseScriptedAttractor(const CTaskComplexUseScriptedAttractor&);
    ~CTaskComplexUseScriptedAttractor() = default;

    CTask*    Clone() const override { return new CTaskComplexUseScriptedAttractor{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override { return nullptr; }
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }

private: // Wrappers for hooks
    // 0x633330
    CTaskComplexUseScriptedAttractor* Constructor(CPedAttractor const& attractor) {
        this->CTaskComplexUseScriptedAttractor::CTaskComplexUseScriptedAttractor(attractor);
        return this;
    }
    // 0x633360
    CTaskComplexUseScriptedAttractor* Destructor() {
        this->CTaskComplexUseScriptedAttractor::~CTaskComplexUseScriptedAttractor();
        return this;
    }
};
