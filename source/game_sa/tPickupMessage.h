/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CRGBA;
class CVector;

struct tPickupMessage {
public:
    CVector  pos;
    float    width;
    float    height;
    CRGBA    color;
    uint8    flags;
    char     field_19;
    uint32   price;
    GxtChar* text;
};

VALIDATE_SIZE(tPickupMessage, 0x24);
