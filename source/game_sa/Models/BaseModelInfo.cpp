/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "BaseModelInfo.h"

void CBaseModelInfo::InjectHooks() {
    RH_ScopedVirtualClass(CBaseModelInfo, 0x85BB9C, 15);
    RH_ScopedCategory("Models");

    RH_ScopedVMTInstall(AsAtomicModelInfoPtr, 0x4C4A80);
    RH_ScopedVMTInstall(AsDamageAtomicModelInfoPtr, 0x4C4A90);
    RH_ScopedVMTInstall(AsLodAtomicModelInfoPtr, 0x4C4AA0);
    RH_ScopedVMTInstall(GetTimeInfo, 0x4C4AB0);
    RH_ScopedVMTInstall(Init, 0x4C4B10);
    RH_ScopedVMTInstall(Shutdown, 0x4C4D50);
    RH_ScopedVMTInstall(SetAnimFile, 0x4C4AC0);
    RH_ScopedVMTInstall(ConvertAnimFileIndex, 0x4C4AD0);
    RH_ScopedVMTInstall(GetAnimFileIndex, 0x4C4AE0);
    RH_ScopedInstall(SetTexDictionary, 0x4C4B40);
    RH_ScopedInstall(ClearTexDictionary, 0x4C4B70);
    RH_ScopedInstall(AddTexDictionaryRef, 0x4C4B80);
    RH_ScopedInstall(RemoveTexDictionaryRef, 0x4C4B90);
    RH_ScopedInstall(AddRef, 0x4C4BA0);
    RH_ScopedInstall(RemoveRef, 0x4C4BB0);
    RH_ScopedInstall(SetColModel, 0x4C4BC0);
    RH_ScopedInstall(Init2dEffects, 0x4C4C20);
    RH_ScopedInstall(DeleteCollisionModel, 0x4C4C40);
    RH_ScopedInstall(Get2dEffect, 0x4C4C70);
    RH_ScopedInstall(Add2dEffect, 0x4C4D20);
    RH_ScopedInstall(GetIsDrawLast, 0x5328C0);
    RH_ScopedInstall(HasBeenPreRendered, 0x5328B0);
    RH_ScopedInstall(HasComplexHierarchy, 0x4C4E00);
    RH_ScopedInstall(IsBackfaceCulled, 0x5328F0);
    RH_ScopedInstall(IsLod, 0x4C4A00);
    RH_ScopedInstall(IsRoad, 0x4C4DF0);
    RH_ScopedInstall(SetHasBeenPreRendered, 0x4C42F0);
    RH_ScopedInstall(SetIsLod, 0x4C4A10);
    RH_ScopedInstall(SetOwnsColModel, 0x5328D0);
    RH_ScopedInstall(IncreaseAlpha, 0x532900);

    RH_ScopedInstall(IsBreakableStatuePart, 0x59F090);
    RH_ScopedInstall(IsTagModel, 0x49CC20);
    // Hooking SwaysInWind function causes side effects
    RH_ScopedInstall(SwaysInWind, 0x4212C0, { .reversed = false });

    RH_ScopedInstall(SetBaseModelInfoFlags, 0x5B3AD0);
}


CBaseModelInfo::CBaseModelInfo() {
    m_nRefCount = 0;
    ClearTexDictionary();
}

// 0x4C4A80
CAtomicModelInfo* CBaseModelInfo::AsAtomicModelInfoPtr() {
    return nullptr;
}

// 0x4C4A90
CDamageAtomicModelInfo* CBaseModelInfo::AsDamageAtomicModelInfoPtr() {
    return nullptr;
}

// 0x4C4AA0
CLodAtomicModelInfo* CBaseModelInfo::AsLodAtomicModelInfoPtr() {
    return nullptr;
}

// 0x4C4AB0
CTimeInfo* CBaseModelInfo::GetTimeInfo() {
    return nullptr;
}

// 0x4C4B10
void CBaseModelInfo::Init() {
    m_nRefCount = 0;
    m_pColModel = nullptr;
    ClearTexDictionary();
    Init2dEffects();
    m_nObjectInfoIndex = -1;
    m_fDrawDistance = 2000.0F;
    m_pRwObject = nullptr;

    m_nFlags = 0;
    bIsBackfaceCulled = true;
    bIsLod = true;
}

