#pragma once

#include "TaskSimple.h"
#include "TaskTimer.h"
#include <Audio/Enums/PedSpeechContexts.h>

class CEvent;
class CTaskSimpleSay;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleSay : public CTaskSimple {
public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_SAY;

    CTaskSimpleSay(eGlobalSpeechContext gctx, int32 sayDuration = -1);
    CTaskSimpleSay(const CTaskSimpleSay& o) : CTaskSimpleSay{o.m_GlobalSpeechContext, o.m_Duration} {} // NOTSA
    ~CTaskSimpleSay() = default;

    CTask* Clone() const override { return new CTaskSimpleSay(*this); }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return true; }
    bool      ProcessPed(CPed* ped) override;

private:
    eGlobalSpeechContextS32 m_GlobalSpeechContext{};
    int32                   m_Duration{};
    CTaskTimer              m_Timer{};

private: // Wrappers for hooks
    // 0x48E360
    CTaskSimpleSay* Constructor(eGlobalSpeechContextS16 sayId, uint32 sayDuration) {
        this->CTaskSimpleSay::CTaskSimpleSay(sayId, sayDuration);
        return this;
    }
    // 0x48E4E0
    CTaskSimpleSay* Destructor() {
        this->CTaskSimpleSay::~CTaskSimpleSay();
        return this;
    }
};
