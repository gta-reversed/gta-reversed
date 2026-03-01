/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "ModelInfo.h"
#include "TempColModels.h"

CBaseModelInfo *(&CModelInfo::ms_modelInfoPtrs)[NUM_MODEL_INFOS] = *(CBaseModelInfo*(*)[NUM_MODEL_INFOS])0xA9B0C8;
int32& CModelInfo::ms_lastPositionSearched = *(int32*)0xAAE948;

CStore<CAtomicModelInfo, CModelInfo::NUM_ATOMIC_MODEL_INFOS>& CModelInfo::ms_atomicModelInfoStore = *(CStore<CAtomicModelInfo, NUM_ATOMIC_MODEL_INFOS>*)0xAAE950;
CStore<CDamageAtomicModelInfo, CModelInfo::NUM_DAMAGE_ATOMIC_MODEL_INFOS>& CModelInfo::ms_damageAtomicModelInfoStore = *(CStore<CDamageAtomicModelInfo, NUM_DAMAGE_ATOMIC_MODEL_INFOS>*)0xB1BF58;
CStore<CLodAtomicModelInfo, CModelInfo::NUM_LOD_ATOMIC_MODEL_INFOS>& CModelInfo::ms_lodAtomicModelInfoStore = *(CStore<CLodAtomicModelInfo, NUM_LOD_ATOMIC_MODEL_INFOS>*)0xB1C934;
CStore<CTimeModelInfo, CModelInfo::NUM_TIME_MODEL_INFOS>& CModelInfo::ms_timeModelInfoStore = *(CStore<CTimeModelInfo, NUM_TIME_MODEL_INFOS>*)0xB1C960;
CStore<CLodTimeModelInfo, CModelInfo::NUM_LOD_TIME_MODEL_INFOS>& CModelInfo::ms_lodTimeModelInfoStore = *(CStore<CLodTimeModelInfo, NUM_LOD_TIME_MODEL_INFOS>*)0xB1E128;
CStore<CWeaponModelInfo, CModelInfo::NUM_WEAPON_MODEL_INFOS>& CModelInfo::ms_weaponModelInfoStore = *(CStore<CWeaponModelInfo, NUM_WEAPON_MODEL_INFOS>*)0xB1E158;
CStore<CClumpModelInfo, CModelInfo::NUM_CLUMP_MODEL_INFOS>& CModelInfo::ms_clumpModelInfoStore = *(CStore<CClumpModelInfo, NUM_CLUMP_MODEL_INFOS>*)0xB1E958;
CStore<CVehicleModelInfo, CModelInfo::NUM_VEHICLE_MODEL_INFOS>& CModelInfo::ms_vehicleModelInfoStore = *(CStore<CVehicleModelInfo, NUM_VEHICLE_MODEL_INFOS>*)0xB1F650;
CStore<CPedModelInfo, CModelInfo::NUM_PED_MODEL_INFOS>& CModelInfo::ms_pedModelInfoStore = *(CStore<CPedModelInfo, NUM_PED_MODEL_INFOS>*)0xB478F8;
CStore<C2dEffect, CModelInfo::NUM_2DFX_INFOS>& CModelInfo::ms_2dFXInfoStore = *(CStore<C2dEffect, NUM_2DFX_INFOS>*)0xB4C2D8;

