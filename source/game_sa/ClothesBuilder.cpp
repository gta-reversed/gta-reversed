#include "StdInc.h"

#include <extensions/ci_string.hpp>

#include "ClothesBuilder.h"
#include "PedClothesDesc.h"

auto& playerImg = StaticRef<CDirectory>(0xBC12C0);
auto& playerImgEntries = StaticRef<CDirectory::DirectoryInfo>(0xBBCDC8);

auto& gBoneIndices = StaticRef<notsa::mdarray<int16, 10, 64>>(0xBBC8C8);

auto& ms_ratiosHaveChanged  = StaticRef<bool>(0x8D0AA4);
auto& ms_geometryHasChanged = StaticRef<bool>(0x8D0AA5);
auto& ms_textureHasChanged  = StaticRef<bool>(0x8D0AA6);

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
    RH_ScopedInstall(PreprocessClothesDesc, 0x5A44C0);
    RH_ScopedInstall(ReleaseGeometry, 0x5A47B0);
    RH_ScopedGlobalInstall(GetAtomicWithName, 0x5A4810);
    RH_ScopedInstall(AddWeightToBoneVertex, 0x5A4840);
    RH_ScopedInstall(StoreBoneArray, 0x5A48B0);
    RH_ScopedOverloadedInstall(BlendGeometry, "3", 0x5A4940, RpGeometry * (*)(RpClump*, const char*, const char*, const char*, float, float, float));
    RH_ScopedOverloadedInstall(BlendGeometry, "2", 0x5A4F10, RpGeometry* (*)(RpClump*, const char*, const char*, float, float));
    RH_ScopedInstall(CopyGeometry, 0x5A5340);
    RH_ScopedInstall(ConstructGeometryArray, 0x5A55A0);
    RH_ScopedInstall(DestroySkinArrays, 0x5A56C0);
    RH_ScopedInstall(BuildBoneIndexConversionTable, 0x5A56E0);
    RH_ScopedInstall(CopyTexture, 0x5A5730);
    RH_ScopedInstall(PlaceTextureOnTopOfTexture, 0x5A57B0);
    RH_ScopedOverloadedInstall(BlendTextures, "Dst-Src", 0x5A5820, void (*)(RwTexture*, RwTexture*, float, float, int32));
    RH_ScopedOverloadedInstall(BlendTextures, "Dst-Src1-Src2", 0x5A59C0, void (*)(RwTexture*, RwTexture*, RwTexture*, float, float, float, int32));
    RH_ScopedOverloadedInstall(BlendTextures, "Dst-Src1-Src2-Tat", 0x5A5BC0, void (*)(RwTexture*, RwTexture*, RwTexture*, float, float, float, int32, RwTexture*));
    RH_ScopedGlobalInstall(GetTextureFromTxdAndLoadNextTxd, 0x5A5F70);
    RH_ScopedInstall(ConstructTextures, 0x5A6040);
    RH_ScopedInstall(ConstructGeometryAndSkinArrays, 0x5A6530);
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

    uint32 size;
    CdStreamPos pos;
    VERIFY(playerImg.FindItem(CKeyGen::AppendStringToKey(modelNameKey, ".DFF"), pos, size));
    CStreaming::RequestFile(modelId, pos, size, CClothes::ms_clothesImageId, STREAMING_PRIORITY_REQUEST | STREAMING_GAME_REQUIRED);
}

// 0x5A4220
int32 CClothesBuilder::RequestTexture(uint32 txdNameKey) {
    if (txdNameKey == 0) {
        return -1;
    }

    auto& defaultTxdIdx = StaticRef<uint32>(0xBC12D0);
    const auto defaultTxd = CTxdStore::defaultTxds[defaultTxdIdx];
    defaultTxdIdx = (defaultTxdIdx + 1) % 4;

    uint32 size;
    CdStreamPos pos;
    VERIFY(playerImg.FindItem(CKeyGen::AppendStringToKey(txdNameKey, ".TXD"), pos, size));
    CStreaming::RequestFile(TXDToModelId(defaultTxd), pos, size, CClothes::ms_clothesImageId, STREAMING_PRIORITY_REQUEST | STREAMING_GAME_REQUIRED);

    return defaultTxd;
}

