#include "StdInc.h"
#include "Shopping.h"
#include "PedClothesDesc.h"
#include <extensions/enumerate.hpp>

void CShopping::InjectHooks() {
    RH_ScopedClass(CShopping);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x49C290);
    RH_ScopedInstall(ShutdownForRestart, 0x49B640);
    RH_ScopedInstall(Buy, 0x49BF70);
    RH_ScopedInstall(FindItem, 0x49AD20);
    RH_ScopedInstall(FindSection, 0x49AE70);
    RH_ScopedInstall(FindSectionInSection, 0x49AF90);
    RH_ScopedInstall(GetExtraInfo, 0x49ADE0);
    RH_ScopedInstall(GetItemIndex, 0x49AB10);
    RH_ScopedInstall(GetKey, 0x49AB30);
    RH_ScopedInstall(GetNameTag, 0x49ADA0);
    RH_ScopedInstall(GetNextSection, 0x49AF10);
    RH_ScopedInstall(GetPrice, 0x49AD50);
    RH_ScopedInstall(GetPriceSectionFromName, 0x49AAD0);
    RH_ScopedInstall(SetPlayerHasBought, 0x49B610);
    RH_ScopedInstall(HasPlayerBought, 0x49B5E0);
    RH_ScopedInstall(IncrementStat, 0x49BE70);
    RH_ScopedInstall(IncrementStat2, 0x49AFD0);
    RH_ScopedInstall(LoadPrices, 0x49B8D0);
    RH_ScopedInstall(LoadShop, 0x49BBE0);
    RH_ScopedInstall(LoadStats, 0x49B6A0);
    RH_ScopedInstall(RemoveLoadedPrices, 0x49AC90);
    RH_ScopedInstall(RemoveLoadedShop, 0x49AE30);
    RH_ScopedInstall(StoreClothesState, 0x49B200);
    RH_ScopedInstall(StoreVehicleMods, 0x49B280);
    RH_ScopedInstall(RestoreClothesState, 0x49B240);
    RH_ScopedInstall(RestoreVehicleMods, 0x49B3C0);
    RH_ScopedInstall(UpdateStats, 0x49BEF0);
    RH_ScopedInstall(Load, 0x5D3E40, {.enabled = LOAD_HOOKS_ENABLED });
    RH_ScopedInstall(Save, 0x5D3DE0, {.enabled = SAVE_HOOKS_ENABLED });

    // address?
    // RH_ScopedInstall(AddPriceModifier, 0x0, { .reversed = false });
    // RH_ScopedInstall(AddPriceModifier, 0x0, { .reversed = false });
    // RH_ScopedInstall(RemovePriceModifier, 0x0, { .reversed = false });
    // RH_ScopedInstall(RemovePriceModifier, 0x0, { .reversed = false });

    // unused
    RH_ScopedInstall(SetCurrentProperty, 0x49B1F0);
}

/*!
 * @brief Get changing stat index from name. (Used only in CShopping::LoadStats)
 * @addr 0x49ABD0
 * @param stat Changing stat name.
 * @returns Stat index from the list provided in the function.
 */
int32 GetChangingStatIndex(const char* stat) {
    static constexpr const char* statNames[] = {"fat", "respect", "sexy", "health", "stamina", "calories"};

    // todo: use notsa::enumerate
    for (auto i = 0u; i < std::size(statNames); i++) {
        if (!strcmp(statNames[i], stat)) {
            return i;
        }
    }

    return -1;
}

// 0x49C290
void CShopping::Init() {
    rng::fill(ms_keys, 0u);
    rng::fill(ms_bHasBought, false);
    ms_numPrices = 0;
    ms_numPriceModifiers = 0;
    ms_numBuyableItems = 0u;
    gClothesHaveBeenStored = 0;

    LoadStats();
}

// 0x49B640
void CShopping::ShutdownForRestart() {
    RemoveLoadedShop();

    ms_numPriceModifiers = 0;
    rng::fill(ms_bHasBought, false);
}

