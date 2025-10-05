/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Entity.h"

#include "PointLights.h"
#include "Escalators.h"
#include "CustomBuildingDNPipeline.h"
#include "ActiveOccluder.h"
#include "Occlusion.h"
#include "MotionBlurStreaks.h"
#include "TagManager.h"
#include "WindModifiers.h"
#include "EntryExitManager.h"
#include "TrafficLights.h"
#include "Glass.h"
#include "TheScripts.h"
#include "Shadows.h"
#include "CustomBuildingRenderer.h"

void CEntity::InjectHooks() {
    RH_ScopedVirtualClass(CEntity, 0x863928, 22);
    RH_ScopedCategory("Entity");

    // clang moment: RH_ScopedVirtualOverloadedInstall(Add, "void", 0x533020, void(CEntity::*)());
    // clang moment: RH_ScopedVirtualOverloadedInstall(Add, "rect", 0x5347D0, void(CEntity::*)(const CRect&));
    RH_ScopedVMTInstall(Remove, 0x534AE0);
    RH_ScopedVMTInstall(SetIsStatic, 0x403E20);
    RH_ScopedVMTInstall(SetModelIndex, 0x532AE0);
    RH_ScopedVMTInstall(SetModelIndexNoCreate, 0x533700);
    RH_ScopedVMTInstall(CreateRwObject, 0x533D30);
    RH_ScopedVMTInstall(DeleteRwObject, 0x534030);
    RH_ScopedVMTInstall(GetBoundRect, 0x534120);
    RH_ScopedVMTInstall(ProcessControl, 0x403E40);
    RH_ScopedVMTInstall(ProcessCollision, 0x403E50);
    RH_ScopedVMTInstall(ProcessShift, 0x403E60);
    RH_ScopedVMTInstall(TestCollision, 0x403E70);
    RH_ScopedVMTInstall(Teleport, 0x403E80);
    RH_ScopedVMTInstall(SpecialEntityPreCollisionStuff, 0x403E90);
    RH_ScopedVMTInstall(SpecialEntityCalcCollisionSteps, 0x403EA0);
    RH_ScopedVMTInstall(PreRender, 0x535FA0);
    RH_ScopedVMTInstall(Render, 0x534310);
    RH_ScopedVMTInstall(SetupLighting, 0x553DC0);
    RH_ScopedVMTInstall(RemoveLighting, 0x553370);
    RH_ScopedVMTInstall(FlagToDestroyWhenNextProcessed, 0x403EB0);
    RH_ScopedInstall(UpdateRwFrame, 0x532B00);
    RH_ScopedInstall(UpdateRpHAnim, 0x532B20);
    RH_ScopedInstall(HasPreRenderEffects, 0x532B70);
    RH_ScopedInstall(DoesNotCollideWithFlyers, 0x532D40);
    RH_ScopedInstall(BuildWindSockMatrix, 0x532DB0);
    RH_ScopedInstall(LivesInThisNonOverlapSector, 0x533050);
    RH_ScopedInstall(SetupBigBuilding, 0x533150);
    RH_ScopedInstall(ModifyMatrixForCrane, 0x533170);
    RH_ScopedInstall(PreRenderForGlassWindow, 0x533240);
    RH_ScopedInstall(SetRwObjectAlpha, 0x5332C0);
    RH_ScopedInstall(FindTriggerPointCoors, 0x533380);
    RH_ScopedInstall(GetRandom2dEffect, 0x533410);
    RH_ScopedOverloadedInstall(TransformFromObjectSpace, "ref", 0x5334F0, CVector(CEntity::*)(const CVector&) const);
    RH_ScopedOverloadedInstall(TransformFromObjectSpace, "ptr", 0x533560, CVector*(CEntity::*)(CVector&, const CVector&) const);
    RH_ScopedInstall(CreateEffects, 0x533790);
    RH_ScopedInstall(DestroyEffects, 0x533BF0);
    RH_ScopedInstall(AttachToRwObject, 0x533ED0);
    RH_ScopedInstall(DetachFromRwObject, 0x533FB0);
    RH_ScopedOverloadedInstall(GetBoundCentre, "ptr", 0x534250, CVector*(CEntity::*)(CVector*) const);
    RH_ScopedOverloadedInstall(GetBoundCentre, "ref", 0x534290, void(CEntity::*)(CVector&) const);
    RH_ScopedInstall(RenderEffects, 0x5342B0);
    RH_ScopedOverloadedInstall(GetIsTouching, "ent", 0x5343F0, bool(CEntity::*)(CEntity*) const);
    RH_ScopedOverloadedInstall(GetIsTouching, "vec", 0x5344B0, bool(CEntity::*)(const CVector&, float) const);
    RH_ScopedInstall(GetIsOnScreen, 0x534540);
    RH_ScopedInstall(GetIsBoundingBoxOnScreen, 0x5345D0);
    RH_ScopedInstall(ModifyMatrixForTreeInWind, 0x534E90);
    RH_ScopedInstall(ModifyMatrixForBannerInWind, 0x535040);
    RH_ScopedInstall(GetColModel, 0x535300);
    RH_ScopedInstall(CalculateBBProjection, 0x535340);
    RH_ScopedInstall(UpdateAnim, 0x535F00);
    RH_ScopedInstall(IsVisible, 0x536BC0);
    RH_ScopedInstall(GetDistanceFromCentreOfMassToBaseOfModel, 0x536BE0);
    RH_ScopedOverloadedInstall(CleanUpOldReference, "", 0x571A00, void(CEntity::*)(CEntity**));
    RH_ScopedInstall(ResolveReferences, 0x571A40);
    RH_ScopedInstall(PruneReferences, 0x571A90);
    RH_ScopedOverloadedInstall(RegisterReference, "", 0x571B70, void(CEntity::*)(CEntity**));
    RH_ScopedInstall(ProcessLightsForEntity, 0x6FC7A0);
    RH_ScopedInstall(RemoveEscalatorsForEntity, 0x717900);
    RH_ScopedInstall(IsEntityOccluded, 0x71FAE0);
    RH_ScopedInstall(GetModellingMatrix, 0x46A2D0);
    RH_ScopedInstall(UpdateRW, 0x446F90);
    RH_ScopedInstall(SetAtomicAlphaCB, 0x533290);
    RH_ScopedInstall(SetMaterialAlphaCB, 0x533280);
    RH_ScopedGlobalInstall(MaterialUpdateUVAnimCB, 0x532D70);
    RH_ScopedGlobalInstall(IsEntityPointerValid, 0x533310);

    RH_ScopedGlobalInstall(IsGlassModel, 0x46A760);
}

CEntity::CEntity() : CPlaceable() {
    SetStatus(STATUS_ABANDONED);
    SetType(ENTITY_TYPE_NOTHING);

    m_nFlags = 0;
    SetIsVisible(true);
    SetIsBackfaceCulled(true);

    SetScanCode(0);
    SetAreaCode(AREA_CODE_NORMAL_WORLD);
    m_nModelIndex = MODEL_INVALID;
    m_pRwObject = nullptr;
    SetIplIndex(0);
    m_nRandomSeed = CGeneral::GetRandomNumber();
    m_pReferences = nullptr;
    m_pStreamingLink = nullptr;
    m_nNumLodChildren = 0;
    ResetLodChildrenRendered();
    SetLod(nullptr);
}

CEntity::~CEntity() {
    if (GetLod()) {
        GetLod()->RemoveLodChildren();
    }

    CEntity::DeleteRwObject();
    CEntity::ResolveReferences();
}

void CEntity::Add() {
    Add(GetBoundRect());
}

void CEntity::Add(const CRect& rect) {
    CRect usedRect = rect;
    if (usedRect.left < -3000.0F) {
        usedRect.left = -3000.0F;
    }
    if (usedRect.right >= 3000.0F) {
        usedRect.right = 2999.0F;
    }
    if (usedRect.bottom < -3000.0F) {
        usedRect.bottom = -3000.0F;
    }
    if (usedRect.top >= 3000.0F) {
        usedRect.top = 2999.0F;
    }

    if (m_bIsBIGBuilding) {
        CWorld::IterateLodSectorsOverlappedByRect({ usedRect }, [&](int32 x, int32 y) {
            auto& pLodListEntry = CWorld::GetLodPtrList(x, y);
            pLodListEntry.AddItem(this);
            return true;
        });
    } else {
        CWorld::IterateSectorsOverlappedByRect({ usedRect }, [&](int32 x, int32 y) {
            auto* const s = GetSector(x, y);
            auto* const rs = GetRepeatSector(x, y);
            const auto  ProcessAddItem = [this]<typename PtrListType>(PtrListType& list) {
                list.AddItem(static_cast<typename PtrListType::ItemType>(this)); // TODO: notsa::cast
            };
            switch (GetType()) {
            case ENTITY_TYPE_DUMMY:    ProcessAddItem(s->m_dummies); break;
            case ENTITY_TYPE_VEHICLE:  ProcessAddItem(rs->Vehicles); break;
            case ENTITY_TYPE_PED:      ProcessAddItem(rs->Peds); break;
            case ENTITY_TYPE_OBJECT:   ProcessAddItem(rs->Objects); break;
            case ENTITY_TYPE_BUILDING: ProcessAddItem(s->m_buildings); break;
            }
            return true;
        });
    }
}

// 0x534AE0
void CEntity::Remove() {
    auto usedRect = GetBoundRect();
    if (usedRect.left < -3000.0F) {
        usedRect.left = -3000.0F;
    }
    if (usedRect.right >= 3000.0F) {
        usedRect.right = 2999.0F;
    }
    if (usedRect.bottom < -3000.0F) {
        usedRect.bottom = -3000.0F;
    }
    if (usedRect.top >= 3000.0F) {
        usedRect.top = 2999.0F;
    }

    if (m_bIsBIGBuilding) {
        CWorld::IterateLodSectorsOverlappedByRect({ usedRect }, [&](int32 x, int32 y) {
            auto& list = CWorld::GetLodPtrList(x, y);
            list.DeleteItem(this);
            return true;
        });
    } else {
        CWorld::IterateSectorsOverlappedByRect({ usedRect }, [&](int32 x, int32 y) {
            auto* const s = GetSector(x, y);
            auto* const rs = GetRepeatSector(x, y);
            const auto  ProcessDeleteItem = [this]<typename PtrListType>(PtrListType& list) {
                list.DeleteItem(static_cast<typename PtrListType::ItemType>(this)); // TODO: notsa::cast
            };
            switch (GetType()) {
            case ENTITY_TYPE_DUMMY:    ProcessDeleteItem(s->m_dummies); break;
            case ENTITY_TYPE_VEHICLE:  ProcessDeleteItem(rs->Vehicles); break;
            case ENTITY_TYPE_PED:      ProcessDeleteItem(rs->Peds); break;
            case ENTITY_TYPE_OBJECT:   ProcessDeleteItem(rs->Objects); break;
            case ENTITY_TYPE_BUILDING: ProcessDeleteItem(s->m_buildings); break;
            }
            return true;
        });
    }
}

// 0x532AE0
void CEntity::SetModelIndex(uint32 index) {
    CEntity::SetModelIndexNoCreate(index);
    CEntity::CreateRwObject();
}

// 0x533700
void CEntity::SetModelIndexNoCreate(uint32 index) {
    auto mi = CModelInfo::GetModelInfo(index);
    m_nModelIndex = index;

    m_bHasPreRenderEffects = HasPreRenderEffects();

    if (mi->GetIsDrawLast()) {
        m_bDrawLast = true;
    }

    if (!mi->IsBackfaceCulled()) {
        SetIsBackfaceCulled(false);
    }

    auto ami = mi->AsAtomicModelInfoPtr();
    if (ami && !ami->bTagDisabled && ami->IsTagModel()) {
        CTagManager::AddTag(this);
    }
}

