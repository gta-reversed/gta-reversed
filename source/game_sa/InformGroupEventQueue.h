#pragma once
#include "InformGroupEvent.h"

class CEventEditableResponse;
class CPed;
class CPedGroup;

class CInformGroupEventQueue {
public:
    static inline std::array<CInformGroupEvent, 8>& ms_informGroupEvents = StaticRef<std::array<CInformGroupEvent, 8>>(0xA9B018);

public:
    static void InjectHooks();

    static void Init();
    static bool Add(CPed* ped, CPedGroup* pedGroup, CEventEditableResponse* event);
    static void Flush();
    static void Process();
};
