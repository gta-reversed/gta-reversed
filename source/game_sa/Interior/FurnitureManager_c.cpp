#include "StdInc.h"
#include "FurnitureManager_c.h"

auto& g_currSubGroupId = StaticRef<uint32, 0xBAB37C>();
auto& g_currFurnitureId = StaticRef<uint32, 0xBAB378>();

void FurnitureManager_c::InjectHooks() {
    RH_ScopedClass(FurnitureManager_c);
    RH_ScopedCategory("Interior");

    RH_ScopedGlobalInstall(GetGroupId, 0x5BFB40);
    RH_ScopedGlobalInstall(GetSubGroupId, 0x5BFBF0);
    RH_ScopedInstall(Exit, 0x5911A0);
    RH_ScopedInstall(GetFurniture, 0x5911E0);
    RH_ScopedInstall(GetRandomId, 0x591220);
    RH_ScopedInstall(LoadFurniture, 0x5C0280);
    RH_ScopedInstall(Init, 0x5C04A0);
}

// 0x5C04A0
bool FurnitureManager_c::Init() {
    g_currSubGroupId = 0;
    g_currFurnitureId = 0;

    for (auto& i : m_FurnitureItem) {
        m_FurnitureList.AddItem(&i);
    }
    
    rng::for_each(m_Groups, &FurnitureGroup_c::Init);

    LoadFurniture();

    return true;
}

// 0x5911A0
void FurnitureManager_c::Exit() {
    m_FurnitureList.RemoveAll();
    rng::for_each(m_Groups, &FurnitureGroup_c::Exit);
}

// notsa
FurnitureSubGroup_c* FurnitureManager_c::NewSubGroup() {
    return g_currSubGroupId < std::size(g_subGroupStore)
        ? &g_subGroupStore[g_currSubGroupId++]
        : nullptr;
}

// 0x5C0280
void FurnitureManager_c::LoadFurniture() {
    auto file = CFileMgr::OpenFile("data\\furnitur.dat", "rb");

    /**
    * GROUP:    <group name>
    * SUBGROUP: <sub-group name> <min width> <min depth> <max width> <max depth> <can place in front of Window> <is tall> <is stealable>
    * ITEM:	    <model name> <item id> <min wealth> <max wealth> <max angle>
    * <Items....>
    *
    * A sub-group can have multiple items.
    * A group can't direcly have items, only a subgroup can.
    **/

    // They originally used IDs, and searched for the sub-group each loop, so I'm replacing it with pointers instead
    FurnitureGroup_c* grp{};
    FurnitureSubGroup_c* subGrp{};
    while (const auto l = CFileLoader::LoadLine(file)) {
        if (*l == 0 || *l == '#') { // Skip empty and comment lines
            continue;
        }

        char tag[32];
        VERIFY(sscanf_s(l, "%s", SCANF_S_STR(tag)) == 1);

        if (!strcmp(tag, "GROUP:")) {
            int32 grpId;
            char groupName[128];
            VERIFY(sscanf(l, "%s %s", tag, groupName) == 2);
            VERIFY((grpId = FurnitureManager_c::GetGroupId(groupName)) != -1);
            grp = &m_Groups[grpId];
        } else if (!strcmp(tag, "SUBGROUP:")) {
            char  subGroupName[128];
            int32 subGrpId;
            int32 minWidth, minDepth, maxWidth, maxDepth, canPlaceInFrontOfWindow, isTall, canSteal;
            VERIFY(sscanf_s(l, "%s %s %d %d %d %d %d %d %d", SCANF_S_STR(tag), SCANF_S_STR(subGroupName), &minWidth, &minDepth, &maxWidth, &maxDepth, &canPlaceInFrontOfWindow, &isTall, &canSteal) == 9);
            VERIFY((subGrpId = FurnitureManager_c::GetSubGroupId(subGroupName)) != -1);
            VERIFY(grp->AddSubGroup(subGrpId, minWidth, minDepth, maxWidth, maxDepth, canPlaceInFrontOfWindow, isTall == 1, canSteal == 1));
            VERIFY(subGrp = grp->GetSubGroup(subGrpId));
        } else if (!strcmp(tag, "ITEM:")) {
            char modelName[64];
            int32 id, wealthMin, wealthMax, maxAng;
            uint16 modelIndex = -1;
            VERIFY(sscanf_s(l, "%s %s %d %d %d %d", SCANF_S_STR(tag), SCANF_S_STR(modelName), &id, &wealthMin, &wealthMax, &maxAng) == 6);
            VERIFY(CModelInfo::GetModelInfoUInt16(modelName, &modelIndex) != nullptr);
            VERIFY(subGrp->AddFurniture(modelIndex, id, wealthMin, wealthMax, maxAng));
        } else {
            NOTSA_UNREACHABLE("Unknown tag {}", tag);
        }
    }
    CFileMgr::CloseFile(file);
}