void CModelInfo::InjectHooks()
{
    RH_ScopedClass(CModelInfo);
    RH_ScopedCategory("Models");

    RH_ScopedInstall(Initialise, 0x4C6810);
    RH_ScopedInstall(ShutDown, 0x4C63E0);
    RH_ScopedInstall(ReInit2dEffects, 0x4C63B0);

    RH_ScopedInstall(GetModelInfoUInt16, 0x4C59F0);
    RH_ScopedInstall(GetModelInfoFromHashKey, 0x4C59B0);
    RH_ScopedOverloadedInstall(GetModelInfo, "full", 0x4C5940, CBaseModelInfo * (*)(const char*, int32*));
    RH_ScopedOverloadedInstall(GetModelInfo, "minmax", 0x4C5A20, CBaseModelInfo*(*)(const char*, int32, int32));

    RH_ScopedInstall(AddAtomicModel, 0x4C6620);
    RH_ScopedInstall(AddDamageAtomicModel, 0x4C6650);
    RH_ScopedInstall(AddLodAtomicModel, 0x4C6680);
    RH_ScopedInstall(AddTimeModel, 0x4C66B0);
    RH_ScopedInstall(AddLodTimeModel, 0x4C66E0);
    RH_ScopedInstall(AddWeaponModel, 0x4C6710);
    RH_ScopedInstall(AddClumpModel, 0x4C6740);
    RH_ScopedInstall(AddVehicleModel, 0x4C6770);
    RH_ScopedInstall(AddPedModel, 0x4C67A0);

    RH_ScopedInstall(IsBoatModel, 0x4C5A70);
    RH_ScopedInstall(IsCarModel, 0x4C5AA0);
    RH_ScopedInstall(IsTrainModel, 0x4C5AD0);
    RH_ScopedInstall(IsHeliModel, 0x4C5B00);
    RH_ScopedInstall(IsPlaneModel, 0x4C5B30);
    RH_ScopedInstall(IsBikeModel, 0x4C5B60);
    RH_ScopedInstall(IsFakePlaneModel, 0x4C5B90);
    RH_ScopedInstall(IsMonsterTruckModel, 0x4C5BC0);
    RH_ScopedInstall(IsQuadBikeModel, 0x4C5BF0);
    RH_ScopedInstall(IsBmxModel, 0x4C5C20);
    RH_ScopedInstall(IsTrailerModel, 0x4C5C50);
    RH_ScopedInstall(IsVehicleModelType, 0x4C5C80);
}

// 0x4C63B0
void CModelInfo::ReInit2dEffects()
{
    ms_2dFXInfoStore.m_nCount = 0;
    for (int32 i = 0; i < NUM_MODEL_INFOS; ++i)
        GetModelInfo(i)->Init2dEffects();
}

// 0x4C63E0
void CModelInfo::ShutDown()
{
    for (uint32 i = 0; i < ms_atomicModelInfoStore.m_nCount; ++i)
        ms_atomicModelInfoStore.GetItemAtIndex(i).Shutdown();

    for (uint32 i = 0; i < ms_damageAtomicModelInfoStore.m_nCount; ++i)
        ms_damageAtomicModelInfoStore.GetItemAtIndex(i).Shutdown();

    for (uint32 i = 0; i < ms_lodAtomicModelInfoStore.m_nCount; ++i)
        ms_lodAtomicModelInfoStore.GetItemAtIndex(i).Shutdown();

    for (uint32 i = 0; i < ms_timeModelInfoStore.m_nCount; ++i)
        ms_timeModelInfoStore.GetItemAtIndex(i).Shutdown();

    for (uint32 i = 0; i < ms_lodTimeModelInfoStore.m_nCount; ++i)
        ms_lodTimeModelInfoStore.GetItemAtIndex(i).Shutdown();

    for (uint32 i = 0; i < ms_weaponModelInfoStore.m_nCount; ++i)
        ms_weaponModelInfoStore.GetItemAtIndex(i).Shutdown();

    for (uint32 i = 0; i < ms_clumpModelInfoStore.m_nCount; ++i)
        ms_clumpModelInfoStore.GetItemAtIndex(i).Shutdown();

    for (uint32 i = 0; i < ms_vehicleModelInfoStore.m_nCount; ++i)
        ms_vehicleModelInfoStore.GetItemAtIndex(i).Shutdown();

    for (uint32 i = 0; i < ms_pedModelInfoStore.m_nCount; ++i)
        ms_pedModelInfoStore.GetItemAtIndex(i).Shutdown();

    for (uint32 i = 0; i < ms_2dFXInfoStore.m_nCount; ++i)
        ms_2dFXInfoStore.GetItemAtIndex(i).Shutdown();

    ms_atomicModelInfoStore.m_nCount = 0;
    ms_damageAtomicModelInfoStore.m_nCount = 0;
    ms_lodAtomicModelInfoStore.m_nCount = 0;
    ms_timeModelInfoStore.m_nCount = 0;
    ms_lodTimeModelInfoStore.m_nCount = 0;
    ms_weaponModelInfoStore.m_nCount = 0;
    ms_clumpModelInfoStore.m_nCount = 0;
    ms_vehicleModelInfoStore.m_nCount = 0;
    ms_pedModelInfoStore.m_nCount = 0;
    ms_2dFXInfoStore.m_nCount = 0;
}

