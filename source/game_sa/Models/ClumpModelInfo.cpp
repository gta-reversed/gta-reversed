/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "ClumpModelInfo.h"
#include "CustomBuildingRenderer.h"
#include "CarFXRenderer.h"

void CClumpModelInfo::InjectHooks()
{
    RH_ScopedVirtualClass(CClumpModelInfo, 0x85BD30, 17);
    RH_ScopedCategory("Models");

    RH_ScopedVMTInstall(GetModelType, 0x4C5720);
    RH_ScopedVMTInstall(Init, 0x4C4E40);
    RH_ScopedVMTInstall(Shutdown, 0x4C4E60);
    RH_ScopedVMTInstall(DeleteRwObject, 0x4C4E70);
    RH_ScopedVMTInstall(GetRwModelType, 0x4C5730);
    RH_ScopedVMTOverloadedInstall(CreateInstance, "void", 0x4C5140, RwObject * (CClumpModelInfo::*)());
    RH_ScopedVMTOverloadedInstall(CreateInstance, "mat", 0x4C5110, RwObject * (CClumpModelInfo::*)(RwMatrix*));
    RH_ScopedVMTInstall(SetAnimFile, 0x4C5200);
    RH_ScopedVMTInstall(ConvertAnimFileIndex, 0x4C5250);
    RH_ScopedVMTInstall(GetAnimFileIndex, 0x4C5740);
    RH_ScopedVMTInstall(GetBoundingBox, 0x4C5710);
    RH_ScopedVMTInstall(SetClump, 0x4C4F70);
    RH_ScopedInstall(SetFrameIds, 0x4C5460);
    RH_ScopedInstall(SetHierarchyForSkinAtomic, 0x4C4EF0);
    RH_ScopedInstall(AtomicSetupLightingCB, 0x4C4F30);
    RH_ScopedInstall(SetAtomicRendererCB, 0x4C5280);
    RH_ScopedInstall(FindFrameFromNameCB, 0x4C52A0);
    RH_ScopedInstall(FindFrameFromNameWithoutIdCB, 0x4C52F0);
    RH_ScopedInstall(FindFrameFromIdCB, 0x4C5350);
    RH_ScopedInstall(FillFrameArrayCB, 0x4C5390);
    RH_ScopedInstall(GetFrameFromId, 0x4C53C0);
    RH_ScopedInstall(GetFrameFromName, 0x4C5400);
    RH_ScopedInstall(FillFrameArray, 0x4C5440);

    RH_ScopedInstall(SetClumpModelInfoFlags, 0x5B3C30);
}

// 0x4C5720
ModelInfoType CClumpModelInfo::GetModelType()
{
    return ModelInfoType::MODEL_INFO_CLUMP;
}

// 0x4C4E40
void CClumpModelInfo::Init()
{
    CBaseModelInfo::Init();
    m_nAnimFileIndex = -1;
}

// 0x4C4E60
void CClumpModelInfo::Shutdown()
{
    CBaseModelInfo::Shutdown();
}

// 0x4C4E70
void CClumpModelInfo::DeleteRwObject()
{
    if (!m_pRwObject)
        return;

    auto atomic = Get2DEffectAtomic(m_pRwClump);
    if (atomic)
        m_n2dfxCount -= RpGeometryGet2dFxCount(RpAtomicGetGeometry(atomic));

    RpClumpDestroy(m_pRwClump);
    m_pRwObject = nullptr;

    CBaseModelInfo::RemoveTexDictionaryRef();
    auto iAnimIndex = GetAnimFileIndex();
    if (iAnimIndex != -1)
        CAnimManager::RemoveAnimBlockRef(iAnimIndex);

    if (bOwnsCollisionModel)
        CBaseModelInfo::DeleteCollisionModel();
}

// 0x4C5730
uint32 CClumpModelInfo::GetRwModelType()
{
    return rpCLUMP;
}

RwObject* CClumpModelInfo::CreateInstance()
{
    if (!m_pRwObject)
        return nullptr;

    CBaseModelInfo::AddRef();
    auto clonedClump = RpClumpClone(m_pRwClump);
    auto atomic = GetFirstAtomic(clonedClump);
    if (atomic && RpSkinGeometryGetSkin(RpAtomicGetGeometry(atomic)) && !bHasComplexHierarchy) {
        auto hierarchy = GetAnimHierarchyFromClump(clonedClump);
        RpClumpForAllAtomics(clonedClump, SetHierarchyForSkinAtomic, hierarchy);
        auto anim = RpAnimBlendCreateAnimationForHierarchy(hierarchy);
        RtAnimInterpolatorSetCurrentAnim(hierarchy->currentAnim, anim);
        hierarchy->flags = rpHANIMHIERARCHYUPDATEMODELLINGMATRICES | rpHANIMHIERARCHYUPDATELTMS;
    }

    if (bHasAnimBlend) {
        RpAnimBlendClumpInit(clonedClump);
        if (const auto anim = CAnimManager::GetAnimation(m_nKey, &CAnimManager::GetAnimBlocks()[m_nAnimFileIndex])) {
            CAnimManager::BlendAnimation(clonedClump, anim, ANIMATION_IS_LOOPED, 1.0F);
        }
    }

    CBaseModelInfo::RemoveRef();
    return reinterpret_cast<RwObject*>(clonedClump);
}

