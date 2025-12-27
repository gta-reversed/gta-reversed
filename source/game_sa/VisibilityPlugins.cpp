/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "VisibilityPlugins.h"

RwInt32 CVisibilityPlugins::ms_atomicPluginOffset = -1;
RwInt32 CVisibilityPlugins::ms_clumpPluginOffset = -1;
RwInt32 CVisibilityPlugins::ms_framePluginOffset = -1;
RpAtomicCallBackRender CVisibilityPlugins::ms_defaultRenderer = nullptr;
RwCamera* CVisibilityPlugins::ms_pCamera = nullptr;
RwV3d* CVisibilityPlugins::ms_pCameraPosn = nullptr;

float CVisibilityPlugins::ms_vehicleLod0RenderMultiPassDist;
float CVisibilityPlugins::ms_vehicleLod0Dist;
float CVisibilityPlugins::ms_vehicleLod1Dist;
float CVisibilityPlugins::ms_bigVehicleLod0Dist;
float CVisibilityPlugins::ms_pedLodDist;
float CVisibilityPlugins::ms_pedFadeDist;
float CVisibilityPlugins::ms_cullCompsDist;
float CVisibilityPlugins::ms_cullBigCompsDist;

CLinkList<CVisibilityPlugins::AlphaObjectInfo> CVisibilityPlugins::m_alphaList;
CLinkList<CVisibilityPlugins::AlphaObjectInfo> CVisibilityPlugins::m_alphaBoatAtomicList;
CLinkList<CVisibilityPlugins::AlphaObjectInfo> CVisibilityPlugins::m_alphaEntityList;
CLinkList<CVisibilityPlugins::AlphaObjectInfo> CVisibilityPlugins::m_alphaUnderwaterEntityList;
CLinkList<CVisibilityPlugins::AlphaObjectInfo> CVisibilityPlugins::m_alphaReallyDrawLastList;

CLinkList<CPed*> CVisibilityPlugins::ms_weaponPedsForPC;

float gVehicleDistanceFromCamera; // 0xC88024
float gAngleWithHorizontal; // 0xC88020

bool preAlloc; // 0xC8801C

void CVisibilityPlugins::InjectHooks() {
    RH_ScopedClass(CVisibilityPlugins);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x733A20);
    RH_ScopedInstall(Shutdown, 0x732EB0);
    RH_ScopedInstall(AtomicCopyConstructor, 0x732170);
    RH_ScopedInstall(AtomicDestructor, 0x7321A0);
    RH_ScopedInstall(CalculateFadingAtomicAlpha, 0x732500);
    RH_ScopedInstall(ClearAtomicFlag, 0x732310);
    RH_ScopedInstall(SetAtomicFlag, 0x7322B0);
    RH_ScopedInstall(ClearClumpForAllAtomicsFlag, 0x732350);
    RH_ScopedInstall(ClumpConstructor, 0x732E10);
    RH_ScopedInstall(ClumpCopyConstructor, 0x732200);
    RH_ScopedInstall(ClumpDestructor, 0x732220);
    RH_ScopedInstall(DefaultVisibilityCB, 0x732A30);
    RH_ScopedInstall(FrameConstructor, 0x7321B0);
    RH_ScopedInstall(FrameCopyConstructor, 0x7321D0);
    RH_ScopedInstall(FrameDestructor, 0x7321F0);
    RH_ScopedInstall(FrustumSphereCB, 0x732A40);
    RH_ScopedInstall(GetAtomicId, 0x732370);
    RH_ScopedInstall(GetModelInfo, 0x732260);
    RH_ScopedInstall(GetClumpAlpha, 0x732B20);
    RH_ScopedInstall(GetClumpModelInfo, 0x732AC0);
    RH_ScopedOverloadedInstall(GetDistanceSquaredFromCamera, "Frame", 0x732C80, float (*)(RwFrame*));
    RH_ScopedOverloadedInstall(GetDistanceSquaredFromCamera, "RwV3d", 0x732CC0, float (*)(RwV3d*));
    RH_ScopedInstall(GetDotProductWithCameraVector, 0x7326D0);
    RH_ScopedInstall(GetFrameHierarchyId, 0x732A20);
    RH_ScopedInstall(GetModelInfoIndex, 0x732250);
    RH_ScopedInstall(GetUserValue, 0x7323A0);
    RH_ScopedInstall(InitAlphaAtomicList, 0x734530);
    RH_ScopedInstall(InitAlphaEntityList, 0x734540);
    RH_ScopedInstall(InsertObjectIntoSortedList, 0x733DD0);
    RH_ScopedInstall(InsertEntityIntoUnderwaterList, 0x733D90);
    RH_ScopedInstall(InsertAtomicIntoReallyDrawLastList, 0x733E10);
    RH_ScopedInstall(InsertEntityIntoReallyDrawLastList, 0x733E50);
    RH_ScopedInstall(InsertEntityIntoSortedList, 0x734570);
    RH_ScopedInstall(IsAtomicVisible, 0x732990);
    RH_ScopedInstall(PluginAttach, 0x732E30);
    RH_ScopedInstall(RenderAtomicWithAlpha, 0x732480);
    RH_ScopedInstall(RenderAlphaAtomics, 0x733E90);
    RH_ScopedInstall(RenderAtomicWithAlphaCB, 0x732660);
    RH_ScopedInstall(RenderBoatAlphaAtomics, 0x733EC0);
    RH_ScopedInstall(RenderEntity, 0x732B40);
    RH_ScopedInstall(RenderFadingAtomic, 0x732610);
    RH_ScopedInstall(RenderFadingClump, 0x732680);
    RH_ScopedInstall(RenderFadingClumpCB, 0x733630);
    RH_ScopedInstall(RenderFadingEntities, 0x733F10);
    RH_ScopedInstall(RenderFadingUnderwaterEntities, 0x7337D0);
    RH_ScopedInstall(RenderHeliRotorAlphaCB, 0x7340B0);
    RH_ScopedInstall(RenderHeliTailRotorAlphaCB, 0x734170);
    RH_ScopedInstall(RenderObjNormalAtomic, 0x7323E0);
    RH_ScopedInstall(RenderOrderedList, 0x7337A0);
    RH_ScopedInstall(RenderPedCB, 0x7335B0);
    RH_ScopedInstall(RenderPlayerCB, 0x732870);
    RH_ScopedInstall(RenderReallyDrawLastObjects, 0x733800);
    RH_ScopedInstall(RenderTrainHiDetailAlphaCB, 0x734240);
    RH_ScopedInstall(RenderTrainHiDetailCB, 0x733330);
    RH_ScopedInstall(RenderVehicleHiDetailAlphaCB, 0x733F80);
    RH_ScopedInstall(RenderVehicleHiDetailAlphaCB_BigVehicle, 0x734370);
    RH_ScopedInstall(RenderVehicleHiDetailAlphaCB_Boat, 0x7344A0);
    RH_ScopedInstall(RenderVehicleHiDetailCB, 0x733240);
    RH_ScopedInstall(RenderVehicleHiDetailCB_BigVehicle, 0x733420);
    RH_ScopedInstall(RenderVehicleHiDetailCB_Boat, 0x733550);
    RH_ScopedInstall(RenderVehicleLoDetailCB_Boat, 0x7334F0);
    RH_ScopedInstall(RenderVehicleReallyLowDetailCB, 0x7331E0);
    RH_ScopedInstall(RenderVehicleReallyLowDetailCB_BigVehicle, 0x732820);
    RH_ScopedInstall(RenderWeaponCB, 0x733670);
    RH_ScopedInstall(RenderWeaponPedsForPC, 0x732F30);
    RH_ScopedInstall(SetClumpForAllAtomicsFlag, 0x732307);
    RH_ScopedInstall(SetAtomicId, 0x732290);
    RH_ScopedInstall(SetModelInfoIndex, 0x732230);
    RH_ScopedInstall(SetAtomicRenderCallback, 0x7328A0);
    RH_ScopedInstall(SetClumpAlpha, 0x732B00);
    RH_ScopedInstall(SetClumpModelInfo, 0x733750);
    RH_ScopedInstall(SetFrameHierarchyId, 0x732A00);
    RH_ScopedInstall(SetRenderWareCamera, 0x7328C0);
    RH_ScopedInstall(SetUserValue, 0x732380);
    RH_ScopedInstall(SetupVehicleVariables, 0x733160);
    RH_ScopedInstall(VehicleVisibilityCB, 0x7336F0);
    RH_ScopedInstall(VehicleVisibilityCB_BigVehicle, 0x732AB0);
    RH_ScopedInstall(RenderWheelAtomicCB, 0x7323C0);
    RH_ScopedInstall(AddWeaponPedForPC, 0x5E46D0);
    RH_ScopedInstall(IsClumpVisible, 0x732AE0);

    RH_ScopedGlobalInstall(clearClumpForAllAtomicsFlagCB, 0x732330);
    RH_ScopedGlobalInstall(setClumpForAllAtomicsFlagCB, 0x7322D0);
    RH_ScopedGlobalInstall(SetTextureCB, 0x732850);
    RH_ScopedGlobalInstall(SetAlphaCB, 0x7323B0);
}

