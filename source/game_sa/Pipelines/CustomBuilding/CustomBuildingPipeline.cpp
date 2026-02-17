#include "StdInc.h"

#include "CustomBuildingPipeline.h"
#include <CustomBuildingDNPipeline.h>
#include <CustomCarEnvMapPipeline.h>
#include <PipelinesCommon.hpp>

RxPipeline*& CCustomBuildingPipeline::ObjPipeline = *(RxPipeline**)0xC02C68;

constexpr auto PLUGIN_ID = 0x53F2009C;

void CCustomBuildingPipeline::InjectHooks() {
    RH_ScopedClass(CCustomBuildingPipeline);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(CreatePipe, 0x5D7D90);
    RH_ScopedInstall(DestroyPipe, 0x5D7380);
    RH_ScopedInstall(CustomPipeAtomicSetup, 0x5D7E50);
    RH_ScopedInstall(CreateCustomObjPipe, 0x5D7AA0);
    RH_ScopedInstall(CustomPipeMaterialSetup, 0x5D7DB0);
    RH_ScopedInstall(CustomPipeRenderCB, 0x5D77D0);
    RH_ScopedInstall(CustomPipeReinstanceCB, 0x5D77A0);
}

// 0x5D7D90
bool CCustomBuildingPipeline::CreatePipe() {
    ObjPipeline = CreateCustomObjPipe();
    return ObjPipeline != nullptr;
}

// 0x5D7380
void CCustomBuildingPipeline::DestroyPipe() {
    if (auto* const pipe = std::exchange(ObjPipeline, nullptr)) {
        RxPipelineDestroy(pipe);
    }
}

// 0x5D7E50
RpAtomic* CCustomBuildingPipeline::CustomPipeAtomicSetup(RpAtomic* atomic) {
    RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), CustomPipeMaterialSetup, nullptr);
    atomic->pipeline = ObjPipeline;
    SetPipelineID(atomic, PLUGIN_ID);
    return atomic;
}

// 0x5D7AA0
RxPipeline* CCustomBuildingPipeline::CreateCustomObjPipe() {
    auto* const pipe = RxPipelineCreate();
    if (!pipe) {
        return nullptr;
    }

    auto* const lockedPipe = RxPipelineLock(pipe);
    if (!lockedPipe || !RxLockedPipeAddFragment(lockedPipe, 0, RxNodeDefinitionGetD3D9AtomicAllInOne(), 0) || !RxLockedPipeUnlock(lockedPipe)) {
        RxPipelineDestroy(pipe);
        return nullptr;
    }

    auto* const node = RxPipelineFindNodeByName(pipe, RxNodeDefinitionGetD3D9AtomicAllInOne()->name, nullptr, nullptr);
    RxD3D9AllInOneSetInstanceCallBack(node, RxD3D9AllInOneGetInstanceCallBack(node));
    RxD3D9AllInOneSetReinstanceCallBack(node, CustomPipeReinstanceCB);
    RxD3D9AllInOneSetRenderCallBack(node, CustomPipeRenderCB);

    pipe->pluginId = PLUGIN_ID;
    pipe->pluginData = PLUGIN_ID;

    return pipe;
}

// 0x5D7DB0
RpMaterial* CCustomBuildingPipeline::CustomPipeMaterialSetup(RpMaterial* material, void* data) {
    auto** const envMapData = &CCustomCarEnvMapPipeline::EnvMapPlGetData(material);

    if (RpMatFXMaterialGetEffects(material) == rpMATFXEFFECTENVMAP) {
        if (auto* const data = CCustomBuildingDNPipeline::SetFxEnvTexture(envMapData)) {
            data->Texture = (MATFXD3D9ENVMAPGETDATA(material, 0))->texture; // this macro (afaik) isn't otherwise public, I have no clue how they managed to get this working

            if (data->Texture) {
                RwTextureSetAddressing(data->Texture, rwTEXTUREADDRESSWRAP);
                RwTextureSetFilterMode(data->Texture, rwFILTERLINEAR);
            }
        }
    }

    CCustomCarEnvMapPipeline::SetMaterialFlags(
        material,
        *envMapData && (*envMapData)->Shininess != 0.f && (*envMapData)->Texture
        ? CCustomCarEnvMapPipeline::MF_HAS_SHINE_CAM
        : CCustomCarEnvMapPipeline::MF_NONE
    );

    return material;
}

