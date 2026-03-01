#pragma once

#include "TaskSimpleWaitUntilAreaCodesMatch.h"
#include "Vector.h"

class CPed;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskSimpleWaitUntilLeaderAreaCodesMatch final : public CTaskSimpleWaitUntilAreaCodesMatch {
public:
    bool    m_bGrabbedStartPos{};
    CVector m_vStartPos{};
    bool    m_b{};

public:
    static constexpr auto Type = eTaskType::TASK_WAIT_FOR_MATCHING_LEADER_AREA_CODES;

    static void InjectHooks();

    using CTaskSimpleWaitUntilAreaCodesMatch::CTaskSimpleWaitUntilAreaCodesMatch;
    CTaskSimpleWaitUntilLeaderAreaCodesMatch(const CTaskSimpleWaitUntilLeaderAreaCodesMatch& o) = default;

    CTask*    Clone() const override { return new CTaskSimpleWaitUntilLeaderAreaCodesMatch{*this}; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) override;
    bool      ProcessPed(CPed* ped) override;
};
