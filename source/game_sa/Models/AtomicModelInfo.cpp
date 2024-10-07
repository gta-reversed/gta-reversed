/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "AtomicModelInfo.h"
#include "TagManager.h"
#include "CustomBuildingRenderer.h"
#include "CarFXRenderer.h"

void CAtomicModelInfo::InjectHooks()
{
    RH_ScopedVirtualClass(CAtomicModelInfo, 0x85BBF0, 16);
    RH_ScopedCategory("Models");

    RH_ScopedVMTInstall(AsAtomicModelInfoPtr, 0x4C5560);
    RH_ScopedVMTInstall(GetModelType, 0x4C5570);
    RH_ScopedVMTInstall(Init, 0x4C4430);
    RH_ScopedVMTInstall(DeleteRwObject, 0x4C4440);
    RH_ScopedVMTInstall(GetRwModelType, 0x4C5580);
    // clang moment: RH_ScopedVirtualOverloadedInstall(CreateInstance, "void", 0x4C4530, RwObject * (CAtomicModelInfo::*)());
    // clang moment: RH_ScopedVirtualOverloadedInstall(CreateInstance, "rwmat", 0x4C44D0, RwObject * (CAtomicModelInfo::*)(RwMatrix*));
    RH_ScopedVMTInstall(SetAtomic, 0x4C4360);

    RH_ScopedInstall(GetAtomicFromDistance, 0x4C44B0);
    RH_ScopedInstall(SetupVehicleUpgradeFlags, 0x4C4570);
    RH_ScopedGlobalInstall(SetAtomicModelInfoFlags, 0x5B3B20);
}

// 0x4C5560
CAtomicModelInfo* CAtomicModelInfo::AsAtomicModelInfoPtr()
{
    return this;
}

// 0x4C5570
ModelInfoType CAtomicModelInfo::GetModelType()
{
    return ModelInfoType::MODEL_INFO_ATOMIC;
}

// 0x4C4430
void CAtomicModelInfo::Init()
{
    return CBaseModelInfo::Init();
}

// 0x4C4440
void CAtomicModelInfo::DeleteRwObject()
{
    if (!m_pRwAtomic)
        return;

    auto uiEffectsCount = RpGeometryGet2dFxCount(RpAtomicGetGeometry(m_pRwAtomic));
    m_n2dfxCount -= uiEffectsCount;

    auto frame = RpAtomicGetFrame(m_pRwAtomic);
    RpAtomicDestroy(m_pRwAtomic);
    RwFrameDestroy(frame);
    m_pRwObject = nullptr;

    CBaseModelInfo::RemoveTexDictionaryRef();
    auto iAnimIndex = GetAnimFileIndex();
    if (iAnimIndex != -1)
        CAnimManager::RemoveAnimBlockRef(iAnimIndex);
}

// 0x4C5580
uint32 CAtomicModelInfo::GetRwModelType()
{
    return rpATOMIC;
}

RwObject* CAtomicModelInfo::CreateInstance()
{
    if (!m_pRwObject)
        return nullptr;

    CBaseModelInfo::AddRef();
    auto clonedAtomic = RpAtomicClone(m_pRwAtomic);
    auto frame = RwFrameCreate();
    RpAtomicSetFrame(clonedAtomic, frame);
    CBaseModelInfo::RemoveRef();

    return reinterpret_cast<RwObject*>(clonedAtomic);
}

RwObject* CAtomicModelInfo::CreateInstance(RwMatrix* matrix)
{
    if (!m_pRwObject)
        return nullptr;

    CBaseModelInfo::AddRef();
    auto clonedAtomic = RpAtomicClone(m_pRwAtomic);
    auto frame = RwFrameCreate();
    memcpy(RwFrameGetMatrix(frame), matrix, sizeof(RwMatrix));
    RpAtomicSetFrame(clonedAtomic, frame);
    CBaseModelInfo::RemoveRef();

    return reinterpret_cast<RwObject*>(clonedAtomic);
}

// 0x4C4360
void CAtomicModelInfo::SetAtomic(RpAtomic* atomic)
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
}RpAtomic* CAtomicModelInfo::GetAtomicFromDistance(float distance)
{
    if (TheCamera.m_fLODDistMultiplier * m_fDrawDistance <= distance)
        return nullptr;

    return m_pRwAtomic;
}

void CAtomicModelInfo::SetupVehicleUpgradeFlags(const char* name)
{
    if (bUseCommonVehicleDictionary)
        return;

    const tVehicleComponentFlag aDummyComps[] = {
        { "chss_",   0x1  },
        { "wheel_",  0x2  },
        { "exh_",    0x13 },
        { "fbmp_",   0xC  },
        { "rbmp_",   0xD  },
        { "misc_a_", 0x14 },
        { "misc_b_", 0x15 },
        { "misc_c_", 0x16 },
        { nullptr,   0x0  }
    };

    const tVehicleComponentFlag aChassisComps[] = {
        { "bnt_",      0x0  },
        { "bntl_",     0x1  },
        { "bntr_",     0x2  },
        { "spl_",      0x6  },
        { "wg_l_",     0x8  },
        { "wg_r",      0x9  },
        { "fbb_",      0xA  },
        { "bbb_",      0xB  },
        { "lgt_",      0xC  },
        { "rf_",       0xE  },
        { "nto_",      0xF  },
        { "hydralics", 0x10 },
        { "stereo",    0x11 },
        { nullptr,     0x0  }
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
