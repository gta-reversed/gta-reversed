#pragma once

#include "Base.h"

#include "Vector.h"
#include "InteriorInfo_t.h"

class CEntity;
class CPed;
class Interior_c;
struct InteriorInfo_t;

enum class eInteriorGroupType : int8 {
    HOUSE = 0,
    SHOP = 1,
    OFFICE = 2,
};

class InteriorGroup_c : public ListItem_c<InteriorGroup_c> {
public: // TODO: private
    CEntity*                   m_Entity;       // 0x8
    uint8                      m_Id;           // 0xC
    uint8                      m_GroupId;      // 0xD
    uint8                      m_GroupType;    // 0xE - TODO: eInteriorGroupType
    uint8                      m_NumInteriors; // 0xF
    std::array<Interior_c*, 8> m_Interiors;    // 0x10
    CEntryExit*                m_EntryExit;    // 0x30

    bool m_IsVisible;     // 0x34
    bool m_LastIsVisible; // 0x35

    int8                  m_NumPeds;      // 0x36
    std::array<CPed*, 16> m_Peds;         // 0x38
    std::array<CPed*, 16> m_PedsToRemove; // 0x78

    bool m_PathsSetup;      // 0xB8
    bool m_PedsSetup;       // 0xB9
    bool m_AnimsReferenced; // 0xBA

public:
    static void InjectHooks();

    InteriorGroup_c() = default;  // 0x597FE0
    ~InteriorGroup_c() = default; // 0x597FF0

    auto GetInteriors() { return m_Interiors | rng::views::take(m_NumInteriors); }
    auto GetPeds() { return m_Peds | rng::views::take(m_NumPeds); }

    void Init(CEntity* entity, int32 groupId);
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
    auto GetId() const { return m_Id; }

    auto GetInteriors() const { return m_Interiors | std::views::take(m_NumInteriors); }
private:

    //! @notsa
    const char* GetAnimBlockName();
};
VALIDATE_SIZE(InteriorGroup_c, 0xBC);
