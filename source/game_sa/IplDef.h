/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Rect.h"

struct IplDef {
    CRect bb{};
    char  name[18]{};

    int16 firstBuilding{ SHRT_MAX };
    int16 lastBuilding{ SHRT_MIN };

    int16 firstDummy{ SHRT_MAX };
    int16 lastDummy{ SHRT_MIN };

    int16 staticIdx{ -1 }; // entity arrays index
    bool  isInterior{};
    char  loaded{}; 
    bool  loadRequested{};
    bool  disableDynamicStreaming{ true };
    char  ignoreWhenDeleted{};
    char  isLarge{}; // Makes bounding box bigger. (+350 vs +200 units). See `CIplStore::LoadIpl`

    constexpr IplDef() = default;

    IplDef(const char* name) {
        strcpy_s(this->name, name);
    }
};
VALIDATE_SIZE(IplDef, 0x34);