// 0x5A44C0
void CClothesBuilder::PreprocessClothesDesc(CPedClothesDesc& desc, bool checkCutscenePlayer) {
    uint32 ignoredModels[8]{};
    std::fill(std::begin(ignoredModels), std::end(ignoredModels), 0xFFFFFFFF);

    bool isExclusive[8]{};
    uint32 ruleIdx = 0;
    while (ruleIdx < CClothes::ms_numRuleTags) {
        const auto rule = (eClothRule)CClothes::ms_clothesRules[ruleIdx];
        uint32 findModel = -1, findTex = -1, setModel = -1, setTex = -1;
        switch (rule) {
        case eClothRule::TAG_CUTS: // body part replaced when wearing a given model
            findModel = CClothes::ms_clothesRules[ruleIdx + 1];
            setModel  = CClothes::ms_clothesRules[ruleIdx + 2];
            ruleIdx += 3;
            break;
        case eClothRule::TAG_SETC: // model + dependent texture copied onto another part
            findModel = CClothes::ms_clothesRules[ruleIdx + 1];
            findTex   = CClothes::ms_clothesRules[ruleIdx + 2];
            setModel  = CClothes::ms_clothesRules[ruleIdx + 3];
            setTex    = CClothes::ms_clothesRules[ruleIdx + 4];
            ruleIdx += 5;
            break;
        case eClothRule::TAG_TEX:
            findModel = CClothes::ms_clothesRules[ruleIdx + 1];
            setTex    = CClothes::ms_clothesRules[ruleIdx + 2];
            ruleIdx += 3;
            break;
        case eClothRule::TAG_HIDE:
            findModel = CClothes::ms_clothesRules[ruleIdx + 1];
            findTex   = CClothes::ms_clothesRules[ruleIdx + 2];
            ruleIdx += 3;
            break;
        case eClothRule::TAG_END_IGNORE:
            for (auto i = 0; i < 8; i++) {
                if (ignoredModels[i] == CClothes::ms_clothesRules[ruleIdx + 1]) {
                    ignoredModels[i] = -1;
                    isExclusive[i] = true;
                    break;
                }
            }
            ruleIdx += 2;
            break;
        case eClothRule::TAG_IGNORE:
            for (auto i = 0; i < 8; i++) {
                if (ignoredModels[i] == 0xFFFFFFFF) {
                    ignoredModels[i] = CClothes::ms_clothesRules[ruleIdx + 1];
                    isExclusive[i] = true;
                    break;
                }
            }
            ruleIdx += 2;
            break;
        case eClothRule::TAG_END_EXCLUSIVE:
            for (auto i = 0; i < 8; i++) {
                if (ignoredModels[i] == CClothes::ms_clothesRules[ruleIdx + 1]) {
                    ignoredModels[i] = -1;
                    isExclusive[i] = false;
                    break;
                }
            }
            ruleIdx += 2;
            break;
        case eClothRule::TAG_EXCLUSIVE:
            for (auto i = 0; i < 8; i++) {
                if (ignoredModels[i] == 0xFFFFFFFF) {
                    ignoredModels[i] = CClothes::ms_clothesRules[ruleIdx + 1];
                    isExclusive[i] = false;
                    break;
                }
            }
            ruleIdx += 2;
            break;
        }

        // Check each of the 10 body parts against the ignore/exclusive lists
        for (auto modelPart = 0; modelPart < CLOTHES_MODEL_TOTAL; modelPart++) {
            const int32 texPart = findTex != 0xFFFFFFFF ? findTex : modelPart;
            bool skip = false;
            for (auto i = 0; i < 8; i++) {
                if (ignoredModels[i] != 0xFFFFFFFF && (desc.m_anModelKeys[texPart] == ignoredModels[i]) == isExclusive[i]) {
                    skip = true;
                    break;
                }
            }
            if (skip) {
                continue;
            }
            switch (rule) {
            case eClothRule::TAG_CUTS:
                if (checkCutscenePlayer && desc.m_anModelKeys[modelPart] == findModel) {
                    desc.SetModel((uint32)setModel, (eClothesModelPart)modelPart);
                }
                break;
            case eClothRule::TAG_SETC:
                if (desc.m_anModelKeys[modelPart] == findModel) {
                    if (findTex == CLOTHES_MODEL_TORSO) {
                        findTex = (uint32)CClothes::GetDependentTexture((eClothesModelPart)findTex);
                    }
                    if (setTex == 0) {
                        const auto depTex = CClothes::GetDependentTexture((eClothesModelPart)findTex);
                        setTex = desc.m_anTextureKeys[depTex];
                    }
                    if (setModel == 0) {
                        setModel = desc.m_anModelKeys[findTex];
                    }
                    desc.SetTextureAndModel(setTex, setModel, CClothes::GetDependentTexture((eClothesModelPart)findTex));
                }
                break;
            case eClothRule::TAG_TEX:
                if (desc.m_anModelKeys[modelPart] == findModel) {
                    desc.SetTextureAndModel(setTex, findModel, CClothes::GetDependentTexture((eClothesModelPart)modelPart));
                }
                break;
            case eClothRule::TAG_HIDE:
                if (desc.m_anModelKeys[modelPart] == findModel) {
                    desc.SetModel((uint32)0, (eClothesModelPart)findTex);
                }
                break;
            default:
                break;
            }
        }
    }
}

