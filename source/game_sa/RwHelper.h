#pragma once

#include "RenderWare.h"

class CEventGlobalGroup;

void RwHelperInjectHooks();

CEventGlobalGroup* GetEventGlobalGroup();

void GetNameAndDamage(const char* name, char* objName, bool& bIsDamageModel);
RpAtomic* GetFirstAtomicCallback(RpAtomic* atomic, void* data);
RpAtomic* GetFirstAtomic(RpClump* clump);
RpAtomic* Get2DEffectAtomicCallback(RpAtomic* atomic, void* data);
RpAtomic* Get2DEffectAtomic(RpClump* clump);
RwObject* GetFirstObjectCallback(RwObject* object, void* data);
RwObject* GetFirstObject(RwFrame* frame);
RwFrame* GetFirstFrameCallback(RwFrame* frame, void* data);
RwFrame* GetFirstChild(RwFrame* frame);
RpAtomic* SkinAtomicGetHAnimHierarchCB(RpAtomic* atomic, void* data);
RpHAnimHierarchy* GetAnimHierarchyFromSkinClump(RpClump* clump);
RpHAnimHierarchy* GetAnimHierarchyFromFrame(RwFrame* frame);
RpHAnimHierarchy* GetAnimHierarchyFromClump(RpClump* clump);
RpAtomic* AtomicRemoveAnimFromSkinCB(RpAtomic* atomic, void* data);
bool RpAtomicConvertGeometryToTL(RpAtomic* atomic);
bool RpAtomicConvertGeometryToTS(RpAtomic* atomic);
RpAtomic* atomicConvertGeometryToTL(RpAtomic* atomic, void* data);
bool RpClumpConvertGeometryToTL(RpClump* clump);
RpAtomic* atomicConvertGeometryToTS(RpAtomic* atomic, void* data);
bool RpClumpConvertGeometryToTS(RpClump* clump);
RpMaterial* forceLinearFilteringMatTexturesCB(RpMaterial* material, void* data);
bool SetFilterModeOnAtomicsTextures(RpAtomic* atomic, RwTextureFilterMode filtering);
RpAtomic* forceLinearFilteringAtomicsCB(RpAtomic* atomic, void* data);
bool SetFilterModeOnClumpsTextures(RpClump* clump, RwTextureFilterMode filtering);
bool RpGeometryReplaceOldMaterialWithNewMaterial(RpGeometry* geometry, RpMaterial* oldMaterial, RpMaterial* newMaterial);
RwTexture* RwTexDictionaryFindHashNamedTexture(RwTexDictionary* txd, uint32 hash);
RpClump* RpClumpGetBoundingSphere(RpClump* clump, RwSphere* sphere, bool bUseLTM);
void SkinGetBonePositions(RpClump* clump);
void SkinSetBonePositions(RpClump* clump);
void SkinGetBonePositionsToTable(RpClump* clump, RwV3d* table);

RpAtomic* RemoveRefsCB(RpAtomic* atomic, void* data);
void RemoveRefsForAtomic(RpClump* clump);
