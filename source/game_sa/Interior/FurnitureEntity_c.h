#pragma once

#include <Base.h>
#include <ListItem_c.h>

class CEntity;

struct FurnitureEntity_c : ListItem_c<FurnitureEntity_c> {
    CEntity* m_Entity;
    uint16 m_TileX, m_TileY;
};
