#include "StdInc.h"

#include "PedPlacement.h"
#include "TaskComplexBeInGroup.h"
#include "TaskComplexFollowLeaderInFormation.h"
#include "Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocators.h"

void CPedGroupPlacer::InjectHooks() {
    RH_ScopedClass(CPedGroupPlacer);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(PlaceFormationGroup, 0x5FC9B0);
    RH_ScopedInstall(PlaceChatGroup, 0x5FCE80);
    RH_ScopedInstall(PlaceRandomGroup, 0x5FD330);
    RH_ScopedInstall(PlaceGroup, 0x5FD810);
}

// 0x5FC9B0
bool CPedGroupPlacer::PlaceFormationGroup(ePedType type, uint32 numOfPeds, const CVector& origin, ePedGroupDefaultTaskAllocatorType unused) {
    const auto groupId = CPedGroups::AddGroup();
    if (groupId < 0) {
        return false;
    }

    if (TheCamera.IsSphereVisible(origin, 3.0f)) {
        const auto dist = (FindPlayerPed()->GetPosition() - origin).Magnitude2D();
        if (dist < CPopulation::PedCreationDistMultiplier() * 42.5f) {
            return false;
        }
    }

    if (!CPedPlacement::IsPositionClearForPed(origin, 3.0f, -1, nullptr, true, true, true)) {
        return false;
    }

    bool bGroundHit{};
    const auto groundZ = CWorld::FindGroundZFor3DCoord({ origin.x, origin.y, origin.z + 1.0f }, &bGroundHit, nullptr) + 1.0f;
    if (!bGroundHit) {
        return false;
    }
    const CVector leaderPos{ origin.x, origin.y, std::max(groundZ, origin.z) };

    const auto leaderModel = CPopulation::ChooseGangOccupation(static_cast<eGangID>(type - PED_TYPE_GANG1));
    if (!CModelInfo::GetModelInfo(leaderModel)->GetRwObject()) {
        return false;
    }
    const auto leader = CPopulation::AddPed(type, leaderModel, leaderPos, false);
    if (!leader) {
        return false;
    }

    CPed* peds[TOTAL_PED_GROUP_MEMBERS]{ leader };
    int32 numPlaced = 1;

    if (numOfPeds > 1) {
        const auto& offsets = CTaskComplexFollowLeaderInFormation::ms_offsets.Offsets;
        for (uint32 i = 1; i < numOfPeds; i++) {
            const auto model = CPopulation::ChooseGangOccupation(static_cast<eGangID>(type - PED_TYPE_GANG1));
            if (!CModelInfo::GetModelInfo(model)->GetRwObject()) {
                continue;
            }
            auto* const ped = CPopulation::AddPed(type, model, leaderPos, false);
            if (!ped) {
                continue;
            }
            ped->SetPosn(leader->GetPosition() + CVector{ offsets[i] });
            const auto pos = ped->GetPosition();
            bool bHit{};
            const auto z = CWorld::FindGroundZFor3DCoord({ pos.x, pos.y, pos.z + 1.0f }, &bHit, nullptr) + 1.0f;
            if (!bHit) {
                CPopulation::RemovePed(ped);
                continue;
            }
            const auto finalZ = std::max(z, pos.z);
            if (std::abs(finalZ - leader->GetPosition().z) > 1.0f) {
                CPopulation::RemovePed(ped);
                continue;
            }
            if (!CWorld::GetIsLineOfSightClear({ pos.x, pos.y, finalZ }, leader->GetPosition(), true, false, false, false)) {
                CPopulation::RemovePed(ped);
                continue;
            }
            ped->SetPosn(pos.x, pos.y, finalZ);
            peds[numPlaced++] = ped;
            CVisibilityPlugins::SetClumpAlpha(ped->GetRpClump(), 0);
        }
    }

    auto& group = CPedGroups::GetGroup(groupId);
    group.GetIntelligence().SetDefaultTaskAllocator(CPedGroupDefaultTaskAllocators::Get(ePedGroupDefaultTaskAllocatorType::FOLLOW_ANY_MEANS));
    group.GetMembership().SetLeader(peds[0]);
    group.GetMembership().Process();
    group.GetIntelligence().Process();
    peds[0]->GetTaskManager().SetTask(new CTaskComplexBeInGroup(groupId, true), TASK_PRIMARY_PRIMARY);
    for (int32 i = 1; i < numPlaced; i++) {
        group.GetMembership().AddFollower(peds[i]);
        group.GetMembership().Process();
        group.GetIntelligence().Process();
        peds[i]->GetTaskManager().SetTask(new CTaskComplexBeInGroup(groupId, false), TASK_PRIMARY_PRIMARY);
    }
    return true;
}

