/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Pickups.h"
#include "Garages.h"
#include "tPickupMessage.h"
#include "Radar.h"
#include "Shadows.h"
#include "Coronas.h"
#include "PostEffects.h"
#include "TaskSimpleJetPack.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskComplexUseMobilePhone.h"

using namespace ModelIndices;

constexpr float PICKUP_ANGLE_STEP = 0.024543693f; // 2 PI / 256 - NOTE: The original's constant (0x859BBC), `TWO_PI / 256.f` is a few ulp off
constexpr auto  NUM_SPOT_ATTEMPTS = 32;

void CPickups::InjectHooks() {
    RH_ScopedClass(CPickups);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x454A70);
    RH_ScopedInstall(ReInit, 0x456E60);
    RH_ScopedInstall(AddToCollectedPickupsArray, 0x455240);
    RH_ScopedOverloadedInstall(CreatePickupCoorsCloseToCoors, "", 0x458A80, void(*)(float, float, float, float&, float&, float&));
    RH_ScopedInstall(CreateSomeMoney, 0x458970);
    RH_ScopedInstall(DetonateMinesHitByGunShot, 0x4590C0);
    RH_ScopedInstall(DoCollectableEffects, 0x455E20);
    RH_ScopedInstall(DoMineEffects, 0x4560E0);
    RH_ScopedInstall(DoMoneyEffects, 0x454E80);
    RH_ScopedInstall(DoPickUpEffects, 0x455720);
    RH_ScopedInstall(FindPickUpForThisObject, 0x4551C0);

    // Cannot be hooked at all for now due to ABI fuckery, the return value is 32 bit, but causes the function to assume the calling convention of of T* Function(T*, ...)
    //RH_ScopedInstall(GenerateNewOne, 0x456F20, { .Reversed = false });

    RH_ScopedInstall(GenerateNewOne_WeaponType, 0x457380);
    RH_ScopedInstall(GetActualPickupIndex, 0x4552A0);
    RH_ScopedInstall(GetNewUniquePickupIndex, 0x456A30);
    RH_ScopedInstall(GetUniquePickupIndex, 0x455280);
    RH_ScopedInstall(GivePlayerGoodiesWithPickUpMI, 0x4564F0);
    RH_ScopedInstall(IsPickUpPickedUp, 0x454B40);
    RH_ScopedInstall(ModelForWeapon, 0x454AC0);
    RH_ScopedInstall(PassTime, 0x455200);
    RH_ScopedInstall(PickedUpHorseShoe, 0x455390);
    RH_ScopedInstall(PickedUpOyster, 0x4552D0);
    RH_ScopedInstall(PictureTaken, 0x456A70);
    RH_ScopedInstall(PlayerCanPickUpThisWeaponTypeAtThisMoment, 0x4554C0);
    RH_ScopedInstall(RemoveMissionPickUps, 0x456DE0);
    RH_ScopedInstall(RemovePickUp, 0x4573D0);
    RH_ScopedInstall(RemovePickUpsInArea, 0x456D30);
    RH_ScopedInstall(RemovePickupObjects, 0x455470);
    RH_ScopedInstall(RemoveUnnecessaryPickups, 0x4563A0);
    RH_ScopedInstall(RenderPickUpText, 0x455000);
    RH_ScopedInstall(TestForPickupsInBubble, 0x456450);
    RH_ScopedInstall(TryToMerge_WeaponType, 0x4555A0);
    RH_ScopedInstall(Update, 0x458DE0);
    RH_ScopedInstall(UpdateMoneyPerDay, 0x455680);
    RH_ScopedInstall(WeaponForModel, 0x454AE0);
    RH_ScopedInstall(Load, 0x5D35A0);
    RH_ScopedInstall(Save, 0x5D3540);

    RH_ScopedGlobalInstall(ModifyStringLabelForControlSetting, 0x454B70);
}

// 0x454A70
void CPickups::Init() {
    ZoneScoped;

    ms_NumMessages = 0;
    for (auto& pickup : aPickUps) {
        pickup.m_nPickupType = PICKUP_NONE;
        pickup.m_nReferenceIndex = 1;
        pickup.m_pObject = nullptr;
    }
    ms_PickUpsCollected.fill(0);
    ms_CollectedPickUpIndex = 0u;
    ms_DisplayHelpMessage   = 10u;
}

// 0x456E60
void CPickups::ReInit() {
    rng::for_each(GetAllActivePickups(), [](auto& pickup) { pickup.Remove(); });
    Init();
}

// 0x455240
void CPickups::AddToCollectedPickupsArray(int32 pickupIndex) {
    ms_PickUpsCollected[ms_CollectedPickUpIndex++] = GetUniquePickupIndex(pickupIndex).num;
    ms_CollectedPickUpIndex %= std::size(ms_PickUpsCollected);
}

