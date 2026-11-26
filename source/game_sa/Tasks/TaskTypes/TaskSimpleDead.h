#pragma once

#include "TaskSimple.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleDead : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_DEAD;

    CTaskSimpleDead(uint32 deathTime, bool hasDrowned);
    CTaskSimpleDead(const CTaskSimpleDead&);
    ~CTaskSimpleDead() override = default; // 0x6305F0

    eTaskType GetTaskType() const override { return Type; }                                                                                      // 0x6305D0
    CTask*    Clone() const override { return new CTaskSimpleDead{ *this }; }                                                                    // 0x636100
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return true; } // 0x6305E0
    bool      ProcessPed(CPed* ped) override;

public:
    static void InjectHooks();

public:
    uint32 m_TimeOfDeathMs{};
    bool   m_bFirstTime : 1 { true };
    bool   m_bHasDrowned : 1 {};
    bool   m_bBloodPuddleCreated : 1 {};
};
VALIDATE_SIZE(CTaskSimpleDead, 0x10);
