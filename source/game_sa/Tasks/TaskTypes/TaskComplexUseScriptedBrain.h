#pragma once

#include "TaskComplex.h"

class CPed;
class CEvent;
class CTaskComplexUseScriptedBrain;
class CTask;

class NOTSA_EXPORT_VTABLE CTaskComplexUseScriptedBrain : public CTaskComplex {

public:
    bool   m_aborting = {};
    char   m_name[8] = {};
    CTask* m_currTask = {};

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_USE_SCRIPTED_BRAIN;

    CTaskComplexUseScriptedBrain(char const* a2);
    CTaskComplexUseScriptedBrain(const CTaskComplexUseScriptedBrain&);
    ~CTaskComplexUseScriptedBrain();

    CTask*    Clone() const override { return new CTaskComplexUseScriptedBrain{ *this }; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private:
    /// Clear the current task (if any) from the brain entry (if any)
    void ClearCurrentTask();

private: // Wrappers for hooks
    // 0x6358B0
    CTaskComplexUseScriptedBrain* Constructor(char const* a2) {
        this->CTaskComplexUseScriptedBrain::CTaskComplexUseScriptedBrain(a2);
        return this;
    }
    // 0x635900
    CTaskComplexUseScriptedBrain* Destructor() {
        this->CTaskComplexUseScriptedBrain::~CTaskComplexUseScriptedBrain();
        return this;
    }
};
