#include <StdInc.h>

#include "Commands.hpp"
#include <CommandParser/Parser.hpp>

/*!
* Various draw commands
*/

namespace {

/// DRAW_SPRITE(038D)
void DrawSprite(int32 slot, float x, float y, float width, float height, CRGBA color) {
    assert(slot > 0 && slot <= MAX_NUM_SCRIPT_SPRITES);

    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].m_nType             = eScriptRectangleType::TEXTURED;
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].m_nTextureId        = slot - 1;
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].cornerA             = { SCREEN_STRETCH_X(x - width * 0.5f), SCREEN_STRETCH_Y(y - height * 0.5f) };
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].cornerB             = { SCREEN_STRETCH_X(x + width * 0.5f), SCREEN_STRETCH_Y(y + height * 0.5f) };
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].m_nAngle            = 0.0f;
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].m_nTransparentColor = color;
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].gxt1[0]             = '\0';

    ++CTheScripts::NumberOfIntroRectanglesThisFrame;
}

/// DRAW_RECT(038E)
void DrawRect(float x, float y, float width, float height, CRGBA color) {
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].m_nType             = eScriptRectangleType::MONOCOLOR;
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].m_nTextureId        = -1;
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].cornerA             = { SCREEN_STRETCH_X(x - width * 0.5f), SCREEN_STRETCH_Y(y - height * 0.5f) };
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].cornerB             = { SCREEN_STRETCH_X(x + width * 0.5f), SCREEN_STRETCH_Y(y + height * 0.5f) };
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].m_nAngle            = 0.0f;
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].m_nTransparentColor = color;
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].gxt1[0]             = '\0';
    ++CTheScripts::NumberOfIntroRectanglesThisFrame;
}

/// LOAD_SPRITE(038F)
void LoadSprite(int32 slot, const char* spriteName) {
    char lowered[16]{ 0 };
    for (auto i = 0; i < 15 && spriteName[i] != '\0'; i++) {
        lowered[i] = tolower(spriteName[i]);
    }

    auto slotTxd = CTxdStore::FindTxdSlot("script");
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(slotTxd);
    assert(slot > 0 && slot <= MAX_NUM_SCRIPT_SPRITES); // script slot is 1-based
    CTheScripts::ScriptSprites[slot - 1].SetTexture(lowered);
    CTxdStore::PopCurrentTxd();
}

/// LOAD_TEXTURE_DICTIONARY(0390)
void LoadTextureDictionary(CRunningScript& S, const char* txdName) {
    std::string path = std::format("models\\txd\\{}.txd", txdName);

    const auto slot = CTxdStore::FindOrAddTxdSlot("script");

    CTxdStore::LoadTxd(slot, path.c_str());
    CTxdStore::AddRef(slot);
    if (S.m_UsesMissionCleanup) {
        CTheScripts::MissionCleanUp.AddEntityToList(1, MISSION_CLEANUP_ENTITY_TYPE_TXD);
    }
}

/// REMOVE_TEXTURE_DICTIONARY(0391)
void RemoveTextureDictionary(CRunningScript& S) {
    CTheScripts::RemoveScriptTextureDictionary();
    if (S.m_UsesMissionCleanup) {
        CTheScripts::MissionCleanUp.RemoveEntityFromList(1, MISSION_CLEANUP_ENTITY_TYPE_TXD);
    }
}

/// SET_SPRITES_DRAW_BEFORE_FADE(03E3)
void SetSpritesDrawBeforeFade(bool enabled) {
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].m_bDrawBeforeFade = enabled;
}

/// DRAW_SPRITE_WITH_ROTATION(074B)
void DrawSpriteWithRotation(int32 slot, float x, float y, float width, float height, float angle, CRGBA color) {
    assert(slot > 0 && slot <= MAX_NUM_SCRIPT_SPRITES);

    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].m_nType             = eScriptRectangleType::MONOCOLOR_ANGLED;
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].m_nTextureId        = slot - 1;
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].cornerA             = { SCREEN_STRETCH_X(x - width * 0.5f), SCREEN_STRETCH_Y(y - height * 0.5f) };
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].cornerB             = { SCREEN_STRETCH_X(x + width * 0.5f), SCREEN_STRETCH_Y(y + height * 0.5f) };
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].m_nAngle            = DegreesToRadians(angle);
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].m_nTransparentColor = color;
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].gxt1[0]             = '\0';

    ++CTheScripts::NumberOfIntroRectanglesThisFrame;
}

/// DRAW_WINDOW(0937)
void DrawWindow(CRect rect, const char* header, int32 style) {
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].cornerA             = { SCREEN_STRETCH_X(rect.left), SCREEN_STRETCH_Y(rect.top) };
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].cornerB             = { SCREEN_STRETCH_X(rect.right), SCREEN_STRETCH_Y(rect.bottom) };
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].m_nType             = eScriptRectangleType::TEXT;
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].m_nTextureId        = -1;
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].m_nAngle            = 0.0f;
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].m_nTransparentColor = CRGBA{ 0, 0, 0, 190 };
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].gxt2[0]             = '\0';
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].m_Alignment         = eFontAlignment::ALIGN_LEFT;
    CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].m_nTextboxStyle     = style;

    strncpy_s(CTheScripts::IntroRectangles[CTheScripts::NumberOfIntroRectanglesThisFrame].gxt1, (stricmp(header, "DUMMY") == 0 ? "\0" : header), 8);

    ++CTheScripts::NumberOfIntroRectanglesThisFrame;
}

} // namespace

void notsa::script::commands::draw::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER_BEGIN("Draw");

    REGISTER_COMMAND_HANDLER(COMMAND_DRAW_SPRITE, DrawSprite);
    REGISTER_COMMAND_HANDLER(COMMAND_DRAW_SPRITE_WITH_ROTATION, DrawSpriteWithRotation);
    REGISTER_COMMAND_HANDLER(COMMAND_LOAD_SPRITE, LoadSprite);
    REGISTER_COMMAND_HANDLER(COMMAND_LOAD_TEXTURE_DICTIONARY, LoadTextureDictionary);
    REGISTER_COMMAND_HANDLER(COMMAND_REMOVE_TEXTURE_DICTIONARY, RemoveTextureDictionary);
    REGISTER_COMMAND_HANDLER(COMMAND_DRAW_RECT, DrawRect);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_SPRITES_DRAW_BEFORE_FADE, SetSpritesDrawBeforeFade);
    REGISTER_COMMAND_HANDLER(COMMAND_DRAW_WINDOW, DrawWindow);

    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_TEXT_BACKGROUND_COLOUR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_TEXT_BACKGROUND_ONLY_TEXT);
}