/*!
 * @addr 0x458A80
 * @brief Created a pickup close to pos (inX, inY, inZ)
 *
 * @param [out] outX, outY, outZ Created pickup's position
 */
void CPickups::CreatePickupCoorsCloseToCoors(float inX, float inY, float inZ, float& outX, float& outY, float& outZ) {
    for (auto attempt = 0; attempt < NUM_SPOT_ATTEMPTS; attempt++) {
        // 0x458AC9 - A random point on the ground around the position
        const auto angle = (float)(CGeneral::GetRandomNumber() & 0xFF) * PICKUP_ANGLE_STEP; // NOTE: Not `GetRandomNumberInRange`, the angle is quantized to 256 steps
        CVector    pos{ inX + std::sin(angle) * 1.5f, inY + std::cos(angle) * 1.5f, inZ };
        bool       isGroundFound{};
        pos.z = CWorld::FindGroundZFor3DCoord(pos, &isGroundFound, nullptr) + 0.5f;
        if (!isGroundFound) {
            continue;
        }

        // 0x458CB7 - Not too close to the player, nor to other pickups (Unless we're running out of attempts)
        const auto isDesperate = attempt > 16;
        if (CVector2D{ pos - FindPlayerCoors() }.SquaredMagnitude() > sq(2.f)) {
            if (!isDesperate && TestForPickupsInBubble(pos, 1.3f)) {
                continue;
            }
        } else if (!isDesperate) {
            continue;
        }

        // 0x458CF8 - It must be reachable from the original position
        const CVector origin{ inX, inY, inZ + 0.3f };
        const CVector dir = pos - origin;
        if (!CWorld::GetIsLineOfSightClear(origin + dir * ((dir.Magnitude() + 0.4f) / dir.Magnitude()), origin, true, attempt < 16, false, attempt < 16, false, false, false)) {
            continue;
        }
        if (!isDesperate && CWorld::TestSphereAgainstWorld(pos, 1.2f, nullptr, false, true, false, false, false, false)) {
            continue;
        }

        outX = pos.x;
        outY = pos.y;
        outZ = pos.z;
        return;
    }

    // 0x458D6C - Couldn't find a good spot, just put it there
    outX = inX;
    outY = inY;
    outZ = inZ + 0.4f;
}

/*!
 * @addr 0x458970
 * @brief Creates wads of money that is worth amount of money at the position coors.
 */
void CPickups::CreateSomeMoney(CVector coors, int32 amount) {
    const auto wads = std::min(amount / 20 + 1, 7);
    const auto perWad = amount / wads;

    CVector pos       = coors;
    for (auto i = 0; i < wads; i++) {
        // NOTE: Each coordinate gets its own random angle, quantized to 256 steps
        pos.x += std::sin((float)(CGeneral::GetRandomNumber() & 0xFF) * PICKUP_ANGLE_STEP) * 1.5f;
        pos.y += std::cos((float)(CGeneral::GetRandomNumber() & 0xFF) * PICKUP_ANGLE_STEP) * 1.5f;

        bool isGroundFound{};
        pos.z = CWorld::FindGroundZFor3DCoord(pos, &isGroundFound, nullptr) + 0.5f;
        if (isGroundFound) {
            GenerateNewOne(pos, MI_MONEY, PICKUP_MONEY, perWad + (CGeneral::GetRandomNumber() % 4));
        }
    }
}

// 0x4590C0
void CPickups::DetonateMinesHitByGunShot(const CVector& shotOrigin, const CVector& shotTarget) {
    for (auto& pickup : aPickUps) {
        if (pickup.m_nPickupType == PICKUP_NAUTICAL_MINE_ARMED) {
            pickup.ProcessGunShot(shotOrigin, shotTarget);
        }
    }
}

