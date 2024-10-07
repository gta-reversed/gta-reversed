#include "StdInc.h"

#include <extensions/ci_string.hpp>

#include "ClothesBuilder.h"
#include "PedClothesDesc.h"

CDirectory& playerImg = *(CDirectory*)0xBC12C0;
CDirectory::DirectoryInfo& playerImgEntries = *(CDirectory::DirectoryInfo*)0xBBCDC8;

auto& gBoneIndices = StaticRef<notsa::mdarray<int16, 10, 64>, 0xBBC8C8>();

auto& ms_ratiosHaveChanged  = StaticRef<bool, 0x8D0AA4>();
auto& ms_geometryHasChanged = StaticRef<bool, 0x8D0AA5>();
auto& ms_textureHasChanged  = StaticRef<bool, 0x8D0AA6>();

void CClothesBuilder::InjectHooks() {
    RH_ScopedClass(CClothesBuilder);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(LoadCdDirectory, 0x5A4190);
    RH_ScopedInstall(RequestGeometry, 0x5A41C0);
    RH_ScopedInstall(RequestTexture, 0x5A4220);
    //RH_ScopedInstall(nullptr, 0x5A42B0, { .reversed = false }); 
    //RH_ScopedInstall(nullptr, 0x5A4380, { .reversed = false }); AtomicInstanceCB
    //RH_ScopedInstall(nullptr, 0x5A43A0, { .reversed = false });
    //RH_ScopedInstall(nullptr, 0x5A44A0, { .reversed = false }); DestroyTextureCB
    RH_ScopedInstall(PreprocessClothesDesc, 0x5A44C0, { .reversed = false });
    RH_ScopedInstall(ReleaseGeometry, 0x5A47B0, { .reversed = false });
    RH_ScopedGlobalInstall(GetAtomicWithName, 0x5A4810);
    RH_ScopedInstall(AddWeightToBoneVertex, 0x5A4840);
    RH_ScopedInstall(StoreBoneArray, 0x5A48B0);
    RH_ScopedOverloadedInstall(BlendGeometry, "3", 0x5A4940, RpGeometry * (*)(RpClump*, const char*, const char*, const char*, float, float, float), { .reversed = false });
    RH_ScopedOverloadedInstall(BlendGeometry, "2", 0x5A4F10, RpGeometry* (*)(RpClump*, const char*, const char*, float, float), {.reversed = false});
    RH_ScopedInstall(CopyGeometry, 0x5A5340, { .reversed = false });
    RH_ScopedInstall(ConstructGeometryArray, 0x5A55A0, { .reversed = false }); // Makes the game crash - Probably a register is changed or smth
    RH_ScopedInstall(DestroySkinArrays, 0x5A56C0);
    RH_ScopedInstall(BuildBoneIndexConversionTable, 0x5A56E0);
    RH_ScopedInstall(CopyTexture, 0x5A5730);
    RH_ScopedInstall(PlaceTextureOnTopOfTexture, 0x5A57B0);
    RH_ScopedOverloadedInstall(BlendTextures, "Dst-Src", 0x5A5820, void (*)(RwTexture*, RwTexture*, float, float, int32));
    RH_ScopedOverloadedInstall(BlendTextures, "Dst-Src1-Src2", 0x5A59C0, void (*)(RwTexture*, RwTexture*, RwTexture*, float, float, float, int32));
    RH_ScopedOverloadedInstall(BlendTextures, "Dst-Src1-Src2-Tat", 0x5A5BC0, void (*)(RwTexture*, RwTexture*, RwTexture*, float, float, float, int32, RwTexture*));
    RH_ScopedGlobalInstall(GetTextureFromTxdAndLoadNextTxd, 0x5A5F70);
    RH_ScopedInstall(ConstructTextures, 0x5A6040, { .reversed = false });
    RH_ScopedInstall(ConstructGeometryAndSkinArrays, 0x5A6530, { .reversed = false });
    RH_ScopedInstall(CreateSkinnedClump, 0x5A69D0);
}