// 0x49BF70
void CShopping::Buy(uint32 key, int32 extraInfo) {
    const auto index = GetItemIndex(key);

    auto ped = FindPlayerPed();
    auto& playerInfo = FindPlayerInfo();

    auto playerClothes = ped->GetClothesDesc();
    if (gClothesHaveBeenStored) {
        playerClothes = &gStoredClothesState;
    }

    const auto price = GetPrice(key);
    const auto fPrice = static_cast<float>(price);
    const auto& priceInfo = ms_prices[FindItem(key)];
    playerInfo.m_nMoney -= price;

    for (const auto modifier : ms_statModifiers[index].modifiers) {
        IncrementStat(modifier.statIndex, modifier.change);
    }
    CStats::ModifyStat(STAT_TOTAL_SHOPPING_BUDGET, fPrice);
    ms_bHasBought[index] = true;

    switch (ms_priceSectionLoaded) {
    case PRICE_SECTION_CAR_MODS: {
        // inlined:
        const auto veh = FindPlayerVehicle();
        const auto vehModel = veh->GetModelInfo()->AsVehicleModelInfoPtr();
        const auto upgradeModel = CModelInfo::GetModelInfo(key);
        veh->AddVehicleUpgrade(key);
        if (veh->IsAutomobile()) {
            const auto car = veh->AsAutomobile();

            // todo: refactor
            const auto carModId = upgradeModel->nCarmodId;
            auto parentId = vehModel->m_pVehicleStruct->m_aUpgrades[carModId].m_nParentComponentId;
            if (upgradeModel->bUsesVehDummy) {
                parentId = carModId;
            }

            switch (parentId) {
            case CAR_WHEEL_RF:
                car->FixTyre(CAR_WHEEL_FRONT_LEFT);
                car->FixTyre(CAR_WHEEL_FRONT_RIGHT);
                car->FixTyre(CAR_WHEEL_REAR_LEFT);
                car->FixTyre(CAR_WHEEL_REAR_RIGHT);
                break;
            case CAR_BUMP_FRONT:
                car->FixPanel(CAR_BUMP_FRONT, FRONT_BUMPER);
                break;
            case CAR_BUMP_REAR:
                car->FixPanel(CAR_BUMP_REAR, REAR_BUMPER);
                break;
            case CAR_BONNET:
                car->FixDoor(CAR_BONNET, DOOR_BONNET);
                break;
            case CAR_BOOT:
                car->FixDoor(CAR_BOOT, DOOR_BOOT);
                break;
            default:
                break;
            }
        }

        [[fallthrough]];
    }

    case PRICE_SECTION_CAR_PAINTJOBS:
        CStats::ModifyStat(STAT_CAR_MODIFICATION_BUDGET, fPrice);
        break;

    case PRICE_SECTION_CLOTHES: {
        CStats::ModifyStat(STAT_FASHION_BUDGET, fPrice);

        const auto texKey = playerClothes->m_anTextureKeys[priceInfo.clothes.type];
        if (texKey) {
            UpdateStats(GetItemIndex(texKey), false);
        }
        ped->GetClothesDesc()->SetTextureAndModel(key, priceInfo.clothes.modelKey, (eClothesTexturePart)priceInfo.clothes.type);

        switch (CClothes::GetTextureDependency((eClothesTexturePart)priceInfo.clothes.type)) {
        case CLOTHES_MODEL_TORSO:
        case CLOTHES_MODEL_LEGS:
        case CLOTHES_MODEL_HEAD:
        case CLOTHES_MODEL_GLASSES:
        case CLOTHES_MODEL_HATS:
            FindPlayerWanted()->ClearWantedLevelAndGoOnParole();
            break;
        default:
            break;
        }
        break;
    }

    case PRICE_SECTION_HAIRCUTS: {
        CStats::ModifyStat(STAT_HAIRDRESSING_BUDGET, fPrice);

        const auto texKey = playerClothes->m_anTextureKeys[priceInfo.clothes.type];
        if (texKey) {
            UpdateStats(GetItemIndex(texKey), false);
        }
        ped->GetClothesDesc()->SetTextureAndModel(key, priceInfo.clothes.modelKey, (eClothesTexturePart)priceInfo.clothes.type);

        FindPlayerWanted()->ClearWantedLevelAndGoOnParole();
        break;
    }

    case PRICE_SECTION_TATTOOS:
        CStats::ModifyStat(STAT_TATTOO_BUDGET, fPrice);

        ped->GetClothesDesc()->SetTextureAndModel(key, 0u, (eClothesTexturePart)priceInfo.tattoos.type1);
        break;

    case PRICE_SECTION_GIFTS:
        ped->GiveWeapon(WEAPON_FLOWERS, 0, false);
        break;

    case PRICE_SECTION_FOOD:
        CStats::ModifyStat(STAT_FOOD_BUDGET, fPrice);
        CStats::ModifyStat(STAT_NUMBER_OF_MEALS_EATEN, 1.0f);
        break;

    case PRICE_SECTION_WEAPONS:
        CStats::ModifyStat(STAT_WEAPON_BUDGET, fPrice);

        if (key == WEAPON_ARMOUR) {
            ped->m_fArmour = playerInfo.m_nMaxArmour;
        } else {
            ped->GiveWeapon((eWeaponType)key, priceInfo.weapon.ammo, false);
            ped->SetCurrentWeapon((eWeaponType)key);
        }
        break;

    default:
        break;
    }
}