// 0x533D30
void CEntity::CreateRwObject() {
    if (!GetIsVisible()) {
        return;
    }

    auto* mi = CModelInfo::GetModelInfo(GetModelIndex());

    // Create instance based on damage state
    if (m_bRenderDamaged) {
        CDamageAtomicModelInfo::ms_bCreateDamagedVersion = true;
        m_pRwObject = mi->CreateInstance();
        CDamageAtomicModelInfo::ms_bCreateDamagedVersion = false;
    } else {
        m_pRwObject = mi->CreateInstance();
    }

    if (!GetRwObject()) {
        return;
    }

    // Update building counter and RenderWare state
    if (GetIsTypeBuilding()) {
        gBuildings++;
    }
    UpdateRW();

    // Handle different RenderWare object types
    const auto objectType = RwObjectGetType(GetRwObject());

    if (objectType == rpATOMIC) {
        if (CTagManager::IsTag(this)) {
            CTagManager::ResetAlpha(this);
        }
        CCustomBuildingDNPipeline::PreRenderUpdate(m_pRwAtomic, true);
    } else if (objectType == rpCLUMP && mi->bIsRoad) {
        // Add to moving list for road objects
        if (GetIsTypeObject()) {
            auto* obj = AsObject();
            if (!obj->m_pMovingList) {
                obj->AddToMovingList();
            }
            obj->SetIsStatic(false);
        } else {
            CWorld::ms_listMovingEntityPtrs.AddItem(AsPhysical());
        }

        // Synchronize LOD animation timing
        if (GetLod() && GetLod()->GetRwObject() && RwObjectGetType(GetLod()->GetRwObject()) == rpCLUMP) {
            if (auto* pLodAssoc = RpAnimBlendClumpGetFirstAssociation(GetLod()->m_pRwClump)) {
                if (auto* pAssoc = RpAnimBlendClumpGetFirstAssociation(m_pRwClump)) {
                    pAssoc->SetCurrentTime(pLodAssoc->m_CurrentTime);
                }
            }
        }
    }

    // Finalize object setup
    mi->AddRef();
    m_pStreamingLink = CStreaming::AddEntity(this);
    CreateEffects();

    // Determine lighting requirement
    auto* usedAtomic = objectType == rpATOMIC ? m_pRwAtomic : GetFirstAtomic(m_pRwClump);

    if (!CCustomBuildingRenderer::IsCBPCPipelineAttached(usedAtomic)) {
        m_bLightObject = true;
    }
}

// 0x534030
void CEntity::DeleteRwObject() {
    if (!GetRwObject()) {
        return;
    }

    const auto rwObjectType = RwObjectGetType(GetRwObject());

    if (rwObjectType == rpATOMIC) {
        if (auto* frame = RpAtomicGetFrame(m_pRwAtomic)) {
            RpAtomicDestroy(m_pRwAtomic);
            RwFrameDestroy(frame);
        }
    } else if (rwObjectType == rpCLUMP) {
#ifdef SA_SKINNED_PEDS
        if (IsClumpSkinned(m_pRwClump)) {
            RpClumpForAllAtomics(m_pRwClump, AtomicRemoveAnimFromSkinCB, nullptr);
        }
#endif
        RpClumpDestroy(m_pRwClump);
    }

    m_pRwObject = nullptr;
    auto* mi = CModelInfo::GetModelInfo(GetModelIndex());
    mi->RemoveRef();
    CStreaming::RemoveEntity(std::exchange(m_pStreamingLink, nullptr));

    if (GetIsTypeBuilding()) {
        gBuildings--;
    }

    if (mi->GetModelType() == MODEL_INFO_CLUMP && mi->IsRoad() && !GetIsTypeObject()) {
        CWorld::ms_listMovingEntityPtrs.DeleteItem(AsPhysical());
    }

    CEntity::DestroyEffects();
    CEntity::RemoveEscalatorsForEntity();
}

// 0x534120
CRect CEntity::GetBoundRect() const {
    auto* colModel = CModelInfo::GetModelInfo(GetModelIndex())->GetColModel();
    auto [boundBoxMin, boundBoxMax] = std::pair{ colModel->GetBoundingBox().m_vecMin, colModel->GetBoundingBox().m_vecMax };

    CRect rect;
    CVector result;

    // Processing the first two corners
    TransformFromObjectSpace(result, boundBoxMin);
    rect.StretchToPoint(result.x, result.y);

    TransformFromObjectSpace(result, boundBoxMax);
    rect.StretchToPoint(result.x, result.y);

    // Exchange of X coordinates and processing of remaining angles
    std::ranges::swap(boundBoxMin.x, boundBoxMax.x);

    TransformFromObjectSpace(result, boundBoxMin);
    rect.StretchToPoint(result.x, result.y);

    TransformFromObjectSpace(result, boundBoxMax);
    rect.StretchToPoint(result.x, result.y);

    return rect;
}

// 0x535FA0
void CEntity::PreRender() {
    const auto mi = CModelInfo::GetModelInfo(GetModelIndex());
    const auto ami = mi->AsAtomicModelInfoPtr();

    if (mi->m_n2dfxCount) {
        ProcessLightsForEntity();
    }

    if (!mi->HasBeenPreRendered()) {
        mi->SetHasBeenPreRendered(true);

        if (ami && ami->m_pRwObject) {
            if (RpMatFXAtomicQueryEffects(ami->m_pRwAtomic) && RpAtomicGetGeometry(ami->m_pRwAtomic)) {
                RpGeometryForAllMaterials(RpAtomicGetGeometry(ami->m_pRwAtomic), MaterialUpdateUVAnimCB, nullptr);
            }
        }

        mi->IncreaseAlpha();

        // PC Only
        if (ami) {
            CCustomBuildingDNPipeline::PreRenderUpdate(ami->m_pRwAtomic, false);
        } else if (mi->GetModelType() == MODEL_INFO_CLUMP) {
            CCustomBuildingDNPipeline::PreRenderUpdate(mi->m_pRwClump, false);
        }
        // PC Only
    }

    if (!m_bHasPreRenderEffects) {
        return;
    }

    if (ami && ami->SwaysInWind() && (!GetIsTypeObject() || !AsObject()->objectFlags.bIsExploded)) {
        const auto fDist = DistanceBetweenPoints2D(GetPosition(), TheCamera.GetPosition());
        CObject::fDistToNearestTree = std::min(CObject::fDistToNearestTree, fDist);
        ModifyMatrixForTreeInWind();
    }

    if (GetIsTypeBuilding()) {
        if (ami && ami->IsCrane()) {
            ModifyMatrixForCrane();
        }
        return;
    }

    if (!GetIsTypeObject() && !GetIsTypeDummy()) {
        return;
    }

    if (GetIsTypeObject()) {
        const auto obj = AsObject();
        const auto modelIndex = GetModelIndex();
        
        if (modelIndex == ModelIndices::MI_COLLECTABLE1) {
            CPickups::DoCollectableEffects(this);
            UpdateRW();
            UpdateRwFrame();
        } else if (modelIndex == ModelIndices::MI_MONEY) {
            CPickups::DoMoneyEffects(this);
            UpdateRW();
            UpdateRwFrame();
        } else if (modelIndex == ModelIndices::MI_CARMINE || 
                   modelIndex == ModelIndices::MI_NAUTICALMINE || 
                   modelIndex == ModelIndices::MI_BRIEFCASE) {
            if (obj->objectFlags.bIsPickup) {
                CPickups::DoMineEffects(this);
                UpdateRW();
                UpdateRwFrame();
            }
        } else if (modelIndex == MODEL_MISSILE) {
            if (CReplay::Mode != MODE_PLAYBACK) {
                const auto& vecPos = GetPosition();
                const auto fRand = static_cast<float>(CGeneral::GetRandomNumber() % 16) / 16.0F;
                
                CShadows::StoreShadowToBeRendered(
                    eShadowTextureType::SHADOW_TEX_PED, gpShadowExplosionTex, vecPos,
                    8.0F, 0.0F, 0.0F, -8.0F, 255,
                    static_cast<uint8>(fRand * 200.0F),
                    static_cast<uint8>(fRand * 160.0F),
                    static_cast<uint8>(fRand * 120.0F),
                    20.0F, false, 1.0F, nullptr, false
                );

                CPointLights::AddLight(
                    ePointLightType::PLTYPE_POINTLIGHT, vecPos, CVector(0.0F, 0.0F, 0.0F),
                    8.0F, fRand, fRand * 0.8F, fRand * 0.6F,
                    RwFogType::rwFOGTYPENAFOGTYPE, true, nullptr
                );

                CCoronas::RegisterCorona(
                    reinterpret_cast<uint32>(this), nullptr,
                    static_cast<uint8>(fRand * 255.0F),
                    static_cast<uint8>(fRand * 220.0F),
                    static_cast<uint8>(fRand * 190.0F), 255, vecPos,
                    fRand * 6.0F, 300.0F, gpCoronaTexture[CORONATYPE_SHINYSTAR],
                    eCoronaFlareType::FLARETYPE_NONE, eCoronaReflType::CORREFL_SIMPLE,
                    eCoronaLOSCheck::LOSCHECK_OFF, eCoronaTrail::TRAIL_OFF,
                    0.0F, false, 1.5F, 0, 15.0F, false, false
                );
            }
        } else if (modelIndex == ModelIndices::MI_FLARE) {
            const auto& vecPos = GetPosition();
            const auto fRand = std::max(static_cast<float>(CGeneral::GetRandomNumber() % 16) / 16.0F, 0.5F);
            
            CShadows::StoreShadowToBeRendered(
                eShadowTextureType::SHADOW_TEX_PED, gpShadowExplosionTex, vecPos,
                8.0F, 0.0F, 0.0F, -8.0F, 255,
                static_cast<uint8>(fRand * 200.0F),
                static_cast<uint8>(fRand * 200.0F),
                static_cast<uint8>(fRand * 200.0F),
                20.0F, false, 1.0F, nullptr, false
            );

            CPointLights::AddLight(
                ePointLightType::PLTYPE_POINTLIGHT, vecPos, CVector(0.0F, 0.0F, 0.0F),
                32.0F, fRand, fRand, fRand,
                RwFogType::rwFOGTYPENAFOGTYPE, true, nullptr
            );

            CCoronas::RegisterCorona(
                reinterpret_cast<uint32>(this), nullptr,
                static_cast<uint8>(fRand * 255.0F),
                static_cast<uint8>(fRand * 255.0F),
                static_cast<uint8>(fRand * 255.0F), 255, vecPos,
                fRand * 6.0F, 300.0F, gpCoronaTexture[CORONATYPE_SHINYSTAR],
                eCoronaFlareType::FLARETYPE_NONE, eCoronaReflType::CORREFL_SIMPLE,
                eCoronaLOSCheck::LOSCHECK_OFF, eCoronaTrail::TRAIL_OFF,
                0.0F, false, 1.5F, 0, 15.0F, false, false
            );
        } else if (IsGlassModel(this)) {
            PreRenderForGlassWindow();
        } else if (obj->objectFlags.bIsPickup) {
            CPickups::DoPickUpEffects(this);
            UpdateRW();
            UpdateRwFrame();
        } else if (modelIndex == MODEL_GRENADE) {
            const auto& vecPos = GetPosition();
            const auto vecScaledCam = TheCamera.m_mCameraMatrix.GetRight() * 0.07F;
            const auto vecStreakStart = vecPos - vecScaledCam;
            const auto vecStreakEnd = vecPos + vecScaledCam;
            
            if (CVector2D(obj->m_vecMoveSpeed).Magnitude() > 0.03F) {
                CMotionBlurStreaks::RegisterStreak(
                    reinterpret_cast<uint32>(this), 100, 100, 100, 255, 
                    vecStreakStart, vecStreakEnd
                );
            }
        } else if (modelIndex == MODEL_MOLOTOV) {
            const auto& vecPos = GetPosition();
            const auto vecScaledCam = TheCamera.m_mCameraMatrix.GetRight() * 0.07F;
            const auto vecStreakStart = vecPos - vecScaledCam;
            const auto vecStreakEnd = vecPos + vecScaledCam;
            
            if (CVector2D(obj->m_vecMoveSpeed).Magnitude() > 0.03F) {
                float fWaterLevel;
                if (!CWaterLevel::GetWaterLevelNoWaves(vecPos, &fWaterLevel, nullptr, nullptr) || 
                    vecPos.z > fWaterLevel) {
                    CMotionBlurStreaks::RegisterStreak(
                        reinterpret_cast<uint32>(this), 255, 160, 100, 255, 
                        vecStreakStart, vecStreakEnd
                    );
                }
            }
        } else if (modelIndex == ModelIndices::MI_BEACHBALL) {
            if (DistanceBetweenPoints(GetPosition(), TheCamera.GetPosition()) < 50.0F) {
                const auto ucShadowStrength = static_cast<uint8>(CTimeCycle::m_CurrentColours.m_nShadowStrength);
                CShadows::StoreShadowToBeRendered(
                    eShadowType::SHADOW_DEFAULT, gpShadowPedTex, GetPosition(),
                    0.4F, 0.0F, 0.0F, -0.4F,
                    ucShadowStrength, ucShadowStrength, ucShadowStrength, ucShadowStrength,
                    20.0F, false, 1.0F, nullptr, false
                );
            }
        } else if (modelIndex == ModelIndices::MI_MAGNOCRANE_HOOK || 
                   modelIndex == ModelIndices::MI_WRECKING_BALL || 
                   modelIndex == ModelIndices::MI_CRANE_MAGNET || 
                   modelIndex == ModelIndices::MI_MINI_MAGNET || 
                   modelIndex == ModelIndices::MI_CRANE_HARNESS) {
            if (DistanceBetweenPoints(GetPosition(), TheCamera.GetPosition()) < 100.0F) {
                CShadows::StoreShadowToBeRendered(
                    eShadowType::SHADOW_DEFAULT, gpShadowPedTex, GetPosition(),
                    2.0F, 0.0F, 0.0F, -2.0F, 128, 128, 128, 128,
                    50.0F, false, 1.0F, nullptr, false
                );
            }
        } else if (modelIndex == ModelIndices::MI_WINDSOCK) {
            BuildWindSockMatrix();
        }
    }

    const auto modelIndex = GetModelIndex();
    constexpr auto storeShadowForPole = [](auto* entity, float x, float y, float z) {
        CShadows::StoreShadowForPole(entity, x, y, z, 16.0F, 0.4F, 0);
    };

    if (modelIndex == ModelIndices::MI_TRAFFICLIGHTS) {
        CTrafficLights::DisplayActualLight(this);
        storeShadowForPole(this, 2.957F, 0.147F, 0.0F);
    } else if (modelIndex == ModelIndices::MI_TRAFFICLIGHTS_VERTICAL) {
        CTrafficLights::DisplayActualLight(this);
    } else if (modelIndex == ModelIndices::MI_TRAFFICLIGHTS_MIAMI) {
        CTrafficLights::DisplayActualLight(this);
        storeShadowForPole(this, 4.81F, 0.0F, 0.0F);
    } else if (modelIndex == ModelIndices::MI_TRAFFICLIGHTS_TWOVERTICAL) {
        CTrafficLights::DisplayActualLight(this);
        storeShadowForPole(this, 7.503F, 0.0F, 0.0F);
    } else if (modelIndex == ModelIndices::MI_TRAFFICLIGHTS_3 || 
               modelIndex == ModelIndices::MI_TRAFFICLIGHTS_4 || 
               modelIndex == ModelIndices::MI_TRAFFICLIGHTS_5 || 
               modelIndex == ModelIndices::MI_TRAFFICLIGHTS_GAY) {
        CTrafficLights::DisplayActualLight(this);
    } else if (modelIndex == ModelIndices::MI_SINGLESTREETLIGHTS1) {
        storeShadowForPole(this, 0.744F, 0.0F, 0.0F);
    } else if (modelIndex == ModelIndices::MI_SINGLESTREETLIGHTS2) {
        storeShadowForPole(this, 0.043F, 0.0F, 0.0F);
    } else if (modelIndex == ModelIndices::MI_SINGLESTREETLIGHTS3) {
        storeShadowForPole(this, 1.143F, 0.145F, 0.0F);
    } else if (modelIndex == ModelIndices::MI_DOUBLESTREETLIGHTS) {
        storeShadowForPole(this, 0.0F, -0.048F, 0.0F);
    } else if (modelIndex == ModelIndices::MI_TRAFFICLIGHTS_VEGAS) {
        CTrafficLights::DisplayActualLight(this);
        storeShadowForPole(this, 7.5F, 0.2F, 0.0F);
    }
}