// 0x733A20
void CVisibilityPlugins::Initialise() {
    auto initList = [](auto& list, int size) {
        list.Init(size);
        list.usedListHead.data.m_distance = 0.0f;
        list.usedListTail.data.m_distance = 100000000.0f;
    };

    initList(m_alphaList, TOTAL_ALPHA_LISTS);
    initList(m_alphaBoatAtomicList, TOTAL_ALPHA_BOAT_ATOMIC_LISTS);
    initList(m_alphaEntityList, TOTAL_ALPHA_ENTITY_LISTS);
    initList(m_alphaUnderwaterEntityList, TOTAL_ALPHA_UNDERWATER_ENTITY_LISTS);
    // New in SA
    initList(m_alphaReallyDrawLastList, TOTAL_ALPHA_DRAW_LAST_LISTS);

    ms_weaponPedsForPC.Init(TOTAL_WEAPON_PEDS_FOR_PC);
}


// 0x732EB0
void CVisibilityPlugins::Shutdown() {
    m_alphaList.Shutdown();
    m_alphaBoatAtomicList.Shutdown();
    m_alphaEntityList.Shutdown();
    m_alphaUnderwaterEntityList.Shutdown();
    m_alphaReallyDrawLastList.Shutdown();
    ms_weaponPedsForPC.Shutdown();
}

// unused
// inline
// 0x732DE0
inline void CVisibilityPlugins::RenderVehicleCB_ControlRenderMultiPassLOD(RpAtomic* atomic, float vehicleDistanceFromCamera) {
    if (vehicleDistanceFromCamera >= ms_vehicleLod0RenderMultiPassDist) {
        SetAtomicFlag(atomic, ATOMIC_UNIQUE_MATERIALS);
    } else {
        ClearAtomicFlag(atomic, ATOMIC_UNIQUE_MATERIALS);
    }
}

// 0x734530
void CVisibilityPlugins::InitAlphaAtomicList() {
    m_alphaList.Clear();
}

// 0x734540
void CVisibilityPlugins::InitAlphaEntityList() {
    m_alphaEntityList.Clear();
    m_alphaBoatAtomicList.Clear();
    m_alphaUnderwaterEntityList.Clear();
    m_alphaReallyDrawLastList.Clear();
}

// NOTSA
template<typename TObject>
bool InsertIntoList(CLinkList<CVisibilityPlugins::AlphaObjectInfo>& list, TObject* obj, float dist, CVisibilityPlugins::RenderFunction callback) {
    CVisibilityPlugins::AlphaObjectInfo info;
    info.m_pObj      = obj;
    info.m_pCallback = callback;
    info.m_distance  = dist;
    return list.InsertSorted(info);
}

// inline
// 0x733D10
bool CVisibilityPlugins::InsertAtomicIntoSortedList(RpAtomic* atomic, float dist) {
    return InsertIntoList(m_alphaList, atomic, dist, &RenderAtomic);
}

// inline
// 0x733D50
bool CVisibilityPlugins::InsertAtomicIntoBoatSortedList(RpAtomic* atomic, float dist) {
    return InsertIntoList(m_alphaBoatAtomicList, atomic, dist, &RenderAtomic);
}

// 0x734570
bool CVisibilityPlugins::InsertEntityIntoSortedList(CEntity* entity, float dist) {
    if (entity->GetModelIndex() == ModelIndices::MI_GRASSHOUSE
        //  || entity->GetModelIndex() == ModelIndices::MI_GASSTATION // In Android
        ) {
        if (InsertEntityIntoReallyDrawLastList(entity, dist)) {
            return true;
        }
    }

    if (entity->m_bUnderwater) {
        return InsertIntoList(m_alphaUnderwaterEntityList, entity, dist, &RenderEntity);
    }

    return InsertIntoList(m_alphaEntityList, entity, dist, &RenderEntity);
}

// 0x733D90
bool CVisibilityPlugins::InsertEntityIntoUnderwaterList(CEntity* entity, float dist) {
    return InsertIntoList(m_alphaUnderwaterEntityList, entity, dist, &RenderEntity);
}

// unused
// 0x733DD0
bool CVisibilityPlugins::InsertObjectIntoSortedList(void* obj, float dist, RenderFunction fn) {
    return InsertIntoList(m_alphaEntityList, obj, dist, fn);
}

// unused
// 0x733E10
bool CVisibilityPlugins::InsertAtomicIntoReallyDrawLastList(RpAtomic* atomic, float dist) {
    return InsertIntoList(m_alphaReallyDrawLastList, atomic, dist, &RenderAtomic);
}

// unused
// 0x733E50
bool CVisibilityPlugins::InsertEntityIntoReallyDrawLastList(CEntity* entity, float dist) {
    return InsertIntoList(m_alphaReallyDrawLastList, entity, dist, &RenderEntity);
}