/*!
 * @brief Find item by searching through ms_prices.
 * @addr 0x49AD20 (inlined)
 * @param item Key ID of the item.
 * @returns 'Buyable' index of the item
 */
int32 CShopping::FindItem(uint32 itemKey) {
    if (ms_numPrices >= 1) {
        for (auto&& [i, p] : notsa::enumerate(std::span{ms_prices.data(), (size_t)ms_numPrices})) {
            if (p.key == itemKey) {
                return i;
            }
        }
    }

    NOTSA_UNREACHABLE();
}

/*!
 * @brief Find a section inside the file.
 * @addr 0x49AE70
 * @param file A shopping.dat-like formatted file.
 * @param The section to find
 * @returns If the section was found. The file's read pointer will be right after the section's beginning (after the `section` line)
 */
bool CShopping::FindSection(FILESTREAM file, const char* sectionName) {
    auto counter = 0;
    for (auto line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (*line == '\0' || *line == '#')
            continue;

        char* nextToken{};
        if (!strncmp(line, "section", 7u)) {
            counter++;
            RET_IGNORED(strtok_s(line, " \t", &nextToken));

            if (counter == 1 && !_stricmp(sectionName, strtok_s(nullptr, " \t", &nextToken))) {
                return true;
            }
        } else if (!strncmp(line, "end", 3u) && --counter < 0) {
            break;
        }
    }

    return false;
}

// 0x49AF90 (unused)
bool CShopping::FindSectionInSection(FILESTREAM file, const char* parentSection, const char* childSection) {
    return FindSection(file, parentSection) && FindSection(file, childSection);
}

// 0x49ADE0
int32 CShopping::GetExtraInfo(uint32 itemKey, int32 index) {
    const auto id = FindItem(itemKey);
    switch (index) {
    case 0:
        return ms_prices[id].general.extra1;
    case 1:
        return ms_prices[id].general.extra2;
    default:
        break;
    }

    return 0;
}

/*!
 * @brief Find item by searching through ms_keys.
 * @addr 0x49AB10
 * @param itemKey Key ID of the item.
 * @returns 'Absolute' index of the item
 */
int32 CShopping::GetItemIndex(uint32 itemKey) {
    for (const auto&& [i, key] : notsa::enumerate(ms_keys)) {
        if (key == itemKey) {
            return i;
        }
    }

    NOTSA_UNREACHABLE();
}

// 0x49AB30
uint32 CShopping::GetKey(const char* modelName, ePriceSection section) {
    switch (section) {
    case PRICE_SECTION_CLOTHES:
    case PRICE_SECTION_HAIRCUTS:
    case PRICE_SECTION_TATTOOS:
        return CKeyGen::GetUppercaseKey(modelName);
    case PRICE_SECTION_WEAPONS:
        return CWeaponInfo::FindWeaponType(modelName);
    case PRICE_SECTION_CAR_PAINTJOBS:
        return PRICE_SECTION_CAR_PAINTJOBS;
    default:
        break;
    }

    int32 ret = -1;
    CModelInfo::GetModelInfo(modelName, &ret);
    return static_cast<uint32>(ret);
}

// inlined
const char* CShopping::GetNameTag(uint32 itemKey) {
    return ms_prices[FindItem(itemKey)].nameTag;
}

/*!
 * @brief Finds the next 'root' section name from the file
 * @addr 0x49AF10
 * @param file A shopping.dat-like formatted file.
 * @returns The file's read pointer right after the next section, nullptr if no next section is available.
 */
