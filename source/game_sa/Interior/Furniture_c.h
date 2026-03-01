#pragma once

#include "Base.h"

class Furniture_c : public ListItem_c<Furniture_c> {
public:
    int16 m_nModelId;                 // 0x8
    int16 m_nId;                      // 0xA
    int8  m_nWidthX;                  // 0xC
    int8  m_nWidthY;                  // 0xD
    int8  m_nWealthMin;               // 0xE
    int8  m_nWealthMax;               // 0xF
    bool  m_bCanPlaceInFrontOfWindow; // 0x10
    bool  m_bIsTall;                  // 0x11
    bool  m_bCanSteal;                // 0x12
    int8  m_nMaxAng;                  // 0x13

public:
    static void InjectHooks();

    Furniture_c() = default;  // 0x590EC0
    ~Furniture_c() = default; // 0x590ED0
};
VALIDATE_SIZE(Furniture_c, 0x14);
