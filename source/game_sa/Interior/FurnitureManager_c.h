#pragma once

#include "Base.h"
#include "FurnitureGroup_c.h"
#include "FurnitureSubGroup_c.h"
#include "FurnitureEntity_c.h"
#include "List_c.h"

class Furniture_c;

class FurnitureManager_c {
private:
    static inline auto& g_currSubGroupId  = StaticRef<uint32>(0xBAB37C);
    static inline auto& g_currFurnitureId = StaticRef<uint32>(0xBAB378);
    static inline auto& g_subGroupStore   = StaticRef<FurnitureSubGroup_c[128]>(0xBAD3F8);

public:
    FurnitureManager_c() = default;  // 0x591260
    ~FurnitureManager_c() = default; // 0x5912E0

    int32 Init();
    void Exit();
    Furniture_c* GetFurniture(int32 groupId, int32 subGroupId, int16 refId, uint8 rating);
    int32 GetRandomId(int32 groupId, int32 subGroupId, uint8 rating);

    void LoadFurniture();
    int32 GetGroupId(const char* groupString);
    int32 GetSubGroupId(const char* subGroupString);

private:
    std::array<FurnitureGroup_c, 9>     m_Groups;
    std::array<FurnitureEntity_c, 512>  m_FurnitureItem;
    TList_c<FurnitureEntity_c>          m_FurnitureList;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

public:
    static FurnitureSubGroup_c* NewSubGroup();
};
VALIDATE_SIZE(FurnitureManager_c, 0x2078);

static inline auto& g_furnitureMan = StaticRef<FurnitureManager_c, 0xBAB380>();
