#include "StdInc.h"

#include "PlantMgr.h"
#include "GrassRenderer.h"
#include "PlantColEntEntry.h"
#include "PlantLocTri.h"
#include "PlantSurfPropMgr.h"
#include "ColHelpers.h"
#include "ProcObjectMan.h"
#include <extensions/enumerate.hpp>

// 0x5DD100 (todo: move)
static void AtomicCreatePrelitIfNeeded(RpAtomic* atomic) {
    plugin::Call<0x5DD100, RpAtomic*>(atomic);
}

// 0x5DD1E0 (do not hook! it has retarded calling conv)
static bool GeometrySetPrelitConstantColor(RpGeometry* geometry, RwRGBA color) {
    if ((RpGeometryGetFlags(geometry) & rpGEOMETRYPRELIT) == 0) {
        return false;
    }

    RpGeometryLock(geometry, rpGEOMETRYLOCKALL);
    auto prelit = RpGeometryGetPreLightColors(geometry);
    if (prelit) {
        auto numVertices = RpGeometryGetNumVertices(geometry);
        for (auto i = 0; i < numVertices; ++i) {
            prelit[i] = color;
        }
    }
    RpGeometryUnlock(geometry);
    return true;
}

// 0x5DD220
static bool LoadModels(std::initializer_list<const char*> models, RpAtomic** atomics) {
    for (auto& model : models) {
        auto stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, std::format("models\\grass\\{}", model).c_str());
        RpClump* clump = nullptr;

        if (stream && RwStreamFindChunk(stream, rwID_CLUMP, nullptr, nullptr)) {
            clump = RpClumpStreamRead(stream);
        }
        RwStreamClose(stream, nullptr);
        assert(clump);

        auto firstAtomic = GetFirstAtomic(clump);
        SetFilterModeOnAtomicsTextures(firstAtomic, rwFILTERMIPLINEAR);
        AtomicCreatePrelitIfNeeded(firstAtomic);

        auto geometry = RpAtomicGetGeometry(firstAtomic);
        RpGeometryLock(geometry, rpGEOMETRYLOCKALL);
        RpGeometryGetFlags(geometry) &= 0xFFFFFF8F; // Negate bit 5 and 6, nothing seems to match it currently
        RpGeometryGetFlags(geometry) |= rpGEOMETRYMODULATEMATERIALCOLOR;
        RpGeometryUnlock(geometry);
        GeometrySetPrelitConstantColor(geometry, RwRGBA{ 255, 255, 255, 255 });

        RwRGBA color = { 0, 0, 0, 50 };
        RpGeometryForAllMaterials(geometry, [](RpMaterial* material, void* color) {
                RpMaterialSetColor(material, (RwRGBA*)color);
                RpMaterialSetTexture(material, tex_gras07Si);
                return material;
            }, &color
        );

        auto atomicCopy = RpAtomicClone(firstAtomic);
        RpClumpDestroy(clump);
        SetFilterModeOnAtomicsTextures(atomicCopy, rwFILTERLINEAR);
        RpAtomicSetFrame(atomicCopy, RwFrameCreate());
        *atomics++ = atomicCopy;
    }

    return true;
}

