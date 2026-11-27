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
    //RH_ScopedInstall(nullptr, 0x5A42B0, { .Reversed = false });
    //RH_ScopedInstall(nullptr, 0x5A4380, { .Reversed = false }); AtomicInstanceCB
    //RH_ScopedInstall(nullptr, 0x5A43A0, { .Reversed = false });
    //RH_ScopedInstall(nullptr, 0x5A44A0, { .Reversed = false }); DestroyTextureCB
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
void CClothesBuilder::PreprocessClothesDesc(CPedClothesDesc& desc, bool isCutscenePlayer) {
    constexpr auto NO_TAG = UINT32_MAX;

    // Model keys of the open `IGNORE` / `EXCLUSIVE` blocks
    std::array<uint32, 8> tagKeys;
    tagKeys.fill(NO_TAG);
    std::array<bool, 8> tagIsIgnore{};

    // A rule is skipped if the model is the key of an `IGNORE` block, or isn't the key of an `EXCLUSIVE` block
    const auto IsRuleIgnored = [&](uint32 modelKey) {
        for (auto&& [key, isIgnore] : rngv::zip(tagKeys, tagIsIgnore)) {
            if (key != NO_TAG && (modelKey == key) == isIgnore) {
                return true;
            }
        }
        return false;
    };
    const auto OpenTag = [&](uint32 key, bool isIgnore) {
        if (const auto it = rng::find(tagKeys, NO_TAG); it != tagKeys.end()) {
            *it                                             = key;
            tagIsIgnore[rng::distance(tagKeys.begin(), it)] = isIgnore;
            return true;
        }
        return false;
    };
    const auto CloseTag = [&](uint32 key) {
        if (const auto it = rng::find(tagKeys, key); it != tagKeys.end()) {
            *it = NO_TAG;
        }
    };

    const auto& rules = CClothes::ms_clothesRules;
    for (uint32 r = 0; r < CClothes::ms_numRuleTags;) {
        const auto rule = static_cast<eClothRule>(rules[r++]);

        uint32 key{}, model{}, texture{};
        int32  rulePart = -1; // eClothesModelPart
        switch (rule) {
        case eClothRule::TAG_CUTS: // 0x5A45C1
            key   = rules[r++];
            model = rules[r++];
            break;
        case eClothRule::TAG_SETC: // 0x5A45DA
            key      = rules[r++];
            rulePart = static_cast<int32>(rules[r++]);
            model    = rules[r++];
            texture  = rules[r++];
            break;
        case eClothRule::TAG_TEX: // 0x5A4607
            key     = rules[r++];
            texture = rules[r++];
            break;
        case eClothRule::TAG_HIDE: // 0x5A4620
            key      = rules[r++];
            rulePart = static_cast<int32>(rules[r++]);
            break;
        case eClothRule::TAG_END_IGNORE:    // 0x5A453B
        case eClothRule::TAG_END_EXCLUSIVE: // 0x5A4597
            CloseTag(rules[r++]);
            continue;
        case eClothRule::TAG_IGNORE:    // 0x5A4513
        case eClothRule::TAG_EXCLUSIVE: // 0x5A456F
            // NOTE: The original only skips the key if a slot was free, otherwise the key is read as the next rule
            if (OpenTag(rules[r], rule == eClothRule::TAG_IGNORE)) {
                r++;
            }
            continue;
        default:
            continue;
        }

        for (auto i = 0; i < CLOTHES_MODEL_TOTAL; i++) {
            const auto part = static_cast<eClothesModelPart>(i);
            if (IsRuleIgnored(desc.m_anModelKeys[rulePart != -1 ? rulePart : i])) {
                continue;
            }
            if (desc.m_anModelKeys[part] != key) {
                continue;
            }
            switch (rule) {
            case eClothRule::TAG_CUTS: { // 0x5A468D
                if (isCutscenePlayer) {
                    desc.SetModel(model, part);
                }
                break;
            }
            case eClothRule::TAG_SETC: { // 0x5A46B1
                const auto setPart = static_cast<eClothesModelPart>(rulePart);
                if (setPart == CLOTHES_MODEL_HANDS) { // Hands have no texture
                    desc.SetModel(model, setPart);
                    break;
                }
                if (!texture) {
                    texture = desc.m_anTextureKeys[CClothes::GetDependentTexture(setPart)];
                }
                if (!model) {
                    model = desc.m_anModelKeys[setPart];
                }
                desc.SetTextureAndModel(texture, model, CClothes::GetDependentTexture(setPart));
                break;
            }
            case eClothRule::TAG_TEX: // 0x5A471D
                desc.SetTextureAndModel(texture, key, CClothes::GetDependentTexture(part));
                break;
            case eClothRule::TAG_HIDE: // 0x5A4743
                desc.SetModel(0u, static_cast<eClothesModelPart>(rulePart));
                break;
            default:
                NOTSA_UNREACHABLE();
            }
        }
    }
}