// 0x534310
void CEntity::Render() {
    if (!GetRwObject()) {
        return;
    }

    if (RwObjectGetType(GetRwObject()) == rpATOMIC && CTagManager::IsTag(this)) {
        CTagManager::RenderTagForPC(m_pRwAtomic);
        return;
    }

    const bool isJellyfish = GetModelIndex() == ModelIndices::MI_JELLYFISH || GetModelIndex() == ModelIndices::MI_JELLYFISH01;

    uint32 savedAlphaRef;
    if (isJellyfish) {
        RwRenderStateGet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(&savedAlphaRef));
        RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(0u));
    }

    m_bImBeingRendered = true;

    if (RwObjectGetType(GetRwObject()) == rpATOMIC) {
        RpAtomicRender(m_pRwAtomic);
    } else {
        RpClumpRender(m_pRwClump);
    }

    CStreaming::RenderEntity(m_pStreamingLink);
    RenderEffects();

    m_bImBeingRendered = false;

    if (isJellyfish) {
        RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(savedAlphaRef));
    }
}

// 0x553DC0
bool CEntity::SetupLighting() {
    if (!m_bLightObject) {
        return false;
    }

    ActivateDirectional();
    const auto fLight = CPointLights::GenerateLightsAffectingObject(&GetPosition(), nullptr, this) * 0.5F;
    SetLightColoursForPedsCarsAndObjects(fLight);

    return true;
}

// 0x553370
void CEntity::RemoveLighting(bool reset) {
    if (reset) {
        SetAmbientColours();
        DeActivateDirectional();
        CPointLights::RemoveLightsAffectingObject();
    }
}

// 0x532B00
void CEntity::UpdateRwFrame() {
    if (GetRwObject()) {
        RwFrameUpdateObjects(RwFrameGetParent(rwObjectGetParent(GetRwObject())));
    }
}

// 0x532B20
void CEntity::UpdateRpHAnim() {
    if (const auto atomic = GetFirstAtomic(m_pRwClump)) {
        if (RpSkinGeometryGetSkin(RpAtomicGetGeometry(atomic)) && !m_bDontUpdateHierarchy) {
            RpHAnimHierarchyUpdateMatrices(GetAnimHierarchyFromSkinClump(m_pRwClump));
        }
    }
}

// 0x532B70
bool CEntity::HasPreRenderEffects() {
    const auto modelIndex = GetModelIndex();
    auto mi = CModelInfo::GetModelInfo(modelIndex);

    if (mi->SwaysInWind() || mi->IsCrane()) {
        return true;
    }

    // Check for special models
    if (modelIndex == ModelIndices::MI_COLLECTABLE1
        || modelIndex == ModelIndices::MI_MONEY
        || modelIndex == ModelIndices::MI_CARMINE
        || modelIndex == ModelIndices::MI_NAUTICALMINE
        || modelIndex == ModelIndices::MI_BRIEFCASE
        || modelIndex == MODEL_MISSILE
        || modelIndex == MODEL_GRENADE
        || modelIndex == MODEL_MOLOTOV
        || modelIndex == ModelIndices::MI_BEACHBALL
        || modelIndex == ModelIndices::MI_MAGNOCRANE_HOOK
        || modelIndex == ModelIndices::MI_WRECKING_BALL
        || modelIndex == ModelIndices::MI_CRANE_MAGNET
        || modelIndex == ModelIndices::MI_MINI_MAGNET
        || modelIndex == ModelIndices::MI_CRANE_HARNESS
        || modelIndex == ModelIndices::MI_WINDSOCK
        || modelIndex == ModelIndices::MI_FLARE) {
        return true;
    }

    if (IsGlassModel(this)) {
        return true;
    }

    // Checking for a pickup object
    if (GetIsTypeObject() && reinterpret_cast<CObject*>(this)->objectFlags.bIsPickup) {
        return true;
    }

    // Checking traffic lights
    if (CTrafficLights::IsMITrafficLight(modelIndex)) {
        return true;
    }

    // Checking street lights
    if (modelIndex == ModelIndices::MI_SINGLESTREETLIGHTS1
        || modelIndex == ModelIndices::MI_SINGLESTREETLIGHTS2
        || modelIndex == ModelIndices::MI_SINGLESTREETLIGHTS3
        || modelIndex == ModelIndices::MI_DOUBLESTREETLIGHTS) {
        return true;
    }

    // Checking for 2D light effects
    if (!mi->m_n2dfxCount) {
        return false;
    }

    for (int32 i = 0; i < mi->m_n2dfxCount; ++i) {
        if (mi->Get2dEffect(i)->m_Type == e2dEffectType::EFFECT_LIGHT) {
            return true;
        }
    }

    return false;
}

// 0x532D40
bool CEntity::DoesNotCollideWithFlyers() {
    auto mi = CModelInfo::GetModelInfo(GetModelIndex());
    return mi->SwaysInWind() || mi->bDontCollideWithFlyer;
}

// 0x532D70
RpMaterial* MaterialUpdateUVAnimCB(RpMaterial* material, void* data) {
    if (!RpMaterialUVAnimExists(material)) {
        return material;
    }

    auto fTimeStep = CTimer::GetTimeStepInSeconds();
    RpMaterialUVAnimAddAnimTime(material, fTimeStep);
    RpMaterialUVAnimApplyUpdate(material);
    return material;
}

// 0x532DB0
void CEntity::BuildWindSockMatrix() {
    auto vecWindDir = CVector(CWeather::WindDir.x + 0.01F, CWeather::WindDir.y + 0.01F, 0.1F);

    auto forward = vecWindDir;
    forward.Normalise();

    constexpr CVector side{ 0.0F, 0.0F, 1.0F };
    auto right = side.Cross(forward);
    right.Normalise();

    const auto up = forward.Cross(right);
    auto& matrix = GetMatrix();
    matrix.GetRight() = right;
    matrix.GetForward() = forward;
    matrix.GetUp() = up;

    UpdateRW();
    UpdateRwFrame();
}

// 0x533050
bool CEntity::LivesInThisNonOverlapSector(int32 x, int32 y) {
    const auto rect = GetBoundRect();

    float centerX, centerY;
    rect.GetCenter(&centerX, &centerY);

    const auto middleX = CWorld::GetSectorX(centerX);
    const auto middleY = CWorld::GetSectorY(centerY);

    return x == middleX && y == middleY;
}

// 0x533150
void CEntity::SetupBigBuilding() {
    SetUsesCollision(false);
    m_bIsBIGBuilding = true;
    m_bStreamingDontDelete = true;
    CModelInfo::GetModelInfo(GetModelIndex())->SetOwnsColModel(true);
}

// 0x533170
void CEntity::ModifyMatrixForCrane() {
    if (CTimer::GetIsPaused()) {
        return;
    }

    if (!GetRwObject()) {
        return;
    }

    auto parentMatrix = GetModellingMatrix();
    if (!parentMatrix) {
        return;
    }

    auto tempMat = CMatrix(parentMatrix, false);
    auto fRot = (CTimer::GetTimeInMS() & 0x3FF) * (PI / 512.26F);
    tempMat.SetRotateZOnly(fRot);
    tempMat.UpdateRW();
    UpdateRwFrame();
}