void CPlantMgr::InjectHooks() {
    RH_ScopedClass(CPlantMgr);
    RH_ScopedCategory("Plant");

    RH_ScopedInstall(Initialise, 0x5DD910);
    RH_ScopedInstall(Shutdown, 0x5DB940);
    RH_ScopedInstall(ReloadConfig, 0x5DD780);
    RH_ScopedInstall(MoveLocTriToList, 0x5DB590);
    RH_ScopedInstall(MoveColEntToList, 0x5DB5F0);
    RH_ScopedInstall(SetPlantFriendlyFlagInAtomicMI, 0x5DB650);
    RH_ScopedInstall(Update, 0x5DCFA0);
    RH_ScopedInstall(PreUpdateOnceForNewCameraPos, 0x5DCF30);
    RH_ScopedInstall(UpdateAmbientColor, 0x5DB310);
    RH_ScopedInstall(CalculateWindBending, 0x5DB3D0);
    RH_ScopedInstall(_ColEntityCache_Add, 0x5DBEB0);
    RH_ScopedInstall(_ColEntityCache_FindInCache, 0x5DB530);
    RH_ScopedInstall(_ColEntityCache_Remove, 0x5DBEF0);
    RH_ScopedInstall(_ColEntityCache_Update, 0x5DC510);
    RH_ScopedInstall(_ProcessEntryCollisionDataSections, 0x5DCD80);
    RH_ScopedInstall(_ProcessEntryCollisionDataSections_AddLocTris, 0x5DC8B0);
    RH_ScopedInstall(_ProcessEntryCollisionDataSections_RemoveLocTris, 0x5DBF20);
    RH_ScopedInstall(_UpdateLocTris, 0x5DCF00);
    RH_ScopedInstall(Render, 0x5DBAE0);
    //RH_ScopedGlobalInstall(LoadModels, 0x5DD220); // uses `__usercall`, can't hook

    // Do not uncomment!
    // RH_ScopedInstall(_CalcDistanceSqrToEntity, 0x5DBE40, {.reversed = false}); <-- bad call conv.

    // debug shit, all of them just return true.
    // addresses (probably not in order): 0x5DB550 - 0x5DB580

    // RH_ScopedInstall(DbgCountCachedEntities, 0x0, {.reversed = false});
    // RH_ScopedInstall(DbgCountLocTrisAndPlants, 0x0, {.reversed = false});
    // RH_ScopedInstall(DbgRenderCachedEntities, 0x0, {.reversed = false});
    // RH_ScopedInstall(DbgRenderLocTris, 0x0, {.reversed = false});
}

// 0x5DD910
bool CPlantMgr::Initialise() {
    ZoneScoped;

    if (!ReloadConfig())
        return false;

    // Load textures
    {
        CStreaming::MakeSpaceFor(0x8800);
        CStreaming::ImGonnaUseStreamingMemory();
        CTxdStore::PushCurrentTxd();
        auto slot = CTxdStore::FindOrAddTxdSlot("grass_pc");
        CTxdStore::LoadTxd(slot, "models\\grass\\plant1.txd");
        CTxdStore::AddRef(slot);
        CTxdStore::SetCurrentTxd(slot);

        const auto ReadTexture = [](const char* name) {
            auto texture = RwTextureRead(name, nullptr);
            RwTextureSetAddressing(texture, rwTEXTUREADDRESSWRAP);
            RwTextureSetFilterMode(texture, rwFILTERLINEAR);
            return texture;
        };

        for (auto&& [i, tab] : notsa::enumerate(PC_PlantTextureTab | rng::views::take(2))) {
            for (auto&& [j, tex] : notsa::enumerate(tab)) {
                tex = ReadTexture(std::format("txgrass{}_{}", i, j).c_str());
            }
        }
        tex_gras07Si = ReadTexture("gras07Si");
        CTxdStore::PopCurrentTxd();
        CStreaming::IHaveUsedStreamingMemory();

        grassTexturesPtr[0] = grassTexturesPtr[2] = PC_PlantTextureTab[0];
        grassTexturesPtr[1] = grassTexturesPtr[3] = PC_PlantTextureTab[1];
    }

    // Load models
    {
        const auto models1 = { "grass0_1.dff", "grass0_2.dff", "grass0_3.dff", "grass0_4.dff" };
        const auto models2 = { "grass1_1.dff", "grass1_2.dff", "grass1_3.dff", "grass1_4.dff" };
        if (LoadModels(models1, PC_PlantModelsTab[0]) && LoadModels(models2, PC_PlantModelsTab[1])) {
            grassModelsPtr[0] = grassModelsPtr[2] = *PC_PlantModelsTab[0];
            grassModelsPtr[1] = grassModelsPtr[3] = *PC_PlantModelsTab[1];

            for (auto i = 0u; i < 4u; i++) {
                CGrassRenderer::SetPlantModelsTab(i, PC_PlantModelsTab[0]); // grassModelsPtr[0]
            }

            CGrassRenderer::SetCloseFarAlphaDist(PLANTS_ALPHA_MIN_DIST, PLANTS_ALPHA_MAX_DIST);
            return true;
        }
    }

    return false;
}