// unused
// 0x5A47B0
void CClothesBuilder::ReleaseGeometry(int32 numToRelease) {
    for (auto i = numToRelease; i-- != 0;) {
        CStreaming::SetModelIsDeletable(MODEL_CLOTHES01_ID384 + i);
    }
}

// 0x5A4810
RpAtomic* GetAtomicWithName(RpClump* clump, const char* name) {
    struct Context {
        notsa::ci_string_view name{};
        RpAtomic*             atomic{};
    } c{name};
    RpClumpForAllAtomics(clump, [](RpAtomic* a, void* data) -> RpAtomic* { // 0x5A47E0
        auto& ctx = *static_cast<Context*>(data);
        if (ctx.name == GetFrameNodeName(RpAtomicGetFrame(a))) {
            ctx.atomic = a;
            return nullptr; // Found it, stop
        }
        return a;
    },
                         &c);
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
            if (i + 1 < 8) {
                weights[i + 1] = 0.f; // Mark next as unused [Though this step in our case is not necessary as the whole weights array is already zero-inited]
            }
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

    auto& boneIds = gBoneIndices[idx];
    for (auto&& [i, node] : rngv::enumerate(std::span{ h->pNodeInfo, (size_t)h->numNodes })) {
        boneIds[i] = static_cast<int16>(node.nodeID);
    }
    if (h->numNodes < (RwInt32)boneIds.size()) {
        boneIds[h->numNodes] = -1; // NOTE: Only this one is set, the rest keeps the bone ids of the last model built in this slot
    }
}

//! Vertex data of a skinned geometry of a clothes model
struct ClothesGeometryData {
    RpGeometry*      Geometry;
    CVector*         Verts;
    CVector*         Normals;
    RwTexCoords*     UVs;
    RwUInt8*         BoneIdxs; //!< 4 per vertex
    RwMatrixWeights* BoneWeights;

    explicit ClothesGeometryData(RpClump* clump, const char* frameName) :
        Geometry{ RpAtomicGetGeometry(GetAtomicWithName(clump, frameName)) } {
        const auto skin = RpSkinGeometryGetSkin(Geometry);
        const auto mt   = RpGeometryGetMorphTarget(Geometry, 0);
        Verts           = reinterpret_cast<CVector*>(RpMorphTargetGetVertices(mt));
        Normals         = reinterpret_cast<CVector*>(RpMorphTargetGetVertexNormals(mt));
        UVs             = RpGeometryGetVertexTexCoords(Geometry, rwTEXTURECOORDINATEINDEX0);
        BoneIdxs        = reinterpret_cast<RwUInt8*>(const_cast<RwUInt32*>(RpSkinGetVertexBoneIndices(skin)));
        BoneWeights     = RpSkinGetVertexBoneWeights(skin);
    }
};

//! Store the blended bone weights of a vertex, normalized if more than 4 bones were used
static void StoreBlendedBoneWeights(ClothesGeometryData& out, int32 v, const float (&weights)[8], const uint8 (&boneIdxs)[8]) {
    for (auto b = 0; b < 4; b++) {
        out.BoneIdxs[v * 4 + b] = boneIdxs[b];
    }
    auto& w = out.BoneWeights[v];
    w       = { weights[0], weights[1], weights[2], weights[3] };
    if (weights[4] != 0.f) {
        const auto t = 1.f / (w.w3 + w.w2 + w.w1 + w.w0);
        w.w0 *= t;
        w.w1 *= t;
        w.w2 *= t;
        w.w3 *= t;
    }
}