#define ATOMICPLG_MODELID(atomic) \
    (RWPLUGINOFFSET(tAtomicVisibilityPlugin, atomic, ms_atomicPluginOffset)->m_modelId)

#define ATOMICPLG_FLAGS(atomic) \
    (RWPLUGINOFFSET(tAtomicVisibilityPlugin, atomic, ms_atomicPluginOffset)->m_flags)

#define FRAMEPLG_HIERARCHYID(frame) \
    (RWPLUGINOFFSET(tFrameVisibilityPlugin, frame, ms_framePluginOffset)->m_hierarchyId)

#define CLUMPPLG_VISIBILITYCB(clump) \
    (RWPLUGINOFFSET(tClumpVisibilityPlugin, clump, ms_clumpPluginOffset)->m_visibilityCallBack)

#define CLUMPPLG_ALPHA(clump) \
    (RWPLUGINOFFSET(tClumpVisibilityPlugin, clump, ms_clumpPluginOffset)->m_alpha)

// 0x732E30
bool CVisibilityPlugins::PluginAttach() {
    ms_atomicPluginOffset = RpAtomicRegisterPlugin(
        sizeof(tAtomicVisibilityPlugin),
        MAKECHUNKID(rwVENDORID_DEVELOPER, 0x00),
        AtomicConstructor,
        AtomicDestructor,
        AtomicCopyConstructor
    );
    ms_framePluginOffset = RwFrameRegisterPlugin(
        sizeof(tFrameVisibilityPlugin),
        MAKECHUNKID(rwVENDORID_DEVELOPER, 0x02),
        FrameConstructor,
        FrameDestructor,
        FrameCopyConstructor
    );
    ms_clumpPluginOffset = RpClumpRegisterPlugin(
        sizeof(tClumpVisibilityPlugin),
        MAKECHUNKID(rwVENDORID_DEVELOPER, 0x01),
        ClumpConstructor,
        ClumpDestructor,
        ClumpCopyConstructor
    );
    return ms_atomicPluginOffset != -1 && ms_clumpPluginOffset != -1;
}

// 0x732150
void* CVisibilityPlugins::AtomicConstructor(void* atomic, RwInt32 offset, RwInt32 size) {
    ATOMICPLG_MODELID(atomic) = -1;
    ATOMICPLG_FLAGS(atomic) = 0;
    return atomic;
}

// 0x732170
void* CVisibilityPlugins::AtomicCopyConstructor(void* destAtomic, const void* srcAtomic, RwInt32 offset, RwInt32 size) {
    ATOMICPLG_MODELID(destAtomic) = ATOMICPLG_MODELID(srcAtomic);
    ATOMICPLG_FLAGS(destAtomic) = ATOMICPLG_FLAGS(srcAtomic);
    return destAtomic;
}

// 0x7321A0
void* CVisibilityPlugins::AtomicDestructor(void* atomic, RwInt32 offset, RwInt32 size) {
    return atomic;
}

// 0x732500
int32 CVisibilityPlugins::CalculateFadingAtomicAlpha(CBaseModelInfo* modelInfo, CEntity* entity, float distance) {
    float fFadingDistance = MAX_FADING_DISTANCE;
    float fDrawDistanceRadius = modelInfo->GetColModel()->GetBoundRadius() + CRenderer::ms_fFarClipPlane;
    fDrawDistanceRadius = std::min(fDrawDistanceRadius, TheCamera.m_fLODDistMultiplier * modelInfo->m_fDrawDistance);

    if (!entity->GetLod()) {
        const float fDrawDistance = std::min(modelInfo->m_fDrawDistance, fDrawDistanceRadius);
        if (fDrawDistance > MAX_LOWLOD_DISTANCE) {
            fFadingDistance = fDrawDistance / 15.0f + 10.0f;
        }
        if (entity->m_bIsBIGBuilding) {
            fDrawDistanceRadius *= CRenderer::ms_lowLodDistScale;
        }
    }
    float fFade = std::min((fDrawDistanceRadius + MAX_FADING_DISTANCE - distance) / fFadingDistance, 1.0f);
    return static_cast<int32>(modelInfo->m_nAlpha * fFade);
}

// 0x732310
void CVisibilityPlugins::ClearAtomicFlag(RpAtomic* atomic, uint16 flag) {
    ATOMICPLG_FLAGS(atomic) &= ~flag;
}

// 0x732330
RpAtomic* clearClumpForAllAtomicsFlagCB(RpAtomic* atomic, void* data) {
    CVisibilityPlugins::ClearAtomicFlag(atomic, (int32)data);
    return atomic;
}

// 0x732350
void CVisibilityPlugins::ClearClumpForAllAtomicsFlag(RpClump* clump, int32 id) {
    RpClumpForAllAtomics(clump, clearClumpForAllAtomicsFlagCB, (void*)id);
}

// 0x732E10
void* CVisibilityPlugins::ClumpConstructor(void* clump, RwInt32 offset, RwInt32 size) {
    CLUMPPLG_VISIBILITYCB(clump) = &DefaultVisibilityCB;
    CLUMPPLG_ALPHA(clump) = 255;
    return clump;
}

// 0x732200
void* CVisibilityPlugins::ClumpCopyConstructor(void* destClump, const void* srcClump, RwInt32 offset, RwInt32 size) {
    CLUMPPLG_VISIBILITYCB(destClump) = CLUMPPLG_VISIBILITYCB(srcClump);
    return destClump;
}

// 0x732220
void* CVisibilityPlugins::ClumpDestructor(void* clump, RwInt32 offset, RwInt32 size) {
    return clump;
}

// NOTSA: added clump, like Mobile
// 0x732A30
RwBool CVisibilityPlugins::DefaultVisibilityCB(RpClump* clump) {
    return true;
}

// 0x7321B0
void* CVisibilityPlugins::FrameConstructor(void* frame, RwInt32 offset, RwInt32 size) {
    FRAMEPLG_HIERARCHYID(frame) = 0;
    return frame;
}

// 0x7321D0
void* CVisibilityPlugins::FrameCopyConstructor(void* destFrame, const void* srcFrame, RwInt32 offset, RwInt32 size) {
    FRAMEPLG_HIERARCHYID(destFrame) = FRAMEPLG_HIERARCHYID(srcFrame);
    return destFrame;
}

// 0x7321F0
void* CVisibilityPlugins::FrameDestructor(void* frame, RwInt32 offset, RwInt32 size) {
    return frame;
}

// 0x732A40
RwBool CVisibilityPlugins::FrustumSphereCB(RpClump* clump) {
    RwSphere sphere{};
    RwFrame* frame = RpClumpGetFrame(clump);
    CClumpModelInfo* modelInfo = (CClumpModelInfo*)GetFrameHierarchyId(frame);
    sphere.radius = modelInfo->GetColModel()->GetBoundRadius();
    sphere.center = modelInfo->GetColModel()->GetBoundCenter();
    RwMatrix* transformMatrix = RwFrameGetLTM(frame);
    RwV3dTransformPoints(&sphere.center, &sphere.center, 1, transformMatrix);
    return RwCameraFrustumTestSphere(ms_pCamera, &sphere) != rwSPHEREOUTSIDE;
}

