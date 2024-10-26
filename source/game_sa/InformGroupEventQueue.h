#pragma once

class CEvent;
class CEntity;
class CPed;
class CPedGroup;

struct CInformGroupEvent {
    CPed*      m_Ped;
    CPedGroup* m_PedGroup;
    CEvent*    m_Event;
    int32      m_Time;
};
VALIDATE_SIZE(CInformGroupEvent, 0x10);

class CInformGroupEventQueue {
public:
    static inline std::array<CInformGroupEvent, 8>& ms_informGroupEvents = StaticRef<std::array<CInformGroupEvent, 8>>(0xA9B018);

public:
    static void InjectHooks();

    static void Init();
    static bool Add(CPed* ped, CPedGroup* pedGroup, CEventAcquaintancePed* event);
    static void Flush();
    static void Process();
};