// inlined
// 0x5A4190
void CClothesBuilder::LoadCdDirectory() {
    playerImg.Init(550, &playerImgEntries);
    playerImg.ReadDirFile("MODELS\\PLAYER.IMG");
}

// 0x5A41C0
void CClothesBuilder::RequestGeometry(int32 modelId, uint32 modelNameKey) {
    CModelInfo::GetModelInfo(modelId)->bHasComplexHierarchy = true; // TODO/NOTE: Not sure
    uint32 offset, size;
    VERIFY(playerImg.FindItem(CKeyGen::AppendStringToKey(modelNameKey, ".DFF"), offset, size));
    CStreaming::RequestFile(modelId, offset, size, CClothes::ms_clothesImageId, STREAMING_PRIORITY_REQUEST | STREAMING_GAME_REQUIRED);
}

// 0x5A4220
int32 CClothesBuilder::RequestTexture(uint32 txdNameKey) {
    if (txdNameKey == 0) {
        return -1;
    }

    auto& defaultTxdIdx = StaticRef<uint32, 0xBC12D0>();
    const auto defaultTxd = CTxdStore::defaultTxds[defaultTxdIdx];
    defaultTxdIdx = (defaultTxdIdx + 1) % 4;

    uint32 offset, size;
    VERIFY(playerImg.FindItem(CKeyGen::AppendStringToKey(txdNameKey, ".TXD"), offset, size));
    CStreaming::RequestFile(TXDToModelId(defaultTxd), offset, size, CClothes::ms_clothesImageId, STREAMING_PRIORITY_REQUEST | STREAMING_GAME_REQUIRED);

    return defaultTxd;
}

// 0x5A44C0
void CClothesBuilder::PreprocessClothesDesc(CPedClothesDesc& desc, bool a2) {
    plugin::Call<0x5A44C0, CPedClothesDesc&, bool>(desc, a2);
}

// unused
// 0x5A47B0
void CClothesBuilder::ReleaseGeometry(int32 numToRelease) {
    for (auto i = numToRelease; i; CStreaming::SetModelIsDeletable(MODEL_CLOTHES01_ID384 + i))
        --i;
}

// 0x5A4810
RpAtomic* GetAtomicWithName(RpClump* clump, const char* name) {
    struct Context {
        notsa::ci_string_view name{};
        RpAtomic*             atomic{};
    } c{name};
    RpClumpForAllAtomics(clump, [](RpAtomic* a, void* data) { // 0x5A47E0
        auto& ctx = *static_cast<Context*>(data);
        if (ctx.name == GetFrameNodeName(RpAtomicGetFrame(a))) {
            ctx.atomic = a;
        }
        return a;
    }, &c);
    return c.atomic;
}

// 0x5A4840
void CClothesBuilder::AddWeightToBoneVertex(float (&weights)[8], uint8(&boneVertexIdxs)[8], float weightToAdd, RwUInt32 targetVertexIdx) { // Unknown OG name
    if (weightToAdd == 0.f) {
        return;
    }
    for (auto i = 0; i < 8; i++) {
        if (weights[i] == 0.f) { // Weight not yet used?
            boneVertexIdxs[i] = targetVertexIdx; // Add to list
            weights[i]        = weightToAdd;
            weights[i + 1]    = 0.f;             // Mark next as unused [Though this step in our case is not necessary as the whole weights array is already zero-inited]
            return;
        }
        if (boneVertexIdxs[i] == targetVertexIdx) { // Already in the list, use that
            weights[i] += weightToAdd;
            return;
        }
    }
    NOTSA_UNREACHABLE(); // OG code had UB in this case
}