// The function name is misleading, it returns the flags
// 0x732370
int32 CVisibilityPlugins::GetAtomicId(RpAtomic* atomic) {
    return ATOMICPLG_FLAGS(atomic);
}

// 0x732260
CBaseModelInfo* CVisibilityPlugins::GetModelInfo(RpAtomic* atomic) {
    int32 index = ATOMICPLG_MODELID(atomic);
    if (index == -1) {
        return nullptr;
    }
    return CModelInfo::GetModelInfo(index);
}

// 0x732B20
int32 CVisibilityPlugins::GetClumpAlpha(RpClump* clump) {
    return CLUMPPLG_ALPHA(clump);
}

// 0x732AC0
CClumpModelInfo* CVisibilityPlugins::GetClumpModelInfo(RpClump* clump) {
    RwFrame* frame = RpClumpGetFrame(clump);

    return (CClumpModelInfo*)GetFrameHierarchyId(frame);
}

// 0x732C80
float CVisibilityPlugins::GetDistanceSquaredFromCamera(RwFrame* frame) {
    RwMatrix* transformMatrix = RwFrameGetLTM(frame);
    CVector distance;
    RwV3dSub(&distance, &transformMatrix->pos, ms_pCameraPosn);
    return distance.SquaredMagnitude();
}

// 0x732CC0
float CVisibilityPlugins::GetDistanceSquaredFromCamera(RwV3d* pos) {
    CVector distance = *pos - *ms_pCameraPosn;
    return distance.SquaredMagnitude();
}

// 0x7326D0
float CVisibilityPlugins::GetDotProductWithCameraVector(RwMatrix* matrix, RwMatrix* carMatrix, uint32 atomicId) {
    const CVector distance = matrix->pos - *ms_pCameraPosn;
    float primaryDot{};

    if (atomicId & (ATOMIC_REAR | ATOMIC_FRONT)) {
        primaryDot = distance.Dot(carMatrix->up);
    } else if (atomicId & (ATOMIC_RIGHT | ATOMIC_LEFT)) {
        primaryDot = distance.Dot(carMatrix->right);
    }

    // flip sign for Rear or Left components
    if (atomicId & (ATOMIC_REAR | ATOMIC_LEFT)) {
        primaryDot = -primaryDot;
    }

    // adjust for TOP components (Tailgates, Hoods, or Top of doors)
    const bool isDoor = (atomicId & (ATOMIC_FRONT_DOOR | ATOMIC_REAR_DOOR)) != 0;

    const float tailgatMul = 2.5f;
    const float doorsMul = 0.25f;
    if (atomicId & ATOMIC_TOP) {
        const float zDot = distance.Dot(carMatrix->at);
        const float multiplier = isDoor ? doorsMul : tailgatMul;

        primaryDot += zDot * multiplier;
    }

    // if it's not a door, we are done
    if (!isDoor) {
        return primaryDot;
    }

    // secondary calculation specifically for doors
    // This logic usually handles visibility when looking at the car from an angle
    float secondaryDot = 0.0f;

    if (atomicId & ATOMIC_REAR_DOOR) {
        secondaryDot = -distance.Dot(carMatrix->up);
    } else if (atomicId & ATOMIC_FRONT_DOOR) {
        secondaryDot = distance.Dot(carMatrix->up);
    }

    // combine results if both dot products share the same sign (both visible or both hidden)
    const bool bothNegative = (primaryDot < 0.0f && secondaryDot < 0.0f);
    const bool bothPositive = (primaryDot > 0.0f && secondaryDot > 0.0f);

    if (bothNegative || bothPositive) {
        return primaryDot + secondaryDot;
    }

    return primaryDot;
}

// 0x732A20
int32 CVisibilityPlugins::GetFrameHierarchyId(RwFrame* frame) {
    return FRAMEPLG_HIERARCHYID(frame);
}

// 0x732230
void CVisibilityPlugins::SetModelInfoIndex(RpAtomic* atomic, int32 index) {
    ATOMICPLG_MODELID(atomic) = index;
}

// 0x732250
int32 CVisibilityPlugins::GetModelInfoIndex(RpAtomic* atomic) {
    return ATOMICPLG_MODELID(atomic);
}

// 0x7323A0
uint16 CVisibilityPlugins::GetUserValue(RpAtomic* atomic) {
    return ATOMICPLG_FLAGS(atomic);
}

// unused
// 0x732990
bool CVisibilityPlugins::IsAtomicVisible(RpAtomic* atomic) {
    if (atomic->interpolator.flags & rpINTERPOLATORDIRTYSPHERE) {
        _rpAtomicResyncInterpolatedSphere(atomic);
    }
    RwSphere sphere = atomic->boundingSphere;
    RwMatrix* transformMatrix = RwFrameGetMatrix(RpAtomicGetFrame(atomic));
    RwV3d point;
    RwV3dTransformPoints(&point, &sphere.center, 1, transformMatrix);
    return RwCameraFrustumTestSphere(ms_pCamera, &sphere) != rwSPHEREOUTSIDE;
}

// 0x732AE0
bool CVisibilityPlugins::IsClumpVisible(RpClump* clump) {
    return CLUMPPLG_VISIBILITYCB(clump);
}

// 0x732480
void CVisibilityPlugins::RenderAtomicWithAlpha(RpAtomic* atomic, int32 alpha) {
    uint8 alphas[256]; // OG: ~152

    RpGeometry* geometry = RpAtomicGetGeometry(atomic);
    uint32 geometryFlags = RpGeometryGetFlags(geometry);
    RpGeometrySetFlags(geometry, geometryFlags | rpGEOMETRYMODULATEMATERIALCOLOR);

    const int32 numMaterials = RpGeometryGetNumMaterials(geometry);
    for (int32 i = 0; i < numMaterials; i++) {
        RpMaterial* material = RpGeometryGetMaterial(geometry, i);
        RwRGBA* color = RpMaterialGetColor(material);
        alphas[i] = color->alpha;
        color->alpha = std::min(color->alpha, (uint8)alpha);
    }

    AtomicDefaultRenderCallBack(atomic);

    for (int32 i = 0; i < numMaterials; i++) {
        RpMaterial* material = RpGeometryGetMaterial(geometry, i);
        RwRGBA* color = RpMaterialGetColor(material);
        color->alpha = alphas[i];
    }
    RpGeometrySetFlags(geometry, geometryFlags);
}

// 0x733E90
void CVisibilityPlugins::RenderAlphaAtomics() {
    RenderOrderedList(m_alphaList);
}

// 0x732660
RpAtomic* CVisibilityPlugins::RenderAtomicWithAlphaCB(RpAtomic* atomic, void* data) {
    if (RpAtomicGetFlags(atomic) & rpATOMICRENDER) {
        RenderAtomicWithAlpha(atomic, (int32)(uintptr)data); // TODO: maybe
    }
    return atomic;
}

