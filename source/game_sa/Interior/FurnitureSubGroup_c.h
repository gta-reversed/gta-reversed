#pragma once

#include "Base.h"

#include "ListItem_c.h"

class Furniture_c;

class FurnitureSubGroup_c : public ListItem_c<FurnitureSubGroup_c> {
public:
    static void InjectHooks();

    FurnitureSubGroup_c() = default;  // 0x590E20
    ~FurnitureSubGroup_c() = default; // 0x590E70

    bool AddFurniture(uint16 modelIndex, int16 refId, uint8 minRating, uint8 maxRating, uint8 maxAngle);
    Furniture_c* GetFurniture(int16 refId, uint8 rating);
    int32 GetRandomId(uint8 rating);

public:
    int32 m_Id;
    TList_c<Furniture_c> m_FurnitureList;

    bool m_OnWindowTile;
    bool m_IsTall;

    bool m_IsStealable;

public:
    void Exit();
};
VALIDATE_SIZE(FurnitureSubGroup_c, 0x1C);