// 0x5A48B0
void CClothesBuilder::StoreBoneArray(RpClump* clump, int32 idx) {
    const auto a = GetAtomicWithName(clump, "normal");
    assert(a);

    const auto h = RpSkinAtomicGetHAnimHierarchy(a);
    assert(h);
    
    rng::fill(gBoneIndices[idx], -1);
    for (auto i = h->numNodes; i-- > 0;) {
        gBoneIndices[idx][i] = static_cast<int16>(h->pNodeInfo[i].nodeID);
    }
}

/*
* @notsa
* 
* Based on 0x5A4940
* Blend any number of geometries together
* The result is stored in the 0th frame's geometry.
*
* @arg clump  The clump to which the frames belong to
* @arg frames A list of frames whose geometry should be blended together
*/
template<size_t N>
RpGeometry* BlendGeometry(RpClump* clump, std::pair<const char*, float> (&&frameNamesRatios)[N]) {
#ifdef NOTSA_DEBUG
    {
        float a{};
        for (auto&& v : frameNamesRatios) {
            a += v.second;
        }
        assert(a >= 0.f && a <= 1.f);
    }
#endif

    // Process data needed for blending
    struct GeoBlendData {
        RpAtomic*        a;
        RpGeometry*      g;
        const RwUInt8*   boneIdxs;
        RwMatrixWeights* boneWeights;
        RwTexCoords*     uvs;
        CVector*         verts;
        CVector*         nrmls;
        float            r; // Blend ratio
    } fds[N];
    auto& out = fds[0];
    for (auto&& [i, v] : notsa::enumerate(frameNamesRatios)) {
        const auto a  = GetAtomicWithName(clump, v.first);
        const auto g  = RpAtomicGetGeometry(a);
        const auto s  = RpSkinGeometryGetSkin(g);
        const auto mt = RpGeometryGetMorphTarget(g, 0);
        fds[i] = {
            a,
            g,
            (const RwUInt8*)RpSkinGetVertexBoneIndices(s), // NOTE: Not sure why it's casted to UInt8, but that really is how the data is stored / TODO: Okay, so actually it seems like something is fucked
            RpSkinGetVertexBoneWeights(s),
            RpGeometryGetVertexTexCoords(g, 1),
            (CVector*)RpMorphTargetGetVertices(mt),
            (CVector*)RpMorphTargetGetVertexNormals(mt),
            v.second
        };
    }

    RpGeometryLock(out.g, rpGEOMETRYLOCKALL);

    for (auto i = 0; i < RpGeometryGetNumVertices(out.g); i++) {
        const auto Blend = [&](auto&& Get) {
            return multiply_weighted(fds | rng::views::transform([&](auto&& fd) { return WeightedValue{ std::invoke(Get, &fd)[i], fd.r }; }));
        };
        out.verts[i] = Blend(&GeoBlendData::verts);
        out.nrmls[i] = Blend(&GeoBlendData::nrmls);
        out.uvs[i]   = Blend(&GeoBlendData::uvs);

        // Helper function to get the weight at a given weight index
        const auto RwMatrixWeightsGetWeight = [](RwMatrixWeights& mw, int32 wi) -> float& {
            switch (wi) {
            case 0:  return mw.w0;
            case 1:  return mw.w1;
            case 2:  return mw.w2;
            case 3:  return mw.w3;
            default: NOTSA_UNREACHABLE();
            }
        };

        // Calculate bone weights
        float weights[8]{};
        uint8 boneVertexIdxs[8]{};
        for (auto& fd : fds) {
            for (auto wi = 0; wi < 4; wi++) {
                const auto vtxIdx = i + wi;
                CClothesBuilder::AddWeightToBoneVertex(
                    weights,
                    boneVertexIdxs,
                    RwMatrixWeightsGetWeight(fd.boneWeights[vtxIdx], wi),
                    fd.boneIdxs[vtxIdx]
                );
            }
        }
        for (auto b = 0; b < 4; b++) {
            const_cast<RwUInt8*>(out.boneIdxs)[i + b] = boneVertexIdxs[b];
        }
        const auto t = weights[4] != 0.f
            ? 1.f / std::accumulate(weights, weights + 4, 0.f)
            : 1.f;
        for (auto wi = 0; wi < 4; wi++) {
            RwMatrixWeightsGetWeight(out.boneWeights[i + wi], wi) = weights[wi] * t;
        }
    }

    RpGeometryUnlock(out.g);
    out.g->refCount++; // TODO: Function missing

    return out.g;
}

