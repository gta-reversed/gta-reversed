#pragma once

#include "Base.h"

#include "List_c.h"
#include "InteriorGroup_c.h"

class Furniture_c;
class FurnitureSubGroup_c;

class FurnitureGroup_c {
public:
    static void InjectHooks();

    FurnitureGroup_c() = default;  // 0x590DE0
    ~FurnitureGroup_c() = default; // 0x590DF0

    int32 Init();
    void Exit();
    bool AddSubGroup(eInteriorSubGroupIdS32 subGroupId, int32 minWidth, int32 minDepth, int32 maxWidth, int32 maxDepth, uint8 canPlaceInFrontOfWindow, bool isTall, bool canSteal);
    Furniture_c* GetFurniture(eInteriorSubGroupIdS32 subGroupId, int16 furnitureId, uint8 rating);
    int32 GetRandomId(eInteriorSubGroupIdS32 subGroupId, uint8 rating);
    bool AddFurniture(eInteriorSubGroupIdS32 subGroupId, uint16 modelId, int16 id, uint8 wealthMin, uint8 wealthMax, uint8 maxAng);
    FurnitureSubGroup_c* GetSubGroup(eInteriorSubGroupIdS32 subGroupId);

private:
    TList_c<FurnitureSubGroup_c> m_subGroupsList;
};
VALIDATE_SIZE(FurnitureGroup_c, 0xC);
