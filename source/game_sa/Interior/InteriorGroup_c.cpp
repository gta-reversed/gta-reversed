#include "StdInc.h"
#include "InteriorGroup_c.h"

#include "InteriorManager_c.h"
#include "Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.h"
#include "Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h"
#include "Tasks/TaskTypes/Interior/TaskInteriorBeInShop.h"
#include "Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.h"

void InteriorGroup_c::InjectHooks() {
    RH_ScopedClass(InteriorGroup_c);
    RH_ScopedCategory("Interior");

    //RH_ScopedInstall(Constructor, 0x597FE0, { .reversed = false });
    //RH_ScopedInstall(Destructor, 0x597FF0, { .reversed = false });

    RH_ScopedInstall(Init, 0x5947E0);
    RH_ScopedInstall(Update, 0x5968E0);
    RH_ScopedInstall(SetupPeds, 0x596890);
    RH_ScopedInstall(UpdatePeds, 0x596830);
    RH_ScopedInstall(SetupHousePeds, 0x5965E0);
    RH_ScopedInstall(SetupPaths, 0x595590, { .reversed = false });
    RH_ScopedInstall(ArePathsLoaded, 0x595380);
    RH_ScopedInstall(Setup, 0x595320);
    RH_ScopedInstall(Exit, 0x595290);
    RH_ScopedInstall(ContainsInteriorType, 0x595250);
    RH_ScopedInstall(CalcIsVisible, 0x595200);
    RH_ScopedInstall(DereferenceAnims, 0x595160);
    RH_ScopedInstall(ReferenceAnims, 0x5950D0);
    RH_ScopedInstall(UpdateOfficePeds, 0x594E90);
    RH_ScopedInstall(RemovePed, 0x594E30);
    RH_ScopedInstall(SetupShopPeds, 0x594C10);
    RH_ScopedInstall(SetupOfficePeds, 0x594BF0);
    RH_ScopedInstall(GetEntity, 0x594BD0);
    RH_ScopedInstall(GetPed, 0x594B90);
    RH_ScopedInstall(FindClosestInteriorInfo, 0x594A50);
    RH_ScopedInstall(FindInteriorInfo, 0x594970);
    RH_ScopedInstall(GetNumInteriorInfos, 0x594920);
    RH_ScopedInstall(GetRandomInterior, 0x5948C0);
    RH_ScopedInstall(AddInterior, 0x594840);
}

// 0x5947E0
void InteriorGroup_c::Init(CEntity* entity, int32 id) {
    rng::fill(m_interiors, nullptr);
    rng::fill(m_peds, nullptr);
    rng::fill(m_pedsToRemove, nullptr);
    m_numInteriors = 0;
    m_pathSetupComplete = 0;
    m_updatePeds = 0;
    m_isVisible = false;
    m_lastIsVisible = false;
    m_animBlockReferenced = 0;
    m_pEntity = entity;
    m_groupId = (uint8)id;
}

// 0x5968E0
void InteriorGroup_c::Update() {
    CalcIsVisible();
    if (!m_pathSetupComplete) {
        SetupPaths();
    }
    if (m_pathSetupComplete && !m_updatePeds) {
        SetupPeds();
    }
    if (m_updatePeds) {
        UpdatePeds();
    }
    ReferenceAnims();
}

// 0x594840
int32 InteriorGroup_c::AddInterior(Interior_c* interior) {
    for (auto i = 0; i < 8; i++) {
        if (!m_interiors[i]) {
            m_interiors[i] = interior;
            m_numInteriors++;
            return i;
        }
    }
    return m_numInteriors;
}

// 0x596890
void InteriorGroup_c::SetupPeds() {
    if (!m_EnEx) {
        return;
    }
    // 0xBB3DC2 is InteriorManager_c::m_ArePedsEnabled (0xBAF670 + 0x4752)
    static auto& s_ArePedsEnabled = StaticRef<bool>(0xBB3DC2);
    if (!s_ArePedsEnabled) {
        return;
    }
    switch ((eInteriorGroupType)m_groupType) {
    case eInteriorGroupType::HOUSE:
        SetupHousePeds();
        break;
    case eInteriorGroupType::SHOP:
        SetupShopPeds();
        break;
    case eInteriorGroupType::OFFICE:
        SetupOfficePeds();
        break;
    default:
        break;
    }
    m_updatePeds = 1;
}