const char* CShopping::GetNextSection(FILESTREAM file) {
    auto line = CFileLoader::LoadLine(file);
    if (!line)
        return nullptr;

    while (true) {
        if (*line != '\0' && *line != '#') {
            if (!strncmp(line, "section", 7u)) {
                break;
            }

            if (!strncmp(line, "end", 3u)) {
                return nullptr;
            }
        }

        line = CFileLoader::LoadLine(file);
        if (!line) {
            return nullptr;
        }
    }

    char* lastToken{};
    RET_IGNORED(strtok_s(line, " \t", &lastToken));
    return strtok_s(nullptr, " \t", &lastToken);
}

// 0x49AD50
int32 CShopping::GetPrice(uint32 itemKey) {
    return GetPriceMultipliedByLevel(ms_prices[FindItem(itemKey)].price);
}

// 0x49AAD0
ePriceSection CShopping::GetPriceSectionFromName(const char* name) {
    for (const auto&& [i, sectionName] : notsa::enumerate(ms_sectionNames)) {
        if (!_stricmp(name, sectionName)) {
            return static_cast<ePriceSection>(i);
        }
    }

    NOTSA_UNREACHABLE();
}

// 0x49B610
void CShopping::SetPlayerHasBought(uint32 itemKey) {
    ms_bHasBought[GetItemIndex(itemKey)] = true;
}

// 0x49B5E0
bool CShopping::HasPlayerBought(uint32 itemKey) {
    return ms_bHasBought[GetItemIndex(itemKey)];
}

/*!
 * @brief Increments the stat from stat index
 * @addr 0x49BE70
 * @param statIndex Index of a stat list described in the function.
 * @param change Change value of the stat.
 */
void CShopping::IncrementStat(uint32 statIndex, int32 change) {
    if (statIndex == -1)
        return;

    static constexpr eStats indexStats[] = {
        STAT_FAT, STAT_CLOTHES_RESPECT, STAT_APPEARANCE,
        STAT_RIOT_MISSION_ACCOMPLISHED, STAT_STAMINA, STAT_CALORIES
    };

    IncrementStat2(indexStats[statIndex], change);
}

/*!
 * @brief Increments the stat from real stat id
 * @addr 0x49AFD0 (inlined)
 * @param stat Stat id
 * @param change Change value of the stat.
 */
void CShopping::IncrementStat2(eStats stat, int32 change) {
    assert(stat != (eStats)(-1));

    switch (stat) {
    case STAT_CLOTHES_RESPECT:
    case STAT_APPEARANCE:
        change *= 10;
        break;
    default:
        break;
    }

    CStats::ModifyStat(stat, static_cast<float>(change));
}

// 0x49B8D0
void CShopping::LoadPrices(const char* sectionName) {
    const auto priceSection = GetPriceSectionFromName(sectionName);
    if (priceSection == ms_priceSectionLoaded)
        return;

    if (ms_priceSectionLoaded != PRICE_SECTION_NONE) {
        RemoveLoadedPrices();
    }
    ms_priceSectionLoaded = priceSection;
    ms_numPrices = 0;
    ms_numItemsInShop = 0;
    CTimer::Suspend();

    const auto file = CFileMgr::OpenFile("data\\shopping.dat", "r");
    if (FindSection(file, "prices")) {
        FindSection(file, sectionName);
    }

    for (auto line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file), ms_numPrices++) {
        if (*line == '\0' || *line == '#')
            continue;

        if (!strncmp(line, "end", 3u))
            break;

        auto& priceInfo = ms_prices[ms_numPrices];

        char* lastToken{};
        const auto model = strtok_s(line, " \t,", &lastToken);
        priceInfo.key = GetKey(model, ms_priceSectionLoaded);

        const auto nameTag = strtok_s(nullptr, " \t,", &lastToken);
        strncpy_s(priceInfo.nameTag, nameTag, 8u);

        switch (ms_priceSectionLoaded) {
        case PRICE_SECTION_CLOTHES:
        case PRICE_SECTION_HAIRCUTS: {
            priceInfo.clothes.modelKey = CKeyGen::GetUppercaseKey(strtok_s(nullptr, " \t,", &lastToken));
            priceInfo.clothes.type = std::atoi(strtok_s(nullptr, " \t,", &lastToken));
            break;
        }
        case PRICE_SECTION_TATTOOS: {
            const auto type = strtok_s(nullptr, " \t,", &lastToken);
            const auto txtkey = strtok_s(nullptr, " \t,", &lastToken);
            priceInfo.tattoos.type1 = (type[0] == '-') ? -1 : std::atoi(type);
            priceInfo.tattoos.texKey = CKeyGen::GetUppercaseKey(txtkey);
            break;
        }
        case PRICE_SECTION_WEAPONS: {
            priceInfo.weapon.ammo = std::atoi(strtok_s(nullptr, " \t,", &lastToken));
            break;
        }
        default:
            break;
        }

        for (auto i = 0; i < 4; i++)
            RET_IGNORED(strtok_s(nullptr, " \t,", &lastToken));

        priceInfo.price = std::atoi(strtok_s(nullptr, " \t,", &lastToken));
        for (auto& priceModifier : ms_priceModifiers | rng::views::take((size_t)ms_numPriceModifiers)) {
            if (priceInfo.key == priceModifier.key) {
                priceInfo.price = priceModifier.price;
                break;
            }
        }
    }
    CFileMgr::CloseFile(file);

    const auto animBlockIndex = CAnimManager::GetAnimationBlockIndex(ms_sectionNames[ms_priceSectionLoaded]);
    if (animBlockIndex != -1) {
        CStreaming::RequestModel(IFPToModelId(animBlockIndex), STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY | STREAMING_GAME_REQUIRED);
        CStreaming::LoadAllRequestedModels(true);
    }
    CTimer::Resume();
}

