#pragma once

#include "Base.h"

#include "ListItem_c.h"

class Furniture_c;

class FurnitureSubGroup_c : public ListItem_c<FurnitureSubGroup_c> {
public:
    static void InjectHooks();

    FurnitureSubGroup_c() = default;  // 0x590E20
    ~FurnitureSubGroup_c() = default; // 0x590E70

    bool AddFurniture(uint16 modelId, int16 id, uint8 wealthMin, uint8 wealthMax, uint8 maxAng);
    Furniture_c* GetFurniture(int16 id, uint8 wealth);
    int32 GetRandomId(uint8 a2);
    void Exit();

public:
    int32                m_SubGroupId;
    TList_c<Furniture_c> m_Furnitures;
    bool                 m_bCanPlaceInFrontOfWindow;
    bool                 m_bIsTall;
    bool                 m_bCanSteal;
};
VALIDATE_SIZE(FurnitureSubGroup_c, 0x1C);