// 0x733EC0
void CVisibilityPlugins::RenderBoatAlphaAtomics() {
    RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));
    RenderOrderedList(m_alphaBoatAtomicList);
    RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLBACK));
}

// dist - unused
// 0x732B30
void CVisibilityPlugins::RenderAtomic(void* obj, float dist) {
    AtomicDefaultRenderCallBack((RpAtomic*)obj);
}

// 0x732B40
void CVisibilityPlugins::RenderEntity(void* obj, float dist) {
    CEntity* entity = (CEntity*)obj;
    if (!entity->GetRwObject()) {
        return;
    }

    CBaseModelInfo* mi = CModelInfo::GetModelInfo(entity->m_nModelIndex);
    if (mi->bDontWriteZBuffer) {
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));
    }

    if (!entity->m_bDistanceFade) {
        if (CGame::currArea || mi->bDontWriteZBuffer) {
            RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(0));
        } else {
            RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(100u));
        }
        CRenderer::RenderOneNonRoad(entity);
    } else {
        RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(0));
        int32 alpha = CalculateFadingAtomicAlpha(mi, entity, dist);
        entity->m_bImBeingRendered = true;
        if (!entity->GetIsBackfaceCulled()) {
            RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));
        }
        bool bLightingSetup = entity->SetupLighting();
        if (RwObjectGetType(entity->GetRwObject()) == rpATOMIC) {
            RenderFadingAtomic(mi, entity->m_pRwAtomic, alpha);
        } else {
            RenderFadingClump(mi, entity->m_pRwClump, alpha);
        }
        entity->RemoveLighting(bLightingSetup);
        entity->m_bImBeingRendered = false;
        if (!entity->GetIsBackfaceCulled()) {
            RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLBACK));
        }
    }

    if (mi->bDontWriteZBuffer) {
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(TRUE));
    }
}

// alpha - unused
// inline
inline void CVisibilityPlugins::SetupRenderFadingAtomic(CBaseModelInfo* modelInfo, int32 alpha) {
    // uint32 newAlphaTest; // unused
    // int32 alphaValue; // unused

    if (modelInfo->bAdditiveRender) {
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDONE));
    }
}

// inline
inline void CVisibilityPlugins::ResetRenderFadingAtomic(CBaseModelInfo* modelInfo) {
    if (modelInfo->bAdditiveRender) {
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
    }
}

// 0x732610
void CVisibilityPlugins::RenderFadingAtomic(CBaseModelInfo* modelInfo, RpAtomic* atomic, int32 alpha) {
    SetupRenderFadingAtomic(modelInfo, alpha);
    RenderAtomicWithAlpha(atomic, alpha);
    ResetRenderFadingAtomic(modelInfo);
}

// 0x732680
void CVisibilityPlugins::RenderFadingClump(CBaseModelInfo* modelInfo, RpClump* clump, int32 alpha) {
    SetupRenderFadingAtomic(modelInfo, alpha);
    RpClumpForAllAtomics(clump, RenderAtomicWithAlphaCB, &alpha);
    ResetRenderFadingAtomic(modelInfo);
}

// 0x733630
RpAtomic* CVisibilityPlugins::RenderFadingClumpCB(RpAtomic* atomic) {
    RpClump* clump = RpAtomicGetClump(atomic);
    uint32 alpha = GetClumpAlpha(clump);

    if (alpha == 255) {
        AtomicDefaultRenderCallBack(atomic);
    } else {
        RenderAtomicWithAlpha(atomic, alpha);
    }
    return atomic;
}

// 0x733F10
void CVisibilityPlugins::RenderFadingEntities() {
    RenderOrderedList(m_alphaEntityList);
    RenderBoatAlphaAtomics();
}

// 0x7337D0
void CVisibilityPlugins::RenderFadingUnderwaterEntities() {
    RenderOrderedList(m_alphaUnderwaterEntityList);
}

// NOTSA:
// Helper to calculate Dot Product and Atomic ID commonly used in vehicle rendering
// Returns Dot Product. Outputs Atomic ID.
float CVisibilityPlugins::GetVehicleDotProduct(RpAtomic* atomic, uint32& outAtomicId) {
    RwFrame* frame = RpAtomicGetFrame(atomic);
    RpClump* clump = RpAtomicGetClump(atomic);
    RwFrame* carFrame = RpClumpGetFrame(clump);

    RwMatrix* matrix = RwFrameGetLTM(frame);
    RwMatrix* carMatrix = RwFrameGetLTM(carFrame);

    outAtomicId = GetAtomicId(atomic);
    return GetDotProductWithCameraVector(matrix, carMatrix, outAtomicId);
}

// NOTSA:
// Common Culling Logic based on the provided reference code.
// Returns TRUE if the object should be CULLED (Skipped), FALSE if it should be drawn.
bool CVisibilityPlugins::ShouldCullVehicleAtomic(float distFromCam, float cullDist, float dotProduct, uint32 atomicId, bool bUseComplexHeuristic) {
    // Condition: Vehicle is far enough, not forced visible, and viewed from a flat angle
    if (distFromCam > cullDist && !(atomicId & ATOMIC_DONT_CULL) && gAngleWithHorizontal < 0.2f) {
        // If on the wrong side of the vehicle cull...
        if (dotProduct > 0.0f) {
            // If using complex heuristic (Size check / Flat check)
            if (bUseComplexHeuristic) {
                if ((atomicId & ATOMIC_FLAT) || (sq(dotProduct) > distFromCam * 0.1f)) {
                    return true; // Cull it
                }
            } else {
                // Simple heuristic: just check facing
                return true; // Cull it
            }
        }
    }
    return false; // Do not cull
}

// 0x7340B0
RpAtomic* CVisibilityPlugins::RenderHeliRotorAlphaCB(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera < ms_bigVehicleLod0Dist) {
        uint32 atomicId;
        float dotProduct = GetVehicleDotProduct(atomic, atomicId);

        // Reference: dotProduct * 20.0f
        if (!InsertAtomicIntoSortedList(atomic, gVehicleDistanceFromCamera + dotProduct * 20.0f)) {
            AtomicDefaultRenderCallBack(atomic);
        }
    }
    return atomic;
}

// 0x734170
RpAtomic* CVisibilityPlugins::RenderHeliTailRotorAlphaCB(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera < ms_vehicleLod0Dist) {
        RwMatrix* atomicMatrix = RwFrameGetLTM(RpAtomicGetFrame(atomic));
        RwMatrix* clumpMatrix = RwFrameGetLTM(RpClumpGetFrame(RpAtomicGetClump(atomic)));
        
        CVector distance = atomicMatrix->pos - *ms_pCameraPosn;
        const float dotProduct1 = distance.Dot(clumpMatrix->right);
        const float dotProduct2 = distance.Dot(clumpMatrix->up);
        
        if (!InsertAtomicIntoSortedList(atomic, gVehicleDistanceFromCamera - dotProduct1 - dotProduct2)) {
            AtomicDefaultRenderCallBack(atomic);
        }
    }
    return atomic;
}

