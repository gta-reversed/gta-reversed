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

    RH_ScopedInstall(Constructor, 0x532A90);
    RH_ScopedInstall(Destructor, 0x535E90);

    //RH_ScopedOverloadedInstall(Add, "void", 0x533020, void(CEntity::*)());
    RH_ScopedOverloadedInstall(Add, "rect", 0x5347D0, void(CEntity::*)(const CRect&));
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
    RH_ScopedOverloadedInstall(GetBoundCentre, "vec", 0x534250, CVector(CEntity::*)() const);
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
    RH_ScopedInstall(UpdateRwMatrix, 0x446F90);

    RH_ScopedInstall(GetIsTypePhysical, 0x4DA030);
    RH_ScopedInstall(GetIsStatic, 0x4633E0);
    RH_ScopedInstall(TreatAsPlayerForCollisions, 0x541F70);

    RH_ScopedGlobalInstall(SetAtomicAlpha, 0x533290);
    RH_ScopedGlobalInstall(SetCompAlphaCB, 0x533280);
    RH_ScopedGlobalInstall(MaterialUpdateUVAnimCB, 0x532D70);
    RH_ScopedGlobalInstall(IsEntityPointerValid, 0x533310);
}

// Initializes a new CEntity instance with default values
// 0x532A90
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
    m_NumLodChildren = 0;
    ResetLodChildrenRendered();
    SetLod(nullptr);
}

// Destructor for CEntity, handling cleanup of LOD children,
// RenderWare objects, and resolving any references to this entity
// 0x535E90
CEntity::~CEntity() {
    if (GetLod()) {
        GetLod()->RemoveLodChildren();
    }

    CEntity::DeleteRwObject();
    CEntity::ResolveReferences();
}

// Adds the entity to the world sectors based on its current bounding rectangle
// 0x533020
void CEntity::Add() {
    Add(GetBoundRect());
}

// Adds the entity to the appropriate world sectors based on a given rectangle
// The entity is added to LOD sectors if it's a big building, otherwise to regular sectors
// 0x5347D0
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
            auto& s = CWorld::GetSector(x, y);
            auto& rs = CWorld::GetRepeatSector(x, y);
            const auto  ProcessAddItem = [this]<typename PtrListType>(PtrListType list) {
                list.AddItem(static_cast<typename PtrListType::ItemType>(this)); // TODO: notsa::cast
            };
            switch (GetType()) {
            case ENTITY_TYPE_DUMMY:    ProcessAddItem(s.Dummies); break;
            case ENTITY_TYPE_VEHICLE:  ProcessAddItem(rs.Vehicles); break;
            case ENTITY_TYPE_PED:      ProcessAddItem(rs.Peds); break;
            case ENTITY_TYPE_OBJECT:   ProcessAddItem(rs.Objects); break;
            case ENTITY_TYPE_BUILDING: ProcessAddItem(s.Buildings); break;
            }
            return true;
        });
    }
}

// Removes the entity from the world sectors it occupies
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
            auto& s = CWorld::GetSector(x, y);
            auto& rs = CWorld::GetRepeatSector(x, y);
            const auto  ProcessDeleteItem = [this]<typename PtrListType>(PtrListType list) {
                list.DeleteItem(static_cast<typename PtrListType::ItemType>(this)); // TODO: notsa::cast
            };
            switch (GetType()) {
            case ENTITY_TYPE_DUMMY:    ProcessDeleteItem(s.Dummies); break;
            case ENTITY_TYPE_VEHICLE:  ProcessDeleteItem(rs.Vehicles); break;
            case ENTITY_TYPE_PED:      ProcessDeleteItem(rs.Peds); break;
            case ENTITY_TYPE_OBJECT:   ProcessDeleteItem(rs.Objects); break;
            case ENTITY_TYPE_BUILDING: ProcessDeleteItem(s.Buildings); break;
            }
            return true;
        });
    }
}

// Sets the entity's model by index and then creates its RenderWare object
// 0x532AE0
void CEntity::SetModelIndex(uint32 index) {
    CEntity::SetModelIndexNoCreate(index);
    CEntity::CreateRwObject();
}

// Sets the entity's model by index without creating its RenderWare object
// Also updates entity flags based on the new model's properties
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

