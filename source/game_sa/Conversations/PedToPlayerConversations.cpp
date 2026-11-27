#include "PedToPlayerConversations.h"

#include "Conversations.h"
#include "PedClothesDesc.h"
#include "Ragdoll/IKChainManager.h"
#include "Events/EventScriptCommand.h"
#include "Tasks/TaskComplexSequence.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFoot.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"

namespace {
constexpr uint32 TIME_BETWEEN_CONVERSATIONS_MS = 30'000;
constexpr uint32 PLAYER_REPLY_TIMEOUT_MS       = 4'000;
constexpr uint32 FINAL_WORD_DELAY_MS           = 3'000;
constexpr uint32 FINISH_DELAY_MS               = 2'500;
constexpr int32  PEDS_CHECKED_PER_UPDATE       = 8;
constexpr float  MAX_DIST_TO_PLAYER            = 7.0f;
constexpr float  MAX_DIST_TO_CAR               = 20.0f;
constexpr float  MIN_HEALTH_OF_LIKED_CAR       = 800.0f;
constexpr int32  LOOK_AT_TIME_MS               = 100'000;
constexpr int32  LOOK_AT_PRIORITY              = 8;

// NOTE: The game's `CGeneral::GetRandomNumberInRange(int32, int32)` (0x407180) is `[min, max)`, evenly spread.
//       The project's version scales by `max - 1` and (almost) never returns `max - 1`.
int32 GetRandomNumberInRange(int32 min, int32 max) {
    return min + (int32)((float)(CGeneral::GetRandomNumber()) * (1.0f / 32768.0f) * (float)(max - min));
}
} // namespace

// 0x43AAE0
void CPedToPlayerConversations::Clear() {
    ZoneScoped;

    if (m_State != eP2pState::INACTIVE) {
        CAEPedSpeechAudioEntity::ReleasePlayerConversation();
        m_State = eP2pState::INACTIVE;
    }

    m_pPed                         = nullptr;
    m_TimeOfLastPlayerConversation = 0;
}