// unused
// 0x7323E0
RpAtomic* CVisibilityPlugins::RenderObjNormalAtomic(RpAtomic* atomic) {
    RwMatrix* atomicMatrix = RwFrameGetLTM(RpAtomicGetFrame(atomic));
    CVector distance = atomicMatrix->pos - *ms_pCameraPosn;
    const float length = RwV3dLength(&distance);
    if (distance.Dot(atomicMatrix->up) < length * -0.3f && length > 8.0f) {
        return atomic;
    }
    AtomicDefaultRenderCallBack(atomic);
    return atomic;
}

// 0x7337A0
void CVisibilityPlugins::RenderOrderedList(CLinkList<CVisibilityPlugins::AlphaObjectInfo>& alphaObjectInfoList) {
    auto link = alphaObjectInfoList.usedListTail.prev;
    for (; link != &alphaObjectInfoList.usedListHead; link = link->prev) {
        auto callBack = reinterpret_cast<RenderFunction>(link->data.m_pCallback);
        callBack(link->data.m_pObj, link->data.m_distance);
    }
}

// 0x7335B0
RpAtomic* CVisibilityPlugins::RenderPedCB(RpAtomic* atomic) {
    RpClump* clump = RpAtomicGetClump(atomic);
    float len = GetDistanceSquaredFromCamera(RpClumpGetFrame(clump));

    if (len < ms_pedLodDist) {
        const auto alpha = GetClumpAlpha(clump);
        if (alpha == 255) {
            AtomicDefaultRenderCallBack(atomic);
        } else {
            RenderAtomicWithAlpha(atomic, alpha);
        }
    }
    return atomic;
}

// 0x732850
RpMaterial* SetTextureCB(RpMaterial* material, void* texture) {
    RpMaterialSetTexture(material, (RwTexture*)texture);
    return material;
}

// 0x732870
RpAtomic* CVisibilityPlugins::RenderPlayerCB(RpAtomic* atomic) {
    RwTexture* skinTexture = CWorld::Players[0].m_pSkinTexture;
    if (skinTexture) {
        RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), SetTextureCB, skinTexture);
    }
    AtomicDefaultRenderCallBack(atomic);
    return atomic;
}

// 0x733800
void CVisibilityPlugins::RenderReallyDrawLastObjects() {
    ZoneScoped;

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATECULLMODE,          RWRSTATE(rwCULLMODECULLNONE));

    SetAmbientColours();
    DeActivateDirectional();
    RenderOrderedList(m_alphaReallyDrawLastList);

    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
}

// 0x733240
RpAtomic* CVisibilityPlugins::RenderVehicleHiDetailCB(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera < ms_vehicleLod0Dist) {
        RenderVehicleCB_ControlRenderMultiPassLOD(atomic, gVehicleDistanceFromCamera);
        
        uint32 atomicId;
        float dotProduct = GetVehicleDotProduct(atomic, atomicId);

        // cullCompsDist, Complex Heuristic = true (Size Check)
        if (ShouldCullVehicleAtomic(gVehicleDistanceFromCamera, ms_cullCompsDist, dotProduct, atomicId, true)) {
            return atomic; // Cull
        }
        
        AtomicDefaultRenderCallBack(atomic);
    }
    return atomic;
}

// 0x733F80
RpAtomic* CVisibilityPlugins::RenderVehicleHiDetailAlphaCB(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera < ms_vehicleLod0Dist) {
        RenderVehicleCB_ControlRenderMultiPassLOD(atomic, gVehicleDistanceFromCamera);
        
        uint32 atomicId;
        float dotProduct = GetVehicleDotProduct(atomic, atomicId);
        
        // complex heuristic = true
        if (ShouldCullVehicleAtomic(gVehicleDistanceFromCamera, ms_cullCompsDist, dotProduct, atomicId, true)) {
            return atomic; // Cull
        }

        if (atomicId & ATOMIC_ALPHA) {
            // Ref: gVehicleDistanceFromCamera-0.0001f
            if (!InsertAtomicIntoSortedList(atomic, gVehicleDistanceFromCamera - 0.0001f)) {
                AtomicDefaultRenderCallBack(atomic);
            }
            return atomic;
        }

        if (!InsertAtomicIntoSortedList(atomic, gVehicleDistanceFromCamera + dotProduct)) {
            AtomicDefaultRenderCallBack(atomic);
        }
    }
    return atomic;
}

// 0x733330
RpAtomic* CVisibilityPlugins::RenderTrainHiDetailCB(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera < ms_bigVehicleLod0Dist) {
        RenderVehicleCB_ControlRenderMultiPassLOD(atomic, gVehicleDistanceFromCamera);
        
        uint32 atomicId;
        float dotProduct = GetVehicleDotProduct(atomic, atomicId);

        if (ShouldCullVehicleAtomic(gVehicleDistanceFromCamera, ms_cullCompsDist, dotProduct, atomicId, true)) {
            return atomic;
        }

        AtomicDefaultRenderCallBack(atomic);
    }
    return atomic;
}

// 0x734240
RpAtomic* CVisibilityPlugins::RenderTrainHiDetailAlphaCB(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera < ms_bigVehicleLod0Dist) {
        RenderVehicleCB_ControlRenderMultiPassLOD(atomic, gVehicleDistanceFromCamera);
        
        uint32 atomicId;
        float dotProduct = GetVehicleDotProduct(atomic, atomicId);

        if (ShouldCullVehicleAtomic(gVehicleDistanceFromCamera, ms_cullCompsDist, dotProduct, atomicId, true)) {
            return atomic;
        }

        if (atomicId & ATOMIC_ALPHA) {
            // Reference: just gVehicleDistanceFromCamera (no epsilon subtraction)
            if (!InsertAtomicIntoSortedList(atomic, gVehicleDistanceFromCamera)) {
                AtomicDefaultRenderCallBack(atomic);
            }
            return atomic;
        }

        if (!InsertAtomicIntoSortedList(atomic, gVehicleDistanceFromCamera + dotProduct)) {
            AtomicDefaultRenderCallBack(atomic);
        }
    }
    return atomic;
}

// 0x732820
RpAtomic* CVisibilityPlugins::RenderVehicleReallyLowDetailCB_BigVehicle(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera >= ms_bigVehicleLod0Dist) {
        AtomicDefaultRenderCallBack(atomic);
    }
    return atomic;
}

// 0x733420
RpAtomic* CVisibilityPlugins::RenderVehicleHiDetailCB_BigVehicle(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera < ms_bigVehicleLod0Dist) {
        RenderVehicleCB_ControlRenderMultiPassLOD(atomic, gVehicleDistanceFromCamera);
        
        uint32 atomicId;
        float dotProduct = GetVehicleDotProduct(atomic, atomicId);

        // NOTE: BigVehicle Immedate Mode Uses SIMPLE Heuristic (False) in reference
        if (ShouldCullVehicleAtomic(gVehicleDistanceFromCamera, ms_cullBigCompsDist, dotProduct, atomicId, false)) {
            return atomic;
        }

        AtomicDefaultRenderCallBack(atomic);
    }
    return atomic;
}