// 0x455E20
void CPickups::DoCollectableEffects(CEntity* entity) {
    const auto& entityPos = entity->GetPosition();

    if (const auto d = DistanceBetweenPoints(TheCamera.GetPosition(), entityPos); d < 14.0f) {
        // shade of gray
        const auto t = (uint8)((std::sinf((float)(((uint16)std::bit_cast<uintptr_t>(entity) + (uint16)CTimer::GetTimeInMS()) % 2'048) * 0.0030664064f) + 1.0f) * 0.5f * (14.0f - d) * (1.0f / 14.0f) * 255.0f);

        CShadows::StoreStaticShadow(
            (uint32)entity,
            SHADOW_ADDITIVE,
            gpShadowExplosionTex,
            entityPos,
            2.0f,
            0.0f,
            0.0f,
            -2.0f,
            0,
            t,
            t,
            t,
            4.0f,
            1.0f,
            40.0f,
            false,
            0.0f
        );
        CCoronas::RegisterCorona(
            (uint32)entity,
            nullptr,
            t,
            t,
            t,
            255,
            entityPos,
            0.6f,
            40.0f,
            CORONATYPE_TORUS,
            FLARETYPE_NONE,
            eCoronaReflType::CORREFL_NONE,
            eCoronaLOSCheck::LOSCHECK_OFF,
            eCoronaTrail::TRAIL_OFF,
            0.0f,
            false,
            1.5f,
            0,
            15.0f,
            false,
            false
        );
    }

    entity->GetMatrix().SetRotateZOnly(static_cast<float>(CTimer::GetTimeInMS() % 4096) * 0.0015283204f);
}

// 0x4560E0
void CPickups::DoMineEffects(CEntity* entity) {
    const auto& entityPos = entity->GetPosition();

    if (const auto d = DistanceBetweenPoints(TheCamera.GetPosition(), entityPos); d < 20.0f) {
        // shade of red
        const auto t = (uint8)((std::sinf((float)(((uint16)std::bit_cast<uintptr_t>(entity) + (uint16)CTimer::GetTimeInMS()) % 512) * 0.012265625f) + 1.0f) * 0.5f * (20.0f - d) * 0.05f * 64.0f);

        CShadows::StoreStaticShadow(
            (uint32)entity,
            SHADOW_ADDITIVE,
            gpShadowExplosionTex,
            entityPos,
            2.0f,
            0.0f,
            0.0f,
            -2.0f,
            0,
            t,
            0,
            0,
            4.0f,
            1.0f,
            40.0f,
            false,
            0.0f
        );
        CCoronas::RegisterCorona(
            (uint32)entity,
            nullptr,
            t,
            0,
            0,
            255,
            entityPos,
            0.6f,
            60.0f,
            CORONATYPE_TORUS,
            FLARETYPE_NONE,
            eCoronaReflType::CORREFL_NONE,
            eCoronaLOSCheck::LOSCHECK_OFF,
            eCoronaTrail::TRAIL_OFF,
            0.0f,
            false,
            1.5f,
            0,
            15.0f,
            false,
            false
        );
    }

    entity->GetMatrix().SetRotateZOnly(static_cast<float>(CTimer::GetTimeInMS() % 1024) * 0.0061132815f);
}

// 0x454E80
void CPickups::DoMoneyEffects(CEntity* entity) {
    entity->GetMatrix().SetRotateZOnly(static_cast<float>(CTimer::GetTimeInMS() % 2048) * 0.0030566407f);
}

// 0x455720
void CPickups::DoPickUpEffects(CEntity* entity) {
    auto* const object = entity->AsObject();
    auto* const pickup = FindPickUpForThisObject(object);
    const auto  mi     = object->GetModelId();

    if (mi == MI_PICKUP_CAMERA) {
        if (TheCamera.GetActiveCam().m_nMode == MODE_CAMERA) { // 0x455767
            object->objectFlags.bDoNotRender = false;
        } else {
            object->objectFlags.bDoNotRender = true;

            // 0x455793 - Glow at night (or with night/thermal vision on)
            if (CClock::GetGameClockHours() < 5 || CPostEffects::IsVisionFXActive()) {
                // NOTE: Not `GetRandomNumberInRange`, the original scales by 1 / 32768 and truncates towards zero
                const auto red = 100 - (int32)((float)(CGeneral::GetRandomNumber() & 0xFFFF) * (1.f / 32768.f) * -50.f);
                const auto gb  = (uint8)((double)red * (double)0.7f); // NOTE: The original multiplies in 80-bit precision, with `float` `110 * 0.7f` would round up to 77
                CCoronas::RegisterCorona(
                    reinterpret_cast<uint32>(&entity) + 1, // NOTE: The original uses a stack address as the id, so it's the same for all camera pickups
                    nullptr,
                    (uint8)red,
                    gb,
                    gb,
                    255,
                    object->GetPosition(),
                    (std::sin((float)(CTimer::GetTimeInMS() & 0x1FFF) * 0.0007666016f) + 1.7f) * 3.7f,
                    100.f,
                    CORONATYPE_HEADLIGHT,
                    FLARETYPE_NONE,
                    eCoronaReflType::CORREFL_NONE,
                    eCoronaLOSCheck::LOSCHECK_OFF,
                    eCoronaTrail::TRAIL_OFF,
                    0.f,
                    false,
                    1.5f,
                    false,
                    15.f,
                    false,
                    false
                );
            }
        }
    } else {
        object->objectFlags.bDoNotRender = pickup->PickUpShouldBeInvisible();
    }
    if (object->objectFlags.bDoNotRender) {
        return;
    }

    // 0x455885 - Index into `ms_PickupColours` for the text
    const auto colourIdx = [&]() -> int32 {
        // NOTE/BUG: The original leaves the index uninitialised for these models (It ends up being the low word of `entity`)
        const auto uninitialisedIdx = notsa::bugfixes::GenericUB ? 0 : (int32)(int16)(reinterpret_cast<uintptr_t>(entity) & 0xFFFF);

        if (mi == MI_PICKUP_ADRENALINE) {
            return 47;
        }
        if (mi == MI_PICKUP_BODYARMOUR) {
            return 48;
        }
        if (mi == MI_PICKUP_BRIBE || mi == MI_PICKUP_INFO || mi == MI_PICKUP_KILLFRENZY) {
            return uninitialisedIdx;
        }
        if (mi == MI_PICKUP_HEALTH || mi == MI_PICKUP_BONUS) {
            return 47;
        }
        if (mi == MI_PICKUP_PROPERTY) {
            return uninitialisedIdx;
        }
        if (mi == MI_PICKUP_PROPERTY_FORSALE) {
            return 47;
        }
        if (mi == MI_PICKUP_REVENUE) {
            return 53;
        }
        if (mi == MI_PICKUP_SAVEGAME) {
            return uninitialisedIdx;
        }
        if (mi == MI_PICKUP_CLOTHES) {
            return 47;
        }
        return WeaponForModel(mi);
    }();

    // 0x455943 - Show the price/bonus above shop and property pickups
    if (object->objectFlags.bPickupPropertyForSale || object->objectFlags.bPickupInShopOutOfStock || object->m_nBonusValue || object->m_wCostValue) {
        const auto distToCam = CVector2D{ TheCamera.GetPosition() - object->GetPosition() }.Magnitude();
        CVector    screenPos{};
        float      width{}, height{};
        if (distToCam < 14.f && ms_NumMessages < MAX_PICKUP_MESSAGES && CSprite::CalcScreenCoors(object->GetPosition() + CVector{ 0.f, 0.f, 0.7f }, &screenPos, &width, &height, true, true)) {
            const auto& colour     = ms_PickupColours.data()[colourIdx]; // NOTE: Not `[]`, the index may be out of range (See above)

            auto& msg              = ms_Messages[ms_NumMessages];
            msg.pos                = CVector2D{ screenPos };
            msg.width              = width;
            msg.height             = height;
            msg.weaponType         = WeaponForModel(mi);
            msg.color              = CRGBA{ colour.r, colour.g, colour.b, (uint8)((1.f - distToCam * (1.f / 14.f)) * 255.f) };
            msg.flags.IsOutOfStock = object->objectFlags.bPickupInShopOutOfStock;
            msg.bonus              = object->m_nBonusValue;
            msg.price              = object->m_wCostValue * 5u;
            if (mi == MI_PICKUP_PROPERTY) { // 0x455B61
                msg.text                    = TheText.Get(CPickup::FindStringForTextIndex((ePickupPropertyText)pickup->m_nFlags.nPropertyTextIndex));
                msg.flags.IsPropertyForSale = false;
            } else if (mi == MI_PICKUP_PROPERTY_FORSALE) { // 0x455BA6
                msg.text                    = TheText.Get(CPickup::FindStringForTextIndex((ePickupPropertyText)pickup->m_nFlags.nPropertyTextIndex));
                msg.flags.IsPropertyForSale = true;
            } else {
                msg.text                    = nullptr;
                msg.flags.IsPropertyForSale = false;
            }
            ms_NumMessages++;
        }
    }

    // 0x455C07 - Spin it, and scale it up if it's small
    const auto& bb        = object->GetModelInfo()->GetColModel()->GetBoundingBox();
    const auto  bbSize    = bb.GetSize();
    const auto  scale     = mi == MODEL_MINIGUN
        ? 1.2f
        : (std::max(1.2f / std::max({ bbSize.x, bbSize.y, bbSize.z }), 1.f) - 1.f) * 0.6f + 1.f;
    const auto  angle     = (float)(CTimer::GetTimeInMS() % 2'048) * 0.0030566407f;
    const auto  cosScaled = std::cos(angle) * scale;
    const auto  sinScaled = std::sin(angle) * scale;

    auto& mat             = object->GetMatrix();
    mat.GetRight()        = CVector{ cosScaled, sinScaled, 0.f };
    mat.GetForward()      = CVector{ -sinScaled, cosScaled, 0.f };
    mat.GetUp()           = CVector{ 0.f, 0.f, scale };
}

// 0x4551C0
CPickup* CPickups::FindPickUpForThisObject(CObject* object) {
    for (auto& pickup : GetAllActivePickups()) {
        if (pickup.m_pObject == object) {
            return &pickup;
        }
    }
    return aPickUps.data();
}

// returns pickup handle
// g
tPickupReference CPickups::GenerateNewOne(CVector coors, uint32 modelId, ePickupType pickupType, uint32 ammo, uint32 moneyPerDay, bool isEmpty, char* message) {
    auto retVal = plugin::CallAndReturn<int32, 0x456F20, CVector, uint32, ePickupType, uint32, uint32, bool, char*>(coors, modelId, pickupType, ammo, moneyPerDay, isEmpty, message);
    return tPickupReference(retVal);
}

/*!
 *
 * @param coors Position of new pickup
 * @param weaponType Weapon type
 * @param pickupType Pickup type
 * @param ammo
 * @param isEmpty
 * @param message
 * @return Pickup handle
 * @addr 0x457380
 */
tPickupReference CPickups::GenerateNewOne_WeaponType(CVector coors, eWeaponType weaponType, ePickupType pickupType, uint32 ammo, bool isEmpty, char* message) {
    return GenerateNewOne(coors, CWeaponInfo::GetWeaponInfo(weaponType)->m_nModelId1, pickupType, ammo, 0u, isEmpty, message);
}

/*!
 * @param pickupIndex Index of pickup
 * @return -1 if this index is not actual
 * @addr 0x4552A0
 */
int32 CPickups::GetActualPickupIndex(tPickupReference pickupRef) {
    if (pickupRef.num == -1)
        return -1;

    if (pickupRef.refIndex != aPickUps.at(pickupRef.index).m_nReferenceIndex)
        return -1;

    return pickupRef.index;
}

// 0x456A30
tPickupReference CPickups::GetNewUniquePickupIndex(int32 pickupIndex) {
    auto& refIdx = aPickUps[pickupIndex].m_nReferenceIndex;
    refIdx       = (uint16)refIdx >= 0xFFFE ? 1 : refIdx + 1; // NOTE: The original treats it as unsigned, so -2 and -1 wrap to 1

    return GetUniquePickupIndex(pickupIndex);
}

// returns pickup handle
// 0x455280
tPickupReference CPickups::GetUniquePickupIndex(int32 pickupIndex) {
    return tPickupReference(pickupIndex, aPickUps.at(pickupIndex).m_nReferenceIndex);
}

// returns TRUE if player got goodies
// 0x4564F0
bool CPickups::GivePlayerGoodiesWithPickUpMI(uint16 modelId, int32 playerId) {
    auto* const ped = FindPlayerPed(playerId > 0 ? playerId : -1); // NOTE: The original uses the player in focus for player 0 too

    if (modelId == MI_PICKUP_ADRENALINE) {
        ped->GetPlayerData()->m_bAdrenaline = true;
        ped->GetPlayerData()->m_nAdrenalineEndTime = CTimer::GetTimeInMS() + 20'000;
        ped->ResetSprintEnergy();
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_ADRENALINE);
        return true;
    }

    if (modelId == MI_PICKUP_BODYARMOUR) {
        ped->m_fArmour = (float)FindPlayerInfo(playerId).m_nMaxArmour;
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_BODY_ARMOUR);
        return true;
    }

    if (modelId == MI_PICKUP_INFO) {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_INFO);
        return true;
    }

    if (modelId == MI_PICKUP_HEALTH) {
        auto maxHealth = FindPlayerInfo(playerId).m_nMaxHealth;
        CStats::UpdateStatsAddToHealth((uint32)(int32)((float)maxHealth - ped->m_fHealth));
        ped->m_fHealth = (float)maxHealth;
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_HEALTH);
        return true;
    }

    if (modelId == MI_PICKUP_BONUS) {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_INFO);
        return true;
    }

    if (modelId == MI_PICKUP_BRIBE) {
        auto wantedLevel = std::max(+eWantedLevel::WANTED_CLEAN, +FindPlayerPed()->GetWantedLevel() - +eWantedLevel::WANTED_LEVEL_1);
        FindPlayerPed(0)->SetWantedLevel((eWantedLevel)wantedLevel);
        CStats::IncrementStat(STAT_NUMBER_OF_POLICE_BRIBES, 1.0f);
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_INFO);
        return true;
    }

    if (modelId == MI_PICKUP_KILLFRENZY) {
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

    if (modelId == MI_OYSTER) {
        PickedUpOyster();
        return true;
    }

    if (modelId == MI_HORSESHOE) {
        PickedUpHorseShoe();
        return true;
    }

    return false;
}