// 0x5DB940
void CPlantMgr::Shutdown() {
    auto* nextEntry = CPlantMgr::m_CloseColEntListHead;
    while (nextEntry) {
        auto* curEntry = nextEntry;
        nextEntry      = curEntry->m_NextEntry;
        curEntry->ReleaseEntry();
    }

    // Destroy atomics
    for (auto i = 0u; i < 4u; i++) {
        const auto DestroyAtomics = [](auto& atomics) {
            for (auto& a : atomics) {
                if (a) {
                    if (const auto f = RpAtomicGetFrame(a)) {
                        RpAtomicSetFrame(a, nullptr);
                        RwFrameDestroy(f);
                    }
                    RpAtomicDestroy(std::exchange(a, nullptr));
                }
            }
        };
        DestroyAtomics(PC_PlantModelsTab[i]);
    }

    // Destroy textures
    for (auto i = 0u; i < 4u; i++) {
        const auto DestroyTextures = [](auto& textures) {
            for (auto& t : textures) {
                if (t) {
                    RwTextureDestroy(std::exchange(t, nullptr));
                }
            }
        };
        DestroyTextures(PC_PlantTextureTab[i]);
    }

    // And finally unload the texture's txd
    CTxdStore::SafeRemoveTxdSlot("grass_pc");
}

// 0x5DD780
bool CPlantMgr::ReloadConfig() {
    if (!CPlantSurfPropMgr::Initialise()) {
        return false;
    }

    std::ranges::fill(m_CloseLocTriListHead, nullptr);
    m_UnusedLocTriListHead = m_LocTrisTab;

    CPlantLocTri* prevTri = nullptr;
    for (auto& tab : m_LocTrisTab) {
        tab.m_V1 = tab.m_V2 = tab.m_V3 = tab.m_Center = CVector{0.0f, 0.0f, 0.0f};
        tab.m_SurfaceId = 0u;
        rng::fill(tab.m_nMaxNumPlants, 0u);

        tab.m_PrevTri = prevTri;
        if (prevTri) {
            prevTri->m_NextTri = &tab;
        }

        prevTri = &tab;
    }
    m_LocTrisTab[MAX_NUM_PLANT_TRIANGLES - 1].m_NextTri = nullptr;
    m_CloseColEntListHead = nullptr;
    m_UnusedColEntListHead = m_ColEntCacheTab;

    CPlantColEntEntry* prevEntry = nullptr;
    for (auto& tab : m_ColEntCacheTab) {
        tab.m_Entity = nullptr;
        tab.m_Objects = nullptr;
        tab.m_numTriangles = 0u;

        tab.m_PrevEntry = prevEntry;
        if (prevEntry) {
            prevEntry->m_NextEntry = &tab;
        }

        prevEntry = &tab;
    }
    m_ColEntCacheTab[MAX_NUM_PROC_OBJECTS - 1].m_NextEntry = nullptr;

    return true;
}

// 0x5DB590
void CPlantMgr::MoveLocTriToList(CPlantLocTri*& oldList, CPlantLocTri*& newList, CPlantLocTri* triangle) {
    if (auto prev = triangle->m_PrevTri) {
        if (auto next = triangle->m_NextTri) {
            next->m_PrevTri = prev;
            prev->m_NextTri = next;
        } else {
            prev->m_NextTri = nullptr;
        }
    } else {
        if (oldList = triangle->m_NextTri) {
            oldList->m_PrevTri = nullptr;
        }
    }
    triangle->m_NextTri = newList;
    triangle->m_PrevTri = nullptr;
    newList = triangle;

    if (auto next = triangle->m_NextTri) {
        next->m_PrevTri = triangle;
    }
}

// 0x5DB5F0
// unused/inlined
void CPlantMgr::MoveColEntToList(CPlantColEntEntry*& oldList, CPlantColEntEntry*& newList, CPlantColEntEntry* entry) {
    if (auto prev = entry->m_PrevEntry) {
        if (auto next = entry->m_NextEntry) {
            next->m_PrevEntry = prev;
            prev->m_NextEntry = next;
        } else {
            prev->m_NextEntry = nullptr;
        }
    } else {
        if (oldList = entry->m_NextEntry) {
            oldList->m_PrevEntry = nullptr;
        }
    }
    entry->m_NextEntry = newList;
    entry->m_PrevEntry = nullptr;
    newList = entry;

    if (auto next = entry->m_NextEntry) {
        next->m_PrevEntry = entry;
    }
}

