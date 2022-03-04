#include "StdInc.h"

char* (&CCutsceneObject::ms_sCutsceneVehNames)[NUM_CUTSCENE_VEHS] = *(char* (*)[NUM_CUTSCENE_VEHS])0x8D0F68;

void CCutsceneObject::InjectHooks()
{
    RH_ScopedClass(CCutsceneObject);
    RH_ScopedCategory("Entity/Object");

    RH_ScopedInstall(SetModelIndex_Reversed, 0x5B1B20);
    RH_ScopedInstall(SetupLighting_Reversed, 0x553F40);
    RH_ScopedInstall(RemoveLighting_Reversed, 0x5533F0);
    RH_ScopedInstall(ProcessControl_Reversed, 0x5B1B90);
    RH_ScopedInstall(PreRender_Reversed, 0x5B1E00);
    RH_ScopedInstall(SetupCarPipeAtomicsForClump, 0x5B1AB0);
}

CCutsceneObject::CCutsceneObject() : CObject()
{
    m_vWorldPosition.Set(0.0F, 0.0F, 0.0F);
    m_vForce.Set(0.0F, 0.0F, 0.0F);
    m_bUsesCollision = false;
    m_nStatus = eEntityStatus::STATUS_SIMPLE;
    m_nObjectType = eObjectType::OBJECT_TYPE_CUTSCENE;
    m_nAttachBone = 0;
    m_pAttachmentObject = nullptr;
    m_bLightObject = true;
    m_bStreamingDontDelete = true;

    CObject::SetIsStatic(false);
    m_bBackfaceCulled = false;
    m_fContactSurfaceBrightness = 0.5F;
}

void CCutsceneObject::SetModelIndex(unsigned index)
{
    CCutsceneObject::SetModelIndex_Reversed(index);
}
void CCutsceneObject::SetModelIndex_Reversed(unsigned index)
{
    CEntity::SetModelIndex(index);
    if (RwObjectGetType(m_pRwObject) == rpCLUMP)
    {
        RpAnimBlendClumpInit(m_pRwClump);
        auto* animData = RpClumpGetAnimBlendClumpData(m_pRwClump);
        animData->m_pvecPedPosition = &m_vecMoveSpeed;
        animData->m_pFrames->m_bUpdateSkinnedWith3dVelocityExtraction = true;
        CCutsceneObject::SetupCarPipeAtomicsForClump(index, m_pRwClump);
    }
    CModelInfo::GetModelInfo(index)->m_nAlpha = 0xFF;
}

void CCutsceneObject::ProcessControl()
{
    CCutsceneObject::ProcessControl_Reversed();
}
void CCutsceneObject::ProcessControl_Reversed()
{
    if (m_nAttachBone && m_pAttachmentObject && !m_bWasPostponed)
    {
        m_bWasPostponed = true;
        return;
    }

    CPhysical::ProcessControl();
    if (m_pAttachTo)
    {
        if (m_pAttachmentObject)
        {
            auto* hierarchy = GetAnimHierarchyFromClump(m_pAttachmentObject->m_pRwClump);
            auto* matArr = RpHAnimHierarchyGetMatrixArray(hierarchy);
            const auto boneMat = CMatrix(&matArr[m_nAttachBone], false);
            *static_cast<CMatrix*>(m_matrix) = boneMat;
        }
        else
        {
            auto* pLtm = RwFrameGetLTM(m_pAttachTo);
            const auto attachMat = CMatrix(pLtm, false);
            *static_cast<CMatrix*>(m_matrix) = attachMat;
        }
    }
    else
    {
        if (CTimer::GetTimeStep() >= 0.01F)
            m_vecMoveSpeed *= 1.0F / CTimer::GetTimeStep();
        else
            m_vecMoveSpeed *= 100.0F;

        if (physicalFlags.bDontApplySpeed || physicalFlags.bDisableMoveForce)
            m_vecMoveSpeed.Set(0.0F, 0.0F, 0.0F);
        else
        {
            m_vForce += m_vecMoveSpeed * CTimer::GetTimeStep();
            m_matrix->SetTranslateOnly(m_vWorldPosition + m_vForce);
        }
    }
}

