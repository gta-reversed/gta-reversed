/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Pickups.h"
#include "TaskSimpleJetPack.h"

int32& CollectPickupBuffer = *(int32*)0x97D644;

void CPickups::InjectHooks() {
    RH_ScopedClass(CPickups);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x454A70);
    // RH_ScopedInstall(ReInit, 0x456E60);
    // RH_ScopedInstall(AddToCollectedPickupsArray, 0x455240);
    // RH_ScopedInstall(CreatePickupCoorsCloseToCoors, 0x458A80);
    // RH_ScopedInstall(CreateSomeMoney, 0x458970);
    // RH_ScopedInstall(DetonateMinesHitByGunShot, 0x4590C0);
    // RH_ScopedInstall(DoCollectableEffects, 0x455E20);
    // RH_ScopedInstall(DoMineEffects, 0x4560E0);
    // RH_ScopedInstall(DoMoneyEffects, 0x454E80);
    // RH_ScopedInstall(DoPickUpEffects, 0x455720);
    // RH_ScopedInstall(FindPickUpForThisObject, 0x4551C0);
    // RH_ScopedInstall(GenerateNewOne, 0x456F20);
    // RH_ScopedInstall(GenerateNewOne_WeaponType, 0x457380);
    // RH_ScopedInstall(GetActualPickupIndex, 0x4552A0);
    // RH_ScopedInstall(GetNewUniquePickupIndex, 0x456A30);
    // RH_ScopedInstall(GetUniquePickupIndex, 0x455280);
    RH_ScopedInstall(GivePlayerGoodiesWithPickUpMI, 0x4564F0);
    RH_ScopedInstall(IsPickUpPickedUp, 0x454B40);
    RH_ScopedInstall(ModelForWeapon, 0x454AC0);
    RH_ScopedInstall(PassTime, 0x455200);
    RH_ScopedInstall(PickedUpHorseShoe, 0x455390);
    RH_ScopedInstall(PickedUpOyster, 0x4552D0);
    // RH_ScopedInstall(PictureTaken, 0x456A70);
    RH_ScopedInstall(PlayerCanPickUpThisWeaponTypeAtThisMoment, 0x4554C0);
    // RH_ScopedInstall(RemoveMissionPickUps, 0x456DE0);
    // RH_ScopedInstall(RemovePickUp, 0x4573D0);
    // RH_ScopedInstall(RemovePickUpsInArea, 0x456D30);
    // RH_ScopedInstall(RemovePickupObjects, 0x455470);
    // RH_ScopedInstall(RemoveUnnecessaryPickups, 0x4563A0);
    // RH_ScopedInstall(RenderPickUpText, 0x455000);
    // RH_ScopedInstall(TestForPickupsInBubble, 0x456450);
    // RH_ScopedInstall(TryToMerge_WeaponType, 0x4555A0);
    // RH_ScopedInstall(Update, 0x458DE0);
    // RH_ScopedInstall(UpdateMoneyPerDay, 0x455680);
    // RH_ScopedInstall(WeaponForModel, 0x454AE0);
    // RH_ScopedInstall(Load, 0x5D35A0);
    // RH_ScopedInstall(Save, 0x5D3540);
}

// 0x454A70
void CPickups::Init() {
    NumMessages = 0;
    for (auto& pickup : aPickUps) {
        pickup.m_nPickupType = PICKUP_NONE;
        pickup.m_nReferenceIndex = 1;
        pickup.m_pObject = nullptr;
    }
    std::ranges::fill(aPickUpsCollected, 0);
    CollectedPickUpIndex = 0;
    DisplayHelpMessage = 10;
}

// 0x456E60
void CPickups::ReInit() {
    plugin::Call<0x456E60>();
}

// 0x455240
void CPickups::AddToCollectedPickupsArray(int32 handle) {
    plugin::Call<0x455240, int32>(handle);
}