// 0x596830
void InteriorGroup_c::UpdatePeds() {
    if (!m_EnEx) {
        return;
    }
    // 0xBB3DC2 is InteriorManager_c::m_ArePedsEnabled (0xBAF670 + 0x4752)
    static auto& s_ArePedsEnabled = StaticRef<bool>(0xBB3DC2);
    if (!s_ArePedsEnabled) {
        return;
    }
    for (auto i = 0; i < 16; i++) {
        auto* ped = m_pedsToRemove[i];
        if (ped) {
            if (ped->IsPointerValid()) {
                RemovePed(ped);
            }
            m_pedsToRemove[i] = nullptr;
        }
    }
    if (m_groupType == (uint8)eInteriorGroupType::OFFICE) {
        UpdateOfficePeds();
    }
}

// 0x5965E0
int32 InteriorGroup_c::SetupHousePeds() {
    CStreaming::StreamPedsForInterior(0);
    CStreaming::LoadAllRequestedModels(false);
    auto* const entity = m_pEntity;
    m_numPeds = 0;
    CVector spawnPos{};
    if (!entity->m_matrix) {
        entity->AllocateMatrix();
        entity->m_placement.UpdateMatrix(entity->m_matrix);
    }
    spawnPos = entity->m_matrix->TransformPoint(entity->GetPosition());
    // Original RNG idiom ( here and below ): `(int)((rand() & 0xFFFF) * (1/32768) * max)` - see Interior_c::RandomNumberInRange
    auto gangPedType = PED_TYPE_NONE;
    auto numPeds = (int32)((float)(rand() & 0xFFFF) * (1.0f / 32768.0f) * 100.0f) <= 50 ? 2 : 1; // (r <= 0x32) + 1
    auto bIsGangHouse = false;
    if (CPopCycle::m_pCurrZoneInfo && (gangPedType = CPopCycle::PickGangToCreateMembersOf()) != (ePedType)0 // CMP EDI, 0
        && CPopulation::ChooseGangOccupation(GetGangOfPedType(gangPedType)) != MODEL_INVALID
    ) {
        bIsGangHouse = true;
        if ((int32)((float)(rand() & 0xFFFF) * (1.0f / 32768.0f) * 100.0f) <= 15) { // JLE 0xF
            numPeds = CGeneral::GetRandomNumber() >= 0x3FFF ? 4 : 3; // (rand >= 0x3FFF) + 3
        } else {
            bIsGangHouse = false;
        }
    }
    for (auto i = 0; i < numPeds; i++) {
        eModelID modelId;
        if (bIsGangHouse) {
            modelId = CPopulation::ChooseGangOccupation(GetGangOfPedType(gangPedType));
        } else {
            auto slot = i;
            if (numPeds == 1) {
                slot = CGeneral::GetRandomNumber() >= 0x3FFF ? 1 : 0; // rand SETGE + push
            }
            modelId = (eModelID)CStreaming::FindMIPedSlotForInterior(slot);
        }
        const auto pedType = CModelInfo::GetPedModelInfo(modelId)->GetPedType();
        auto* ped = CPopulation::AddPed(pedType, modelId, spawnPos, false);
        m_peds[m_numPeds] = ped;
        if (ped) {
            m_numPeds++;
            CTheScripts::ScriptsForBrains.StartOrRequestNewStreamedScriptBrainWithThisName("house", ped, 3);
            ped->SetCharCreatedBy(PED_MISSION);
            ped->GetIntelligence()->SetPedDecisionMakerType(7);
            auto* task = new CTaskInteriorBeInHouse{ this }; // 0x674FC0 via inline new (orig: CTask::operator new(0x18) + ctor)
            ped->GetTaskManager().SetTask(task, TASK_PRIMARY_PRIMARY); // PUSH 0 / PUSH 4
        }
        if (!StaticRef<bool>(0xBB3D9C + i)) { // g_interiorMan.m_InteriorPedsAliveState[i] (private; 0xBAF670 + 0x472C)
            RemovePed(m_peds[m_numPeds - (ped ? 1 : 0)]);
        }
    }
    return m_numPeds;
}

// 0x595590
int8 InteriorGroup_c::SetupPaths() {
    return plugin::CallMethodAndReturn<int8, 0x595590, InteriorGroup_c*>(this);
}

