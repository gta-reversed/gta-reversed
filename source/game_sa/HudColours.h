/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RGBA.h"

enum eHudColours : uint8 {
    HUD_COLOUR_RED        = 0,
    HUD_COLOUR_GREEN      = 1,
    HUD_COLOUR_DARK_BLUE  = 2,
    HUD_COLOUR_LIGHT_BLUE = 3,
    HUD_COLOUR_LIGHT_GRAY = 4,
    HUD_COLOUR_BLACK      = 5,
    HUD_COLOUR_GOLD       = 6,
    HUD_COLOUR_PURPLE     = 7,
    HUD_COLOUR_DARK_GRAY  = 8,
    HUD_COLOUR_DARK_RED   = 9,
    HUD_COLOUR_DARK_GREEN = 10,
    HUD_COLOUR_CREAM      = 11,
    HUD_COLOUR_NIGHT_BLUE = 12,
    HUD_COLOUR_BLUE       = 13,
    HUD_COLOUR_YELLOW     = 14,

    HUD_COLOUR_COUNT,
};

class CHudColours {
public:
    std::array<CRGBA, HUD_COLOUR_COUNT> m_aColours;

public:
    static void InjectHooks();

    CHudColours();
    ~CHudColours() = default; // 0x58FD90
    CHudColours* Constructor();

    void SetRGBAValue(eHudColours colorIndex, uint8 red, uint8 green, uint8 blue, uint8 alpha);
    uint32 GetIntColour(eHudColours colorIndex) const;

    CRGBA GetRGB(eHudColours colorIndex) const;
    CRGBA GetRGBA(eHudColours index, uint8 alpha) const;
};

VALIDATE_SIZE(CHudColours, 0x3C);

extern CHudColours& HudColour;
