#include "StdInc.h"

void CHandObject::InjectHooks()
{
    RH_ScopedVirtualClass(CHandObject, 0x866EE0, 23);
    RH_ScopedCategory("Entity/Object");

    RH_ScopedVMTInstall(ProcessControl, 0x59EC40);
    RH_ScopedVMTInstall(PreRender, 0x59ECD0);
    RH_ScopedVMTInstall(Render, 0x59EE80);
}

CHandObject::CHandObject(int32 handModelIndex, CPed* ped, bool bLeftHand) : CObject()
{
    auto* animHierarchy = GetAnimHierarchyFromSkinClump(ped->m_pRwClump);
    m_pPed = ped;
    ped->UpdateRpHAnim();

    if (bLeftHand)
        m_nBoneIndex = RpHAnimIDGetIndex(animHierarchy, eBoneTag::BONE_L_FORE_ARM);
    else
        m_nBoneIndex = RpHAnimIDGetIndex(animHierarchy, eBoneTag::BONE_R_FORE_ARM);

    CEntity::SetModelIndex(handModelIndex);
    RpAnimBlendClumpInit(m_pRwClump);

    auto* pedModelInfo = CModelInfo::GetModelInfo(ped->m_nModelIndex);
    auto* txd = CTxdStore::ms_pTxdPool->GetAt(pedModelInfo->m_nTxdIndex);
    m_pTexture = RwTexDictionaryFindHashNamedTexture(txd->m_pRwDictionary, pedModelInfo->m_nKey);
    if (!m_pTexture)
        m_pTexture = GetFirstTexture(txd->m_pRwDictionary);

    m_nStatus = eEntityStatus::STATUS_SIMPLE;
    m_bUsesCollision = false;
    m_bLightObject = true;
    m_bStreamingDontDelete = true;
    m_nObjectType = eObjectType::OBJECT_TYPE_DECORATION;

    CObject::SetIsStatic(false);
    CModelInfo::GetModelInfo(handModelIndex)->m_nAlpha = 0xFF;
    m_bUpdatedMatricesArray = true;

}

// 0x59EC40
void CHandObject::ProcessControl()
{
    auto* animHierarchy = GetAnimHierarchyFromSkinClump(m_pPed->m_pRwClump);
    auto* matArr = RpHAnimHierarchyGetMatrixArray(animHierarchy);
    const auto boneMat = CMatrix(&matArr[m_nBoneIndex], false);
    *static_cast<CMatrix*>(m_matrix) = boneMat;

    m_bIsInSafePosition = true;
    CPhysical::RemoveAndAdd();
}

// 0x59ECD0
void CHandObject::PreRender()
{
    auto* animHierarchy = GetAnimHierarchyFromSkinClump(m_pPed->m_pRwClump);
    m_pPed->UpdateRpHAnim();
    m_pPed->m_bDontUpdateHierarchy = true;

    auto* matArr = RpHAnimHierarchyGetMatrixArray(animHierarchy);
    *static_cast<CMatrix*>(m_matrix) = CMatrix(&matArr[m_nBoneIndex], false);

    if (m_bUpdatedMatricesArray)
        m_bUpdatedMatricesArray = false;
    else
    {
        auto nBoneInd = m_nBoneIndex + 1;
        auto iStackCounter = 0;
        while (iStackCounter >= 0)
        {
            auto* boneMat = &matArr[nBoneInd];
            *RwMatrixGetAt(boneMat) = { 0.0F, 0.0F, 0.0F };
            *RwMatrixGetUp(boneMat) = { 0.0F, 0.0F, 0.0F };
            *RwMatrixGetRight(boneMat) = { 0.0F, 0.0F, 0.0F };

            const auto unFlags = static_cast<uint32>(animHierarchy->pNodeInfo[nBoneInd].flags);
            if ((unFlags & rpHANIMPUSHPARENTMATRIX) != 0)
                ++iStackCounter;
            else if ((unFlags & rpHANIMPOPPARENTMATRIX) != 0)
                --iStackCounter;

            ++nBoneInd;
        }
    }

    m_fContactSurfaceBrightness = m_pPed->m_fContactSurfaceBrightness;
    CEntity::UpdateRW();
    CEntity::UpdateRwFrame();
    CEntity::UpdateRpHAnim();
}

// 0x59EE80
void CHandObject::Render()
{
    auto* firstAtomic = GetFirstAtomic(m_pRwClump);
    RpMaterialSetTexture(RpGeometryGetMaterial(RpAtomicGetGeometry(firstAtomic), 0), m_pTexture);
    CObject::Render();
}