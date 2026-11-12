#include "StdInc.h"
#include "InteriorGroup_c.h"

#include <array>

#include "BaseModelInfo.h"
#include "Box.h"
#include "ColModel.h"
#include "Entity.h"
#include "Matrix.h"
#include "PathFind.h"

void InteriorGroup_c::InjectHooks() {
    RH_ScopedClass(InteriorGroup_c);
    RH_ScopedCategory("Interior");

    //RH_ScopedInstall(Constructor, 0x597FE0, { .Reversed = false });
    //RH_ScopedInstall(Destructor, 0x597FF0, { .Reversed = false });

    RH_ScopedInstall(Init, 0x5947E0);
    RH_ScopedInstall(Update, 0x5968E0, { .Reversed = false });
    RH_ScopedInstall(SetupPeds, 0x596890, { .Reversed = false });
    RH_ScopedInstall(UpdatePeds, 0x596830, { .Reversed = false });
    RH_ScopedInstall(SetupHousePeds, 0x5965E0, { .Reversed = false });
    RH_ScopedInstall(SetupPaths, 0x595590, { .Reversed = false });
    RH_ScopedInstall(ArePathsLoaded, 0x595380);
    RH_ScopedInstall(Setup, 0x595320);
    RH_ScopedInstall(Exit, 0x595290, { .Reversed = false });
    RH_ScopedInstall(ContainsInteriorType, 0x595250, { .Reversed = false });
    RH_ScopedInstall(CalcIsVisible, 0x595200, { .Reversed = false });
    RH_ScopedInstall(DereferenceAnims, 0x595160);
    RH_ScopedInstall(ReferenceAnims, 0x5950D0);
    RH_ScopedInstall(UpdateOfficePeds, 0x594E90, { .Reversed = false });
    RH_ScopedInstall(RemovePed, 0x594E30, { .Reversed = false });
    RH_ScopedInstall(SetupShopPeds, 0x594C10, { .Reversed = false });
    RH_ScopedInstall(SetupOfficePeds, 0x594BF0, { .Reversed = false });
    RH_ScopedInstall(GetEntity, 0x594BD0, { .Reversed = false });
    RH_ScopedInstall(GetPed, 0x594B90, { .Reversed = false });
    RH_ScopedInstall(FindClosestInteriorInfo, 0x594A50, { .Reversed = false });
    RH_ScopedInstall(FindInteriorInfo, 0x594970, { .Reversed = false });
    RH_ScopedInstall(GetNumInteriorInfos, 0x594920, { .Reversed = false });
    RH_ScopedInstall(GetRandomInterior, 0x5948C0, { .Reversed = false });
    RH_ScopedInstall(AddInterior, 0x594840, { .Reversed = false });
}

// 0x5947E0
void InteriorGroup_c::Init(CEntity* entity, int32 id) {
    rng::fill(m_interiors, nullptr);
    rng::fill(m_peds, nullptr);
    rng::fill(m_pedsToRemove, nullptr);
    
    m_numInteriors        = 0;
    m_pathSetupComplete   = 0;
    m_updatePeds          = 0;
    m_isVisible           = false;
    m_lastIsVisible       = false;
    m_animBlockReferenced = 0;
    m_pEntity = entity;
    m_groupId = static_cast<uint8>(id);
}

// 0x5968E0
void InteriorGroup_c::Update() {
    plugin::CallMethod<0x5968E0, InteriorGroup_c*>(this);
}

// 0x594840
int32 InteriorGroup_c::AddInterior(Interior_c* interior) {
    return plugin::CallMethodAndReturn<int32, 0x594840, InteriorGroup_c*, Interior_c*>(this, interior);
}

// 0x596890
void InteriorGroup_c::SetupPeds() {
    plugin::CallMethod<0x596890, InteriorGroup_c*>(this);
}

// 0x596830
void InteriorGroup_c::UpdatePeds() {
    plugin::CallMethod<0x596830, InteriorGroup_c*>(this);
}

// 0x5965E0
int32 InteriorGroup_c::SetupHousePeds() {
    return plugin::CallMethodAndReturn<int32, 0x5965E0, InteriorGroup_c*>(this);
}

// 0x595590
int8 InteriorGroup_c::SetupPaths() {
    return plugin::CallMethodAndReturn<int8, 0x595590, InteriorGroup_c*>(this);
}

// 0x595380
int8 InteriorGroup_c::ArePathsLoaded() {
    auto* entity = m_pEntity;

    // Bounding box of the entity's collision model
    const auto& bbox = entity->GetModelInfo()->GetColModel()->GetBoundingBox();
    const CVector& boxMin = bbox.m_vecMin;
    const CVector& boxMax = bbox.m_vecMax;

    // The 8 corners of the box, in the exact order the binary builds them
    const auto corners = std::array{
        CVector{ boxMin.x, boxMin.y, boxMin.z },
        CVector{ boxMin.x, boxMax.y, boxMin.z },
        CVector{ boxMax.x, boxMax.y, boxMin.z },
        CVector{ boxMax.x, boxMin.y, boxMin.z },
        CVector{ boxMin.x, boxMin.y, boxMax.z },
        CVector{ boxMin.x, boxMax.y, boxMax.z },
        CVector{ boxMax.x, boxMax.y, boxMax.z },
        CVector{ boxMax.x, boxMin.y, boxMax.z },
    };

    CBox worldBounds{
        CVector{ 999999.0f, 999999.0f, 999999.0f },
        CVector{ -999999.0f, -999999.0f, -999999.0f }
    };

    const auto& matrix = entity->GetMatrix();
    for (const auto& corner : corners) {
        worldBounds.StretchToPoint(matrix.TransformPoint(corner));
    }

    return ThePaths.AreNodesLoadedForArea(worldBounds.m_vecMin.x, worldBounds.m_vecMax.x, worldBounds.m_vecMin.y, worldBounds.m_vecMax.y);
}

