#include <StdInc.h>

#include "Commands.hpp"
#include <CommandParser/Parser.hpp>
#include "Conversations.h"

using namespace notsa::script;


/*!
* Various conversation commands
*/
namespace {

/// START_SETTING_UP_CONVERSATION(0717)
void StartSettingUpConversation(CPed& ped) {
    CConversations::StartSettingUpConversation(&ped);
}

/// FINISH_SETTING_UP_CONVERSATION(0719)
void FinishSettingUpConversation() {
    CConversations::DoneSettingUpConversation(false);
}

/// IS_CONVERSATION_AT_NODE(071A)
bool IsConversationAtNode(CPed& ped, const char* nodeName) {
    return CConversations::IsConversationAtNode(nodeName, &ped);
}

/// IS_PLAYER_IN_POSITION_FOR_CONVERSATION(089B)
bool IsPlayerInPositionForConversation(CPed& ped) {
    return CConversations::IsPlayerInPositionForConversation(&ped, false);
}

/// ENABLE_CONVERSATION(089C)
void EnableConversation(CPed& ped, bool state) {
    CConversations::EnableConversation(&ped, state);
}

/// CLEAR_CONVERSATION_FOR_CHAR(08ED)
void ClearConversationForChar(CPed* ped) {
    if (ped) {
        CConversations::RemoveConversationForPed(ped);
    }
}

/// SET_UP_CONVERSATION_NODE_WITH_SPEECH(09A4)
void SetUpConversationNodeWithSpeech(const char* questionKey, const char* answerYesKey, const char* answerNoKey, int32 questionWAV, int32 answerYesWAV, int32 answerNoWAV) {
    CConversations::SetUpConversationNode(questionKey, answerYesKey, answerNoKey, questionWAV, answerYesWAV, answerNoWAV);
}

/// SET_UP_CONVERSATION_END_NODE_WITH_SPEECH(09AA)
void SetUpConversationEndNodeWithSpeech(const char* questionKey, int32 questionWAV) {
    CConversations::SetUpConversationNode(questionKey, nullptr, nullptr, questionWAV, 0, 0);
}

/// FINISH_SETTING_UP_CONVERSATION_NO_SUBTITLES(0A47)
void FinishSettingUpConversationNoSubtitles() {
    CConversations::DoneSettingUpConversation(true);
}

/// SET_UP_CONVERSATION_NODE_WITH_SCRIPTED_SPEECH(0A18)
void SetUpConversationNodeWithScriptedSpeech(const char* questionKey, const char* answerYesKey, const char* answerNoKey, int32 questionWAV, int32 answerYesWAV, int32 answerNoWAV) {
    CConversations::SetUpConversationNode(questionKey, answerYesKey, answerNoKey, -questionWAV, -answerYesWAV, -answerNoWAV);
}

/// SET_UP_CONVERSATION_END_NODE_WITH_SCRIPTED_SPEECH(0A3C)
void SetUpConversationEndNodeWithScriptedSpeech(const char* questionKey, int32 questionWAV) {
    CConversations::SetUpConversationNode(questionKey, nullptr, nullptr, -questionWAV, 0, 0);
}

} // namespace

void notsa::script::commands::conversation::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER_BEGIN("Conversation");

    REGISTER_COMMAND_HANDLER(COMMAND_IS_CONVERSATION_AT_NODE, IsConversationAtNode);
    REGISTER_COMMAND_HANDLER(COMMAND_ENABLE_CONVERSATION, EnableConversation);
    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_CONVERSATION_FOR_CHAR, ClearConversationForChar);
    REGISTER_COMMAND_HANDLER(COMMAND_START_SETTING_UP_CONVERSATION, StartSettingUpConversation);
    REGISTER_COMMAND_HANDLER(COMMAND_FINISH_SETTING_UP_CONVERSATION, FinishSettingUpConversation);
    REGISTER_COMMAND_HANDLER(COMMAND_FINISH_SETTING_UP_CONVERSATION_NO_SUBTITLES, FinishSettingUpConversationNoSubtitles);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_UP_CONVERSATION_NODE_WITH_SPEECH, SetUpConversationNodeWithSpeech);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_PLAYER_IN_POSITION_FOR_CONVERSATION, IsPlayerInPositionForConversation);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_UP_CONVERSATION_END_NODE_WITH_SPEECH, SetUpConversationEndNodeWithSpeech);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_UP_CONVERSATION_NODE_WITH_SCRIPTED_SPEECH, SetUpConversationNodeWithScriptedSpeech);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_UP_CONVERSATION_END_NODE_WITH_SCRIPTED_SPEECH, SetUpConversationEndNodeWithScriptedSpeech);
}
