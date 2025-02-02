/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "VisibilityPlugins.h"

int32& CVisibilityPlugins::ms_atomicPluginOffset = *(int32*)0x8D608C;
int32& CVisibilityPlugins::ms_clumpPluginOffset = *(int32*)0x8D6094;
int32& CVisibilityPlugins::ms_framePluginOffset = *(int32*)0x8D6090;

CLinkList<CVisibilityPlugins::AlphaObjectInfo>& CVisibilityPlugins::m_alphaEntityList = *(CLinkList<CVisibilityPlugins::AlphaObjectInfo>*)0xC88120;
CLinkList<CVisibilityPlugins::AlphaObjectInfo>& CVisibilityPlugins::m_alphaList = *(CLinkList<CVisibilityPlugins::AlphaObjectInfo>*)0xC88070;
CLinkList<CVisibilityPlugins::AlphaObjectInfo>& CVisibilityPlugins::m_alphaBoatAtomicList = *(CLinkList<CVisibilityPlugins::AlphaObjectInfo>*)0xC880C8;
CLinkList<CVisibilityPlugins::AlphaObjectInfo>& CVisibilityPlugins::m_alphaUnderwaterEntityList = *(CLinkList<CVisibilityPlugins::AlphaObjectInfo>*)0xC88178;
CLinkList<CVisibilityPlugins::AlphaObjectInfo>& CVisibilityPlugins::m_alphaReallyDrawLastList = *(CLinkList<CVisibilityPlugins::AlphaObjectInfo>*)0xC881D0;
CLinkList<CPed*>& CVisibilityPlugins::ms_weaponPedsForPC = *(CLinkList<CPed*>*)0xC88224;
RwCamera*& CVisibilityPlugins::ms_pCamera = *(RwCamera**)0xC8804C;
RwV3d*& CVisibilityPlugins::ms_pCameraPosn = *(RwV3d**)0xC88050;
float& CVisibilityPlugins::ms_cullCompsDist = *(float*)0x0C8802C;
float& CVisibilityPlugins::ms_cullBigCompsDist = *(float*)0x0C88028;
float& CVisibilityPlugins::ms_vehicleLod0RenderMultiPassDist = *(float*)0xC88044;
float& CVisibilityPlugins::ms_vehicleLod0Dist = *(float*)0xC88040;
float& CVisibilityPlugins::ms_vehicleLod1Dist = *(float*)0xC8803C;
float& CVisibilityPlugins::ms_bigVehicleLod0Dist = *(float*)0xC88038;
float& CVisibilityPlugins::ms_pedLodDist = *(float*)0xC88034;
float& CVisibilityPlugins::ms_pedFadeDist = *(float*)0xC88030;
float& CVisibilityPlugins::gVehicleDistanceFromCamera = *(float*)0xC88024;
float& CVisibilityPlugins::gVehicleAngleToCamera = *(float*)0xC88020;

void CVisibilityPlugins::InjectHooks() {
    RH_ScopedClass(CVisibilityPlugins);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x733A20);
    RH_ScopedInstall(Shutdown, 0x732EB0);
    RH_ScopedInstall(AtomicCopyConstructor, 0x732170);
    RH_ScopedInstall(AtomicDestructor, 0x7321A0);
    RH_ScopedInstall(CalculateFadingAtomicAlpha, 0x732500);

    RH_ScopedOverloadedInstall(ClearAtomicFlag, "int32", 0x732310, void(*)(RpAtomic*, int32));
    RH_ScopedOverloadedInstall(ClearAtomicFlag, "uint16", 0x732330, void(*)(RpAtomic*, uint16));
    RH_ScopedOverloadedInstall(SetAtomicFlag, "int32", 0x7322D0, void(*)(RpAtomic*, int32));
    RH_ScopedOverloadedInstall(SetAtomicFlag, "uint16", 0x7322B0, void(*)(RpAtomic*, uint16));

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
    RH_ScopedInstall(GetAtomicModelInfo, 0x732260);
    RH_ScopedInstall(GetClumpAlpha, 0x732B20);
    RH_ScopedInstall(GetClumpModelInfo, 0x732AC0);
    RH_ScopedOverloadedInstall(GetDistanceSquaredFromCamera, "0", 0x732C80, float (*)(RwFrame*));
    RH_ScopedOverloadedInstall(GetDistanceSquaredFromCamera, "1", 0x732CC0, float (*)(const CVector*));
    RH_ScopedInstall(GetDotProductWithCameraVector, 0x7326D0);
    RH_ScopedInstall(GetFrameHierarchyId, 0x732A20);
    RH_ScopedInstall(GetModelInfoIndex, 0x732250);
    RH_ScopedInstall(GetUserValue, 0x7323A0);
    RH_ScopedInstall(InitAlphaAtomicList, 0x734530);
    RH_ScopedInstall(InitAlphaEntityList, 0x734540);
    RH_ScopedInstall(InsertEntityIntoEntityList, 0x733DD0);
    RH_ScopedInstall(InsertEntityIntoUnderwaterEntities, 0x733D90);
    RH_ScopedInstall(InsertAtomicIntoReallyDrawLastList, 0x733E10);
    RH_ScopedInstall(InsertEntityIntoReallyDrawLastList, 0x733E50);
    RH_ScopedInstall(InsertEntityIntoSortedList, 0x734570);
    RH_ScopedInstall(IsAtomicVisible, 0x732990);
    RH_ScopedInstall(PluginAttach, 0x732E30);
    RH_ScopedInstall(RenderAlphaAtomic, 0x732480);
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
    RH_ScopedInstall(SetAtomicId, 0x732230);
    RH_ScopedInstall(SetAtomicRenderCallback, 0x7328A0);
    RH_ScopedInstall(SetClumpAlpha, 0x732B00);
    RH_ScopedInstall(SetClumpModelInfo, 0x733750);
    RH_ScopedInstall(SetFrameHierarchyId, 0x732A00);
    RH_ScopedInstall(SetRenderWareCamera, 0x7328C0);
    RH_ScopedInstall(SetUserValue, 0x732380);
    RH_ScopedInstall(SetupVehicleVariables, 0x733160);
    RH_ScopedInstall(VehicleVisibilityCB, 0x7336F0);
    RH_ScopedInstall(VehicleVisibilityCB_BigVehicle, 0x732AB0);
}

