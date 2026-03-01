#pragma once

class CEntity;
class CEvent;

struct CInformFriendsEvent {
    CPed*   m_Ped{};
    CEvent* m_Event{};
    int32   m_Time{-1};
};
VALIDATE_SIZE(CInformFriendsEvent, 0xC);

class CInformFriendsEventQueue {
public:
    static inline std::array<CInformFriendsEvent, 8>& ms_informFriendsEvents = StaticRef<std::array<CInformFriendsEvent, 8>>(0xA9AFB8);

public:
    static void InjectHooks();

    CInformFriendsEventQueue() = default;
    ~CInformFriendsEventQueue() = default;

    static void Init();
    static void Flush();
    static bool Add(CPed* ped, CEvent* event);
    static void Process();
};
