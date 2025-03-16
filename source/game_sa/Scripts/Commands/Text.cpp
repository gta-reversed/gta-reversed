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

/* Implementation for `COMMAND_DISPLAY_TEXT` and `COMMAND_DISPLAY_TEXT_WITH_FLOAT` */
void I_DisplayText(CVector2D pos, const char* gxtKey, int32 p1, int32 p2) {
    assert(CTheScripts::UseTextCommands != CTheScripts::eUseTextCommandState::DISABLED);

    auto* const line = &CTheScripts::IntroTextLines[CTheScripts::NumberOfIntroTextLinesThisFrame++];
    line->Pos      = pos;
    line->NumberToInsert1     = p1;
    line->NumberToInsert2     = p2;
    strcpy_s(line->GXTKey, gxtKey);
}

// COMMAND_DISPLAY_TEXT - 0x481A0C
void DisplayText(CVector2D pos, const char* gxtKey) {
    I_DisplayText(pos, gxtKey, -1, -1);
}

// COMMAND_DISPLAY_TEXT_WITH_FLOAT - 0x4730F8
void DisplayTextWithFloat(CVector2D pos, const char* gxtKey, float value, float precision) {
    float whole;
    float fract = std::modf(value, &whole);
    I_DisplayText(
        pos,
        gxtKey,
        (int32)(whole),
        (int32)(std::pow(10.f, precision) * fract)
    );
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

// COMMAND_PRINT_HELP_FOREVER - 0x48CB76
void PrintHelpForever(CRunningScript& S, const char* key) {
    const auto* const text = TheText.Get(key);
    if (CTheScripts::bDisplayNonMiniGameHelpMessages || S.m_IsTextBlockOverride || !CTheScripts::bMiniGameInProgress) {
        CHud::SetHelpMessage(text, false, true, false);
        if (CTheScripts::bAddNextMessageToPreviousBriefs) {
            CMessages::AddToPreviousBriefArray(text);
        }
    }
    CTheScripts::bAddNextMessageToPreviousBriefs = true;
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

// COMMAND_SET_TEXT_SCALE - 0x481AB2
void SetTextScale(float x, float y) {
    auto* const line = &CTheScripts::IntroTextLines[CTheScripts::NumberOfIntroTextLinesThisFrame];
    line->Scale.x    = x;
    line->Scale.y    = y;
}

// COMMAND_SET_TEXT_COLOUR - 0x481B10
void SetTextColour(uint8 r, uint8 b, uint8 g, uint8 a) {
    auto* const line = &CTheScripts::IntroTextLines[CTheScripts::NumberOfIntroTextLinesThisFrame];
    line->Color      = CRGBA{ r, g, b, a };
}

// COMMAND_SET_TEXT_JUSTIFY - 0x481B4B
void SetTextJustify(bool enabled) {
    auto* const line = &CTheScripts::IntroTextLines[CTheScripts::NumberOfIntroTextLinesThisFrame];
    line->Justify    = enabled;
}

// COMMAND_SET_TEXT_CENTRE - 0x481B88
void SetTextCentre(bool enabled) {
    auto* const line = &CTheScripts::IntroTextLines[CTheScripts::NumberOfIntroTextLinesThisFrame];
    line->IsCentered = enabled;
}

// COMMAND_SET_TEXT_WRAPX - 0x481BC5
void SetTextWrapx(float wrap) {
    auto* const line = &CTheScripts::IntroTextLines[CTheScripts::NumberOfIntroTextLinesThisFrame];
    line->WrapX      = wrap;
}

// COMMAND_SET_TEXT_CENTRE_SIZE - 0x481BE9
void SetTextCentreSize(float width) {
    auto* const line = &CTheScripts::IntroTextLines[CTheScripts::NumberOfIntroTextLinesThisFrame];
    line->CentreSize = width;
}

// COMMAND_SET_TEXT_BACKGROUND - 0x481C17
void SetTextBackground(bool enabled) {
    auto* const line = &CTheScripts::IntroTextLines[CTheScripts::NumberOfIntroTextLinesThisFrame];
    line->HasBg      = enabled;
}

// COMMAND_SET_TEXT_PROPORTIONAL - 0x481C49
void SetTextProportional(bool enabled) {
    auto* const line     = &CTheScripts::IntroTextLines[CTheScripts::NumberOfIntroTextLinesThisFrame];
    line->IsProportional = enabled;
}

// COMMAND_SET_TEXT_FONT - 0x481C86
void SetTextFont(eFontStyle font) {
    auto* const line = &CTheScripts::IntroTextLines[CTheScripts::NumberOfIntroTextLinesThisFrame];
    line->FontStyle  = font;
}

// COMMAND_SET_TEXT_DRAW_BEFORE_FADE - 0x485536
void SetTextDrawBeforeFade(bool enabled) {
    auto* const line       = &CTheScripts::IntroTextLines[CTheScripts::NumberOfIntroTextLinesThisFrame];
    line->IsDrawBeforeFade = enabled;
}

// COMMAND_SET_TEXT_RIGHT_JUSTIFY - 0x4855BA
void SetTextRightJustify(bool enabled) {
    auto* const line      = &CTheScripts::IntroTextLines[CTheScripts::NumberOfIntroTextLinesThisFrame];
    line->HasRightJustify = enabled;
}

// COMMAND_SET_TEXT_DROPSHADOW - 0x491579
void SetTextDropshadow(int32 shadow, uint8 r, uint8 g, uint8 b, uint8 a) {
    auto* const line      = &CTheScripts::IntroTextLines[CTheScripts::NumberOfIntroTextLinesThisFrame];
    line->DropShadowColor = CRGBA{ r, g, b, a };
    line->DropShadow      = shadow;
}

// COMMAND_SET_TEXT_EDGE - 0x473DD5
void SetTextEdge(int32 edge, uint8 r, uint8 g, uint8 b, uint8 a) {
    auto* const line      = &CTheScripts::IntroTextLines[CTheScripts::NumberOfIntroTextLinesThisFrame];
    line->DropShadowColor = CRGBA{ r, g, b, a };
    line->TextEdge        = edge;
}

// COMMAND_LOAD_MISSION_TEXT - 0x48D590
void LoadMissionText(const char* key) {
    TheText.LoadMissionText(key);
}

// COMMAND_SAVE_TEXT_LABEL_TO_DEBUG_FILE - 0x47366D
void SaveTextLabelToDebugFile(const char* key) {
    /* noop */
}

// COMMAND_DRAW_WINDOW_TEXT - 0x474937
void DrawWindowText() {
    /* noop */
}
};

void notsa::script::commands::text::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER_BEGIN("Text");

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
    REGISTER_COMMAND_HANDLER(COMMAND_DISPLAY_TEXT_WITH_FLOAT, DisplayTextWithFloat);

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
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_HELP_FOREVER, PrintHelpForever);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_PRINT_HELP_WITH_NUMBER);
    REGISTER_COMMAND_HANDLER(COMMAND_PRINT_HELP_FOREVER_WITH_NUMBER, PrintHelpForeverWithNumber);
    
    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_THIS_PRINT, ClearThisPrint);
    REGISTER_COMMAND_HANDLER(COMMAND_CLEAR_THIS_BIG_PRINT, ClearThisBigPrint);

    REGISTER_COMMAND_HANDLER(COMMAND_SET_TEXT_SCALE, SetTextScale);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_TEXT_COLOUR, SetTextColour);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_TEXT_JUSTIFY, SetTextJustify);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_TEXT_CENTRE, SetTextCentre);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_TEXT_WRAPX, SetTextWrapx);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_TEXT_CENTRE_SIZE, SetTextCentreSize);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_TEXT_BACKGROUND, SetTextBackground);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_TEXT_BACKGROUND_COLOUR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_TEXT_BACKGROUND_ONLY_TEXT);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_TEXT_PROPORTIONAL, SetTextProportional);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_TEXT_FONT, SetTextFont);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_TEXT_DRAW_BEFORE_FADE, SetTextDrawBeforeFade);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_TEXT_RIGHT_JUSTIFY, SetTextRightJustify);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_TEXT_DROPSHADOW, SetTextDropshadow);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_TEXT_EDGE, SetTextEdge);

    REGISTER_COMMAND_HANDLER(COMMAND_LOAD_MISSION_TEXT, LoadMissionText);
    REGISTER_COMMAND_HANDLER(COMMAND_SAVE_TEXT_LABEL_TO_DEBUG_FILE, SaveTextLabelToDebugFile);
    REGISTER_COMMAND_HANDLER(COMMAND_DRAW_WINDOW_TEXT, DrawWindowText);
}
