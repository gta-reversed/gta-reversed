/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"

struct RpHAnimBlendInterpFrame {
    RtQuat  q{0.f, 0.f, 0.f, 1.f};
    CVector t{0.f, 0.f, 0.f};
};
VALIDATE_SIZE(RpHAnimBlendInterpFrame, 0x1C);