RwObject* CClumpModelInfo::CreateInstance(RwMatrix* matrix)
{
    if (!m_pRwObject)
        return nullptr;

    auto clump = CreateInstance();
    memcpy(RpClumpGetFrame(clump), matrix, sizeof(RwMatrix));
    return clump;
}

// 0x4C5200
void CClumpModelInfo::SetAnimFile(const char* filename)
{
    if (!strcmp(filename, "null"))
        return;

    const auto size = strlen(filename) + 1;
    auto name = new char[size];
    strcpy_s(name, size, filename);
    m_animFileName = name;
}

// 0x4C5250
void CClumpModelInfo::ConvertAnimFileIndex()
{
    if (m_nAnimFileIndex == -1)
        return;

    auto iIndex = CAnimManager::GetAnimationBlockIndex(m_animFileName);
    delete[] m_animFileName;
    m_nAnimFileIndex = iIndex;
}

// 0x4C5740
int32 CClumpModelInfo::GetAnimFileIndex()
{
    return m_nAnimFileIndex;
}

// 0x4C5710
CBox* CClumpModelInfo::GetBoundingBox()
{
    return &GetColModel()->GetBoundingBox();
}

// 0x4C4F70
void CClumpModelInfo::SetClump(RpClump* clump)
{
    if (m_pRwObject) {
        if (auto atomic = Get2DEffectAtomic(m_pRwClump))
            m_n2dfxCount -= RpGeometryGet2dFxCount(RpAtomicGetGeometry(atomic));
    }

    m_pRwClump = clump;
    if (m_pRwClump) {
        if (auto atomic = Get2DEffectAtomic(m_pRwClump))
            m_n2dfxCount += RpGeometryGet2dFxCount(RpAtomicGetGeometry(atomic));
    }

    CVisibilityPlugins::SetClumpModelInfo(m_pRwClump, this);
    CBaseModelInfo::AddTexDictionaryRef();

    auto iAnimIndex = GetAnimFileIndex();
    if (iAnimIndex != -1)
        CAnimManager::AddAnimBlockRef(iAnimIndex);

    RpClumpForAllAtomics(m_pRwClump, AtomicSetupLightingCB, this);
    auto firstAtomic = GetFirstAtomic(m_pRwClump);
    if (firstAtomic && RpSkinGeometryGetSkin(RpAtomicGetGeometry(firstAtomic))) {
        if (bHasComplexHierarchy) {
            RpClumpForAllAtomics(m_pRwClump, SetHierarchyForSkinAtomic, nullptr);
        }
        else {
            auto sphere = RpMorphTargetGetBoundingSphere(RpGeometryGetMorphTarget(RpAtomicGetGeometry(firstAtomic), 0));
            sphere->radius *= 1.2F;

            auto hierarchy = GetAnimHierarchyFromClump(m_pRwClump);
            RpClumpForAllAtomics(m_pRwClump, SetHierarchyForSkinAtomic, hierarchy);

            auto geometry = RpAtomicGetGeometry(firstAtomic);
            auto skin = RpSkinGeometryGetSkin(geometry);
            for (int32 i = 0; i < RpGeometryGetNumVertices(geometry); ++i) {
                auto& weight = RpSkinGetVertexBoneWeights(skin)[i]; // todoL: originally RpSkinGetVertexBoneWeights returns const obj*
                auto fRecip = 1.0F / (weight.w0 + weight.w1 + weight.w2 + weight.w3);
                weight.w0 *= fRecip;
                weight.w1 *= fRecip;
                weight.w2 *= fRecip;
                weight.w3 *= fRecip;
            }

            hierarchy->flags = rpHANIMHIERARCHYUPDATEMODELLINGMATRICES | rpHANIMHIERARCHYUPDATELTMS;
        }
    }
}

// 0x4C5460
void CClumpModelInfo::SetFrameIds(RwObjectNameIdAssocation* data) {
    if (!data->m_pName)
        return;

    auto curComponent = data;
    while (curComponent->m_pName) {
        if ((curComponent->m_dwFlags & 1) == 0) {
            auto searchInfo = tCompSearchStructByName(curComponent->m_pName, nullptr);
            RwFrameForAllChildren(RpClumpGetFrame(m_pRwClump), FindFrameFromNameWithoutIdCB, &searchInfo);
            if (searchInfo.m_pFrame)
                CVisibilityPlugins::SetFrameHierarchyId(searchInfo.m_pFrame, curComponent->m_dwHierarchyId);
        }

        curComponent++;
    }
}