// 0x458A80
/*!
* @addr 0x458A80
* @brief Created a pickup close to pos (\r in_x, \r in_y, \r in_z)
*
* @param [out] out_x, out_y, out_z Created pickup's position
*/
void CPickups::CreatePickupCoorsCloseToCoors(float in_x, float in_y, float in_z, float* out_x, float* out_y, float* out_z) {
    plugin::Call<0x458A80, float, float, float, float*, float*, float*>(in_x, in_y, in_z, out_x, out_y, out_z);
}

// 0x458970
void CPickups::CreateSomeMoney(CVector coors, int32 amount) {
    plugin::Call<0x458970, CVector, int32>(coors, amount);
}

// 0x4590C0
void CPickups::DetonateMinesHitByGunShot(CVector* shotOrigin, CVector* shotTarget) {
    plugin::Call<0x4590C0, CVector*, CVector*>(shotOrigin, shotTarget);
}

// 0x455E20
void CPickups::DoCollectableEffects(CEntity* entity) {
    plugin::Call<0x455E20, CEntity*>(entity);
}

// 0x4560E0
void CPickups::DoMineEffects(CEntity* entity) {
    plugin::Call<0x4560E0, CEntity*>(entity);
}

// 0x454E80
void CPickups::DoMoneyEffects(CEntity* entity) {
    plugin::Call<0x454E80, CEntity*>(entity);
}

// 0x455720
void CPickups::DoPickUpEffects(CEntity* entity) {
    plugin::Call<0x455720, CEntity*>(entity);
}

// 0x4551C0
CPickup* CPickups::FindPickUpForThisObject(CObject* object) {
    return plugin::CallAndReturn<CPickup*, 0x4551C0, CObject*>(object);
}

// 0x456F20
int32 CPickups::GenerateNewOne(CVector coors, uint32 modelId, uint8 pickupType, uint32 ammo, uint32 moneyPerDay, bool isEmpty, char* message) {
    return plugin::CallAndReturn<int32, 0x456F20, CVector, uint32, uint8, uint32, uint32, bool, char*>(coors, modelId, pickupType, ammo, moneyPerDay, isEmpty, message);
}

// 0x457380
int32 CPickups::GenerateNewOne_WeaponType(CVector coors, eWeaponType weaponType, uint8 pickupType, uint32 ammo, bool isEmpty, char* message) {
    return plugin::CallAndReturn<int32, 0x457380, CVector, eWeaponType, uint8, uint32, bool, char*>(coors, weaponType, pickupType, ammo, isEmpty, message);
}

// 0x4552A0
int32 CPickups::GetActualPickupIndex(int32 pickupIndex) {
    return plugin::CallAndReturn<int32, 0x4552A0, int32>(pickupIndex);
}

// 0x456A30
int32 CPickups::GetNewUniquePickupIndex(int32 pickupIndex) {
    return plugin::CallAndReturn<int32, 0x456A30, int32>(pickupIndex);
}

// 0x455280
int32 CPickups::GetUniquePickupIndex(int32 pickupIndex) {
    return plugin::CallAndReturn<int32, 0x455280, int32>(pickupIndex);
}

