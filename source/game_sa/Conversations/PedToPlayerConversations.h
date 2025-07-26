#pragma once

class CPed;

class CPedToPlayerConversations {
public:
    enum class eP2pState : uint32 {
        INACTIVE = 0,
        PEDHASOPENED,
        WAITINGFORFINALWORD,
        WAITINGTOFINISH,
    };

    static inline eP2pState& m_State                        = StaticRef<eP2pState, 0x969A20>();
    static inline CPed*&     m_pPed                         = StaticRef<CPed*, 0x9691C0>();
    static inline int32&     m_Topic                        = StaticRef<int32, 0x9691BC>();
    static inline uint32&    m_TimeOfLastPlayerConversation = StaticRef<uint32, 0x9691B4>();
    static inline uint32&    m_StartTime                    = StaticRef<uint32, 0x9691B8>();
    static inline bool&      m_bPositiveReply               = StaticRef<bool, 0x9691B0>(); // unused
    static inline bool&      m_bPositiveOpening             = StaticRef<bool, 0x9691B1>();

    static void InjectHooks();
    static void Clear();
    static void Update();
    static void EndConversation();
};