// 0x595380
int8 InteriorGroup_c::ArePathsLoaded() {
    const auto& bbox = CModelInfo::GetModelInfo(m_pEntity->GetModelIndex())->GetColModel()->GetBoundingBox();
    const CVector corners[] = {
        bbox.m_vecMin,
        { bbox.m_vecMin.x, bbox.m_vecMax.y, bbox.m_vecMin.z },
        { bbox.m_vecMax.x, bbox.m_vecMax.y, bbox.m_vecMin.z },
        { bbox.m_vecMax.x, bbox.m_vecMin.y, bbox.m_vecMin.z },
        { bbox.m_vecMin.x, bbox.m_vecMin.y, bbox.m_vecMax.z },
        { bbox.m_vecMin.x, bbox.m_vecMax.y, bbox.m_vecMax.z },
        { bbox.m_vecMax.x, bbox.m_vecMax.y, bbox.m_vecMax.z },
        { bbox.m_vecMax.x, bbox.m_vecMin.y, bbox.m_vecMax.z },
    };
    if (!m_pEntity->m_matrix) {
        m_pEntity->AllocateMatrix();
        m_pEntity->m_placement.UpdateMatrix(m_pEntity->m_matrix);
    }
    auto min = CVector{ 999999.0f, 999999.0f, 999999.0f };
    auto max = CVector{ -999999.0f, -999999.0f, -999999.0f };
    for (const auto& corner : corners) {
        const auto transformed = m_pEntity->m_matrix->TransformPoint(corner);
        min.x = std::min(min.x, transformed.x);
        min.y = std::min(min.y, transformed.y);
        min.z = std::min(min.z, transformed.z);
        max.x = std::max(max.x, transformed.x);
        max.y = std::max(max.y, transformed.y);
        max.z = std::max(max.z, transformed.z);
    }
    return ThePaths.AreNodesLoadedForArea(min.x, max.x, min.y, max.y);
}

// 0x595320
void InteriorGroup_c::Setup() {
    if (ContainsInteriorType(2)) {
        m_groupType = (uint8)eInteriorGroupType::HOUSE;
        ReferenceAnims();
        return;
    }
    if (ContainsInteriorType(0) || ContainsInteriorType(6)) {
        m_groupType = (uint8)eInteriorGroupType::SHOP;
        ReferenceAnims();
        return;
    }
    m_groupType = ContainsInteriorType(1) ? (uint8)eInteriorGroupType::OFFICE : (uint8)-1;
    ReferenceAnims();
}

// 0x595290
int8 InteriorGroup_c::Exit() {
    for (auto i = 0; i < 8; i++) {
        if (auto* interior = m_interiors[i]) {
            interior->Exit();
            g_interiorMan.ReturnInteriorToPool(interior);
        }
    }
    for (auto i = 0; i < 16; i++) {
        auto* ped = m_peds[i];
        if (!ped) {
            continue;
        }
        if (ped->IsPointerValid()) {
            if (!ped->IsAlive()) {
                // 0xBB3D9C is InteriorManager_c::m_InteriorPedsAliveState (0xBAF670 + 0x472C), indexed by ped slot
                StaticRef<bool>(0xBB3D9C + i) = false;
            }
            CPopulation::RemovePed(ped);
        } else {
            // 0xBB3D9C is InteriorManager_c::m_InteriorPedsAliveState (0xBAF670 + 0x472C), indexed by ped slot
            StaticRef<bool>(0xBB3D9C + i) = false;
        }
        m_peds[i] = nullptr;
    }
    ThePaths.RemoveInterior(m_groupId);
    m_numPeds = 0;
    DereferenceAnims();
    return 1;
}

// 0x595250
int8 InteriorGroup_c::ContainsInteriorType(int32 a2) {
    for (auto i = 0; i < 8; i++) {
        const auto interior = m_interiors[i];
        if (interior && interior->m_box->m_type == a2) {
            return true;
        }
    }
    return false;
}

