#pragma once

#include "TaskTimer.h"
#include "TaskSimple.h"

class CPed;
class CEvent;
class CTaskSimplePlayerOnFire;

class NOTSA_EXPORT_VTABLE CTaskSimplePlayerOnFire : public CTaskSimple {

public:
    CTaskTimer m_timer = {};       // 0x8
    bool m_bAddedDamageEvent = {}; // 0x14

public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_PLAYER_ON_FIRE;

    CTaskSimplePlayerOnFire() = default;
    CTaskSimplePlayerOnFire(const CTaskSimplePlayerOnFire&) : CTaskSimplePlayerOnFire{} {} // NOTSA
    ~CTaskSimplePlayerOnFire() = default;

    CTask*    Clone() const override { return new CTaskSimplePlayerOnFire{*this}; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return false; }
    bool      ProcessPed(CPed* ped) override;
private: // Wrappers for hooks
    // 0x633560
    CTaskSimplePlayerOnFire* Constructor() {
        this->CTaskSimplePlayerOnFire::CTaskSimplePlayerOnFire();
        return this;
    }
    // 0x6335B0
    CTaskSimplePlayerOnFire* Destructor() {
        this->CTaskSimplePlayerOnFire::~CTaskSimplePlayerOnFire();
        return this;
    }
};