void CVisibilityPlugins::Initialise() {
    m_alphaList.Init(TOTAL_ALPHA_LISTS);
    m_alphaList.usedListHead.data.m_distance = 0.0f;
    m_alphaList.usedListTail.data.m_distance = 100000000.0f;

    m_alphaBoatAtomicList.Init(TOTAL_ALPHA_BOAT_ATOMIC_LISTS);
    m_alphaBoatAtomicList.usedListHead.data.m_distance = 0.0f;
    m_alphaBoatAtomicList.usedListTail.data.m_distance = 100000000.0f;

    m_alphaEntityList.Init(TOTAL_ALPHA_ENTITY_LISTS);
    m_alphaEntityList.usedListHead.data.m_distance = 0.0f;
    m_alphaEntityList.usedListTail.data.m_distance = 100000000.0f;

    m_alphaUnderwaterEntityList.Init(TOTAL_ALPHA_UNDERWATER_ENTITY_LISTS);
    m_alphaUnderwaterEntityList.usedListHead.data.m_distance = 0.0f;
    m_alphaUnderwaterEntityList.usedListTail.data.m_distance = 100000000.0f;

    // New in SA
    m_alphaReallyDrawLastList.Init(TOTAL_ALPHA_DRAW_LAST_LISTS);
    m_alphaReallyDrawLastList.usedListHead.data.m_distance = 0.0f;
    m_alphaReallyDrawLastList.usedListTail.data.m_distance = 100000000.0f;

    ms_weaponPedsForPC.Init(TOTAL_WEAPON_PEDS_FOR_PC);
}

void CVisibilityPlugins::Shutdown() {
    m_alphaList.Shutdown();
    m_alphaBoatAtomicList.Shutdown();
    m_alphaEntityList.Shutdown();
    m_alphaUnderwaterEntityList.Shutdown();
    m_alphaReallyDrawLastList.Shutdown();
    ms_weaponPedsForPC.Shutdown();
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

// 0x733DD0
bool CVisibilityPlugins::InsertEntityIntoEntityList(CEntity* entity, float distance, void* callback)
{
    AlphaObjectInfo info{};
    info.m_entity = entity;
    info.m_pCallback = callback;
    info.m_distance = distance;
    return m_alphaEntityList.InsertSorted(info);
}

// 0x733D90
bool CVisibilityPlugins::InsertEntityIntoUnderwaterEntities(CEntity* entity, float distance)
{
    AlphaObjectInfo info{};
    info.m_distance = distance;
    info.m_entity = entity;
    info.m_pCallback = RenderEntity;
    return m_alphaUnderwaterEntityList.InsertSorted(info);
}

// 0x733E10
bool CVisibilityPlugins::InsertAtomicIntoReallyDrawLastList(RpAtomic* atomic, float distance) {
    AlphaObjectInfo info{};
    info.m_distance = distance;
    info.m_atomic = atomic;
    info.m_pCallback = DefaultAtomicRenderCallback;
    return m_alphaReallyDrawLastList.InsertSorted(info);
}

// 0x733E50
bool CVisibilityPlugins::InsertEntityIntoReallyDrawLastList(CEntity* entity, float distance) {
    AlphaObjectInfo info{};
    info.m_distance = distance;
    info.m_entity = entity;
    info.m_pCallback = RenderEntity;
    return m_alphaReallyDrawLastList.InsertSorted(info);
}

// todo: Add MI_GASSTATION (see Android)
// 0x734570
bool CVisibilityPlugins::InsertEntityIntoSortedList(CEntity* entity, float distance) {
    if (entity->m_nModelIndex == ModelIndices::MI_GRASSHOUSE) {
        if (InsertEntityIntoReallyDrawLastList(entity, distance)) {
            return true;
        }
    }

    if (entity->m_bUnderwater) {
        return InsertEntityIntoUnderwaterEntities(entity, distance);
    }

    return InsertEntityIntoEntityList(entity, distance, RenderEntity);
}

#define ATOMICPLG(atomic, var) \
    (RWPLUGINOFFSET(tAtomicVisibilityPlugin, atomic, ms_atomicPluginOffset)->var)

#define ATOMICPLGCONST(atomic, var) \
    (RWPLUGINOFFSETCONST(tAtomicVisibilityPlugin, atomic, ms_atomicPluginOffset)->var)

#define FRAMEPLG(frame, var) \
    (RWPLUGINOFFSET(tFrameVisibilityPlugin, frame, ms_framePluginOffset)->var)

#define FRAMEPLGCONST(frame, var) \
    (RWPLUGINOFFSETCONST(tFrameVisibilityPlugin, frame, ms_framePluginOffset)->var)

#define CLUMPPLG(clump, var) \
    (RWPLUGINOFFSET(tClumpVisibilityPlugin, clump, ms_clumpPluginOffset)->var)

#define CLUMPPLGCONST(clump, var) \
    (RWPLUGINOFFSETCONST(tClumpVisibilityPlugin, clump, ms_clumpPluginOffset)->var)

// 0x732E30
bool CVisibilityPlugins::PluginAttach() {
    ms_atomicPluginOffset = RpAtomicRegisterPlugin(
        sizeof(tAtomicVisibilityPlugin),
        MAKECHUNKID(rwVENDORID_DEVELOPER, 0x00),
        AtomicConstructor,
        AtomicDestructor,
        AtomicCopyConstructor
    );
    ms_clumpPluginOffset = RpClumpRegisterPlugin(
        sizeof(tClumpVisibilityPlugin),
        MAKECHUNKID(rwVENDORID_DEVELOPER, 0x01),
        ClumpConstructor,
        ClumpDestructor,
        ClumpCopyConstructor
    );
    ms_framePluginOffset = RwFrameRegisterPlugin(
        sizeof(tFrameVisibilityPlugin),
        MAKECHUNKID(rwVENDORID_DEVELOPER, 0x02),
        FrameConstructor,
        FrameDestructor,
        FrameCopyConstructor
    );
    return ms_atomicPluginOffset != -1 && ms_clumpPluginOffset != -1;
}

RpAtomic* CVisibilityPlugins::DefaultAtomicRenderCallback(RpAtomic* atomic)
{
    return AtomicDefaultRenderCallBack(atomic);
}

void* CVisibilityPlugins::AtomicConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    ATOMICPLG(object, m_modelId) = -1;
    ATOMICPLG(object, m_flags) = 0;
    return object;
}

