#pragma once

#include "Base.h"

#include "ListItem_c.h"

class CObject;

class FurnitureItem : public ListItem_c<FurnitureItem> {
public:
    FurnitureItem() = default;

    CObject* m_item;
    int16 m_field_C;
    int16 m_field_E;
};
