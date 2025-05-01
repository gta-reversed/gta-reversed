#pragma once

#include "RenderWare.h"

class FxBox_c {
public:
    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;

    FxBox_c() {
        Reset();
    }

    void Reset() {
        minX = minY = minZ = 999999.0f;
        maxX = maxY = maxZ = -999999.0f;
    }

    FxBox_c& operator=(const FxBox_c& right) = default;

    FxBox_c& operator=(const RwMatrix& right) {
        minX = right.pos.x;
        minY = right.pos.y;
        minZ = right.pos.z;

        maxX = right.pos.x;
        maxY = right.pos.y;
        maxZ = right.pos.z;
        return *this;
    }
};
VALIDATE_SIZE(FxBox_c, 0x18);