// 0x5DB650
void CPlantMgr::SetPlantFriendlyFlagInAtomicMI(CAtomicModelInfo* ami) {
    ami->bAtomicFlag0x200 = false;

    auto cd = ami->GetColModel()->GetData();
    if (!cd)
        return;

    auto numTriangles = cd->m_nNumTriangles;
    if (numTriangles <= 0)
        return;

    for (auto& triangle : std::span{cd->m_pTriangles, numTriangles}) {
        if (g_surfaceInfos.CreatesPlants(triangle.m_nMaterial)
            || g_surfaceInfos.CreatesObjects(triangle.m_nMaterial)) {
            ami->bAtomicFlag0x200 = true;

            return;
        }
    }
}

// 0x5DCFA0
void CPlantMgr::Update(const CVector& cameraPosition) {
    ZoneScoped;

    static int8& nUpdateEntCache    = *(int8*)0xC09171;
    static int8& nLocTriSkipCounter = *(int8*)0xC09170;

    IncrementScanCode();
    CGrassRenderer::SetCurrentScanCode(m_scanCode);
    CGrassRenderer::SetGlobalCameraPos(cameraPosition);

    UpdateAmbientColor();
    CGrassRenderer::SetGlobalWindBending(CalculateWindBending());

    _ColEntityCache_Update(cameraPosition, (++nUpdateEntCache % MAX_PLANTS) != 0);

    auto skipMask = nLocTriSkipCounter++ % 8;
    for (auto head = m_CloseColEntListHead; head; head = head->m_NextEntry) {
        _ProcessEntryCollisionDataSections(*head, cameraPosition, skipMask);
    }
}

// 0x5DCF30
void CPlantMgr::PreUpdateOnceForNewCameraPos(const CVector& posn) {
    IncrementScanCode();
    CGrassRenderer::SetCurrentScanCode(m_scanCode);
    CGrassRenderer::SetGlobalCameraPos(posn);
    UpdateAmbientColor();
    CGrassRenderer::SetGlobalWindBending(CalculateWindBending());
    _ColEntityCache_Update(posn, false);

    for (auto i = m_CloseColEntListHead; i; i = i->m_NextEntry) {
        _ProcessEntryCollisionDataSections(*i, posn, 0xFAFAFAFA);
    }
}

// 0x5DB310
void CPlantMgr::UpdateAmbientColor() {
    auto r = 64 + (uint32)(CTimeCycle::GetAmbientRed()   * 2.5f * 255.0f);
    auto g = 64 + (uint32)(CTimeCycle::GetAmbientGreen() * 2.5f * 255.0f);
    auto b = 64 + (uint32)(CTimeCycle::GetAmbientBlue()  * 2.5f * 255.0f);
    m_AmbientColor.r = (uint8)std::max(r, 255u);
    m_AmbientColor.g = (uint8)std::max(g, 255u);
    m_AmbientColor.b = (uint8)std::max(b, 255u);
}

