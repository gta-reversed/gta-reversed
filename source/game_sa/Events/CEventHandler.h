/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "CEventHandlerHistory.h"

class CTask;
class CEvent;
class CPed;

class CEventHandler {
public:
    CPed* m_ped;
    CEventHandlerHistory m_history;
    CTask* m_physicalResponseTask;
    CTask* m_eventResponseTask;
    CTask* m_attackTask;
    CTask* m_sayTask;
    CTask* m_partialAnimTask;

public:
    void HandleEvents();
    void FlushImmediately();
    eEventType GetCurrentEventType();
    static bool IsTemporaryEvent(const CEvent& event);
};

VALIDATE_SIZE(CEventHandler, 0x34);
