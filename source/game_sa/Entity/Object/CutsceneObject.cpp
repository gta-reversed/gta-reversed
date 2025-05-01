#include "StdInc.h"

#include "CutsceneObject.h"

#include "RealTimeShadowManager.h"
#include "Shadows.h"
#include "CarFXRenderer.h"

char* (&CCutsceneObject::ms_sCutsceneVehNames)[NUM_CUTSCENE_VEHS] = *(char* (*)[NUM_CUTSCENE_VEHS])0x8D0F68;

void CCutsceneObject::InjectHooks() {
    RH_ScopedVirtualClass(CCutsceneObject, 0x868A60, 23);
    RH_ScopedCategory("Entity/Object");

    RH_ScopedVMTInstall(SetModelIndex, 0x5B1B20);
    RH_ScopedVMTInstall(SetupLighting, 0x553F40);
    RH_ScopedVMTInstall(RemoveLighting, 0x5533F0);
    RH_ScopedVMTInstall(ProcessControl, 0x5B1B90);
    RH_ScopedVMTInstall(PreRender, 0x5B1E00);
    RH_ScopedInstall(SetupCarPipeAtomicsForClump, 0x5B1AB0);
}

// 0x5B19D0
CCutsceneObject::CCutsceneObject() : CObject() {
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

// 0x5B1B20
void CCutsceneObject::SetModelIndex(unsigned index) {
    CEntity::SetModelIndex(index);
    if (RwObjectGetType(m_pRwObject) == rpCLUMP) {
        RpAnimBlendClumpInit(m_pRwClump);
        auto* animData = RpAnimBlendClumpGetData(m_pRwClump);
        animData->m_PedPosition = &m_vecMoveSpeed;
        animData->m_FrameDatas->HasZVelocity = true;
        CCutsceneObject::SetupCarPipeAtomicsForClump(index, m_pRwClump);
    }
    GetModelInfo()->m_nAlpha = 0xFF;
}

// 0x5B1B90
void CCutsceneObject::ProcessControl() {
    if (m_nAttachBone && m_pAttachmentObject && !m_bWasPostponed) {
        m_bWasPostponed = true;
        return;
    }

    CPhysical::ProcessControl(); // exactly CPhysical
    if (m_pAttachToFrame) {
        if (m_pAttachmentObject) {
            auto* hierarchy = GetAnimHierarchyFromClump(m_pAttachmentObject->m_pRwClump);
            auto* matArr = RpHAnimHierarchyGetMatrixArray(hierarchy);
            const auto boneMat = CMatrix(&matArr[m_nAttachBone], false);
            *static_cast<CMatrix*>(m_matrix) = boneMat;
        } else {
            auto* pLtm = RwFrameGetLTM(m_pAttachToFrame);
            const auto attachMat = CMatrix(pLtm, false);
            *static_cast<CMatrix*>(m_matrix) = attachMat;
        }
    } else {
        if (CTimer::GetTimeStep() >= 0.01F)
            m_vecMoveSpeed *= 1.0F / CTimer::GetTimeStep();
        else
            m_vecMoveSpeed *= 100.0F;

        if (physicalFlags.bDontApplySpeed || physicalFlags.bDisableMoveForce)
            ResetMoveSpeed();
        else {
            m_vForce += m_vecMoveSpeed * CTimer::GetTimeStep();
            m_matrix->SetTranslateOnly(m_vWorldPosition + m_vForce);
        }
    }
}

// 0x5B1E00
void CCutsceneObject::PreRender() {
    if (m_pAttachToFrame) {
        if (m_pAttachmentObject) {
            auto* hierarchy = GetAnimHierarchyFromClump(m_pAttachmentObject->m_pRwClump);
            auto* matArr = RpHAnimHierarchyGetMatrixArray(hierarchy);
            const auto boneMat = CMatrix(&matArr[m_nAttachBone], false);
            *static_cast<CMatrix*>(m_matrix) = boneMat;
        } else {
            auto* pLtm = RwFrameGetLTM(m_pAttachToFrame);
            const auto attachMat = CMatrix(pLtm, false);
            *static_cast<CMatrix*>(m_matrix) = attachMat;
        }

        if (RwObjectGetType(m_pRwObject) == rpCLUMP) {
            const auto* firstAtomic = GetFirstAtomic(m_pRwClump);
            if (firstAtomic) {
                if (RpSkinGeometryGetSkin(RpAtomicGetGeometry(firstAtomic))) {
                    auto* animData = RpAnimBlendClumpGetData(m_pRwClump);
                    auto* morphTarget = RpGeometryGetMorphTarget(RpAtomicGetGeometry(firstAtomic), 0);
                    auto* sphere = RpMorphTargetGetBoundingSphere(morphTarget);
                    sphere->center = animData->m_FrameDatas[0].BonePos;
                }
            }
        }
    }

    if (RwObjectGetType(m_pRwObject) == rpCLUMP)
        CEntity::UpdateRpHAnim();

    g_realTimeShadowMan.DoShadowThisFrame(this);
    if (!m_pShadowData) {
        CShadows::StoreShadowForPedObject(
            AsPed(),
            CTimeCycle::m_fShadowDisplacementX[CTimeCycle::m_CurrentStoredValue],
            CTimeCycle::m_fShadowDisplacementY[CTimeCycle::m_CurrentStoredValue],
            CTimeCycle::m_fShadowFrontX[CTimeCycle::m_CurrentStoredValue],
            CTimeCycle::m_fShadowFrontY[CTimeCycle::m_CurrentStoredValue],
            CTimeCycle::m_fShadowSideX[CTimeCycle::m_CurrentStoredValue],
            CTimeCycle::m_fShadowSideY[CTimeCycle::m_CurrentStoredValue]
        );
    }

    if (m_nModelIndex == MODEL_CSPLAY) {
        CPed::ShoulderBoneRotation(m_pRwClump);
        m_bDontUpdateHierarchy = true;
    }

    CVector vecPos = GetPosition();
    vecPos.z += 0.5F;
    const auto fHeight = vecPos.z - 5.0F;
    CColPoint colPoint{};
    CEntity* entity = nullptr;
    if (CWorld::ProcessVerticalLine(vecPos, fHeight, colPoint, entity, true)) {
        const auto fDayNight = colPoint.m_nLightingB.GetCurrentLighting();
        m_fContactSurfaceBrightness = lerp(m_fContactSurfaceBrightness, fDayNight, CTimer::GetTimeStep() / 10.0F);
    }
}

// 0x553F40
bool CCutsceneObject::SetupLighting() {
    ActivateDirectional();
    return CRenderer::SetupLightingForEntity(this);
}

// 0x5533F0
void CCutsceneObject::RemoveLighting(bool bRemove) {
    if (!physicalFlags.bRenderScorched)
        CPointLights::RemoveLightsAffectingObject();

    SetAmbientColours();
    DeActivateDirectional();
}

// 0x5B1AB0
void CCutsceneObject::SetupCarPipeAtomicsForClump(unsigned modelId, RpClump* clump) {
    static bool& bCarPipeAtomicsInitialized = *(bool*)0xBC4058;                              // TODO | STATICREF // = false;
    static uint32(&anHashKeys)[NUM_CUTSCENE_VEHS] = *(uint32(*)[NUM_CUTSCENE_VEHS])0xBC4040; // TODO | STATICREF

    if (!bCarPipeAtomicsInitialized) {
        bCarPipeAtomicsInitialized = true;
        for (auto i = 0; i < NUM_CUTSCENE_VEHS; ++i) {
            anHashKeys[i] = CKeyGen::GetUppercaseKey(ms_sCutsceneVehNames[i]);
        }
    }

    for (auto anHashKey : anHashKeys) {
        if (CModelInfo::GetModelInfo(modelId)->m_nKey == anHashKey) {
            RpClumpForAllAtomics(clump, SetupCarPipeAtomicCB, nullptr);
            return;
        }
    }
}

RpAtomic* CCutsceneObject::SetupCarPipeAtomicCB(RpAtomic* atomic, void* data) {
    UNUSED(data);
    CCarFXRenderer::CustomCarPipeAtomicSetup(atomic);
    return atomic;
}
