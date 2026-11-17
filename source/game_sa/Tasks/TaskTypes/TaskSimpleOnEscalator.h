#pragma once

#include "TaskSimpleStandStill.h"

class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleOnEscalator : public CTaskSimpleStandStill {
public:
   static constexpr auto Type = TASK_SIMPLE_ON_ESCALATOR;

   CTaskSimpleOnEscalator();
   CTaskSimpleOnEscalator(const CTaskSimpleOnEscalator&);
   ~CTaskSimpleOnEscalator() override = default;

   CTask*    Clone() const override { return new CTaskSimpleOnEscalator{ *this }; }
   eTaskType GetTaskType() const override { return Type; }
   bool      ProcessPed(CPed* ped) override;

public:
    static void InjectHooks();
};