//! Add the 4 bone weights of a vertex scaled by `ratio`
static void AddBoneWeights(const ClothesGeometryData& d, int32 v, float ratio, float (&weights)[8], uint8 (&boneIdxs)[8]) {
    const auto& w = d.BoneWeights[v];
    for (const auto [b, weight] : rngv::enumerate(std::array{ w.w0, w.w1, w.w2, w.w3 })) {
        CClothesBuilder::AddWeightToBoneVertex(weights, boneIdxs, ratio * weight, d.BoneIdxs[v * 4 + b]);
    }
}

// 0x5A4940
RpGeometry* CClothesBuilder::BlendGeometry(RpClump* clump, const char* frameName0, const char* frameName1, const char* frameName2, float r0, float r1, float r2) {
    ClothesGeometryData       d0{ clump, frameName0 };
    const ClothesGeometryData d1{ clump, frameName1 }, d2{ clump, frameName2 };

    // NOTE: The sums are in the original's order, the float results depend on it
    const auto Blend = [&](float a0, float a1, float a2) {
        return r1 * a1 + r2 * a2 + r0 * a0;
    };

    RpGeometryLock(d0.Geometry, rpGEOMETRYLOCKALL);
    for (auto v = 0; v < RpGeometryGetNumVertices(d0.Geometry); v++) {
        auto& pos = d0.Verts[v];
        pos       = CVector{
                  r1 * d1.Verts[v].x + r0 * pos.x + r2 * d2.Verts[v].x,
                  Blend(pos.y, d1.Verts[v].y, d2.Verts[v].y),
                  Blend(pos.z, d1.Verts[v].z, d2.Verts[v].z),
        };

        auto& nrm = d0.Normals[v];
        nrm       = CVector{
                  Blend(nrm.x, d1.Normals[v].x, d2.Normals[v].x),
                  Blend(nrm.y, d1.Normals[v].y, d2.Normals[v].y),
                  Blend(nrm.z, d1.Normals[v].z, d2.Normals[v].z),
        };
        RwV3dNormalize(&nrm, &nrm);

        auto& uv = d0.UVs[v];
        uv       = {
                  Blend(uv.u, d1.UVs[v].u, d2.UVs[v].u),
                  Blend(uv.v, d1.UVs[v].v, d2.UVs[v].v),
        };

        float weights[8]{};
        uint8 boneIdxs[8]{};
        AddBoneWeights(d0, v, r0, weights, boneIdxs);
        AddBoneWeights(d1, v, r1, weights, boneIdxs);
        AddBoneWeights(d2, v, r2, weights, boneIdxs);
        StoreBlendedBoneWeights(d0, v, weights, boneIdxs);
    }
    RpGeometryUnlock(d0.Geometry);

    d0.Geometry->refCount++; // RpGeometryAddRef (0x74CCB0)
    return d0.Geometry;
}

