#include <StdInc.h>

#include "./Commands.hpp"
#include <CommandParser/Parser.hpp>
#include "Messages.h"
#include "Hud.h"
#include "CommandParser/Parser.hpp"

/*!
* Various text commands
*/

namespace {
void ClearSmallPrints() {
    CMessages::ClearSmallMessagesOnly();
}

// COMMAND_PRINT_HELP - 0x485601
void PrintHelp(CRunningScript& S, const char* key) {
    if (CTheScripts::bDisplayNonMiniGameHelpMessages || S.m_IsTextBlockOverride || !CTheScripts::bMiniGameInProgress) {
        const auto* const text = TheText.Get(key);
        CHud::SetHelpMessage(text);
        if (CTheScripts::bAddNextMessageToPreviousBriefs) {
            CMessages::AddToPreviousBriefArray(text);
        }
    }
    CTheScripts::bAddNextMessageToPreviousBriefs = true;
}

// COMMAND_CLEAR_HELP - 0x485672
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
    if (!text || strncmp(AsciiFromGxtChar(text), "~z~", 3u) != 0 || FrontEndMenuManager.m_bShowSubtitles) {
        CMessages::AddMessageQ(text, time, flags, CTheScripts::bAddNextMessageToPreviousBriefs);
    }
    CTheScripts::bAddNextMessageToPreviousBriefs = true;
}