// 0x4564F0
bool CPickups::GivePlayerGoodiesWithPickUpMI(uint16 modelId, int32 playerId) {
    auto* ped = FindPlayerPed(playerId);

    if (modelId == ModelIndices::MI_PICKUP_ADRENALINE) {
        ped->m_pPlayerData->m_bAdrenaline = true;
        ped->m_pPlayerData->m_nAdrenalineEndTime = CTimer::GetTimeInMS() + 20'000;
        ped->ResetSprintEnergy();
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_ADRENALINE);
        return true;
    }

    if (modelId == ModelIndices::MI_PICKUP_BODYARMOUR) {
        ped->m_fArmour = (float)FindPlayerInfo(playerId).m_nMaxArmour;
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_BODY_ARMOUR);
        return true;
    }

    if (modelId == ModelIndices::MI_PICKUP_INFO) {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_INFO);
        return true;
    }

    if (modelId == ModelIndices::MI_PICKUP_HEALTH) {
        auto maxHealth = FindPlayerInfo(playerId).m_nMaxHealth;
        CStats::UpdateStatsAddToHealth((uint32)((float)maxHealth - ped->m_fHealth));
        ped->m_fHealth = (float)maxHealth;
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_HEALTH);
        return true;
    }

    if (modelId == ModelIndices::MI_PICKUP_BONUS) {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_INFO);
        return true;
    }

    if (modelId == ModelIndices::MI_PICKUP_BRIBE) {
        auto wantedLevel = std::max(0u, FindPlayerPed()->GetWantedLevel() - 1);
        FindPlayerPed(0)->SetWantedLevel(wantedLevel);
        CStats::IncrementStat(STAT_NUMBER_OF_POLICE_BRIBES, 1.0f);
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_INFO);
        return true;
    }

    if (modelId == ModelIndices::MI_PICKUP_KILLFRENZY) {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_INFO);
        return true;
    }

    if (modelId == MODEL_JETPACK) {
        auto* task = new CTaskSimpleJetPack(nullptr, 10.0f, 0, nullptr);
        CEventScriptCommand event(3, task, 0);
        ped->GetEventGroup().Add(&event, false);
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_INFO);
        return true;
    }

    if (modelId == ModelIndices::MI_OYSTER) {
        PickedUpOyster();
        return true;
    }

    if (modelId == ModelIndices::MI_HORSESHOE) {
        PickedUpHorseShoe();
        return true;
    }

    return false;
}

/*!
* @brief Check if pickup was picked up, and then mark it as not picked up.
* @addr 0x454B40
*/
bool CPickups::IsPickUpPickedUp(int32 pickupHandle) {
    if (const auto it = rng::find(aPickUpsCollected, pickupHandle); it != aPickUpsCollected.end()) {
        *it = NULL; // Reset 
        return true;
    }
    return false;
}

/*!
* @addr 0x454AC0
* @returns The `nModelId1` of the given weapon type.
*/
int32 CPickups::ModelForWeapon(eWeaponType weaponType) {
    return CWeaponInfo::GetWeaponInfo(weaponType)->m_nModelId1;
}

/*!
* @brief Update each pickup's (except if of type NONE or ASSET_REVENUE) `nRegenerationTime` field.
* @addr 0x455200
*/
void CPickups::PassTime(uint32 time) {
    for (auto& p : aPickUps) {
        switch (p.m_nPickupType) {
        case ePickupType::PICKUP_NONE:
        case ePickupType::PICKUP_ASSET_REVENUE:
            continue;
        }

        if (p.m_nRegenerationTime <= time) {
            p.m_nRegenerationTime = 0;
        } else {
            p.m_nRegenerationTime -= time;
        }
    }
}

// 0x455390
void CPickups::PickedUpHorseShoe() {
    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_COLLECTABLE1);

    CStats::IncrementStat(STAT_HORSESHOES_COLLECTED);
    CStats::IncrementStat(STAT_LUCK, 1000.f / CStats::GetStatValue(STAT_TOTAL_HORSESHOES)); // TODO: Is this some inlined stuff? (The divion part)

    FindPlayerInfo(0).m_nMoney += 100;

    if (CStats::GetStatValue(STAT_TOTAL_HORSESHOES) == CStats::GetStatValue(STAT_HORSESHOES_COLLECTED)) { // Check if there's more to collect or not
        CGarages::TriggerMessage("HO_ALL");
        FindPlayerInfo(0).m_nMoney += 1000;
    } else {
        CGarages::TriggerMessage("HO_ONE", (int16)CStats::GetStatValue(STAT_TOTAL_HORSESHOES), 5000u, (int16)CStats::GetStatValue(STAT_HORSESHOES_COLLECTED));
    }
}

// 0x4552D0
void CPickups::PickedUpOyster() {
    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_COLLECTABLE1);

    CStats::IncrementStat(STAT_OYSTERS_COLLECTED);

    FindPlayerInfo(0).m_nMoney += 100;

    if (CStats::GetStatValue(STAT_TOTAL_OYSTERS) == CStats::GetStatValue(STAT_OYSTERS_COLLECTED)) { // Check if there's more to collect or not
        CGarages::TriggerMessage("OY_ALL");
        FindPlayerInfo(0).m_nMoney += 100'000;
    } else {
        CGarages::TriggerMessage("OY_ONE", (int16)CStats::GetStatValue(STAT_TOTAL_OYSTERS), 5000u, (int16)CStats::GetStatValue(STAT_OYSTERS_COLLECTED));
    }
}