void* CVisibilityPlugins::AtomicCopyConstructor(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject)
{
    ATOMICPLG(dstObject, m_modelId) = ATOMICPLGCONST(srcObject, m_modelId);
    ATOMICPLG(dstObject, m_flags) = ATOMICPLGCONST(srcObject, m_flags);
    return dstObject;
}

void* CVisibilityPlugins::AtomicDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    return object;
}

int32 CVisibilityPlugins::CalculateFadingAtomicAlpha(CBaseModelInfo* modelInfo, CEntity* entity, float distance) {
    float fFadingDistance = MAX_FADING_DISTANCE;
    float fDrawDistanceRadius = modelInfo->GetColModel()->GetBoundRadius() + CRenderer::ms_fFarClipPlane;
    fDrawDistanceRadius = std::min(fDrawDistanceRadius, TheCamera.m_fLODDistMultiplier * modelInfo->m_fDrawDistance);
    if (!entity->m_pLod) {
        const float fDrawDistance = std::min(modelInfo->m_fDrawDistance, fDrawDistanceRadius);
        if (fDrawDistance > MAX_LOWLOD_DISTANCE)
            fFadingDistance = fDrawDistance / 15.0f + 10.0f;
        if (entity->m_bIsBIGBuilding)
            fDrawDistanceRadius *= CRenderer::ms_lowLodDistScale;
    }
    float fFade = std::min((fDrawDistanceRadius + MAX_FADING_DISTANCE - distance) / fFadingDistance, 1.0f);
    return static_cast<int32>(modelInfo->m_nAlpha * fFade);
}

// 0x732330
void CVisibilityPlugins::ClearAtomicFlag(RpAtomic* atomic, uint16 flag) {
    ATOMICPLG(atomic, m_flags) &= ~flag;
}

// 0x732310
void CVisibilityPlugins::ClearAtomicFlag(RpAtomic* atomic, int32 flag) {
    ATOMICPLG(atomic, m_flags) &= ~flag;
}

RpAtomic* CVisibilityPlugins::ClearAtomicFlagCB(RpAtomic* atomic, void* data) {
    uint16 flag = *reinterpret_cast<uint16*>(&data);
    ATOMICPLG(atomic, m_flags) &= ~flag;
    return atomic;
}

// 0x732350
void CVisibilityPlugins::ClearClumpForAllAtomicsFlag(RpClump* clump, uint16 flag) {
    RpClumpForAllAtomics(clump, ClearAtomicFlagCB, (void*)flag);
}

// 0x732E10
void* CVisibilityPlugins::ClumpConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    CLUMPPLG(object, m_visibilityCallBack) = DefaultVisibilityCB;
    CLUMPPLG(object, m_alpha) = 255;
    return object;
}

// 0x732200
void* CVisibilityPlugins::ClumpCopyConstructor(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    CLUMPPLG(dstObject, m_visibilityCallBack) = CLUMPPLGCONST(srcObject, m_visibilityCallBack);
    return dstObject;
}

// 0x732220
void* CVisibilityPlugins::ClumpDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    return object;
}

// 0x732A30
int32 CVisibilityPlugins::DefaultVisibilityCB() {
    return 1;
}

// 0x7321B0
void* CVisibilityPlugins::FrameConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    FRAMEPLG(object, m_hierarchyId) = 0;
    return object;
}

// 0x7321D0
void* CVisibilityPlugins::FrameCopyConstructor(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    FRAMEPLG(dstObject, m_hierarchyId) = FRAMEPLGCONST(srcObject, m_hierarchyId);
    return dstObject;
}

// 0x7321F0
void* CVisibilityPlugins::FrameDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    return object;
}

// 0x732A40
bool CVisibilityPlugins::FrustumSphereCB(RpClump* clump) {
    RwSphere sphere{};
    RwFrame* frame = RpClumpGetFrame(clump);
    CBaseModelInfo* modelInfo = FRAMEPLG(frame, m_modelInfo);
    sphere.radius = modelInfo->GetColModel()->GetBoundRadius();
    sphere.center = modelInfo->GetColModel()->GetBoundCenter();
    RwMatrix* transformMatrix = RwFrameGetLTM(frame);
    RwV3dTransformPoints(&sphere.center, &sphere.center, 1, transformMatrix);
    return RwCameraFrustumTestSphere(ms_pCamera, &sphere) != rwSPHEREOUTSIDE;
}

// The function name is misleading, it returns the flags
// 0x732370
uint16 CVisibilityPlugins::GetAtomicId(RpAtomic* atomic) {
    return ATOMICPLG(atomic, m_flags);
}

// 0x732260
CAtomicModelInfo* CVisibilityPlugins::GetAtomicModelInfo(RpAtomic* atomic) {
    int16 modelId = ATOMICPLG(atomic, m_modelId);
    if (modelId == -1)
        return nullptr;
    return CModelInfo::GetModelInfo(modelId)->AsAtomicModelInfoPtr();
}