void CClumpModelInfo::SetClumpModelInfoFlags(uint32 flags) {
    ::SetClumpModelInfoFlags(this, flags);
}

RpAtomic* CClumpModelInfo::SetAtomicRendererCB(RpAtomic* atomic, void* renderFunc)
{
    CVisibilityPlugins::SetAtomicRenderCallback(atomic, reinterpret_cast<RpAtomicCallBackRender>(renderFunc));
    return atomic;
}

RpAtomic* CClumpModelInfo::AtomicSetupLightingCB(RpAtomic* atomic, void* data)
{
    if (CCustomBuildingRenderer::IsCBPCPipelineAttached(atomic))
    {
        CCustomBuildingRenderer::AtomicSetup(atomic);
    }
    else if (CCarFXRenderer::IsCCPCPipelineAttached(atomic))
    {
        CCarFXRenderer::CustomCarPipeAtomicSetup(atomic);
    }

    return atomic;
}

RpAtomic* CClumpModelInfo::SetHierarchyForSkinAtomic(RpAtomic* atomic, void* data)
{
    if (data) {
        RpSkinAtomicSetHAnimHierarchy(atomic, reinterpret_cast<RpHAnimHierarchy*>(data));
        return nullptr;
    }

    auto hierarchy = GetAnimHierarchyFromFrame(RpAtomicGetFrame(atomic));
    RpSkinAtomicSetHAnimHierarchy(atomic, hierarchy);
    return atomic;
}

RwFrame* CClumpModelInfo::FindFrameFromNameCB(RwFrame* frame, void* searchData)
{
    auto searchInfo = reinterpret_cast<tCompSearchStructByName*>(searchData);
    auto name = GetFrameNodeName(frame);
    if (!strcmp(searchInfo->m_pName, name)) {
        searchInfo->m_pFrame = frame;
        return nullptr;
    }

    RwFrameForAllChildren(frame, FindFrameFromNameCB, searchData);
    if (searchInfo->m_pFrame)
        return nullptr;
    else
        return frame;
}

RwFrame* CClumpModelInfo::FindFrameFromNameWithoutIdCB(RwFrame* frame, void* searchData)
{
    auto searchInfo = reinterpret_cast<tCompSearchStructByName*>(searchData);
    auto name = GetFrameNodeName(frame);
    if (!CVisibilityPlugins::GetFrameHierarchyId(frame) && !strcmp(searchInfo->m_pName, name)) {
        searchInfo->m_pFrame = frame;
        return nullptr;
    }
    RwFrameForAllChildren(frame, FindFrameFromNameWithoutIdCB, searchData);
    if (searchInfo->m_pFrame)
        return nullptr;
    else
        return frame;
}

RwFrame* CClumpModelInfo::FindFrameFromIdCB(RwFrame* frame, void* searchData)
{
    auto searchInfo = reinterpret_cast<tCompSearchStructById*>(searchData);
    if (searchInfo->m_nId == CVisibilityPlugins::GetFrameHierarchyId(frame)) {
        searchInfo->m_pFrame = frame;
        return nullptr;
    }

    RwFrameForAllChildren(frame, FindFrameFromIdCB, searchData);
    if (searchInfo->m_pFrame)
        return nullptr;
    else
        return frame;
}

RwFrame* CClumpModelInfo::FillFrameArrayCB(RwFrame* frame, void* data)
{
    auto iId = CVisibilityPlugins::GetFrameHierarchyId(frame);
    if (iId > 0)
        reinterpret_cast<RwFrame**>(data)[iId] = frame;

    RwFrameForAllChildren(frame, FillFrameArrayCB, data);
    return frame;
}

RwFrame* CClumpModelInfo::GetFrameFromId(RpClump* clump, int32 id)
{
    auto searchInfo = tCompSearchStructById(id, nullptr);
    RwFrameForAllChildren(RpClumpGetFrame(clump), FindFrameFromIdCB, &searchInfo);
    return searchInfo.m_pFrame;
}

RwFrame* CClumpModelInfo::GetFrameFromName(RpClump* clump, const char* name)
{
    auto searchInfo = tCompSearchStructByName(name, nullptr);
    RwFrameForAllChildren(RpClumpGetFrame(clump), FindFrameFromNameCB, &searchInfo);
    return searchInfo.m_pFrame;
}

void CClumpModelInfo::FillFrameArray(RpClump* clump, RwFrame** frames)
{
    RwFrameForAllChildren(RpClumpGetFrame(clump), FillFrameArrayCB, frames);
}

void SetClumpModelInfoFlags(CClumpModelInfo* modelInfo, uint32 dwFlags)
{
    SetBaseModelInfoFlags(modelInfo, dwFlags);

    auto flagsStruct = sItemDefinitionFlags(dwFlags);
    modelInfo->bAnimSomething = flagsStruct.bAnimSomething;
}