// NOTSA: The topic checks of `Update` (0x43B36B). Returns nothing if the ped has no opinion about it.
std::optional<CPedToPlayerConversations::Opening> CPedToPlayerConversations::GetOpeningAboutTopic(CPed* ped, CPlayerPed* player, eTopic topic) {
    // Every ped judges the player a bit differently
    const auto GetOpinion = [ped](eStats stat, uint16 randomSeedMask, float offset) {
        return (int32)(CStats::GetStatValue(stat) + (float)(ped->m_nRandomSeed & randomSeedMask) - offset);
    };

    switch (topic) {
    case eTopic::CAR: { // 0x43B380
        const auto* const veh = s_LastPlayerVehicle;
        if (!veh || (veh->GetPosition() - ped->GetPosition()).SquaredMagnitude2D() >= sq(MAX_DIST_TO_CAR)) {
            return std::nullopt;
        }
        switch (veh->GetVehicleModelInfo()->m_nVehicleClass) {
        case VEHICLE_CLASS_POORFAMILY:
        case VEHICLE_CLASS_WORKER:
        case VEHICLE_CLASS_BIG:
        case VEHICLE_CLASS_TAXI: // 0x43B44D
            return Opening{ CTX_GLOBAL_CONV_DISL_CAR, false };
        case VEHICLE_CLASS_RICHFAMILY:
        case VEHICLE_CLASS_EXECUTIVE:
        case VEHICLE_CLASS_LEISUREBOAT: // 0x43B406
            if (veh->GetHealth() > MIN_HEALTH_OF_LIKED_CAR) {
                return Opening{ CTX_GLOBAL_CONV_LIKE_CAR, true };
            }
            return std::nullopt;
        default: // 0x43B94E
            return std::nullopt;
        }
    }
    case eTopic::CLOTHES: { // 0x43B47C
        const auto opinion = GetOpinion(STAT_CLOTHES_RESPECT, 0x1FF, 256.0f);
        if (opinion > 500) {
            return Opening{ CTX_GLOBAL_CONV_LIKE_CLOTHES, true };
        }
        if (opinion < 150) {
            return Opening{ CTX_GLOBAL_CONV_DISL_CLOTHES, false };
        }
        return std::nullopt;
    }
    case eTopic::HAIR: { // 0x43B516
        const auto opinion = GetOpinion(STAT_CLOTHES_RESPECT, 0x1FF, 256.0f);
        if (!player->GetClothesDesc()->HasVisibleNewHairCut(0)) {
            return std::nullopt;
        }
        if (opinion > 500) {
            return Opening{ CTX_GLOBAL_CONV_LIKE_HAIR, true };
        }
        if (opinion < 150 || CStats::GetStatValue(STAT_HAIRDRESSING_BUDGET) < 5.0f) {
            return Opening{ CTX_GLOBAL_CONV_DISL_HAIR, false };
        }
        return std::nullopt;
    }
    case eTopic::PHYSIQUE: { // 0x43B5E9
        const auto muscle = GetOpinion(STAT_MUSCLE, 0xFF, 128.0f);
        const auto fat    = GetOpinion(STAT_FAT, 0xFF, 128.0f);
        if (muscle > 400 && fat < 250) {
            return Opening{ CTX_GLOBAL_CONV_LIKE_PHYS, true };
        }
        if (muscle - fat < 200 && fat > 500) {
            return Opening{ CTX_GLOBAL_CONV_DISL_PHYS, false };
        }
        return std::nullopt;
    }
    case eTopic::SHOES: { // 0x43B6BE
        const auto opinion = GetOpinion(STAT_CLOTHES_RESPECT, 0x3FF, 512.0f);
        if (opinion > 700) {
            return Opening{ CTX_GLOBAL_CONV_LIKE_SHOES, true };
        }
        if (opinion < 300) {
            return Opening{ CTX_GLOBAL_CONV_DISL_SHOES, false };
        }
        return std::nullopt;
    }
    case eTopic::SMELL: { // 0x43B758
        const auto opinion = GetOpinion(STAT_SEX_APPEAL, 0x1FF, 256.0f);
        if (opinion > 700) {
            return Opening{ CTX_GLOBAL_CONV_LIKE_SMELL, true };
        }
        if (opinion < 300) {
            return Opening{ CTX_GLOBAL_CONV_DISL_SMELL, false };
        }
        return std::nullopt;
    }
    case eTopic::TATTOO: { // 0x43B7F1
        if (!player->GetClothesDesc()->HasVisibleTattoo()) {
            return std::nullopt;
        }
        const auto opinion = GetRandomNumberInRange(0, 1'000);
        if (opinion > 700) {
            return Opening{ CTX_GLOBAL_CONV_LIKE_TATTOO, true };
        }
        if (opinion < 300) {
            return Opening{ CTX_GLOBAL_CONV_DISL_TATTOO, false };
        }
        return std::nullopt;
    }
    case eTopic::WEATHER: { // 0x43B890
        if (CWeather::Wind < 0.1f) {
            if (CClock::ClockHoursInRange(6, 20)) {
                return Opening{ CTX_GLOBAL_CONV_LIKE_WEATHER, true };
            }
            return std::nullopt;
        }
        if (CWeather::Wind > 0.5f) {
            return Opening{ CTX_GLOBAL_CONV_DISL_WEATHER, false };
        }
        return std::nullopt;
    }
    case eTopic::WHERE_YOU_FROM: // 0x43B923
        return Opening{ CTX_GLOBAL_WHERE_YOU_FROM, true };
    case eTopic::GANGBANG: // 0x43B941
        return Opening{ CTX_GLOBAL_GANGBANG, true };
    default: // 0x43B94E
        return std::nullopt;
    }
}

// NOTSA: Inlined at 0x43BC19, 0x43BD1F and 0x43BF79
void CPedToPlayerConversations::MakePedAttackPlayer(CPlayerPed* player) {
    auto* const tWaitAndAttack = new CTaskComplexSequence(new CTaskSimpleStandStill{ 2'000 }, new CTaskComplexKillPedOnFoot{ player, -1, 0, 0, 0, 0 });
    m_pPed->GetEventGroup().Add(CEventScriptCommand{ TASK_PRIMARY_PRIMARY, tWaitAndAttack });
}

// 0x43B0F0
void CPedToPlayerConversations::Update() {
    auto* const player = FindPlayerPed();
    const auto  now    = CTimer::GetTimeInMS();

    // NOTE: The reference of the previous vehicle is not removed
    if (auto* const veh = FindPlayerVehicle()) {
        s_LastPlayerVehicle = veh;
        CEntity::RegisterReference(s_LastPlayerVehicle);
    }

    switch (m_State) {
    case eP2pState::INACTIVE: { // 0x43B15E
        if (now <= m_TimeOfLastPlayerConversation + TIME_BETWEEN_CONVERSATIONS_MS || FindPlayerVehicle()) {
            return;
        }
        if (FindPlayerPed()->GetWantedLevel() != eWantedLevel::WANTED_CLEAN || CConversations::IsConversationGoingOn() || !player->PedIsReadyForConversation(true)) {
            return;
        }
        for (auto i = 0; i < PEDS_CHECKED_PER_UPDATE; i++) {
            if (++s_PedPoolIndex >= (int32)(GetPedPool()->GetSize())) {
                s_PedPoolIndex = 0;
            }
            auto* const ped = GetPedPool()->GetAt(s_PedPoolIndex);
            if (!ped || !ped->IsCreatedBy(PED_GAME) || !ped->PedIsReadyForConversation(true) || ped->bHasAScriptBrain || !ped->GetIsOnScreen()) {
                continue;
            }
            if (ped->GetForwardVector().Dot(player->GetPosition() - ped->GetPosition()) <= 0.0f) {
                continue;
            }
            if (player->GetForwardVector().Dot(ped->GetPosition() - player->GetPosition()) <= 0.0f) {
                continue;
            }
            if ((ped->GetPosition() - FindPlayerCoors()).SquaredMagnitude() >= sq(MAX_DIST_TO_PLAYER)) {
                continue;
            }

            const auto isEnemyGangMember = ped->IsGangster() && ped->m_nPedType != PED_TYPE_GANG2;
            // NOTE/BUG: The upper bounds are exclusive, so `WEATHER` is never picked [0x43B341]
            m_Topic = (eTopic)(isEnemyGangMember ? GetRandomNumberInRange(8, 10) : GetRandomNumberInRange(0, 7));
            if (!ped->GetSpeechAE().WillPedChatAboutTopic((int16)(m_Topic))) {
                continue;
            }

            if (const auto opening = GetOpeningAboutTopic(ped, player, m_Topic); opening && CAEPedSpeechAudioEntity::RequestPlayerConversation(ped)) { // 0x43B99B
                m_PositiveOpening = opening->IsPositive;
                ped->Say(opening->Line, 0, 1.0f, true);

                m_State = eP2pState::PEDHASOPENED;
                m_pPed  = ped;
                CEntity::RegisterReference(m_pPed);
                m_TimeOfLastPlayerConversation = now;
                m_StartTime                    = now;
                ped->DisablePedSpeech(false);
                g_ikChainMan.LookAt("Ped2Pl_Conversation", ped, player, LOOK_AT_TIME_MS, BONE_HEAD, nullptr, false, 0.25f, 500, LOOK_AT_PRIORITY, false);
                g_ikChainMan.LookAt("Ped2Pl_ConversationP", player, ped, LOOK_AT_TIME_MS, BONE_HEAD, nullptr, false, 0.25f, 500, LOOK_AT_PRIORITY, false);
                return;
            }

            if ((CGeneral::GetRandomNumber() & 0xFFF) == 3) { // 0x43B94E
                ped->Say(CTX_GLOBAL_EYEING_PLAYER);
                return;
            }
        }
        return;
    }
    case eP2pState::PEDHASOPENED: { // 0x43BA38
        if (!m_pPed) {
            return EndConversation();
        }

        if (now > m_StartTime + PLAYER_REPLY_TIMEOUT_MS) { // 0x43BA8C
            m_State = eP2pState::INACTIVE;
            CAEPedSpeechAudioEntity::ReleasePlayerConversation();
            m_pPed->EnablePedSpeech();
            g_ikChainMan.AbortLookAtIfLooking(player);
            if (g_ikChainMan.IsLooking(m_pPed)) {
                g_ikChainMan.AbortLookAt(m_pPed);
                if (m_PositiveOpening) { // NOTE: The ped only complains if it was still looking at the player
                    m_pPed->Say(CTX_GLOBAL_CONV_IGNORED);
                }
            }
            return;
        }

        const auto pedIsFemale = IsPedTypeFemale(m_pPed->m_nPedType);

        if (CPad::GetPad()->ConversationYesJustDown()) { // 0x43BB2E
            if (!m_PositiveOpening) {                    // 0x43BE19
                player->Say(pedIsFemale ? CTX_GLOBAL_LIKE_DISMISS_FEMALE : CTX_GLOBAL_LIKE_DISMISS_MALE, 0, 1.0f, true);
            } else {
                switch (m_Topic) {
                case eTopic::CAR: // 0x43BB4F
                    player->Say(CTX_GLOBAL_LIKE_CAR_REPLY, 0, 1.0f, true);
                    break;
                case eTopic::CLOTHES: // 0x43BB63
                    player->Say(CTX_GLOBAL_LIKE_CLOTHES_REPLY, 0, 1.0f, true);
                    break;
                case eTopic::HAIR: // 0x43BB77
                    player->Say(CTX_GLOBAL_LIKE_HAIR_REPLY, 0, 1.0f, true);
                    break;
                case eTopic::PHYSIQUE: // 0x43BB8B
                    player->Say(CTX_GLOBAL_LIKE_PHYS_REPLY, 0, 1.0f, true);
                    break;
                case eTopic::SHOES: // 0x43BB9F
                    player->Say(CTX_GLOBAL_LIKE_SHOES_REPLY, 0, 1.0f, true);
                    break;
                case eTopic::SMELL: // 0x43BBB3
                    player->Say(CTX_GLOBAL_LIKE_SMELL_REPLY, 0, 1.0f, true);
                    break;
                case eTopic::TATTOO: // 0x43BBC7
                    player->Say(CTX_GLOBAL_LIKE_TATTOO_REPLY, 0, 1.0f, true);
                    break;
                case eTopic::WEATHER: // 0x43BBDB
                    player->Say(CTX_GLOBAL_WEATHER_LIKE_REPLY, 0, 1.0f, true);
                    break;
                case eTopic::WHERE_YOU_FROM: // 0x43BBEF
                case eTopic::GANGBANG:       // 0x43BCF8
                    player->Say(m_Topic == eTopic::WHERE_YOU_FROM ? CTX_GLOBAL_WHERE_YOU_FROM_POS : CTX_GLOBAL_GANGBANG_YES, 0, 1.0f, true);
                    if (GetRandomNumberInRange(0, 100) < 75) {
                        MakePedAttackPlayer(player);
                    }
                    return EndConversation();
                default: // 0x43BE4F
                    break;
                }
            }
            m_State     = eP2pState::WAITINGTOFINISH;
            m_StartTime = now;
            return;
        }

        if (CPad::GetPad()->ConversationNoJustDown()) { // 0x43BE7E
            if (!m_PositiveOpening) {                   // 0x43BF1B
                player->Say(pedIsFemale ? CTX_GLOBAL_LIKE_NEGATIVE_FEMALE : CTX_GLOBAL_LIKE_NEGATIVE_MALE, 0, 1.0f, true);
                if (m_pPed->m_nPedType != PED_TYPE_GANG2 && GetRandomNumberInRange(0, 100) < 40) {
                    MakePedAttackPlayer(player);
                }
                m_State     = eP2pState::WAITINGTOFINISH;
                m_StartTime = now;
                return;
            }
            switch (m_Topic) {
            case eTopic::WHERE_YOU_FROM: // 0x43BF00
                player->Say(CTX_GLOBAL_WHERE_YOU_FROM_NEG, 0, 1.0f, true);
                return EndConversation();
            case eTopic::GANGBANG: // 0x43BEE8
                player->Say(CTX_GLOBAL_GANGBANG_NO, 0, 1.0f, true);
                return EndConversation();
            default: // 0x43BE97
                player->Say(pedIsFemale ? CTX_GLOBAL_LIKE_DISMISS_FEMALE : CTX_GLOBAL_LIKE_DISMISS_MALE, 0, 1.0f, true);
                m_State     = eP2pState::WAITINGFORFINALWORD;
                m_StartTime = now;
                return;
            }
        }
        return;
    }
    case eP2pState::WAITINGFORFINALWORD: { // 0x43C086
        if (!m_pPed) {
            return EndConversation();
        }
        if (now > m_StartTime + FINAL_WORD_DELAY_MS && !m_pPed->GetPedTalking()) {
            m_pPed->Say(CTX_GLOBAL_LIKE_DISMISS_REPLY, 0, 1.0f, true);
            m_State     = eP2pState::WAITINGTOFINISH;
            m_StartTime = now;
        }
        return;
    }
    case eP2pState::WAITINGTOFINISH: { // 0x43C0CE
        if (now > m_StartTime + FINISH_DELAY_MS || !m_pPed) {
            EndConversation();
        }
        return;
    }
    default: // 0x43C0F1
        return;
    }
}

// 0x43AB10
void CPedToPlayerConversations::EndConversation() {
    m_State = eP2pState::INACTIVE;

    CAEPedSpeechAudioEntity::ReleasePlayerConversation();

    if (m_pPed) {
        m_pPed->EnablePedSpeech();
    }

    g_ikChainMan.AbortLookAtIfLooking(FindPlayerPed());

    if (m_pPed) {
        g_ikChainMan.AbortLookAtIfLooking(m_pPed);
    }
}

void CPedToPlayerConversations::InjectHooks() {
    RH_ScopedClass(CPedToPlayerConversations);
    RH_ScopedCategory("Conversations");

    RH_ScopedInstall(Clear, 0x43AAE0);
    RH_ScopedInstall(Update, 0x43B0F0);
    RH_ScopedInstall(EndConversation, 0x43AB10);
}