// 0x49BBE0
void CShopping::LoadShop(const char* sectionName) {
    if (!_stricmp(sectionName, ms_shopLoaded))
        return;

    strcpy_s(ms_shopLoaded, sectionName);
    CTimer::Suspend();
    ms_numItemsInShop = 0;

    if (!_stricmp("bought", sectionName)) {
        rng::for_each_n(ms_prices.begin(), ms_numPrices, [](auto& price) {
            if (ms_bHasBought[GetItemIndex(price.key)])
                ms_shopContents[ms_numItemsInShop++] = price.key;
        });

        // FIX_BUGS: CTimer::Resume()?
    } else {
        const auto file = CFileMgr::OpenFile("data\\shopping.dat", "r");
        if (FindSection(file, "shops")) {
            FindSection(file, sectionName);
        }

        char sectionName[32];
        for (auto line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
            if (*line == '\0' || *line == '#')
                continue;

            if (!strncmp(line, "end", 3u))
                break;

            char* lastToken{};
            const auto type = strtok_s(line, " \t", &lastToken);
            if (!strcmp("type", type)) {
                strcpy_s(sectionName, strtok_s(nullptr, " \t", &lastToken));
                LoadPrices(sectionName);
            } else if (!strcmp("item", type)) {
                const auto veh = FindPlayerVehicle();
                const auto model = GetKey(strtok_s(nullptr, " \t", &lastToken), ms_priceSectionLoaded);

                if (ms_priceSectionLoaded != PRICE_SECTION_CAR_MODS || IsValidModForVehicle(model, veh)) {
                    ms_shopContents[ms_numItemsInShop++] = model;
                }
            }
        }
        CFileMgr::CloseFile(file);
        CTimer::Resume();
    }
}

// 0x49B6A0
void CShopping::LoadStats() {
    const auto file = CFileMgr::OpenFile("data\\shopping.dat", "r");
    FindSection(file, "prices");
    ms_numBuyableItems = 0u;

    const auto ProcessSection = [file](ePriceSection section) {
        for (auto line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
            if (*line == '\0' || *line == '#')
                continue;

            if (!strncmp(line, "end", 3u))
                break;

            char* lastToken{};
            ms_keys[ms_numBuyableItems] = GetKey(strtok_s(line, " \t,", &lastToken), section);
            ms_bHasBought[ms_numBuyableItems] = false;

            RET_IGNORED(strtok_s(nullptr, " \t,", &lastToken));

            switch (section) {
            case PRICE_SECTION_CLOTHES:
            case PRICE_SECTION_HAIRCUTS:
            case PRICE_SECTION_TATTOOS:
                RET_IGNORED(strtok_s(nullptr, " \t,", &lastToken));
                [[fallthrough]];

            case PRICE_SECTION_WEAPONS:
                RET_IGNORED(strtok_s(nullptr, " \t,", &lastToken));
                break;
            default:
                break;
            }

            for (auto& modifier : ms_statModifiers[ms_numBuyableItems].modifiers) {
                modifier = {
                    (int8)GetChangingStatIndex(strtok_s(nullptr, " \t,", &lastToken)),
                    (int8)std::atoi(strtok_s(nullptr, " \t,", &lastToken))
                };
            }

            ms_numBuyableItems++;
        }
    };

    for (auto section = GetNextSection(file); section; section = GetNextSection(file)) {
        ProcessSection(GetPriceSectionFromName(section));
    }

    CFileMgr::CloseFile(file);
}