// 0x595200
int8 InteriorGroup_c::CalcIsVisible() {
    m_lastIsVisible = m_isVisible;
    m_isVisible = false;
    for (auto i = 0; i < m_numInteriors; i++) {
        if (m_interiors[i]->IsVisible()) {
            m_isVisible = true;
            break;
        }
    }
    return m_isVisible;
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
    if (!m_isVisible) {
        if (m_lastIsVisible) {
            for (auto i = 0; i < 16; i++) {
                auto* ped = m_peds[i];
                if (ped && ped->IsPointerValid()) {
                    CPopulation::RemovePed(ped);
                }
                m_peds[i] = nullptr;
            }
            m_numPeds = 0;
        }
        return;
    }
    if (m_lastIsVisible) {
        return;
    }
    auto* const entity = m_pEntity;
    CVector spawnPos{};
    if (!entity->m_matrix) {
        entity->AllocateMatrix();
        entity->m_placement.UpdateMatrix(entity->m_matrix);
    }
    spawnPos = entity->m_matrix->TransformPoint(entity->GetPosition());
    // desired = GetNumInteriorInfos(6); CClock scheduling: day shift gets Random(half, all), evening/morning Random(0, half), night none
    const auto numType6 = GetNumInteriorInfos(6);
    int32 numPeds;
    if (CClock::GetIsTimeInRange(9, 18)) {
        numPeds = CGeneral::GetRandomNumberInRange(numType6 / 2, numType6); // PUSH EDI(num), PUSH EAX(num/2)
    } else if (CClock::GetIsTimeInRange(18, 22)) {
        numPeds = CGeneral::GetRandomNumberInRange(0, numType6 / 2); // PUSH EAX(num/2), PUSH 0
    } else if (CClock::GetIsTimeInRange(6, 9)) {
        numPeds = CGeneral::GetRandomNumberInRange(0, numType6 / 2);
    } else {
        numPeds = 0;
    }
    numPeds = std::min(numPeds, 16); // CMP EAX, 0x10
    for (auto i = 0; i < numPeds; i++) {
        // slot = (int)((rand & 0xFFFF) * (1/32768) * 8); fMUL(8) at 0x859000
        const auto slot = (int32)((float)(rand() & 0xFFFF) * (1.0f / 32768.0f) * 8.0f);
        const auto modelId = (eModelID)CStreaming::FindMIPedSlotForInterior(slot);
        const auto pedType = CModelInfo::GetPedModelInfo(modelId)->GetPedType();
        auto* ped = CPopulation::AddPed(pedType, modelId, spawnPos, false);
        m_peds[m_numPeds] = ped;
        if (ped) {
            m_numPeds++;
            ped->SetCharCreatedBy(PED_MISSION);
            ped->GetIntelligence()->SetPedDecisionMakerType(7);
            auto* task = new CTaskInteriorBeInOffice{ this }; // 0x675220 via inline new (orig: CTask::operator new(0x18) + ctor)
            ped->GetTaskManager().SetTask(task, TASK_PRIMARY_PRIMARY); // PUSH 0 / PUSH 4
        }
    }
}


// 0x594E30
int8 InteriorGroup_c::RemovePed(CPed* a2) {
    for (auto i = 0; i < 16; i++) {
        if (m_peds[i] && m_peds[i] == a2) {
            CPopulation::RemovePed(a2);
            m_peds[i] = nullptr;
            m_numPeds--;
            return m_numPeds;
        }
    }
    return m_numPeds;
}

// 0x594C10
int32 InteriorGroup_c::SetupShopPeds() {
    CStreaming::StreamPedsForInterior(1);
    CStreaming::LoadAllRequestedModels(false);
    m_numPeds = 0;
    // count = (2 - (int)((rand & 0xFFFF) * (1/32768) * -3)) * m_numInteriors + 1; fMUL(-3) at 0x859024
    const auto numPeds = (2 - (int32)((float)(rand() & 0xFFFF) * (1.0f / 32768.0f) * -3.0f)) * m_numInteriors + 1;
    for (auto i = 0; i < numPeds; i++) {
        // slot: PUSH 0 for the keeper, else PUSH (1 - (int)((rand & 0xFFFF) * (1/32768) * -7)); fMUL(-7) at 0x866D24
        const auto modelId = (eModelID)CStreaming::FindMIPedSlotForInterior(i == 0 ? 0 : 1 - (int32)((float)(rand() & 0xFFFF) * (1.0f / 32768.0f) * -7.0f));
        const auto pedType = CModelInfo::GetPedModelInfo(modelId)->GetPedType();
        CVector spawnPos{};
        auto* const interior = reinterpret_cast<Interior_c*>(GetRandomInterior());
        int32 tileX{}, tileY{};
        interior->GetRandomTile(3, &tileX, &tileY);
        interior->GetTileCentre((float)tileX, (float)tileY, &spawnPos);
        spawnPos.z += 1.0f; // DAT_00858624
        auto* ped = CPopulation::AddPed(pedType, modelId, spawnPos, false);
        m_peds[m_numPeds] = ped;
        if (ped) {
            m_numPeds++;
            ped->SetCharCreatedBy(PED_MISSION);
            ped->GetIntelligence()->SetPedDecisionMakerType(7);
            CTask* task{};
            if (i == 0) {
                task = new CTaskInteriorShopKeeper{ this, false }; // 0x675660 via inline new (orig: CTask::operator new(0x20) + ctor), bIsSetup = 0
            } else {
                task = new CTaskInteriorBeInShop{ this }; // 0x6753E0 via inline new (orig: CTask::operator new(0x34) + ctor)
            }
            ped->GetTaskManager().SetTask(task, TASK_PRIMARY_PRIMARY); // PUSH 0 / PUSH 4
        }
    }
    return m_numPeds;
}

