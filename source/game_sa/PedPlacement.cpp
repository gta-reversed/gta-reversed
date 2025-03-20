/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "PedPlacement.h"

void CPedPlacement::InjectHooks() {
    RH_ScopedClass(CPedPlacement);
    RH_ScopedCategoryGlobal();

    //RH_ScopedInstall(FindZCoorForPed, 0x616920);
    RH_ScopedOverloadedInstall(IsPositionClearForPed, "OG",  0x616860, bool(*)(const CVector&, float, int32, CEntity**, bool, bool, bool));
    RH_ScopedOverloadedInstall(IsPositionClearOfCars, "Pos", 0x6168E0, CVehicle*(*)(const CVector*));
    RH_ScopedOverloadedInstall(IsPositionClearOfCars, "Ped", 0x616A40, CVehicle*(*)(const CPed*));
}

// 0x616920 - Refactored
std::pair<CVector, bool> CPedPlacement::FindZCoorForPed(CVector pos) {
    const auto FindZ = [&](CVector offset) {
        CEntity*  hit{};
        CColPoint cp{};
        return CWorld::ProcessVerticalLine(pos + offset, pos.z - 100.0f, cp, hit, true, true, false, false, true, false, nullptr)
            ? cp.m_vecPoint.z
            : MAP_Z_LOW_LIMIT;
    };
    const auto highestZ = std::max(
        FindZ({ 0.f, 0.f, 0.5f }),
        FindZ({ 0.1f, 0.1f, 0.5f })
    );
    if (highestZ >= MAP_Z_LOW_LIMIT + 1.0f) {
        return {CVector{pos, highestZ + 1.f}, true};
    }
    return {pos, false};
}

// 0x616860
bool CPedPlacement::IsPositionClearForPed(const CVector& pos, float radius, int32 maxHitEntities, CEntity** outHitEntities, bool bCheckVehicles, bool bCheckPeds, bool bCheckObjects) {
    int16 hitCount{};
    CWorld::FindObjectsKindaColliding(
        pos,
        radius == -1.0f ? 0.75f : radius,
        true,
        &hitCount,
        maxHitEntities == -1 ? 2 : maxHitEntities,
        outHitEntities,
        false,
        bCheckVehicles,
        bCheckPeds,
        bCheckObjects,
        false
    );
    return hitCount == 0;
}

//! NOTSA
bool CPedPlacement::IsPositionClearForPed(
    const CVector&                  pos,
    float                           radius,
    std::initializer_list<CEntity*> ignoreEntities,
    bool                            bCheckVehicles,
    bool                            bCheckPeds,
    bool                            bCheckObjects
) {
    const auto numHitEntitiesToStore = ignoreEntities.size() + 1;

    CEntity* hitEntities[8]{};
    assert(numHitEntitiesToStore <= std::size(hitEntities)); // If ever hit, increase array size

    if (!IsPositionClearForPed(pos, radius, (int32)(numHitEntitiesToStore), hitEntities, bCheckVehicles, bCheckPeds, bCheckObjects)) {
        for (const auto hitEntity : hitEntities | rng::views::take(numHitEntitiesToStore)) { // If anything was hit, check if it's in the ignored entities array
            if (!notsa::contains(ignoreEntities, hitEntity)) {
                return false; // Nope, we hit something else, so it's not clear.
            }
        }
    }
    return true;
}

// 0x6168E0
CVehicle* CPedPlacement::IsPositionClearOfCars(const CVector* pos) {
    return CWorld::TestSphereAgainstWorld(*pos, 0.25, nullptr, false, true, false, false, false, false)->AsVehicle();
}

// 0x616A40
CVehicle* CPedPlacement::IsPositionClearOfCars(const CPed* ped) {
    const auto& pedPos = ped->GetPosition();
    const auto vehHit = IsPositionClearOfCars(&pedPos);

    if (!vehHit)
        return nullptr;

    if (vehHit->IsAutomobile() || vehHit->vehicleFlags.bIsBig) {
        std::array<CColPoint, 32> unused{}; // 0xC102A0 - Originally static, but this is fine too.
        if (CCollision::ProcessColModels(
            *ped->m_matrix,
            *CModelInfo::GetModelInfo(ped->m_nModelIndex)->GetColModel(),

            *vehHit->m_matrix,
            *CModelInfo::GetModelInfo(vehHit->m_nModelIndex)->GetColModel(),

            unused, nullptr, nullptr, false
        )) {
            return vehHit;
        }
    }
    return nullptr;
}