/*!
 * @brief Check if pickup was picked up, and then mark it as not picked up.
 * @addr 0x454B40
 */
bool CPickups::IsPickUpPickedUp(tPickupReference pickupRef) {
    if (const auto it = rng::find(ms_PickUpsCollected, pickupRef.num); it != ms_PickUpsCollected.end()) {
        *it = 0; // Reset
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
    for (auto& pickup : aPickUps) {
        switch (pickup.m_nPickupType) {
        case PICKUP_NONE:
        case PICKUP_ASSET_REVENUE:
            continue;
        }

        if (pickup.m_nRegenerationTime <= time) {
            pickup.m_nRegenerationTime = 0;
        } else {
            pickup.m_nRegenerationTime -= time;
        }
    }
}

// 0x455390
void CPickups::PickedUpHorseShoe() {
    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_COLLECTABLE1);

    CStats::IncrementStat(STAT_HORSESHOES_COLLECTED);
    CStats::IncrementStat(STAT_LUCK, 1000.f / CStats::GetStatValue(STAT_TOTAL_HORSESHOES)); // TODO: Is this some inlined stuff? (The division part)

    FindPlayerInfo(0).m_nMoney += 100; // NOTE: Always player 1

    const auto collected = CStats::GetStatValue(STAT_HORSESHOES_COLLECTED);
    const auto total = CStats::GetStatValue(STAT_TOTAL_HORSESHOES);
    if (collected == total) {
        CGarages::TriggerMessage("HO_ALL");
        FindPlayerInfo(0).m_nMoney += 100'000; // NOTE: Always player 1
    } else {
        CGarages::TriggerMessage("HO_ONE", static_cast<int16>(collected), 5000u, static_cast<int16>(total));
    }
}

