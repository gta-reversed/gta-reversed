#pragma once

#include "Event.h"

#include "Event.h"

class CEventSignalAtPed final : public CEvent {
public:
    CPed* m_SignalAt;
    int32 m_InitialPause;
    bool  m_bPlayAnimAtEnd;

public:
    CEventSignalAtPed(CPed* signalAt, int32 initialPause, bool m_bPlayAnimAtEnd);
   ~CEventSignalAtPed() override;

   eEventType GetEventType() const override { return EVENT_SIGNAL_AT_PED; }
   int32 GetEventPriority() const override { return 10; }
   int32 GetLifeTime() override { return 0; }
   bool AffectsPed(CPed* ped) override;
   CEvent* Clone() override { return new CEventSignalAtPed(m_SignalAt, m_InitialPause, m_bPlayAnimAtEnd); }
   bool IsValid(CPed* ped) override;
};