// 0x732B20
int32 CVisibilityPlugins::GetClumpAlpha(RpClump* clump) {
    return CLUMPPLG(clump, m_alpha);
}

// 0x732AC0
CClumpModelInfo* CVisibilityPlugins::GetClumpModelInfo(RpClump* clump) {
    return FRAMEPLG(RpClumpGetFrame(clump), m_modelInfo);
}

// 0x732C80
float CVisibilityPlugins::GetDistanceSquaredFromCamera(RwFrame* frame) {
    RwMatrix* transformMatrix = RwFrameGetLTM(frame);
    CVector distance;
    RwV3dSub(&distance, &transformMatrix->pos, ms_pCameraPosn);
    return distance.SquaredMagnitude();
}

// 0x732CC0
float CVisibilityPlugins::GetDistanceSquaredFromCamera(const CVector* pos) {
    CVector distance = *pos - *ms_pCameraPosn;
    return distance.SquaredMagnitude();
}

// 0x7326D0
float CVisibilityPlugins::GetDotProductWithCameraVector(RwMatrix* atomicMatrix, RwMatrix* clumpMatrix, uint16 flags) {
    float dotProduct1 = 0.0f;
    float dotProduct2 = *(float*)&atomicMatrix; // really?
    CVector distance = atomicMatrix->pos - *ms_pCameraPosn;
    if (flags & (ATOMIC_IS_REAR | ATOMIC_IS_FRONT))
        dotProduct1 = distance.Dot(clumpMatrix->up);
    else if (flags & (ATOMIC_IS_RIGHT | ATOMIC_IS_LEFT))
        dotProduct1 = distance.Dot(clumpMatrix->right);

    if (flags & (ATOMIC_IS_REAR | ATOMIC_IS_LEFT))
        dotProduct1 = -dotProduct1;

    if (flags & ATOMIC_VEHCOMP_15) {
        const float dot = distance.Dot(clumpMatrix->at);
        if (flags & (ATOMIC_IS_FRONT_DOOR | ATOMIC_IS_REAR_DOOR))
            dotProduct1 += dot * 0.25f;
        else
            dotProduct1 += dot * 2.5f;
    }

    if (!(flags & (ATOMIC_IS_FRONT_DOOR | ATOMIC_IS_REAR_DOOR)))
        return dotProduct1;

    if (flags & ATOMIC_IS_REAR_DOOR)
        dotProduct2 = -distance.Dot(clumpMatrix->up);
    else if (flags & ATOMIC_IS_FRONT_DOOR)
        dotProduct2 = distance.Dot(clumpMatrix->up);

    if (dotProduct1 < 0.0f && dotProduct2 < 0.0f)
        return dotProduct1 + dotProduct2;

    if (dotProduct1 > 0.0f && dotProduct2 > 0.0f)
        return dotProduct1 + dotProduct2;

    return dotProduct1;
}

// 0x732A20
int32 CVisibilityPlugins::GetFrameHierarchyId(RwFrame* frame) {
    return FRAMEPLG(frame, m_hierarchyId);
}

// 0x732250
int16 CVisibilityPlugins::GetModelInfoIndex(RpAtomic* atomic) {
    return ATOMICPLG(atomic, m_modelId);
}

// 0x7323A0
int16 CVisibilityPlugins::GetUserValue(RpAtomic* atomic) {
    return ATOMICPLG(atomic, m_userValue);
}

// Unused
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
    return plugin::CallAndReturn<bool, 0x732AE0, RpClump*>(clump);
}

// 0x732480
void CVisibilityPlugins::RenderAlphaAtomic(RpAtomic* atomic, int32 alpha) {
    uint8 alphas[256]; // OG: ~152

    RpGeometry* geometry = RpAtomicGetGeometry(atomic);
    uint32 geometryFlags = RpGeometryGetFlags(geometry);
    RpGeometrySetFlags(geometry, geometryFlags | rpGEOMETRYMODULATEMATERIALCOLOR);

    const int32 numMaterials = RpGeometryGetNumMaterials(geometry);
    for (int32 i = 0; i < numMaterials; i++) {
        RpMaterial* material = RpGeometryGetMaterial(geometry, i);
        RwRGBA* color        = RpMaterialGetColor(material);
        alphas[i]            = color->alpha;
        color->alpha         = std::min(color->alpha, (uint8)alpha);
    }

    AtomicDefaultRenderCallBack(atomic);

    for (int32 i = 0; i < numMaterials; i++) {
        RpMaterial* material = RpGeometryGetMaterial(geometry, i);
        RwRGBA* color        = RpMaterialGetColor(material);
        color->alpha         = alphas[i];
    }
    RpGeometrySetFlags(geometry, geometryFlags);
}

// 0x733E90
void CVisibilityPlugins::RenderAlphaAtomics() {
    RenderOrderedList(m_alphaList);
}

// 0x732660
RpAtomic* CVisibilityPlugins::RenderAtomicWithAlphaCB(RpAtomic* atomic, void* data) {
    if (RpAtomicGetFlags(atomic) & rpATOMICRENDER)
        RenderAlphaAtomic(atomic, *reinterpret_cast<int32*>(data));
    return atomic;
}

// 0x733EC0
void CVisibilityPlugins::RenderBoatAlphaAtomics() {
    RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));
    RenderOrderedList(m_alphaBoatAtomicList);
    RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLBACK));
}