// 0x734370
RpAtomic* CVisibilityPlugins::RenderVehicleHiDetailAlphaCB_BigVehicle(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera < ms_bigVehicleLod0Dist) {
        RenderVehicleCB_ControlRenderMultiPassLOD(atomic, gVehicleDistanceFromCamera);
        
        uint32 atomicId;
        float dotProduct = GetVehicleDotProduct(atomic, atomicId);
        
        // NOTE: BigVehicle Alpha Mode Uses COMPLEX Heuristic (True) in provided reference code
        // (dotProduct * dotProduct > gVehicleDistanceFromCamera * 0.1f) check is present
        if (ShouldCullVehicleAtomic(gVehicleDistanceFromCamera, ms_cullBigCompsDist, dotProduct, atomicId, true)) {
            return atomic;
        }

        if (atomicId & ATOMIC_ALPHA) {
            if (!InsertAtomicIntoSortedList(atomic, gVehicleDistanceFromCamera - 0.0001f)) {
                AtomicDefaultRenderCallBack(atomic);
            }
            return atomic;
        }

        if (!InsertAtomicIntoSortedList(atomic, gVehicleDistanceFromCamera + dotProduct)) {
            AtomicDefaultRenderCallBack(atomic);
        }
    }
    return atomic;
}

// the same as RenderVehicleReallyLowDetailCB
// 0x7334F0
RpAtomic* CVisibilityPlugins::RenderVehicleLoDetailCB_Boat(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera >= ms_vehicleLod0Dist) {
        SetAtomicFlag(atomic, ATOMIC_UNIQUE_MATERIALS);
        if (GetClumpAlpha(RpAtomicGetClump(atomic)) == 255) {
            AtomicDefaultRenderCallBack(atomic);
        } else {
            RenderAtomicWithAlpha(atomic, GetClumpAlpha(RpAtomicGetClump(atomic)));
        }
    }

    return atomic;
}

// the same as RenderVehicleLoDetailCB_Boat
// 0x7331E0
RpAtomic* CVisibilityPlugins::RenderVehicleReallyLowDetailCB(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera >= ms_vehicleLod0Dist) {
        SetAtomicFlag(atomic, ATOMIC_UNIQUE_MATERIALS);
        if (GetClumpAlpha(RpAtomicGetClump(atomic)) == 255) {
            AtomicDefaultRenderCallBack(atomic);
        } else {
            RenderAtomicWithAlpha(atomic, GetClumpAlpha(RpAtomicGetClump(atomic)));
        }
    }

    return atomic;
}

// 0x733550
RpAtomic* CVisibilityPlugins::RenderVehicleHiDetailCB_Boat(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera < ms_vehicleLod0Dist) {
        RenderVehicleCB_ControlRenderMultiPassLOD(atomic, gVehicleDistanceFromCamera);
        AtomicDefaultRenderCallBack(atomic);
    }
    return atomic;
}

// 0x7344A0
RpAtomic* CVisibilityPlugins::RenderVehicleHiDetailAlphaCB_Boat(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera < ms_vehicleLod0Dist) {
        RenderVehicleCB_ControlRenderMultiPassLOD(atomic, gVehicleDistanceFromCamera);
        
        uint32 atomicId = CVisibilityPlugins::GetAtomicId(atomic);
        if (atomicId & ATOMIC_ALPHA) {
            if (!InsertAtomicIntoBoatSortedList(atomic, gVehicleDistanceFromCamera)) {
                AtomicDefaultRenderCallBack(atomic);
            }
        } else {
            AtomicDefaultRenderCallBack(atomic);
        }
    }
    return atomic;
}

// 0x733670
RpAtomic* CVisibilityPlugins::RenderWeaponCB(RpAtomic* atomic) {
    RpClump* clump = RpAtomicGetClump(atomic);
    CClumpModelInfo* mi = GetClumpModelInfo(clump);
    float len = GetDistanceSquaredFromCamera(RpClumpGetFrame(clump));
    float lodDist = TheCamera.m_fLODDistMultiplier * mi->m_fDrawDistance;
    if (len < sq(lodDist)) {
        AtomicDefaultRenderCallBack(atomic);
    }
    return atomic;
}

// 0x732F30
void CVisibilityPlugins::RenderWeaponPedsForPC() {
    ZoneScoped;

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,            RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,             RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,            RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(20));
    auto link = ms_weaponPedsForPC.usedListTail.prev;
    for (; link != &ms_weaponPedsForPC.usedListHead; link = link->prev) {
        CPed* ped = link->data;
        if (ped && ped->m_pWeaponObject) {
            ped->SetupLighting();
            const CWeapon& activeWeapon = ped->GetActiveWeapon();
            RpHAnimHierarchy* pRpAnimHierarchy = GetAnimHierarchyFromSkinClump(ped->m_pRwClump);
            const int32 boneID = activeWeapon.m_Type != WEAPON_PARACHUTE ? BONE_R_HAND : BONE_SPINE1;
            int32 animIDIndex = RpHAnimIDGetIndex(pRpAnimHierarchy, boneID);
            RwMatrix* pRightHandMatrix = &RpHAnimHierarchyGetMatrixArray(pRpAnimHierarchy)[animIDIndex];
            { // todo: NOTSA
                if (boneID == BONE_ROOT) {
                    pRightHandMatrix = ped->GetModellingMatrix();
                }
            }
            RwFrame* weaponFrame = RpClumpGetFrame(ped->m_pWeaponObject);
            RwMatrix* weaponRwMatrix = RwFrameGetMatrix(weaponFrame);
            memcpy(weaponRwMatrix, pRightHandMatrix, sizeof(RwMatrixTag));
            if (activeWeapon.m_Type == WEAPON_PARACHUTE) {
                static RwV3d rightWeaponTranslate = { 0.1f, -0.15f, 0.0f };
                RwMatrixTranslate(weaponRwMatrix, &rightWeaponTranslate, rwCOMBINEPRECONCAT);
                RwMatrixRotate(weaponRwMatrix, &CPedIK::YaxisIK, 90.0f, rwCOMBINEPRECONCAT);
            }
            ped->SetGunFlashAlpha(false);
            RwFrameUpdateObjects(weaponFrame);
            RpClumpRender(ped->m_pWeaponObject);
            eWeaponSkill weaponSkill = ped->GetWeaponSkill();
            if (CWeaponInfo::GetWeaponInfo(activeWeapon.m_Type, weaponSkill)->flags.bTwinPistol) {
                int32 animIDIndex = RpHAnimIDGetIndex(pRpAnimHierarchy, BONE_L_HAND);
                RwMatrix* pLeftHandMatrix = &RpHAnimHierarchyGetMatrixArray(pRpAnimHierarchy)[animIDIndex];
                memcpy(weaponRwMatrix, pLeftHandMatrix, sizeof(RwMatrixTag));
                RwMatrixRotate(weaponRwMatrix, &CPedIK::XaxisIK, 180.0f, rwCOMBINEPRECONCAT); // axis, 0x8D60A4
                static RwV3d leftWeaponTranslate = { 0.04f, -0.05f, 0.0f }; // trans, 0x8D6098
                RwMatrixTranslate(weaponRwMatrix, &leftWeaponTranslate, rwCOMBINEPRECONCAT);
                ped->SetGunFlashAlpha(true);
                RwFrameUpdateObjects(weaponFrame);
                RpClumpRender(ped->m_pWeaponObject);
            }
            ped->ResetGunFlashAlpha();
        }
    }
}

