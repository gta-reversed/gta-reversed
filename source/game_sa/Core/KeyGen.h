/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

// CRC-32-IEEE
// https://www.xilinx.com/support/documentation/application_notes/xapp209.pdf

class CKeyGen {
public:
    static const uint32 keyTable[256];

public:
    static void InjectHooks();

    [[nodiscard]] static uint32 AppendStringToKey(uint32 key, const char* str) noexcept;
    [[nodiscard]] static uint32 GetKey(const char* str) noexcept;
    [[nodiscard]] static uint32 GetKey(const char* str, int32 size) noexcept;
    [[nodiscard]] static uint32 GetUppercaseKey(const char* str) noexcept;
    [[nodiscard]] static uint32 GetUppercaseKey(const char* begin, const char* end) noexcept;
};
