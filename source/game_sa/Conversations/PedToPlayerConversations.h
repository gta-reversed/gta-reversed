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

    enum class eTopic : uint32 {
        CAR = 0,
        CLOTHES,
        HAIR,
        PHYSICS,
        SHOES,
        SMELL,
        TATTOO,
        WEATHER,
        WHERE_ARE_YOU_FROM,
        GANGBANG
    };

    static inline eP2pState& m_State                        = StaticRef<eP2pState, 0x969A20>();
    static inline CPed*&     m_pPed                         = StaticRef<CPed*, 0x9691C0>();
    static inline eTopic&    m_Topic                        = StaticRef<eTopic, 0x9691BC>();
    static inline uint32&    m_StartTime                    = StaticRef<uint32, 0x9691B8>();
    static inline uint32&    m_TimeOfLastPlayerConversation = StaticRef<uint32, 0x9691B4>();
    static inline bool&      m_bPositiveReply               = StaticRef<bool, 0x9691B0>(); // unused
    static inline bool&      m_bPositiveOpening             = StaticRef<bool, 0x9691B1>();

    static void Clear();
    static void Update();
    static void EndConversation();

public: // NOTSA:
    static void InjectHooks();
    static eTopic GetRandomTopicForPed(CPed* ped) {
        if (ped->IsGangster() && ped->m_nPedType != PED_TYPE_GANG2) { // enemy gangster
            return CGeneral::RandomChoiceFromList({ eTopic::WHERE_ARE_YOU_FROM, eTopic::GANGBANG });
        } else {
            return static_cast<eTopic>(CGeneral::GetRandomNumberInRange(0, 7));
        }
    }
};