// unused
// 0x5A4F10
RpGeometry* CClothesBuilder::BlendGeometry(RpClump* clump, const char* frameName0, const char* frameName1, float r0, float r1) {
    ClothesGeometryData       d0{ clump, frameName0 };
    const ClothesGeometryData d1{ clump, frameName1 };

    RpGeometryLock(d0.Geometry, rpGEOMETRYLOCKALL);
    uint8 boneIdxs[8]{}; // NOTE: Not cleared per vertex (unused slots keep the last vertex's indices), the original doesn't even initialize it
    for (auto v = 0; v < RpGeometryGetNumVertices(d0.Geometry); v++) {
        const auto Blend = [&](float a0, float a1) {
            return r1 * a1 + r0 * a0;
        };

        auto& pos = d0.Verts[v];
        pos       = CVector{ Blend(pos.x, d1.Verts[v].x), Blend(pos.y, d1.Verts[v].y), Blend(pos.z, d1.Verts[v].z) };

        auto& nrm = d0.Normals[v];
        nrm       = CVector{ Blend(nrm.x, d1.Normals[v].x), Blend(nrm.y, d1.Normals[v].y), Blend(nrm.z, d1.Normals[v].z) };
        RwV3dNormalize(&nrm, &nrm);

        auto& uv = d0.UVs[v];
        uv       = { Blend(uv.u, d1.UVs[v].u), Blend(uv.v, d1.UVs[v].v) };

        float weights[8]{};
        AddBoneWeights(d0, v, r0, weights, boneIdxs);
        AddBoneWeights(d1, v, r1, weights, boneIdxs);
        StoreBlendedBoneWeights(d0, v, weights, boneIdxs);
    }
    RpGeometryUnlock(d0.Geometry);

    d0.Geometry->refCount++; // RpGeometryAddRef (0x74CCB0)
    return d0.Geometry;
}

// unused
// 0x5A5340
RpGeometry* CClothesBuilder::CopyGeometry(RpClump* clump, const char* frameNameDst, const char* frameNameSrc) {
    const ClothesGeometryData dst{ clump, frameNameDst }, src{ clump, frameNameSrc };

    RpGeometryLock(dst.Geometry, rpGEOMETRYLOCKALL);
    for (auto v = 0; v < RpGeometryGetNumVertices(dst.Geometry); v++) {
        dst.Verts[v]   = src.Verts[v];
        dst.Normals[v] = src.Normals[v];
        dst.UVs[v]     = src.UVs[v];
        rng::copy_n(&src.BoneIdxs[v * 4], 4, &dst.BoneIdxs[v * 4]);
        dst.BoneWeights[v] = src.BoneWeights[v];
    }
    RpGeometryUnlock(dst.Geometry);
    // NOTE: The original fetches the vertex data once more here and loops over the vertices doing nothing

    dst.Geometry->refCount++; // RpGeometryAddRef (0x74CCB0)
    return dst.Geometry;
}

// 0x5A55A0
void CClothesBuilder::ConstructGeometryArray(RpGeometry** outGeometries, uint32* modelKeys, float normal, float fatness, float strength) {
    for (auto i = 0; i < CLOTHES_MODEL_TOTAL; i++) {
        if (!modelKeys[i]) {
            outGeometries[i] = nullptr;
            continue;
        }

        const auto modelId = MODEL_CLOTHES01_ID384 + i;
        RequestGeometry(modelId, modelKeys[i]);
        CStreaming::LoadAllRequestedModels(true);

        if (i + 1 < CLOTHES_MODEL_TOTAL && modelKeys[i + 1]) { // Request the next one, so it's (partially) loaded by the next iteration
            RequestGeometry(modelId + 1, modelKeys[i + 1]);
            CStreaming::LoadRequestedModels();
        }

        const auto clump = CModelInfo::GetModelInfo(modelId)->GetRpClump();
        outGeometries[i] = BlendGeometry(clump, "normal", "fat", "ripped", normal, fatness, strength);
        StoreBoneArray(clump, i);
        CStreaming::RemoveModel(modelId);
    }
}

// inlined, see 0x5A6CE1
// 0x5A56C0
void CClothesBuilder::DestroySkinArrays(RwMatrixWeights* weights, RwUInt32* bones) {
    delete[] weights;
    delete[] bones;
}