// 0x533240
void CEntity::PreRenderForGlassWindow() {
    auto mi = CModelInfo::GetModelInfo(GetModelIndex());
    if (!mi->IsGlassType2()) {
        CGlass::AskForObjectToBeRenderedInGlass(this);
        SetIsVisible(false);
    }
}

// 0x5332C0
void CEntity::SetRwObjectAlpha(int32 alpha) {
    if (!GetRwObject()) {
        return;
    }

    switch (RwObjectGetType(GetRwObject())) {
    case rpATOMIC:
        SetAtomicAlphaCB(m_pRwAtomic, (void*)alpha);
        break;
    case rpCLUMP:
        RpClumpForAllAtomics(m_pRwClump, SetAtomicAlphaCB, (void*)alpha);
        break;
    }
}

// 0x533310
bool IsEntityPointerValid(CEntity* entity) {
    if (!entity) {
        return false;
    }

    switch (entity->GetType()) {
    case ENTITY_TYPE_BUILDING:
        return IsBuildingPointerValid(entity->AsBuilding());
    case ENTITY_TYPE_VEHICLE:
        return IsVehiclePointerValid(entity->AsVehicle());
    case ENTITY_TYPE_PED:
        return IsPedPointerValid(entity->AsPed());
    case ENTITY_TYPE_OBJECT:
        return IsObjectPointerValid(entity->AsObject());
    case ENTITY_TYPE_DUMMY:
        return IsDummyPointerValid(entity->AsDummy());
    case ENTITY_TYPE_NOTINPOOLS:
        return true;
    }

    return false;
}

// 0x533380
CVector* CEntity::FindTriggerPointCoors(CVector* outVec, int32 index) {
    auto mi = CModelInfo::GetModelInfo(GetModelIndex());
    for (int32 iFxInd = 0; iFxInd < mi->m_n2dfxCount; ++iFxInd) {
        auto effect = mi->Get2dEffect(iFxInd);
        if (effect->m_Type == e2dEffectType::EFFECT_TRIGGER_POINT && effect->slotMachineIndex.m_nId == index) {
            *outVec = GetMatrix().TransformPoint(effect->m_Pos);
            return outVec;
        }
    }

    outVec->Reset();
    return outVec;
}

/**
 * @addr 0x533410
 * 
 * Returns a random effect with the given effectType among all the effects of the entity.
 * 
 * @param   effectType Type of effect. See e2dEffectType. (Always EFFECT_ATTRACTOR)
 * @param   mustBeFree Should check for empty slot. (Always true)
 * 
 * @return Random effect
 */
C2dEffect* CEntity::GetRandom2dEffect(int32 effectType, bool mustBeFree) {
    C2dEffect* apArr[32]{}; // todo: static_vector
    auto       mi = CModelInfo::GetModelInfo(GetModelIndex());
    size_t     iFoundCount = 0;
    for (int32 iFxInd = 0; iFxInd < mi->m_n2dfxCount; ++iFxInd) {
        auto effect = mi->Get2dEffect(iFxInd);
        if (effect->m_Type != effectType) {
            continue;
        }

        if (mustBeFree && !GetPedAttractorManager()->HasEmptySlot(notsa::cast<C2dEffectPedAttractor>(effect), this)) {
            continue;
        }

        if (iFoundCount < 32) {
            apArr[iFoundCount] = effect;
            ++iFoundCount;
        }
    }

    if (iFoundCount) {
        return apArr[CGeneral::GetRandomNumberInRange(0u, iFoundCount)];
    }

    return nullptr;
}

// 0x5334F0
CVector CEntity::TransformFromObjectSpace(const CVector& offset) const {
    auto result = CVector();
    if (m_matrix) {
        result = m_matrix->TransformPoint(offset);
        return result;
    }

    TransformPoint(result, m_placement, offset);
    return result;
}

// 0x533560
CVector* CEntity::TransformFromObjectSpace(CVector& outPos, const CVector& offset) const {
    auto result = TransformFromObjectSpace(offset);
    outPos = result;
    return &outPos;
}

// 0x533790
void CEntity::CreateEffects() {
    m_bHasRoadsignText = false;
    auto mi = CModelInfo::GetModelInfo(GetModelIndex());
    if (!mi->m_n2dfxCount) {
        return;
    }

    for (int32 iFxInd = 0; iFxInd < mi->m_n2dfxCount; ++iFxInd) {
        auto effect = mi->Get2dEffect(iFxInd);
        switch (effect->m_Type) {
        case e2dEffectType::EFFECT_LIGHT: {
            m_bHasPreRenderEffects = true;
            break;
        }
        case e2dEffectType::EFFECT_PARTICLE: {
            g_fx.CreateEntityFx(this, effect->particle.m_szName, effect->m_Pos, GetModellingMatrix());
            break;
        }
        case e2dEffectType::EFFECT_ATTRACTOR: {
            if (effect->pedAttractor.m_nAttractorType == ePedAttractorType::PED_ATTRACTOR_TRIGGER_SCRIPT) {
                CTheScripts::ScriptsForBrains.RequestAttractorScriptBrainWithThisName(effect->pedAttractor.m_szScriptName);
            }
            break;
        }
        case e2dEffectType::EFFECT_ENEX: {
            auto vecExit = effect->m_Pos + effect->enEx.m_vecExitPosn;
            auto vecWorldEffect = TransformFromObjectSpace(effect->m_Pos);
            auto vecWorldExit = TransformFromObjectSpace(vecExit);

            if (effect->enEx.bTimedEffect) {
                auto ucDays = CClock::GetGameClockDays();
                if (effect->enEx.m_nTimeOn > effect->enEx.m_nTimeOff && CClock::ms_nGameClockHours < effect->enEx.m_nTimeOff) {
                    ucDays--;
                }

                srand(reinterpret_cast<uint32>(this) + ucDays);
            }

            auto fHeading = GetHeading();
            auto fExitRot = effect->enEx.m_fExitAngle + RadiansToDegrees(fHeading);
            auto fEnterRot = effect->enEx.m_fEnterAngle + RadiansToDegrees(fHeading);
            auto iEnExId = CEntryExitManager::AddOne(
                vecWorldEffect.x,
                vecWorldEffect.y,
                vecWorldEffect.z,
                fEnterRot,
                effect->enEx.m_vecRadius.x,
                effect->enEx.m_vecRadius.y,
                0,
                vecWorldExit.x,
                vecWorldExit.y,
                vecWorldExit.z,
                fExitRot,
                effect->enEx.m_nInteriorId,
                (CEntryExit::eFlags)(effect->enEx.m_nFlags1 + (effect->enEx.m_nFlags2 << 8)),
                effect->enEx.m_nSkyColor,
                effect->enEx.m_nTimeOn,
                effect->enEx.m_nTimeOff,
                0,
                effect->enEx.m_szInteriorName
            );

            if (iEnExId != -1) {
                if (auto* const enex = CEntryExitManager::GetInSlot(iEnExId)) {
                    if (enex->m_pLink && !enex->m_pLink->bEnableAccess) {
                        enex->bEnableAccess = false;
                    }
                }
            }
            break;
        }
        case e2dEffectType::EFFECT_ROADSIGN: {
            m_bHasRoadsignText = true;
            auto uiPalleteId = C2dEffect::Roadsign_GetPaletteIDFromFlags(effect->roadsign.m_nFlags);
            auto uiLettersPerLine = C2dEffect::Roadsign_GetNumLettersFromFlags(effect->roadsign.m_nFlags);
            auto uiNumLines = C2dEffect::Roadsign_GetNumLinesFromFlags(effect->roadsign.m_nFlags);

            auto signAtomic = CCustomRoadsignMgr::CreateRoadsignAtomic(effect->roadsign.m_vecSize.x, effect->roadsign.m_vecSize.y, uiNumLines,
                                                                       &effect->roadsign.m_pText[0], // todo: does it good indexing for sign lines?
                                                                       &effect->roadsign.m_pText[16],
                                                                       &effect->roadsign.m_pText[32],
                                                                       &effect->roadsign.m_pText[48],
                                                                       uiLettersPerLine,
                                                                       uiPalleteId);

            auto frame = RpAtomicGetFrame(signAtomic);
            RwFrameSetIdentity(frame);

            const CVector axis0{ 1.0F, 0.0F, 0.0F }, axis1{ 0.0F, 1.0F, 0.0F }, axis2{ 0.0F, 0.0F, 1.0F };
            RwFrameRotate(frame, &axis2, effect->roadsign.m_vecRotation.z, RwOpCombineType::rwCOMBINEREPLACE);
            RwFrameRotate(frame, &axis0, effect->roadsign.m_vecRotation.x, RwOpCombineType::rwCOMBINEPOSTCONCAT);
            RwFrameRotate(frame, &axis1, effect->roadsign.m_vecRotation.y, RwOpCombineType::rwCOMBINEPOSTCONCAT);
            RwFrameTranslate(frame, &effect->m_Pos, RwOpCombineType::rwCOMBINEPOSTCONCAT);
            RwFrameUpdateObjects(frame);
            effect->roadsign.m_pAtomic = signAtomic;
            break;
        }
        case e2dEffectType::EFFECT_ESCALATOR: {
            auto vecStart = TransformFromObjectSpace(effect->m_Pos);
            auto vecBottom = TransformFromObjectSpace(effect->escalator.m_vecBottom);
            auto vecTop = TransformFromObjectSpace(effect->escalator.m_vecTop);
            auto vecEnd = TransformFromObjectSpace(effect->escalator.m_vecEnd);
            auto bMovingDown = effect->escalator.m_nDirection == 0;

            CEscalators::AddOne(vecStart, vecBottom, vecTop, vecEnd, bMovingDown, this);
            break;
        }
        }
    }
}

// 0x533BF0
void CEntity::DestroyEffects() {
    auto mi = CModelInfo::GetModelInfo(GetModelIndex());
    if (!mi->m_n2dfxCount) {
        return;
    }

    for (int32 iFxInd = 0; iFxInd < mi->m_n2dfxCount; ++iFxInd) {
        auto effect = mi->Get2dEffect(iFxInd);
        switch (effect->m_Type) {
        case e2dEffectType::EFFECT_ATTRACTOR: {
            if (effect->pedAttractor.m_nAttractorType == ePedAttractorType::PED_ATTRACTOR_TRIGGER_SCRIPT) {
                CTheScripts::ScriptsForBrains.MarkAttractorScriptBrainWithThisNameAsNoLongerNeeded(effect->pedAttractor.m_szScriptName);
            }
            break;
        }
        case e2dEffectType::EFFECT_PARTICLE: {
            g_fx.DestroyEntityFx(this);
            break;
        }
        case e2dEffectType::EFFECT_ROADSIGN: {
            C2dEffect::DestroyAtomic(effect->roadsign.m_pAtomic);
            effect->roadsign.m_pAtomic = nullptr;
            break;
        }
        case e2dEffectType::EFFECT_ENEX: {
            auto vecWorld = TransformFromObjectSpace(effect->m_Pos);
            auto iNearestEnex = CEntryExitManager::FindNearestEntryExit(vecWorld, 1.5F, -1);
            if (iNearestEnex != -1) {
                auto enex = CEntryExitManager::GetInSlot(iNearestEnex);
                if (enex->bEnteredWithoutExit) {
                    enex->bDeleteEnex = true;
                } else {
                    CEntryExitManager::DeleteOne(iNearestEnex);
                }
            }
            break;
        }
        }
    }
}

