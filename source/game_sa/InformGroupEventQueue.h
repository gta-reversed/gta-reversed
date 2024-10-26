#pragma once

class CEvent;
class CEntity;
class CPed;
class CPedGroup;

struct CInformGroupEvent {
    CPed*   m_Ped;
    CEvent* m_Event;
    int32   m_Time;
};
VALIDATE_SIZE(CInformGroupEvent, 0xC);

class CInformGroupEventQueue {
public:
    static void InjectHooks();

    static void Init();
    static bool Add(CEntity* entity, CPedGroup* pedGroup, CEvent* event);
    static void Flush();
    static void Process();
};
