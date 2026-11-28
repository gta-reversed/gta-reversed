#pragma once

#include "Vector.h"
#include "TaskSimpleRunNamedAnim.h"

constexpr auto STAND_STILL_TIME = 20000; // 0x86DB24

class NOTSA_EXPORT_VTABLE CTaskSimpleSlideToCoord : public CTaskSimpleRunNamedAnim {
public:
    static constexpr auto Type = TASK_SIMPLE_SLIDE_TO_COORD;

    CTaskSimpleSlideToCoord(const CVector& slideToPos, float aimingRotation, float speed);
    CTaskSimpleSlideToCoord(const CVector& slideToPos, float aimingRotation, float speed, const char* animName, const char* animGroupName, uint32 animFlags, float animBlendDelta, bool runInSequence, uint32 time);
    ~CTaskSimpleSlideToCoord() override = default;

    CTask*    Clone() const override;
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;

    static void InjectHooks();

private:
    CVector m_SlideToPos{};
    float   m_AimingRotation{};
    float   m_Speed{};
    bool    m_IsFirstTime : 1 {};
    bool    m_IsRunningAnim : 1 {};
    int32   m_EndTime{}; //!< Game time [ms] when `ProcessPed` gives up, -1 until the timeout starts
};
VALIDATE_SIZE(CTaskSimpleSlideToCoord, 0x80);
