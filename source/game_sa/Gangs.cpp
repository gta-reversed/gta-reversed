#include "StdInc.h"
#include "Gangs.h"

void CGangs::InjectHooks() {
    RH_ScopedClass(CGangs);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5DE680);
    RH_ScopedInstall(Load, 0x5D3A90, {.enabled = LOAD_HOOKS_ENABLED });
    RH_ScopedInstall(Save, 0x5D3A60, {.enabled = SAVE_HOOKS_ENABLED });
    RH_ScopedInstall(SetGangWeapons, 0x5DE550);
    RH_ScopedInstall(ChooseGangPedModel, 0x5DE5A0, { .reversed = false });
    RH_ScopedInstall(GetWillAttackPlayerWithCops, 0x5DE500);
    RH_ScopedInstall(SetWillAttackPlayerWithCops, 0x5DE4E0);
    RH_ScopedInstall(SetGangPedModelOverride, 0x5DE580);
}

// 0x5DE680
void CGangs::Initialise() {
    SetGangWeapons(GANG_BALLAS,     WEAPON_PISTOL,       WEAPON_MICRO_UZI, WEAPON_UNARMED);
    SetGangWeapons(GANG_GROVE,      WEAPON_PISTOL,       WEAPON_UNARMED,   WEAPON_UNARMED);
    SetGangWeapons(GANG_VAGOS,      WEAPON_PISTOL,       WEAPON_UNARMED,   WEAPON_UNARMED);
    SetGangWeapons(GANG_DANANGBOYS, WEAPON_PISTOL,       WEAPON_MICRO_UZI, WEAPON_UNARMED);
    SetGangWeapons(GANG_MAFIA,      WEAPON_DESERT_EAGLE, WEAPON_UNARMED,   WEAPON_UNARMED);
    SetGangWeapons(GANG_TRIAD,      WEAPON_PISTOL,       WEAPON_AK47,      WEAPON_UNARMED);
    SetGangWeapons(GANG_AZTECAS,    WEAPON_PISTOL,       WEAPON_MICRO_UZI, WEAPON_UNARMED);
}

// 0x5D3A90
bool CGangs::Load() {
    for (auto& gang : Gang) {
        CGenericGameStorage::LoadDataFromWorkBufferOrg(&gang, sizeof(CGangInfo));
    }
    return true;
}

// 0x5D3A60
bool CGangs::Save() {
    for (auto& gang : Gang) {
        CGenericGameStorage::SaveDataToWorkBuffer(&gang, sizeof(CGangInfo));
    }
    return true;
}

// 0x5DE550
void CGangs::SetGangWeapons(int16 gangId, eWeaponType weapId1, eWeaponType weapId2, eWeaponType weapId3) {
    Gang[gangId].SetWeapons({ weapId1, weapId2, weapId3 });
}

// 0x5DE5A0
eModelID CGangs::ChooseGangPedModel(eGangID gangId) {
    return plugin::CallAndReturn<eModelID, 0x5DE5A0, eGangID>(gangId);
    /*
    // see sub_406E80
    if (Gang[gangId].m_nPedModelOverride != -1) {
        return CPopulation::m_PedGroups[0][21 * CPopulation::m_TranslationArray[18].pedGroupIds[2 * gangId + CPopulation::CurrentWorldZone + gangId]];
    }

    CCarCtrl::InitSequence(CPopulation::m_nNumPedsInGroup[CPopulation::m_TranslationArray[gangId + 18].pedGroupIds[0]]);
    if (CPopulation::m_nNumPedsInGroup[CPopulation::m_TranslationArray[gangId + 18].pedGroupIds[0]] <= 0) {
        return -1;
    }

    int v1 = 0;
    int v2 = gangId;
    int SequenceElement;
    while (1) {
        SequenceElement = CCarCtrl::FindSequenceElement(v1);
        auto id = CPopulation::m_PedGroups[0][21 * CPopulation::m_TranslationArray[v2 + 18].pedGroupIds[CPopulation::CurrentWorldZone] + SequenceElement];
        if (CStreaming::GetInfo(id).IsLoaded())
            break;

        if (++v1 >= CPopulation::m_nNumPedsInGroup[CPopulation::m_TranslationArray[gangId + 18].pedGroupIds[0]])
            return MODEL_INVALID;
    }
    return CPopulation::m_PedGroups[0][21 * CPopulation::m_TranslationArray[18].pedGroupIds[CPopulation::CurrentWorldZone + v2 * 3] + SequenceElement];
    */
}

// unused (0x5DE500)
bool CGangs::GetWillAttackPlayerWithCops(ePedType pedType) {
    assert(IsPedTypeGang(pedType));
    return GangAttackWithCops[pedType - PED_TYPE_GANG1];
}

// unused (0x5DE4E0)
void CGangs::SetWillAttackPlayerWithCops(ePedType pedType, bool attackPlayerWithCops) {
    assert(IsPedTypeGang(pedType));
    GangAttackWithCops[pedType - PED_TYPE_GANG1] = attackPlayerWithCops;
}

// unused (0x5DE580)
void CGangs::SetGangPedModelOverride(int16 gangId, int8 pedModelOverride) {
    Gang[gangId].m_nPedModelOverride = pedModelOverride;
}
