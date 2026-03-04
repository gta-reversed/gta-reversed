#pragma once

#include "Base.h"

#include "List_c.h"

class Furniture_c;
class FurnitureSubGroup_c;

class FurnitureGroup_c {
public:
    FurnitureGroup_c() = default;  // 0x590DE0
    ~FurnitureGroup_c() = default; // 0x590DF0

    int32 Init();
    void Exit();

    bool AddSubGroup(int32 subGroupId, int32 minWidth, int32 minDepth, int32 maxWidth, int32 maxDepth, bool onWindow, bool isTall, bool isStealable);
    bool AddFurniture(int32 subGroupId, uint16 modelIndex, int16 refId, uint8 minRating, uint8 maxRating, uint8 maxAngle);

    Furniture_c* GetFurniture(int32 subGroupId, int16 furnitureId, uint8 rating);
    int32 GetRandomId(int32 subGroupId, uint8 rating);


private:
    TList_c<FurnitureSubGroup_c> m_SubGroupList;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

public:
    FurnitureSubGroup_c* GetSubGroup(int32 subGroupId);
};

VALIDATE_SIZE(FurnitureGroup_c, 0xC);
