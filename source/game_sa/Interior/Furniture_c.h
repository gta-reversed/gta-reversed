#pragma once

#include "Base.h"

class Furniture_c : public ListItem_c<Furniture_c> {
public:
    uint16 m_ModelId; // 0x8
    int16  m_RefId;   // 0xA

    uint8 m_Width; // 0xC
    uint8 m_Depth; // 0xD

    uint8 m_RatingMin; // 0xE
    uint8 m_RatingMax; // 0xF

    bool m_OnWindowTile; // 0x10
    bool m_IsTall;       // 0x11

    bool  m_IsStealable; // 0x12
    uint8 m_MaxAngle;    // 0x13

public:
    static void InjectHooks();

    Furniture_c() = default;  // 0x590EC0
    ~Furniture_c() = default; // 0x590ED0
};
VALIDATE_SIZE(Furniture_c, 0x14);
