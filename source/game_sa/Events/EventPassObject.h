#pragma once

#include "Event.h"

#include "Event.h"

class NOTSA_EXPORT_VTABLE CEventPassObject : public CEvent {
public:
    CPed* m_giver;
    bool  m_dontPassObject;

public:
    explicit CEventPassObject(CPed* giver, bool dontPassObject = false);
    ~CEventPassObject() override;

    eEventType GetEventType() const override { return EVENT_PASS_OBJECT; }
    int32 GetEventPriority() const override { return 11; }
    int32 GetLifeTime() override { return 0; }
    CEventPassObject* Clone() override { return new CEventPassObject(m_giver, m_dontPassObject); }
    bool AffectsPed(CPed* ped) override { return true; }
    bool IsValid(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventPassObject* Constructor(CPed* giver, bool dontPassObject);
    CEventPassObject* Destructor();

};
VALIDATE_SIZE(CEventPassObject, 0x14);