// 0x49AC90
void CShopping::RemoveLoadedPrices() {
    const auto animBlockIndex = CAnimManager::GetAnimationBlockIndex(ms_sectionNames[ms_priceSectionLoaded]);
    if (animBlockIndex != -1) {
        CStreaming::SetModelIsDeletable(IFPToModelId(animBlockIndex));
    }
    ms_priceSectionLoaded = PRICE_SECTION_NONE;
}

// 0x49AE30
void CShopping::RemoveLoadedShop() {
    ms_shopLoaded[0] = '\0';
    RemoveLoadedPrices();
}

// 0x
void CShopping::AddPriceModifier(const char* name, const char* section, int32 price) {
    AddPriceModifier(GetKey(name, GetPriceSectionFromName(section)), price);
}

// 0x (inlined)
void CShopping::AddPriceModifier(uint32 key, int32 price) {
    // the code may not be same, can not test.
    for (auto& priceModifier : std::span{ms_priceModifiers.data(), (size_t)ms_numPriceModifiers}) {
        if (key == priceModifier.key) {
            priceModifier.price = price;
            return;
        }
    }

    ms_priceModifiers[ms_numPriceModifiers].key = key;
    ms_priceModifiers[ms_numPriceModifiers].price = price;
    ms_numPriceModifiers++;
}

// 0x
void CShopping::RemovePriceModifier(const char* name, const char* section) {
    RemovePriceModifier(GetKey(name, GetPriceSectionFromName(section)));
}

// 0x (inlined)
void CShopping::RemovePriceModifier(uint32 key) {
    if (ms_numPriceModifiers <= 0)
        return;

    for (const auto&& [i, priceMod] : notsa::enumerate(std::span{ms_priceModifiers.data(), (size_t)ms_numPriceModifiers})) {
        if (key == priceMod.key) {
            ms_numPriceModifiers--;
            if (ms_numPriceModifiers >= 1u) {
                ms_priceModifiers[i] = ms_priceModifiers[ms_numPriceModifiers];
            }
        }
    }
}

// 0x49B200
void CShopping::StoreClothesState() {
    memcpy(&gStoredClothesState, FindPlayerPed()->GetClothesDesc(), sizeof(CPedClothesDesc));
    gClothesHaveBeenStored = 1u;
}

// 0x49B280
void CShopping::StoreVehicleMods() {
    const auto veh = FindPlayerVehicle();
    std::copy_n(veh->m_anUpgrades.begin(), veh->m_anUpgrades.size(), gStoredVehicleMods.begin());

    if (!veh->IsAutomobile())
        return;

    const auto& damage = veh->AsAutomobile()->m_damageManager;
    for (const auto& [i, state] : notsa::enumerate(gComponentDamageState)) {
        switch (i) {
        case 2:
            state = (eDamageState)damage.GetWheelStatus(CAR_WHEEL_FRONT_RIGHT);
            break;
        case 4:
            state = (eDamageState)damage.GetWheelStatus(CAR_WHEEL_REAR_RIGHT);
            break;
        case 5:
            state = (eDamageState)damage.GetWheelStatus(CAR_WHEEL_FRONT_LEFT);
            break;
        case 7:
            state = (eDamageState)damage.GetWheelStatus(CAR_WHEEL_REAR_LEFT);
            break;
        case 12:
            state = (eDamageState)damage.GetPanelStatus(FRONT_BUMPER);
            break;
        case 13:
            state = (eDamageState)damage.GetPanelStatus(REAR_BUMPER);
            break;
        case 16:
            state = (eDamageState)damage.GetDoorStatus(DOOR_BONNET);
            break;
        case 17:
            state = (eDamageState)damage.GetDoorStatus(DOOR_BOOT);
            break;
        default:
            state = DAMAGE_STATE_OK;
            break;
        }
    }
}