void CCutsceneObject::PreRender()
{
    CCutsceneObject::PreRender_Reversed();
}
void CCutsceneObject::PreRender_Reversed()
{
    if (m_pAttachTo)
    {
        if (m_pAttachmentObject)
        {
            auto* hierarchy = GetAnimHierarchyFromClump(m_pAttachmentObject->m_pRwClump);
            auto* matArr = RpHAnimHierarchyGetMatrixArray(hierarchy);
            const auto boneMat = CMatrix(&matArr[m_nAttachBone], false);
            *static_cast<CMatrix*>(m_matrix) = boneMat;
        }
        else
        {
            auto* pLtm = RwFrameGetLTM(m_pAttachTo);
            const auto attachMat = CMatrix(pLtm, false);
            *static_cast<CMatrix*>(m_matrix) = attachMat;
        }

        if (RwObjectGetType(m_pRwObject) == rpCLUMP)
        {
            const auto* firstAtomic = GetFirstAtomic(m_pRwClump);
            if (firstAtomic)
            {
                if (RpSkinGeometryGetSkin(RpAtomicGetGeometry(firstAtomic)))
                {
                    auto* animData = RpClumpGetAnimBlendClumpData(m_pRwClump);
                    auto* morphTarget = RpGeometryGetMorphTarget(RpAtomicGetGeometry(firstAtomic), 0);
                    auto* sphere = RpMorphTargetGetBoundingSphere(morphTarget);
                    sphere->center = animData->m_pFrames->m_pIFrame->translation;
                }
            }
        }
    }

    if (RwObjectGetType(m_pRwObject) == rpCLUMP)
        CEntity::UpdateRpHAnim();

    g_realTimeShadowMan.DoShadowThisFrame(this);
    if (!m_pShadowData)
    {
        CShadows::StoreShadowForPedObject(
            this,
            CTimeCycle::m_fShadowDisplacementX[CTimeCycle::m_CurrentStoredValue],
            CTimeCycle::m_fShadowDisplacementY[CTimeCycle::m_CurrentStoredValue],
            CTimeCycle::m_fShadowFrontX[CTimeCycle::m_CurrentStoredValue],
            CTimeCycle::m_fShadowFrontY[CTimeCycle::m_CurrentStoredValue],
            CTimeCycle::m_fShadowSideX[CTimeCycle::m_CurrentStoredValue],
            CTimeCycle::m_fShadowSideY[CTimeCycle::m_CurrentStoredValue]
        );
    }

    if (m_nModelIndex == MODEL_CSPLAY)
    {
        CPed::ShoulderBoneRotation(m_pRwClump);
        m_bDontUpdateHierarchy = true;
    }

    CVector vecPos = GetPosition();
    vecPos.z += 0.5F;
    const auto fHeight = vecPos.z - 5.0F;
    CColPoint colPoint;
    CEntity* entity = nullptr;
    if (CWorld::ProcessVerticalLine(vecPos, fHeight, colPoint, entity, true, false, false, false, false, false, nullptr)) {
        const auto fDayNight = colPoint.m_nLightingB.GetCurrentLighting();
        m_fContactSurfaceBrightness = lerp(m_fContactSurfaceBrightness, fDayNight, CTimer::GetTimeStep() / 10.0F);
    }
}

bool CCutsceneObject::SetupLighting()
{
    return CCutsceneObject::SetupLighting_Reversed();
}
bool CCutsceneObject::SetupLighting_Reversed()
{
    ActivateDirectional();
    return CRenderer::SetupLightingForEntity(this);
}

void CCutsceneObject::RemoveLighting(bool bRemove)
{
    return CCutsceneObject::RemoveLighting_Reversed(bRemove);
}
void CCutsceneObject::RemoveLighting_Reversed(bool bRemove)
{
    if (!physicalFlags.bDestroyed)
        CPointLights::RemoveLightsAffectingObject();

    SetAmbientColours();
    DeActivateDirectional();
}

void CCutsceneObject::SetupCarPipeAtomicsForClump(unsigned modelId, RpClump* clump)
{
    static bool& bCarPipeAtomicsInitialized = *(bool*)0xBC4058; // TODO | STATICREF // = false;
    static uint32(&anHashKeys)[NUM_CUTSCENE_VEHS] = *(uint32(*)[NUM_CUTSCENE_VEHS])0xBC4040; // TODO | STATICREF

    if (!bCarPipeAtomicsInitialized)
    {
        bCarPipeAtomicsInitialized = true;
        for (auto i = 0; i < NUM_CUTSCENE_VEHS; ++i)
            anHashKeys[i] = CKeyGen::GetUppercaseKey(ms_sCutsceneVehNames[i]);
    }

    for (auto anHashKey : anHashKeys)
    {
        if (CModelInfo::GetModelInfo(modelId)->m_nKey == anHashKey)
        {
            RpClumpForAllAtomics(clump, SetupCarPipeAtomicCB, nullptr);
            return;
        }
    }
}

RpAtomic* CCutsceneObject::SetupCarPipeAtomicCB(RpAtomic* atomic, void* data)
{
    CCarFXRenderer::CustomCarPipeAtomicSetup(atomic);
    return atomic;
}
