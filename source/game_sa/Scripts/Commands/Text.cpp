#include <StdInc.h>

#include "./Commands.hpp"
#include <CommandParser/Parser.hpp>
#include "Messages.h"
#include "Hud.h"
#include "CommandParser/Parser.hpp"

/*!
* Various text commands
*/

void ClearSmallPrints() {
    CMessages::ClearSmallMessagesOnly();
}

void ClearHelp() {
    CHud::SetHelpMessage(nullptr, true, false, false);
}

void FlashHudObject(eHudItem item) {
    CHud::m_ItemToFlash = item;
}

// NOTSA: time: int32 -> uint32
void PrintBig(const char* key, uint32 time, uint32 flags) {
    const auto text = TheText.Get(key);
    CMessages::AddBigMessage(text, time, static_cast<eMessageStyle>(flags - 1));
}

void Print(const char* key, uint32 time, uint32 flags) {
    const auto text = TheText.Get(key);
    if (!text || strncmp(AsciiFromGxtChar(text), "~z~", 3u) != 0 || FrontEndMenuManager.m_bShowSubtitles)
        CMessages::AddMessageQ(text, time, flags, CTheScripts::bAddNextMessageToPreviousBriefs);
    CTheScripts::bAddNextMessageToPreviousBriefs = true;
}

void PrintNow(const char* key, uint32 time, uint32 flags) {
    const auto text = TheText.Get(key);
    if (!text || strncmp(AsciiFromGxtChar(text), "~z~", 3u) != 0 || FrontEndMenuManager.m_bShowSubtitles)
        CMessages::AddMessageJump(text, time, flags, CTheScripts::bAddNextMessageToPreviousBriefs);
    CTheScripts::bAddNextMessageToPreviousBriefs = true;
}

void ClearThisPrintBigNow(uint16 item) {
    CMessages::ClearThisPrintBigNow(static_cast<eMessageStyle>(item - 1));
}

void ClearPrints() {
    CMessages::ClearMessages(false);
}

// COMMAND_USE_TEXT_COMMANDS - 0x48961F
void UseTextCommands(bool state) {
    using enum CTheScripts::eUseTextCommandState;
    CTheScripts::UseTextCommands = state ? ENABLED_BY_SCRIPT : DISABLE_NEXT_FRAME;
}

// COMMAND_DISPLAY_TEXT - 0x481A0C
void DisplayText(CVector2D pos, const char* gxtKey) {
    assert(CTheScripts::UseTextCommands != CTheScripts::eUseTextCommandState::DISABLED);

    auto* const line = &CTheScripts::IntroTextLines[CTheScripts::NumberOfIntroTextLinesThisFrame++];
    line->m_Pos      = pos;
    line->param1     = -1;
    line->param2     = -1;
    strcpy_s(line->m_szGxtEntry, gxtKey);
}

void notsa::script::commands::text::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_SMALL_PRINTS, ClearSmallPrints);
    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_HELP, ClearHelp);
    REGISTER_COMMAND_HANDLER(COMMAND_FLASH_HUD_OBJECT, FlashHudObject);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_BIG, PrintBig);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT, Print);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_NOW, PrintNow);
    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_THIS_PRINT_BIG_NOW, ClearThisPrintBigNow);
    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_PRINTS, ClearPrints);
    REGISTER_COMMAND_HANDLER(COMMAND_DISPLAY_TEXT, DisplayText);
}
