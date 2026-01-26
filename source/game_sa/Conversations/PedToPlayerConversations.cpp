#include "PedToPlayerConversations.h"
#include "Conversations.h"
#include "IKChainManager_c.h"
#include "PedClothesDesc.h"
#include "TaskSimpleStandStill.h"

#include <reversiblebugfixes/Bugs.hpp>

void CPedToPlayerConversations::InjectHooks() {
    RH_ScopedClass(CPedToPlayerConversations);
    RH_ScopedCategory("Conversations");

    RH_ScopedInstall(Clear, 0x43AAE0);
    RH_ScopedInstall(Update, 0x43B0F0);
    RH_ScopedInstall(EndConversation, 0x43AB10);
}

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

// 0x43B0F0
void CPedToPlayerConversations::Update() {
    static CVehicle* lastVehicle; // 0x969A40
    static int32 randomPed; // 0x969A3C

    if (const auto vehicle = FindPlayerVehicle()) {
        lastVehicle = vehicle;
        CEntity::RegisterReference(lastVehicle);
    }

    const auto player = FindPlayerPed();
    switch (m_State) {
    case eP2pState::INACTIVE: {
        if (CTimer::GetTimeInMS() <= m_TimeOfLastPlayerConversation + 30'000) {
            return;
        }

        if (FindPlayerVehicle()) {
            return;
        }

        if (player->GetWantedLevel() != 0 || !player->PedIsReadyForConversation(true)) {
            return;
        }

        if (CConversations::IsConversationGoingOn()) {
            return;
        }

        if (notsa::bugfixes::CPedToPlayerConversations_Update_DontWithGangWarn) {
            // Added in Mobile:
            if (CGangWars::bGangWarsActive) {
                return;
            }
        }
        
        for (auto tries = 0; tries < 8; tries++) {
            if (++randomPed >= (int32)GetPedPool()->GetSize()) {
                // rewind
                randomPed = 0;
            }

            const auto ped = GetPedPool()->GetAt(randomPed);
            if (!ped || ped->m_nCreatedBy != PED_GAME || ped->bHasAScriptBrain || !ped->GetIsOnScreen()) {
                continue;
            }

            const auto &playerPos = player->GetPosition(), &pedPos = ped->GetPosition();

            if (DistanceBetweenPoints(playerPos, pedPos) >= 7.0f) {
                continue;
            }

            if (ped->GetForwardVector().Dot(playerPos - pedPos) <= 0.0f) {
                continue;
            }

            if (player->GetForwardVector().Dot(pedPos - playerPos) <= 0.0f) {
                continue;
            }

            m_Topic = GetRandomTopicForPed(ped);
            if (!ped->GetSpeechAE().WillPedChatAboutTopic(static_cast<int16>(m_Topic))) {
                continue;
            }

            const auto RandomPedTalk = [ped] {
                if (CGeneral::RandomBool(1.0f / 4095.0f * 100.0f)) { // NOTSA, originally: rand() % 4096 == 3
                    ped->Say(CTX_GLOBAL_EYEING_PLAYER);
                }
            };

            const auto CalculateStatRespect = [ped](eStats stat, uint32 modulo, float sub) {
                return (uint32)(CStats::GetStatValue(stat) + (float)(ped->m_nRandomSeed % modulo) - sub);
            };

            // shorthands, these were called from each topic's procedure.
            if (!CAEPedSpeechAudioEntity::RequestPlayerConversation(ped)) {
                RandomPedTalk();
                continue;
            }

            switch (m_Topic) {
            case eTopic::CAR: {
                if (!lastVehicle || DistanceBetweenPoints2D(lastVehicle->GetPosition(), ped->GetPosition()) >= 20.0f) {
                    RandomPedTalk();
                    continue;
                }

                switch (lastVehicle->GetModelInfo()->m_n2dfxCount) {
                case 1:
                case 4:
                case 5:
                case 6:
                    m_bPositiveOpening = false;
                    ped->Say(CTX_GLOBAL_CONV_DISL_CAR);
                    break;
                case 2:
                case 3:
                case 9:
                    if (lastVehicle->m_fHealth <= 800.0f) {
                        RandomPedTalk();
                        continue;
                    }

                    m_bPositiveOpening = true;
                    ped->Say(CTX_GLOBAL_CONV_LIKE_CAR);
                    break;
                default:
                    RandomPedTalk();
                    continue;
                }
                break;
            }
            case eTopic::CLOTHES: {
                const auto respect = CalculateStatRespect(STAT_CLOTHES_RESPECT, 512, 256.0f);
                if (respect > 500) {
                    m_bPositiveOpening = true;
                    ped->Say(CTX_GLOBAL_CONV_LIKE_CLOTHES);
                } else if (respect < 150) {
                    m_bPositiveOpening = false;
                    ped->Say(CTX_GLOBAL_CONV_DISL_CLOTHES);
                }

                break;
            }
            case eTopic::HAIR: {
                const auto desc = player->GetClothesDesc();
                if (!player->GetClothesDesc()->HasVisibleNewHairCut(0)) {
                    RandomPedTalk();
                    continue;
                }

                const auto respect = CalculateStatRespect(STAT_CLOTHES_RESPECT, 512, 256.0f);
                if (respect > 500) {
                    m_bPositiveOpening = true;
                    ped->Say(CTX_GLOBAL_CONV_LIKE_HAIR);
                } else if (respect < 150 || CStats::GetStatValue(STAT_HAIRDRESSING_BUDGET) < 5.0f) {
                    m_bPositiveOpening = false;
                    ped->Say(CTX_GLOBAL_CONV_DISL_HAIR);
                }
                break;
            }
            case eTopic::PHYSICS: {
                const auto muscle = CalculateStatRespect(STAT_MUSCLE, 256, 128.0f), fat = CalculateStatRespect(STAT_FAT, 256, 128.0f);

                if (muscle > 400 && fat < 250) {
                    m_bPositiveOpening = true;
                    ped->Say(CTX_GLOBAL_CONV_LIKE_PHYS);
                } else if (muscle - fat < 200 && fat > 500) {
                    m_bPositiveOpening = false;
                    ped->Say(CTX_GLOBAL_CONV_DISL_PHYS);
                }
                break;
            }
            case eTopic::SHOES: {
                const auto respect = CalculateStatRespect(STAT_CLOTHES_RESPECT, 1'024, 512.0f);
                if (respect > 700) {
                    m_bPositiveOpening = true;
                    ped->Say(CTX_GLOBAL_CONV_LIKE_SHOES);
                } else if (respect < 300) {
                    m_bPositiveOpening = false;
                    ped->Say(CTX_GLOBAL_CONV_DISL_SHOES);
                }
                break;
            }
            case eTopic::SMELL: {
                const auto respect = CalculateStatRespect(STAT_SEX_APPEAL, 512, 256.0f);
                if (respect > 700) {
                    m_bPositiveOpening = true;
                    ped->Say(CTX_GLOBAL_CONV_LIKE_SMELL);
                } else if (respect < 300) {
                    m_bPositiveOpening = false;
                    ped->Say(CTX_GLOBAL_CONV_DISL_SMELL);
                }
                break;
            }
            case eTopic::TATTOO: {
                if (!player->GetClothesDesc()->HasVisibleTattoo()) {
                    RandomPedTalk();
                    continue;
                }

                const auto respect = CGeneral::GetRandomNumberInRange(0, 1'000);
                if (respect > 700) {
                    m_bPositiveOpening = true;
                    ped->Say(CTX_GLOBAL_CONV_LIKE_TATTOO);
                } else if (respect < 300) {
                    m_bPositiveOpening = false;
                    ped->Say(CTX_GLOBAL_CONV_DISL_TATTOO);
                }
                break;
            }
            case eTopic::WEATHER: {
                if (CWeather::Wind > 0.5f) {
                    m_bPositiveOpening = false;
                    ped->Say(CTX_GLOBAL_CONV_DISL_WEATHER);
                } else if (CWeather::Wind < 0.1f && CClock::GetGameClockHours() < 20 && CClock::GetGameClockHours() > 6) {
                    m_bPositiveOpening = true;
                    ped->Say(CTX_GLOBAL_CONV_LIKE_WEATHER);
                } else {
                    RandomPedTalk();
                    continue;
                }
                break;
            }
            case eTopic::WHERE_ARE_YOU_FROM:
                ped->Say(CTX_GLOBAL_WHERE_YOU_FROM);
                break;
            case eTopic::GANGBANG:
                m_bPositiveOpening = true;
                ped->Say(CTX_GLOBAL_GANGBANG);
                break;
            default:
                RandomPedTalk();
                continue;
            }

            // Ped said something to the player, change the conversation state.
            m_State = eP2pState::PEDHASOPENED;
            m_TimeOfLastPlayerConversation = m_StartTime = CTimer::GetTimeInMS();

            m_pPed = ped;
            CEntity::RegisterReference(m_pPed);
            ped->DisablePedSpeech(false);

            g_ikChainMan.LookAt("Ped2Pl_Conversation", ped, player, 100'000, BONE_HEAD, nullptr, false, 0.25f, 500, 8, false);

            g_ikChainMan.LookAt("Ped2Pl_ConversationP", player, ped, 100'000, BONE_HEAD, nullptr, false, 0.25f, 500, 8, false);
            break; // Found a ped for talking, Update()'s job is done. originally return;
        }
        break;
    }
    case eP2pState::PEDHASOPENED: {
        if (!m_pPed) {
            return EndConversation();
        }

        if (CTimer::GetTimeInMS() > m_StartTime + 4'000) {
            if (g_ikChainMan.IsLooking(m_pPed) && m_bPositiveOpening) {
                m_pPed->Say(CTX_GLOBAL_CONV_IGNORED);
            }
            return EndConversation();
        }

        const auto MakePedAttackPlayer = [player] {
            CTaskComplexSequence taskSequence{};
            taskSequence.AddTask(new CTaskSimpleStandStill(2'000));
            taskSequence.AddTask(new CTaskComplexKillPedOnFoot(player, -1, 0, 0, 0, 0));

            m_pPed->GetIntelligence()->m_eventGroup.Add(CEventScriptCommand{ 3, taskSequence.AsComplex() });
        };

        if (CPad::GetPad()->ConversationYesJustDown()) {
            if (!m_bPositiveOpening) {
                // responding catcalling
                player->Say(IsPedTypeFemale(m_pPed->m_nPedType) ? CTX_GLOBAL_LIKE_DISMISS_FEMALE : CTX_GLOBAL_LIKE_DISMISS_MALE);

                m_State     = eP2pState::WAITINGTOFINISH;
                m_StartTime = CTimer::GetTimeInMS();
                break;
            }

            // Player replies...
            switch (m_Topic) {
            case eTopic::CAR:
                player->Say(CTX_GLOBAL_LIKE_CAR_REPLY);
                break;
            case eTopic::CLOTHES:
                player->Say(CTX_GLOBAL_LIKE_CLOTHES_REPLY);
                break;
            case eTopic::HAIR:
                player->Say(CTX_GLOBAL_LIKE_HAIR_REPLY);
                break;
            case eTopic::PHYSICS:
                player->Say(CTX_GLOBAL_LIKE_PHYS_REPLY);
                break;
            case eTopic::SHOES:
                player->Say(CTX_GLOBAL_LIKE_SHOES_REPLY);
                break;
            case eTopic::SMELL:
                player->Say(CTX_GLOBAL_LIKE_SMELL_REPLY);
                break;
            case eTopic::TATTOO:
                player->Say(CTX_GLOBAL_LIKE_TATTOO_REPLY);
                break;
            case eTopic::WEATHER:
                player->Say(CTX_GLOBAL_WEATHER_LIKE_REPLY);
                break;
            case eTopic::WHERE_ARE_YOU_FROM:
            case eTopic::GANGBANG:           {
                // Player always dismisses calls from enemy gangsters
                player->Say(m_Topic == eTopic::WHERE_ARE_YOU_FROM ? CTX_GLOBAL_WHERE_YOU_FROM_POS : CTX_GLOBAL_GANGBANG_YES);

                if (CGeneral::RandomBool(75.0f)) {
                    MakePedAttackPlayer();
                }
                return EndConversation();
            }
            default:
                break;
            }

            m_State     = eP2pState::WAITINGTOFINISH;
            m_StartTime = CTimer::GetTimeInMS();
        } else if (CPad::GetPad()->ConversationNoJustDown()) {
            if (m_bPositiveOpening) {
                // Player dismisses the ped.
                switch (m_Topic) {
                case eTopic::WHERE_ARE_YOU_FROM:
                case eTopic::GANGBANG:
                    player->Say(m_Topic == eTopic::WHERE_ARE_YOU_FROM ? CTX_GLOBAL_WHERE_YOU_FROM_NEG : CTX_GLOBAL_GANGBANG_NO);
                    return EndConversation();
                default:
                    player->Say(IsPedTypeFemale(m_pPed->m_nPedType) ? CTX_GLOBAL_LIKE_DISMISS_FEMALE : CTX_GLOBAL_LIKE_DISMISS_MALE);
                    break;
                }

                m_State     = eP2pState::WAITINGFORFINALWORD;
                m_StartTime = CTimer::GetTimeInMS();
                break;
            }

            // Player curses the ped.
            player->Say(IsPedTypeFemale(m_pPed->m_nPedType) ? CTX_GLOBAL_LIKE_NEGATIVE_FEMALE : CTX_GLOBAL_LIKE_NEGATIVE_MALE);

            if (m_pPed->m_nPedType != PED_TYPE_GANG2 && CGeneral::RandomBool(40.0f)) {
                MakePedAttackPlayer();
            }

            m_State     = eP2pState::WAITINGTOFINISH;
            m_StartTime = CTimer::GetTimeInMS();
        }

        break;
    }
    case eP2pState::WAITINGFORFINALWORD:
        if (!m_pPed) {
            return EndConversation();
        }

        if (CTimer::GetTimeInMS() > m_StartTime + 3'000 && !m_pPed->GetPedTalking()) {
            m_pPed->Say(CTX_GLOBAL_LIKE_DISMISS_REPLY);
            m_State     = eP2pState::WAITINGTOFINISH;
            m_StartTime = CTimer::GetTimeInMS();
        }
        break;
    case eP2pState::WAITINGTOFINISH:
        if (!m_pPed || CTimer::GetTimeInMS() > m_StartTime + 2'500) {
            return EndConversation();
        }
        break;
    default:
        break;
    }
}

// 0x43AB10
void CPedToPlayerConversations::EndConversation() {
    CPlayerPed* playerPed = FindPlayerPed();
    
    m_State = eP2pState::INACTIVE;
    CAEPedSpeechAudioEntity::ReleasePlayerConversation();
    if (m_pPed != nullptr) {
        m_pPed->EnablePedSpeech();
    
        if (g_ikChainMan.IsLooking(playerPed)) {
            g_ikChainMan.AbortLookAt(playerPed);
        }
    }
    
    if (m_pPed != nullptr) {
        if (g_ikChainMan.IsLooking(m_pPed)) {
            g_ikChainMan.AbortLookAt(m_pPed);
        }
    }
}
