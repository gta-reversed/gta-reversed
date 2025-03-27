#pragma once

#include "TaskSimple.h"

class CTaskSimpleDoHandSignal;
class CPed;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskSimpleDoHandSignal : public CTaskSimple {
public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_SIMPLE_DO_HAND_SIGNAL;

    CTaskSimpleDoHandSignal() = default;
    CTaskSimpleDoHandSignal(const CTaskSimpleDoHandSignal&);
    ~CTaskSimpleDoHandSignal() = default;

    CTask*    Clone() const override { return new CTaskSimpleDoHandSignal{*this}; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) override { return true; }
    bool      ProcessPed(CPed* ped) override;

private: // Wrappers for hooks
    // 0x660880
    CTaskSimpleDoHandSignal* Constructor() {
        this->CTaskSimpleDoHandSignal::CTaskSimpleDoHandSignal();
        return this;
    }
    // 0x6608B0
    CTaskSimpleDoHandSignal* Destructor() {
        this->CTaskSimpleDoHandSignal::~CTaskSimpleDoHandSignal();
        return this;
    }

protected:
    bool m_Initialized{};
};
