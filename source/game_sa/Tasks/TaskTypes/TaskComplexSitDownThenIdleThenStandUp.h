#pragma once

#include "TaskComplex.h"

class CEvent;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexSitDownThenIdleThenStandUp : public CTaskComplex {

public:
    int32 m_durationMs = {};      // 0xC
    bool   m_sitOnStep = {};       // 0x10
    bool   m_doInstantly = {};     // 0x11
    bool   m_animBlockRefAdded = {}; // 0x12

public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_COMPLEX_SIT_DOWN_THEN_IDLE_THEN_STAND_UP;

    CTaskComplexSitDownThenIdleThenStandUp(int32 durationMs, bool bSitOnStep, bool bInstant);
    CTaskComplexSitDownThenIdleThenStandUp(const CTaskComplexSitDownThenIdleThenStandUp&);
    ~CTaskComplexSitDownThenIdleThenStandUp();

    CTask* CreateSubTask(eTaskType taskType);

    CTask*    Clone() const override { return new CTaskComplexSitDownThenIdleThenStandUp{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed * ped) override;
    CTask*    CreateFirstSubTask(CPed * ped) override;
    CTask*    ControlSubTask(CPed * ped) override;


private: // Wrappers for hooks
    // 0x631460
    CTaskComplexSitDownThenIdleThenStandUp* Constructor(int32 durationMs, uint8 a3, uint8 a4) {
        this->CTaskComplexSitDownThenIdleThenStandUp::CTaskComplexSitDownThenIdleThenStandUp(durationMs, a3, a4);
        return this;
    }
    // 0x6314A0
    CTaskComplexSitDownThenIdleThenStandUp* Destructor() {
        this->CTaskComplexSitDownThenIdleThenStandUp::~CTaskComplexSitDownThenIdleThenStandUp();
        return this;
    }
};