// 0x4552D0
void CPickups::PickedUpOyster() {
    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_COLLECTABLE1);

    CStats::IncrementStat(STAT_OYSTERS_COLLECTED);
    FindPlayerInfo(0).m_nMoney += 100; // NOTE: Always player 1

    const auto collected = CStats::GetStatValue(STAT_OYSTERS_COLLECTED);
    const auto total = CStats::GetStatValue(STAT_TOTAL_OYSTERS);
    if (collected == total) {
        CGarages::TriggerMessage("OY_ALL");
        FindPlayerInfo(0).m_nMoney += 100'000; // NOTE: Always player 1
    } else {
        CGarages::TriggerMessage("OY_ONE", static_cast<int16>(collected), 5000u, static_cast<int16>(total));
    }
}

// 0x456A70
void CPickups::PictureTaken() {
    std::optional<size_t> capturedPickup{};
    auto lastFoundDist = 999'999.88f; // maybe FLT_MAX

    for (auto&& [i, pickup] : rngv::enumerate(aPickUps)) {
        if (pickup.m_nPickupType != PICKUP_SNAPSHOT)
            continue;

        const auto pupPos = pickup.GetPosn();
        const auto dist = DistanceBetweenPoints(TheCamera.GetPosition(), pupPos);

        if (90.0f / TheCamera.FindCamFOV() * 20.0f > dist && dist < lastFoundDist) {
            if (TheCamera.IsSphereVisible(pupPos, 0.2f) || TheCamera.IsSphereVisibleInMirror(pupPos, 0.2f)) {
                capturedPickup = i;
                lastFoundDist = dist;
            }
        }
    }

    if (!capturedPickup.has_value())
        return;

    aPickUps[*capturedPickup].Remove();

    FindPlayerInfo(0).m_nMoney += 100; // NOTE: Always player 1

    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_COLLECTABLE1);
    CStats::IncrementStat(STAT_SNAPSHOTS_TAKEN, 1.0f);

    const auto taken = CStats::GetStatValue(STAT_SNAPSHOTS_TAKEN);
    const auto total = CStats::GetStatValue(STAT_TOTAL_SNAPSHOTS);
    if (taken == total) {
        CGarages::TriggerMessage("SN_ALL");
        FindPlayerInfo().m_nMoney += 100'000;
    } else {
        CGarages::TriggerMessage("SN_ONE", static_cast<int16>(taken), 5000u, static_cast<int16>(total));
    }
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
    for (auto&& [i, pickup] : rngv::enumerate(aPickUps)) {
        switch (pickup.m_nPickupType) {
        case PICKUP_ONCE_FOR_MISSION: {
            CRadar::ClearBlipForEntity(BLIP_PICKUP, GetUniquePickupIndex(i).num);
            pickup.GetRidOfObjects();

            pickup.m_nFlags.bDisabled = true;
            pickup.m_nPickupType = PICKUP_NONE;
            break;
        }
        }
    }
}