// 0x5BFB40
int32 FurnitureManager_c::GetGroupId(const char* name) {
    static const auto groups = notsa::make_mapping<std::string_view, int32>({
        {"IT_SHOP",    0},
        {"IT_OFFICE",  1},
        {"IT_LOUNGE",  2},
        {"IT_BEDROOM", 3},
        {"IT_KITCHEN", 4},
        {"IT_MISC",    8},
    });
    return notsa::find_value_or(groups, name, -1);
}

// 0x5BFBF0
int32 FurnitureManager_c::GetSubGroupId(const char* name) {
    static const auto subGroups = notsa::make_mapping<std::string_view, int32>({
        // IT_SHOP
        {"SHOP_UNIT1_L",          0 },
        { "SHOP_UNIT1_R",         1 },
        { "SHOP_UNIT1_M",         2 },
        { "SHOP_UNIT2_L",         3 },
        { "SHOP_UNIT2_R",         4 },
        { "SHOP_UNIT2_M",         5 },
        { "SHOP_UNIT3_L",         6 },
        { "SHOP_UNIT3_R",         7 },
        { "SHOP_UNIT3_M",         8 },
        { "SHOP_UNIT4_L",         9 },
        { "SHOP_UNIT4_R",         10},
        { "SHOP_UNIT4_M",         11},
        { "SHOP_CHECKOUTS",       12},
        { "SHOP_BASKETS",         13},
        { "SHOP_LIGHTS",          14},

        // IT_OFFICE
        { "OFFICE_DESKS",         0 },
        { "OFFICE_CHAIRS",        1 },
        { "OFFICE_WATERCOOLERS",  2 },
        { "OFFICE_FILINGCABS",    3 },

        // IT_LOUNGE
        { "LOUNGE_SOFAS",         0 },
        { "LOUNGE_CHAIRS",        1 },
        { "LOUNGE_TVSTANDS",      2 },
        { "LOUNGE_TVS",           3 },
        { "LOUNGE_TABLES",        4 },
        { "LOUNGE_LAMPS",         5 },
        { "LOUNGE_HIFI_TABLES",   6 },
        { "LOUNGE_VIDEOS",        7 },
        { "LOUNGE_HIFIS",         8 },
        { "LOUNGE_CONSOLES",      9 },

        // IT_BEDROOM
        { "BEDROOM_BEDS",         0 },
        { "BEDROOM_TABLES",       1 },
        { "BEDROOM_DRAWERS",      2 },
        { "BEDROOM_WARDROBES",    3 },

        // IT_KITCHEN
        { "KITCHEN_UNIT_L",       0 },
        { "KITCHEN_UNIT_M",       1 },
        { "KITCHEN_UNIT_R",       2 },
        { "KITCHEN_UNIT_SINKS",   3 },
        { "KITCHEN_UNIT_FRIDGES", 4 },
        { "KITCHEN_UNIT_OVENS",   5 },
        { "KITCHEN_UNIT_WASHERS", 6 },
        { "KITCHEN_UNIT_CORNERS", 7 },
        { "KITCHEN_MICROWAVES",   8 },
        { "KITCHEN_TOASTERS",     9 },
        { "KITCHEN_TVS",          10},

        // IT_MISC
        { "MISC_PLANTS",          0 },
        { "MISC_LIGHTS",          1 },
        { "MISC_RUGS",            2 },
        { "MISC_PLATES",          3 },
        { "MISC_FOOD",            4 },
        { "MISC_MAGAZINES",       5 },
        { "MISC_CLOTHES",         6 },
    });
    return notsa::find_value_or(subGroups, name, -1);
}

// 0x5911E0
Furniture_c* FurnitureManager_c::GetFurniture(int32 groupId, int32 subGroupId, int16 id, uint8 wealth) {
    return m_Groups[groupId].GetFurniture(subGroupId, id, wealth);
}

// 0x591220
int32 FurnitureManager_c::GetRandomId(int32 groupId, int32 subGroupId, uint8 wealth) {
    return m_Groups[groupId].GetRandomId(subGroupId, wealth);
}
