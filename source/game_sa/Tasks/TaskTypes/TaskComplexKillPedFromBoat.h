#pragma once

#include "TaskComplex.h"
#include <extensions/EntityRef.hpp>

class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexKillPedFromBoat : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_KILL_PED_FROM_BOAT;

    explicit CTaskComplexKillPedFromBoat(CPed* targetPed);
    CTaskComplexKillPedFromBoat(const CTaskComplexKillPedFromBoat&);
    ~CTaskComplexKillPedFromBoat() override = default; // 0x622830

    CTask*    Clone() const override { return new CTaskComplexKillPedFromBoat{ *this }; } // 0x6238A0
    eTaskType GetTaskType() const override { return Type; }                               // 0x622820
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    static void InjectHooks();

private:
    notsa::EntityRef<CPed> m_TargetPed{};
};
VALIDATE_SIZE(CTaskComplexKillPedFromBoat, 0x10);