// 0x732B40
void CVisibilityPlugins::RenderEntity(CEntity* entity, float distance) {
    if (!entity->m_pRwObject)
        return;

    CBaseModelInfo* mi = CModelInfo::GetModelInfo(entity->m_nModelIndex);
    if (mi->bDontWriteZBuffer)
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));

    if (!entity->m_bDistanceFade) {
        if (CGame::currArea || mi->bDontWriteZBuffer)
            RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(0));
        else
            RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(100u));
        CRenderer::RenderOneNonRoad(entity);
    }
    else {
        RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(0));
        int32 alpha = CalculateFadingAtomicAlpha(mi, entity, distance);
        entity->m_bImBeingRendered = true;
        if (!entity->m_bBackfaceCulled) {
            RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));
        }
        bool bLightingSetup = entity->SetupLighting();
        if (RwObjectGetType(entity->m_pRwObject) == rpATOMIC) {
            RenderFadingAtomic(mi, entity->m_pRwAtomic, alpha);
        } else {
            RenderFadingClump(mi, entity->m_pRwClump, alpha);
        }
        entity->RemoveLighting(bLightingSetup);
        entity->m_bImBeingRendered = false;
        if (!entity->m_bBackfaceCulled)
            RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLBACK));
    }

    if (mi->bDontWriteZBuffer)
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(TRUE));
}

// 0x732610
void CVisibilityPlugins::RenderFadingAtomic(CBaseModelInfo* modelInfo, RpAtomic* atomic, int32 alpha) {
    if (modelInfo->bAdditiveRender) {
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDONE));
    }

    RenderAlphaAtomic(atomic, alpha);

    if (modelInfo->bAdditiveRender) {
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
    }
}

// 0x732680
void CVisibilityPlugins::RenderFadingClump(CBaseModelInfo* modelInfo, RpClump* clump, int32 alpha) {
    if (modelInfo->bAdditiveRender) {
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDONE));
    }

    RpClumpForAllAtomics(clump, RenderAtomicWithAlphaCB, &alpha);
    if (modelInfo->bAdditiveRender) {
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
    }
}

// 0x733630
RpAtomic* CVisibilityPlugins::RenderFadingClumpCB(RpAtomic* atomic) {
    if (GetClumpAlpha(RpAtomicGetClump(atomic)) == 255)
        AtomicDefaultRenderCallBack(atomic);
    else
        RenderAlphaAtomic(atomic, GetClumpAlpha(RpAtomicGetClump(atomic)));
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

// 0x7340B0
RpAtomic* CVisibilityPlugins::RenderHeliRotorAlphaCB(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera >= ms_bigVehicleLod0Dist)
        return atomic;

    RwMatrix* atomicMatrix = RwFrameGetLTM(RpAtomicGetFrame(atomic));
    RwMatrix* clumpMatrix = RwFrameGetLTM(RpClumpGetFrame(RpAtomicGetClump(atomic)));
    CVector distance = atomicMatrix->pos - *ms_pCameraPosn;
    const float dotProduct = distance.Dot(clumpMatrix->at);
    AlphaObjectInfo objectInfo{};
    objectInfo.m_atomic = atomic;
    objectInfo.m_pCallback = DefaultAtomicRenderCallback;
    objectInfo.m_distance = dotProduct * MAX_FADING_DISTANCE + gVehicleDistanceFromCamera;
    if (!m_alphaList.InsertSorted(objectInfo)) {
        AtomicDefaultRenderCallBack(atomic);
    }
    return atomic;
}

// 0x734170
RpAtomic* CVisibilityPlugins::RenderHeliTailRotorAlphaCB(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera >= ms_vehicleLod0Dist)
        return atomic;

    RwMatrix* atomicMatrix = RwFrameGetLTM(RpAtomicGetFrame(atomic));
    RwMatrix* clumpMatrix = RwFrameGetLTM(RpClumpGetFrame(RpAtomicGetClump(atomic)));
    CVector distance = atomicMatrix->pos - *ms_pCameraPosn;
    const float dotProduct1 = distance.Dot(clumpMatrix->right);
    const float dotProduct2 = distance.Dot(clumpMatrix->up);
    AlphaObjectInfo info{};
    info.m_atomic = atomic;
    info.m_pCallback = DefaultAtomicRenderCallback;
    info.m_distance = -dotProduct1 - dotProduct2 + gVehicleDistanceFromCamera;
    if (!m_alphaList.InsertSorted(info))
        AtomicDefaultRenderCallBack(atomic);
    return atomic;
}

// Unused
// 0x7323E0
RpAtomic* CVisibilityPlugins::RenderObjNormalAtomic(RpAtomic* atomic) {
    RwMatrix* atomicMatrix = RwFrameGetLTM(RpAtomicGetFrame(atomic));
    CVector distance = atomicMatrix->pos - *ms_pCameraPosn;
    const float length = RwV3dLength(&distance);
    if (distance.Dot(atomicMatrix->up) < length * -0.3f && length > 8.0f)
        return atomic;
    AtomicDefaultRenderCallBack(atomic);
    return atomic;
}

// 0x7337A0
void CVisibilityPlugins::RenderOrderedList(CLinkList<CVisibilityPlugins::AlphaObjectInfo>& alphaObjectInfoList) {
    auto link = alphaObjectInfoList.usedListTail.prev;
    for (; link != &alphaObjectInfoList.usedListHead; link = link->prev) {
        auto callBack = reinterpret_cast<tAlphaRenderOrderedListCB>(link->data.m_pCallback);
        callBack(link->data.m_entity, link->data.m_distance);
    }
}

// 0x7335B0
RpAtomic* CVisibilityPlugins::RenderPedCB(RpAtomic* atomic) {
    const float distanceSquared = GetDistanceSquaredFromCamera(RpAtomicGetFrame(atomic));
    if (distanceSquared >= ms_pedLodDist)
        return atomic;

    int32 alpha = GetClumpAlpha(RpAtomicGetClump(atomic));
    if (alpha == 255) {
        AtomicDefaultRenderCallBack(atomic);
        return atomic;
    }
    RenderAlphaAtomic(atomic, alpha);
    return atomic;
}

// 0x732850
RpMaterial* CVisibilityPlugins::SetTextureCB(RpMaterial* material, void* texture)
{
    RpMaterialSetTexture(material, reinterpret_cast<RwTexture*>(texture));
    return material;
}