// 0x456A70
void CPickups::PictureTaken() {
    plugin::Call<0x456A70>();
}

// 0x4554C0
bool CPickups::PlayerCanPickUpThisWeaponTypeAtThisMoment(eWeaponType weaponType) {
    if (!CWeaponInfo::GetWeaponInfo(weaponType)->flags.bAimWithArm) {
        if (FindPlayerPed()->GetIntelligence()->GetTaskJetPack()) {
            return false;
        }
    }
    return true;
}

// 0x456DE0
void CPickups::RemoveMissionPickUps() {
    plugin::Call<0x456DE0>();
}

// 0x4573D0
void CPickups::RemovePickUp(int32 pickupHandle) {
    plugin::Call<0x4573D0, int32>(pickupHandle);
}

// 0x456D30
void CPickups::RemovePickUpsInArea(float cornerA_x, float cornerA_y, float cornerA_z, float cornerB_x, float cornerB_y, float cornerB_z) {
    plugin::Call<0x456D30, float, float, float, float, float, float>(cornerA_x, cornerA_y, cornerA_z, cornerB_x, cornerB_y, cornerB_z);
}

// 0x455470
void CPickups::RemovePickupObjects() {
    plugin::Call<0x455470>();
}

// 0x4563A0
void CPickups::RemoveUnnecessaryPickups(const CVector& posn, float radius) {
    plugin::Call<0x4563A0, const CVector&, float>(posn, radius);
}

// 0x455000
void CPickups::RenderPickUpText() {
    plugin::Call<0x455000>();
}

// 0x456450
bool CPickups::TestForPickupsInBubble(CVector posn, float radius) {
    return plugin::CallAndReturn<bool, 0x456450, CVector, float>(posn, radius);
}

// 0x4555A0
bool CPickups::TryToMerge_WeaponType(CVector posn, eWeaponType weaponType, uint8 pickupType, uint32 ammo, bool _IGNORED_ arg4) {
    return plugin::CallAndReturn<bool, 0x4555A0, CVector, eWeaponType, uint8, uint32, bool>(posn, weaponType, pickupType, ammo, arg4);
}

// 0x458DE0
void CPickups::Update() {
    plugin::Call<0x458DE0>();
}

// 0x455680
void CPickups::UpdateMoneyPerDay(int32 pickupHandle, uint16 money) {
    plugin::Call<0x455680, int32, uint16>(pickupHandle, money);
}

// 0x454AE0
int32 CPickups::WeaponForModel(int32 modelId) {
    return plugin::CallAndReturn<int32, 0x454AE0, int32>(modelId);
}

// 0x5D35A0
bool CPickups::Load() {
    return plugin::CallAndReturn<bool, 0x5D35A0>();
}
// 0x5D3540
bool CPickups::Save() {
    return plugin::CallAndReturn<bool, 0x5D3540>();
}

/*!
* @brief Our custom Vector based overload
* @copydocs CPickups::CreatePickupCoorsCloseToCoors
*/
void CPickups::CreatePickupCoorsCloseToCoors(const CVector& pos, CVector& createdAtPos) {
    return CreatePickupCoorsCloseToCoors(pos.x, pos.y, pos.z, &createdAtPos.x, &createdAtPos.y, &createdAtPos.z);
}

/*!
* @brief Our custom Vector based overload
* @copydocs CPickups::CreatePickupCoorsCloseToCoors
*/
void CPickups::CreatePickupCoorsCloseToCoors(const CVector& pos, float& out_x, float& out_y, float& out_z) {
    return CreatePickupCoorsCloseToCoors(pos.x, pos.y, pos.z, &out_x, &out_y, &out_z);
}

// 0x454B70
void ModifyStringLabelForControlSetting(char* stringLabel) {
    plugin::Call<0x454B70, char*>(stringLabel);
}
