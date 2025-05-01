/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "CompressedVector.h"
#include "Vector.h"

class CObject;
class CVehicle;
class CPlayerPed;

enum ePickupType : uint8 {
    PICKUP_NONE = 0,
    PICKUP_IN_SHOP = 1,
    PICKUP_ON_STREET = 2,
    PICKUP_ONCE = 3,
    PICKUP_ONCE_TIMEOUT = 4,
    PICKUP_ONCE_TIMEOUT_SLOW = 5,
    PICKUP_COLLECTABLE1 = 6,
    PICKUP_IN_SHOP_OUT_OF_STOCK = 7,
    PICKUP_MONEY = 8,
    PICKUP_MINE_INACTIVE = 9,
    PICKUP_MINE_ARMED = 10,
    PICKUP_NAUTICAL_MINE_INACTIVE = 11,
    PICKUP_NAUTICAL_MINE_ARMED = 12,
    PICKUP_FLOATINGPACKAGE = 13,
    PICKUP_FLOATINGPACKAGE_FLOATING = 14,
    PICKUP_ON_STREET_SLOW = 15,
    PICKUP_ASSET_REVENUE = 16,
    PICKUP_PROPERTY_LOCKED = 17,
    PICKUP_PROPERTY_FORSALE = 18,
    PICKUP_MONEY_DOESNTDISAPPEAR = 19,
    PICKUP_SNAPSHOT = 20,
    PICKUP_2P = 21,
    PICKUP_ONCE_FOR_MISSION = 22
};

enum ePickupPropertyText : int32 {
    PICKUP_PROPERTY_TEXT_CANCEL   = 0, // "Cancel"
    PICKUP_PROPERTY_TEXT_CANT_BUY = 1, // "You can't by..."
    PICKUP_PROPERTY_TEXT_CAN_BUY  = 2  // "Press TAB to buy ..."
};

class CPickup {
public:
    float            m_fRevenueValue;
    CObject*         m_pObject;
    uint32           m_nAmmo;
    uint32           m_nRegenerationTime;
    CompressedVector m_vecPos;
    uint16           m_nMoneyPerDay;
    int16            m_nModelIndex;
    int16            m_nReferenceIndex;
    ePickupType      m_nPickupType;
    struct {
        uint8 bDisabled : 1; // waiting for regeneration
        uint8 bEmpty : 1;    // no ammo
        uint8 bHelpMessageDisplayed : 1;
        uint8 bVisible : 1;
        uint8 nPropertyTextIndex : 3; // see enum ePickupPropertyText
    } m_nFlags;

public:
    static void InjectHooks();

    void SetPosn(CVector posn) { m_vecPos = CompressLargeVector(posn); } // 0x454960
    [[nodiscard]] CVector GetPosn() const { return UncompressLargeVector(m_vecPos); } // 0x4549A0
    [[nodiscard]] CVector2D GetPosn2D() const { return GetPosn(); };    // NOTSA
    [[nodiscard]] float GetXCoord() const { return m_vecPos.x / 8.0f; } // 0x4549F0
    [[nodiscard]] float GetYCoord() const { return m_vecPos.y / 8.0f; } // 0x454A10
    [[nodiscard]] float GetZCoord() const { return m_vecPos.z / 8.0f; } // 0x454A30
    void SetXCoord(float coord) { m_vecPos.x = static_cast<int16>(coord * 8.0f); }
    void SetYCoord(float coord) { m_vecPos.y = static_cast<int16>(coord * 8.0f); }
    void SetZCoord(float coord) { m_vecPos.z = static_cast<int16>(coord * 8.0f); }

    void ExtractAmmoFromPickup(CPlayerPed* player);
    [[nodiscard]] bool IsVisible();
    void GetRidOfObjects();
    bool PickUpShouldBeInvisible();
    void Remove();
    void GiveUsAPickUpObject(CObject** obj, int32 slotIndex);
    bool Update(CPlayerPed* player, CVehicle* vehicle, int32 playerId);
    void ProcessGunShot(const CVector& origin, const CVector& target);

    static ePickupPropertyText FindTextIndexForString(const char* message);
    static const char* FindStringForTextIndex(ePickupPropertyText index);
};
VALIDATE_SIZE(CPickup, 0x20);
