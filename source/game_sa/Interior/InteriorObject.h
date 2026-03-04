#pragma once

#include "Base.h"

#include "Vector.h"

class CEntity;

struct StealableInfo_t {
    CEntity* Entity;
    int32    ModelId;
    int32    InteriorId;
    CVector  Position;
    bool     IsStolen;
};

VALIDATE_SIZE(StealableInfo_t, 0x1C);
