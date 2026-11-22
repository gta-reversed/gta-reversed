/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
#include "TaskTimer.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleStandStill : public CTaskSimple {
public:
    int32      m_nTime;
    CTaskTimer m_timer;
    bool       m_bLooped;
    bool       m_bUseAnimIdleStance;
    float      m_fBlendData;

public:
    static constexpr auto Type = TASK_SIMPLE_STAND_STILL;

    CTaskSimpleStandStill(int32 nTime = 0, bool Looped = false, bool bUseAnimIdleStance = false, float fBlendData = 8.0f);
    ~CTaskSimpleStandStill() override = default;

public:
    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskSimpleStandStill(m_nTime, m_bLooped, m_bUseAnimIdleStance, m_fBlendData); } // 0x635CF0
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimpleStandStill* Constructor(int32 nTime, bool Looped, bool bUseAnimIdleStance, float fBlendData) { this->CTaskSimpleStandStill::CTaskSimpleStandStill(nTime, Looped, bUseAnimIdleStance, fBlendData); return this; }
};
VALIDATE_SIZE(CTaskSimpleStandStill, 0x20);