// 0x5A4940
RpGeometry* CClothesBuilder::BlendGeometry(RpClump* clump, const char* frameName0, const char* frameName1, const char* frameName2, float r0, float r1, float r2) {
    return ::BlendGeometry(clump, { {frameName0, r0}, {frameName1, r1}, {frameName2, r2} });
}

// 0x5A4F10
RpGeometry* CClothesBuilder::BlendGeometry(RpClump* clump, const char* frameName0, const char* frameName1, float r0, float r1) {
    return ::BlendGeometry(clump, { {frameName0, r0}, {frameName1, r1} });
}

// 0x5A5340
RpGeometry* CClothesBuilder::CopyGeometry(RpClump* clump, const char* a2, const char* a3) {
    return plugin::CallAndReturn<RpGeometry*, 0x5A5340, RpClump*, const char*, const char*>(clump, a2, a3);
}

// 0x5A55A0
void CClothesBuilder::ConstructGeometryArray(RpGeometry** out, uint32* modelNameKeys, float normal, float fatness, float strength) {
    for (auto i = 0; i < 10; i++) {
        if (modelNameKeys[i] == 0) {
            *out = nullptr;
            continue;
        }
        const auto modelIdx = (eModelID)((int)MODEL_CLOTHES01_ID384 + i);
        const auto mi       = CModelInfo::GetModelInfo(modelIdx);

        CModelInfo::GetModelInfo(modelIdx)->bHasComplexHierarchy = true;
        RequestGeometry(modelIdx, modelNameKeys[i]);
        CStreaming::LoadAllRequestedModels(true);

        if (i + 1 < 10 && modelNameKeys[i + 1]) { // Request next model to be loaded in advance
            RequestGeometry((eModelID)((int)MODEL_CLOTHES01_ID384 + i + 1), modelNameKeys[i + 1]);
            CStreaming::LoadRequestedModels();
        }

        *out = BlendGeometry(mi->m_pRwClump, "normal", "fat", "ripped", normal, fatness, strength);
        StoreBoneArray(mi->m_pRwClump, i);
        CStreaming::RemoveModel(modelIdx);
    }
}

// inlined, see 0x5A6CE1
// 0x5A56C0
void CClothesBuilder::DestroySkinArrays(RwMatrixWeights* weights, RwUInt32* bones) {
    // TODO: Should this be `delete[]` or `delete`?
    delete weights;
    delete bones;
}

// 0x5A56E0
void CClothesBuilder::BuildBoneIndexConversionTable(uint8* pTable, RpHAnimHierarchy* hier, int32 index) {
    for (const auto [tableIdx, boneId] : notsa::enumerate(gBoneIndices[index])) {
        if (boneId == -1) {
            break;
        }
        const auto idx = RpHAnimIDGetIndex(hier, boneId);
        pTable[tableIdx] = idx == 0xFF ? 0 : idx;
    }
}

void AssertTextureLayouts(std::initializer_list<RwTexture*> textures) {
    assert(textures.size() >= 2);
    for (auto i = 0u; i < textures.size() - 1; i++) {
        const auto r1 = RwTextureGetRaster(textures.begin()[i]), r2 = RwTextureGetRaster(textures.begin()[i + 1]);

        assert(RwRasterGetWidth(r1) == RwRasterGetWidth(r2));
        assert(RwRasterGetHeight(r1) == RwRasterGetHeight(r2));
        assert(RwRasterGetDepth(r1) == RwRasterGetDepth(r2));
        assert(RwRasterGetDepth(r1) == 32);
    }
}