// 0x4C6620
CAtomicModelInfo* CModelInfo::AddAtomicModel(int32 index)
{
    auto& mi = ms_atomicModelInfoStore.AddItem();
    mi.Init();
    SetModelInfo(index, &mi);
    return &mi;
}

// 0x4C6650
CDamageAtomicModelInfo* CModelInfo::AddDamageAtomicModel(int32 index)
{
    auto& mi = ms_damageAtomicModelInfoStore.AddItem();
    mi.Init();
    SetModelInfo(index, &mi);
    return &mi;
}

// 0x4C6680
CLodAtomicModelInfo* CModelInfo::AddLodAtomicModel(int32 index)
{
    auto& mi =  ms_lodAtomicModelInfoStore.AddItem();
    mi.Init();
    SetModelInfo(index, &mi);
    return &mi;
}

// 0x4C66B0
CTimeModelInfo* CModelInfo::AddTimeModel(int32 index)
{
    auto& mi = ms_timeModelInfoStore.AddItem();
    mi.Init();
    SetModelInfo(index, &mi);
    return &mi;
}

// 0x4C66E0
CLodTimeModelInfo* CModelInfo::AddLodTimeModel(int32 index)
{
    auto& mi = ms_lodTimeModelInfoStore.AddItem();
    mi.Init();
    SetModelInfo(index, &mi);
    return &mi;
}

// 0x4C6710
CWeaponModelInfo* CModelInfo::AddWeaponModel(int32 index)
{
    auto& mi = ms_weaponModelInfoStore.AddItem();
    mi.Init();
    SetModelInfo(index, &mi);
    return &mi;
}

// 0x4C6740
CClumpModelInfo* CModelInfo::AddClumpModel(int32 index)
{
    auto& mi = ms_clumpModelInfoStore.AddItem();
    mi.Init();
    SetModelInfo(index, &mi);
    return &mi;
}

// 0x4C6770
CVehicleModelInfo* CModelInfo::AddVehicleModel(int32 index)
{
    auto& mi = ms_vehicleModelInfoStore.AddItem();
    mi.Init();
    SetModelInfo(index, &mi);
    return &mi;
}

// 0x4C67A0
CPedModelInfo* CModelInfo::AddPedModel(int32 index)
{
    auto& mi = ms_pedModelInfoStore.AddItem();
    mi.Init();
    SetModelInfo(index, &mi);
    return &mi;
}

