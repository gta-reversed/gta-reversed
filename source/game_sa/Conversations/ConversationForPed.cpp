#include "StdInc.h"
#include "ConversationForPed.h"
#include "Conversations.h"

constexpr uint32 CONVERSATION_MESSAGE_TIME_MS = 4'000;
constexpr uint32 PLAYER_SPEAKING_TIME_MS      = 4'000;
constexpr uint32 PED_SPEAKING_TIME_MS         = 4'000;
constexpr uint32 MIN_TIME_BEFORE_ANSWER_MS    = 400;
constexpr uint32 RESTART_AFTER_AWAY_FOR_MS    = 10'000;

void CConversationForPed::InjectHooks() {
    RH_ScopedClass(CConversationForPed);
    RH_ScopedCategory("Conversations");

    RH_ScopedInstall(Clear, 0x43A770);
    RH_ScopedInstall(Update, 0x43C190);
    RH_ScopedInstall(IsPlayerInPositionForConversation, 0x43AC40);
}

// 0x43A770
void CConversationForPed::Clear(bool dontClearNodes) {
    if (!dontClearNodes) {
        CConversations::m_Nodes[m_FirstNode].ClearRecursively();
    }
    m_FirstNode                 = -1;
    m_CurrentNode               = -1;
    m_pPed                      = nullptr;
    m_LastChange                = 0;
    m_LastTimeWeWereCloseEnough = 0;
}

// 0x43C190
void CConversationForPed::Update() {
    if (!m_pPed || !m_Enabled || !IsPlayerInPositionForConversation(false)) {
        m_Status = INACTIVE;
        return;
    }

    //! Positive: a speech context, negative: an audio event
    const auto Say = [](CPed* speaker, int32 speech) {
        if (speech > 0) {
            speaker->Say((eGlobalSpeechContext)(speech));
        }
        if (speech < 0) {
            CConversations::AwkwardSay((eAudioEvents)(-speech), speaker);
        }
    };

    const auto  now = CTimer::GetTimeInMS();
    auto* const pad = CPad::GetPad(0);
    if (now > m_LastTimeWeWereCloseEnough + RESTART_AFTER_AWAY_FOR_MS || m_Status == PLAYER_SPEAKING && (now > m_LastChange + PLAYER_SPEAKING_TIME_MS || pad->ConversationNoJustDown() || pad->ConversationYesJustDown())) { // 0x43C4C6
        const auto& node = GetCurrentNode();
        if (!m_SuppressSubtitles) {
            CMessages::ClearSmallMessagesOnly();
            CMessages::AddMessageJump(TheText.Get(node.m_Name), CONVERSATION_MESSAGE_TIME_MS, 1, true);
        }
        Say(m_pPed, node.m_Speech);
        m_Status     = PED_SPEAKING;
        m_LastChange = now;
    } else if (now > m_LastChange + MIN_TIME_BEFORE_ANSWER_MS) { // 0x43C22E
        switch (m_Status) {
        case INACTIVE:
        case PED_SPEAKING:
        case WAITINGFORINPUT: {
            const auto PlayerAnswers = [&](int16 nextNode, const char* keySuffix, int32 speech) {
                if (!m_SuppressSubtitles) {
                    CMessages::ClearSmallMessagesOnly();
                    strcpy_s(gString, GetCurrentNode().m_Name);
                    strcat_s(gString, keySuffix);
                    CMessages::AddMessageJump(TheText.Get(gString), CONVERSATION_MESSAGE_TIME_MS, 3, true);
                }
                Say(FindPlayerPed(), speech);
                m_CurrentNode = nextNode;
                m_LastChange  = now;
                if (!m_SuppressSubtitles) {
                    CMessages::AddMessageQ(TheText.Get(GetCurrentNode().m_Name), CONVERSATION_MESSAGE_TIME_MS, 1, true);
                }
                m_Status = PLAYER_SPEAKING;
            };
            const auto& node = GetCurrentNode();
            if (pad->ConversationNoJustDown() && node.m_NodeNo >= 0) { // 0x43C25C
                PlayerAnswers(node.m_NodeNo, "N", node.m_SpeechN);
            } else if (pad->ConversationYesJustDown() && node.m_NodeYes >= 0) { // 0x43C391
                PlayerAnswers(node.m_NodeYes, "Y", node.m_SpeechY);
            }
            break;
        }
        case PLAYER_SPEAKING: // 0x43C23D
            break;
        default:
            NOTSA_UNREACHABLE_CASE(m_Status);
        }
    }
    m_LastTimeWeWereCloseEnough = now;

    if (m_Status == PED_SPEAKING && now > m_LastChange + PED_SPEAKING_TIME_MS) {
        m_Status = WAITINGFORINPUT;
    }
}

// 0x43AC40
bool CConversationForPed::IsPlayerInPositionForConversation(bool isRandomConversation) {
    // NOTE: 3D distance (`CVector::DistSqr` is 2D)
    if ((FindPlayerCoors() - m_pPed->GetPosition()).SquaredMagnitude() > sq(4.f)) {
        return false;
    }

    auto* const player = FindPlayerPed();

    // NOTE: Checks that they don't look in the same direction, not that they face each other
    if (player->GetForward().Dot(m_pPed->GetForward()) > 0.f) {
        return false;
    }

    if (player->GetMoveSpeed().SquaredMagnitude2D() > sq(0.01f)) {
        return false;
    }

    // Check if ped was damaged by the player in the last 6 seconds
    if (m_pPed->m_pLastEntityDamage == player && CTimer::GetTimeInMS() < (uint32)(m_pPed->field_768) + 6'000) { // NOTE: `field_768` = time of the last damage
        return false;
    }

    if (isRandomConversation && player->GetPlayerGroup().GetMembership().CountMembersExcludingLeader() >= 1) {
        return false;
    }

    return player->PedIsReadyForConversation(isRandomConversation)
        && m_pPed->PedIsReadyForConversation(isRandomConversation);
}

// NOTSA
CConversationNode& CConversationForPed::GetCurrentNode() const {
    return CConversations::m_Nodes[m_CurrentNode];
}
