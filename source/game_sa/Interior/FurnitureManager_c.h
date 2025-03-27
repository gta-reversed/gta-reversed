#pragma once

#include "Base.h"
#include "FurnitureGroup_c.h"
#include "FurnitureSubGroup_c.h"
#include "FurnitureItem.h"
#include "List_c.h"

class Furniture_c;

class FurnitureManager_c {
private:
    static inline auto& g_currSubGroupId  = StaticRef<uint32>(0xBAB37C);
    static inline auto& g_currFurnitureId = StaticRef<uint32>(0xBAB378);
    static inline auto& g_subGroupStore   = StaticRef<FurnitureSubGroup_c[128]>(0xBAD3F8);

public:
    static void InjectHooks();

    FurnitureManager_c() = default;  // 0x591260
    ~FurnitureManager_c() = default; // 0x5912E0

    static int32 GetGroupId(const char* name);
    static int32 GetSubGroupId(const char* name);
    static FurnitureSubGroup_c* NewSubGroup();

    bool Init();
    void Exit();
    void LoadFurniture();
    Furniture_c* GetFurniture(int32 furnitureGroupId, int32 furnitureSubgroupId, int16 id, uint8 wealth);
    int32 GetRandomId(int32 groupId, int32 a3, uint8 a4);

private:
    std::array<FurnitureGroup_c, 9> m_Groups;
    std::array<FurnitureItem, 512>  m_FurnitureItem;
    TList_c<FurnitureItem>          m_FurnitureList;
};
VALIDATE_SIZE(FurnitureManager_c, 0x2078);

static inline auto& g_furnitureMan = StaticRef<FurnitureManager_c, 0xBAB380>();