// 0x4C6810
void CModelInfo::Initialise() {
    ZoneScoped;

    memset(ms_modelInfoPtrs, 0, sizeof(ms_modelInfoPtrs));
    ms_damageAtomicModelInfoStore.m_nCount = 0;
    ms_lodAtomicModelInfoStore.m_nCount = 0;
    ms_timeModelInfoStore.m_nCount = 0;
    ms_lodTimeModelInfoStore.m_nCount = 0;
    ms_weaponModelInfoStore.m_nCount = 0;
    ms_clumpModelInfoStore.m_nCount = 0;
    ms_vehicleModelInfoStore.m_nCount = 0;
    ms_pedModelInfoStore.m_nCount = 0;
    ms_2dFXInfoStore.m_nCount = 0;
    ms_atomicModelInfoStore.m_nCount = 0;

    auto door1 = AddAtomicModel(MODEL_TEMPCOL_DOOR1);
    door1->SetColModel(&CTempColModels::ms_colModelDoor1, false);
    door1->SetTexDictionary("generic");
    door1->m_fDrawDistance = 80.0F;

    auto bumper1 = AddAtomicModel(MODEL_TEMPCOL_BUMPER1);
    bumper1->SetColModel(&CTempColModels::ms_colModelBumper1, false);
    bumper1->SetTexDictionary("generic");
    bumper1->m_fDrawDistance = 80.0F;

    auto modelPanel1 = AddAtomicModel(MODEL_TEMPCOL_PANEL1);
    modelPanel1->SetColModel(&CTempColModels::ms_colModelPanel1, false);
    modelPanel1->SetTexDictionary("generic");
    modelPanel1->m_fDrawDistance = 80.0F;

    auto bonnet1 = AddAtomicModel(MODEL_TEMPCOL_BONNET1);
    bonnet1->SetColModel(&CTempColModels::ms_colModelBonnet1, false);
    bonnet1->SetTexDictionary("generic");
    bonnet1->m_fDrawDistance = 80.0F;

    auto boot1 = AddAtomicModel(MODEL_TEMPCOL_BOOT1);
    boot1->SetColModel(&CTempColModels::ms_colModelBoot1, false);
    boot1->SetTexDictionary("generic");
    boot1->m_fDrawDistance = 80.0F;

    auto wheel1 = AddAtomicModel(MODEL_TEMPCOL_WHEEL1);
    wheel1->SetColModel(&CTempColModels::ms_colModelWheel1, false);
    wheel1->SetTexDictionary("generic");
    wheel1->m_fDrawDistance = 80.0F;

    auto bodyPart1 = AddAtomicModel(MODEL_TEMPCOL_BODYPART1);
    bodyPart1->SetColModel(&CTempColModels::ms_colModelBodyPart1, false);
    bodyPart1->SetTexDictionary("generic");
    bodyPart1->m_fDrawDistance = 80.0F;

    auto bodyPart2 = AddAtomicModel(MODEL_TEMPCOL_BODYPART2);
    bodyPart2->SetColModel(&CTempColModels::ms_colModelBodyPart2, false);
    bodyPart2->SetTexDictionary("generic");
    bodyPart2->m_fDrawDistance = 80.0F;
}

// 0x4C5940
CBaseModelInfo* CModelInfo::GetModelInfo(const char* name, int32* index)
{
    auto iKey = CKeyGen::GetUppercaseKey(name);
    auto iCurInd = ms_lastPositionSearched;

    while (iCurInd < NUM_MODEL_INFOS) {
        auto mi = GetModelInfo(iCurInd);
        if (mi && mi->m_nKey == iKey) {
            ms_lastPositionSearched = iCurInd;
            if (index)
                *index = iCurInd;

            return mi;
        }

        ++iCurInd;
    }

    iCurInd = ms_lastPositionSearched;
    if (iCurInd < 0)
        return nullptr;

    while (iCurInd >= 0) {
        auto mi = GetModelInfo(iCurInd);
        if (mi && mi->m_nKey == iKey) {
            ms_lastPositionSearched = iCurInd;
            if (index)
                *index = iCurInd;

            return mi;
        }

        --iCurInd;
    }

    return nullptr;
}

// 0x4C59B0
CBaseModelInfo* CModelInfo::GetModelInfoFromHashKey(uint32 uiHash, int32* index)
{
    for (int32 i = 0; i < NUM_MODEL_INFOS; ++i) {
        auto mi = GetModelInfo(i);
        if (mi && mi->m_nKey == uiHash) {
            if (index)
                *index = i;

            return mi;
        }
    }

    return nullptr;
}

// 0x4C59F0
CBaseModelInfo* CModelInfo::GetModelInfoUInt16(const char* name, uint16* pOutIndex)
{
    int32 modelId = 0;
    auto result = GetModelInfo(name, &modelId);
    if (pOutIndex)
        *pOutIndex = modelId;

    return result;
}

