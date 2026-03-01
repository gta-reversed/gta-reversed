#pragma once

#include "Base.h"

#include "Vector.h"

class CEntity;

struct InteriorObject {
    CEntity* entity;
    int32    modelId;
    int32    interiorId;
    CVector  pos;
    bool     wasStolen;
};