// 0x5FCE80
bool CPedGroupPlacer::PlaceChatGroup(ePedType type, uint32 numOfPeds, const CVector& origin, ePedGroupDefaultTaskAllocatorType unused) {
    if (numOfPeds < 2) {
        return false;
    }
    const auto groupId = CPedGroups::AddGroup();
    if (groupId < 0) {
        return false;
    }

    const auto angleStep = TWO_PI / numOfPeds;
    const auto radius = std::sqrt(0.5f / (1.0f - std::cos(angleStep)));

    if (TheCamera.IsSphereVisible(origin, radius)) {
        const auto dist = (FindPlayerPed()->GetPosition() - origin).Magnitude2D();
        if (dist < CPopulation::PedCreationDistMultiplier() * 42.5f) {
            return false;
        }
    }
    if (!CPedPlacement::IsPositionClearForPed(origin, radius, -1, nullptr, true, true, true)) {
        return false;
    }

    CPed* peds[TOTAL_PED_GROUP_MEMBERS]{};
    int32 numPlaced = 0;
    CVector refPos{};
    for (uint32 attempt = 0; attempt < numOfPeds; attempt++) {
        const auto angle = attempt * angleStep + CGeneral::GetRandomNumberInRange(-0.2f, 0.2f) * angleStep;
        const auto rad = radius + CGeneral::GetRandomNumberInRange(-0.2f, 0.2f) * radius;
        CVector pos{ origin.x + std::cos(angle) * rad, origin.y + std::sin(angle) * rad, 0.0f };

        bool bGroundHit{};
        pos.z = CWorld::FindGroundZFor3DCoord({ pos.x, pos.y, origin.z + 1.0f }, &bGroundHit, nullptr) + 1.0f;
        if (!bGroundHit) {
            continue;
        }
        pos.z = std::max(pos.z, origin.z);
        if (attempt == 0) {
            refPos = pos;
        }

        const auto model = CPopulation::ChooseGangOccupation(static_cast<eGangID>(type - PED_TYPE_GANG1));
        if (!CModelInfo::GetModelInfo(model)->GetRwObject()) {
            continue;
        }
        CEntity* hitEntities[9]{};
        CPedPlacement::IsPositionClearForPed(pos, CModelInfo::GetModelInfo(model)->GetColModel()->GetBoundRadius(), 9, hitEntities, true, true, true);
        bool blocked = false;
        for (auto* const hit : hitEntities) {
            if (!hit) {
                continue;
            }
            bool isOwn = false;
            for (int32 i = 0; i < numPlaced; i++) {
                if (hit == peds[i]) {
                    isOwn = true;
                    break;
                }
            }
            if (!isOwn) {
                blocked = true;
                break;
            }
        }
        const bool lineClear = attempt == 0 || CWorld::GetIsLineOfSightClear(pos, refPos, true, false, false, false);
        const bool heightOk = attempt == 0 || std::abs(pos.z - refPos.z) <= 1.0f;
        if (blocked || !lineClear || !heightOk) {
            continue;
        }
        auto* const ped = CPopulation::AddPed(type, model, pos, false);
        if (!ped) {
            CPopulation::RemovePed(nullptr); // Matches original (passes null when AddPed fails)
            continue;
        }
        peds[numPlaced++] = ped;
        const auto heading = CGeneral::GetRadianAngleBetweenPoints(origin.x, origin.y, pos.x, pos.y);
        ped->m_fCurrentRotation = heading;
        ped->m_fAimingRotation = heading;
        CVisibilityPlugins::SetClumpAlpha(ped->GetRpClump(), 0);
    }

    if (numPlaced < 1) {
        for (int32 i = 0; i < numPlaced; i++) {
            CPopulation::RemovePed(peds[i]);
        }
        return false;
    }

    auto& group = CPedGroups::GetGroup(groupId);
    group.GetIntelligence().SetDefaultTaskAllocator(CPedGroupDefaultTaskAllocators::Get(ePedGroupDefaultTaskAllocatorType::STAND_STILL));
    group.GetMembership().SetLeader(peds[0]);
    group.GetMembership().Process();
    group.GetIntelligence().Process();
    peds[0]->GetTaskManager().SetTask(new CTaskComplexBeInGroup(groupId, true), TASK_PRIMARY_PRIMARY);
    for (int32 i = 1; i < numPlaced; i++) {
        group.GetMembership().AddFollower(peds[i]);
        group.GetMembership().Process();
        group.GetIntelligence().Process();
        peds[i]->GetTaskManager().SetTask(new CTaskComplexBeInGroup(groupId, false), TASK_PRIMARY_PRIMARY);
    }
    return true;
}