// 0x732870
RpAtomic* CVisibilityPlugins::RenderPlayerCB(RpAtomic* atomic) {
    RwTexture* skinTexture = CWorld::Players[0].m_pSkinTexture;
    if (skinTexture)
        RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), SetTextureCB, skinTexture);
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

// 0x734240
RpAtomic* CVisibilityPlugins::RenderTrainHiDetailAlphaCB(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera >= ms_bigVehicleLod0Dist)
        return atomic;

    if (gVehicleDistanceFromCamera >= ms_vehicleLod0RenderMultiPassDist)
        SetAtomicFlag(atomic, ATOMIC_DISABLE_REFLECTIONS);
    else
        ClearAtomicFlag(atomic, ATOMIC_DISABLE_REFLECTIONS);

    RwMatrix* atomicMatrix = RwFrameGetLTM(RpAtomicGetFrame(atomic));
    uint16 atomicFlags = GetAtomicId(atomic);
    RwMatrix* clumpMatrix = RwFrameGetLTM(RpClumpGetFrame(RpAtomicGetClump(atomic)));
    const float dot = GetDotProductWithCameraVector(atomicMatrix, clumpMatrix, atomicFlags);
    if (gVehicleDistanceFromCamera > ms_cullCompsDist
        && !(atomicFlags & ATOMIC_RENDER_ALWAYS)
        && gVehicleAngleToCamera < 0.2f
        && dot > 0.0f
        && ((atomicFlags & ATOMIC_CULL) || gVehicleDistanceFromCamera * 0.1f < dot * dot))
    {
        return atomic;
    }
    AlphaObjectInfo info{};
    info.m_pCallback = DefaultAtomicRenderCallback;
    info.m_atomic = atomic;
    if (atomicFlags & ATOMIC_HAS_ALPHA)
        info.m_distance = gVehicleDistanceFromCamera;
    else
        info.m_distance = gVehicleDistanceFromCamera + dot;
    if (!m_alphaList.InsertSorted(info))
        AtomicDefaultRenderCallBack(atomic);

    return atomic;
}

// 0x733330
RpAtomic* CVisibilityPlugins::RenderTrainHiDetailCB(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera >= ms_bigVehicleLod0Dist)
        return atomic;

    if (gVehicleDistanceFromCamera >= ms_vehicleLod0RenderMultiPassDist)
        SetAtomicFlag(atomic, ATOMIC_DISABLE_REFLECTIONS);
    else
        ClearAtomicFlag(atomic, ATOMIC_DISABLE_REFLECTIONS);

    uint16 atomicFlags = GetAtomicId(atomic);
    if (gVehicleDistanceFromCamera > ms_cullCompsDist && !(atomicFlags & ATOMIC_RENDER_ALWAYS)) {
        if (gVehicleAngleToCamera < 0.2f) {
            RwMatrix* atomicMatrix = RwFrameGetLTM(RpAtomicGetFrame(atomic));
            RwMatrix* clumpMatrix = RwFrameGetLTM(RpClumpGetFrame(RpAtomicGetClump(atomic)));
            const float dot = GetDotProductWithCameraVector(atomicMatrix, clumpMatrix, atomicFlags);
            if (dot > 0.0f && ((atomicFlags & ATOMIC_CULL) || gVehicleDistanceFromCamera * 0.1f < dot * dot))
                return atomic;
        }
    }
    AtomicDefaultRenderCallBack(atomic);
    return atomic;

}

// 0x733F80
RpAtomic* CVisibilityPlugins::RenderVehicleHiDetailAlphaCB(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera >= ms_vehicleLod0Dist)
        return atomic;

    if (gVehicleDistanceFromCamera >= ms_vehicleLod0RenderMultiPassDist)
        SetAtomicFlag(atomic, ATOMIC_DISABLE_REFLECTIONS);
    else
        ClearAtomicFlag(atomic, ATOMIC_DISABLE_REFLECTIONS);

    uint16 atomicFlags = GetAtomicId(atomic);
    RwMatrix* atomicMatrix = RwFrameGetLTM(RpAtomicGetFrame(atomic));
    RwMatrix* clumpMatrix = RwFrameGetLTM(RpClumpGetFrame(RpAtomicGetClump(atomic)));
    const float dot = GetDotProductWithCameraVector(atomicMatrix, clumpMatrix, atomicFlags);
    if (gVehicleDistanceFromCamera > ms_cullCompsDist
        && !(atomicFlags & ATOMIC_RENDER_ALWAYS)
        && gVehicleAngleToCamera < 0.2f
        && dot > 0.0f
        && ((atomicFlags & ATOMIC_CULL) || gVehicleDistanceFromCamera * 0.1f < dot * dot))
    {
        return atomic;
    }
    AlphaObjectInfo info{};
    info.m_pCallback = DefaultAtomicRenderCallback;
    info.m_atomic = atomic;
    if (atomicFlags & ATOMIC_HAS_ALPHA)
        info.m_distance = gVehicleDistanceFromCamera - 0.0001f;
    else
        info.m_distance = gVehicleDistanceFromCamera + dot;
    if (!m_alphaList.InsertSorted(info))
        AtomicDefaultRenderCallBack(atomic);

    return atomic;
}