// 0x4C5A20
CBaseModelInfo* CModelInfo::GetModelInfo(const char* name, int32 minIndex, int32 maxIndex)
{
    auto iKey = CKeyGen::GetUppercaseKey(name);
    if (minIndex > maxIndex)
        return nullptr;

    for (int32 i = minIndex; i <= maxIndex; ++i) {
        auto mi = GetModelInfo(i);
        if (mi && mi->m_nKey == iKey)
            return mi;
    }

    return nullptr;
}

// 0x4C5A60
CStore<C2dEffect, CModelInfo::NUM_2DFX_INFOS>* CModelInfo::Get2dEffectStore()
{
    return ((CStore<C2dEffect, NUM_2DFX_INFOS>* (__cdecl *)())0x4C5A60)();
}

// 0x4C5A70
bool CModelInfo::IsBoatModel(int32 index)
{
    auto mi = GetModelInfo(index);
    if (!mi)
        return false;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return mi->AsVehicleModelInfoPtr()->IsBoat();
}

// 0x4C5AA0
bool CModelInfo::IsCarModel(int32 index)
{
    auto mi = GetModelInfo(index);
    if (!mi)
        return false;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return mi->AsVehicleModelInfoPtr()->IsAutomobile();
}

// 0x4C5AD0
bool CModelInfo::IsTrainModel(int32 index)
{
    auto mi = GetModelInfo(index);
    if (!mi)
        return false;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return mi->AsVehicleModelInfoPtr()->IsTrain();
}

// 0x4C5B00
bool CModelInfo::IsHeliModel(int32 index)
{
    auto mi = GetModelInfo(index);
    if (!mi)
        return false;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return mi->AsVehicleModelInfoPtr()->IsHeli();
}

// 0x4C5B30
bool CModelInfo::IsPlaneModel(int32 index)
{
    auto mi = GetModelInfo(index);
    if (!mi)
        return false;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return mi->AsVehicleModelInfoPtr()->IsPlane();
}

// 0x4C5B60
bool CModelInfo::IsBikeModel(int32 index)
{
    auto mi = GetModelInfo(index);
    if (!mi)
        return false;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return mi->AsVehicleModelInfoPtr()->IsBike();
}

// 0x4C5B90
bool CModelInfo::IsFakePlaneModel(int32 index)
{
    auto mi = GetModelInfo(index);
    if (!mi)
        return false;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return mi->AsVehicleModelInfoPtr()->m_nVehicleType == VEHICLE_TYPE_FPLANE;
}

// 0x4C5BC0
bool CModelInfo::IsMonsterTruckModel(int32 index)
{
    auto mi = GetModelInfo(index);
    if (!mi)
        return false;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return mi->AsVehicleModelInfoPtr()->IsMonsterTruck();
}

// 0x4C5BF0
bool CModelInfo::IsQuadBikeModel(int32 index)
{
    auto mi = GetModelInfo(index);
    if (!mi)
        return false;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return mi->AsVehicleModelInfoPtr()->IsQuad();
}

// 0x4C5C20
bool CModelInfo::IsBmxModel(int32 index)
{
    auto mi = GetModelInfo(index);
    if (!mi)
        return false;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return mi->AsVehicleModelInfoPtr()->IsBMX();
}

// 0x4C5C50
bool CModelInfo::IsTrailerModel(int32 index)
{
    auto mi = GetModelInfo(index);
    if (!mi)
        return false;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return mi->AsVehicleModelInfoPtr()->IsTrailer();
}

// 0x4C5C80
int32 CModelInfo::IsVehicleModelType(int32 index)
{
    if (index >= NUM_MODEL_INFOS)
        return -1;

    auto mi = GetModelInfo(index);
    if (!mi)
        return -1;

    if (mi->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return -1;

    return mi->AsVehicleModelInfoPtr()->m_nVehicleType;
}