// Creates the RenderWare object (RpAtomic or RpClump) for the entity
// This function handles model instantiation, streaming, and effect creation
// 0x533D30
void CEntity::CreateRwObject() {
    if (!GetIsVisible()) {
        return;
    }

    auto* mi = GetModelInfo();

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
    UpdateRwMatrix();

    // Handle different RenderWare object types
    switch (RwObjectGetType(GetRwObject())) {
    case rpATOMIC:
        if (CTagManager::IsTag(this)) {
            CTagManager::ResetAlpha(this);
        }
        CCustomBuildingDNPipeline::PreRenderUpdate(m_pRwAtomic, true);
        break;
    case rpCLUMP:
        if (mi->bIsRoad) {
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
        break;
    }

    // Finalize object setup
    mi->AddRef();
    m_pStreamingLink = CStreaming::AddEntity(this);
    CreateEffects();

    // Determine lighting requirement
    auto* usedAtomic = RwObjectGetType(GetRwObject()) == rpATOMIC ? m_pRwAtomic : GetFirstAtomic(m_pRwClump);

    if (!CCustomBuildingRenderer::IsCBPCPipelineAttached(usedAtomic)) {
        m_bLightObject = true;
    }
}

// Destroys the entity's RenderWare object, removing it from the world
// It also cleans up streaming links, model references, and associated effects
// 0x534030
void CEntity::DeleteRwObject() {
    if (!GetRwObject()) {
        return;
    }

    switch (RwObjectGetType(GetRwObject())) {
    case rpATOMIC:
        if (auto* frame = RpAtomicGetFrame(m_pRwAtomic)) {
            RpAtomicDestroy(m_pRwAtomic);
            RwFrameDestroy(frame);
        }
        break;
    case rpCLUMP:
#ifdef SA_SKINNED_PEDS
        if (IsClumpSkinned(m_pRwClump)) {
            RpClumpForAllAtomics(m_pRwClump, AtomicRemoveAnimFromSkinCB, nullptr);
        }
#endif
        RpClumpDestroy(m_pRwClump);
        break;
    }

    m_pRwObject = nullptr;
    auto* mi = GetModelInfo();
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

// Calculates the 2D bounding rectangle of the entity in world space
// by transforming the corners of its collision model's bounding box
// 0x534120
CRect CEntity::GetBoundRect() const {
    auto* colModel = GetModelInfo()->GetColModel();
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

// Performs pre-rendering tasks for the entity, such as processing lights,
// updating model animations, and applying special effects like wind on trees or pickup effects
// 0x535FA0
void CEntity::PreRender() {
    const auto mi = GetModelInfo();
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
            UpdateRwMatrix();
            UpdateRwFrame();
        } else if (modelIndex == ModelIndices::MI_MONEY) {
            CPickups::DoMoneyEffects(this);
            UpdateRwMatrix();
            UpdateRwFrame();
        } else if (modelIndex == ModelIndices::MI_CARMINE
                   || modelIndex == ModelIndices::MI_NAUTICALMINE
                   || modelIndex == ModelIndices::MI_BRIEFCASE) {
            if (obj->objectFlags.bIsPickup) {
                CPickups::DoMineEffects(this);
                UpdateRwMatrix();
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
                    (uint32)this, nullptr,
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
                (uint32)this, nullptr,
                static_cast<uint8>(fRand * 255.0F),
                static_cast<uint8>(fRand * 255.0F),
                static_cast<uint8>(fRand * 255.0F), 255, vecPos,
                fRand * 6.0F, 300.0F, gpCoronaTexture[CORONATYPE_SHINYSTAR],
                eCoronaFlareType::FLARETYPE_NONE, eCoronaReflType::CORREFL_SIMPLE,
                eCoronaLOSCheck::LOSCHECK_OFF, eCoronaTrail::TRAIL_OFF,
                0.0F, false, 1.5F, 0, 15.0F, false, false
            );
        } else if (CGlass::IsObjectGlass(this)) {
            PreRenderForGlassWindow();
        } else if (obj->objectFlags.bIsPickup) {
            CPickups::DoPickUpEffects(this);
            UpdateRwMatrix();
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
        } else if (modelIndex == ModelIndices::MI_MAGNOCRANE_HOOK
                   || modelIndex == ModelIndices::MI_WRECKING_BALL
                   || modelIndex == ModelIndices::MI_CRANE_MAGNET
                   || modelIndex == ModelIndices::MI_MINI_MAGNET
                   || modelIndex == ModelIndices::MI_CRANE_HARNESS) {
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

// Renders the entity's RenderWare object if it exists and is visible
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

// Sets up lighting for the entity by activating directional lights
// and calculating the contribution from nearby point lights
// in Renderer.cpp
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

// Resets the entity's lighting to the default ambient and directional colors
// in Renderer.cpp
// 0x553370
void CEntity::RemoveLighting(bool reset) {
    if (reset) {
        SetAmbientColours();
        DeActivateDirectional();
        CPointLights::RemoveLightsAffectingObject();
    }
}

// Updates the matrices of the RenderWare object's frame hierarchy
// 0x532B00
void CEntity::UpdateRwFrame() {
    if (GetRwObject()) {
        RwFrameUpdateObjects(static_cast<RwFrame*>(rwObjectGetParent(GetRwObject())));
    }
}

// Updates the bone matrices for a skinned (animated) clump
// 0x532B20
void CEntity::UpdateRpHAnim() {
    if (const auto atomic = GetFirstAtomic(m_pRwClump)) {
        if (RpSkinGeometryGetSkin(RpAtomicGetGeometry(atomic)) && !m_bDontUpdateHierarchy) {
            RpHAnimHierarchyUpdateMatrices(GetAnimHierarchyFromSkinClump(m_pRwClump));
        }
    }
}

// Checks if the entity model has effects that need to be processed
// during the pre-render stage, such as wind effects, glass, or pickups
// 0x532B70
bool CEntity::HasPreRenderEffects() {
    const auto modelIndex = GetModelIndex();
    auto mi = CModelInfo::GetModelInfo(modelIndex);

    if (mi->SwaysInWind() || mi->IsCrane()) {
        return true;
    }

    // Check for special models
    if (notsa::contains<eModelID>(
            { // dynamic.ide
              ModelIndices::MI_COLLECTABLE1,
              ModelIndices::MI_MONEY,
              ModelIndices::MI_CARMINE,
              ModelIndices::MI_NAUTICALMINE,
              ModelIndices::MI_BRIEFCASE,

              // multiobj.ide
              ModelIndices::MI_BEACHBALL,

              ModelIndices::MI_MAGNOCRANE_HOOK,

              // levelxre.ide
              ModelIndices::MI_WRECKING_BALL,
              ModelIndices::MI_CRANE_MAGNET,
              ModelIndices::MI_MINI_MAGNET,

              ModelIndices::MI_CRANE_HARNESS,

              // countn2.ide
              ModelIndices::MI_WINDSOCK,

              // defauld.ide
              ModelIndices::MI_FLARE },
            GetModelId()
        )) {
        return true;
    }

    // fixed model IDs, in defauld.ide
    if (notsa::contains({MODEL_MISSILE, MODEL_GRENADE, MODEL_MOLOTOV }, GetModelId())) {
        return true;
    }

    if (CGlass::IsObjectGlass(this)) {
        return true;
    }

    // Checking for a pickup object
    if (GetIsTypeObject() && AsObject()->objectFlags.bIsPickup) {
        return true;
    }

    // Checking traffic lights
    if (CTrafficLights::IsMITrafficLight(modelIndex)) {
        return true;
    }

    // Checking street lights
    if (notsa::contains<eModelID>(
            { ModelIndices::MI_SINGLESTREETLIGHTS1,
              ModelIndices::MI_SINGLESTREETLIGHTS2,
              ModelIndices::MI_SINGLESTREETLIGHTS3,
              ModelIndices::MI_DOUBLESTREETLIGHTS },
            GetModelId()
        )) {
        return true;
    }

    // Checking for 2D light effects
    const int32 numEffects = mi->m_n2dfxCount;
    if (!numEffects) {
        return false;
    }

    for (int32 i = 0; i < numEffects; ++i) {
        if (mi->Get2dEffect(i)->m_Type == e2dEffectType::EFFECT_LIGHT) {
            return true;
        }
    }

    return false;
}

// Determines if the entity should be ignored by flying vehicles during collision checks
// 0x532D40
bool CEntity::DoesNotCollideWithFlyers() {
    auto mi = GetModelInfo();
    return mi->SwaysInWind() || mi->bDontCollideWithFlyer;
}

// RenderWare callback to update UV animations for a material
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

// Calculates and applies a rotation matrix to a windsock entity
// to make it point in the direction of the current wind
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

    UpdateRwMatrix();
    UpdateRwFrame();
}

// Determines if the entity's center point is located within the given world sector
// 0x533050
bool CEntity::LivesInThisNonOverlapSector(int32 x, int32 y) {
    const auto rect = GetBoundRect();

    float centerX, centerY;
    rect.GetCenter(&centerX, &centerY);

    const auto middleX = CWorld::GetSectorX(centerX);
    const auto middleY = CWorld::GetSectorY(centerY);

    return x == middleX && y == middleY;
}

// Configures the entity as a "big building" for LOD streaming purposes,
// disabling its collision and making it persistent
// 0x533150
void CEntity::SetupBigBuilding() {
    SetUsesCollision(false);
    m_bIsBIGBuilding = true;
    m_bStreamingDontDelete = true;
    GetModelInfo()->SetOwnsColModel(true);
}

// Rotates the entity's matrix around the Z-axis over time
// This is used for the spinning part of a crane
// 0x533170
void CEntity::ModifyMatrixForCrane() {
    if (!CTimer::GetIsPaused()) {
        if (GetRwObject()) {
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
    }
}

// Handles pre-rendering logic for glass objects by registering
// them with the glass manager and temporarily hiding them
// 0x533240
void CEntity::PreRenderForGlassWindow() {
    if (!GetModelInfo()->IsGlassType2()) {
        CGlass::AskForObjectToBeRenderedInGlass(this);
        SetIsVisible(false);
    }
}

// Sets the alpha transparency for all materials of the entity's RenderWare object
// 0x5332C0
void CEntity::SetRwObjectAlpha(int32 alpha) {
    if (!GetRwObject()) {
        return;
    }

    switch (RwObjectGetType(GetRwObject())) {
    case rpATOMIC:
        RpGeometryForAllMaterials(RpAtomicGetGeometry(m_pRwAtomic), SetCompAlphaCB, (void*)alpha);
        break;
    case rpCLUMP:
        RpClumpForAllAtomics(m_pRwClump, SetAtomicAlpha, (void*)alpha);
        break;
    }
}

// Checks if an entity pointer is valid by verifying it's within the
// memory pool for its respective type (e.g., building, ped, vehicle)
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

// Finds the world coordinates of a 2dEffect trigger point on the entity's model by its index
// 0x533380
CVector* CEntity::FindTriggerPointCoors(CVector* outVec, int32 index) {
    auto mi = GetModelInfo();
    const int32 numEffects = mi->m_n2dfxCount;
    if (numEffects) {
        for (int32 iFxInd = 0; iFxInd < numEffects; ++iFxInd) {
            auto effect = mi->Get2dEffect(iFxInd);
            if (effect->m_Type == e2dEffectType::EFFECT_TRIGGER_POINT && effect->slotMachineIndex.m_nId == index) {
                *outVec = GetMatrix().TransformPoint(effect->m_Pos);
                return outVec;
            }
        }
    }

    outVec->Reset();
    return outVec;
}

// Finds a random 2dEffect of a specific type attached to the entity's model,
// optionally only considering effects with free slots (for ped attractors)
// 0x533410
C2dEffect* CEntity::GetRandom2dEffect(int32 effectType, bool mustBeFree) {
    auto mi = GetModelInfo();

    std::array<C2dEffect*, 32> candidates{};
    size_t numCandidates = 0;

    const int32 numEffects = mi->m_n2dfxCount;
    if (numEffects) {
        for (int32 iFxInd = 0; iFxInd < numEffects; ++iFxInd) {
            auto effect = mi->Get2dEffect(iFxInd);

            if (effect->m_Type != effectType) {
                continue;
            }

            if (mustBeFree && !GetPedAttractorManager()->HasEmptySlot(notsa::cast<C2dEffectPedAttractor>(effect), this)) {
                continue;
            }

            if (numCandidates < candidates.size()) {
                candidates[numCandidates++] = effect;
            }
        }
    }

    return numCandidates ? candidates[CGeneral::GetRandomNumberInRange(0u, numCandidates)] : nullptr;
}

// Transforms a point from the entity's local (object) space to world space
// PC Only
// 0x5334F0
CVector CEntity::TransformFromObjectSpace(const CVector& offset) const {
    if (m_matrix) {
        return m_matrix->TransformPoint(offset);
    }
    CVector result;
    TransformPoint(result, m_placement, offset);
    return result;
}

// Transforms a point from the entity's local (object) space to world space,
// storing the result in an output parameter
// PC Only
// 0x533560
CVector* CEntity::TransformFromObjectSpace(CVector& outPos, const CVector& offset) const {
    outPos = TransformFromObjectSpace(offset);
    return &outPos;
}

// Creates all 2dEffects (e.g., lights, particles, entry/exits)
// associated with the entity's model when the entity is created
// 0x533790
void CEntity::CreateEffects() {
    m_bHasRoadsignText = false;
    const auto* const mi = GetModelInfo();

    const int32 numEffects = mi->m_n2dfxCount;
    if (!numEffects) {
        return;
    }

    for (int32 iFxInd = 0; iFxInd < numEffects; ++iFxInd) {
        auto* effect = mi->Get2dEffect(iFxInd);

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
            const auto vecExit = effect->m_Pos + effect->enEx.m_vecExitPosn;
            const auto vecWorldEffect = TransformFromObjectSpace(effect->m_Pos);
            const auto vecWorldExit = TransformFromObjectSpace(vecExit);

            if (effect->enEx.bTimedEffect) {
                auto ucDays = CClock::GetGameClockDays();
                if (effect->enEx.m_nTimeOn > effect->enEx.m_nTimeOff && CClock::ms_nGameClockHours < effect->enEx.m_nTimeOff) {
                    ucDays--;
                }
                srand(std::bit_cast<uint32>(this) + ucDays);
            }

            const auto fHeading = GetHeading();
            const auto fExitRot = effect->enEx.m_fExitAngle + RadiansToDegrees(fHeading);
            const auto fEnterRot = effect->enEx.m_fEnterAngle + RadiansToDegrees(fHeading);
            const auto iEnExId = CEntryExitManager::AddOne(
                vecWorldEffect.x, vecWorldEffect.y, vecWorldEffect.z,
                fEnterRot,
                effect->enEx.m_vecRadius.x, effect->enEx.m_vecRadius.y,
                0,
                vecWorldExit.x, vecWorldExit.y, vecWorldExit.z,
                fExitRot,
                effect->enEx.m_nInteriorId,
                static_cast<CEntryExit::eFlags>(effect->enEx.m_nFlags1 + (effect->enEx.m_nFlags2 << 8)),
                effect->enEx.m_nSkyColor, effect->enEx.m_nTimeOn, effect->enEx.m_nTimeOff, 0,
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
            const auto uiPalleteId = C2dEffect::Roadsign_GetPaletteIDFromFlags(effect->roadsign.m_nFlags);
            const auto uiLettersPerLine = C2dEffect::Roadsign_GetNumLettersFromFlags(effect->roadsign.m_nFlags);
            const auto uiNumLines = C2dEffect::Roadsign_GetNumLinesFromFlags(effect->roadsign.m_nFlags);

            const auto signAtomic = CCustomRoadsignMgr::CreateRoadsignAtomic(
                effect->roadsign.m_vecSize.x, effect->roadsign.m_vecSize.y, uiNumLines,
                &effect->roadsign.m_pText[0],
                &effect->roadsign.m_pText[16],
                &effect->roadsign.m_pText[32],
                &effect->roadsign.m_pText[48],
                uiLettersPerLine, uiPalleteId
            );

            const auto frame = RpAtomicGetFrame(signAtomic);
            RwFrameSetIdentity(frame);

            static const RwV3d axisX{ 1.0F, 0.0F, 0.0F };
            static const RwV3d axisY{ 0.0F, 1.0F, 0.0F };
            static const RwV3d axisZ{ 0.0F, 0.0F, 1.0F };

            RwFrameRotate(frame, &axisZ, effect->roadsign.m_vecRotation.z, RwOpCombineType::rwCOMBINEREPLACE);
            RwFrameRotate(frame, &axisX, effect->roadsign.m_vecRotation.x, RwOpCombineType::rwCOMBINEPOSTCONCAT);
            RwFrameRotate(frame, &axisY, effect->roadsign.m_vecRotation.y, RwOpCombineType::rwCOMBINEPOSTCONCAT);
            RwFrameTranslate(frame, &effect->m_Pos, RwOpCombineType::rwCOMBINEPOSTCONCAT);
            RwFrameUpdateObjects(frame);

            effect->roadsign.m_pAtomic = signAtomic;
            break;
        }
        case e2dEffectType::EFFECT_ESCALATOR: {
            const auto vecStart = TransformFromObjectSpace(effect->m_Pos);
            const auto vecBottom = TransformFromObjectSpace(effect->escalator.m_vecBottom);
            const auto vecTop = TransformFromObjectSpace(effect->escalator.m_vecTop);
            const auto vecEnd = TransformFromObjectSpace(effect->escalator.m_vecEnd);
            const auto bMovingDown = effect->escalator.m_nDirection == 0;

            CEscalators::AddOne(vecStart, vecBottom, vecTop, vecEnd, bMovingDown, this);
            break;
        }
        }
    }
}

// Destroys all 2dEffects associated with the entity,
// cleaning up particles, entry/exits, and other special effects
// 0x533BF0
void CEntity::DestroyEffects() {
    auto mi = GetModelInfo();
    const int32 numEffects = mi->m_n2dfxCount;
    if (!numEffects) {
        return;
    }

    for (int32 iFxInd = 0; iFxInd < numEffects; ++iFxInd) {
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
            C2dEffect::DestroyAtomic(std::exchange(effect->roadsign.m_pAtomic, nullptr));
            break;
        }
        case e2dEffectType::EFFECT_ENEX: {
            const auto vecWorld = TransformFromObjectSpace(effect->m_Pos);
            const auto iNearestEnex = CEntryExitManager::FindNearestEntryExit(vecWorld, 1.5F, -1);

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

// Attaches an existing RenderWare object to this entity
// This is used for objects that share a single RwObject, like LODs
// 0x533ED0
void CEntity::AttachToRwObject(RwObject* object, bool updateMatrix) {
    if (!GetIsVisible()) {
        return;
    }

    m_pRwObject = object;
    if (!GetRwObject()) {
        return;
    }

    if (updateMatrix) {
        GetMatrix().UpdateMatrix(GetModellingMatrix());
    }

    auto* mi = GetModelInfo();

    if (RwObjectGetType(GetRwObject()) == rpCLUMP && mi->IsRoad()) {
        if (GetIsTypeObject()) {
            AsObject()->AddToMovingList();
            SetIsStatic(false);
        } else {
            CWorld::ms_listMovingEntityPtrs.AddItem(AsPhysical());
        }
    }

    mi->AddRef();
    m_pStreamingLink = CStreaming::AddEntity(this);
    CreateEffects();
}

// Detaches the RenderWare object from the entity, cleaning up references
// and effects without destroying the RwObject itself
// 0x533FB0
void CEntity::DetachFromRwObject() {
    if (!GetRwObject()) {
        return;
    }

    auto* mi = GetModelInfo();
    mi->RemoveRef();

    CStreaming::RemoveEntity(m_pStreamingLink);
    m_pStreamingLink = nullptr;

    if (mi->GetModelType() == MODEL_INFO_CLUMP && mi->IsRoad() && !GetIsTypeObject()) {
        CWorld::ms_listMovingEntityPtrs.DeleteItem(AsPhysical());
    }

    DestroyEffects();
    m_pRwObject = nullptr;
}

// Gets the center of the entity's collision bounding sphere in world coordinates
// 0x534290
void CEntity::GetBoundCentre(CVector& centre) const {
    TransformFromObjectSpace(centre, GetModelInfo()->GetColModel()->GetBoundCenter());
}

// Gets the center of the entity's collision bounding sphere in world coordinates
// 0x534250
CVector CEntity::GetBoundCentre() const {
    CVector result;
    TransformFromObjectSpace(result, GetModelInfo()->GetColModel()->GetBoundCenter());
    return result;
}

// Renders special 2dEffects associated with the entity, such as roadsign text
// 0x5342B0
void CEntity::RenderEffects() {
    if (!m_bHasRoadsignText) {
        return;
    }

    auto* mi = GetModelInfo();
    const int32 numEffects = mi->m_n2dfxCount;
    if (!numEffects) {
        return;
    }

    for (int32 iFxInd = 0; iFxInd < numEffects; ++iFxInd) {
        auto effect = mi->Get2dEffect(iFxInd);
        if (effect->m_Type != e2dEffectType::EFFECT_ROADSIGN) {
            continue;
        }

        CCustomRoadsignMgr::RenderRoadsignAtomic(effect->roadsign.m_pAtomic, TheCamera.GetPosition());
    }
}

// Checks if the bounding sphere of this entity intersects with another entity's bounding sphere
// 0x5343F0
bool CEntity::GetIsTouching(CEntity* entity) const {
    CVector centreA;
    CVector centreB;

    GetBoundCentre(centreA);
    entity->GetBoundCentre(centreB);

    const auto radius = GetBoundRadius() + entity->GetBoundRadius();

    return (centreA - centreB).SquaredMagnitude() < sq(radius);
}

// Checks if the bounding sphere of this entity intersects with a given sphere
// 0x5344B0
bool CEntity::GetIsTouching(const CVector& centre, float radius) const {
    CVector centreB;

    GetBoundCentre(centreB);
    float totalRadius = GetBoundRadius() + radius;

    return (centreB - centre).SquaredMagnitude() < sq(totalRadius);
}

// Checks if the entity's bounding sphere is visible within the camera's view frustum
// 0x534540
bool CEntity::GetIsOnScreen() {
    return TheCamera.IsSphereVisible(GetBoundCentre(), GetBoundRadius());
}

// Performs a more accurate check to see if the entity's oriented bounding box is on screen
// 0x5345D0
bool CEntity::GetIsBoundingBoxOnScreen() {
    auto cm = GetModelInfo()->GetColModel();

    std::array<RwV3d, 2> vecNormals;

    if (m_matrix) {
        auto tempMat = CMatrix();
        Invert(*m_matrix, tempMat);
        TransformVectors(vecNormals.data(), 2, tempMat, &TheCamera.m_avecFrustumWorldNormals[0]);
    } else {
        auto tempTrans = CSimpleTransform();
        tempTrans.Invert(m_placement);
        TransformVectors(vecNormals.data(), 2, tempTrans, &TheCamera.m_avecFrustumWorldNormals[0]);
    }

    for (int32 i = 0; i < 2; ++i) {
        auto chooseComponent = [&](auto member_ptr) {
            return std::invoke(member_ptr, vecNormals[i]) < 0
                ? std::invoke(member_ptr, cm->m_boundBox.m_vecMax)
                : std::invoke(member_ptr, cm->m_boundBox.m_vecMin);
        };

        CVector worldBBPoint = TransformFromObjectSpace(CVector{
            chooseComponent(&CVector::x),
            chooseComponent(&CVector::y),
            chooseComponent(&CVector::z)
        });

        // Check whether the point is outside the main frustum
        if (worldBBPoint.Dot(TheCamera.m_avecFrustumWorldNormals[i]) > TheCamera.m_fFrustumPlaneOffsets[i]) {
            // If the mirror is active and the point is inside the mirror frustum, skip
            if (TheCamera.m_bMirrorActive) {
                if (worldBBPoint.Dot(TheCamera.m_avecFrustumWorldNormals_Mirror[i]) <= TheCamera.m_fFrustumPlaneOffsets_Mirror[i]) {
                    continue;
                }
            }
            // Point outside the screen
            ++numBBFailed;
            return false;
        }
    }
    return true;
}

// Modifies the entity's matrix to simulate swaying in the wind,
// typically used for trees and other vegetation
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
    if (GetModelInfo()->IsSwayInWind2()) {
        at->x += CWeather::Wind * 0.03F;
    }

    at->y = at->x;
    at->x *= CWeather::WindDir.x;
    at->y *= CWeather::WindDir.y;

    CWindModifiers::FindWindModifier(GetPosition(), &at->x, &at->y);
    UpdateRwFrame();
}

// Modifies the entity's matrix to simulate a banner flapping in the wind
// This function is unused in the final game
// unused
// 0x535040
void CEntity::ModifyMatrixForBannerInWind() {
    if (CTimer::GetIsPaused()) {
        return;
    }

    auto vecPos = CVector2D(GetPosition());
    auto uiOffset = static_cast<uint16>(16 * (CTimer::GetTimeInMS() + (static_cast<uint16>(vecPos.x + vecPos.y) * 64)));

    float wind;
    if (CWeather::Wind < 0.1f) {
        wind = 0.2f;
    } else if (CWeather::Wind < 0.8f) {
        wind = 0.43f;
    } else {
        wind = 0.66f;
    }

    auto   fContrib = static_cast<float>(uiOffset & 0x7FF) / 2048.0F;
    uint32 uiIndex = uiOffset / 2'048;
    auto   fWindOffset = (1.0F - fContrib) * CWeather::saBannerWindOffsets[uiIndex];
    fWindOffset += fContrib * CWeather::saBannerWindOffsets[(uiIndex + 1) & 0x1F];
    fWindOffset *= CWeather::Wind;

    auto fZPos = sqrt(1.0F - pow(fWindOffset, 2.0F));

    CMatrix& matrix = GetMatrix();
    auto vecCross = matrix.GetForward().Cross(matrix.GetUp());
    vecCross.z = 0.0F;
    vecCross.Normalise();

    auto vecWind = CVector(vecCross * fWindOffset, fZPos);
    auto vecCross2 = matrix.GetForward().Cross(vecWind);

    matrix.GetRight() = vecCross2;
    matrix.GetUp() = vecWind;

    UpdateRwMatrix();
    UpdateRwFrame();
}

// Gets the RenderWare matrix associated with the entity's RwObject frame
// 0x46A2D0
RwMatrix* CEntity::GetModellingMatrix() {
    if (!GetRwObject()) {
        return nullptr;
    }

    return GetRwMatrix();
}

// Gets the collision model for the entity
// For vehicles, this can return a special collision model if one is active
// 0x535300
CColModel* CEntity::GetColModel() const {
    if (GetIsTypeVehicle()) {
        const auto veh = AsVehicle();
        if (veh->m_vehicleSpecialColIndex > -1) {
            return &CVehicle::m_aSpecialColModel[veh->m_vehicleSpecialColIndex];
        }
    }

    return GetModelInfo()->GetColModel();
}

// Calculates the four corner points of the 2D projection of the entity's
// oriented bounding box onto the XY plane
// https://gamedev.stackexchange.com/a/35948
// https://gamedev.stackexchange.com/questions/153326/how-to-rotate-directional-billboard-particle-sprites-toward-the-direction-the-pa/153814#153814
// 0x535340
void CEntity::CalculateBBProjection(CVector* point1, CVector* point2, CVector* point3, CVector* point4) {
    CMatrix& matrix = GetMatrix();

    // Extract local axes from the matrix
    const auto& [right, forward, up] = std::make_tuple(
        matrix.GetRight(),
        matrix.GetForward(),
        matrix.GetUp()
    );

    // Calculate the lengths of the projections of the object's local axes. This allows for scaling to be taken into account,
    // since when scaling is present, the lengths of the right, forward, and up vectors will differ from 1
    const auto [lengthProjX, lengthProjY, lengthProjZ] = std::make_tuple(
        right.Magnitude(),
        forward.Magnitude(),
        up.Magnitude()
    );

    // Get bounding box
    const auto cm = GetModelInfo()->GetColModel();
    const auto [sizeBBX, sizeBBY, sizeBBZ] = std::make_tuple(
        std::max(-cm->m_boundBox.m_vecMin.x, cm->m_boundBox.m_vecMax.x),
        std::max(-cm->m_boundBox.m_vecMin.y, cm->m_boundBox.m_vecMax.y),
        std::max(-cm->m_boundBox.m_vecMin.z, cm->m_boundBox.m_vecMax.z)
    );

    // Calculate sizes
    const auto [totalSizeX, totalSizeY, totalSizeZ] = std::make_tuple(
        sizeBBX * lengthProjX * 2.0F,
        sizeBBY * lengthProjY * 2.0F,
        sizeBBZ * lengthProjZ * 2.0F
    );

    const auto& pos = GetPosition();

    float offsetLength1, offsetWidth1, offsetLength;
    float offsetLength2, offsetWidth2, offsetWidth;
    CVector main;
    CVector higherPoint, lowerPoint;

    // Determine dominant axis and compute direction vectors
    if (totalSizeX > totalSizeY && totalSizeX > totalSizeZ) {
        main = CVector(right.x, right.y, 0.0F);
        higherPoint = pos + main * sizeBBX;
        lowerPoint = pos - main * sizeBBX;

        main.Normalise();
        offsetLength1 = (main.x * forward.x + main.y * forward.y) * sizeBBY;
        offsetWidth1  = (main.x * forward.y - main.y * forward.x) * sizeBBY;
        offsetLength2 = (main.x * up.x      + main.y * up.y)      * sizeBBZ;
        offsetWidth2  = (main.x * up.y      - main.y * up.x)      * sizeBBZ;
    } else if (totalSizeY > totalSizeZ) {
        main = CVector(forward.x, forward.y, 0.0F);
        higherPoint = pos + main * sizeBBY;
        lowerPoint = pos - main * sizeBBY;

        main.Normalise();
        offsetLength1 = (main.x * right.x + main.y * right.y) * sizeBBX;
        offsetWidth1  = (main.x * right.y - main.y * right.x) * sizeBBX;
        offsetLength2 = (main.x * up.x    + main.y * up.y)    * sizeBBZ;
        offsetWidth2  = (main.x * up.y    - main.y * up.x)    * sizeBBZ;
    } else {
        main = CVector(up.x, up.y, 0.0F);
        higherPoint = pos + main * sizeBBZ;
        lowerPoint = pos - main * sizeBBZ;

        main.Normalise();
        offsetLength1 = (main.x * right.x   + main.y * right.y)   * sizeBBX;
        offsetWidth1  = (main.x * right.y   - main.y * right.x)   * sizeBBX;
        offsetLength2 = (main.x * forward.x + main.y * forward.y) * sizeBBY;
        offsetWidth2  = (main.x * forward.y - main.y * forward.x) * sizeBBY;
    }

    // Compute absolute values and sums
    offsetLength = std::fabs(offsetLength1) + std::fabs(offsetLength2);
    offsetWidth  = std::fabs(offsetWidth1)  + std::fabs(offsetWidth2);

    // Helper lambda for corner calculation
    const auto calcCorner = [&](const CVector& base, float sign1, float sign2) -> CVector {
        return {
            base.x + sign1 * (main.x * offsetLength) + sign2 * (main.y * offsetWidth),
            base.y + sign1 * (main.y * offsetLength) + sign2 * (-main.x * offsetWidth),
            pos.z
        };
    };

    *point1 = calcCorner(higherPoint, 1.0F, -1.0F);
    *point2 = calcCorner(higherPoint, 1.0F, 1.0F);
    *point3 = calcCorner(lowerPoint, -1.0F, 1.0F);
    *point4 = calcCorner(lowerPoint, -1.0F, -1.0F);
}

// Updates the animations for the entity's RenderWare clump, if it has any
// Animations are only updated if the entity is on screen
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

// Checks if the entity is currently visible on screen
// 0x536BC0
bool CEntity::IsVisible() {
    if (!GetRwObject() || !GetIsVisible()) {
        return false;
    }

    return GetIsOnScreen();
}

// Calculates the distance from the collision model's center of mass
// to the base (lowest Z point) of the model
// 0x536BE0
float CEntity::GetDistanceFromCentreOfMassToBaseOfModel() const {
    float lowestZ;

    CColModel& colMod = *GetColModel();
    lowestZ = colMod.m_boundBox.m_vecMin.z;
    return -lowestZ;
}

// Removes a specific reference to this entity from its list of references
// Used when an entity that was pointing to this one is now pointing elsewhere or is being destroyed
// in references.cpp
// 0x571A00
void CEntity::CleanUpOldReference(CEntity** entity) {
    auto lastnextp = &m_pReferences;
    for (auto ref = m_pReferences; ref; ref = ref->m_pNext) {
        if (ref->m_ppEntity == entity) {
            *lastnextp = ref->m_pNext;
            ref->m_pNext = CReferences::pEmptyList;
            CReferences::pEmptyList = ref;
            ref->m_ppEntity = nullptr;
            break;
        }
        lastnextp = &ref->m_pNext;
    }
}

// Notifies all entities that reference this one that it's being destroyed
// It iterates through its list of references and sets their pointers to null
// in references.cpp
// 0x571A40
void CEntity::ResolveReferences() {
    for (auto ref = m_pReferences; ref; ref = ref->m_pNext) {
        if (ref->m_ppEntity && *ref->m_ppEntity == this) {
            *ref->m_ppEntity = nullptr;
        }
    }

    if (m_pReferences) {
        auto head = m_pReferences;

        auto tail = head;
        while (tail->m_pNext) {
            tail = tail->m_pNext;
        }

        for (auto ref = head; ref; ref = ref->m_pNext) {
            ref->m_ppEntity = nullptr;
        }

        tail->m_pNext = CReferences::pEmptyList;
        CReferences::pEmptyList = head;

        m_pReferences = nullptr;
    }
}

// Cleans up the entity's reference list by removing any references
// from other entities that no longer point to this one
// in references.cpp
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

// Registers a new reference to this entity
// The `entity` parameter is a pointer to a CEntity pointer that now points to `this`
// in references.cpp
// 0x571B70
void CEntity::RegisterReference(CEntity** entity) {
    if (GetIsTypeBuilding() && !m_bIsTempBuilding && !m_bIsProcObject && !GetIplIndex()) {
        return;
    }

    for (auto refs = m_pReferences; refs; refs = refs->m_pNext) {
        if (refs->m_ppEntity == entity) {
            return;
        }
    }

    if (!CReferences::pEmptyList) {
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

// Processes all light-type 2dEffects for this entity
// This creates coronas, point lights, and shadows based on the effect's properties
// in Coronas.cpp
// 0x6FC7A0
void CEntity::ProcessLightsForEntity() {
    constexpr static const uint16 randomSeedRandomiser[8] = { 0, 27'034, 43'861, 52'326, 64'495, 38'437, 21'930, 39'117 }; // 0x8D5028
    constexpr static const float FADE_RATE = 0.0009f; // 0x872538

    const auto dayNightBalance = GetDayNightBalance();
    if (m_bRenderDamaged || !GetIsVisible()) {
        return;
    }

    if (GetIsTypeVehicle()) {
        if (AsVehicle()->physicalFlags.bRenderScorched) {
            return;
        }
    } else if (m_matrix && m_matrix->GetUp().z < 0.96f) {
        return;
    }

    CBaseModelInfo* mi = GetModelInfo();
    const int32 numEffects = mi->m_n2dfxCount;
    if (!numEffects) {
        return;
    }

    for (int32 C = 0; C < numEffects; ++C) {
        const auto* effect = mi->Get2dEffect(C);
        float TimeFade = 1.0f;
        const auto randomSeed = m_nRandomSeed ^ randomSeedRandomiser[C % 8];

        if (effect->m_Type == e2dEffectType::EFFECT_SUN_GLARE && CWeather::SunGlare >= 0.0f) {
            auto effectPos = TransformFromObjectSpace(effect->m_Pos);
            auto direction = effectPos - GetPosition();
            direction.Normalise();

            const auto cameraPos = TheCamera.GetPosition();
            auto cameraDir = cameraPos - effectPos;
            const auto cameraDist = cameraDir.Magnitude();
            const auto scale = 2.0f / cameraDist;
            auto scaledCameraDir = cameraDir * scale;

            direction += scaledCameraDir;
            direction.Normalise();

            const auto dotProduct = -direction.Dot(CTimeCycle::m_VectorToSun[CTimeCycle::m_CurrentStoredValue]);
            if (dotProduct > 0.0f) {
                const auto glare = sqrt(dotProduct) * CWeather::SunGlare;
                const auto radius = sqrt(cameraDist) * CWeather::SunGlare * 0.5f;
                effectPos += scaledCameraDir;

                CCoronas::RegisterCorona(
                    m_nRandomSeed + C + 1,
                    nullptr,
                    static_cast<uint8>((CTimeCycle::m_CurrentColours.m_nSunCoreRed + 510) * glare / 3.0f),
                    static_cast<uint8>((CTimeCycle::m_CurrentColours.m_nSunCoreGreen + 510) * glare / 3.0f),
                    static_cast<uint8>((CTimeCycle::m_CurrentColours.m_nSunCoreBlue + 510) * glare / 3.0f),
                    255,
                    effectPos,
                    radius,
                    120.0f,
                    gpCoronaTexture[CORONATYPE_SHINYSTAR],
                    eCoronaFlareType::FLARETYPE_NONE,
                    eCoronaReflType::CORREFL_NONE,
                    eCoronaLOSCheck::LOSCHECK_OFF,
                    eCoronaTrail::TRAIL_OFF,
                    0.0f,
                    false,
                    1.5f,
                    0,
                    15.0f,
                    false,
                    false
                );
            }
            continue;
        }

        if (effect->m_Type != e2dEffectType::EFFECT_LIGHT) {
            continue;
        }

        auto effectPos = TransformFromObjectSpace(effect->m_Pos);
        bool ApplyLight = false;
        bool ApplyOffVersion = false;
        bool ApplyOnVersion = false;
        bool UpdateCoors = false;
        float LightFade = 1.0f;

        if (effect->light.m_bAtDay && effect->light.m_bAtNight) {
            ApplyOffVersion = true;
        } else if (effect->light.m_bAtDay && dayNightBalance < 1.0f) {
            ApplyOffVersion = true;
            LightFade = 1.0f - dayNightBalance;
        } else if (effect->light.m_bAtNight && dayNightBalance > 0.0f) {
            ApplyOffVersion = true;
            LightFade = dayNightBalance;
        }

        const auto& entityPos = GetPosition();
        const auto flashType = effect->light.m_nCoronaFlashType;

        if (flashType == e2dCoronaFlashType::FLASH_RANDOM_WHEN_WET && CWeather::WetRoads > 0.5f || ApplyOffVersion) {
            switch (flashType) {
            case e2dCoronaFlashType::FLASH_DEFAULT:
                ApplyLight = true;
                break;
            case e2dCoronaFlashType::FLASH_RANDOM:
            case e2dCoronaFlashType::FLASH_RANDOM_WHEN_WET:
                if ((CTimer::GetTimeInMS() ^ randomSeed) & 0x60) {
                    ApplyLight = true;
                } else {
                    ApplyOnVersion = true;
                }
                if ((randomSeed ^ (CTimer::GetTimeInMS() / 4'096)) & 0x3) {
                    ApplyLight = true;
                }
                break;
            case e2dCoronaFlashType::FLASH_ANIM_SPEED_4X:
                if (((CTimer::GetTimeInMS() + C * 128) & 0x200) == 0) {
                    UpdateCoors = true;
                } else {
                    ApplyLight = true;
                }
                break;
            case e2dCoronaFlashType::FLASH_ANIM_SPEED_2X:
                if (((CTimer::GetTimeInMS() + C * 256) & 0x400) == 0) {
                    UpdateCoors = true;
                } else {
                    ApplyLight = true;
                }
                break;
            case e2dCoronaFlashType::FLASH_ANIM_SPEED_1X:
                if (((CTimer::GetTimeInMS() + C * 512) & 0x800) == 0) {
                    UpdateCoors = true;
                } else {
                    ApplyLight = true;
                }
                break;
            case e2dCoronaFlashType::FLASH_UNKN:
                if (static_cast<uint8>(randomSeed) > 0x10) {
                    ApplyLight = true;
                    break;
                }
                if ((CTimer::GetTimeInMS() ^ (randomSeed * 8)) & 0x60) {
                    ApplyLight = true;
                } else {
                    ApplyOnVersion = true;
                }
                if ((randomSeed ^ (CTimer::GetTimeInMS() / 4'096)) & 0x3) {
                    ApplyLight = true;
                }
                break;
            case e2dCoronaFlashType::FLASH_TRAINCROSSING:
                if (GetIsTypeObject() && AsObject()->objectFlags.bTrainCrossEnabled) {
                    if (CTimer::GetTimeInMS() & 0x400) {
                        ApplyLight = true;
                    }
                    if (C & 1) {
                        ApplyLight = !ApplyLight;
                    }
                }
                if (C >= 4) {
                    ApplyLight = false;
                }
                break;
            case e2dCoronaFlashType::FLASH_UNUSED:
                if (CBridge::ShouldLightsBeFlashing() && (CTimer::GetTimeInMS() & 0x1FF) < 0x3C) {
                    ApplyLight = true;
                }
                break;
            case e2dCoronaFlashType::FLASH_ONLY_RAIN:
                if (CWeather::Rain > 0.0001f) {
                    TimeFade = CWeather::Rain;
                    ApplyLight = true;
                }
                break;
            case e2dCoronaFlashType::FLASH_5ON_5OFF:
            case e2dCoronaFlashType::FLASH_6ON_4OFF:
            case e2dCoronaFlashType::FLASH_4ON_6OFF: {
                ApplyLight = true;
                uint32 offset = CTimer::GetTimeInMS() + 3'333 * (flashType - 11);
                offset += static_cast<uint32>(entityPos.x * 20.0f);
                offset += static_cast<uint32>(entityPos.y * 10.0f);

                const auto mode = 9 * (offset % 10'000 / 10'000);
                const auto fade = (offset % 10'000 - (1'111 * mode)) * FADE_RATE;

                switch (mode) {
                case 0:  TimeFade = fade; break;
                case 1:
                case 2:  TimeFade = 1.0f; break;
                case 3:  TimeFade = 1.0f - fade; break;
                default: ApplyLight = false; break;
                }
                break;
            }
            default:
                break;
            }
        }

        // CORONAS
        bool skipCoronaChecks = false;
        if (CGameLogic::LaRiotsActiveHere()) {
            bool lightsOn = ApplyLight;
            lightsOn = lightsOn && !GetIsTypeVehicle();
            lightsOn = lightsOn && ((randomSeed & 3) == 0 || (randomSeed & 3) == 1 && ((CTimer::GetTimeInMS() ^ (randomSeed * 8)) & 0x60));

            if (lightsOn) {
                ApplyLight = false;
                ApplyOnVersion = true;
                skipCoronaChecks = true;

                CCoronas::RegisterCorona(
                    (uint32)this + C,
                    nullptr,
                    0,
                    0,
                    0,
                    255,
                    effectPos,
                    effect->light.m_fCoronaSize,
                    effect->light.m_fCoronaFarClip,
                    effect->light.m_pCoronaTex,
                    static_cast<eCoronaFlareType>(effect->light.m_nCoronaFlareType),
                    static_cast<eCoronaReflType>(effect->light.m_bCoronaEnableReflection),
                    static_cast<eCoronaLOSCheck>(effect->light.m_bCheckObstacles),
                    eCoronaTrail::TRAIL_OFF,
                    0.0f,
                    effect->light.m_bOnlyLongDistance,
                    1.5f,
                    0,
                    15.0f,
                    false,
                    false
                );
            }
        }

        if (!skipCoronaChecks && ApplyLight) {
            bool canCreateLight = true;
            if (effect->light.m_bCheckDirection) {
                const auto& camPos = TheCamera.GetPosition();
                CVector lightOffset{
                    static_cast<float>(effect->light.offsetX),
                    static_cast<float>(effect->light.offsetY),
                    static_cast<float>(effect->light.offsetZ)
                };
                const auto lightPos = GetMatrix().TransformVector(lightOffset);
                const auto dot = lightPos.Dot(camPos - effectPos);
                canCreateLight = dot >= 0.0f;
            }

            if (canCreateLight) {
                skipCoronaChecks = true;
                auto brightness = TimeFade;
                if (effect->light.m_bBlinking1) {
                    brightness = (1.0f - (CGeneral::GetRandomNumber() % 32) * 0.012f) * TimeFade;
                }
                if (effect->light.m_bBlinking2 && (CTimer::GetFrameCounter() + randomSeed) & 3) {
                    brightness = 0.0f;
                }
                if (effect->light.m_bBlinking3 && (CTimer::GetFrameCounter() + randomSeed) & 0x3F) {
                    if (((CTimer::GetFrameCounter() + randomSeed) & 0x3F) == 1) {
                        brightness *= 0.5f;
                    } else {
                        brightness = 0.0f;
                    }
                }

                auto sizeMult = 1.0f;
                if (GetModelIndex() == MODEL_RCBARON) {
                    brightness *= 1.9f;
                    sizeMult = 2.0f;
                }

                TimeFade = CTimeCycle::m_CurrentColours.m_fSpriteBrightness * brightness / 10.0f;
                const auto coronaSize = effect->light.m_fCoronaSize * sizeMult;

                CCoronas::RegisterCorona(
                    (uint32)this + C,
                    nullptr,
                    static_cast<uint8>(static_cast<float>(effect->light.m_color.red) * TimeFade),
                    static_cast<uint8>(static_cast<float>(effect->light.m_color.green) * TimeFade),
                    static_cast<uint8>(static_cast<float>(effect->light.m_color.blue) * TimeFade),
                    static_cast<uint8>(LightFade * 255.0f),
                    effectPos,
                    coronaSize,
                    effect->light.m_fCoronaFarClip,
                    effect->light.m_pCoronaTex,
                    static_cast<eCoronaFlareType>(effect->light.m_nCoronaFlareType),
                    static_cast<eCoronaReflType>(effect->light.m_bCoronaEnableReflection),
                    static_cast<eCoronaLOSCheck>(effect->light.m_bCheckObstacles),
                    eCoronaTrail::TRAIL_OFF,
                    0.0f,
                    effect->light.m_bOnlyLongDistance,
                    0.8f,
                    0,
                    15.0f,
                    effect->light.m_bOnlyFromBelow,
                    effect->light.m_bUpdateHeightAboveGround
                );
            } else {
                ApplyLight = false;
                UpdateCoors = true;
            }
        }

        if (!skipCoronaChecks && ApplyOnVersion) {
            skipCoronaChecks = true;
            CCoronas::RegisterCorona(
                (uint32)this + C,
                nullptr,
                0,
                0,
                0,
                255,
                effectPos,
                effect->light.m_fCoronaSize,
                effect->light.m_fCoronaFarClip,
                effect->light.m_pCoronaTex,
                static_cast<eCoronaFlareType>(effect->light.m_nCoronaFlareType),
                static_cast<eCoronaReflType>(effect->light.m_bCoronaEnableReflection),
                static_cast<eCoronaLOSCheck>(effect->light.m_bCheckObstacles),
                eCoronaTrail::TRAIL_OFF,
                0.0f,
                effect->light.m_bOnlyLongDistance,
                1.5f,
                0,
                15.0f,
                false,
                false
            );
        }

        if (!skipCoronaChecks && UpdateCoors) {
            CCoronas::UpdateCoronaCoors(
                (uint32)this + C,
                effectPos,
                effect->light.m_fCoronaFarClip,
                0.0f
            );
        }

        // POINT LIGHTS
        bool skipLights = false;
        if (effect->light.m_fPointlightRange != 0.0f && ApplyLight) {
            const auto& color = effect->light.m_color;
            if (color.red || color.green || color.blue) {
                const float colorMult = LightFade * TimeFade / 256.0f;
                skipLights = true;
                CPointLights::AddLight(
                    ePointLightType::PLTYPE_POINTLIGHT,
                    effectPos,
                    CVector{},
                    effect->light.m_fPointlightRange,
                    static_cast<float>(color.red) * colorMult,
                    static_cast<float>(color.green) * colorMult,
                    static_cast<float>(color.blue) * colorMult,
                    effect->light.m_nFogType,
                    true,
                    nullptr
                );
            } else {
                CPointLights::AddLight(
                    ePointLightType::PLTYPE_ANTILIGHT,
                    effectPos,
                    CVector{},
                    effect->light.m_fPointlightRange,
                    0.0f,
                    0.0f,
                    0.0f,
                    RwFogType::rwFOGTYPENAFOGTYPE,
                    true,
                    nullptr
                );
            }
        }

        if (!skipLights) {
            const auto& color = effect->light.m_color;
            if (effect->light.m_nFogType & RwFogType::rwFOGTYPEEXPONENTIAL) {
                CPointLights::AddLight(
                    ePointLightType::PLTYPE_ONLYFOGEFFECT_ALWAYS,
                    effectPos,
                    CVector{},
                    0.0f,
                    color.red / 256.0f,
                    color.green / 256.0f,
                    color.blue / 256.0f,
                    RwFogType::rwFOGTYPEEXPONENTIAL,
                    true,
                    nullptr
                );
            } else if (effect->light.m_nFogType & RwFogType::rwFOGTYPELINEAR && ApplyLight && effect->light.m_fPointlightRange == 0.0f) {
                CPointLights::AddLight(
                    ePointLightType::PLTYPE_ONLYFOGEFFECT,
                    effectPos,
                    CVector{},
                    0.0f,
                    color.red / 256.0f,
                    color.green / 256.0f,
                    color.blue / 256.0f,
                    RwFogType::rwFOGTYPELINEAR,
                    true,
                    nullptr
                );
            }
        }

        // PC Only?
        // SHADOWS
        if (effect->light.m_fShadowSize != 0.0f) {
            float shadowZ = 15.0f;
            if (effect->light.m_nShadowZDistance) {
                shadowZ = static_cast<float>(effect->light.m_nShadowZDistance);
            }

            if (ApplyLight) {
                auto color = effect->light.m_color;
                const float colorMult = effect->light.m_nShadowColorMultiplier * TimeFade / 256.0f;
                color.red = static_cast<uint8>(static_cast<float>(color.red) * colorMult);
                color.green = static_cast<uint8>(static_cast<float>(color.green) * colorMult);
                color.blue = static_cast<uint8>(static_cast<float>(color.blue) * colorMult);

                CShadows::StoreStaticShadow(
                    (uint32)this + C,
                    eShadowType::SHADOW_ADDITIVE,
                    effect->light.m_pShadowTex,
                    effectPos,
                    effect->light.m_fShadowSize,
                    0.0f,
                    0.0f,
                    -effect->light.m_fShadowSize,
                    128,
                    color.red,
                    color.green,
                    color.blue,
                    shadowZ,
                    1.0f,
                    40.0f,
                    false,
                    0.0f
                );
            } else if (ApplyOnVersion) {
                CShadows::StoreStaticShadow(
                    (uint32)this + C,
                    eShadowType::SHADOW_ADDITIVE,
                    effect->light.m_pShadowTex,
                    effectPos,
                    effect->light.m_fShadowSize,
                    0.0f,
                    0.0f,
                    -effect->light.m_fShadowSize,
                    0,
                    0,
                    0,
                    0,
                    shadowZ,
                    1.0f,
                    40.0f,
                    false,
                    0.0f
                );
            }
        }
    }
}

// Finds and removes any escalator data associated with this entity
// in fluff.cpp
// 0x717900
void CEntity::RemoveEscalatorsForEntity() {
    for (auto& escalator : CEscalators::GetAllExists()) {
        if (escalator.GetEntity() == this) {
            escalator.Remove();
            return;
        }
    }
}

// Checks if the entity is occluded by any active occluders in the world
// It tests various points on the entity's bounding box against the occluders
// in occlusion.cpp
// 0x71FAE0
bool CEntity::IsEntityOccluded() {
    if (COcclusion::GetActiveOccluders().empty()) {
        return false;
    }

    CVector center = GetBoundCentre();

    CVector centerScrPos;
    float   scaleX, scaleY;
    if (!CalcScreenCoors(center, centerScrPos, scaleX, scaleY)) {
        return false;
    }

    const auto* mi = GetModelInfo();
    const auto& bb = mi->GetColModel()->GetBoundingBox();

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

    // The first 4 points (bounding box corners)
    const std::array firstFourPoints = {
        GetOccluderPt(min),                     // Point 1: min
        GetOccluderPt(max),                     // Point 2: max
        GetOccluderPt({ min.x, max.y, max.z }), // Point 3
        GetOccluderPt({ max.x, min.y, min.z })  // Point 4
    };
    const std::array lastTwoPoints = {
        GetOccluderPt({ min.x, min.y, max.z }), // Point 5
        GetOccluderPt({ max.x, min.y, max.z })  // Point 6
    };

    return rng::any_of(COcclusion::GetActiveOccluders(), [&](const auto& o) -> bool {
        if (o.GetDistToCam() >= centerScrPos.z - boundingRadius) {
            return false;
        }

        // Checking the center with the occlusion radius
        if (o.IsPointWithinOcclusionArea(centerScrPos, occlusionRadius)) {
            if (o.IsPointBehindOccluder(center, boundingRadius)) {
                return true;
            }
        }

        // Checking the center without radius
        if (!o.IsPointWithinOcclusionArea(centerScrPos)) {
            return false;
        }

        // Common lambda for point verification (screen + world, with radius=0.0f)
        const auto checkPoint = [&](const std::optional<CVector>& scrOpt, const CVector& ws) -> bool {
            if (!scrOpt.has_value()) {
                return false;
            }
            const auto& scr = *scrOpt;
            return o.IsPointWithinOcclusionArea(scr) && o.IsPointBehindOccluder(ws);
        };

        // Checking the first 4 points with early exit
        bool allFirstFourPass = true;
        for (const auto& [ws, scr] : firstFourPoints) {
            if (!checkPoint(scr, ws)) {
                allFirstFourPass = false;
                break;
            }
        }
        if (!allFirstFourPass) {
            return false;
        }

        // Checking bounding box dimensions
        const auto width = max.x - min.x;
        const auto length = max.y - min.y;
        const auto height = max.z - min.z;
        if ((width <= 60.0f && length <= 60.0f) || height <= 30.0f) {
            return true;
        }

        // Check the last 2 points (only for large objects)
        for (const auto& [ws, scr] : lastTwoPoints) {
            if (!checkPoint(scr, ws)) {
                return false;
            }
        }
        return true;
    });
}

// in header
inline float CEntity::GetBoundRadius() const {
    return GetModelInfo()->GetColModel()->GetBoundingSphere().m_fRadius;
}

// Updates the entity's RenderWare matrix from its CPlaceable transform data
// This synchronizes the game's coordinate system with RenderWare's
// 0x446F90
void CEntity::UpdateRwMatrix() {
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

// Traverses the LOD chain to find the last (lowest-detail) LOD entity
// If the entity has no LOD, it returns itself
// NOTSA
CEntity* CEntity::FindLastLOD() noexcept {
    CEntity* it = this;
    for (; it->m_pLod; it = it->m_pLod);
    return it;
}

// A helper to get the CBaseModelInfo pointer for the entity's model index
// NOTSA
CBaseModelInfo* CEntity::GetModelInfo() const {
    return CModelInfo::GetModelInfo(GetModelIndex());
}

// Updates the entity's scan code to the current frame's code
// This is used to prevent processing the same entity multiple times in one frame
// Returns true if the entity's scan code was updated
// NOTSA
bool CEntity::ProcessScan() {
    if (IsScanCodeCurrent()) {
        return false;
    }
    SetCurrentScanCode();
    return true;
}

// A RenderWare callback to set the alpha on all materials of an atomic
// 0x533290
RpAtomic* SetAtomicAlpha(RpAtomic* atomic, void* data) {
    auto geometry = RpAtomicGetGeometry(atomic);
    RpGeometrySetFlags(geometry, rpGEOMETRYMODULATEMATERIALCOLOR);
    RpGeometryForAllMaterials(geometry, SetCompAlphaCB, data);
    return atomic;
}

// A RenderWare callback to set the alpha component of a single material's color
// 0x533280
RpMaterial* SetCompAlphaCB(RpMaterial* material, void* data) {
    RpMaterialGetColor(material)->alpha = (RwUInt8)(uintptr)data;
    return material;
}

// Checks if the entity's scan code matches the current global scan code,
// indicating if it has already been processed in the current frame
// NOTSA
bool CEntity::IsScanCodeCurrent() const {
    return GetScanCode() == CWorld::GetCurrentScanCode();
}

// Sets the entity's scan code to the current global scan code
// NOTSA
void CEntity::SetCurrentScanCode() {
    SetScanCode(CWorld::GetCurrentScanCode());
}
