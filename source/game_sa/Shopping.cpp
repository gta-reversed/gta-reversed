#include "StdInc.h"
#include <extensions/enumerate.hpp>
#include "Shopping.h"
#include "PedClothesDesc.h"
#include <AECutsceneTrackManager.h>
#include <AEAmbienceTrackManager.h>
#include <AEAudioHardware.h>
#include <AESoundManager.h>

CPedClothesDesc& gStoredClothesState = *(CPedClothesDesc*)0xA9A810;
char& gClothesHaveBeenStored = *(char*)0xA97298;
std::array<eDamageState, NUM_COMPONENTS>& gComponentDamageState = *(std::array<eDamageState, 20u>*)0xA97258;
std::array<int16, NUM_VEHICLE_UPGRADES>& gStoredVehicleMods = *(std::array<int16, 15u>*)0xA97274;
std::array<float, NUM_LEVELS>& gPriceMultipliers = *(std::array<float, NUM_LEVELS>*)0x8A6204;
CMultiBuilding* gpCurrentProperty = (CMultiBuilding*)0x0;

void CShopping::InjectHooks() {
    RH_ScopedClass(CShopping);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x49C290);
    //RH_ScopedOverloadedInstall(AddPriceModifier, 0x0, { .reversed = false });
    //RH_ScopedOverloadedInstall(AddPriceModifier, 0x0, { .reversed = false });
    RH_ScopedInstall(Buy, 0x49BF70, { .reversed = false });
    // RH_ScopedInstall(FindItem, 0x0, { .reversed = false });
    RH_ScopedInstall(FindSection, 0x49AE70, { .reversed = false });
    RH_ScopedInstall(FindSectionInSection, 0x49AF90);
    RH_ScopedInstall(GetExtraInfo, 0x49ADE0);
    RH_ScopedInstall(GetItemIndex, 0x49AB10);
    RH_ScopedInstall(GetKey, 0x49AB30, { .reversed = false });
    // RH_ScopedInstall(GetNameTag, 0x0, { .reversed = false });
    RH_ScopedInstall(GetNextSection, 0x49AF10, { .reversed = false });
    RH_ScopedInstall(GetPrice, 0x49AD50);
    RH_ScopedInstall(GetPriceSectionFromName, 0x49AAD0);
    RH_ScopedInstall(SetPlayerHasBought, 0x49B610, {.reversed = false});
    RH_ScopedInstall(HasPlayerBought, 0x49B5E0, { .reversed = false });
    // RH_ScopedInstall(IncrementStat, 0x0, { .reversed = false });
    // RH_ScopedInstall(IncrementStat2, 0x0, { .reversed = false });
    RH_ScopedInstall(LoadPrices, 0x49B8D0, { .reversed = false });
    RH_ScopedInstall(LoadShop, 0x49BBE0, { .reversed = false });
    RH_ScopedInstall(LoadStats, 0x49B6A0, { .reversed = false });
    // RH_ScopedInstall(RemoveLoadedPrices, 0x0, { .reversed = false });
    RH_ScopedInstall(RemoveLoadedShop, 0x49AE30);
    //RH_ScopedInstall(RemovePriceModifier, 0x0, { .reversed = false });
    //RH_ScopedInstall(RemovePriceModifier, 0x0, { .reversed = false });
    RH_ScopedInstall(RestoreClothesState, 0x49B240);
    RH_ScopedInstall(RestoreVehicleMods, 0x49B3C0);
    RH_ScopedInstall(ShutdownForRestart, 0x49B640);
    RH_ScopedInstall(StoreClothesState, 0x49B200);
    RH_ScopedInstall(StoreVehicleMods, 0x49B280);
    // RH_ScopedInstall(UpdateStats, 0x0, { .reversed = false });
    RH_ScopedInstall(Load, 0x5D3E40, { .reversed = false });
    RH_ScopedInstall(Save, 0x5D3DE0, { .reversed = false });
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

// Used only in CShopping::LoadStats()
// 0x49ABD0
int32 GetChangingStatIndex(const char* stat) {
    static constexpr const char* statNames[] = {
        "fat", "respect", "sexy", "health", "calories"
    };

    // todo: use notsa::enumerate
    for (auto i = 0u; i < std::size(statNames); i++) {
        if (!strcmp(statNames[i], stat)) {
            return i;
        }
    }

    return -1;
}

// 0x
void CShopping::AddPriceModifier(const char* a1, const char* a2, int32 a3) {
    plugin::Call<0x0, const char*, const char*, int32>(a1, a2, a3);
}

// 0x
void CShopping::AddPriceModifier(uint32 a1, int32 a2) {
    plugin::Call<0x0, uint32, int32>(a1, a2);
}

// 0x49BF70
void CShopping::Buy(uint32 a1, int32 a2) {
    plugin::Call<0x49BF70, uint32, int32>(a1, a2);
}

// inlined
int32 CShopping::FindItem(uint32 itemKey) {
    auto itemId = -1;
    if (ms_numPrices >= 1) {
        for (auto&& [i, p] : notsa::enumerate(std::span{ms_prices.data(), (size_t)ms_numPrices})) {
            if (p.key == itemKey) {
                itemId = i;
            }
        }
    }

    assert(itemId != -1);
    return itemId;
}

// 0x49AE70
bool CShopping::FindSection(FILESTREAM file, const char* sectionName) {
    return plugin::CallAndReturn<bool, 0x49AE70, FILESTREAM, const char*>(file, sectionName);
}