// 0x49B240
void CShopping::RestoreClothesState() {
    auto clothesDesc = FindPlayerPed()->GetClothesDesc();
    clothesDesc = &gStoredClothesState;
    gClothesHaveBeenStored = 0u;
}

// 0x49B3C0
void CShopping::RestoreVehicleMods() {
    const auto veh = FindPlayerVehicle();

    for (auto&& [i, storedMod] : notsa::enumerate(gStoredVehicleMods)) {
        auto& upgrade = veh->m_anUpgrades[i];

        if (upgrade != -1) {
            veh->RemoveVehicleUpgrade(upgrade);
        }
        upgrade = storedMod;
        if (upgrade != -1) {
            CStreaming::RequestVehicleUpgrade(upgrade, STREAMING_GAME_REQUIRED | STREAMING_KEEP_IN_MEMORY);
        }
    }

    CStreaming::LoadAllRequestedModels(false);
    veh->SetupUpgradesAfterLoad();

    for (const auto& mod : gStoredVehicleMods) {
        if (mod != -1)
            CStreaming::SetModelIsDeletable(mod);
    }

    if (!veh->IsAutomobile())
        return;

    auto& damage = veh->AsAutomobile()->m_damageManager;
    for (const auto& [i, state] : notsa::enumerate(gComponentDamageState)) {
        if (state == DAMAGE_STATE_OK)
            continue;

        switch (i) {
        case 2:
            damage.SetWheelStatus(CAR_WHEEL_FRONT_RIGHT, (eCarWheelStatus)state);
            break;
        case 4:
            damage.SetWheelStatus(CAR_WHEEL_REAR_RIGHT, (eCarWheelStatus)state);
            break;
        case 5:
            damage.SetWheelStatus(CAR_WHEEL_FRONT_LEFT, (eCarWheelStatus)state);
            break;
        case 7:
            damage.SetWheelStatus(CAR_WHEEL_REAR_LEFT, (eCarWheelStatus)state);
            break;
        case 12:
            damage.SetPanelStatus(FRONT_BUMPER, (ePanelDamageState)state);
            break;
        case 13:
            damage.SetPanelStatus(REAR_BUMPER, (ePanelDamageState)state);
            break;
        case 16:
            damage.SetDoorStatus(DOOR_BONNET, (eDoorStatus)state);
            break;
        case 17:
            damage.SetDoorStatus(DOOR_BOOT, (eDoorStatus)state);
            break;
        default:
            break;
        }
    }
}

// 0x49B1F0 (unused)
void CShopping::SetCurrentProperty(CMultiBuilding* property) {
    gpCurrentProperty = property;
}

// 0x49BEF0
void CShopping::UpdateStats(size_t index, bool increment) {
    for (const auto& modifier : ms_statModifiers[index].modifiers) {
        IncrementStat(modifier.statIndex, (!increment) ? -modifier.change : modifier.change);
    }
}

// 0x5D3E40
void CShopping::Load() {
    CGenericGameStorage::LoadDataFromWorkBuffer(&ms_numPriceModifiers, sizeof(int32));
    for (auto i = 0; i < ms_numPriceModifiers; i++) {
        CGenericGameStorage::LoadDataFromWorkBuffer(&ms_priceModifiers[i], sizeof(PriceModifier));
    }

    CGenericGameStorage::LoadDataFromWorkBuffer(&ms_numBuyableItems, sizeof(uint32));
    CGenericGameStorage::LoadDataFromWorkBuffer(&ms_bHasBought, ms_numBuyableItems);
}

// 0x5D3DE0
void CShopping::Save() {
    CGenericGameStorage::SaveDataToWorkBuffer(&ms_numPriceModifiers, sizeof(int32));
    for (auto i = 0; i < ms_numPriceModifiers; i++) {
        CGenericGameStorage::SaveDataToWorkBuffer(&ms_priceModifiers[i], sizeof(PriceModifier));
    }

    CGenericGameStorage::SaveDataToWorkBuffer(&ms_numBuyableItems, sizeof(uint32));
    CGenericGameStorage::SaveDataToWorkBuffer(&ms_bHasBought, ms_numBuyableItems);
}
