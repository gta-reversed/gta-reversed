#pragma once

#include "TaskComplex.h"

class NOTSA_EXPORT_VTABLE CTaskComplexInAirAndLand : public CTaskComplex {
public:
    bool m_bUsingJumpGlide;
    bool m_bUsingFallGlide;
    bool m_bInvalidClimb;

public:
    static constexpr auto Type = TASK_COMPLEX_IN_AIR_AND_LAND;

    CTaskComplexInAirAndLand(bool bUsingJumpGlide, bool bUsingFallGlide);
    ~CTaskComplexInAirAndLand() override = default;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskComplexInAirAndLand(m_bUsingJumpGlide, m_bUsingFallGlide); }
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;


private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexInAirAndLand* Constructor(bool bUsingJumpGlide, bool bUsingFallGlide);
};

VALIDATE_SIZE(CTaskComplexInAirAndLand, 0x10);