// unused
// 0x49AF90
bool CShopping::FindSectionInSection(FILESTREAM file, const char* parentSection, const char* childSection) {
    return FindSection(file, parentSection) && FindSection(file, childSection);
}

// 0x49ADE0
int32 CShopping::GetExtraInfo(uint32 itemKey, int32 index) {
    switch (auto id = FindItem(itemKey)) {
    case 0:
        return ms_prices[id].general.extra1;
    case 1:
        return ms_prices[id].general.extra2;
    default:
        break;
    }

    return 0;
}

// 0x49AB10
int32 CShopping::GetItemIndex(uint32 itemKey) {
    for (auto&& [i, key] : notsa::enumerate(ms_keys)) {
        if (key == itemKey) {
            return i;
        }
    }

    assert(false);
    return -1;
}

// 0x49AB30
void CShopping::GetKey(const char* modelName, int32 index) {
    plugin::Call<0x49AB30, const char*, int32>(modelName, index);
}

// inlined
const char* CShopping::GetNameTag(uint32 itemKey) {
    return ms_prices[FindItem(itemKey)].nameTag;
}

// 0x49AF10
void CShopping::GetNextSection(FILE* file) {
    plugin::Call<0x49AF10, FILE*>(file);
}

// 0x49AD50
int32 CShopping::GetPrice(uint32 itemKey) {
    return GetPriceMultipliedByLevel(ms_prices[FindItem(itemKey)].price);
}

// 0x49AAD0
int32 CShopping::GetPriceSectionFromName(const char* name) {
    for (auto&& [i, sectionName] : notsa::enumerate(ms_sectionNames)) {
        if (!_stricmp(name, sectionName)) {
            return i;
        }
    }

    assert(false);
    return -1;
}

// 0x49B610
void CShopping::SetPlayerHasBought(uint32 itemKey) {
    ms_bHasBought[GetItemIndex(itemKey)] = true;
}

// 0x49B5E0
bool CShopping::HasPlayerBought(uint32 itemKey) {
    return ms_bHasBought[GetItemIndex(itemKey)];
}

// 0x
void CShopping::IncrementStat(int32 a1, int32 a2) {
    plugin::Call<0x0, int32, int32>(a1, a2);
}

// 0x
void CShopping::IncrementStat2(int32 a1, int32 a2) {
    plugin::Call<0x0, int32, int32>(a1, a2);
}

// 0x49B8D0
void CShopping::LoadPrices(const char* sectionName) {
    plugin::Call<0x49B8D0, const char*>(sectionName);
}

// 0x49BBE0
void CShopping::LoadShop(const char* sectionName) {
    plugin::Call<0x49BBE0, const char*>(sectionName);
}

// 0x49B6A0
void CShopping::LoadStats() {
    plugin::Call<0x49B6A0>();
}

// 0x
void CShopping::RemoveLoadedPrices() {
    auto animBlockIndex = CAnimManager::GetAnimationBlockIndex(ms_sectionNames[ms_priceSectionLoaded]);
    if (animBlockIndex != -1) {
        CStreaming::SetModelIsDeletable(IFPToModelId(animBlockIndex));
    }
    ms_priceSectionLoaded = 0;
}

// 0x49AE30
void CShopping::RemoveLoadedShop() {
    ms_shopLoaded[0] = false;
    RemoveLoadedPrices();
}

// 0x
void CShopping::RemovePriceModifier(const char* a1, const char* a2) {
    plugin::Call<0x0, const char*, const char*>(a1, a2);
}

// 0x
void CShopping::RemovePriceModifier(uint32 a1) {
    plugin::Call<0x0, uint32>(a1);
}

// 0x49B240
void CShopping::RestoreClothesState() {
    memcpy(FindPlayerPed()->m_pPlayerData->m_pPedClothesDesc, &gStoredClothesState, sizeof(CPedClothesDesc));
    gClothesHaveBeenStored = 0u;
}

// 0x49B3C0
void CShopping::RestoreVehicleMods() {
    auto veh = FindPlayerVehicle()->AsAutomobile();

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

    if (!veh->IsAutomobile())
        return;

    auto& damage = veh->m_damageManager;
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

// unused
// 0x
void CShopping::SetCurrentProperty(CMultiBuilding* property) {
    //gpCurrentProperty = property;
}

// 0x49B640
void CShopping::ShutdownForRestart() {
    RemoveLoadedShop();

    ms_numPriceModifiers = 0;
    rng::fill(ms_bHasBought, false);
}

// 0x49B200
void CShopping::StoreClothesState() {
    // todo: operator=
    memcpy(&gStoredClothesState, FindPlayerPed()->m_pPlayerData->m_pPedClothesDesc, sizeof(CPedClothesDesc));
    gClothesHaveBeenStored = 1u;
}

// 0x49B280
void CShopping::StoreVehicleMods() {
    auto veh = FindPlayerVehicle()->AsAutomobile();

    memcpy(&gStoredVehicleMods, &veh->m_anUpgrades, NUM_VEHICLE_UPGRADES);

    if (!veh->IsAutomobile())
        return;

    const auto& damage = veh->m_damageManager;
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

// 0x
void CShopping::UpdateStats(int32 a1, bool a2) {
    plugin::Call<0x0, int32, bool>(a1, a2);
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
