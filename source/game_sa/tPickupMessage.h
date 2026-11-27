/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "eWeaponType.h"

class CRGBA;
class CVector2D;

struct tPickupMessage {
public:
    CVector2D   pos{};        //!< 0x0  - Screen position
    eWeaponType weaponType{}; //!< 0x8  - Weapon type of the pickup (Unused)
    float       width{};      //!< 0xC
    float       height{};     //!< 0x10
    CRGBA       color{};      //!< 0x14
    struct {
        uint8 IsOutOfStock : 1;      //!< 0x1
        uint8 IsPropertyForSale : 1; //!< 0x2 - The price is inserted into the text
    } flags{};                       //!< 0x18
    uint8          bonus{};          //!< 0x19 - Bonus value of the pickup
    uint32         price{};          //!< 0x1C
    const GxtChar* text{};           //!< 0x20
};

VALIDATE_SIZE(tPickupMessage, 0x24);