// 0x533ED0
void CEntity::AttachToRwObject(RwObject* object, bool updateMatrix) {
    if (!m_bIsVisible) {
        return;
    }

    m_pRwObject = object;
    if (!GetRwObject()) {
        return;
    }

    if (updateMatrix) {
        CMatrix& matrix = GetMatrix();
        auto     parentMatrix = GetModellingMatrix();
        matrix.UpdateMatrix(parentMatrix);
    }

    auto mi = CModelInfo::GetModelInfo(GetModelIndex());
    if (RwObjectGetType(GetRwObject()) == rpCLUMP && mi->IsRoad()) {
        if (GetIsTypeObject()) {
            reinterpret_cast<CObject*>(this)->AddToMovingList();
            SetIsStatic(false);
        } else {
            CWorld::ms_listMovingEntityPtrs.AddItem(AsPhysical());
        }
    }

    mi->AddRef();
    m_pStreamingLink = CStreaming::AddEntity(this);
    CreateEffects();
}

// 0x533FB0
void CEntity::DetachFromRwObject() {
    if (!GetRwObject()) {
        return;
    }

    auto mi = CModelInfo::GetModelInfo(GetModelIndex());
    mi->RemoveRef();
    CStreaming::RemoveEntity(m_pStreamingLink);
    m_pStreamingLink = nullptr;

    if (mi->GetModelType() == ModelInfoType::MODEL_INFO_CLUMP
        && mi->IsRoad()
        && !GetIsTypeObject()) {
        CWorld::ms_listMovingEntityPtrs.DeleteItem(AsPhysical());
    }

    DestroyEffects();
    m_pRwObject = nullptr;
}

// 0x534250
CVector* CEntity::GetBoundCentre(CVector* pOutCentre) const {
    auto        mi = CModelInfo::GetModelInfo(GetModelIndex());
    const auto& colCenter = mi->GetColModel()->GetBoundCenter();
    return TransformFromObjectSpace(*pOutCentre, colCenter);
}

// 0x534290
void CEntity::GetBoundCentre(CVector& centre) const {
    TransformFromObjectSpace(centre, GetColModel()->GetBoundCenter());
}

CVector CEntity::GetBoundCentre() const {
    CVector v;
    GetBoundCentre(v);
    return v;
}

// 0x5342B0
void CEntity::RenderEffects() {
    if (!m_bHasRoadsignText) {
        return;
    }

    auto mi = CModelInfo::GetModelInfo(GetModelIndex());
    if (!mi->m_n2dfxCount) {
        return;
    }

    for (int32 iFxInd = 0; iFxInd < mi->m_n2dfxCount; ++iFxInd) {
        auto effect = mi->Get2dEffect(iFxInd);
        if (effect->m_Type != e2dEffectType::EFFECT_ROADSIGN) {
            continue;
        }

        CCustomRoadsignMgr::RenderRoadsignAtomic(effect->roadsign.m_pAtomic, TheCamera.GetPosition());
    }
}

// 0x5343F0
bool CEntity::GetIsTouching(CEntity* entity) const {
    CVector thisVec;
    GetBoundCentre(thisVec);

    CVector otherVec;
    entity->GetBoundCentre(otherVec);

    auto fThisRadius = CModelInfo::GetModelInfo(GetModelIndex())->GetColModel()->GetBoundRadius();
    auto fOtherRadius = CModelInfo::GetModelInfo(entity->GetModelIndex())->GetColModel()->GetBoundRadius();

    return (thisVec - otherVec).Magnitude() <= (fThisRadius + fOtherRadius);
}

// 0x5344B0
bool CEntity::GetIsTouching(const CVector& centre, float radius) const {
    CVector thisVec;
    GetBoundCentre(thisVec);
    auto fThisRadius = CModelInfo::GetModelInfo(GetModelIndex())->GetColModel()->GetBoundRadius();

    return (thisVec - centre).Magnitude() <= (fThisRadius + radius);
}

// 0x534540
bool CEntity::GetIsOnScreen() {
    CVector thisVec;
    GetBoundCentre(thisVec);
    return TheCamera.IsSphereVisible(thisVec, CModelInfo::GetModelInfo(GetModelIndex())->GetColModel()->GetBoundRadius());
}

// 0x5345D0
bool CEntity::GetIsBoundingBoxOnScreen() {
    auto cm = CModelInfo::GetModelInfo(GetModelIndex())->GetColModel();

    RwV3d vecNormals[2];
    if (m_matrix) {
        auto tempMat = CMatrix();
        Invert(*m_matrix, tempMat);
        TransformVectors(&vecNormals[0], 2, tempMat, &TheCamera.m_avecFrustumWorldNormals[0]);
    } else {
        auto tempTrans = CSimpleTransform();
        tempTrans.Invert(m_placement);
        TransformVectors(&vecNormals[0], 2, tempTrans, &TheCamera.m_avecFrustumWorldNormals[0]);
    }

    for (int32 i = 0; i < 2; ++i) {
#define ChooseComponent(c) vecNormals[i].c < 0 ? cm->m_boundBox.m_vecMax.c : cm->m_boundBox.m_vecMin.c
        CVector vecWorld = TransformFromObjectSpace(CVector{
            ChooseComponent(x),
            ChooseComponent(y),
            ChooseComponent(z) });
#undef ChooseComponent
        if (DotProduct(vecWorld, TheCamera.m_avecFrustumWorldNormals[i]) > TheCamera.m_fFrustumPlaneOffsets[i]) {
            if (!TheCamera.m_bMirrorActive
                || DotProduct(vecWorld, TheCamera.m_avecFrustumWorldNormals_Mirror[i]) > TheCamera.m_fFrustumPlaneOffsets_Mirror[i]) {
                ++numBBFailed;
                return false;
            }
        }
    }
    return true;
}

