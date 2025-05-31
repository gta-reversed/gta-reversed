/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "eWeaponType.h"
#include <array>

class CEntity;
class CPickup;

struct tPickupMessage;
struct tPickupReference;

constexpr uint32 MAX_COLLECTED_PICKUPS = 20;
constexpr uint32 MAX_PICKUP_MESSAGES = 16;
constexpr uint32 MAX_NUM_PICKUPS = 620;

// Dump @ 0x8A5F50
constexpr uint16 AmmoForWeapon_OnStreet[NUM_WEAPONS]{ 0u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 8u, 8u, 8u, 8u, 4u, 4u, 30u, 10u, 10u, 15u, 10u, 10u, 60u, 60u, 80u, 80u, 60u, 20u, 10u, 4u, 3u, 100u, 500u, 5u, 1u, 500u, 500u, 36u, 0u, 0u, 1u };

class CPickups {
public:
    static inline uint8& DisplayHelpMessage = *(uint8*)0x8A5F48;
    static inline int32& PlayerOnWeaponPickup = *(int32*)0x97D640;
    static inline int32& StaticCamStartTime = *(int32*)0x978618;
    static inline CVector* StaticCamCoors = (CVector*)0x97D660;
    static inline CVehicle*& pPlayerVehicle = *(CVehicle**)0x97861C;
    static inline bool& bPickUpcamActivated = *(bool*)0x978620;
    static inline uint16& CollectedPickUpIndex = *(uint16*)0x978624;
    static inline std::array<int32, MAX_COLLECTED_PICKUPS>& aPickUpsCollected = *(std::array<int32, MAX_COLLECTED_PICKUPS>*)0x978628;
    static inline uint16& NumMessages = *(uint16*)0x978678;
    static inline std::array<tPickupMessage, MAX_PICKUP_MESSAGES>& aMessages = *(std::array<tPickupMessage, MAX_PICKUP_MESSAGES>*)0x978680;
    static inline std::array<CPickup, MAX_NUM_PICKUPS>& aPickUps = *(std::array<CPickup, MAX_NUM_PICKUPS>*)0x9788C0;

public:
    static void InjectHooks();

    static void Init();
    static void ReInit();

    static void AddToCollectedPickupsArray(int32 pickupIndex);
    static void CreatePickupCoorsCloseToCoors(float inX, float inY, float inZ, float& outX, float& outY, float& outZ);
    static void CreateSomeMoney(CVector coors, int32 amount);
    static void DetonateMinesHitByGunShot(const CVector& shotOrigin, const CVector& shotTarget);

    static void DoCollectableEffects(CEntity* entity);
    static void DoMineEffects(CEntity* entity);
    static void DoMoneyEffects(CEntity* entity);
    static void DoPickUpEffects(CEntity* entity);

    static CPickup* FindPickUpForThisObject(CObject* object);
    static tPickupReference GenerateNewOne(CVector coors, uint32 modelId, ePickupType pickupType, uint32 ammo, uint32 moneyPerDay = 0u, bool isEmpty = false, char* message = nullptr);
    static tPickupReference GenerateNewOne_WeaponType(CVector coors, eWeaponType weaponType, ePickupType pickupType, uint32 ammo, bool isEmpty, char* message);
    static int32 GetActualPickupIndex(tPickupReference pickupIndex);
    static tPickupReference GetNewUniquePickupIndex(int32 pickupIndex);
    static tPickupReference GetUniquePickupIndex(int32 pickupIndex);
    static bool GivePlayerGoodiesWithPickUpMI(uint16 modelId, int32 playerId);
    static bool IsPickUpPickedUp(tPickupReference pickupRef);
    static int32 ModelForWeapon(eWeaponType weaponType);
    static void PassTime(uint32 time);
    static void PickedUpHorseShoe();
    static void PickedUpOyster();
    static void PictureTaken();
    static bool PlayerCanPickUpThisWeaponTypeAtThisMoment(eWeaponType weaponType);
    static void RemoveMissionPickUps();
    static void RemovePickUp(tPickupReference pickupRef);
    static void RemovePickUpsInArea(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);
    static void RemovePickupObjects();
    static void RemoveUnnecessaryPickups(const CVector& posn, float radius);
    static void RenderPickUpText();
    static bool TestForPickupsInBubble(const CVector posn, float radius);
    static bool TryToMerge_WeaponType(CVector posn, eWeaponType weaponType, ePickupType pickupType, uint32 ammo, bool arg4);
    static void Update();
    static void UpdateMoneyPerDay(tPickupReference pickupRef, uint16 money);
    static eWeaponType WeaponForModel(int32 modelId);
    static void Load();
    static void Save();

    // Helpers NOTSA

    /*!
     * @brief Our custom Vector based overload
     * @copydocs CPickups::CreatePickupCoorsCloseToCoors
     */
    static void CreatePickupCoorsCloseToCoors(const CVector& pos, CVector& createdAtPos) {
        return CreatePickupCoorsCloseToCoors(pos.x, pos.y, pos.z, createdAtPos.x, createdAtPos.y, createdAtPos.z);
    }
    /*!
     * @brief Our custom Vector based overload
     * @copydocs CPickups::CreatePickupCoorsCloseToCoors
     */
    static void CreatePickupCoorsCloseToCoors(const CVector& pos, float& x, float& y, float& z) {
        return CreatePickupCoorsCloseToCoors(pos.x, pos.y, pos.z, x, y, z);
    }

    static auto GetAllActivePickups() { return aPickUps | std::views::filter([](auto&& p) { return p.m_nPickupType != PICKUP_NONE; }); }
};

// NOTSA
struct tPickupReference {
    union {
        struct {
            int16 index;
            int16 refIndex;
        };

        int32 num;
    };

    tPickupReference() :
        num(-1) {}

    tPickupReference(int32 value) :
        num(value) {}

    tPickupReference(int16 idx, int16 refIdx) :
        index(idx),
        refIndex(refIdx) {}

    tPickupReference(CPickup& pickup) {
        ptrdiff_t arrIndex = (&pickup - CPickups::aPickUps.data());
        assert(arrIndex >= 0 && arrIndex < MAX_NUM_PICKUPS);
        index    = arrIndex;
        refIndex = pickup.m_nReferenceIndex;
    }
};
VALIDATE_SIZE(tPickupReference, 0x4);

inline static int32& CollectPickupBuffer = *(int32*)0x97D644;

void ModifyStringLabelForControlSetting(char* stringLabel);