void PrintNow(const char* key, uint32 time, uint32 flags) {
    const auto text = TheText.Get(key);
    if (!text || strncmp(AsciiFromGxtChar(text), "~z~", 3u) != 0 || FrontEndMenuManager.m_bShowSubtitles) {
        CMessages::AddMessageJump(text, time, flags, CTheScripts::bAddNextMessageToPreviousBriefs);
    }
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

/* implementation */
void PrintWithNumbersNow(const char* key, int32 time, int32 flag, int32 n1, int32 n2 = -1, int32 n3 = -1, int32 n4 = -1, int32 n5 = -1, int32 n6 = -1) {
    CMessages::AddMessageJumpQWithNumber(
        TheText.Get(key),
        time,
        flag,
        n1,
        n2,
        n3,
        n4,
        n5,
        n6,
        CTheScripts::bAddNextMessageToPreviousBriefs
    );
    CTheScripts::bAddNextMessageToPreviousBriefs = true;
}

// COMMAND_PRINT_WITH_NUMBER_NOW - 0x47DAF5
void PrintWithNumberNow(const char* key, int32 n1, int32 time, int32 flag) {
    PrintWithNumbersNow(key, time, flag, n1);
}

// COMMAND_PRINT_WITH_2_NUMBERS_NOW
void PrintWith2NumbersNow(const char* key, int32 n1, int32 n2, int32 time, int32 flag) {
    PrintWithNumbersNow(key, time, flag, n1, n2);
}

// COMMAND_PRINT_WITH_3_NUMBERS_NOW
void PrintWith3NumbersNow(const char* key, int32 n1, int32 n2, int32 n3, int32 time, int32 flag) {
    PrintWithNumbersNow(key, time, flag, n1, n2, n3);
}

// COMMAND_PRINT_WITH_4_NUMBERS_NOW
void PrintWith4NumbersNow(const char* key, int32 n1, int32 n2, int32 n3, int32 n4, int32 time, int32 flag) {
    PrintWithNumbersNow(key, time, flag, n1, n2, n3, n4);
}

// COMMAND_PRINT_WITH_5_NUMBERS_NOW
void PrintWith5NumbersNow(const char* key, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 time, int32 flag) {
    PrintWithNumbersNow(key, time, flag, n1, n2, n3, n4, n5);
}

// COMMAND_PRINT_WITH_6_NUMBERS_NOW
void PrintWith6NumbersNow(const char* key, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6, int32 time, int32 flag) {
    PrintWithNumbersNow(key, time, flag, n1, n2, n3, n4, n5, n6);
}

/* Implementation */
void PrintWithNumbers(const char* key, int32 time, int32 flag, int32 n1, int32 n2 = -1, int32 n3 = -1, int32 n4 = -1, int32 n5 = -1, int32 n6 = -1) {
    CMessages::AddMessageWithNumberQ(
        TheText.Get(key),
        time,
        flag,
        n1,
        n2,
        n3,
        n4,
        n5,
        n6,
        CTheScripts::bAddNextMessageToPreviousBriefs
    );
    CTheScripts::bAddNextMessageToPreviousBriefs = true;
}

// COMMAND_PRINT_WITH_NUMBER - 0x47DA7A
void PrintWithNumber(const char* key, int32 n1, int32 time, int32 flag) {
    PrintWithNumbers(key, time, flag, n1);
}

// COMMAND_PRINT_WITH_2_NUMBERS
void PrintWith2Numbers(const char* key, int32 n1, int32 n2, int32 time, int32 flag) {
    PrintWithNumbers(key, time, flag, n1, n2);
}

// COMMAND_PRINT_WITH_3_NUMBERS
void PrintWith3Numbers(const char* key, int32 n1, int32 n2, int32 n3, int32 time, int32 flag) {
    PrintWithNumbers(key, time, flag, n1, n2, n3);
}

// COMMAND_PRINT_WITH_4_NUMBERS
void PrintWith4Numbers(const char* key, int32 n1, int32 n2, int32 n3, int32 n4, int32 time, int32 flag) {
    PrintWithNumbers(key, time, flag, n1, n2, n3, n4);
}

// COMMAND_PRINT_WITH_5_NUMBERS
void PrintWith5Numbers(const char* key, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 time, int32 flag) {
    PrintWithNumbers(key, time, flag, n1, n2, n3, n4, n5);
}

// COMMAND_PRINT_WITH_6_NUMBERS
void PrintWith6Numbers(const char* key, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6, int32 time, int32 flag) {
    PrintWithNumbers(key, time, flag, n1, n2, n3, n4, n5, n6);
}

/* Implementation */
void PrintWithNumbersBig(const char* key, int32 time, int16 style, int32 n1, int32 n2 = -1, int32 n3 = -1, int32 n4 = -1, int32 n5 = -1, int32 n6 = -1) {
    CMessages::AddBigMessageWithNumber(
        TheText.Get(key),
        time,
        (eMessageStyle)(style - 1),
        n1,
        n2,
        n3,
        n4,
        n5,
        n6
    );
}

// COMMAND_PRINT_WITH_NUMBER_BIG - 0x47DA14
void PrintWithNumberBig(const char* key, int32 time, int16 style, int32 n1) {
    PrintWithNumbersBig(key, time, style, n1);
}

// COMMAND_PRINT_WITH_2_NUMBERS_BIG
void PrintWith2NumbersBig(const char* key, int32 n1, int32 n2, int32 time, int16 style) {
    PrintWithNumbersBig(key, time, style, n1, n2);
}

// COMMAND_PRINT_WITH_3_NUMBERS_BIG
void PrintWith3NumbersBig(const char* key, int32 n1, int32 n2, int32 n3, int32 time, int16 style) {
    PrintWithNumbersBig(key, time, style, n1, n2, n3);
}

// COMMAND_PRINT_WITH_4_NUMBERS_BIG
void PrintWith4NumbersBig(const char* key, int32 n1, int32 n2, int32 n3, int32 n4, int32 time, int16 style) {
    PrintWithNumbersBig(key, time, style, n1, n2, n3, n4);
}

// COMMAND_PRINT_WITH_5_NUMBERS_BIG
void PrintWith5NumbersBig(const char* key, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 time, int16 style) {
    PrintWithNumbersBig(key, time, style, n1, n2, n3, n4, n5);
}

// COMMAND_PRINT_WITH_6_NUMBERS_BIG
void PrintWith6NumbersBig(const char* key, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6, int32 time, int16 style) {
    PrintWithNumbersBig(key, time, style, n1, n2, n3, n4, n5, n6);
}

// COMMAND_PRINT_BIG_Q - 0x47E743
void PrintBigQ(const char* key, int32 time, int16 style) {
    CMessages::AddBigMessageQ(TheText.Get(key), time, (eMessageStyle)(style - 1));
}

// COMMAND_PRINT_HELP_FOREVER_WITH_NUMBER - 0x48CBEB
void PrintHelpForeverWithNumber(CRunningScript& S, const char* key, int32 n1) {
    if (CTheScripts::bDisplayNonMiniGameHelpMessages || S.m_IsTextBlockOverride || !CTheScripts::bMiniGameInProgress) {
        CHud::SetHelpMessageWithNumber(TheText.Get(key), n1, false, true);
    }
}

// COMMAND_CLEAR_THIS_PRINT - 0x485388
void ClearThisPrint(const char* key) {
    CMessages::ClearThisPrint(TheText.Get(key));
}

// COMMAND_CLEAR_THIS_BIG_PRINT - 0x4853B5
void ClearThisBigPrint(const char* key) {
    CMessages::ClearThisBigPrint(TheText.Get(key));
}
};