// 0x5FD330
bool CPedGroupPlacer::PlaceRandomGroup(ePedType type, uint32 numOfPeds, const CVector& origin, ePedGroupDefaultTaskAllocatorType unused) {
    if (numOfPeds < 2) {
        return false;
    }
    const auto groupId = CPedGroups::AddGroup();
    if (groupId < 0) {
        return false;
    }

    const auto angleStep = TWO_PI / numOfPeds;
    const auto radius = std::sqrt(0.5f / (1.0f - std::cos(angleStep)));

    if (TheCamera.IsSphereVisible(origin, radius)) {
        const auto dist = (FindPlayerPed()->GetPosition() - origin).Magnitude2D();
        if (dist < CPopulation::PedCreationDistMultiplier() * 42.5f) {
            return false;
        }
    }
    if (!CPedPlacement::IsPositionClearForPed(origin, radius, -1, nullptr, true, true, true)) {
        return false;
    }

    CPed* peds[TOTAL_PED_GROUP_MEMBERS]{};
    int32 numPlaced = 0;
    CVector refPos{};
    for (uint32 attempt = 0; attempt < numOfPeds; attempt++) {
        const auto angle = attempt * angleStep + CGeneral::GetRandomNumberInRange(-0.2f, 0.2f) * angleStep;
        const auto rad = radius + CGeneral::GetRandomNumberInRange(-0.2f, 0.2f) * radius;
        CVector pos{ origin.x + std::cos(angle) * rad, origin.y + std::sin(angle) * rad, 0.0f };

        bool bGroundHit{};
        pos.z = CWorld::FindGroundZFor3DCoord({ pos.x, pos.y, origin.z + 1.0f }, &bGroundHit, nullptr) + 1.0f;
        if (!bGroundHit) {
            continue;
        }
        pos.z = std::max(pos.z, origin.z);
        if (attempt == 0) {
            refPos = pos;
        }

        const auto model = CPopulation::ChooseGangOccupation(static_cast<eGangID>(type - PED_TYPE_GANG1));
        if (!CModelInfo::GetModelInfo(model)->GetRwObject()) {
            continue;
        }
        CEntity* hitEntities[9]{};
        CPedPlacement::IsPositionClearForPed(pos, CModelInfo::GetModelInfo(model)->GetColModel()->GetBoundRadius(), 9, hitEntities, true, true, true);
        bool blocked = false;
        for (auto* const hit : hitEntities) {
            if (!hit) {
                continue;
            }
            bool isOwn = false;
            for (int32 i = 0; i < numPlaced; i++) {
                if (hit == peds[i]) {
                    isOwn = true;
                    break;
                }
            }
            if (!isOwn) {
                blocked = true;
                break;
            }
        }
        const bool lineClear = attempt == 0 || CWorld::GetIsLineOfSightClear(pos, refPos, true, false, false, false);
        const bool heightOk = attempt == 0 || std::abs(pos.z - refPos.z) <= 1.0f;
        if (blocked || !lineClear || !heightOk) {
            continue;
        }
        auto* const ped = CPopulation::AddPed(type, model, pos, false);
        if (!ped) {
            CPopulation::RemovePed(nullptr); // Matches original (passes null when AddPed fails)
            continue;
        }
        peds[numPlaced++] = ped;
        const auto heading = CGeneral::GetRadianAngleBetweenPoints(origin.x, origin.y, pos.x, pos.y);
        ped->m_fCurrentRotation = heading;
        ped->m_fAimingRotation = heading;
        CVisibilityPlugins::SetClumpAlpha(ped->GetRpClump(), 0);
    }

    if (numPlaced < 1) {
        for (int32 i = 0; i < numPlaced; i++) {
            CPopulation::RemovePed(peds[i]);
        }
        return false;
    }

    auto& group = CPedGroups::GetGroup(groupId);
    group.GetIntelligence().SetDefaultTaskAllocator(CPedGroupDefaultTaskAllocators::Get(ePedGroupDefaultTaskAllocatorType::RANDOM));
    for (int32 i = 0; i < numPlaced; i++) {
        if (i == 0) {
            group.GetMembership().SetLeader(peds[i]);
        } else {
            group.GetMembership().AddFollower(peds[i]);
        }
        group.GetMembership().Process();
        group.GetIntelligence().Process();
        peds[i]->GetTaskManager().SetTask(
            new CTaskComplexWanderGang{ PEDMOVE_WALK, static_cast<uint8>((rand() & 0xFFFF) * (1.0f / 32768.0f) * 8.0f), 5000, true, 0.5f },
            TASK_PRIMARY_DEFAULT
        );
        peds[i]->GetTaskManager().SetTask(new CTaskComplexBeInGroup(groupId, false), TASK_PRIMARY_PRIMARY);
    }
    return true;
}

// 0x5FD810
bool CPedGroupPlacer::PlaceGroup(ePedType type, uint32 numOfPeds, const CVector& origin, ePedGroupDefaultTaskAllocatorType allocType) {
    using enum ePedGroupDefaultTaskAllocatorType;
    switch (allocType) {
    case FOLLOW_ANY_MEANS:
    case FOLLOW_LIMITED:   return PlaceFormationGroup(type, numOfPeds, origin, allocType);
    case STAND_STILL:
    case CHAT:             return PlaceChatGroup(type, numOfPeds, origin, allocType);
    case RANDOM:           return PlaceRandomGroup(type, numOfPeds, origin, allocType);
    default:               NOTSA_UNREACHABLE();
    }
}
