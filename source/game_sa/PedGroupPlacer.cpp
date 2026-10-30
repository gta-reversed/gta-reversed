#include "StdInc.h"

#include "PedGroupPlacer.h"
#include "PedGroups.h"
#include "Camera.h"
#include "PlayerPed.h"
#include "Population.h"
#include "PedPlacement.h"
#include "World.h"
#include "VisibilityPlugins.h"
#include "Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocators.h"
#include "Tasks/TaskTypes/TaskComplexBeInGroup.h"
#include "Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.h"
#include "ModelInfo.h"
#include "General.h"

void CPedGroupPlacer::InjectHooks() {
    RH_ScopedClass(CPedGroupPlacer);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(PlaceFormationGroup, 0x5FC9B0);
    RH_ScopedInstall(PlaceChatGroup, 0x5FCE80);
    RH_ScopedInstall(PlaceRandomGroup, 0x5FD330);
    RH_ScopedInstall(PlaceGroup, 0x5FD810);
}

// NOTSA: Checks if a group spawn origin is clear of obstacles and not visible too close to the player.
// Verifies camera frustum visibility against player distance threshold and ensures position clearance for ped placement.
bool CPedGroupPlacer::CanPlaceGroupAt(const CVector& origin, float radius) {
    if (TheCamera.IsSphereVisible(origin, radius)) {
        if (const auto player = FindPlayerPed(-1)) {
            const float dist = (origin - player->GetPosition()).Magnitude2D();
            if (dist <= CPopulation::PedCreationDistMultiplier() * 42.5f) {
                return false;
            }
        }
    }

    return CPedPlacement::IsPositionClearForPed(origin, radius, -1, nullptr, true, true, true);
}

// NOTSA: Unified helper for circular group placement (shared by PlaceChatGroup and PlaceRandomGroup).
// Calculates angular positions with random jitter, resolves ground height, verifies entity clearance,
// creates gang peds facing the center, and configures group tasks.
static bool PlaceCircularGroup(ePedType type, uint32 numOfPeds, const CVector& origin, ePedGroupDefaultTaskAllocatorType allocatorType, bool hasLeader) {
    if (numOfPeds < 2) {
        return false;
    }

    const int32 groupId = CPedGroups::AddGroup();
    if (groupId < 0) {
        return false;
    }

    const float angleStep = TWO_PI / (float)numOfPeds;
    const float radius = std::sqrt(0.5f / (1.0f - std::cos(angleStep)));

    if (!CPedGroupPlacer::CanPlaceGroupAt(origin, radius)) {
        return false;
    }

    std::array<CPed*, TOTAL_PED_GROUP_MEMBERS> peds{};
    uint32 createdCount = 0;
    CVector firstPedPos{};

    const uint32 pedsToCreate = std::min<uint32>(numOfPeds, TOTAL_PED_GROUP_MEMBERS);
    for (uint32 i = 0; i < pedsToCreate; i++) {
        const float angle = (float)i * angleStep + CGeneral::GetRandomNumberInRange(-0.2f, 0.2f);
        CVector pos = origin + CVector{radius * std::sin(angle), radius * std::cos(angle), 0.0f};

        bool isGroundBelow = false;
        const float groundZ = CWorld::FindGroundZFor3DCoord({pos.x, pos.y, origin.z + 1.0f}, &isGroundBelow, nullptr);
        if (!isGroundBelow) {
            continue;
        }
        pos.z = std::max(origin.z, groundZ);

        if (createdCount == 0) {
            firstPedPos = pos;
        }

        const eModelID modelId = CPopulation::ChooseGangOccupation(GetGangOfPedType(type));
        CBaseModelInfo* mi = CModelInfo::GetPedModelInfo(modelId);
        if (!mi || !mi->GetRwObject()) {
            break;
        }

        std::array<CEntity*, 9> hitEntities{};
        CPedPlacement::IsPositionClearForPed(pos, mi->GetColModel()->GetBoundRadius(), (int32)hitEntities.size(), hitEntities.data(), true, true, true);

        const bool bFoundOtherEntity = rng::any_of(hitEntities, [&](CEntity* ent) {
            return ent && !notsa::contains(peds | rngv::take(createdCount), ent);
        });
        if (bFoundOtherEntity) {
            continue;
        }

        if (createdCount > 0) {
            if (!CWorld::GetIsLineOfSightClear(firstPedPos, pos, true, false, false, false, false, false, false)) {
                continue;
            }
            if (std::abs(firstPedPos.z - pos.z) >= 1.0f) {
                continue;
            }
        }

        CPed* ped = CPopulation::AddPed(type, modelId, pos, false);
        if (!ped) {
            continue;
        }

        const float heading = CGeneral::GetRadianAngleBetweenPoints(pos.x, pos.y, origin.x, origin.y);
        ped->m_fCurrentRotation = heading;
        ped->m_fAimingRotation = heading;
        ped->SetHeading(heading);
        CVisibilityPlugins::SetClumpAlpha(ped->GetRpClump(), 0);

        peds[createdCount++] = ped;
    }

    if (createdCount < 2) {
        for (uint32 i = 0; i < createdCount; i++) {
            CPopulation::RemovePed(peds[i]);
        }
        return false;
    }

    CPedGroup& group = CPedGroups::GetGroup(groupId);
    group.GetIntelligence().SetDefaultTaskAllocator(CPedGroupDefaultTaskAllocators::Get(allocatorType));

    auto AddPedToGroup = [&](CPed* ped, bool isLeader) {
        if (isLeader) {
            group.GetMembership().SetLeader(ped);
        } else {
            group.GetMembership().AddFollower(ped);
        }
        group.GetMembership().Process();
        group.GetIntelligence().Process();
        ped->GetTaskManager().SetTask(new CTaskComplexBeInGroup{groupId, isLeader}, TASK_PRIMARY_PRIMARY, false);
    };

    for (uint32 i = 0; i < createdCount; i++) {
        AddPedToGroup(peds[i], hasLeader && i == 0);
    }

    return true;
}

