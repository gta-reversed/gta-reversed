#pragma once

#include <Base.h>
#include <ListItem_c.h>

class CEntity;

struct FurnitureEntity_c : ListItem_c<FurnitureEntity_c> {
    CEntity* m_entity;
    uint16 m_tileX, m_tileY;
};
