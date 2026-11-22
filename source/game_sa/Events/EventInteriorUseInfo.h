#pragma once

#include "Event.h"

#include "Event.h"
#include "Interior_c.h"
#include "InteriorInfo_t.h"

class NOTSA_EXPORT_VTABLE CEventInteriorUseInfo : public CEvent {
public:
    CEventInteriorUseInfo(InteriorInfo_t* interiorInfo, Interior_c* interior, uint32 animTime, int8 loopAction);
    ~CEventInteriorUseInfo() override = default;

    eEventType GetEventType() const override { return EVENT_INTERIOR_USE_INFO; }
    int32      GetEventPriority() const override { return 9; }
    int32      GetLifeTime() override { return 0; }
    CEvent*    Clone() const noexcept override { return new CEventInteriorUseInfo(m_InteriorInfo, m_Interior, m_ActionAnimTime, m_LoopAction); }
    bool       AffectsPed(CPed* ped) override { return true; }
    bool       IsValid(CPed* ped) override;

public:
    static void InjectHooks();

private:
    InteriorInfo_t* m_InteriorInfo{};
    Interior_c*     m_Interior{};
    uint32          m_ActionAnimTime{};
    int8            m_LoopAction{};
};
VALIDATE_SIZE(CEventInteriorUseInfo, 0x1C);
