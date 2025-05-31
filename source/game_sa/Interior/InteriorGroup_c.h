#pragma once

#include "Base.h"

#include "Vector.h"
#include "InteriorInfo_t.h"

class CEntryExit;
class CEntity;
class CPed;
class Interior_c;
struct InteriorInfo_t;

enum class eInteriorGroupType : int8 {
    HOUSE = 0,
    SHOP = 1,
    OFFICE = 2,
};

enum class eInteriorGroupId { // Based on `GetGroupId`
    SHOP    = 0,
    OFFICE  = 1,
    LOUNGE  = 2,
    BEDROOM = 3,
    KITCHEN = 4,
    MISC    = 8,
};
using eInteriorGroupIdS8 = notsa::WEnumS8<eInteriorGroupId>;
using eInteriorGroupIdS32 = notsa::WEnumS32<eInteriorGroupId>;

enum class eInteriorSubGroupId { // Based on `GetSubGroupId`
    // eInteriorGroupId::SHOP
    SHOP_UNIT1_L   = 0,
    SHOP_UNIT1_R   = 1,
    SHOP_UNIT1_M   = 2,
    SHOP_UNIT2_L   = 3,
    SHOP_UNIT2_R   = 4,
    SHOP_UNIT2_M   = 5,
    SHOP_UNIT3_L   = 6,
    SHOP_UNIT3_R   = 7,
    SHOP_UNIT3_M   = 8,
    SHOP_UNIT4_L   = 9,
    SHOP_UNIT4_R   = 10,
    SHOP_UNIT4_M   = 11,
    SHOP_CHECKOUTS = 12,
    SHOP_BASKETS   = 13,
    SHOP_LIGHTS    = 14,

    // eInteriorGroupId::OFFICE
    OFFICE_DESKS        = 0,
    OFFICE_CHAIRS       = 1,
    OFFICE_WATERCOOLERS = 2,
    OFFICE_FILINGCABS   = 3,

    // eInteriorGroupId::LOUNGE
    LOUNGE_SOFAS       = 0,
    LOUNGE_CHAIRS      = 1,
    LOUNGE_TVSTANDS    = 2,
    LOUNGE_TVS         = 3,
    LOUNGE_TABLES      = 4,
    LOUNGE_LAMPS       = 5,
    LOUNGE_HIFI_TABLES = 6,
    LOUNGE_VIDEOS      = 7,
    LOUNGE_HIFIS       = 8,
    LOUNGE_CONSOLES    = 9,

    // eInteriorGroupId::BEDROOM
    BEDROOM_BEDS      = 0,
    BEDROOM_TABLES    = 1,
    BEDROOM_DRAWERS   = 2,
    BEDROOM_WARDROBES = 3,

    // eInteriorGroupId::KITCHEN
    KITCHEN_UNIT_L       = 0,
    KITCHEN_UNIT_M       = 1,
    KITCHEN_UNIT_R       = 2,
    KITCHEN_UNIT_SINKS   = 3,
    KITCHEN_UNIT_FRIDGES = 4,
    KITCHEN_UNIT_OVENS   = 5,
    KITCHEN_UNIT_WASHERS = 6,
    KITCHEN_UNIT_CORNERS = 7,
    KITCHEN_MICROWAVES   = 8,
    KITCHEN_TOASTERS     = 9,
    KITCHEN_TVS          = 10,

    // eInteriorGroupId::MISC
    MISC_PLANTS    = 0,
    MISC_LIGHTS    = 1,
    MISC_RUGS      = 2,
    MISC_PLATES    = 3,
    MISC_FOOD      = 4,
    MISC_MAGAZINES = 5,
    MISC_CLOTHES   = 6,
};
using eInteriorSubGroupIdS8 = notsa::WEnumS8<eInteriorSubGroupId>;
using eInteriorSubGroupIdS32 = notsa::WEnumS32<eInteriorSubGroupId>;

class InteriorGroup_c : public ListItem_c<InteriorGroup_c> {
public:
    CEntity*    m_pEntity;             // 0x8
    uint8       m_id;                  // 0xC
    uint8       m_groupId;             // 0xD - TODO: eInteriorGroupId
    uint8       m_groupType;           // 0xE - TODO: eInteriorGroupType
    uint8       m_numInteriors;        // 0xF
    Interior_c* m_interiors[8];        // 0x10
    CEntryExit* m_EnEx;                // 0x30
    bool        m_isVisible;           // 0x34
    bool        m_lastIsVisible;       // 0x35
    int8        m_numPeds;             // 0x36
    CPed*       m_peds[16];            // 0x38
    CPed*       m_pedsToRemove[16];    // 0x78
    int8        m_pathSetupComplete;   // 0xB8
    int8        m_updatePeds;          // 0xB9
    int8        m_animBlockReferenced; // 0xBA

public:
    static void InjectHooks();

    InteriorGroup_c() = default;  // 0x597FE0
    ~InteriorGroup_c() = default; // 0x597FF0

    auto GetInteriors() { return m_interiors | rng::views::take(m_numInteriors); }
    auto GetPeds() { return m_peds | rng::views::take(m_numPeds); }

    void Init(CEntity* entity, int32 id);
    void Update();
    int32 AddInterior(Interior_c* interior);
    void SetupPeds();
    void UpdatePeds();
    int32 SetupHousePeds();
    int8 SetupPaths();
    int8 ArePathsLoaded();
    void Setup();
    int8 Exit();
    int8 ContainsInteriorType(int32 a2);
    int8 CalcIsVisible();

    void DereferenceAnims();
    void ReferenceAnims();

    void UpdateOfficePeds();
    int8 RemovePed(CPed* a2);
    int32 SetupShopPeds();
    void SetupOfficePeds();
    CEntity* GetEntity();
    CPed* GetPed(int32);
    bool FindClosestInteriorInfo(int32 a, CVector point, float b, InteriorInfo_t** interiorInfo, Interior_c** interior, float* pSome);
    bool FindInteriorInfo(eInteriorInfoType infoType, InteriorInfo_t** a3, Interior_c** a4);
    int32 GetNumInteriorInfos(int32 a2);
    int32 GetRandomInterior();
    auto GetId() const { return m_id; }

    auto GetInteriors() const { return m_interiors | std::views::take(m_numInteriors); }
private:

    //! @notsa
    const char* GetAnimBlockName();
};
VALIDATE_SIZE(InteriorGroup_c, 0xBC);