// 0x595320
void InteriorGroup_c::Setup() {
    if (ContainsInteriorType(2)) {
        m_groupType = 0; // HOUSE
        ReferenceAnims();
        return;
    }

    if (ContainsInteriorType(0) || ContainsInteriorType(6)) {
        m_groupType = 1; // SHOP
    } else {
        m_groupType = static_cast<uint8>(ContainsInteriorType(1) ? 2 : -1); // OFFICE / none (0xFF)
    }
    ReferenceAnims();
}

// 0x595290
int8 InteriorGroup_c::Exit() {
    return plugin::CallMethodAndReturn<int8, 0x595290, InteriorGroup_c*>(this);
}

// 0x595250
int8 InteriorGroup_c::ContainsInteriorType(int32 a2) {
    return plugin::CallMethodAndReturn<int8, 0x595250, InteriorGroup_c*, int32>(this, a2);
}

// 0x595200
int8 InteriorGroup_c::CalcIsVisible() {
    return plugin::CallMethodAndReturn<int8, 0x595200, InteriorGroup_c*>(this);
}

// 0x595160
void InteriorGroup_c::DereferenceAnims() {
    if (!m_animBlockReferenced) {
        return;
    }
    CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex(GetAnimBlockName()));
    m_animBlockReferenced = false;
}

// 0x5950D0
void InteriorGroup_c::ReferenceAnims() {
    if (m_animBlockReferenced) {
        return;
    }
    const auto animBlkIdx = CAnimManager::GetAnimationBlockIndex(GetAnimBlockName());
    if (CStreaming::IsModelLoaded(IFPToModelId(animBlkIdx))) {
        CAnimManager::AddAnimBlockRef(animBlkIdx);
        m_animBlockReferenced = true;
    } else {
        CStreaming::RequestModel(IFPToModelId(animBlkIdx), STREAMING_KEEP_IN_MEMORY);
    }
}

// 0x594E90
void InteriorGroup_c::UpdateOfficePeds() {
    plugin::CallMethod<0x594E90, InteriorGroup_c*>(this);
}

// 0x594E30
int8 InteriorGroup_c::RemovePed(CPed* a2) {
    return plugin::CallMethodAndReturn<int8, 0x594E30, InteriorGroup_c*, CPed*>(this, a2);
}

// 0x594C10
int32 InteriorGroup_c::SetupShopPeds() {
    return plugin::CallMethodAndReturn<int32, 0x594C10, InteriorGroup_c*>(this);
}

// 0x594BF0
void InteriorGroup_c::SetupOfficePeds() {
    plugin::CallMethod<0x594BF0, InteriorGroup_c*>(this);
}

// 0x594BD0
CEntity* InteriorGroup_c::GetEntity() {
    return plugin::CallMethodAndReturn<CEntity*, 0x594BD0, InteriorGroup_c*>(this);
}

// 0x594B90
CPed* InteriorGroup_c::GetPed(int32 idx) {
    return plugin::CallMethodAndReturn<CPed*, 0x594B90, InteriorGroup_c*, int32>(this, idx);
}

// 0x594A50
bool InteriorGroup_c::FindClosestInteriorInfo(int32 a, CVector point, float b, InteriorInfo_t** interiorInfo, Interior_c** interior, float* pSome) {
    return plugin::CallMethodAndReturn<bool, 0x594A50, InteriorGroup_c*, int32, CVector, float, InteriorInfo_t**, Interior_c**, float*>(this, a, point, b, interiorInfo, interior,
                                                                                                                                      pSome);
}

// 0x594970
bool InteriorGroup_c::FindInteriorInfo(eInteriorInfoType infoType, InteriorInfo_t** a3, Interior_c** a4) {
    return plugin::CallMethodAndReturn<bool, 0x594970, InteriorGroup_c*, eInteriorInfoType, InteriorInfo_t**, Interior_c**>(this, infoType, a3, a4);
}

// 0x594920
int32 InteriorGroup_c::GetNumInteriorInfos(int32 a2) {
    return plugin::CallMethodAndReturn<int32, 0x594920, InteriorGroup_c*, int32>(this, a2);
}

// 0x5948C0
int32 InteriorGroup_c::GetRandomInterior() {
    return plugin::CallMethodAndReturn<int32, 0x5948C0, InteriorGroup_c*>(this);
}

//! @notsa
const char* InteriorGroup_c::GetAnimBlockName() {
    switch ((eInteriorGroupType)m_groupType) {
    case eInteriorGroupType::HOUSE:  return "int_house";
    case eInteriorGroupType::SHOP:   return "int_shop";
    case eInteriorGroupType::OFFICE: return "int_office";
    default:                         NOTSA_UNREACHABLE();
    }
}