// 0x734370
RpAtomic* CVisibilityPlugins::RenderVehicleHiDetailAlphaCB_BigVehicle(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera >= ms_bigVehicleLod0Dist)
        return atomic;

    if (gVehicleDistanceFromCamera >= ms_vehicleLod0RenderMultiPassDist)
        SetAtomicFlag(atomic, ATOMIC_DISABLE_REFLECTIONS);
    else
        ClearAtomicFlag(atomic, ATOMIC_DISABLE_REFLECTIONS);

    uint16 atomicFlags = GetAtomicId(atomic);
    RwMatrix* atomicMatrix = RwFrameGetLTM(RpAtomicGetFrame(atomic));
    RwMatrix* clumpMatrix = RwFrameGetLTM(RpClumpGetFrame(RpAtomicGetClump(atomic)));
    const float dot = GetDotProductWithCameraVector(atomicMatrix, clumpMatrix, atomicFlags);
    if (gVehicleDistanceFromCamera > ms_cullBigCompsDist
        && !(atomicFlags & ATOMIC_RENDER_ALWAYS)
        && gVehicleAngleToCamera < 0.2f
        && dot > 0.0f
        && ((atomicFlags & ATOMIC_CULL) || gVehicleDistanceFromCamera * 0.1f < dot * dot))
    {
        return atomic;
    }
    AlphaObjectInfo info{};
    info.m_pCallback = DefaultAtomicRenderCallback;
    info.m_atomic = atomic;
    if (atomicFlags & ATOMIC_HAS_ALPHA)
        info.m_distance = gVehicleDistanceFromCamera - 0.0001f;
    else
        info.m_distance = gVehicleDistanceFromCamera + dot;
    if (!m_alphaList.InsertSorted(info))
        AtomicDefaultRenderCallBack(atomic);

    return atomic;
}

// 0x7344A0
RpAtomic* CVisibilityPlugins::RenderVehicleHiDetailAlphaCB_Boat(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera >= ms_vehicleLod0Dist)
        return atomic;

    if (gVehicleDistanceFromCamera >= ms_vehicleLod0RenderMultiPassDist)
        SetAtomicFlag(atomic, ATOMIC_DISABLE_REFLECTIONS);
    else
        ClearAtomicFlag(atomic, ATOMIC_DISABLE_REFLECTIONS);

    uint16 atomicFlags = GetAtomicId(atomic);
    if (atomicFlags & ATOMIC_HAS_ALPHA) {
        AlphaObjectInfo info{};
        info.m_distance = gVehicleDistanceFromCamera;
        info.m_atomic = atomic;
        info.m_pCallback = DefaultAtomicRenderCallback;
        if (m_alphaBoatAtomicList.InsertSorted(info))
            return atomic;
    }
    AtomicDefaultRenderCallBack(atomic);
    return atomic;
}

// 0x733240
RpAtomic* CVisibilityPlugins::RenderVehicleHiDetailCB(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera >= ms_vehicleLod0Dist)
        return atomic;

    if (gVehicleDistanceFromCamera >= ms_vehicleLod0RenderMultiPassDist)
        SetAtomicFlag(atomic, ATOMIC_DISABLE_REFLECTIONS);
    else
        ClearAtomicFlag(atomic, ATOMIC_DISABLE_REFLECTIONS);

    uint16 atomicFlags = GetAtomicId(atomic);
    if (gVehicleDistanceFromCamera > ms_cullCompsDist && !(atomicFlags & ATOMIC_RENDER_ALWAYS)) {
        if (gVehicleAngleToCamera < 0.2f) {
            RwMatrix* atomicMatrix = RwFrameGetLTM(RpAtomicGetFrame(atomic));
            RwMatrix* clumpMatrix = RwFrameGetLTM(RpClumpGetFrame(RpAtomicGetClump(atomic)));
            const float dot = GetDotProductWithCameraVector(atomicMatrix, clumpMatrix, atomicFlags);
            if (dot > 0.0f && ((atomicFlags & ATOMIC_CULL) || gVehicleDistanceFromCamera * 0.1f < dot * dot))
                return atomic;
        }
    }
    AtomicDefaultRenderCallBack(atomic);

    return atomic;
}

// 0x733420
RpAtomic* CVisibilityPlugins::RenderVehicleHiDetailCB_BigVehicle(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera >= ms_bigVehicleLod0Dist)
        return atomic;

    if (gVehicleDistanceFromCamera >= ms_vehicleLod0RenderMultiPassDist)
        SetAtomicFlag(atomic, ATOMIC_DISABLE_REFLECTIONS);
    else
        ClearAtomicFlag(atomic, ATOMIC_DISABLE_REFLECTIONS);

    uint16 atomicFlags = GetAtomicId(atomic);
    if (gVehicleDistanceFromCamera > ms_cullBigCompsDist && !(atomicFlags & ATOMIC_RENDER_ALWAYS)) {
        if (gVehicleAngleToCamera < 0.2f) {
            RwMatrix* atomicMatrix = RwFrameGetLTM(RpAtomicGetFrame(atomic));
            RwMatrix* clumpMatrix = RwFrameGetLTM(RpClumpGetFrame(RpAtomicGetClump(atomic)));
            const float dot = GetDotProductWithCameraVector(atomicMatrix, clumpMatrix, atomicFlags);
            if (dot > 0.0f)
                return atomic;
        }
    }
    AtomicDefaultRenderCallBack(atomic);

    return atomic;
}

// 0x733550
RpAtomic* CVisibilityPlugins::RenderVehicleHiDetailCB_Boat(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera >= ms_vehicleLod0Dist)
        return atomic;

    if (gVehicleDistanceFromCamera >= ms_vehicleLod0RenderMultiPassDist)
        SetAtomicFlag(atomic, ATOMIC_DISABLE_REFLECTIONS);
    else
        ClearAtomicFlag(atomic, ATOMIC_DISABLE_REFLECTIONS);

    AtomicDefaultRenderCallBack(atomic);

    return atomic;
}

// 0x7334F0
RpAtomic* CVisibilityPlugins::RenderVehicleLoDetailCB_Boat(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera < ms_vehicleLod0Dist)
        return atomic;

    SetAtomicFlag(atomic, ATOMIC_DISABLE_REFLECTIONS);
    if (GetClumpAlpha(RpAtomicGetClump(atomic)) == 255)
        AtomicDefaultRenderCallBack(atomic);
    else
        RenderAlphaAtomic(atomic, GetClumpAlpha(RpAtomicGetClump(atomic)));

    return atomic;
}