// 0x4C4D50
void CBaseModelInfo::Shutdown() {
    DeleteRwObject();
    DeleteCollisionModel();

    bIsLod = true;
    m_nObjectInfoIndex = -1;
    ClearTexDictionary();
    Init2dEffects();
}

// 0x4C4AC0
void CBaseModelInfo::SetAnimFile(const char* filename) {}

// 0x4C4AD0
void CBaseModelInfo::ConvertAnimFileIndex() {
    // NOP
}

// 0x4C4AE0
int32 CBaseModelInfo::GetAnimFileIndex() {
    return -1;
}void CBaseModelInfo::SetTexDictionary(const char* txdName) {
    m_nTxdIndex = CTxdStore::FindOrAddTxdSlot(txdName);
}

void CBaseModelInfo::ClearTexDictionary() {
    m_nTxdIndex = -1;
}

void CBaseModelInfo::AddTexDictionaryRef() {
    CTxdStore::AddRef(m_nTxdIndex);
}

void CBaseModelInfo::RemoveTexDictionaryRef() {
    CTxdStore::RemoveRef(m_nTxdIndex);
}

void CBaseModelInfo::AddRef() {
    ++m_nRefCount;
    AddTexDictionaryRef();
}

void CBaseModelInfo::RemoveRef() {
    --m_nRefCount;
    RemoveTexDictionaryRef();
}

void CBaseModelInfo::SetColModel(CColModel* colModel, bool bIsLodModel) {
    m_pColModel = colModel;
    if (!bIsLodModel) {
        bIsLod = false;
        return;
    }

    bIsLod = true;
    auto timeInfo = GetTimeInfo();
    if (!timeInfo)
        return;

    if (timeInfo->GetOtherTimeModel() == -1)
        return;

    auto lodInfo = CModelInfo::GetModelInfo(timeInfo->GetOtherTimeModel());
    lodInfo->m_pColModel = colModel;
    lodInfo->bIsLod = false;
}

void CBaseModelInfo::Init2dEffects() {
    m_n2dEffectIndex = -1;
    m_n2dfxCount = 0;
}

void CBaseModelInfo::DeleteCollisionModel() {
    if (m_pColModel && bIsLod)
        delete m_pColModel;

    m_pColModel = nullptr;
}

C2dEffect* CBaseModelInfo::Get2dEffect(int32 index) {
    auto uiStoredEffectsCount = m_n2dfxCount;
    RpGeometry* geometry = nullptr;
    if (m_pRwObject) {
        if (GetRwModelType() == rpATOMIC) {
            geometry = RpAtomicGetGeometry(m_pRwAtomic);
        } else if (GetRwModelType() == rpCLUMP) {
            auto atomic = Get2DEffectAtomic(m_pRwClump);
            if (atomic) {
                geometry = RpAtomicGetGeometry(atomic);
            }
        }

        if (geometry)
            uiStoredEffectsCount -= RpGeometryGet2dFxCount(geometry);
    }

    if (index < uiStoredEffectsCount)
        return &CModelInfo::Get2dEffectStore()->GetItemAtIndex(index + m_n2dEffectIndex);
    else
        return RpGeometryGet2dFxAtIndex(geometry, index - uiStoredEffectsCount);
}

// 0x4C4D20
void CBaseModelInfo::Add2dEffect(C2dEffect* effect) {
    if (m_n2dEffectIndex >= 0)
        ++m_n2dfxCount;
    else {
        m_n2dEffectIndex = (effect - &CModelInfo::Get2dEffectStore()->m_aObjects[0]);
        m_n2dfxCount = 1;
    }
}

// 0x5B3AD0
void SetBaseModelInfoFlags(CBaseModelInfo* modelInfo, uint32 flags) {
    auto flagsStruct = sItemDefinitionFlags(flags);
    modelInfo->bDrawLast          = flagsStruct.bDrawLast;
    modelInfo->bAdditiveRender    = flagsStruct.bAdditive;
    modelInfo->bDontWriteZBuffer  = flagsStruct.bNoZBufferWrite;
    modelInfo->bDontCastShadowsOn = flagsStruct.bDontReceiveShadows;
    modelInfo->bIsBackfaceCulled  = !flagsStruct.bDisableBackfaceCulling;
}

void CBaseModelInfo::SetBaseModelInfoFlags(uint32 flags) {
    ::SetBaseModelInfoFlags(this, flags);
}
