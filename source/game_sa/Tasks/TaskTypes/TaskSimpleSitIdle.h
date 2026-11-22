#pragma once

#include "TaskSimple.h"
#include "TaskTimer.h"

class CAnimBlendAssociation;
class CTaskSimpleSitIdle;
class CEvent;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleSitIdle : public CTaskSimple {

public:
    bool m_bSitAfterStep = {};          //< If `true` animation used will include a "step" (step with a leg) before sitting down
    bool m_unused1 = {};                //< Unknown and unused
    int32 m_durationMs = {};            //< Duration of animation
    CAnimBlendAssociation* m_anim = {}; //< The animation
    CTaskTimer m_animStartedTimer = {}; //< Timer started in the same time as the anim

public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_SIT_IDLE;

    CTaskSimpleSitIdle(int32 durationMs, bool sitAfterStep);
    CTaskSimpleSitIdle(const CTaskSimpleSitIdle&);
    ~CTaskSimpleSitIdle() = default;

    void StartAnim(CPed* ped);

    CTask*    Clone() const override { return new CTaskSimpleSitIdle{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;

private: // Wrappers for hooks
    // 0x631160
    CTaskSimpleSitIdle* Constructor(int32 durationMs, bool sitAfterStep) {
        this->CTaskSimpleSitIdle::CTaskSimpleSitIdle(durationMs, sitAfterStep);
        return this;
    }
    // 0x6311B0
    CTaskSimpleSitIdle* Destructor() {
        this->CTaskSimpleSitIdle::~CTaskSimpleSitIdle();
        return this;
    }
};