// 0x5FC9B0
bool CPedGroupPlacer::PlaceFormationGroup(ePedType type, uint32 numOfPeds, const CVector& origin, ePedGroupDefaultTaskAllocatorType unused) {
    const int32 groupId = CPedGroups::AddGroup();
    if (groupId < 0) {
        return false;
    }

    if (!CanPlaceGroupAt(origin, 3.0f)) {
        return false;
    }

    bool isGroundBelow = false;
    const float groundZ = CWorld::FindGroundZFor3DCoord({origin.x, origin.y, origin.z + 1.0f}, &isGroundBelow, nullptr);
    if (!isGroundBelow) {
        return false;
    }

    const CVector pos = { origin.x, origin.y, std::max(origin.z, groundZ) };

    const eModelID modelId = CPopulation::ChooseGangOccupation(GetGangOfPedType(type));
    CBaseModelInfo* mi = CModelInfo::GetPedModelInfo(modelId);
    if (!mi || !mi->GetRwObject()) {
        return false;
    }

    CPed* leader = CPopulation::AddPed(type, modelId, pos, false);
    if (!leader) {
        return false;
    }

    std::array<CPed*, TOTAL_PED_GROUP_MEMBERS> peds{};
    peds[0] = leader;
    uint32 createdCount = 1;

    const uint32 pedsToCreate = std::min<uint32>(numOfPeds, TOTAL_PED_GROUP_MEMBERS);
    for (uint32 i = 1; i < pedsToCreate; i++) {
        const eModelID followerModelId = CPopulation::ChooseGangOccupation(GetGangOfPedType(type));
        CBaseModelInfo* followerMi = CModelInfo::GetPedModelInfo(followerModelId);
        if (!followerMi || !followerMi->GetRwObject()) {
            continue;
        }

        const CVector followerPos = leader->GetPosition() + CVector{CTaskComplexFollowLeaderInFormation::ms_offsets.Offsets[i]};

        CPed* follower = CPopulation::AddPed(type, followerModelId, followerPos, false);
        if (!follower) {
            continue;
        }

        follower->SetPosn(followerPos);

        bool isFollowerGroundBelow = false;
        const float followerGroundZ = CWorld::FindGroundZFor3DCoord({followerPos.x, followerPos.y, followerPos.z + 1.0f}, &isFollowerGroundBelow, nullptr);
        if (!isFollowerGroundBelow) {
            CPopulation::RemovePed(follower);
            continue;
        }

        if (std::abs(followerGroundZ - leader->GetPosition().z) >= 1.0f) {
            CPopulation::RemovePed(follower);
            continue;
        }

        if (!CWorld::GetIsLineOfSightClear(leader->GetPosition(), followerPos, true, false, false, false, false, false, false)) {
            CPopulation::RemovePed(follower);
            continue;
        }

        follower->SetPosn({followerPos.x, followerPos.y, followerGroundZ});
        peds[createdCount++] = follower;
        CVisibilityPlugins::SetClumpAlpha(follower->GetRpClump(), 0);
    }

    CPedGroup& group = CPedGroups::GetGroup(groupId);
    group.GetIntelligence().SetDefaultTaskAllocator(CPedGroupDefaultTaskAllocators::Get(ePedGroupDefaultTaskAllocatorType::FOLLOW_ANY_MEANS));

    auto AddPedToGroup = [&](CPed* ped, bool isLeader) {
        if (isLeader) {
            group.GetMembership().SetLeader(ped);
        } else {
            group.GetMembership().AddFollower(ped);
        }
        group.GetMembership().Process();
        group.GetIntelligence().Process();
        ped->GetTaskManager().SetTask(new CTaskComplexBeInGroup{groupId, isLeader}, TASK_PRIMARY_PRIMARY, false);
    };

    AddPedToGroup(leader, true);
    for (uint32 i = 1; i < createdCount; i++) {
        AddPedToGroup(peds[i], false);
    }

    return true;
}

// 0x5FCE80
bool CPedGroupPlacer::PlaceChatGroup(ePedType type, uint32 numOfPeds, const CVector& origin, ePedGroupDefaultTaskAllocatorType unused) {
    return PlaceCircularGroup(type, numOfPeds, origin, ePedGroupDefaultTaskAllocatorType::STAND_STILL, true);
}

// 0x5FD330
bool CPedGroupPlacer::PlaceRandomGroup(ePedType type, uint32 numOfPeds, const CVector& origin, ePedGroupDefaultTaskAllocatorType unused) {
    return PlaceCircularGroup(type, numOfPeds, origin, ePedGroupDefaultTaskAllocatorType::RANDOM, false);
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
