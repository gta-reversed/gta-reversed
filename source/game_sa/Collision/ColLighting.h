/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

struct tColLighting {
    union {
        struct {
            uint8 day : 4;
            uint8 night : 4;
        };

        uint8 value;
    };

    tColLighting() = default;

    constexpr explicit tColLighting(const uint8 ucLighting) {
        day   = ucLighting & 0xF;
        night = (ucLighting >> 4) & 0xF;
    }

    [[nodiscard]] float GetCurrentLighting(float fScale = 0.5F) const;
};

VALIDATE_SIZE(tColLighting, 0x1);