// 0x7322B0
void CVisibilityPlugins::SetAtomicFlag(RpAtomic* atomic, uint16 flag) {
    ATOMICPLG_FLAGS(atomic) |= flag;
}

// optimization?
// 0x7322D0
RpAtomic* setClumpForAllAtomicsFlagCB(RpAtomic* atomic, void* data) {
    CVisibilityPlugins::SetAtomicFlag(atomic, int32(data));
    return atomic;
}

// 0x7322F0
void CVisibilityPlugins::SetClumpForAllAtomicsFlag(RpClump* clump, int32 id) {
    // like Mobile (in PC SetAtomicFlag)
    RpClumpForAllAtomics(clump, setClumpForAllAtomicsFlagCB, (void*)id);
}

// 0x732290
void CVisibilityPlugins::SetAtomicId(RpAtomic* atomic, int32 id) {
    ATOMICPLG_FLAGS(atomic) = id;
}

// 0x7328A0
void CVisibilityPlugins::SetAtomicRenderCallback(RpAtomic* atomic, RpAtomicCallBackRender renderCB) {
    if (!renderCB) {
        renderCB = AtomicDefaultRenderCallBack;
    }
    RpAtomicSetRenderCallBack(atomic, renderCB);
}

// 0x732B00
void CVisibilityPlugins::SetClumpAlpha(RpClump* clump, int32 alpha) {
    CLUMPPLG_ALPHA(clump) = alpha;
}

// 0x733750
void CVisibilityPlugins::SetClumpModelInfo(RpClump* clump, CClumpModelInfo* clumpModelInfo) {
    RwFrame* frame = RpClumpGetFrame(clump);

    SetFrameHierarchyId(frame, (int32)clumpModelInfo);
    if (clumpModelInfo->GetModelType() != MODEL_INFO_VEHICLE) {
        return;
    }

    CVehicleModelInfo* mi = clumpModelInfo->AsVehicleModelInfoPtr();
    if (mi->IsTrain() || mi->m_nVehicleType == VEHICLE_TYPE_FPLANE) {
        CLUMPPLG_VISIBILITYCB(clump) = &VehicleVisibilityCB_BigVehicle;
    } else {
        CLUMPPLG_VISIBILITYCB(clump) = &VehicleVisibilityCB;
    }
}

// 0x732A00
void CVisibilityPlugins::SetFrameHierarchyId(RwFrame* frame, int32 id) {
    FRAMEPLG_HIERARCHYID(frame) = id;
}

// 0x7328C0
void CVisibilityPlugins::SetRenderWareCamera(RwCamera* camera) {
    ms_pCamera = camera;
    ms_pCameraPosn = RwMatrixGetPos(RwFrameGetMatrix(RwCameraGetFrame(camera)));

    ms_cullCompsDist = std::powf(TheCamera.m_fLODDistMultiplier * 20.0f, 2.0f);
    ms_cullBigCompsDist = std::powf(TheCamera.m_fLODDistMultiplier * 50.0f, 2.0f);

    ms_vehicleLod0RenderMultiPassDist = std::powf(TheCamera.m_fGenerationDistMultiplier * 45.0f, 2.0f);
    ms_vehicleLod0RenderMultiPassDist += ms_vehicleLod0RenderMultiPassDist;

    ms_vehicleLod0Dist = std::powf(TheCamera.m_fGenerationDistMultiplier * 70.0f, 2.0f);
    ms_vehicleLod0Dist += ms_vehicleLod0Dist;

    float vehicleLodDistance = std::powf(TheCamera.m_fGenerationDistMultiplier * 150.0f, 2.0f);
    vehicleLodDistance += vehicleLodDistance;
    ms_vehicleLod1Dist = vehicleLodDistance;
    ms_bigVehicleLod0Dist = vehicleLodDistance;

    ms_pedLodDist = std::powf(TheCamera.m_fLODDistMultiplier * 60.0f, 2.0f);
    ms_pedLodDist += ms_pedLodDist;

    ms_pedFadeDist = std::powf(TheCamera.m_fLODDistMultiplier * 70.0f, 2.0f);
    ms_pedFadeDist += ms_pedFadeDist;
}

// 0x732380
void CVisibilityPlugins::SetUserValue(RpAtomic* atomic, uint16 value) {
    ATOMICPLG_FLAGS(atomic) = value;
}

// 0x733160
void CVisibilityPlugins::SetupVehicleVariables(RpClump* clump) {
    if (RwObjectGetType(clump) != rpCLUMP) {
        return;
    }

    RwFrame* carFrame = RpClumpGetFrame(clump);
    RwMatrix* transformMatrix = RwFrameGetLTM(carFrame);
    CVector distance1;
    RwV3dSub(&distance1, &transformMatrix->pos, ms_pCameraPosn);
    gVehicleDistanceFromCamera = distance1.SquaredMagnitude();
    CVector2D distance2;
    RwV2dSub(&distance2, ms_pCameraPosn, RwMatrixGetPos(RwFrameGetMatrix(carFrame)));
    gAngleWithHorizontal = atan2(ms_pCameraPosn->z - RwMatrixGetPos(RwFrameGetMatrix(carFrame))->z, distance2.Magnitude());
}

// unused
// 0x7323B0
RpMaterial* SetAlphaCB(RpMaterial* material, void* data) {
    material->color.alpha = (RwUInt8)(uintptr)data;
    return material;
}

// 0x7323C0
RpAtomic* CVisibilityPlugins::RenderWheelAtomicCB(RpAtomic* atomic) {
    // CAtomicModelInfo* pModelInfo; // unused

    AtomicDefaultRenderCallBack(atomic);
    return atomic;
}

// 0x7336F0
RwBool CVisibilityPlugins::VehicleVisibilityCB(RpClump* clump) {
    RwFrame* frame = RpClumpGetFrame(clump);

    const float len = GetDistanceSquaredFromCamera(frame);
    if (len <= ms_vehicleLod1Dist) {
        return FrustumSphereCB(clump);
    }

    return false;
}

// 0x732AB0
RwBool CVisibilityPlugins::VehicleVisibilityCB_BigVehicle(RpClump* clump) {
    // RwFrame* pFrame; // unused

    return FrustumSphereCB(clump);
}

