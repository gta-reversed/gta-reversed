/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CDate {
public:
    int32 seconds{};
    int32 minutes{};
    int32 hours{};
    int32 day{};
    int32 month{};
    int32 year{};

public:
    constexpr CDate() = default;

    constexpr std::strong_ordering operator<=>(const CDate& o);

    constexpr void PopulateDateFields(const uint8& seconds, const uint8& minutes, const uint8& hours, const uint8& day, const uint8& month, int16 year);
};

VALIDATE_SIZE(CDate, 0x18);