// 0x4573D0
void CPickups::RemovePickUp(tPickupReference pickupRef) {
    if (const auto i = GetActualPickupIndex(pickupRef); i != -1) {
        aPickUps[i].Remove();
    }
}

// 0x456D30
void CPickups::RemovePickUpsInArea(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {
    CBoundingBox bb{ { minX, minY, minZ }, { maxX, maxY, maxZ } }; // They didn't use a bounding box, but it's nicer to do so.

    for (auto& pickup : GetAllActivePickups()) {
        if (bb.IsPointWithin(pickup.GetPosn())) {
            pickup.Remove();
        }
    }
}

// 0x455470
void CPickups::RemovePickupObjects() {
    for (auto& pickup : GetAllActivePickups()) {
        if (pickup.m_pObject) {
            pickup.GetRidOfObjects();
            pickup.m_nFlags.bVisible = false;
        }
    }
}

// remove pickups with types PICKUP_ONCE_TIMEOUT and PICKUP_MONEY in area
// 0x4563A0
void CPickups::RemoveUnnecessaryPickups(const CVector& posn, float radius) {
    for (auto& pickup : aPickUps) {
        switch (pickup.m_nPickupType) {
        case PICKUP_ONCE_TIMEOUT:
        case PICKUP_MONEY: {
            if (DistanceBetweenPoints(posn, pickup.GetPosn()) < radius) { // NOTE: Not `IsPointInSphere`, the original's check is `<`
                pickup.Remove();
            }
            break;
        }
        }
    }
}

// 0x455000
void CPickups::RenderPickUpText() {
    GxtChar msgText[352]{};
    for (const auto& message : std::span{ ms_Messages.data(), ms_NumMessages }) {
        const GxtChar* text{};
        if (message.price) {
            AsciiToGxtChar(std::format("${:d}", message.price).c_str(), msgText);
            text = msgText;
        } else if (message.text) {
            if (message.flags.IsPropertyForSale) {
                CMessages::InsertNumberInString(message.text, 0, 0, 0, 0, 0, 0, msgText);
                text = msgText;
            } else {
                text = message.text;
            }
        } else {
            continue;
        }

        const auto maxScale = (float)(RsGlobal.maximumWidth / 640); // NOTE: Integer division in the original
        CFont::SetProportional(true);
        CFont::SetBackground(false, false);
        CFont::SetScale(std::min(maxScale, message.width * (1.f / 30.f)), std::min(maxScale, message.height * (1.f / 30.f)));
        CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
        CFont::SetCentreSize(SCREEN_WIDTH);
        CFont::SetColor(message.color);
        CFont::SetFontStyle(eFontStyle::FONT_PRICEDOWN);
        CFont::PrintString(message.pos.x, message.pos.y, text);
    }
    ms_NumMessages = 0;
}

// check for pickups in area
// 0x456450
bool CPickups::TestForPickupsInBubble(const CVector posn, float radius) {
    // NOTE: (Possible bug) - They dont check if the pickup is active (eg: type != NONE)
    return rng::any_of(aPickUps, [&](const CPickup& p) {
        return DistanceBetweenPoints(posn, p.GetPosn()) < radius; // NOTE: Not `CSphere::IsPointWithin`, the original's check is `<`
    });
}

// search for pickup in area (radius = 5.5 units) with this weapon model and pickup type and add ammo to this pickup; returns TRUE if merged
// 0x4555A0
bool CPickups::TryToMerge_WeaponType(CVector posn, eWeaponType weaponType, ePickupType pickupType, uint32 ammo, bool arg4) {
    const auto mi = CWeaponInfo::GetWeaponInfo(weaponType)->m_nModelId1;

    for (auto& pickup : aPickUps) {
        if (pickup.m_nPickupType == pickupType && mi == pickup.m_nModelIndex && DistanceBetweenPoints(posn, pickup.GetPosn()) < 5.5f) {
            pickup.m_nAmmo += ammo;

            return true;
        }
    }
    return false;
}

// 0x458DE0
void CPickups::Update() {
    ZoneScoped;

    if (CReplay::Mode == MODE_PLAYBACK)
        return;

    auto start = 620 * (CTimer::GetFrameCounter() % 32) / 32;
    auto end   = 620 * (CTimer::GetFrameCounter() % 32 + 1) / 32;
    for (auto i = start; i < end; i++) {
        auto& pickup = aPickUps[i];
        if (pickup.m_nPickupType == PICKUP_NONE)
            continue;

        if (pickup.m_nFlags.bVisible = pickup.IsVisible()) {
            if (!pickup.m_nFlags.bDisabled && !pickup.m_pObject) {
                pickup.GiveUsAPickUpObject(pickup.m_pObject);

                if (auto& obj = pickup.m_pObject; obj) {
                    CWorld::Add(obj);
                }
            }
        } else {
            pickup.GetRidOfObjects();
        }
    }

    const auto pad = CPad::GetPad();
    if (pad->CollectPickupJustDown()) {
        CollectPickupBuffer = 6;
    } else {
        CollectPickupBuffer = std::max(CollectPickupBuffer - 1, 0); // NOTE: Integer counters, `step_down_to` is for floats
    }

    if (ms_PlayerOnWeaponPickup) {
        ms_PlayerOnWeaponPickup = std::max(ms_PlayerOnWeaponPickup - 1, 0);
    }

    if (pad->GetTarget()) {
        CollectPickupBuffer = 0;
    }

    auto&      player1TaskMgr = FindPlayerPed(PED_TYPE_PLAYER1)->GetTaskManager();
    const auto canP1Collect   = !player1TaskMgr.Find<CTaskComplexEnterCarAsDriver>(false) && !player1TaskMgr.Find<CTaskComplexUseMobilePhone>(false);
    const auto canP2Collect   = FindPlayerPed(PED_TYPE_PLAYER2) && canP1Collect; // NOTE: The original checks player 1's tasks for player 2 too

    start = 620 * (CTimer::GetFrameCounter() % 6) / 6;
    end   = 620 * (CTimer::GetFrameCounter() % 6 + 1) / 6;
    for (auto i = start; i < end; i++) {
        auto& pickup = aPickUps[i];

        if (pickup.m_nPickupType == PICKUP_NONE || !pickup.m_nFlags.bVisible)
            continue;

        if (canP1Collect && pickup.Update(FindPlayerPed(), FindPlayerVehicle(), CWorld::PlayerInFocus)) {
            AddToCollectedPickupsArray(i);
            continue;
        }

        // 0x459069 - Player 2 gets a go only if player 1 didn't take it
        if (canP2Collect && pickup.m_nPickupType != PICKUP_NONE && pickup.m_nFlags.bVisible && pickup.Update(FindPlayerPed(1), FindPlayerVehicle(1), PED_TYPE_PLAYER2)) {
            AddToCollectedPickupsArray(i);
        }
    }
}

// 0x455680
void CPickups::UpdateMoneyPerDay(tPickupReference pickupRef, uint16 money) {
    if (auto idx = GetActualPickupIndex(pickupRef); idx != -1) {
        aPickUps[idx].m_nMoneyPerDay = money;
    }
}

// 0x454AE0
eWeaponType CPickups::WeaponForModel(int32 modelId) {
    if (modelId == MI_PICKUP_BODYARMOUR) {
        return WEAPON_ARMOUR;
    }
    if (modelId == MI_PICKUP_HEALTH) {
        return WEAPON_LAST_WEAPON;
    }
    if (modelId == MI_PICKUP_ADRENALINE) {
        return WEAPON_ARMOUR;
    }

    switch (modelId) {
    case MODEL_JETPACK:
        return WEAPON_LAST_WEAPON;

    case MODEL_INVALID:
        return WEAPON_UNARMED;
    }

    if (auto mi = CModelInfo::GetModelInfo(modelId); mi->GetModelType() == MODEL_INFO_WEAPON) {
        return mi->AsWeaponModelInfoPtr()->GetWeaponInfo();
    }

    return WEAPON_UNARMED;
}

// 0x5D35A0
bool CPickups::Load() {
    for (auto& pickup : aPickUps) {
        CGenericGameStorage::LoadDataFromWorkBuffer(pickup);
        if (pickup.m_nPickupType != PICKUP_NONE && pickup.m_pObject) {
            pickup.m_pObject = nullptr;
            pickup.m_nFlags.bVisible = false;
        }
    }
    ms_NumMessages = 0u;
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_CollectedPickUpIndex);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_DisplayHelpMessage);

    for (auto& collected : ms_PickUpsCollected) {
        CGenericGameStorage::LoadDataFromWorkBuffer(collected);
    }
    return true;
}

// 0x5D3540
bool CPickups::Save() {
    for (auto& pickup : aPickUps) {
        CGenericGameStorage::SaveDataToWorkBuffer(pickup);
    }
    CGenericGameStorage::SaveDataToWorkBuffer(ms_CollectedPickUpIndex);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_DisplayHelpMessage);

    for (auto& collected : ms_PickUpsCollected) {
        CGenericGameStorage::SaveDataToWorkBuffer(collected);
    }
    return true;
}

// 0x454B70
void ModifyStringLabelForControlSetting(char* stringLabel) {
    const auto len = strlen(stringLabel);

    if (len <= 2 || stringLabel[len - 2] != '_') {
        return;
    }

    switch (CPad::GetPad(0)->Mode) {
    case 0:
    case 1:
        stringLabel[len - 1] = 'L';
        break;
    case 2:
        stringLabel[len - 1] = 'T';
        break;
    case 3:
        stringLabel[len - 1] = 'C';
        break;
    }
}
