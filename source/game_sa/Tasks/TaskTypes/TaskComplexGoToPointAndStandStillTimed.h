#pragma once

#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskTimer.h"

class NOTSA_EXPORT_VTABLE CTaskComplexGoToPointAndStandStillTimed : public CTaskComplexGoToPointAndStandStill {
public:
    int32      m_nTime;
    CTaskTimer m_timer;

public:
    CTaskComplexGoToPointAndStandStillTimed(eMoveState moveState, const CVector& targetPoint, float fRadius, float fMoveStateRadius, int32 time);
    ~CTaskComplexGoToPointAndStandStillTimed() override;

    CTask* Clone() const override;
    void StopTimer(const CEvent* event) override;
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    // 0x6685E0
    CTaskComplexGoToPointAndStandStillTimed* Constructor(eMoveState moveState, const CVector& targetPoint, float fRadius, float fMoveStateRadius, int32 time) {
        this->CTaskComplexGoToPointAndStandStillTimed::CTaskComplexGoToPointAndStandStillTimed(moveState, targetPoint, fRadius, fMoveStateRadius, time);
        return this;
    }

};

VALIDATE_SIZE(CTaskComplexGoToPointAndStandStillTimed, 0x38);