// 0x534E90
void CEntity::ModifyMatrixForTreeInWind() {
    if (CTimer::GetIsPaused()) {
        return;
    }

    auto parentMatrix = GetModellingMatrix();
    if (!parentMatrix) {
        return;
    }

    auto at = RwMatrixGetAt(parentMatrix);

    float fWindOffset;
    if (CWeather::Wind >= 0.5F) {
        auto uiOffset1 = (((m_nRandomSeed + CTimer::GetTimeInMS() * 8) & 0xFFFF) / 4'096) % 16;
        auto uiOffset2 = (uiOffset1 + 1) % 16;
        auto fContrib = static_cast<float>(((m_nRandomSeed + CTimer::GetTimeInMS() * 8) % 4'096)) / 4096.0F;

        fWindOffset = (1.0F - fContrib) * CWeather::saTreeWindOffsets[uiOffset1];
        fWindOffset += 1.0F + fContrib * CWeather::saTreeWindOffsets[uiOffset2];
        fWindOffset *= CWeather::Wind;
        fWindOffset *= 0.015F;

    } else {
        auto uiTimeOffset = (reinterpret_cast<uint32>(this) + CTimer::GetTimeInMS()) & 0xFFF;

        constexpr float scalingFactor = 6.28f / 4096.f;
        fWindOffset = sin(uiTimeOffset * scalingFactor) * 0.005F;
        if (CWeather::Wind >= 0.2F) {
            fWindOffset *= 1.6F;
        }
    }

    at->x = fWindOffset;
    if (CModelInfo::GetModelInfo(GetModelIndex())->IsSwayInWind2()) {
        at->x += CWeather::Wind * 0.03F;
    }

    at->y = at->x;
    at->x *= CWeather::WindDir.x;
    at->y *= CWeather::WindDir.y;

    CWindModifiers::FindWindModifier(GetPosition(), &at->x, &at->y);
    UpdateRwFrame();
}

// 0x535040
void CEntity::ModifyMatrixForBannerInWind() {
    if (CTimer::GetIsPaused()) {
        return;
    }

    auto vecPos = CVector2D(GetPosition());
    auto uiOffset = static_cast<uint16>(16 * (CTimer::GetTimeInMS() + (static_cast<uint16>(vecPos.x + vecPos.y) * 64)));

    auto fWind = 0.2F;
    if (CWeather::Wind >= 0.1F) {
        if (CWeather::Wind < 0.8F) {
            fWind = 0.43F;
        } else {
            fWind = 0.66F;
        }
    }

    auto   fContrib = static_cast<float>(uiOffset & 0x7FF) / 2048.0F;
    uint32 uiIndex = uiOffset / 2'048;
    auto   fWindOffset = (1.0F - fContrib) * CWeather::saBannerWindOffsets[uiIndex];
    fWindOffset += fContrib * CWeather::saBannerWindOffsets[(uiIndex + 1) & 0x1F];
    fWindOffset *= CWeather::Wind;

    auto fZPos = sqrt(1.0F - pow(fWindOffset, 2.0F));

    CMatrix& matrix = GetMatrix();
    auto     vecCross = CrossProduct(matrix.GetForward(), matrix.GetUp());
    vecCross.z = 0.0F;
    vecCross.Normalise();

    auto vecWind = CVector(vecCross.x * fWindOffset, vecCross.y * fWindOffset, fZPos);
    auto vecCross2 = CrossProduct(matrix.GetForward(), vecWind);

    matrix.GetRight() = vecCross2;
    matrix.GetUp() = vecWind;

    UpdateRW();
    UpdateRwFrame();
}

// 0x46A2D0
RwMatrix* CEntity::GetModellingMatrix() {
    if (!GetRwObject()) {
        return nullptr;
    }

    return GetRwMatrix();
}

// 0x535300
CColModel* CEntity::GetColModel() const {
    if (GetIsTypeVehicle()) {
        const auto veh = static_cast<const CVehicle*>(this);
        if (veh->m_vehicleSpecialColIndex > -1) {
            return &CVehicle::m_aSpecialColModel[veh->m_vehicleSpecialColIndex];
        }
    }

    return CModelInfo::GetModelInfo(GetModelIndex())->GetColModel();
}

// 0x535340
//https://gamedev.stackexchange.com/a/35948
//https://gamedev.stackexchange.com/questions/153326/how-to-rotate-directional-billboard-particle-sprites-toward-the-direction-the-pa/153814#153814
void CEntity::CalculateBBProjection(CVector* corner1, CVector* corner2, CVector* corner3, CVector* corner4) {
    CMatrix& matrix = GetMatrix();
    auto     fMagRight = CVector2D(matrix.GetRight()).Magnitude();
    auto     fMagForward = CVector2D(matrix.GetForward()).Magnitude();
    auto     fMagUp = CVector2D(matrix.GetUp()).Magnitude();

    auto cm = CModelInfo::GetModelInfo(GetModelIndex())->GetColModel();
    auto fMaxX = std::max(-cm->m_boundBox.m_vecMin.x, cm->m_boundBox.m_vecMax.x);
    auto fMaxY = std::max(-cm->m_boundBox.m_vecMin.y, cm->m_boundBox.m_vecMax.y);
    auto fMaxZ = std::max(-cm->m_boundBox.m_vecMin.z, cm->m_boundBox.m_vecMax.z);

    auto fXSize = fMaxX * fMagRight * 2.0F;
    auto fYSize = fMaxY * fMagForward * 2.0F;
    auto fZSize = fMaxZ * fMagUp * 2.0F;

    CVector vecDir, vecNormalized, vecTransformed;
    float   fMult1, fMult2, fMult3, fMult4;
    if (fXSize > fYSize && fXSize > fZSize) {
        vecDir = matrix.GetRight();
        vecDir.z = 0.0F;

        vecNormalized = vecDir;
        vecNormalized.Normalise();

        auto vecScaled = vecDir * fMaxX;
        vecTransformed = vecScaled + GetPosition();

        auto vecTemp = GetPosition() - vecScaled;
        vecDir = vecTemp;

        fMult1 = (vecNormalized.x * matrix.GetForward().x + vecNormalized.y * matrix.GetForward().y) * fMaxY;
        fMult2 = (vecNormalized.x * matrix.GetForward().y - vecNormalized.y * matrix.GetForward().x) * fMaxY;
        fMult3 = (vecNormalized.x * matrix.GetUp().x + vecNormalized.y * matrix.GetUp().y) * fMaxZ;
        fMult4 = (vecNormalized.x * matrix.GetUp().y - vecNormalized.y * matrix.GetUp().x) * fMaxZ;
    } else if (fYSize > fZSize) {
        vecDir = matrix.GetForward();
        vecDir.z = 0.0F;

        vecNormalized = vecDir;
        vecNormalized.Normalise();

        auto vecScaled = vecDir * fMaxY;
        vecTransformed = vecScaled + GetPosition();

        auto vecTemp = GetPosition() - vecScaled;
        vecDir = vecTemp;

        fMult1 = (vecNormalized.x * matrix.GetRight().x + vecNormalized.y * matrix.GetRight().y) * fMaxX;
        fMult2 = (vecNormalized.x * matrix.GetRight().y - vecNormalized.y * matrix.GetRight().x) * fMaxX;
        fMult3 = (vecNormalized.x * matrix.GetUp().x + vecNormalized.y * matrix.GetUp().y) * fMaxZ;
        fMult4 = (vecNormalized.x * matrix.GetUp().y - vecNormalized.y * matrix.GetUp().x) * fMaxZ;
    } else {
        vecDir = matrix.GetUp();
        vecDir.z = 0.0F;

        vecNormalized = vecDir;
        vecNormalized.Normalise();

        auto vecScaled = vecDir * fMaxZ;
        vecTransformed = vecScaled + GetPosition();

        auto vecTemp = GetPosition() - vecScaled;
        vecDir = vecTemp;

        fMult1 = (vecNormalized.x * matrix.GetRight().x + vecNormalized.y * matrix.GetRight().y) * fMaxX;
        fMult2 = (vecNormalized.x * matrix.GetRight().y - vecNormalized.y * matrix.GetRight().x) * fMaxX;
        fMult3 = (vecNormalized.x * matrix.GetForward().x + vecNormalized.y * matrix.GetForward().y) * fMaxY;
        fMult4 = (vecNormalized.x * matrix.GetForward().y - vecNormalized.y * matrix.GetForward().x) * fMaxY;
    }

    auto fNegX = -vecNormalized.x;
    fMult1 = fabs(fMult1);
    fMult2 = fabs(fMult2);
    fMult3 = fabs(fMult3);
    fMult4 = fabs(fMult4);

    auto fMult13 = fMult1 + fMult3;
    auto fMult24 = fMult2 + fMult4;

    *corner1 = CVector{
        vecTransformed.x + (vecNormalized.x * fMult13) - (vecNormalized.y * fMult24),
        vecTransformed.y + (vecNormalized.y * fMult13) - (-vecNormalized.x * fMult24),
        vecTransformed.z + (vecNormalized.z * fMult13) - (vecNormalized.z * fMult24),
    };
    *corner2 = CVector{
        vecTransformed.x + (vecNormalized.x * fMult13) + (vecNormalized.y * fMult24),
        vecTransformed.y + (vecNormalized.y * fMult13) + (-vecNormalized.x * fMult24),
        vecTransformed.z + (vecNormalized.z * fMult13) + (vecNormalized.z * fMult24),
    };
    *corner3 = CVector{
        vecDir.x - (vecNormalized.x * fMult13) + (vecNormalized.y * fMult24),
        vecDir.y - (vecNormalized.y * fMult13) + (-vecNormalized.x * fMult24),
        vecDir.z - (vecNormalized.z * fMult13) + (vecNormalized.z * fMult24),
    };
    *corner4 = CVector{
        vecDir.x - (vecNormalized.x * fMult13) - (vecNormalized.y * fMult24),
        vecDir.y - (vecNormalized.y * fMult13) - (-vecNormalized.x * fMult24),
        vecDir.z - (vecNormalized.z * fMult13) - (vecNormalized.z * fMult24),
    };

    const auto& vecPos = GetPosition();
    corner1->z = vecPos.z;
    corner2->z = vecPos.z;
    corner3->z = vecPos.z;
    corner4->z = vecPos.z;
}

// 0x535F00
void CEntity::UpdateAnim() {
    m_bDontUpdateHierarchy = false;
    if (!GetRwObject() || RwObjectGetType(GetRwObject()) != rpCLUMP) {
        return;
    }

    if (!RpAnimBlendClumpGetFirstAssociation(m_pRwClump)) {
        return;
    }

    bool  bOnScreen;
    float fStep;
    if (GetIsTypeObject() && AsObject()->m_nObjectType == eObjectType::OBJECT_TYPE_CUTSCENE) {
        bOnScreen = true;
        fStep = CTimer::GetTimeStepNonClippedInSeconds();
    } else {
        if (!m_bOffscreen) {
            m_bOffscreen = !GetIsOnScreen();
        }

        bOnScreen = !m_bOffscreen;
        fStep = CTimer::GetTimeStepInSeconds();
    }

    RpAnimBlendClumpUpdateAnimations(m_pRwClump, fStep, bOnScreen);
}

// 0x536BC0
bool CEntity::IsVisible() {
    if (!GetRwObject() || !m_bIsVisible) {
        return false;
    }

    return CEntity::GetIsOnScreen();
}

// 0x536BE0
float CEntity::GetDistanceFromCentreOfMassToBaseOfModel() const {
    auto cm = GetColModel();
    return -cm->m_boundBox.m_vecMin.z;
}

// 0x571A00
void CEntity::CleanUpOldReference(CEntity** entity) {
    if (!m_pReferences) {
        return;
    }

    auto refs = m_pReferences;
    auto ppPrev = &m_pReferences;
    while (refs->m_ppEntity != entity) {
        ppPrev = &refs->m_pNext;
        refs = refs->m_pNext;
        if (!refs) {
            return;
        }
    }

    *ppPrev = refs->m_pNext;
    refs->m_pNext = CReferences::pEmptyList;
    refs->m_ppEntity = nullptr;
    CReferences::pEmptyList = refs;
}

// 0x571A40
// Clear (set to null) references to `this`
void CEntity::ResolveReferences() {
    auto refs = m_pReferences;
    while (refs) {
        if (*refs->m_ppEntity == this) {
            *refs->m_ppEntity = nullptr;
        }

        refs = refs->m_pNext;
    }

    refs = m_pReferences;
    if (!refs) {
        return;
    }

    refs->m_ppEntity = nullptr;
    while (refs->m_pNext) {
        refs = refs->m_pNext;
    }

    refs->m_pNext = CReferences::pEmptyList;
    CReferences::pEmptyList = m_pReferences;
    m_pReferences = nullptr;
}

// 0x571A90
void CEntity::PruneReferences() {
    if (!m_pReferences) {
        return;
    }

    auto refs = m_pReferences;
    auto ppPrev = &m_pReferences;
    while (refs) {
        if (*refs->m_ppEntity == this) {
            ppPrev = &refs->m_pNext;
            refs = refs->m_pNext;
        } else {
            auto refTemp = refs->m_pNext;
            *ppPrev = refs->m_pNext;
            refs->m_pNext = CReferences::pEmptyList;
            CReferences::pEmptyList = refs;
            refs->m_ppEntity = nullptr;
            refs = refTemp;
        }
    }
}

// 0x571B70
void CEntity::RegisterReference(CEntity** entity) {
    if (GetIsTypeBuilding() && !m_bIsTempBuilding && !m_bIsProcObject && !GetIplIndex()) {
        return;
    }

    auto refs = m_pReferences;
    while (refs) {
        if (refs->m_ppEntity == entity) {
            return;
        }
        refs = refs->m_pNext;
    }

    if (!m_pReferences && !CReferences::pEmptyList) {
        for (auto& ped : GetPedPool()->GetAllValid()) {
            ped.PruneReferences();
            if (CReferences::pEmptyList) {
                break;
            }
        }

        if (!CReferences::pEmptyList) {
            for (auto& vehicle : GetVehiclePool()->GetAllValid()) {
                vehicle.PruneReferences();
                if (CReferences::pEmptyList) {
                    break;
                }
            }
        }

        if (!CReferences::pEmptyList) {
            for (auto& obj : GetObjectPool()->GetAllValid()) {
                obj.PruneReferences();
                if (CReferences::pEmptyList) {
                    break;
                }
            }
        }
    }

    if (CReferences::pEmptyList) {
        auto pEmptyRef = CReferences::pEmptyList;
        CReferences::pEmptyList = pEmptyRef->m_pNext;
        pEmptyRef->m_pNext = m_pReferences;
        m_pReferences = pEmptyRef;
        pEmptyRef->m_ppEntity = entity;
    }
}

// 0x6FC7A0
void CEntity::ProcessLightsForEntity() {
    constexpr static std::array<uint16, 8> ms_aEntityLightsOffsets = { 0, 27'034, 43'861, 52'326, 64'495, 38'437, 21'930, 39'117 }; // 0x8D5028
    constexpr static auto                  FADE_RATE = 0.0009f;                                                       // 0x872538

    auto fBalance = GetDayNightBalance();
    if (m_bRenderDamaged || !m_bIsVisible) {
        return;
    }

    if (GetIsTypeVehicle()) {
        if (AsVehicle()->physicalFlags.bRenderScorched) {
            return;
        }
    } else {
        if (m_matrix && m_matrix->GetUp().z < 0.96F) {
            return;
        }
    }

    auto mi = CModelInfo::GetModelInfo(GetModelIndex());
    if (!mi->m_n2dfxCount) {
        return;
    }

    for (int32 iFxInd = 0; iFxInd < mi->m_n2dfxCount; ++iFxInd) {
        auto effect = mi->Get2dEffect(iFxInd);
        auto fIntensity = 1.0F;
        auto uiRand = m_nRandomSeed ^ ms_aEntityLightsOffsets[iFxInd % 8];

        if (effect->m_Type == e2dEffectType::EFFECT_SUN_GLARE && CWeather::SunGlare >= 0.0F) {
            auto vecEffPos = TransformFromObjectSpace(effect->m_Pos);

            auto vecDir = vecEffPos - GetPosition();
            vecDir.Normalise();

            auto vecCamDir = TheCamera.GetPosition() - vecEffPos;
            auto fCamDist = vecCamDir.Magnitude();
            auto fScale = 2.0F / fCamDist;
            auto vecScaledCam = (vecCamDir * fScale);
            vecDir += vecScaledCam;
            vecDir.Normalise();

            auto fDot = -DotProduct(vecDir, CTimeCycle::m_VectorToSun[CTimeCycle::m_CurrentStoredValue]);
            if (fDot <= 0.0F) {
                continue;
            }

            auto fGlare = sqrt(fDot) * CWeather::SunGlare;
            auto fRadius = sqrt(fCamDist) * CWeather::SunGlare * 0.5F;
            vecEffPos += vecScaledCam;

            auto ucRed = static_cast<uint8>((CTimeCycle::m_CurrentColours.m_nSunCoreRed + 510) * fGlare / 3.0F);
            auto ucGreen = static_cast<uint8>((CTimeCycle::m_CurrentColours.m_nSunCoreGreen + 510) * fGlare / 3.0F);
            auto ucBlue = static_cast<uint8>((CTimeCycle::m_CurrentColours.m_nSunCoreBlue + 510) * fGlare / 3.0F);
            CCoronas::RegisterCorona(
                m_nRandomSeed + iFxInd + 1,
                nullptr,
                ucRed,
                ucGreen,
                ucBlue,
                255,
                vecEffPos,
                fRadius,
                120.0F,
                gpCoronaTexture[CORONATYPE_SHINYSTAR],
                eCoronaFlareType::FLARETYPE_NONE,
                eCoronaReflType::CORREFL_NONE,
                eCoronaLOSCheck::LOSCHECK_OFF,
                eCoronaTrail::TRAIL_OFF,
                0.0F,
                false,
                1.5F,
                0,
                15.0F,
                false,
                false
            );

            continue;
        }

        if (effect->m_Type != e2dEffectType::EFFECT_LIGHT) {
            continue;
        }

        auto vecEffPos = TransformFromObjectSpace(effect->m_Pos);
        auto bDoColorLight = false;
        auto bDoNoColorLight = false;
        auto bCoronaVisible = false;
        bool bUpdateCoronaCoors = false;
        auto fDayNight = 1.0F;
        if (effect->light.m_bAtDay && effect->light.m_bAtNight) {
            bCoronaVisible = true;
        } else if (effect->light.m_bAtDay && fBalance < 1.0F) {
            bCoronaVisible = true;
            fDayNight = 1.0F - fBalance;
        } else if (effect->light.m_bAtNight && fBalance > 0.0F) {
            bCoronaVisible = true;
            fDayNight = fBalance;
        }

        const auto& vecPos = GetPosition();
        auto        iFlashType = effect->light.m_nCoronaFlashType;
        float       fBalance; // todo: shadow var
        uint32      uiMode, uiOffset;
        if (iFlashType == e2dCoronaFlashType::FLASH_RANDOM_WHEN_WET && CWeather::WetRoads > 0.5F || bCoronaVisible) {
            switch (iFlashType) {
            case e2dCoronaFlashType::FLASH_DEFAULT:
                bDoColorLight = true;
                break;

            case e2dCoronaFlashType::FLASH_RANDOM:
            case e2dCoronaFlashType::FLASH_RANDOM_WHEN_WET:
                if ((CTimer::GetTimeInMS() ^ uiRand) & 0x60) {
                    bDoColorLight = true;
                } else {
                    bDoNoColorLight = true;
                }

                if ((uiRand ^ (CTimer::GetTimeInMS() / 4'096)) & 0x3) {
                    bDoColorLight = true;
                }

                break;

            case e2dCoronaFlashType::FLASH_ANIM_SPEED_4X:
                if (((CTimer::GetTimeInMS() + iFxInd * 256) & 0x200) == 0) {
                    bUpdateCoronaCoors = true;
                } else {
                    bDoColorLight = true;
                }

                break;

            case e2dCoronaFlashType::FLASH_ANIM_SPEED_2X:
                if (((CTimer::GetTimeInMS() + iFxInd * 512) & 0x400) == 0) {
                    bUpdateCoronaCoors = true;
                } else {
                    bDoColorLight = true;
                }

                break;

            case e2dCoronaFlashType::FLASH_ANIM_SPEED_1X:
                if (((CTimer::GetTimeInMS() + iFxInd * 1'024) & 0x800) == 0) {
                    bUpdateCoronaCoors = true;
                } else {
                    bDoColorLight = true;
                }

                break;

            case e2dCoronaFlashType::FLASH_UNKN:
                if (static_cast<uint8>(uiRand) > 0x10) {
                    bDoColorLight = true;
                    break;
                }

                if ((CTimer::GetTimeInMS() ^ (uiRand * 8)) & 0x60) {
                    bDoColorLight = true;
                } else {
                    bDoNoColorLight = true;
                }

                if ((uiRand ^ (CTimer::GetTimeInMS() / 4'096)) & 0x3) {
                    bDoColorLight = true;
                }

                break;

            case e2dCoronaFlashType::FLASH_TRAINCROSSING:
                if (GetIsTypeObject() && AsObject()->objectFlags.bTrainCrossEnabled) {
                    if (CTimer::GetTimeInMS() & 0x400) {
                        bDoColorLight = true;
                    }

                    if (iFxInd & 1) {
                        bDoColorLight = !bDoColorLight;
                    }
                }

                if (iFxInd >= 4) {
                    bDoColorLight = false;
                }

                break;

            case e2dCoronaFlashType::FLASH_UNUSED:
                if (CBridge::ShouldLightsBeFlashing() && (CTimer::GetTimeInMS() & 0x1FF) < 0x3C) {
                    bDoColorLight = true;
                }

                break;

            case e2dCoronaFlashType::FLASH_ONLY_RAIN:
                if (CWeather::Rain > 0.0001F) {
                    fIntensity = CWeather::Rain;
                    bDoColorLight = true;
                }
                break;

            case e2dCoronaFlashType::FLASH_5ON_5OFF:
            case e2dCoronaFlashType::FLASH_6ON_4OFF:
            case e2dCoronaFlashType::FLASH_4ON_6OFF:

                bDoColorLight = true;

                uiOffset = CTimer::GetTimeInMS() + 3'333 * (iFlashType - 11);
                uiOffset += static_cast<uint32>(vecPos.x * 20.0F);
                uiOffset += static_cast<uint32>(vecPos.y * 10.0F);

                uiMode = 9 * ((uiOffset % 10'000) / 10'000);
                fBalance = ((uiOffset % 10'000) - (1'111 * uiMode)) * FADE_RATE;
                switch (uiMode) {
                case 0:
                    fIntensity = fBalance;
                    break;
                case 1:
                case 2:
                    fIntensity = 1.0F;
                    break;
                case 3:
                    fIntensity = 1.0F - fBalance;
                    break;
                default:
                    bDoColorLight = false;
                }

                break;

            default:
                break;
            }
        }

        // CORONAS
        auto bSkipCoronaChecks = false;
        if (CGameLogic::LaRiotsActiveHere()) {
            bool bLightsOn = bDoColorLight;
            bLightsOn &= !GetIsTypeVehicle();
            bLightsOn &= ((uiRand & 3) == 0 || (uiRand & 3) == 1 && (CTimer::GetTimeInMS() ^ (uiRand * 8)) & 0x60);

            if (bLightsOn) {
                bDoColorLight = false;
                bDoNoColorLight = true;
                bSkipCoronaChecks = true;

                CCoronas::RegisterCorona(
                    reinterpret_cast<uint32>(this) + iFxInd,
                    nullptr,
                    0,
                    0,
                    0,
                    255,
                    vecEffPos,
                    effect->light.m_fCoronaSize,
                    effect->light.m_fCoronaFarClip,
                    effect->light.m_pCoronaTex,
                    static_cast<eCoronaFlareType>(effect->light.m_nCoronaFlareType),
                    static_cast<eCoronaReflType>(effect->light.m_bCoronaEnableReflection),
                    static_cast<eCoronaLOSCheck>(effect->light.m_bCheckObstacles),
                    eCoronaTrail::TRAIL_OFF,
                    0.0F,
                    effect->light.m_bOnlyLongDistance,
                    1.5F,
                    0,
                    15.0F,
                    false,
                    false
                );
            }
        }

        if (!bSkipCoronaChecks && bDoColorLight) {
            auto bCanCreateLight = true;
            if (effect->light.m_bCheckDirection) {
                const auto& camPos = TheCamera.GetPosition();
                CVector     lightOffset{
                    static_cast<float>(effect->light.offsetX),
                    static_cast<float>(effect->light.offsetY),
                    static_cast<float>(effect->light.offsetZ)
                };
                auto vecLightPos = GetMatrix().TransformVector(lightOffset);

                auto fDot = DotProduct(vecLightPos, (camPos - vecEffPos));
                bCanCreateLight = fDot >= 0.0F;
            }

            if (bCanCreateLight) {
                bSkipCoronaChecks = true;
                auto fBrightness = fIntensity;
                if (effect->light.m_bBlinking1) {
                    fBrightness = (1.0F - (CGeneral::GetRandomNumber() % 32) * 0.012F) * fIntensity;
                }

                if (effect->light.m_bBlinking2 && (CTimer::GetFrameCounter() + uiRand) & 3) {
                    fBrightness = 0.0F;
                }

                if (effect->light.m_bBlinking3 && (CTimer::GetFrameCounter() + uiRand) & 0x3F) {
                    if (((CTimer::GetFrameCounter() + uiRand) & 0x3F) == 1) {
                        fBrightness *= 0.5F;
                    } else {
                        fBrightness = 0.0F;
                    }
                }

                auto fSizeMult = 1.0F;
                if (GetModelIndex() == MODEL_RCBARON) {
                    fBrightness *= 1.9F;
                    fSizeMult = 2.0F;
                }

                fIntensity = CTimeCycle::m_CurrentColours.m_fSpriteBrightness * fBrightness * 0.1F;
                auto fSize = effect->light.m_fCoronaSize * fSizeMult;

                auto ucRed = static_cast<uint8>(static_cast<float>(effect->light.m_color.red) * fIntensity);
                auto ucGreen = static_cast<uint8>(static_cast<float>(effect->light.m_color.green) * fIntensity);
                auto ucBlue = static_cast<uint8>(static_cast<float>(effect->light.m_color.blue) * fIntensity);

                CCoronas::RegisterCorona(
                    reinterpret_cast<uint32>(this) + iFxInd,
                    nullptr,
                    ucRed,
                    ucGreen,
                    ucBlue,
                    static_cast<uint8>(fDayNight * 255.0F),
                    vecEffPos,
                    fSize,
                    effect->light.m_fCoronaFarClip,
                    effect->light.m_pCoronaTex,
                    static_cast<eCoronaFlareType>(effect->light.m_nCoronaFlareType),
                    static_cast<eCoronaReflType>(effect->light.m_bCoronaEnableReflection),
                    static_cast<eCoronaLOSCheck>(effect->light.m_bCheckObstacles),
                    eCoronaTrail::TRAIL_OFF,
                    0.0F,
                    effect->light.m_bOnlyLongDistance,
                    0.8F,
                    0,
                    15.0F,
                    effect->light.m_bOnlyFromBelow,
                    effect->light.m_bUpdateHeightAboveGround
                );

            } else {
                bDoColorLight = false;
                bUpdateCoronaCoors = true;
            }
        }

        if (!bSkipCoronaChecks && bDoNoColorLight) {
            bSkipCoronaChecks = true;
            CCoronas::RegisterCorona(
                reinterpret_cast<uint32>(this) + iFxInd,
                nullptr,
                0,
                0,
                0,
                255,
                vecEffPos,
                effect->light.m_fCoronaSize,
                effect->light.m_fCoronaFarClip,
                effect->light.m_pCoronaTex,
                static_cast<eCoronaFlareType>(effect->light.m_nCoronaFlareType),
                static_cast<eCoronaReflType>(effect->light.m_bCoronaEnableReflection),
                static_cast<eCoronaLOSCheck>(effect->light.m_bCheckObstacles),
                eCoronaTrail::TRAIL_OFF,
                0.0F,
                effect->light.m_bOnlyLongDistance,
                1.5F,
                0,
                15.0F,
                false,
                false
            );
        }

        if (!bSkipCoronaChecks && bUpdateCoronaCoors) {
            CCoronas::UpdateCoronaCoors(reinterpret_cast<uint32>(this) + iFxInd, vecEffPos, effect->light.m_fCoronaFarClip, 0.0F);
        }

        // POINT LIGHTS
        bool bSkipLights = false;
        if (effect->light.m_fPointlightRange != 0.0F && bDoColorLight) {
            auto color = effect->light.m_color;
            if (color.red || color.green || color.blue) {
                auto fColorMult = fDayNight * fIntensity / 256.0F;

                bSkipLights = true;
                CPointLights::AddLight(
                    ePointLightType::PLTYPE_POINTLIGHT,
                    vecEffPos,
                    CVector(0.0F, 0.0F, 0.0F),
                    effect->light.m_fPointlightRange,
                    static_cast<float>(color.red) * fColorMult,
                    static_cast<float>(color.green) * fColorMult,
                    static_cast<float>(color.blue) * fColorMult,
                    effect->light.m_nFogType,
                    true,
                    nullptr
                );
            } else {
                CPointLights::AddLight(
                    ePointLightType::PLTYPE_DARKLIGHT,
                    vecEffPos,
                    CVector(0.0F, 0.0F, 0.0F),
                    effect->light.m_fPointlightRange,
                    0.0F,
                    0.0F,
                    0.0F,
                    RwFogType::rwFOGTYPENAFOGTYPE,
                    true,
                    nullptr
                );
            }
        }

        if (!bSkipLights) {
            if (effect->light.m_nFogType & RwFogType::rwFOGTYPEEXPONENTIAL) {
                auto color = effect->light.m_color;
                CPointLights::AddLight(
                    (ePointLightType)3u, // todo: Enum doesn't contain all types?
                    vecEffPos,
                    CVector(0.0F, 0.0F, 0.0F),
                    0.0F,
                    color.red / 256.0F,
                    color.green / 256.0F,
                    color.blue / 256.0F,
                    RwFogType::rwFOGTYPEEXPONENTIAL,
                    true,
                    nullptr
                );
            } else if (effect->light.m_nFogType & RwFogType::rwFOGTYPELINEAR && bDoColorLight && effect->light.m_fPointlightRange == 0.0F) {
                auto color = effect->light.m_color;
                CPointLights::AddLight(
                    (ePointLightType)4u, // todo: Enum doesn't contain all types?
                    vecEffPos,
                    CVector(0.0F, 0.0F, 0.0F),
                    0.0F,
                    color.red / 256.0F,
                    color.green / 256.0F,
                    color.blue / 256.0F,
                    RwFogType::rwFOGTYPELINEAR,
                    true,
                    nullptr
                );
            }
        }

        // SHADOWS
        if (effect->light.m_fShadowSize != 0.0F) {
            auto fShadowZ = 15.0F;
            if (effect->light.m_nShadowZDistance) {
                fShadowZ = static_cast<float>(effect->light.m_nShadowZDistance);
            }

            if (bDoColorLight) {
                auto color = effect->light.m_color;
                auto fColorMult = effect->light.m_nShadowColorMultiplier * fIntensity / 256.0F;
                color.red = static_cast<uint8>(static_cast<float>(color.red) * fColorMult);
                color.green = static_cast<uint8>(static_cast<float>(color.green) * fColorMult);
                color.blue = static_cast<uint8>(static_cast<float>(color.blue) * fColorMult);

                CShadows::StoreStaticShadow(
                    reinterpret_cast<uint32>(this) + iFxInd,
                    eShadowType::SHADOW_ADDITIVE,
                    effect->light.m_pShadowTex,
                    vecEffPos,
                    effect->light.m_fShadowSize,
                    0.0F,
                    0.0F,
                    -effect->light.m_fShadowSize,
                    128,
                    color.red,
                    color.green,
                    color.blue,
                    fShadowZ,
                    1.0F,
                    40.0F,
                    false,
                    0.0F
                );
            } else if (bDoNoColorLight) {
                CShadows::StoreStaticShadow(
                    reinterpret_cast<uint32>(this) + iFxInd,
                    eShadowType::SHADOW_ADDITIVE,
                    effect->light.m_pShadowTex,
                    vecEffPos,
                    effect->light.m_fShadowSize,
                    0.0F,
                    0.0F,
                    -effect->light.m_fShadowSize,
                    0,
                    0,
                    0,
                    0,
                    fShadowZ,
                    1.0F,
                    40.0F,
                    false,
                    0.0F
                );
            }
        }
    }
}

// 0x717900
void CEntity::RemoveEscalatorsForEntity() {
    for (auto& escalator : CEscalators::GetAllExists()) {
        if (escalator.GetEntity() == this) {
            escalator.Remove();
            return;
        }
    }
}

// 0x71FAE0
bool CEntity::IsEntityOccluded() {
    if (COcclusion::GetActiveOccluders().empty()) {
        return false;
    }

    CVector center;
    GetBoundCentre(center);

    CVector centerScrPos;
    float   scaleX, scaleY;
    if (!CalcScreenCoors(center, centerScrPos, scaleX, scaleY)) {
        return false;
    }

    auto* const         mi = CModelInfo::GetModelInfo(GetModelIndex());
    const CBoundingBox& bb = mi->GetColModel()->GetBoundingBox();

    const auto longEdge = std::max(scaleX, scaleY);
    const auto boundingRadius = mi->GetColModel()->GetBoundRadius();
    const auto occlusionRadius = boundingRadius * longEdge * 0.9f;

    const auto GetOccluderPt = [this](CVector pt) -> std::pair<CVector, std::optional<CVector>> {
        const auto ws = TransformFromObjectSpace(pt);
        if (CVector scr; CalcScreenCoors(ws, scr)) {
            return { ws, scr };
        }
        return { ws, std::nullopt };
    };
    const CVector min = bb.m_vecMin,
                  max = bb.m_vecMax;
    const std::array points{
        GetOccluderPt(min),
        GetOccluderPt(max),
        GetOccluderPt({ min.x, max.y, max.z }),
        GetOccluderPt({ max.x, min.y, min.z }),
        GetOccluderPt({ min.x, min.y, max.z }),
        GetOccluderPt({ max.x, min.y, max.z }),
    };

    return rng::any_of(COcclusion::GetActiveOccluders(), [&](const auto& o) -> bool {
        if (o.GetDistToCam() >= centerScrPos.z - boundingRadius) { // Inside the entity?
            return false;
        }
        if (o.IsPointWithinOcclusionArea(centerScrPos, occlusionRadius)) {
            if (o.IsPointBehindOccluder(center, boundingRadius)) {
                return true;
            }
        }
        if (!o.IsPointWithinOcclusionArea(centerScrPos)) {
            return false;
        }
        return rng::all_of(points, [&](const auto& pt) {
            const auto& [ws, scr] = pt; // World-space and screen-space positions
            if (!scr.has_value()) {
                return false;
            }
            return o.IsPointWithinOcclusionArea(*scr)
                && o.IsPointBehindOccluder(ws);
        });
    });

    // Original code for those interested
    // I did leave out a little portion, but the new code should be faster anyways
    //for (auto& o : COcclusion::GetActiveOccluders()) {
    //    if (o.GetDistToCam() >= scrPos.z - boundingRadius) {
    //        continue;
    //    }
    //
    //    if (o.IsPointWithinOcclusionArea(scrPos.x, scrPos.y, occlusionRadius)) {
    //        if (o.IsPointBehindOccluder(center, boundingRadius)) {
    //            return true;
    //        }
    //    }
    //
    //    if (!o.IsPointWithinOcclusionArea(scrPos.x, scrPos.y, 0.0F)) {
    //        continue;
    //    }
    //
    //    auto bInView = false;
    //    CVector vecScreen;
    //
    //    auto vecMin = GetMatrix().TransformPoint(bb.m_vecMin);
    //    if (!CalcScreenCoors(vecMin, vecScreen)
    //        || !o.IsPointWithinOcclusionArea(vecScreen.x, vecScreen.y, 0.0F)
    //        || !o.IsPointBehindOccluder(vecMin, 0.0F)
    //    ) {
    //        bInView = true;
    //    }
    //
    //    auto vecMax = GetMatrix().TransformPoint(bb.m_vecMax);
    //    if (bInView
    //        || !CalcScreenCoors(vecMax, vecScreen)
    //        || !o.IsPointWithinOcclusionArea(vecScreen.x, vecScreen.y, 0.0F)
    //        || !o.IsPointBehindOccluder(vecMax, 0.0F)
    //    ) {
    //        bInView = true;
    //    }
    //
    //    auto vecDiag1 = GetMatrix().TransformPoint(CVector(bb.m_vecMin.x, bb.m_vecMax.y, bb.m_vecMax.z));
    //    if (bInView
    //        || !CalcScreenCoors(vecDiag1, vecScreen)
    //        || !o.IsPointWithinOcclusionArea(vecScreen.x, vecScreen.y, 0.0F)
    //        || !o.IsPointBehindOccluder(vecDiag1, 0.0F)
    //    ) {
    //        bInView = true;
    //    }
    //
    //    auto vecDiag2 = GetMatrix().TransformPoint(CVector(bb.m_vecMax.x, bb.m_vecMin.y, bb.m_vecMin.z));
    //    if (!bInView
    //        && CalcScreenCoors(vecDiag2, vecScreen)
    //        && o.IsPointWithinOcclusionArea(vecScreen.x, vecScreen.y, 0.0F)
    //        && o.IsPointBehindOccluder(vecDiag2, 0.0F)
    //    ) {
    //        if (bb.GetWidth() <= 60.0F)
    //            return true;
    //
    //        if (bb.GetLength() <= 60.0F)
    //            return true;
    //
    //        if (bb.GetHeight() <= 30.0F)
    //            return true;
    //
    //        auto vecDiag3 = GetMatrix().TransformPoint(CVector(bb.m_vecMin.x, bb.m_vecMin.y, bb.m_vecMax.z));
    //        if (!CalcScreenCoors(vecDiag3, vecScreen)
    //            || !o.IsPointWithinOcclusionArea(vecScreen.x, vecScreen.y, 0.0F)
    //            || !o.IsPointBehindOccluder(vecDiag3, 0.0F)) {
    //
    //            bInView = true;
    //        }
    //
    //        auto vecDiag4 = GetMatrix().TransformPoint(CVector(bb.m_vecMax.x, bb.m_vecMin.y, bb.m_vecMax.z));
    //        if (!bInView
    //            && CalcScreenCoors(vecDiag4, vecScreen)
    //            && o.IsPointWithinOcclusionArea(vecScreen.x, vecScreen.y, 0.0F)
    //            && o.IsPointBehindOccluder(vecDiag4, 0.0F)
    //        ) {
    //            return true;
    //        }
    //    }
    //}
    //
    //return false;
}

inline RwMatrix* CEntity::GetRwMatrix() {
    return RwFrameGetMatrix(RwFrameGetParent(GetRwObject()));
}

inline bool CEntity::IsInArea(int32 area) {
    return GetAreaCode() == area || GetAreaCode() == AREA_CODE_13;
}

inline bool CEntity::IsInCurrentArea() const {
    return GetAreaCode() == CGame::currArea || GetAreaCode() == AREA_CODE_13;
}

// 0x446F90
void CEntity::UpdateRW() {
    if (!GetRwObject()) {
        return;
    }

    auto parentMatrix = GetModellingMatrix();
    if (m_matrix) {
        m_matrix->UpdateRwMatrix(parentMatrix);
    } else {
        m_placement.UpdateRwMatrix(parentMatrix);
    }
}

CEntity* CEntity::FindLastLOD() noexcept {
    CEntity* it = this;
    for (; it->m_pLod; it = it->m_pLod)
        ;
    return it;
}

// define?
CBaseModelInfo* CEntity::GetModelInfo() const {
    return CModelInfo::GetModelInfo(GetModelIndex());
}

// NOTSA
bool CEntity::ProcessScan() {
    if (IsScanCodeCurrent()) {
        return false;
    }
    SetCurrentScanCode();
    return true;
}

RpAtomic* CEntity::SetAtomicAlphaCB(RpAtomic* atomic, void* data) {
    auto geometry = RpAtomicGetGeometry(atomic);
    RpGeometrySetFlags(geometry, rpGEOMETRYMODULATEMATERIALCOLOR);
    RpGeometryForAllMaterials(geometry, SetMaterialAlphaCB, data);
    return atomic;
}

RpMaterial* CEntity::SetMaterialAlphaCB(RpMaterial* material, void* data) {
    RpMaterialGetColor(material)->alpha = (RwUInt8)std::bit_cast<uintptr_t>(data);
    return material;
}

// NOTSA
bool CEntity::IsScanCodeCurrent() const {
    return GetScanCode() == GetCurrentScanCode();
}

// NOTSA
void CEntity::SetCurrentScanCode() {
    SetScanCode(GetCurrentScanCode());
}

// 0x46A760
bool IsGlassModel(CEntity* entity) {
    if (!entity->GetIsTypeObject()) {
        return false;
    }

    auto mi = CModelInfo::GetModelInfo(entity->GetModelIndex());
    if (!mi->AsAtomicModelInfoPtr()) {
        return false;
    }

    return mi->IsGlass();
}