// 0x5D77D0
void CCustomBuildingPipeline::CustomPipeRenderCB(RwResEntry* resEntry, void* object, RwUInt8 type, RwUInt32 rxGeoFlags) {
    const auto atomic = (RpAtomic*)(object);

    _rwD3D9EnableClippingIfNeeded(atomic, type);

    DWORD isLightingEnabled = 0;
    RwD3D9GetRenderState(D3DRS_LIGHTING, &isLightingEnabled);

    const auto geoHasNoLighting = !isLightingEnabled && !(rxGeoFlags & rxGEOMETRY_PRELIT);
    if (geoHasNoLighting) {
        RwD3D9SetTexture(NULL, 0);
        RwD3D9SetRenderState(D3DRS_TEXTUREFACTOR, 0xFF000000);
        RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
        RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
        RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
    }

    auto* const header = (RxD3D9ResEntryHeader*)(resEntry + 1);
    auto* const meshes = (RxD3D9InstanceData*)(header + 1);

    if (const auto i = header->indexBuffer) {
        RwD3D9SetIndices(i);
    }

    _rwD3D9SetStreams(header->vertexStream, header->useOffsets);
    RwD3D9SetVertexDeclaration(header->vertexDeclaration);

    for (RwUInt32 i = 0; i < header->numMeshes; i++) { // 0x5D78A0
        auto* const mesh       = &meshes[i];
        auto* const mat        = mesh->material;
        const auto  matFlags   = CCustomCarEnvMapPipeline::GetMaterialFlags(mat);
        const auto  envMapData = CCustomCarEnvMapPipeline::EnvMapPlGetData(mat);

        // 0x5D78B8
        RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
        RwD3D9SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);

        // 0x5D78CD
        if (matFlags & CCustomCarEnvMapPipeline::MF_HAS_SHINE_CAM) {
            RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, RWRSTATE(rwTEXTUREADDRESSWRAP));

            RwD3D9SetTexture(envMapData->Texture, 1);
            RwD3D9SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB((byte)(std::min<float>(envMapData->Shininess, 1.f) * 255.f), 0xFF, 0xFF, 0xFF));

            RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MULTIPLYADD);
            RwD3D9SetTextureStageState(1, D3DTSS_COLORARG0, D3DTA_CURRENT);
            RwD3D9SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            RwD3D9SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TFACTOR);

            RwD3D9SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
            RwD3D9SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            RwD3D9SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

            RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1 | D3DTSS_TCI_CAMERASPACENORMAL);
            RwD3D9SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

            RwD3D9SetTextureStageState(2, D3DTSS_COLOROP, D3DTA_CURRENT);
        }

        // 0x5D79EE
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(mesh->vertexAlpha || mesh->material->color.alpha != 0xFF));

        if (geoHasNoLighting) { // 0x5D7A12 - Render without lighting
            RxD3D9InstanceDataRender(header, mesh);
        } else { // 0x5D7A1A - Render with lighting
            if (isLightingEnabled) {
                RwD3D9SetSurfaceProperties(&mesh->material->surfaceProps, &mesh->material->color, rxGeoFlags);
            }
            RxD3D9InstanceDataRenderLighting(header, mesh, rxGeoFlags, mesh->material->texture); // 0x5D7A41
        }
    }

    RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    RwD3D9SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
    RwD3D9SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, NULL);
}

// 0x5D77A0
RwBool CCustomBuildingPipeline::CustomPipeReinstanceCB(void* object, RwResEntry* resEntry, RxD3D9AllInOneInstanceCallBack instanceCallback) {
    auto* const header = (RxD3D9ResEntryHeader*)(resEntry + 1);
    return instanceCallback && instanceCallback(object, header, true);
}