// 0x5A5730
RwTexture* CClothesBuilder::CopyTexture(RwTexture* srcTex) {
    const auto srcRaster = RwTextureGetRaster(srcTex);
    
    // Create a new raster to which we're going to copy to
    const auto dstRaster = RwRasterCreate(
        RwRasterGetWidth(srcRaster),
        RwRasterGetHeight(srcRaster),
        RwRasterGetDepth(srcRaster),
        (RwRasterGetFormat(srcRaster) & rwRASTERFORMATPIXELFORMATMASK) | 4 // TODO
    );
    
    // Copy data from the src raster to this one
    const auto srcLck = RwRasterLock(srcRaster, 0, rwRASTERLOCKREAD);
    memcpy(
        RwRasterLock(dstRaster, 0, rwRASTERLOCKWRITE),
        srcLck,
        RwRasterGetHeight(srcRaster) * RwRasterGetStride(srcRaster)
    );
    RwRasterUnlock(srcRaster);
    RwRasterUnlock(dstRaster);

    // Create a texture from the copied raster
    const auto dstTex = RwTextureCreate(dstRaster);
    RwTextureSetFilterMode(dstTex, rwFILTERLINEAR);

    AssertTextureLayouts({ dstTex, srcTex });

    return dstTex;
}

// 0x5A57B0
void CClothesBuilder::PlaceTextureOnTopOfTexture(RwTexture* dstTex, RwTexture* srcTex) {
    ZoneScoped;

    AssertTextureLayouts({ dstTex, srcTex });

    const auto dstRaster = RwTextureGetRaster(dstTex);
    const auto srcRaster = RwTextureGetRaster(srcTex);

    auto dstIt = (RwUInt32*)RwRasterLock(dstRaster, 0, rwRASTERLOCKREADWRITE);
    auto srcIt = (RwUInt32*)RwRasterLock(srcRaster, 0, rwRASTERLOCKREADWRITE);

    // NOTE: They don't skip the stride, but it's fine [This way vectorization should be easier for the compiler]
    for (auto i = RwRasterGetHeight(dstRaster) * RwRasterGetWidth(dstRaster); i-- > 0; dstIt++, srcIt++) {
        if (*srcIt & 0xFF000000) { // Check alpha != 0
            *dstIt = *srcIt;
        }
    }

    RwRasterUnlock(dstRaster);
    RwRasterUnlock(srcRaster);
}

// 0x5A5820
void CClothesBuilder::BlendTextures(RwTexture* dst, RwTexture* src, float r1, float r2, int32 numColors) {
    ZoneScoped;

    AssertTextureLayouts({ dst, src });

    const auto dstRaster = RwTextureGetRaster(dst);
    const auto srcRaster = RwTextureGetRaster(src);

    CTimer::Suspend();

    auto srcIt = RwRasterLock(srcRaster, 0, rwRASTERLOCKREAD);
    auto dstIt = RwRasterLock(dstRaster, 0, rwRASTERLOCKREADWRITE);

    for (auto i = RwRasterGetHeight(dstRaster) * RwRasterGetWidth(dstRaster); i-- > 0; dstIt++, srcIt++) {
        for (auto c = 3; i-- > 0; dstIt++, srcIt++) { // Copy RGB, alpha stays the same
            *dstIt = multiply_weighted<RwUInt8>({ { *dstIt, r1 }, { *srcIt, r2 } });
        }
    }

    RwRasterUnlock(dstRaster);
    RwRasterUnlock(srcRaster);

    CTimer::Resume();
}

