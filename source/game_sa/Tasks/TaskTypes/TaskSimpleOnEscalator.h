#pragma once

#include "TaskSimpleStandStill.h"

class CPed;

class CTaskSimpleOnEscalator : public CTaskSimpleStandStill {
public:
   static constexpr auto Type = TASK_SIMPLE_ON_ESCALATOR;

   static void InjectHooks();

   CTaskSimpleOnEscalator();
   CTaskSimpleOnEscalator(const CTaskSimpleOnEscalator&);
   ~CTaskSimpleOnEscalator() override = default;

   CTask*    Clone() const override { return new CTaskSimpleOnEscalator{ *this }; }
   eTaskType GetTaskType() const override { return Type; }
   bool      ProcessPed(CPed* ped) override;
};