// 0x7331E0
RpAtomic* CVisibilityPlugins::RenderVehicleReallyLowDetailCB(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera < ms_vehicleLod0Dist)
        return atomic;

    SetAtomicFlag(atomic, ATOMIC_DISABLE_REFLECTIONS);
    if (GetClumpAlpha(RpAtomicGetClump(atomic)) == 255)
        AtomicDefaultRenderCallBack(atomic);
    else
        RenderAlphaAtomic(atomic, GetClumpAlpha(RpAtomicGetClump(atomic)));

    return atomic;
}

// 0x732820
RpAtomic* CVisibilityPlugins::RenderVehicleReallyLowDetailCB_BigVehicle(RpAtomic* atomic) {
    if (gVehicleDistanceFromCamera >= ms_bigVehicleLod0Dist)
        AtomicDefaultRenderCallBack(atomic);

    return atomic;
}

// 0x733670
RpAtomic* CVisibilityPlugins::RenderWeaponCB(RpAtomic* atomic) {
    RpClump* clump = RpAtomicGetClump(atomic);
    CClumpModelInfo* modelInfo = GetClumpModelInfo(clump);
    const float drawDistanceRadius = TheCamera.m_fLODDistMultiplier * modelInfo->m_fDrawDistance;
    if (GetDistanceSquaredFromCamera(RpClumpGetFrame(clump)) < drawDistanceRadius * drawDistanceRadius)
        AtomicDefaultRenderCallBack(atomic);

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
                RwMatrixRotate(weaponRwMatrix, &CPedIK::XaxisIK, 180.0f, rwCOMBINEPRECONCAT);
                static RwV3d leftWeaponTranslate = { 0.04f, -0.05f, 0.0f };
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
    ATOMICPLG(atomic, m_flags) |= flag;
}

// 0x7322D0
void CVisibilityPlugins::SetAtomicFlag(RpAtomic* atomic, int32 flag) {
    ATOMICPLG(atomic, m_flags) |= flag; // how does this work??? m_flags is `uint16` ffs
}

RpAtomic* CVisibilityPlugins::SetAtomicFlagCB(RpAtomic* atomic, void* data) {
    uint16 flag = *reinterpret_cast<uint16*>(&data);
    SetAtomicFlag(atomic, flag);
    return atomic;
}

// 0x732307
void CVisibilityPlugins::SetClumpForAllAtomicsFlag(RpClump* clump, uint16 flag) {
    RpClumpForAllAtomics(clump, SetAtomicFlagCB, (void*)flag);
}

// 0x732230
void CVisibilityPlugins::SetAtomicId(void* atomic, int16 id) {
    ATOMICPLG(atomic, m_modelId) = id;
}

// 0x7328A0
void CVisibilityPlugins::SetAtomicRenderCallback(RpAtomic* atomic, RpAtomicCallBackRender renderCB) {
    if (!renderCB) {
        renderCB = AtomicDefaultRenderCallBack;
    }
    atomic->renderCallBack = renderCB;
    if (!renderCB) {
        atomic->renderCallBack = AtomicDefaultRenderCallBack;
    }
}

// 0x732B00
void CVisibilityPlugins::SetClumpAlpha(RpClump* clump, int32 alpha) {
    CLUMPPLG(clump, m_alpha) = alpha;
}

// 0x733750
void CVisibilityPlugins::SetClumpModelInfo(RpClump* clump, CClumpModelInfo* clumpModelInfo) {
    FRAMEPLG(RpClumpGetFrame(clump), m_modelInfo) = clumpModelInfo;
    if (clumpModelInfo->GetModelType() != MODEL_INFO_VEHICLE)
        return;

    CVehicleModelInfo* mi = clumpModelInfo->AsVehicleModelInfoPtr();
    if (mi->IsTrain() || mi->m_nVehicleType == VEHICLE_TYPE_FPLANE)
        CLUMPPLG(clump, m_visibilityCallBack) = VehicleVisibilityCB_BigVehicle;
    else
        CLUMPPLG(clump, m_visibilityCallBack) = VehicleVisibilityCB;
}

// 0x732A00
void CVisibilityPlugins::SetFrameHierarchyId(RwFrame* frame, int32 id) {
    FRAMEPLG(frame, m_hierarchyId) = id;
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
    ATOMICPLG(atomic, m_flags) = value;
}

// 0x733160
void CVisibilityPlugins::SetupVehicleVariables(RpClump* clump) {
    if (RwObjectGetType(clump) != rpCLUMP)
        return;

    RwFrame* frame = RpClumpGetFrame(clump);
    RwMatrix* transformMatrix = RwFrameGetLTM(frame);
    CVector distance1;
    RwV3dSub(&distance1, &transformMatrix->pos, ms_pCameraPosn);
    gVehicleDistanceFromCamera = distance1.SquaredMagnitude();
    CVector2D distance2;
    RwV2dSub(&distance2, ms_pCameraPosn, RwMatrixGetPos(RwFrameGetMatrix(frame)));
    gVehicleAngleToCamera = atan2(ms_pCameraPosn->z - RwMatrixGetPos(RwFrameGetMatrix(frame))->z, distance2.Magnitude());
}

// 0x7336F0
bool CVisibilityPlugins::VehicleVisibilityCB(RpClump* clump) {
    const float distanceSquared = GetDistanceSquaredFromCamera(RpClumpGetFrame(clump));
    if (distanceSquared <= ms_vehicleLod1Dist)
        return FrustumSphereCB(clump);

    return false;
}

// 0x732AB0
bool CVisibilityPlugins::VehicleVisibilityCB_BigVehicle(RpClump* clump) {
    return FrustumSphereCB(clump);
}

void weaponPedsForPc_Insert(CPed* ped) {
    plugin::Call<0x5E46D0>(ped);
}
