#pragma once

#include <Audio/Enums/PedSpeechContexts.h>

class CPed;
class CPlayerPed;
class CVehicle;

class CPedToPlayerConversations {
public:
    enum class eP2pState : uint32 {
        INACTIVE = 0,
        PEDHASOPENED,
        WAITINGFORFINALWORD,
        WAITINGTOFINISH,
    };

    enum class eTopic : int32 {
        CAR = 0,
        CLOTHES,
        HAIR,
        PHYSIQUE,
        SHOES,
        SMELL,
        TATTOO,
        WEATHER,
        WHERE_YOU_FROM,
        GANGBANG,
    };

    static inline auto& m_State                        = StaticRef<eP2pState>(0x969A20);
    static inline auto& m_pPed                         = StaticRef<CPed*>(0x9691C0);
    static inline auto& m_Topic                        = StaticRef<eTopic>(0x9691BC);
    static inline auto& m_TimeOfLastPlayerConversation = StaticRef<uint32>(0x9691B4);
    static inline auto& m_StartTime                    = StaticRef<uint32>(0x9691B8);
    static inline auto& m_PositiveReply                = StaticRef<bool>(0x9691B0); // unused
    static inline auto& m_PositiveOpening              = StaticRef<bool>(0x9691B1);

    static void Clear();
    static void Update();
    static void EndConversation();

    static void InjectHooks();

private:
    //! NOTSA: The line a ped opens the conversation with
    struct Opening {
        eGlobalSpeechContext Line{};
        bool                 IsPositive{};
    };

    static std::optional<Opening> GetOpeningAboutTopic(CPed* ped, CPlayerPed* player, eTopic topic);
    static void                   MakePedAttackPlayer(CPlayerPed* player);

private:
    static inline auto& s_PedPoolIndex      = StaticRef<int32>(0x969A3C);     //!< Pool slot of the ped that was checked last
    static inline auto& s_LastPlayerVehicle = StaticRef<CVehicle*>(0x969A40); //!< The vehicle the player was in last
};
