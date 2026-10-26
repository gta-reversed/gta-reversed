#include "StdInc.h"
#include <array>
#include <span>
#include <bit>
#include "InteriorGroup_c.h"
#include "General.h"
#include "Interior_c.h"
#include "Plugins/TwoDEffectPlugin/2dEffect.h"

void InteriorGroup_c::InjectHooks() {
    RH_ScopedClass(InteriorGroup_c);
    RH_ScopedCategory("Interior");

    //RH_ScopedInstall(Constructor, 0x597FE0, { .Reversed = false });
    //RH_ScopedInstall(Destructor, 0x597FF0, { .Reversed = false });

    RH_ScopedInstall(Init, 0x5947E0, { .Reversed = false });
    RH_ScopedInstall(Update, 0x5968E0, { .Reversed = false });
    RH_ScopedInstall(SetupPeds, 0x596890, { .Reversed = false });
    RH_ScopedInstall(UpdatePeds, 0x596830, { .Reversed = false });
    RH_ScopedInstall(SetupHousePeds, 0x5965E0, { .Reversed = false });
    RH_ScopedInstall(SetupPaths, 0x595590, { .Reversed = false });
    RH_ScopedInstall(ArePathsLoaded, 0x595380, { .Reversed = false });
    RH_ScopedInstall(Setup, 0x595320, { .Reversed = false });
    RH_ScopedInstall(Exit, 0x595290, { .Reversed = false });
    RH_ScopedInstall(ContainsInteriorType, 0x595250);
    RH_ScopedInstall(CalcIsVisible, 0x595200, { .Reversed = false });
    RH_ScopedInstall(DereferenceAnims, 0x595160);
    RH_ScopedInstall(ReferenceAnims, 0x5950D0);
    RH_ScopedInstall(UpdateOfficePeds, 0x594E90, { .Reversed = false });
    RH_ScopedInstall(RemovePed, 0x594E30, { .Reversed = false });
    RH_ScopedInstall(SetupShopPeds, 0x594C10, { .Reversed = false });
    RH_ScopedInstall(SetupOfficePeds, 0x594BF0, { .Reversed = false });
    RH_ScopedInstall(GetEntity, 0x594BD0, { .Reversed = false });
    RH_ScopedInstall(GetPed, 0x594B90, { .Reversed = false });
    RH_ScopedInstall(FindClosestInteriorInfo, 0x594A50);
    RH_ScopedInstall(FindInteriorInfo, 0x594970);
    RH_ScopedInstall(GetNumInteriorInfos, 0x594920, { .Reversed = false });
    RH_ScopedInstall(GetRandomInterior, 0x5948C0);
    RH_ScopedInstall(AddInterior, 0x594840, { .Reversed = false });
}

// 0x5947E0
void InteriorGroup_c::Init(CEntity* entity, int32 id) {
    plugin::CallMethod<0x5947E0, InteriorGroup_c*, CEntity*, int32>(this, entity, id);
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
    return plugin::CallMethodAndReturn<int8, 0x595380, InteriorGroup_c*>(this);
}

// 0x595320
void InteriorGroup_c::Setup() {
    plugin::CallMethod<0x595320, InteriorGroup_c*>(this);
}

// 0x595290
int8 InteriorGroup_c::Exit() {
    return plugin::CallMethodAndReturn<int8, 0x595290, InteriorGroup_c*>(this);
}

// 0x595250
int8 InteriorGroup_c::ContainsInteriorType(int32 type) {
    for (const auto* interior : m_interiors) {
        if (interior && interior->m_box->m_type == type) {
            return true;
        }
    }
    return false;
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
    CAnimManager::AddAnimBlockRef(CAnimManager::GetAnimationBlockIndex(GetAnimBlockName()));
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
bool InteriorGroup_c::FindClosestInteriorInfo(int32 type, CVector point, float maxDistance, InteriorInfo_t** outInfo, Interior_c** outInterior, float* outDistanceSq) {
    const auto maxDistanceSq = maxDistance * maxDistance;
    auto bestDistanceSq = 999999.0f;
    InteriorInfo_t* bestInfo{};
    Interior_c* bestInterior{};
    for (auto* interior : m_interiors) {
        if (!interior || !interior->IsPtInside(point) || interior->m_interiorInfosCount <= 0) {
            continue;
        }
        assert(static_cast<size_t>(interior->m_interiorInfosCount) <= std::size(interior->m_interiorInfos));
        for (auto& info : std::span{ interior->m_interiorInfos, static_cast<size_t>(interior->m_interiorInfosCount) }) {
            if ((type == -1 || info.Type == static_cast<eInteriorInfoType>(type)) && !info.IsInUse) {
                const auto diff = point - info.Pos;
                const auto distanceSq = (diff.y * diff.y + diff.z * diff.z) + diff.x * diff.x;
                if (distanceSq < maxDistanceSq && distanceSq < bestDistanceSq) {
                    bestInfo = &info;
                    bestInterior = interior;
                    bestDistanceSq = distanceSq;
                }
            }
        }
    }
    if (!bestInfo) {
        return false;
    }
    *outInfo = bestInfo;
    *outInterior = bestInterior;
    *outDistanceSq = bestDistanceSq;
    return true;
}

// 0x594970
bool InteriorGroup_c::FindInteriorInfo(eInteriorInfoType infoType, InteriorInfo_t** outInfo, Interior_c** outInterior) {
    std::array<InteriorInfo_t*, 64> matchingInfos{};
    std::array<Interior_c*, 64> matchingInteriors{};
    auto count = 0u;
    for (auto* interior : m_interiors) {
        if (!interior || interior->m_interiorInfosCount <= 0) {
            continue;
        }
        assert(static_cast<size_t>(interior->m_interiorInfosCount) <= std::size(interior->m_interiorInfos));
        for (auto& info : std::span{ interior->m_interiorInfos, static_cast<size_t>(interior->m_interiorInfosCount) }) {
            if (info.Type == infoType && !info.IsInUse) {
                assert(count < matchingInfos.size());
                matchingInfos[count] = &info;
                matchingInteriors[count] = interior;
                ++count;
            }
        }
    }
    if (!count) {
        *outInfo = nullptr;
        *outInterior = nullptr;
        return false;
    }
    const auto selected = static_cast<uint32>(CGeneral::GetRandomNumber()) * count / 32768u;
    *outInfo = matchingInfos[selected];
    *outInterior = matchingInteriors[selected];
    return true;
}

// 0x594920
int32 InteriorGroup_c::GetNumInteriorInfos(int32 a2) {
    return plugin::CallMethodAndReturn<int32, 0x594920, InteriorGroup_c*, int32>(this, a2);
}

// 0x5948C0
Interior_c* InteriorGroup_c::GetRandomInterior() {
    // The original loads this byte count as signed.
    const auto count = std::bit_cast<int8>(m_numInteriors);
    const auto selected = static_cast<int32>(CGeneral::GetRandomNumber()) * count / 32768;
    auto ordinal = 0;
    for (auto* interior : m_interiors) {
        if (interior && ordinal++ == selected) {
            return interior;
        }
    }
    return nullptr;
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
