#pragma once

#include "ColLighting.h"
#include "Enums/eSurfaceType.h"

struct tColData {
    eSurfaceType SurfaceType;
    uint8        PieceType;
    tColLighting Lighting;
};

VALIDATE_SIZE(tColData, 0x3);