// 0x594BF0
void InteriorGroup_c::SetupOfficePeds() {
    CStreaming::StreamPedsForInterior(2);
    CStreaming::LoadAllRequestedModels(false);
    m_numPeds = 0;
}

// 0x594BD0
CEntity* InteriorGroup_c::GetEntity() {
    return m_pEntity;
}

// 0x594B90
CPed* InteriorGroup_c::GetPed(int32 idx) {
    return m_peds[idx];
}

// 0x594A50
bool InteriorGroup_c::FindClosestInteriorInfo(int32 a, CVector point, float b, InteriorInfo_t** interiorInfo, Interior_c** interior, float* pSome) {
    const auto maxDistSqr = b * b;
    auto bestDistSqr = 999999.0f;
    InteriorInfo_t* bestInfo = nullptr;
    Interior_c* bestInterior = nullptr;
    for (auto i = 0; i < 8; i++) {
        const auto intr = m_interiors[i];
        if (intr && intr->IsPtInside(point)) {
            for (auto k = 0; k < intr->m_interiorInfosCount; k++) {
                auto& info = intr->m_interiorInfos[k];
                if ((a == -1 || info.Type == (eInteriorInfoType)a) && !info.IsInUse) {
                    const auto distSqr = (point - info.Pos).SquaredMagnitude();
                    if (distSqr < maxDistSqr && distSqr < bestDistSqr) {
                        bestDistSqr = distSqr;
                        bestInfo = &info;
                        bestInterior = intr;
                    }
                }
            }
        }
    }
    if (!bestInfo) {
        return false;
    }
    *interiorInfo = bestInfo;
    *interior = bestInterior;
    *pSome = bestDistSqr;
    return true;
}

// 0x594970
bool InteriorGroup_c::FindInteriorInfo(eInteriorInfoType infoType, InteriorInfo_t** a3, Interior_c** a4) {
    // Original used 64-entry stack arrays (latent overflow past 64 matches); sized safely, behavior identical otherwise
    InteriorInfo_t* matchedInfos[128];
    Interior_c*     matchedInteriors[128];
    int32           count = 0;
    for (auto i = 0; i < 8; i++) {
        const auto interior = m_interiors[i];
        if (!interior) {
            continue;
        }
        for (auto k = 0; k < interior->m_interiorInfosCount; k++) {
            auto& info = interior->m_interiorInfos[k];
            if (info.Type == infoType && !info.IsInUse) {
                matchedInfos[count]     = &info;
                matchedInteriors[count] = interior;
                count++;
            }
        }
    }
    if (count <= 0) {
        *a3 = nullptr;
        *a4 = nullptr;
        return false;
    }
    const auto idx = (int32)((float)(rand() & 0xFFFF) * (1.0f / 32768.0f) * (float)count);
    *a3            = matchedInfos[idx];
    *a4            = matchedInteriors[idx];
    return true;
}

// 0x594920
int32 InteriorGroup_c::GetNumInteriorInfos(int32 a2) {
    auto count = 0;
    for (auto i = 0; i < 8; i++) {
        const auto interior = m_interiors[i];
        if (interior) {
            for (auto k = 0; k < interior->m_interiorInfosCount; k++) {
                if (interior->m_interiorInfos[k].Type == (eInteriorInfoType)a2) {
                    count++;
                }
            }
        }
    }
    return count;
}

// 0x5948C0
int32 InteriorGroup_c::GetRandomInterior() {
    const auto idx = (int32)((float)(rand() & 0xFFFF) * (1.0f / 32768.0f) * (float)m_numInteriors);
    auto seen = 0;
    for (auto i = 0; i < 8; i++) {
        if (m_interiors[i]) {
            if (seen == idx) {
                return reinterpret_cast<int32>(m_interiors[i]);
            }
            seen++;
        }
    }
    return 0;
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