// 0x5A56E0
void CClothesBuilder::BuildBoneIndexConversionTable(uint8* pTable, RpHAnimHierarchy* hier, int32 index) {
    for (const auto [tableIdx, boneId] : rngv::enumerate(gBoneIndices[index])) {
        if (boneId == -1) {
            break;
        }
        const auto idx   = static_cast<uint8>(RpHAnimIDGetIndex(hier, boneId)); // Not found (-1) -> 0
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
        (RwRasterGetFormat(srcRaster) & (rwRASTERFORMATPIXELFORMATMASK | rwRASTERFORMATPAL8 | rwRASTERFORMATPAL4)) | rwRASTERTYPETEXTURE
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

//! NOTE: The original truncates to an int and keeps the low byte (its 0-255 clamp runs on that byte, so it does nothing)
static RwUInt8 ToColorChannel(float value) {
    return static_cast<RwUInt8>(static_cast<int32>(value));
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

    for (auto i = 0; i < RwRasterGetWidth(srcRaster) * RwRasterGetHeight(srcRaster); i++, dstIt += 4, srcIt += 4) {
        for (auto c = 0; c < 3; c++) { // Alpha stays the same
            dstIt[c] = ToColorChannel((float)srcIt[c] * r2 + (float)dstIt[c] * r1);
        }
    }

    RwRasterUnlock(srcRaster);
    RwRasterUnlock(dstRaster);

    CTimer::Resume();
}

// 0x5A59C0
void CClothesBuilder::BlendTextures(RwTexture* dst, RwTexture* src1, RwTexture* src2, float r1, float r2, float r3, int32 numColors) {
    ZoneScoped;

    AssertTextureLayouts({ dst, src1, src2 });

    const auto dstRaster  = RwTextureGetRaster(dst);
    const auto src1Raster = RwTextureGetRaster(src1);
    const auto src2Raster = RwTextureGetRaster(src2);

    CTimer::Suspend();

    auto src1It = RwRasterLock(src1Raster, 0, rwRASTERLOCKREAD);
    auto src2It = RwRasterLock(src2Raster, 0, rwRASTERLOCKREAD);
    auto dstIt  = RwRasterLock(dstRaster, 0, rwRASTERLOCKREADWRITE);

    for (auto i = 0; i < RwRasterGetWidth(src1Raster) * RwRasterGetHeight(src1Raster); i++, dstIt += 4, src1It += 4, src2It += 4) {
        for (auto c = 0; c < 3; c++) { // Alpha stays the same
            dstIt[c] = ToColorChannel((float)src2It[c] * r3 + (float)src1It[c] * r2 + (float)dstIt[c] * r1);
        }
    }

    RwRasterUnlock(src1Raster);
    RwRasterUnlock(src2Raster);
    RwRasterUnlock(dstRaster);

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
    auto dstIt  = RwRasterLock(dstRaster, 0, rwRASTERLOCKREADWRITE);
    auto tatIt  = RwRasterLock(tatRaster, 0, rwRASTERLOCKREAD);

    for (auto i = 0; i < RwRasterGetWidth(src1Raster) * RwRasterGetHeight(src1Raster); i++, dstIt += 4, src1It += 4, src2It += 4, tatIt += 4) {
        const auto tatAlpha = (int32)tatIt[3];
        for (auto c = 0; c < 3; c++) { // Alpha stays the same
            const auto blended = ToColorChannel((float)src2It[c] * r3 + (float)src1It[c] * r2 + (float)dstIt[c] * r1);
            dstIt[c]           = ToColorChannel((float)(((int32)tatIt[c] - (int32)blended) * tatAlpha) * (1.f / 255.f) + (float)blended); // Lerp towards the tattoo by its alpha
        }
    }

    RwRasterUnlock(src1Raster);
    RwRasterUnlock(src2Raster);
    RwRasterUnlock(dstRaster);
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
void CClothesBuilder::ConstructTextures(RwTexDictionary* dict, uint32* hashes, float normal, float fatness, float strength) {
    constexpr auto NUM_COLORS = 108; // Unused

    // Tattoos, placed on top of each other (the last one also requests the base torso textures)
    int32      tattooTxd = RequestTexture(hashes[CLOTHES_TEXTURE_LOWER_LEFT_ARM]);
    int32      torsoBaseTxd{};
    RwTexture* tattoos{};
    CStreaming::LoadRequestedModels();
    for (auto i = +CLOTHES_TEXTURE_LOWER_LEFT_ARM; i <= CLOTHES_TEXTURE_UPPER_BACK; i++) {
        int32 nextTxd{};
        tattoos   = i < CLOTHES_TEXTURE_UPPER_BACK
            ? GetTextureFromTxdAndLoadNextTxd(tattoos, tattooTxd, hashes[i + 1], &nextTxd)
            : GetTextureFromTxdAndLoadNextTxd(tattoos, tattooTxd, CKeyGen::GetUppercaseKey("player_torso"), &torsoBaseTxd);
        tattooTxd = nextTxd;
    }

    // NOTE: Can't be negative (see CreateSkinnedClump), the original checks it anyways
    const auto normalR = fatness < 0.f ? normal + fatness : normal;
    const auto fatR    = fatness < 0.f ? 0.f : fatness;

    // 0x5A60CB - Torso: The blended base + the torso clothes texture on top
    CStreaming::LoadAllRequestedModels(true);
    const auto torsoTxd = RequestTexture(hashes[CLOTHES_TEXTURE_TORSO]);
    CStreaming::LoadRequestedModels();

    const auto torsoBase = CTxdStore::GetTxd(torsoBaseTxd);
    const auto torso     = CopyTexture(RwTexDictionaryFindNamedTexture(torsoBase, "torso"));
    if (tattoos) {
        BlendTextures(torso, RwTexDictionaryFindNamedTexture(torsoBase, "torso_fat"), RwTexDictionaryFindNamedTexture(torsoBase, "torso_ripped"), normalR, fatR, strength, NUM_COLORS, tattoos);
        RwTextureDestroy(tattoos);
    } else {
        BlendTextures(torso, RwTexDictionaryFindNamedTexture(torsoBase, "torso_fat"), RwTexDictionaryFindNamedTexture(torsoBase, "torso_ripped"), normalR, fatR, strength, NUM_COLORS);
    }
    CStreaming::RemoveModel(TXDToModelId(torsoBaseTxd));

    int32 legsBaseTxd{};
    GetTextureFromTxdAndLoadNextTxd(torso, torsoTxd, CKeyGen::GetUppercaseKey("player_legs"), &legsBaseTxd);
    RwTextureSetName(torso, "torso");
    RwTexDictionaryAddTexture(dict, torso);

    // 0x5A61CA - Legs
    CStreaming::LoadAllRequestedModels(true);
    const auto legsTxd = RequestTexture(hashes[CLOTHES_TEXTURE_LEGS]);
    CStreaming::LoadRequestedModels();

    const auto legsBase = CTxdStore::GetTxd(legsBaseTxd);
    const auto legs     = CopyTexture(RwTexDictionaryFindNamedTexture(legsBase, "legs"));
    BlendTextures(legs, RwTexDictionaryFindNamedTexture(legsBase, "legs_fat"), RwTexDictionaryFindNamedTexture(legsBase, "legs_ripped"), normalR, fatR, strength, NUM_COLORS);
    CStreaming::RemoveModel(TXDToModelId(legsBaseTxd));

    int32 headTxd{};
    GetTextureFromTxdAndLoadNextTxd(legs, legsTxd, hashes[CLOTHES_TEXTURE_HEAD] ? hashes[CLOTHES_TEXTURE_HEAD] : CKeyGen::GetUppercaseKey("player_face"), &headTxd);
    RwTextureSetName(legs, "legs");
    RwTexDictionaryAddTexture(dict, legs);

    // 0x5A62B9 - Head
    CStreaming::LoadAllRequestedModels(true);
    const auto feetTxd = RequestTexture(hashes[CLOTHES_TEXTURE_SHOES] ? hashes[CLOTHES_TEXTURE_SHOES] : CKeyGen::GetUppercaseKey("player_feet"));
    CStreaming::LoadRequestedModels();

    const auto headTxdDict = CTxdStore::GetTxd(headTxd);
    const auto face        = RwTexDictionaryFindNamedTexture(headTxdDict, "face");
    const auto faceFat     = RwTexDictionaryFindNamedTexture(headTxdDict, "face_fat");
    const auto head        = CopyTexture(face ? face : GetFirstTexture(headTxdDict));
    if (face && faceFat) {
        const auto sum = normalR + fatR + strength;
        BlendTextures(head, faceFat, (normalR + strength) / sum, fatR / sum, NUM_COLORS);
    }
    CStreaming::RemoveModel(TXDToModelId(headTxd));
    RwTextureSetName(head, "head");
    RwTexDictionaryAddTexture(dict, head);

    // 0x5A63B5 - Feet
    int32      nextTxd{};
    const auto feet = GetTextureFromTxdAndLoadNextTxd(nullptr, feetTxd, hashes[CLOTHES_TEXTURE_NECKLACE], &nextTxd);
    RwTextureSetName(feet, "feet");
    RwTexDictionaryAddTexture(dict, feet);

    // 0x5A63DF - Accessories
    constexpr struct {
        eClothesTexturePart Part;
        const char*         Name;
    } ACCESSORIES[]{
        { CLOTHES_TEXTURE_NECKLACE, "necklace" },
        { CLOTHES_TEXTURE_BRACELET, "watch"    },
        { CLOTHES_TEXTURE_GLASSES,  "glasses"  },
        { CLOTHES_TEXTURE_HATS,     "hat"      },
    };
    for (const auto& [part, name] : ACCESSORIES) {
        int32 txd{};
        if (const auto tex = GetTextureFromTxdAndLoadNextTxd(nullptr, nextTxd, hashes[part + 1], &txd)) {
            RwTextureSetName(tex, name);
            RwTexDictionaryAddTexture(dict, tex);
        }
        nextTxd = txd;
    }

    // 0x5A64B2 - Special
    if (nextTxd != -1) {
        CStreaming::LoadAllRequestedModels(true);
        const auto tex = CopyTexture(GetFirstTexture(CTxdStore::GetTxd(nextTxd)));
        CStreaming::RemoveModel(TXDToModelId(nextTxd));
        if (tex) {
            RwTextureSetName(tex, "extra1");
            RwTexDictionaryAddTexture(dict, tex);
        }
    }
}

// 0x5A6530
void CClothesBuilder::ConstructGeometryAndSkinArrays(RpHAnimHierarchy* hier, RpGeometry** outGeometry, RwMatrixWeights** outWeights, uint32** outBoneIdxs, uint32 numGeometries, RpGeometry** geometries, RpMaterial** materials) {
    const auto parts = std::span{ geometries, numGeometries };

    RwInt32 numVerts{}, numTris{};
    for (const auto g : parts) {
        if (g) {
            numVerts += RpGeometryGetNumVertices(g);
            numTris += RpGeometryGetNumTriangles(g);
        }
    }

    // 0x5A6581 - Merge the vertices and triangles
    const auto geo = *outGeometry = RpGeometryCreate(numVerts, numTris, rpGEOMETRYTRISTRIP | rpGEOMETRYTEXTURED | rpGEOMETRYNORMALS | rpGEOMETRYLIGHT);
    const auto mt                 = RpGeometryGetMorphTarget(geo, 0);
    {
        auto*    vertsIt   = RpMorphTargetGetVertices(mt);
        auto*    normalsIt = RpMorphTargetGetVertexNormals(mt);
        auto*    uvsIt     = RpGeometryGetVertexTexCoords(geo, rwTEXTURECOORDINATEINDEX0);
        auto*    trisIt    = RpGeometryGetTriangles(geo);
        RwUInt16 vertOffset{};
        for (auto&& [i, g] : rngv::enumerate(parts)) {
            if (!g) {
                continue;
            }
            const auto gmt = RpGeometryGetMorphTarget(g, 0);
            for (auto v = 0; v < RpGeometryGetNumVertices(g); v++) {
                *vertsIt++   = RpMorphTargetGetVertices(gmt)[v];
                *normalsIt++ = RpMorphTargetGetVertexNormals(gmt)[v];
                *uvsIt++     = RpGeometryGetVertexTexCoords(g, rwTEXTURECOORDINATEINDEX0)[v];
            }
            for (const auto& tri : std::span{ RpGeometryGetTriangles(g), (size_t)RpGeometryGetNumTriangles(g) }) {
                RpGeometryTriangleSetVertexIndices(geo, trisIt, tri.vertIndex[0] + vertOffset, tri.vertIndex[1] + vertOffset, tri.vertIndex[2] + vertOffset);
                RpGeometryTriangleSetMaterial(geo, trisIt, materials[i]);
                trisIt++;
            }
            vertOffset += RpGeometryGetNumVertices(g);
        }
    }

    RwSphere sphere;
    RpMorphTargetCalcBoundingSphere(mt, &sphere);
    RpMorphTargetSetBoundingSphere(mt, &sphere);
    RpGeometryUnlock(geo);

    // 0x5A6746 - Skin arrays, with the bone indices converted to the indices of `hier`
    auto* weightsIt = *outWeights = new RwMatrixWeights[numVerts];
    auto* boneIdxsIt              = reinterpret_cast<uint8*>(*outBoneIdxs = new uint32[numVerts]);
    uint8 boneIdxConv[64]{}; // NOTE: Not cleared per model (and not initialized at all in the original), only the entries of the model's bones are used
    for (auto&& [i, g] : rngv::enumerate(parts)) {
        if (!g) {
            continue;
        }
        const auto skin = RpSkinGeometryGetSkin(g);
        BuildBoneIndexConversionTable(boneIdxConv, hier, (int32)i);
        for (auto v = 0; v < RpGeometryGetNumVertices(g); v++) {
            const auto* const boneIdxs = reinterpret_cast<const uint8*>(&RpSkinGetVertexBoneIndices(skin)[v]);
            for (auto b = 0; b < 4; b++) {
                *boneIdxsIt++ = boneIdxConv[boneIdxs[b]];
            }
            *weightsIt++ = RpSkinGetVertexBoneWeights(skin)[v];
        }
    }
}

// 0x5A69D0
RpClump* CClothesBuilder::CreateSkinnedClump(RpClump* bones, RwTexDictionary* dict, CPedClothesDesc& ndscr, const CPedClothesDesc* odscr, bool isCutscenePlayer) {
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

    if (odscr) { // 0x5A6B84
        ms_geometryHasChanged = false;
        ms_textureHasChanged  = false;
        ms_ratiosHaveChanged  = odscr->m_fFatStat != ndscr.m_fFatStat || odscr->m_fMuscleStat != ndscr.m_fMuscleStat;
        ms_geometryHasChanged = ms_ratiosHaveChanged || !rng::equal(ndscr.m_anModelKeys, odscr->m_anModelKeys);
        ms_textureHasChanged  = !rng::equal(ndscr.m_anTextureKeys, odscr->m_anTextureKeys);
        if (!ms_ratiosHaveChanged && !ms_geometryHasChanged && !ms_textureHasChanged) {
            return nullptr;
        }
    } else {
        ms_ratiosHaveChanged = ms_geometryHasChanged = ms_textureHasChanged = true;
    }
    CPedClothesDesc dscr = ndscr;
    PreprocessClothesDesc(dscr, isCutscenePlayer);

    //> 0x5A42B0 - Calculate blend ratios
    float rNormal, rFatness, rMuscle;
    {
        rMuscle  = std::min(std::max(CStats::GetStatValue(STAT_MUSCLE), 0.f) * (1.f / 1000.f), 1.f);
        rFatness = std::min(std::max(dscr.m_fFatStat - 200.f, 0.f) * (1.f / 800.f), 1.f);
        rNormal  = 1.f - (rFatness + rMuscle);
        if (const auto sum = rFatness + rMuscle; sum > 1.f) {
            const auto t = 1.f / sum;
            rFatness *= t;
            rMuscle *= t;
            rNormal   = 0.f;
        }
    }

    if ((ms_textureHasChanged || ms_ratiosHaveChanged) && !isCutscenePlayer) {
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
        RpGeometrySetFlags(tmpGeo, RpGeometryGetFlags(tmpGeo) | rpGEOMETRYPOSITIONS); // NOTE: The original sets it right after the destroy below (the atomic keeps the geometry alive)
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
