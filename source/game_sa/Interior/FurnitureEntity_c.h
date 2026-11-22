#pragma once

#include "Base.h"
#include "ListItem_c.h"

class CObject;

struct FurnitureEntity_c : public ListItem_c<FurnitureEntity_c> {
    CObject* Entity;
    uint16   TileX, TileY;
};
VALIDATE_SIZE(FurnitureEntity_c, 0x10);