// 0x5DB3D0
float CPlantMgr::CalculateWindBending() {
    static uint32& calculateTimer = *(uint32*)0xC0916C;
    static uint16& RandomSeed = *(uint16*)0xC09168;

    if ((calculateTimer % 2) == 0) {
        calculateTimer++;
        RandomSeed = CGeneral::GetRandomNumber();
    }

    // The 2PI approximation here is nice, they could have went for `PI = 3` tho.
    constexpr float scalingFactor = 6.28f / 4096.f;

    // TODO: Look CEntity::ModifyMatrixForTreeInWind; it's definitely inlined somewhere. (Not actually inlined anywhere, according to android debug symbols)
    if (CWeather::Wind >= 0.5f) {
        auto uiOffset1 = (((RandomSeed + CTimer::GetTimeInMS() * 8) & 0xFFFF) / 4'096) % 16;
        auto uiOffset2 = (uiOffset1 + 1) % 16;
        auto fContrib  = static_cast<float>(((RandomSeed + CTimer::GetTimeInMS() * 8) % 4'096)) / 4096.0F;

        auto fWindOffset = (1.0F - fContrib) * CWeather::saTreeWindOffsets[uiOffset1];
        fWindOffset += 1.0F + fContrib * CWeather::saTreeWindOffsets[uiOffset2];
        fWindOffset *= CWeather::Wind;
        fWindOffset *= 0.015F;
        return fWindOffset;
    } else {
        return std::sinf(scalingFactor * (float)(CTimer::GetTimeInMS() % 4'096)) / (CWeather::Wind >= 0.2f ? 125.0f : 200.0f);
    }
}

// 0x5DBAE0
void CPlantMgr::Render() {
    if (g_fx.GetFxQuality() == FX_QUALITY_LOW) {
        return;
    }

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,    RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,             RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,            RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,            RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION,    RWRSTATE(rwALPHATESTFUNCTIONALWAYS));

    for (auto i = 0; i < 4; i++) {
        auto* plantTri = m_CloseLocTriListHead[i];
        auto* grassTextures = grassTexturesPtr[i];

        while (plantTri) {
            if (!plantTri->m_createsPlants) {
                plantTri = plantTri->m_NextTri;
                continue;
            }

            auto* surfData = CPlantSurfPropMgr::GetSurfProperties(plantTri->m_SurfaceId);
            for (auto k = 0; k < 3; k++) {
                if (!TheCamera.IsSphereVisible(plantTri->m_Center, plantTri->m_SphereRadius) && !TheCamera.IsSphereVisibleInMirror(plantTri->m_Center, plantTri->m_SphereRadius)) {
                    continue;
                }

                auto& plantSurf = surfData->m_Plants[k];
                if (plantSurf.model_id == 0xFFFF) {
                    continue;
                }

                PPTriPlant triPlant;
                triPlant.V1 = plantTri->m_V1;
                triPlant.V2 = plantTri->m_V2;
                triPlant.V3 = plantTri->m_V3;
                triPlant.Center = plantTri->m_Center;
                triPlant.seed            = plantTri->m_Seed[k];
                triPlant.num_plants = (plantTri->m_nMaxNumPlants[k] + 8) & 0xFFF8;

                triPlant.model_id = plantSurf.model_id;
                triPlant.scale.x  = plantSurf.scale_xy;
                triPlant.scale.y  = plantSurf.scale_z;
                triPlant.texture    = grassTextures[plantSurf.uv_offset];

                auto colorMix = plantTri->m_nLighting.GetCurrentLighting();
                // Not sure if applicable: colorMix *= 255.f;
                triPlant.color = plantSurf.color;
                auto alpha     = triPlant.color.a;
                triPlant.color *= colorMix;
                triPlant.color.a = alpha;

                triPlant.intensity = plantSurf.intensity;
                triPlant.intensity_var = plantSurf.intensity_variation;
                triPlant.scale_var_xy  = plantSurf.scale_variation_xy;
                triPlant.scale_var_z   = plantSurf.scale_variation_z;
                triPlant.wind_bend_scale = plantSurf.wind_blending_scale;
                triPlant.wind_bend_var   = plantSurf.wind_blending_variation;

                CGrassRenderer::AddTriPlant(&triPlant, i);
            }

            plantTri = plantTri->m_NextTri;
        }

        CGrassRenderer::FlushTriPlantBuffer();
    }
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION,    RWRSTATE(rwALPHATESTFUNCTIONGREATEREQUAL));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(NULL));
}

// 0x5DBEB0
void CPlantMgr::_ColEntityCache_Add(CEntity* entity, bool checkAlreadyExists) {
    if (checkAlreadyExists && _ColEntityCache_FindInCache(entity))
        return;

    if (auto head = CPlantMgr::m_UnusedColEntListHead) {
        head->AddEntry(entity);
    }
}

// 0x5DB530
CPlantColEntEntry* CPlantMgr::_ColEntityCache_FindInCache(CEntity* entity) {
    if (!CPlantMgr::m_CloseColEntListHead)
        return nullptr;

    for (auto i = CPlantMgr ::m_CloseColEntListHead; i; i = i->m_NextEntry) {
        if (i->m_Entity == entity) {
            return i;
        }
    }
    return nullptr;
}

// 0x5DBEF0
void CPlantMgr::_ColEntityCache_Remove(CEntity* entity) {
    if (auto entry = _ColEntityCache_FindInCache(entity)) {
        entry->ReleaseEntry();
    }
}

// 0x5DC510
void CPlantMgr::_ColEntityCache_Update(const CVector& cameraPos, bool fast) {
    if (fast) {
        // doing a fast update, prune only ones that have no entity.
        auto* nextFastEntry = CPlantMgr::m_CloseColEntListHead;
        while (nextFastEntry) {
            auto* curEntry = nextFastEntry; // ReleaseEntry() Overwrites m_NextEntry pointer, we need to keep track of it before that can happen
            nextFastEntry  = curEntry->m_NextEntry;
            if (!curEntry->m_Entity) {
                curEntry->ReleaseEntry();
            }
        }

        return;
    }

    // prune ones that have no entity, too far or not in the same area.
    auto* nextEntry = CPlantMgr::m_CloseColEntListHead;
    while (nextEntry) {
        auto* curEntry = nextEntry; // ReleaseEntry() Overwrites m_NextEntry pointer, we need to keep track of it before that can happen
        nextEntry      = curEntry->m_NextEntry;
        if (!curEntry->m_Entity || _CalcDistanceSqrToEntity(curEntry->m_Entity, cameraPos) > PROC_OBJECTS_MAX_DISTANCE_SQUARED || !curEntry->m_Entity->IsInCurrentAreaOrBarberShopInterior()) {
            curEntry->ReleaseEntry();
        }
    }

    if (!CPlantMgr::m_UnusedColEntListHead)
        return;

    CWorld::IncrementCurrentScanCode();
    CWorld::IterateSectorsOverlappedByRect({ cameraPos, PROC_OBJECTS_MAX_DISTANCE }, [cameraPos](int32 x, int32 y) {
        for (auto* const item : GetSector(x, y)->m_buildings) {
            if (item->m_bIsProcObject || item->IsScanCodeCurrent() || !item->IsInCurrentAreaOrBarberShopInterior())
                continue;

            if (auto mi = item->GetModelInfo(); mi->GetModelType() == MODEL_INFO_ATOMIC && mi->bAtomicFlag0x200) {
                bool foundEntity = false;
                for (auto j = m_CloseColEntListHead; j; j = j->m_NextEntry) {
                    if (j->m_Entity == item) {
                        foundEntity = true;
                        break;
                    }
                }
                if (!m_CloseColEntListHead || !foundEntity) {
                    if (_CalcDistanceSqrToEntity(item, cameraPos) <= PROC_OBJECTS_MAX_DISTANCE_SQUARED) {
                        if (!m_UnusedColEntListHead || !m_UnusedColEntListHead->AddEntry(item)) {
                            return false;
                        }
                    }
                }
            }
        }

        return true;
    });
}

// 0x5DCD80
void CPlantMgr::_ProcessEntryCollisionDataSections(const CPlantColEntEntry& entry, const CVector& center, int32 iTriProcessSkipMask) {
    const auto cd = entry.m_Entity->GetColData();
    const auto numTriangles = entry.m_numTriangles;

    if (!cd || numTriangles != cd->m_nNumTriangles)
        return;

    _ProcessEntryCollisionDataSections_RemoveLocTris(entry, center, iTriProcessSkipMask, 0, numTriangles - 1);

    if (!cd->bHasFaceGroups) {
        return _ProcessEntryCollisionDataSections_AddLocTris(entry, center, iTriProcessSkipMask, 0, numTriangles - 1);
    }

    for (const auto& faceGroup : cd->GetFaceGroups()) {

        CVector out[2]{};
        TransformPoints(out, 2, entry.m_Entity->GetMatrix(), (CVector*)&faceGroup.bb);
        CBox box{};
        box.Set(out[0], out[1]);
        box.Recalc();

        if (CCollision::TestSphereBox({ center, PLANTS_MAX_DISTANCE }, box)) {
            _ProcessEntryCollisionDataSections_AddLocTris(entry, center, iTriProcessSkipMask, faceGroup.first, faceGroup.last);
        }
    }
}

// 0x5DC8B0
void CPlantMgr::_ProcessEntryCollisionDataSections_AddLocTris(const CPlantColEntEntry& entry, const CVector& center, int32 iTriProcessSkipMask, int32 start, int32 end) {
    const auto entity = entry.m_Entity;
    const auto cd = entity->GetColData();
    if (!cd)
        return;

    for (auto i = start; i <= end; i++) {
        if ((iTriProcessSkipMask != 0xFAFAFAFA && iTriProcessSkipMask != (i % 8)) || entry.m_Objects[i])
            continue;

        if (!m_UnusedLocTriListHead)
            continue;
    
        const auto& tri = cd->m_pTriangles[i];

        CVector vertices[3];
        cd->GetTrianglePoint(vertices[0], tri.vA);
        cd->GetTrianglePoint(vertices[1], tri.vB);
        cd->GetTrianglePoint(vertices[2], tri.vC);

        TransformPoints (vertices, 3, entity->GetMatrix(), vertices);

        CVector cmp[] = {
            vertices[0],
            vertices[1],
            vertices[2],
            CVector::AverageN(vertices, 3),
            (vertices[0] + vertices[1]) / 2.0f,
            (vertices[0] + vertices[2]) / 2.0f,
            (vertices[1] + vertices[2]) / 2.0f
        };

        if (rng::none_of(cmp, [center](auto v) { return DistanceBetweenPointsSquared(v, center) < PLANTS_MAX_DISTANCE_SQUARED; })) {
            continue;
        }

        auto createsPlants = g_surfaceInfos.CreatesPlants(tri.m_nMaterial);
        auto createsObjects = g_surfaceInfos.CreatesObjects(tri.m_nMaterial);

        if (!createsPlants && !createsObjects)
            continue;

        const auto unusedHead = m_UnusedLocTriListHead;
        bool bAdded = unusedHead->Add(
            vertices[0],
            vertices[1],
            vertices[2],
            tri.m_nMaterial,
            tri.m_nLight,
            createsPlants,
            createsObjects
        );

        if (!bAdded)
            continue;

        entry.m_Objects[i] = unusedHead;
        if (unusedHead->m_createsObjects) {
            auto numTrianglesAdded = g_procObjMan.ProcessTriangleAdded(unusedHead);
            if (numTrianglesAdded == 0) {
                if (!unusedHead->m_createsPlants) {
                    unusedHead->Release();
                    entry.m_Objects[i] = 0;
                }
            } else {
                unusedHead->m_createdObjects = true;
            }
        }

    }
}

// 0x5DBF20
void CPlantMgr::_ProcessEntryCollisionDataSections_RemoveLocTris(const CPlantColEntEntry& entry, const CVector& center, int32 iTriProcessSkipMask, int32 start, int32 end) {
    const auto entity = entry.m_Entity;
    const auto colModel = entity->GetColModel();

    for (auto i = start; i <= end; i++) {
        if (auto object = entry.m_Objects[i]; object) {
            if (object->m_createsObjects && !object->m_createdObjects && g_procObjMan.ProcessTriangleAdded(object)) {
                object->m_createdObjects = true;
            }
        }

        if (iTriProcessSkipMask != 0xFAFAFAFA && iTriProcessSkipMask != (i % 8))
            continue;

        if (auto& object = entry.m_Objects[i]; object) {
            CVector cmp[] = {
                object->m_V1,
                object->m_V2,
                object->m_V3,
                (object->m_V1 + object->m_V2) / 2.0f,
                (object->m_V2 + object->m_V3) / 2.0f,
                (object->m_V1 + object->m_V3) / 2.0f
            };

            if (rng::all_of(cmp, [center](auto v) { return DistanceBetweenPointsSquared(v, center) >= PLANTS_MAX_DISTANCE_SQUARED; })) {
                object->Release();
                object = nullptr;
            }
        }
    }
}

// 0x5DCF00
void CPlantMgr::_UpdateLocTris(const CVector& center, int32 a2) {
    for (auto i = m_CloseColEntListHead; i; i = i->m_NextEntry) {
        _ProcessEntryCollisionDataSections(*i, center, a2);
    }
}

// 0x5DBE40
float CPlantMgr::_CalcDistanceSqrToEntity(CEntity* entity, const CVector& posn) {
    const auto colModel = entity->GetColModel();
    CVector dst;
    entity->TransformFromObjectSpace(dst, colModel->m_boundSphere.m_vecCenter);

    auto d = DistanceBetweenPoints(dst, posn);
    if (auto r = colModel->m_boundSphere.m_fRadius; d > r) {
        d -= r;
    }

    return sq(d);
}

bool CPlantMgr::DbgCountCachedEntities(uint32*) {
    return true;
}

bool CPlantMgr::DbgCountLocTrisAndPlants(uint32, uint32*, uint32*) {
    return true;
}

bool CPlantMgr::DbgRenderCachedEntities(uint32*) {
    return true;
}

bool CPlantMgr::DbgRenderLocTris() {
    return true;
}
