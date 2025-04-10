#include "StdInc.h"

#include "References.h"

CReference (&CReferences::aRefs)[MAX_NUM_REFERENCES] = *(CReference(*)[MAX_NUM_REFERENCES])0xB9B9A8;
CReference*(&CReferences::pEmptyList) = *(CReference**)0xBA1768;

void CReferences::InjectHooks() {
    RH_ScopedClass(CReferences);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x5719B0);
    RH_ScopedInstall(ListSize, 0x5719E0);
    RH_ScopedInstall(RemoveReferencesToPlayer, 0x571AD0);
    RH_ScopedInstall(PruneAllReferencesInWorld, 0x571CC0);
}

// 0x5719B0
void CReferences::Init() {
    ZoneScoped;

    pEmptyList = aRefs;

    // todo: do better
    for (int32 i = 0; i < MAX_NUM_REFERENCES - 1; ++i) {
        (&aRefs[i])->m_pNext = &aRefs[i + 1];
        (&aRefs[i])->m_ppEntity = nullptr;
    }

    (&aRefs[MAX_NUM_REFERENCES - 1])->m_pNext = nullptr;
}

uint32 CReferences::ListSize(CReference* ref) {
    uint32 count = 0;
    while (ref) {
        ref = ref->m_pNext;
        ++count;
    }

    return count;
}

void CReferences::RemoveReferencesToPlayer() {
    auto vehicle = FindPlayerVehicle();
    if (vehicle)
        vehicle->ResolveReferences();

    auto player = FindPlayerPed();
    if (player) {
        player->ResolveReferences();
        auto& group = CPedGroups::GetGroup(player->m_pPlayerData->m_nPlayerGroup);
        group.GetMembership().SetLeader(player);
        group.Process();
    }
}

void CReferences::PruneAllReferencesInWorld() {
    for (int32 i = 0; i < GetPedPool()->GetSize(); ++i) {
        if (auto ped = GetPedPool()->GetAt(i))
            ped->PruneReferences();
    }

    for (int32 i = 0; i < GetVehiclePool()->GetSize(); ++i) {
        if (auto vehicle = GetVehiclePool()->GetAt(i))
            vehicle->PruneReferences();
    }

    for (int32 i = 0; i < GetObjectPool()->GetSize(); ++i) {
        if (auto obj = GetObjectPool()->GetAt(i))
            obj->PruneReferences();
    }
}
