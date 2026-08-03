#pragma once

#include "TaskSimpleNone.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleFinishBrain : public CTaskSimpleNone {
public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_FINISH_BRAIN;

    static void InjectHooks();

    CTaskSimpleFinishBrain() = default;
    CTaskSimpleFinishBrain(const CTaskSimpleFinishBrain&);
    ~CTaskSimpleFinishBrain() override = default;

    CTask*    Clone() const override { return new CTaskSimpleFinishBrain{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
};