// unused
// 0x5A47B0
void CClothesBuilder::ReleaseGeometry(int32 numToRelease) {
    for (; numToRelease; numToRelease--) {
        CStreaming::SetModelIsDeletable(MODEL_CLOTHES01_ID384 + numToRelease);
    }
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

// Helper shared by both BlendGeometry overloads.
// Blends vertex positions, normals, UVs and skin weights of the given frames' geometries
// into the first frame's geometry. Returns the first geometry.
template<size_t N>
static RpGeometry* BlendGeometries(RpClump* clump, const char* (&&frameNames)[N], const float (&&ratios)[N]) {
    struct GeoBlendData {
        RpGeometry*      g;
        const RwUInt8*   boneIdxs;
        RwMatrixWeights* boneWeights;
        RwTexCoords*     uvs;
        RwV3d*           verts;
        RwV3d*           normals;
        float            r;
    } fds[N];

    for (auto i = 0u; i < N; i++) {
        const auto atomic = GetAtomicWithName(clump, frameNames[i]);
        const auto g      = RpAtomicGetGeometry(atomic);
        const auto skin   = RpSkinGeometryGetSkin(g);
        const auto mt     = RpGeometryGetMorphTarget(g, 0);
        fds[i] = {
            g,
            (const RwUInt8*)RpSkinGetVertexBoneIndices(skin),
            RpSkinGetVertexBoneWeights(skin),
            RpGeometryGetVertexTexCoords(g, 1),
            RpMorphTargetGetVertices(mt),
            RpMorphTargetGetVertexNormals(mt),
            ratios[i],
        };
    }
    auto& out = fds[0];

    RpGeometryLock(out.g, rpGEOMETRYLOCKALL);

    const auto numVerts = RpGeometryGetNumVertices(out.g);
    for (auto i = 0; i < numVerts; i++) {
        // Blend vertex positions
        out.verts[i].x = out.verts[i].x * out.r;
        out.verts[i].y = out.verts[i].y * out.r;
        out.verts[i].z = out.verts[i].z * out.r;
        for (auto f = 1u; f < N; f++) {
            out.verts[i].x += fds[f].verts[i].x * fds[f].r;
            out.verts[i].y += fds[f].verts[i].y * fds[f].r;
            out.verts[i].z += fds[f].verts[i].z * fds[f].r;
        }

        // Blend vertex normals
        out.normals[i].x = out.normals[i].x * out.r;
        out.normals[i].y = out.normals[i].y * out.r;
        out.normals[i].z = out.normals[i].z * out.r;
        for (auto f = 1u; f < N; f++) {
            out.normals[i].x += fds[f].normals[i].x * fds[f].r;
            out.normals[i].y += fds[f].normals[i].y * fds[f].r;
            out.normals[i].z += fds[f].normals[i].z * fds[f].r;
        }
        RwV3dNormalize(&out.normals[i], &out.normals[i]);

        // Blend UVs
        out.uvs[i].u = out.uvs[i].u * out.r;
        out.uvs[i].v = out.uvs[i].v * out.r;
        for (auto f = 1u; f < N; f++) {
            out.uvs[i].u += fds[f].uvs[i].u * fds[f].r;
            out.uvs[i].v += fds[f].uvs[i].v * fds[f].r;
        }

        // Blend bone weights
        float weights[8]{};
        uint8 boneVertexIdxs[8]{};
        for (auto& fd : fds) {
            for (auto wi = 0; wi < 4; wi++) {
                const auto vtxIdx = i * 4 + wi;
                CClothesBuilder::AddWeightToBoneVertex(
                    weights,
                    boneVertexIdxs,
                    (&fd.boneWeights[i].w0)[wi] * fd.r,
                    fd.boneIdxs[vtxIdx]
                );
            }
        }
        for (auto b = 0; b < 4; b++) {
            const_cast<RwUInt8*>(out.boneIdxs)[i * 4 + b] = boneVertexIdxs[b];
        }
        if (weights[4] != 0.f) {
            const auto t = 1.f / (weights[0] + weights[1] + weights[2] + weights[3]);
            out.boneWeights[i].w0 = weights[0] * t;
            out.boneWeights[i].w1 = weights[1] * t;
            out.boneWeights[i].w2 = weights[2] * t;
            out.boneWeights[i].w3 = weights[3] * t;
        } else {
            out.boneWeights[i].w0 = weights[0];
            out.boneWeights[i].w1 = weights[1];
            out.boneWeights[i].w2 = weights[2];
            out.boneWeights[i].w3 = weights[3];
        }
    }

    RpGeometryUnlock(out.g);
    out.g->refCount++;

    return out.g;
}

// 0x5A4940
RpGeometry* CClothesBuilder::BlendGeometry(RpClump* clump, const char* frameName0, const char* frameName1, const char* frameName2, float r0, float r1, float r2) {
    const char* names[] = { frameName0, frameName1, frameName2 };
    const float ratios[] = { r0, r1, r2 };
    return BlendGeometries<3>(clump, { names[0], names[1], names[2] }, { ratios[0], ratios[1], ratios[2] });
}

// 0x5A4F10
RpGeometry* CClothesBuilder::BlendGeometry(RpClump* clump, const char* frameName0, const char* frameName1, float r0, float r1) {
    const char* names[] = { frameName0, frameName1 };
    const float ratios[] = { r0, r1 };
    return BlendGeometries<2>(clump, { names[0], names[1] }, { ratios[0], ratios[1] });
}

// 0x5A5340
RpGeometry* CClothesBuilder::CopyGeometry(RpClump* clump, const char* frameName0, const char* frameName1) {
    const auto atomic0 = GetAtomicWithName(clump, frameName0);
    const auto atomic1 = GetAtomicWithName(clump, frameName1);

    RpGeometry* geos[2];
    const RwUInt8* boneIdxs[2];
    RwMatrixWeights* boneWeights[2];
    RwTexCoords* uvs[2];
    RwV3d* verts[2];
    RwV3d* normals[2];
    for (auto i = 0; i < 2; i++) {
        const auto g    = RpAtomicGetGeometry(i == 0 ? atomic0 : atomic1);
        const auto skin = RpSkinGeometryGetSkin(g);
        const auto mt   = RpGeometryGetMorphTarget(g, 0);
        geos[i]        = g;
        boneIdxs[i]    = (const RwUInt8*)RpSkinGetVertexBoneIndices(skin);
        boneWeights[i] = RpSkinGetVertexBoneWeights(skin);
        normals[i]     = RpMorphTargetGetVertexNormals(mt);
        verts[i]       = RpMorphTargetGetVertices(mt);
        uvs[i]         = RpGeometryGetVertexTexCoords(g, 1);
    }
    RpGeometryLock(geos[0], rpGEOMETRYLOCKALL);

    const auto numVerts = RpGeometryGetNumVertices(geos[0]);
    for (auto i = 0; i < numVerts; i++) {
        verts[0][i]   = verts[1][i];
        normals[0][i] = normals[1][i];
        uvs[0][i]     = uvs[1][i];
        for (auto b = 0; b < 4; b++) {
            const_cast<RwUInt8*>(boneIdxs[0])[i * 4 + b] = boneIdxs[1][i * 4 + b];
        }
        boneWeights[0][i] = boneWeights[1][i];
    }

    RpGeometryUnlock(geos[0]);

    // Re-gather pointers (geometry may have moved) and normalize normals
    for (auto i = 0; i < 2; i++) {
        const auto g    = RpAtomicGetGeometry(i == 0 ? atomic0 : atomic1);
        const auto skin = RpSkinGeometryGetSkin(g);
        const auto mt   = RpGeometryGetMorphTarget(g, 0);
        geos[i]        = g;
        boneIdxs[i]    = (const RwUInt8*)RpSkinGetVertexBoneIndices(skin);
        boneWeights[i] = RpSkinGetVertexBoneWeights(skin);
        normals[i]     = RpMorphTargetGetVertexNormals(mt);
        verts[i]       = RpMorphTargetGetVertices(mt);
        uvs[i]         = RpGeometryGetVertexTexCoords(g, 1);
    }
    for (auto i = 0; i < numVerts; i++) {
        RwV3dNormalize(&normals[0][i], &normals[0][i]);
    }
    geos[0]->refCount++;

    return geos[0];
}

// 0x5A55A0
void CClothesBuilder::ConstructGeometryArray(RpGeometry** out, uint32* modelNameKeys, float normal, float fatness, float strength) {
    for (auto i = 0; i < 10; i++, out++, modelNameKeys++) {
        if (!*modelNameKeys) {
            *out = nullptr;
            continue;
        }
        const auto modelId = MODEL_CLOTHES01_ID384 + i;
        CModelInfo::GetModelInfo(modelId)->bHasComplexHierarchy = true;
        RequestGeometry(modelId, *modelNameKeys);
        CStreaming::LoadAllRequestedModels(true);

        if (i + 1 < 10 && modelNameKeys[1]) { // Request next model to be loaded in advance
            RequestGeometry(modelId + 1, modelNameKeys[1]);
            CStreaming::LoadRequestedModels();
        }

        const auto mi = CModelInfo::GetModelInfo(modelId);
        *out = BlendGeometry(mi->GetRpClump(), "normal", "fat", "ripped", normal, fatness, strength);
        StoreBoneArray(mi->GetRpClump(), i);
        CStreaming::RemoveModel(modelId);
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
    for (const auto [tableIdx, boneId] : rngv::enumerate(gBoneIndices[index])) {
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
void CClothesBuilder::ConstructTextures(RwTexDictionary* dict, uint32* hashes, float rNormal, float rFat, float rMuscle) {
    // Blend the tattoo layers together, chaining through their TXDs
    RwTexture* tattoos = nullptr;
    int32 txdId = RequestTexture(hashes[CLOTHES_TEXTURE_LOWER_LEFT_ARM]);
    CStreaming::LoadRequestedModels();
    for (auto i = (int32)CLOTHES_TEXTURE_LOWER_LEFT_ARM; i <= (int32)CLOTHES_TEXTURE_UPPER_BACK; i++) {
        int32 nextTxdId;
        if (i < (int32)CLOTHES_TEXTURE_UPPER_BACK) {
            tattoos = GetTextureFromTxdAndLoadNextTxd(tattoos, txdId, hashes[i + 1], &nextTxdId);
        } else {
            tattoos = GetTextureFromTxdAndLoadNextTxd(tattoos, txdId, CKeyGen::GetUppercaseKey("player_torso"), &nextTxdId);
        }
        txdId = nextTxdId;
    }

    if (rFat < 0.0f) {
        rNormal += rFat;
        rFat = 0.0f;
    }
    CStreaming::LoadAllRequestedModels(true);
    // Torso
    const int32 torsoTxdReq = RequestTexture(hashes[CLOTHES_TEXTURE_TORSO]);
    CStreaming::LoadRequestedModels();
    const auto torsoTxdDict = CTxdStore::GetTxd(txdId);
    RwTexture* torso       = RwTexDictionaryFindNamedTexture(torsoTxdDict, "torso");
    RwTexture* torsoFat    = RwTexDictionaryFindNamedTexture(torsoTxdDict, "torso_fat");
    RwTexture* torsoRipped = RwTexDictionaryFindNamedTexture(torsoTxdDict, "torso_ripped");
    RwTexture* torsoBlended = CopyTexture(torso);
    if (!tattoos) {
        BlendTextures(torsoBlended, torsoFat, torsoRipped, rNormal, rFat, rMuscle, 108);
    } else {
        BlendTextures(torsoBlended, torsoFat, torsoRipped, rNormal, rFat, rMuscle, 108, tattoos);
        RwTextureDestroy(tattoos);
    }
    CStreaming::RemoveModel(TXDToModelId(txdId));
    int32 legsTxdId;
    GetTextureFromTxdAndLoadNextTxd(torsoBlended, torsoTxdReq, CKeyGen::GetUppercaseKey("player_legs"), &legsTxdId);
    RwTextureSetName(torsoBlended, "torso");
    RwTexDictionaryAddTexture(dict, torsoBlended);

    // Legs
    CStreaming::LoadAllRequestedModels(true);
    const int32 legsTxdReq = RequestTexture(hashes[CLOTHES_TEXTURE_LEGS]);
    CStreaming::LoadRequestedModels();
    const auto legsTxdDict = CTxdStore::GetTxd(legsTxdId);
    RwTexture* legs       = RwTexDictionaryFindNamedTexture(legsTxdDict, "legs");
    RwTexture* legsFat    = RwTexDictionaryFindNamedTexture(legsTxdDict, "legs_fat");
    RwTexture* legsRipped = RwTexDictionaryFindNamedTexture(legsTxdDict, "legs_ripped");
    RwTexture* legsBlended = CopyTexture(legs);
    BlendTextures(legsBlended, legsFat, legsRipped, rNormal, rFat, rMuscle, 108);
    CStreaming::RemoveModel(TXDToModelId(legsTxdId));
    int32 faceTxd;
    const uint32 faceHash = hashes[CLOTHES_TEXTURE_HEAD] ? hashes[CLOTHES_TEXTURE_HEAD] : CKeyGen::GetUppercaseKey("player_face");
    GetTextureFromTxdAndLoadNextTxd(legsBlended, legsTxdReq, faceHash, &faceTxd);
    RwTextureSetName(legsBlended, "legs");
    RwTexDictionaryAddTexture(dict, legsBlended);

    // Head
    CStreaming::LoadAllRequestedModels(true);
    int32 feetTxdReq;
    if (!hashes[CLOTHES_TEXTURE_SHOES]) {
        feetTxdReq = RequestTexture(CKeyGen::GetUppercaseKey("player_feet"));
    } else {
        feetTxdReq = RequestTexture(hashes[CLOTHES_TEXTURE_SHOES]);
    }
    CStreaming::LoadRequestedModels();
    const auto faceTxdDict = CTxdStore::GetTxd(faceTxd);
    RwTexture* face    = RwTexDictionaryFindNamedTexture(faceTxdDict, "face");
    RwTexture* faceFat = RwTexDictionaryFindNamedTexture(faceTxdDict, "face_fat");
    RwTexture* headBlended;
    if (!face) {
        headBlended = CopyTexture(GetFirstTexture(faceTxdDict));
    } else {
        headBlended = CopyTexture(face);
        if (faceFat) {
            const auto total = rNormal + rFat + rMuscle;
            BlendTextures(headBlended, faceFat, (rNormal + rMuscle) / total, rFat / total, 108);
        }
    }
    CStreaming::RemoveModel(TXDToModelId(faceTxd));
    RwTextureSetName(headBlended, "head");
    RwTexDictionaryAddTexture(dict, headBlended);

    // Feet + accessories, each chained to the next TXD
    int32 nextTxdA, nextTxdB;
    RwTexture* feet = GetTextureFromTxdAndLoadNextTxd(nullptr, feetTxdReq, hashes[CLOTHES_TEXTURE_NECKLACE], &nextTxdA);
    RwTextureSetName(feet, "feet");
    RwTexDictionaryAddTexture(dict, feet);

    if (RwTexture* necklace = GetTextureFromTxdAndLoadNextTxd(nullptr, nextTxdA, hashes[CLOTHES_TEXTURE_BRACELET], &nextTxdB)) {
        RwTextureSetName(necklace, "necklace");
        RwTexDictionaryAddTexture(dict, necklace);
    }
    if (RwTexture* watch = GetTextureFromTxdAndLoadNextTxd(nullptr, nextTxdB, hashes[CLOTHES_TEXTURE_GLASSES], &nextTxdA)) {
        RwTextureSetName(watch, "watch");
        RwTexDictionaryAddTexture(dict, watch);
    }
    if (RwTexture* glasses = GetTextureFromTxdAndLoadNextTxd(nullptr, nextTxdA, hashes[CLOTHES_TEXTURE_HATS], &nextTxdB)) {
        RwTextureSetName(glasses, "glasses");
        RwTexDictionaryAddTexture(dict, glasses);
    }
    if (RwTexture* hat = GetTextureFromTxdAndLoadNextTxd(nullptr, nextTxdB, hashes[CLOTHES_TEXTURE_SPECIAL], &nextTxdA)) {
        RwTextureSetName(hat, "hat");
        RwTexDictionaryAddTexture(dict, hat);
    }
    if (nextTxdA != -1) {
        CStreaming::LoadAllRequestedModels(true);
        RwTexture* extra = CopyTexture(GetFirstTexture(CTxdStore::GetTxd(nextTxdA)));
        CStreaming::RemoveModel(TXDToModelId(nextTxdA));
        if (extra) {
            RwTextureSetName(extra, "extra1");
            RwTexDictionaryAddTexture(dict, extra);
        }
    }
}

// 0x5A6530
void CClothesBuilder::ConstructGeometryAndSkinArrays(RpHAnimHierarchy* animHierarchy, RpGeometry** outGeometry, RwMatrixWeights** outWeights, RwUInt32** outBoneIndices, uint32 numModels, RpGeometry** geometries, RpMaterial** materials) {
    int32 numVerts = 0, numTris = 0;
    for (auto i = 0u; i < numModels; i++) {
        if (geometries[i]) {
            numVerts += RpGeometryGetNumVertices(geometries[i]);
            numTris  += RpGeometryGetNumTriangles(geometries[i]);
        }
    }

    const auto out = RpGeometryCreate(numVerts, numTris, rpGEOMETRYTRISTRIP | rpGEOMETRYTEXTURED | rpGEOMETRYNORMALS | rpGEOMETRYLIGHT);
    *outGeometry = out;
    const auto outMT = RpGeometryGetMorphTarget(out, 0);
    RwV3d* outVerts         = RpMorphTargetGetVertices(outMT);
    RwV3d* outNormals       = RpMorphTargetGetVertexNormals(outMT);
    RwTexCoords* outUVs     = RpGeometryGetVertexTexCoords(out, 1);
    RpTriangle* outTris     = RpGeometryGetTriangles(out);

    int32 vertBase = 0, triBase = 0;
    for (auto i = 0u; i < numModels; i++) {
        const auto src = geometries[i];
        if (!src) {
            continue;
        }
        const auto numSrcVerts = RpGeometryGetNumVertices(src);
        const auto numSrcTris  = RpGeometryGetNumTriangles(src);
        const auto srcMT = RpGeometryGetMorphTarget(src, 0);
        const RwV3d* srcVerts         = RpMorphTargetGetVertices(srcMT);
        const RwV3d* srcNormals       = RpMorphTargetGetVertexNormals(srcMT);
        const RwTexCoords* srcUVs     = RpGeometryGetVertexTexCoords(src, 1);
        const RpTriangle* srcTris     = RpGeometryGetTriangles(src);
        for (auto v = 0; v < numSrcVerts; v++) {
            outVerts[vertBase + v]   = srcVerts[v];
            outNormals[vertBase + v] = srcNormals[v];
            outUVs[vertBase + v]     = srcUVs[v];
        }
        for (auto t = 0; t < numSrcTris; t++) {
            RpTriangle* dstTri = &outTris[triBase + t];
            RpGeometryTriangleSetVertexIndices(out, dstTri,
                srcTris[t].vertIndex[0] + vertBase, srcTris[t].vertIndex[1] + vertBase, srcTris[t].vertIndex[2] + vertBase);
            RpGeometryTriangleSetMaterial(out, dstTri, materials[i]);
        }
        vertBase += numSrcVerts;
        triBase  += numSrcTris;
    }

    RpMorphTargetCalcBoundingSphere(outMT, &outMT->boundingSphere);
    RpGeometryUnlock(out);

    auto* weights  = new RwMatrixWeights[numVerts];
    auto* boneIdxs = new RwUInt32[numVerts];
    *outWeights     = weights;
    *outBoneIndices = boneIdxs;

    uint8 convTable[64];
    for (auto i = 0u; i < numModels; i++) {
        const auto src = geometries[i];
        if (!src) {
            continue;
        }
        const auto numSrcVerts = RpGeometryGetNumVertices(src);
        const auto skin = RpSkinGeometryGetSkin(src);
        const RwUInt8* srcBoneIdxs = (const RwUInt8*)RpSkinGetVertexBoneIndices(skin);
        const RwMatrixWeights* srcWeights = RpSkinGetVertexBoneWeights(skin);
        BuildBoneIndexConversionTable(convTable, animHierarchy, i);
        for (auto v = 0; v < numSrcVerts; v++) {
            for (auto b = 0; b < 4; b++) {
                ((RwUInt8*)boneIdxs)[v * 4 + b] = convTable[srcBoneIdxs[v * 4 + b]];
            }
            weights[v] = srcWeights[v];
        }
        weights  += numSrcVerts;
        boneIdxs += numSrcVerts;
    }
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
    RpGeometry* gs[NO_BODY_PARTS]{};
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