void notsa::script::commands::text::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER_BEGIN("Text");

    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_WITH_NUMBER_NOW, PrintWithNumberNow);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_WITH_2_NUMBERS_NOW, PrintWith2NumbersNow);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_WITH_3_NUMBERS_NOW, PrintWith3NumbersNow);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_WITH_4_NUMBERS_NOW, PrintWith4NumbersNow);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_WITH_5_NUMBERS_NOW, PrintWith5NumbersNow);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_WITH_6_NUMBERS_NOW, PrintWith6NumbersNow);

    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_PRINT_WITH_NUMBER_SOON);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_PRINT_WITH_2_NUMBERS_SOON);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_PRINT_WITH_3_NUMBERS_SOON);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_PRINT_WITH_4_NUMBERS_SOON);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_PRINT_WITH_5_NUMBERS_SOON);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_PRINT_WITH_6_NUMBERS_SOON);

    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_WITH_NUMBER, PrintWithNumber);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_WITH_2_NUMBERS, PrintWith2Numbers);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_WITH_3_NUMBERS, PrintWith3Numbers);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_WITH_4_NUMBERS, PrintWith4Numbers);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_WITH_5_NUMBERS, PrintWith5Numbers);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_WITH_6_NUMBERS, PrintWith6Numbers);

    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_PRINT_WITH_NUMBER_BIG_Q);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_WITH_NUMBER_BIG, PrintWithNumberBig);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_WITH_2_NUMBERS_BIG, PrintWith2NumbersBig);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_WITH_3_NUMBERS_BIG, PrintWith3NumbersBig);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_WITH_4_NUMBERS_BIG, PrintWith4NumbersBig);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_WITH_5_NUMBERS_BIG, PrintWith5NumbersBig);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_WITH_6_NUMBERS_BIG, PrintWith6NumbersBig);

    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_PRINT_STRING_IN_STRING);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_PRINT_STRING_IN_STRING_NOW);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_PRINT_STRING_IN_STRING_SOON);

    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_BIG_Q, PrintBigQ);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_PRINT_HELP_WITH_NUMBER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_PRINT_HELP_FOREVER);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_HELP_FOREVER_WITH_NUMBER, PrintHelpForeverWithNumber);
    
    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_THIS_PRINT, ClearThisPrint);
    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_THIS_BIG_PRINT, ClearThisBigPrint);

    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_SMALL_PRINTS, ClearSmallPrints);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_HELP, PrintHelp);
    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_HELP, ClearHelp);
    REGISTER_COMMAND_HANDLER(COMMAND_FLASH_HUD_OBJECT, FlashHudObject);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_BIG, PrintBig);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT, Print);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_NOW, PrintNow);
    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_THIS_PRINT_BIG_NOW, ClearThisPrintBigNow);
    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_PRINTS, ClearPrints);
    REGISTER_COMMAND_HANDLER(COMMAND_DISPLAY_TEXT, DisplayText);
}
