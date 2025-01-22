/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
#include "TaskTimer.h"

enum class eFacialExpression {
    NONE = -1,

    SURPRISEPANIC = 0,
    SURPRISEMILD  = 1,
    CURIOSITY     = 2,
    ANGER         = 3,
    HAPPINESS     = 4,
    SADNESS       = 5,

    MAX_NUM_EMOTIONS,

    TALKING = 7,
    CHEWING = 8,
};

class NOTSA_EXPORT_VTABLE CTaskSimpleFacial : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_FACIAL;

    static void InjectHooks();

    CTaskSimpleFacial(eFacialExpression nFacialExpress, int32 nDuration);
    ~CTaskSimpleFacial() override = default; // 0x690CB0

    eTaskType GetTaskType() const override { return Type; } // 0x690CA0
    CTask* Clone() const override { return new CTaskSimpleFacial(m_Type, m_Duration); } // 0x692820
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;

    auto GetFacialType() const { return m_Type; }
    void StartTimer(int32 dur) { m_Timer.Start(dur); }

    static AnimationId GetAnimId(eFacialExpression expression);

private:
    CTaskTimer        m_Timer;
    eFacialExpression m_Type;
    int32             m_Duration;
};
VALIDATE_SIZE(CTaskSimpleFacial, 0x1C);