// 0x5A59C0
void CClothesBuilder::BlendTextures(RwTexture* dst, RwTexture* src1, RwTexture* src2, float r1, float r2, float r3, int32) {
    ZoneScoped;

    AssertTextureLayouts({ dst, src1, src2 });

    const auto dstRaster  = RwTextureGetRaster(dst);
    const auto src1Raster = RwTextureGetRaster(src1);
    const auto src2Raster = RwTextureGetRaster(src2);

    CTimer::Suspend();

    auto src1It = RwRasterLock(src1Raster, 0, rwRASTERLOCKREAD);
    auto src2It = RwRasterLock(src2Raster, 0, rwRASTERLOCKREAD);
    auto dstIt  = RwRasterLock(dstRaster, 0, rwRASTERLOCKREADWRITE);

    for (auto i = RwRasterGetHeight(dstRaster) * RwRasterGetWidth(dstRaster); i-- > 0; dstIt++, src1It++, src2It++) {
        for (auto c = 3; i-- > 0; dstIt++, src1It++, src2It++) { // Copy RGB, alpha doesn't change
            *dstIt = multiply_weighted<RwUInt8>({ { *dstIt, r1 }, { *src1It, r2 }, { *src2It, r3 } });
        }
    }

    RwRasterUnlock(dstRaster);
    RwRasterUnlock(src1Raster);
    RwRasterUnlock(src2Raster);

    CTimer::Resume();
}

// 0x5A5BC0
void CClothesBuilder::BlendTextures(RwTexture* dst, RwTexture* src1, RwTexture* src2, float r1, float r2, float r3, int32 numColors, RwTexture* tattoos) {
    ZoneScoped;

    AssertTextureLayouts({ dst, src1, src2, tattoos });

    const auto dstRaster  = RwTextureGetRaster(dst);
    const auto src1Raster = RwTextureGetRaster(src1);
    const auto src2Raster = RwTextureGetRaster(src2);
    const auto tatRaster  = RwTextureGetRaster(tattoos);

    CTimer::Suspend();

    auto src1It = RwRasterLock(src1Raster, 0, rwRASTERLOCKREAD);
    auto src2It = RwRasterLock(src2Raster, 0, rwRASTERLOCKREAD);
    auto tatIt  = RwRasterLock(tatRaster, 0, rwRASTERLOCKREAD);
    auto dstIt  = RwRasterLock(dstRaster, 0, rwRASTERLOCKREADWRITE);

    for (auto i = RwRasterGetHeight(dstRaster) * RwRasterGetWidth(dstRaster); i-- > 0; dstIt++, src1It++, src2It++, tatIt++) {
        const auto tatAlphaT = (float)tatIt[3] / 255.f;
        for (auto c = 3; i-- > 0; dstIt++, src1It++, src2It++, tatIt++) { // Copy RGB, alpha doesn't change
            *dstIt = (RwUInt8)lerp(multiply_weighted<RwUInt8>({ { *dstIt, r1 }, { *src1It, r2 }, { *src2It, r3 } }), *tatIt, tatAlphaT);
        }
    }

    RwRasterUnlock(dstRaster);
    RwRasterUnlock(src1Raster);
    RwRasterUnlock(src2Raster);
    RwRasterUnlock(tatRaster);

    CTimer::Resume();
}

// 0x5A5F70
RwTexture* GetTextureFromTxdAndLoadNextTxd(RwTexture* dstTex, int32 txdId_withTexture, int32 CRC_nextTxd, int32* nextTxdId) {
    if (txdId_withTexture == -1) {
        if (CRC_nextTxd) {
            *nextTxdId = CClothesBuilder::RequestTexture(CRC_nextTxd);
            CStreaming::LoadRequestedModels();
        } else {
            *nextTxdId = -1;
        }
        return dstTex;
    }

    CStreaming::LoadAllRequestedModels(true);
    if (CRC_nextTxd) {
        *nextTxdId = CClothesBuilder::RequestTexture(CRC_nextTxd);
        CStreaming::LoadRequestedModels();
    } else {
        *nextTxdId = -1;
    }
    const auto tex = GetFirstTexture(CTxdStore::GetTxd(txdId_withTexture));
    const auto res = dstTex
        ? CClothesBuilder::PlaceTextureOnTopOfTexture(dstTex, tex), dstTex
        : CClothesBuilder::CopyTexture(tex);
    CStreaming::RemoveModel(TXDToModelId(txdId_withTexture));
    return res;
}

