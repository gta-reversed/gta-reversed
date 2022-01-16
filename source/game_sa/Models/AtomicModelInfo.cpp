/*
    Plugin-SDK (Grand Theft Auto San Andreas) source file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

void CAtomicModelInfo::InjectHooks()
{
// VTABLE
    ReversibleHooks::Install("CAtomicModelInfo", "AsAtomicModelInfoPtr", 0x4C5560, &CAtomicModelInfo::AsAtomicModelInfoPtr_Reversed);
    ReversibleHooks::Install("CAtomicModelInfo", "GetModelType", 0x4C5570, &CAtomicModelInfo::GetModelType_Reversed);
    ReversibleHooks::Install("CAtomicModelInfo", "Init", 0x4C4430, &CAtomicModelInfo::Init_Reversed);
    ReversibleHooks::Install("CAtomicModelInfo", "DeleteRwObject", 0x4C4440, &CAtomicModelInfo::DeleteRwObject_Reversed);
    ReversibleHooks::Install("CAtomicModelInfo", "GetRwModelType", 0x4C5580, &CAtomicModelInfo::GetRwModelType_Reversed);
    ReversibleHooks::Install("CAtomicModelInfo", "CreateInstance_void", 0x4C4530, (RwObject * (CAtomicModelInfo::*)())(&CAtomicModelInfo::CreateInstance_Reversed));
    ReversibleHooks::Install("CAtomicModelInfo", "CreateInstance_rwmat", 0x4C44D0, (RwObject * (CAtomicModelInfo::*)(RwMatrix*))(&CAtomicModelInfo::CreateInstance_Reversed));
    ReversibleHooks::Install("CAtomicModelInfo", "SetAtomic", 0x4C4360, &CAtomicModelInfo::SetAtomic_Reversed);

// OTHER
    ReversibleHooks::Install("CAtomicModelInfo", "GetAtomicFromDistance", 0x4C44B0, &CAtomicModelInfo::GetAtomicFromDistance);
    ReversibleHooks::Install("CAtomicModelInfo", "SetupVehicleUpgradeFlags", 0x4C4570, &CAtomicModelInfo::SetupVehicleUpgradeFlags);
    ReversibleHooks::Install("CAtomicModelInfo", "SetAtomicModelInfoFlags", 0x5B3B20, &SetAtomicModelInfoFlags);
}

CAtomicModelInfo* CAtomicModelInfo::AsAtomicModelInfoPtr()
{
    return CAtomicModelInfo::AsAtomicModelInfoPtr_Reversed();
}
CAtomicModelInfo* CAtomicModelInfo::AsAtomicModelInfoPtr_Reversed()
{
    return this;
}

ModelInfoType CAtomicModelInfo::GetModelType()
{
    return CAtomicModelInfo::GetModelType_Reversed();
}
ModelInfoType CAtomicModelInfo::GetModelType_Reversed()
{
    return ModelInfoType::MODEL_INFO_ATOMIC;
}

void CAtomicModelInfo::Init()
{
    return CAtomicModelInfo::Init_Reversed();
}
void CAtomicModelInfo::Init_Reversed()
{
    return CBaseModelInfo::Init();
}

void CAtomicModelInfo::DeleteRwObject()
{
    CAtomicModelInfo::DeleteRwObject_Reversed();
}
void CAtomicModelInfo::DeleteRwObject_Reversed()
{
    if (!m_pRwAtomic)
        return;

    auto uiEffectsCount = RpGeometryGet2dFxCount(RpAtomicGetGeometry(m_pRwAtomic));
    m_n2dfxCount -= uiEffectsCount;

    auto pFrame = RpAtomicGetFrame(m_pRwAtomic);
    RpAtomicDestroy(m_pRwAtomic);
    RwFrameDestroy(pFrame);
    m_pRwObject = nullptr;

    CBaseModelInfo::RemoveTexDictionaryRef();
    auto iAnimIndex = GetAnimFileIndex();
    if (iAnimIndex != -1)
        CAnimManager::RemoveAnimBlockRef(iAnimIndex);
}

uint32 CAtomicModelInfo::GetRwModelType()
{
    return CAtomicModelInfo::GetRwModelType_Reversed();
}
uint32 CAtomicModelInfo::GetRwModelType_Reversed()
{
    return rpATOMIC;
}

RwObject* CAtomicModelInfo::CreateInstance()
{
    return CAtomicModelInfo::CreateInstance_Reversed();
}
RwObject* CAtomicModelInfo::CreateInstance_Reversed()
{
    if (!m_pRwObject)
        return nullptr;

    CBaseModelInfo::AddRef();
    auto pClonedAtomic = RpAtomicClone(m_pRwAtomic);
    auto pFrame = RwFrameCreate();
    RpAtomicSetFrame(pClonedAtomic, pFrame);
    CBaseModelInfo::RemoveRef();

    return reinterpret_cast<RwObject*>(pClonedAtomic);
}

RwObject* CAtomicModelInfo::CreateInstance(RwMatrix* matrix)
{
    return CAtomicModelInfo::CreateInstance_Reversed(matrix);
}
RwObject* CAtomicModelInfo::CreateInstance_Reversed(RwMatrix* matrix)
{
    if (!m_pRwObject)
        return nullptr;

    CBaseModelInfo::AddRef();
    auto pClonedAtomic = RpAtomicClone(m_pRwAtomic);
    auto pFrame = RwFrameCreate();
    memcpy(RwFrameGetMatrix(pFrame), matrix, sizeof(RwMatrix));
    RpAtomicSetFrame(pClonedAtomic, pFrame);
    CBaseModelInfo::RemoveRef();

    return reinterpret_cast<RwObject*>(pClonedAtomic);
}

void CAtomicModelInfo::SetAtomic(RpAtomic* atomic)
{
    CAtomicModelInfo::SetAtomic_Reversed(atomic);
}
void CAtomicModelInfo::SetAtomic_Reversed(RpAtomic* atomic)
{
    if (m_pRwObject) {
        auto uiEffectsCount = RpGeometryGet2dFxCount(RpAtomicGetGeometry(m_pRwAtomic));
        m_n2dfxCount -= uiEffectsCount;
    }

    m_pRwAtomic = atomic;
    auto uiNewEffectsCount = RpGeometryGet2dFxCount(RpAtomicGetGeometry(m_pRwAtomic));
    m_n2dfxCount += uiNewEffectsCount;

    CBaseModelInfo::AddTexDictionaryRef();
    auto iAnimIndex = GetAnimFileIndex();
    if (iAnimIndex != -1)
        CAnimManager::AddAnimBlockRef(iAnimIndex);

    if (CCustomBuildingRenderer::IsCBPCPipelineAttached(m_pRwAtomic))
        CCustomBuildingRenderer::AtomicSetup(m_pRwAtomic);
    else if (CCarFXRenderer::IsCCPCPipelineAttached(m_pRwAtomic))
        CCarFXRenderer::SetCustomFXAtomicRenderPipelinesVMICB(m_pRwAtomic, nullptr);

    if (!bTagDisabled && IsTagModel())
        CTagManager::SetupAtomic(m_pRwAtomic);

    SetHasBeenPreRendered(true);
}

RpAtomic* CAtomicModelInfo::GetAtomicFromDistance(float distance)
{
    if (TheCamera.m_fLODDistMultiplier * m_fDrawDistance <= distance)
        return nullptr;

    return m_pRwAtomic;
}

void CAtomicModelInfo::SetupVehicleUpgradeFlags(char const* name)
{
    if (bUseCommonVehicleDictionary)
        return;

    const tVehicleComponentFlag aDummyComps[] = {
        {"chss_",   0x1},
        {"wheel_",  0x2},
        {"exh_",    0x13},
        {"fbmp_",   0xC},
        {"rbmp_",   0xD},
        {"misc_a_", 0x14},
        {"misc_b_", 0x15},
        {"misc_c_", 0x16},
        {nullptr, 0}
    };

    const tVehicleComponentFlag aChassisComps[] = {
        {"bnt_",      0x0},
        {"bntl_",     0x1},
        {"bntr_",     0x2},
        {"spl_",      0x6},
        {"wg_l_",     0x8},
        {"wg_r",      0x9},
        {"fbb_",      0xA},
        {"bbb_",      0xB},
        {"lgt_",      0xC},
        {"rf_",       0xE},
        {"nto_",      0xF},
        {"hydralics", 0x10},
        {"stereo",    0x11},
        {nullptr, 0}
    };

    auto pChassis = aChassisComps;
    while (pChassis->m_ucName) {
        if (strncmp(pChassis->m_ucName, name, strlen(pChassis->m_ucName))) {
            ++pChassis;
            continue;
        }

        bUseCommonVehicleDictionary = true;
        nCarmodId = pChassis->m_nFlag;
        return;
    }

    auto pDummy = aDummyComps;
    while (pDummy->m_ucName) {
        if (strncmp(pDummy->m_ucName, name, strlen(pDummy->m_ucName))) {
            ++pDummy;
            continue;
        }

        bUseCommonVehicleDictionary = true;
        bUsesVehDummy = true;
        nCarmodId = pDummy->m_nFlag;
        return;
    }
}

void SetAtomicModelInfoFlags(CAtomicModelInfo* modelInfo, uint32 dwFlags)
{
    SetBaseModelInfoFlags(modelInfo, dwFlags);

    auto flagsStruct = sItemDefinitionFlags(dwFlags);
    if (flagsStruct.bIsGlassType1)
        modelInfo->nSpecialType = eModelInfoSpecialType::GLASS_TYPE_1;

    if (flagsStruct.bIsGlassType2)
        modelInfo->nSpecialType = eModelInfoSpecialType::GLASS_TYPE_2;

    if (flagsStruct.bIsGarageDoor)
        modelInfo->nSpecialType = eModelInfoSpecialType::GARAGE_DOOR;

    if (flagsStruct.bIsTree)
        modelInfo->nSpecialType = eModelInfoSpecialType::TREE;

    if (flagsStruct.bIsPalm)
        modelInfo->nSpecialType = eModelInfoSpecialType::PALM;

    if (flagsStruct.bIsUnknown)
        modelInfo->nSpecialType = eModelInfoSpecialType::UNKNOWN;

    if (flagsStruct.bIsTag)
        modelInfo->nSpecialType = eModelInfoSpecialType::TAG;

    if (flagsStruct.bisBreakableStatue)
        modelInfo->nSpecialType = eModelInfoSpecialType::BREAKABLE_STATUE;


    modelInfo->bIsRoad = flagsStruct.bIsRoad;
    modelInfo->bDontCollideWithFlyer = flagsStruct.bDoesNotCollideWithFlyer;
}
