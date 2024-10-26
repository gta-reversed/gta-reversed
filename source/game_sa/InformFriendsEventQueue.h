#pragma once

#include "InformGroupEventQueue.h"

class CEntity;
class CEvent;

class CInformFriendsEventQueue {
public:
    static inline std::array<CInformGroupEvent, 8>& ms_informFriendsEvents = StaticRef<std::array<CInformGroupEvent, 8>>(0xA9AFB8);

public:
    static void InjectHooks();

    CInformFriendsEventQueue() = default;
    ~CInformFriendsEventQueue() = default;

    static void Init();
    static void Flush();
    static bool Add(CEntity* entity, CEvent* event);
    static void Process();
};