// 0x5A6040
void CClothesBuilder::ConstructTextures(RwTexDictionary* dict, uint32* hashes, float factorA, float factorB, float factorC) {
    plugin::Call<0x5A6040, RwTexDictionary*, uint32*, float, float, float>(dict, hashes, factorA, factorB, factorC);
}

// 0x5A6530
void CClothesBuilder::ConstructGeometryAndSkinArrays(RpHAnimHierarchy* pBoneHier, RpGeometry** ppGeometry, RwMatrixWeights** ppWeights, uint32** ppIndices, uint32 numModels, RpGeometry** pGeometrys, RpMaterial** pMaterial) {
    plugin::Call<0x5A6530>(pBoneHier, ppGeometry, ppWeights, ppIndices, numModels, pGeometrys, pMaterial);
}

// 0x5A69D0
RpClump* CClothesBuilder::CreateSkinnedClump(RpClump* bones, RwTexDictionary* dict, CPedClothesDesc& ndscr, const CPedClothesDesc* odscr, bool bCutscenePlayer) {
    LoadCdDirectory();

    const struct {
        eClothesModelPart mp;
        const char*       name;
    } parts[]{
        {eClothesModelPart::CLOTHES_MODEL_TORSO, "torso"},
        {eClothesModelPart::CLOTHES_MODEL_HEAD, "head"},
        {eClothesModelPart::CLOTHES_MODEL_HANDS, "hands"},
        {eClothesModelPart::CLOTHES_MODEL_LEGS, "legs"},
        {eClothesModelPart::CLOTHES_MODEL_SHOES, "feet"}
    };
    for (const auto [mp, name] : parts) {
        if (!ndscr.m_anModelKeys[(int)mp]) {
            ndscr.SetModel(name, mp);
        }
    }

    if (odscr) {
        ms_geometryHasChanged = false;
        ms_ratiosHaveChanged  = false;
        if (odscr->m_fFatStat != ndscr.m_fFatStat || odscr->m_fMuscleStat != ndscr.m_fMuscleStat) {
            ms_textureHasChanged = true;
            ms_geometryHasChanged = true;
        } else {
            ms_textureHasChanged = true;
        }
        ms_geometryHasChanged = !rng::equal(ndscr.m_anModelKeys, odscr->m_anModelKeys);
        ms_ratiosHaveChanged  = !rng::equal(ndscr.m_anTextureKeys, odscr->m_anTextureKeys);
        if (!ms_ratiosHaveChanged && !ms_geometryHasChanged && !ms_textureHasChanged) {
            return nullptr;
        }
    } else {
        ms_ratiosHaveChanged = ms_geometryHasChanged = ms_textureHasChanged = true;
    }
    CPedClothesDesc dscr = ndscr;
    PreprocessClothesDesc(dscr, bCutscenePlayer);

    //> 0x5A42B0 - Calculate blend ratios
    float rNormal, rFatness, rMuscle;
    {
        rMuscle  = std::clamp(CStats::GetStatValue(STAT_MUSCLE) / 1000.f, 0.f, 1.f);
        rFatness = std::clamp((dscr.m_fFatStat - 200.f) / 800.f, 0.f, 1.f);
        rNormal  = 1.f - rMuscle - rFatness;
        if (rNormal <= 0.f) {
            const auto t = 1.f / (rFatness + rMuscle);
            rMuscle  *= t;
            rFatness *= t;
            rNormal   = 0.f;
        }
    }

    if ((ms_textureHasChanged || ms_ratiosHaveChanged) && !bCutscenePlayer) {
        RwTexDictionaryForAllTextures(dict, [](RwTexture* t, void* data) {
            RwTexDictionaryRemoveTexture(t);
            RwTextureDestroy(t);
            return t;
        }, nullptr);
        ConstructTextures(dict, dscr.m_anTextureKeys.data(), rNormal, rFatness, rMuscle);
    }

    constexpr auto NO_BODY_PARTS = 10;

    constexpr const char* BODY_PART_TEX_NAMES[NO_BODY_PARTS]{
        "torso",
        "head",
        "torso",
        "legs",
        "feet",
        "necklace",
        "watch",
        "glasses",
        "hat",
        "extra1"
    };

    //> 0x5A6B2C
    RpMaterial*       ms[NO_BODY_PARTS];
    for (uint32 i{}; const auto name : BODY_PART_TEX_NAMES) {
        ms[i++] = [&]() -> RpMaterial* {
            if (const auto tex = RwTexDictionaryFindNamedTexture(dict, name)) {
                const auto mat = RpMaterialCreate();
                RpMaterialSetTexture(mat, tex);
                const auto clr = RwRGBA(0xFF, 0xFF, 0xFF, 0xFF);
                RpMaterialSetColor(mat, &clr);
                return mat;
            }
            return nullptr;
        }();
    }

    //> 0x5A6C5D
    RpGeometry* gs[NO_BODY_PARTS];
    ConstructGeometryArray(gs, dscr.m_anModelKeys.data(), rNormal, rFatness, rMuscle);

    //> 0x5A6C6A
    const auto boneAtomic = GetFirstAtomic(bones);
    const auto boneSkin   = RpSkinGeometryGetSkin(RpAtomicGetGeometry(boneAtomic));
    const auto boneAnimHr = RpSkinAtomicGetHAnimHierarchy(boneAtomic);

    RwMatrixWeights* boneWeights;
    RwUInt32*        boneIdxs;
    RpGeometry*      tmpGeo;
    ConstructGeometryAndSkinArrays(
        boneAnimHr,
        &tmpGeo,
        &boneWeights,
        &boneIdxs,
        NO_BODY_PARTS,
        gs,
        ms
    );
    RpSkinGeometrySetSkin(
        tmpGeo,
        RpSkinCreate(
            RpGeometryGetNumVertices(tmpGeo),
            RpSkinGetNumBones(boneSkin),
            boneWeights,
            boneIdxs,
            const_cast<RwMatrix*>(RpSkinGetSkinToBoneMatrices(boneSkin)) // TODO
        )
    );
    DestroySkinArrays(boneWeights, boneIdxs);
    
    const auto hier = RpHAnimHierarchyCreateFromHierarchy(
        boneAnimHr,
        (RpHAnimHierarchyFlag)boneAnimHr->flags,       // TODO: Use function to access
        boneAnimHr->currentAnim->maxInterpKeyFrameSize // TODO: Use function to access
    );

    const auto childFrame = RwFrameCreate();
    RpHAnimFrameSetHierarchy(childFrame, hier);

    const auto atomic = RpAtomicCreate();
    RpAtomicSetGeometry(atomic, tmpGeo, NULL);
    RpSkinAtomicSetHAnimHierarchy(atomic, hier);
    RpAtomicSetFrame(atomic, childFrame);
    RpSkinAtomicSetType(atomic, rpSKINTYPEGENERIC);

    const auto rootFrame = RwFrameCreate();
    RwFrameAddChild(rootFrame, childFrame);

    const auto clump = RpClumpCreate();
    RpClumpSetFrame(clump, rootFrame);
    RpClumpAddAtomic(clump, atomic);

    // Free memory
    {
        RpGeometryDestroy(tmpGeo);

        RwTexDictionarySetCurrent(dict);
        for (auto i = 0; i < NO_BODY_PARTS; i++) {
            if (gs[i]) {
                RpGeometryDestroy(gs[i]);
            }
            if (ms[i]) {
                RpMaterialDestroy(ms[i]);
            }
        }
    }

    return clump;
}
