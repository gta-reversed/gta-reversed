/*
Plugin-SDK (Grand Theft Auto San Andreas) source file
Authors: GTA Community. See more here
https://github.com/DK22Pac/plugin-sdk
Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

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
    ReversibleHooks::Install("CModelInfo", "Initialise", 0x4C6810, &CModelInfo::Initialise);
    ReversibleHooks::Install("CModelInfo", "ShutDown", 0x4C63E0, &CModelInfo::ShutDown);
    ReversibleHooks::Install("CModelInfo", "ReInit2dEffects", 0x4C63B0, &CModelInfo::ReInit2dEffects);

    ReversibleHooks::Install("CModelInfo", "GetModelInfoUInt16", 0x4C59F0, &CModelInfo::GetModelInfoUInt16);
    ReversibleHooks::Install("CModelInfo", "GetModelInfoFromHashKey", 0x4C59B0, &CModelInfo::GetModelInfoFromHashKey);
    ReversibleHooks::Install("CModelInfo", "GetModelInfo_full", 0x4C5940, (CBaseModelInfo * (*)(char const*, int32*)) & CModelInfo::GetModelInfo);
    ReversibleHooks::Install("CModelInfo", "GetModelInfo_minmax", 0x4C5A20, (CBaseModelInfo*(*)(char const*, int32, int32))&CModelInfo::GetModelInfo);

    ReversibleHooks::Install("CModelInfo", "AddAtomicModel", 0x4C6620, &CModelInfo::AddAtomicModel);
    ReversibleHooks::Install("CModelInfo", "AddDamageAtomicModel", 0x4C6650, &CModelInfo::AddDamageAtomicModel);
    ReversibleHooks::Install("CModelInfo", "AddLodAtomicModel", 0x4C6680, &CModelInfo::AddLodAtomicModel);
    ReversibleHooks::Install("CModelInfo", "AddTimeModel", 0x4C66B0, &CModelInfo::AddTimeModel);
    ReversibleHooks::Install("CModelInfo", "AddLodTimeModel", 0x4C66E0, &CModelInfo::AddLodTimeModel);
    ReversibleHooks::Install("CModelInfo", "AddWeaponModel", 0x4C6710, &CModelInfo::AddWeaponModel);
    ReversibleHooks::Install("CModelInfo", "AddClumpModel", 0x4C6740, &CModelInfo::AddClumpModel);
    ReversibleHooks::Install("CModelInfo", "AddVehicleModel", 0x4C6770, &CModelInfo::AddVehicleModel);
    ReversibleHooks::Install("CModelInfo", "AddPedModel", 0x4C67A0, &CModelInfo::AddPedModel);

    ReversibleHooks::Install("CModelInfo", "IsBoatModel", 0x4C5A70, &CModelInfo::IsBoatModel);
    ReversibleHooks::Install("CModelInfo", "IsCarModel", 0x4C5AA0, &CModelInfo::IsCarModel);
    ReversibleHooks::Install("CModelInfo", "IsTrainModel", 0x4C5AD0, &CModelInfo::IsTrainModel);
    ReversibleHooks::Install("CModelInfo", "IsHeliModel", 0x4C5B00, &CModelInfo::IsHeliModel);
    ReversibleHooks::Install("CModelInfo", "IsPlaneModel", 0x4C5B30, &CModelInfo::IsPlaneModel);
    ReversibleHooks::Install("CModelInfo", "IsBikeModel", 0x4C5B60, &CModelInfo::IsBikeModel);
    ReversibleHooks::Install("CModelInfo", "IsFakePlaneModel", 0x4C5B90, &CModelInfo::IsFakePlaneModel);
    ReversibleHooks::Install("CModelInfo", "IsMonsterTruckModel", 0x4C5BC0, &CModelInfo::IsMonsterTruckModel);
    ReversibleHooks::Install("CModelInfo", "IsQuadBikeModel", 0x4C5BF0, &CModelInfo::IsQuadBikeModel);
    ReversibleHooks::Install("CModelInfo", "IsBmxModel", 0x4C5C20, &CModelInfo::IsBmxModel);
    ReversibleHooks::Install("CModelInfo", "IsTrailerModel", 0x4C5C50, &CModelInfo::IsTrailerModel);
    ReversibleHooks::Install("CModelInfo", "IsVehicleModelType", 0x4C5C80, &CModelInfo::IsVehicleModelType);
}

// 0x4C63B0
void CModelInfo::ReInit2dEffects()
{
    ms_2dFXInfoStore.m_nCount = 0;
    for (int32 i = 0; i < NUM_MODEL_INFOS; ++i)
        CModelInfo::GetModelInfo(i)->Init2dEffects();
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

    for (uint32 i = 0; i < ms_2dFXInfoStore.m_nCount; ++i) {
        auto& pEffect = ms_2dFXInfoStore.GetItemAtIndex(i);
        pEffect.Shutdown();
    }

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
    auto& pInfo = ms_atomicModelInfoStore.AddItem();
    pInfo.Init();
    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

// 0x4C6650
CDamageAtomicModelInfo* CModelInfo::AddDamageAtomicModel(int32 index)
{
    auto& pInfo = ms_damageAtomicModelInfoStore.AddItem();
    pInfo.Init();
    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

// 0x4C6680
CLodAtomicModelInfo* CModelInfo::AddLodAtomicModel(int32 index)
{
    auto& pInfo =  ms_lodAtomicModelInfoStore.AddItem();
    pInfo.Init();
    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

// 0x4C66B0
CTimeModelInfo* CModelInfo::AddTimeModel(int32 index)
{
    auto& pInfo = ms_timeModelInfoStore.AddItem();
    pInfo.Init();
    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

// 0x4C66E0
CLodTimeModelInfo* CModelInfo::AddLodTimeModel(int32 index)
{
    auto& pInfo = ms_lodTimeModelInfoStore.AddItem();
    pInfo.Init();
    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

// 0x4C6710
CWeaponModelInfo* CModelInfo::AddWeaponModel(int32 index)
{
    auto& pInfo = ms_weaponModelInfoStore.AddItem();
    pInfo.Init();
    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

// 0x4C6740
CClumpModelInfo* CModelInfo::AddClumpModel(int32 index)
{
    auto& pInfo = ms_clumpModelInfoStore.AddItem();
    pInfo.Init();
    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

// 0x4C6770
CVehicleModelInfo* CModelInfo::AddVehicleModel(int32 index)
{
    auto& pInfo = ms_vehicleModelInfoStore.AddItem();
    pInfo.Init();
    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

// 0x4C67A0
CPedModelInfo* CModelInfo::AddPedModel(int32 index)
{
    auto& pInfo = ms_pedModelInfoStore.AddItem();
    pInfo.Init();
    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

// 0x4C6810
void CModelInfo::Initialise()
{
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

    auto pDoor1 = CModelInfo::AddAtomicModel(eModelID::MODEL_TEMPCOL_DOOR1);
    pDoor1->SetColModel(&CTempColModels::ms_colModelDoor1, false);
    pDoor1->SetTexDictionary("generic");
    pDoor1->m_fDrawDistance = 80.0F;

    auto pBumper1 = CModelInfo::AddAtomicModel(eModelID::MODEL_TEMPCOL_BUMPER1);
    pBumper1->SetColModel(&CTempColModels::ms_colModelBumper1, false);
    pBumper1->SetTexDictionary("generic");
    pBumper1->m_fDrawDistance = 80.0F;

    auto pModelPanel1 = CModelInfo::AddAtomicModel(eModelID::MODEL_TEMPCOL_PANEL1);
    pModelPanel1->SetColModel(&CTempColModels::ms_colModelPanel1, false);
    pModelPanel1->SetTexDictionary("generic");
    pModelPanel1->m_fDrawDistance = 80.0F;

    auto pBonnet1 = CModelInfo::AddAtomicModel(eModelID::MODEL_TEMPCOL_BONNET1);
    pBonnet1->SetColModel(&CTempColModels::ms_colModelBonnet1, false);
    pBonnet1->SetTexDictionary("generic");
    pBonnet1->m_fDrawDistance = 80.0F;

    auto pBoot1 = CModelInfo::AddAtomicModel(eModelID::MODEL_TEMPCOL_BOOT1);
    pBoot1->SetColModel(&CTempColModels::ms_colModelBoot1, false);
    pBoot1->SetTexDictionary("generic");
    pBoot1->m_fDrawDistance = 80.0F;

    auto pWheel1 = CModelInfo::AddAtomicModel(eModelID::MODEL_TEMPCOL_WHEEL1);
    pWheel1->SetColModel(&CTempColModels::ms_colModelWheel1, false);
    pWheel1->SetTexDictionary("generic");
    pWheel1->m_fDrawDistance = 80.0F;

    auto pBodyPart1 = CModelInfo::AddAtomicModel(eModelID::MODEL_TEMPCOL_BODYPART1);
    pBodyPart1->SetColModel(&CTempColModels::ms_colModelBodyPart1, false);
    pBodyPart1->SetTexDictionary("generic");
    pBodyPart1->m_fDrawDistance = 80.0F;

    auto pBodyPart2 = CModelInfo::AddAtomicModel(eModelID::MODEL_TEMPCOL_BODYPART2);
    pBodyPart2->SetColModel(&CTempColModels::ms_colModelBodyPart2, false);
    pBodyPart2->SetTexDictionary("generic");
    pBodyPart2->m_fDrawDistance = 80.0F;
}

// 0x4C5940
CBaseModelInfo* CModelInfo::GetModelInfo(const char* name, int32* index)
{
    auto iKey = CKeyGen::GetUppercaseKey(name);
    auto iCurInd = CModelInfo::ms_lastPositionSearched;

    while (iCurInd < NUM_MODEL_INFOS) {
        auto pInfo = CModelInfo::GetModelInfo(iCurInd);
        if (pInfo && pInfo->m_nKey == iKey) {
            CModelInfo::ms_lastPositionSearched = iCurInd;
            if (index)
                *index = iCurInd;

            return pInfo;
        }

        ++iCurInd;
    }

    iCurInd = CModelInfo::ms_lastPositionSearched;
    if (iCurInd < 0)
        return nullptr;

    while (iCurInd >= 0) {
        auto pInfo = CModelInfo::GetModelInfo(iCurInd);
        if (pInfo && pInfo->m_nKey == iKey) {
            CModelInfo::ms_lastPositionSearched = iCurInd;
            if (index)
                *index = iCurInd;

            return pInfo;
        }

        --iCurInd;
    }

    return nullptr;
}

// 0x4C59B0
CBaseModelInfo* CModelInfo::GetModelInfoFromHashKey(uint32 uiHash, int32* index)
{
    for (int32 i = 0; i < NUM_MODEL_INFOS; ++i) {
        auto pInfo = CModelInfo::GetModelInfo(i);
        if (pInfo && pInfo->m_nKey == uiHash) {
            if (index)
                *index = i;

            return pInfo;
        }
    }

    return nullptr;
}

// 0x4C59F0
CBaseModelInfo* CModelInfo::GetModelInfoUInt16(const char* name, uint16* pOutIndex)
{
    int32 modelId = 0;
    auto result = CModelInfo::GetModelInfo(name, &modelId);
    if (pOutIndex)
        *pOutIndex = modelId;

    return result;
}

// 0x4C5A20
CBaseModelInfo* CModelInfo::GetModelInfo(char const* name, int32 minIndex, int32 maxIndex)
{
    auto iKey = CKeyGen::GetUppercaseKey(name);
    if (minIndex > maxIndex)
        return nullptr;

    for (int32 i = minIndex; i <= maxIndex; ++i) {
        auto pInfo = CModelInfo::GetModelInfo(i);
        if (pInfo && pInfo->m_nKey == iKey)
            return pInfo;
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
    auto pInfo = CModelInfo::GetModelInfo(index);
    if (!pInfo)
        return false;

    if (pInfo->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return pInfo->AsVehicleModelInfoPtr()->m_nVehicleType == eVehicleType::VEHICLE_BOAT;
}

// 0x4C5AA0
bool CModelInfo::IsCarModel(int32 index)
{
    auto pInfo = CModelInfo::GetModelInfo(index);
    if (!pInfo)
        return false;

    if (pInfo->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return pInfo->AsVehicleModelInfoPtr()->m_nVehicleType == eVehicleType::VEHICLE_AUTOMOBILE;
}

// 0x4C5AD0
bool CModelInfo::IsTrainModel(int32 index)
{
    auto pInfo = CModelInfo::GetModelInfo(index);
    if (!pInfo)
        return false;

    if (pInfo->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return pInfo->AsVehicleModelInfoPtr()->m_nVehicleType == eVehicleType::VEHICLE_TRAIN;
}

// 0x4C5B00
bool CModelInfo::IsHeliModel(int32 index)
{
    auto pInfo = CModelInfo::GetModelInfo(index);
    if (!pInfo)
        return false;

    if (pInfo->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return pInfo->AsVehicleModelInfoPtr()->m_nVehicleType == eVehicleType::VEHICLE_HELI;
}

// 0x4C5B30
bool CModelInfo::IsPlaneModel(int32 index)
{
    auto pInfo = CModelInfo::GetModelInfo(index);
    if (!pInfo)
        return false;

    if (pInfo->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return pInfo->AsVehicleModelInfoPtr()->m_nVehicleType == eVehicleType::VEHICLE_PLANE;
}

// 0x4C5B60
bool CModelInfo::IsBikeModel(int32 index)
{
    auto pInfo = CModelInfo::GetModelInfo(index);
    if (!pInfo)
        return false;

    if (pInfo->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return pInfo->AsVehicleModelInfoPtr()->m_nVehicleType == eVehicleType::VEHICLE_BIKE;
}

// 0x4C5B90
bool CModelInfo::IsFakePlaneModel(int32 index)
{
    auto pInfo = CModelInfo::GetModelInfo(index);
    if (!pInfo)
        return false;

    if (pInfo->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return pInfo->AsVehicleModelInfoPtr()->m_nVehicleType == eVehicleType::VEHICLE_FPLANE;
}

// 0x4C5BC0
bool CModelInfo::IsMonsterTruckModel(int32 index)
{
    auto pInfo = CModelInfo::GetModelInfo(index);
    if (!pInfo)
        return false;

    if (pInfo->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return pInfo->AsVehicleModelInfoPtr()->m_nVehicleType == eVehicleType::VEHICLE_MTRUCK;
}

// 0x4C5BF0
bool CModelInfo::IsQuadBikeModel(int32 index)
{
    auto pInfo = CModelInfo::GetModelInfo(index);
    if (!pInfo)
        return false;

    if (pInfo->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return pInfo->AsVehicleModelInfoPtr()->m_nVehicleType == eVehicleType::VEHICLE_QUAD;
}

// 0x4C5C20
bool CModelInfo::IsBmxModel(int32 index)
{
    auto pInfo = CModelInfo::GetModelInfo(index);
    if (!pInfo)
        return false;

    if (pInfo->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return pInfo->AsVehicleModelInfoPtr()->m_nVehicleType == eVehicleType::VEHICLE_BMX;
}

// 0x4C5C50
bool CModelInfo::IsTrailerModel(int32 index)
{
    auto pInfo = CModelInfo::GetModelInfo(index);
    if (!pInfo)
        return false;

    if (pInfo->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return false;

    return pInfo->AsVehicleModelInfoPtr()->m_nVehicleType == eVehicleType::VEHICLE_TRAILER;
}

// 0x4C5C80
int32 CModelInfo::IsVehicleModelType(int32 index)
{
    if (index >= NUM_MODEL_INFOS)
        return -1;

    auto pInfo = CModelInfo::GetModelInfo(index);
    if (!pInfo)
        return -1;

    if (pInfo->GetModelType() != ModelInfoType::MODEL_INFO_VEHICLE)
        return -1;

    return pInfo->AsVehicleModelInfoPtr()->m_nVehicleType;
}
