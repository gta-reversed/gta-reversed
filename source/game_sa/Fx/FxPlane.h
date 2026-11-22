#pragma once

#include "RenderWare.h"

class FxPlane_c : public RwPlane {
public:
    // CVector m_Normal;
    // float   m_fDistance;

    FxPlane_c& operator=(const RwPlane& rhs) {
        normal   = rhs.normal;
        distance = rhs.distance;
        return *this;
    }
};
VALIDATE_SIZE(FxPlane_c, 0x10);
