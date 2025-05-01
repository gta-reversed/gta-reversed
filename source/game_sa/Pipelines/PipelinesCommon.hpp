#pragma once

#include <rw/rpworld.h>
#include <rw/rwplcore.h>
#include <rw/rwcore.h>

// Helper
inline void SetTextureStagesForLighting(bool isGeoTextured, RwTexture* tex) {
    if (isGeoTextured) {
        //assert(tex);
        RwD3D9SetTexture(tex, 0); 
        RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    } else {
        RwD3D9SetTexture(NULL, 0); 
        RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
        RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    }
}

// 0x5DA640 (Render::Obj)
inline void RxD3D9InstanceDataRender(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* mesh) {
    RwD3D9SetPixelShader(NULL);
    RwD3D9SetVertexShader(mesh->vertexShader);

    if (header->indexBuffer) {
        RwD3D9DrawIndexedPrimitive(header->primType, mesh->baseIndex, 0, mesh->numVertices, mesh->startIndex, mesh->numPrimitives);
    } else {
        RwD3D9DrawPrimitive(header->primType, mesh->baseIndex, mesh->numPrimitives);
    }
}

// 0x5DA6A0 (Render::Obj::Lighting)
inline void RxD3D9InstanceDataRenderLighting(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* mesh, uint32 rxGeoFlags, RwTexture* tex) {
    SetTextureStagesForLighting((rxGeoFlags & (rxGEOMETRY_TEXTURED2 | rxGEOMETRY_TEXTURED)) != 0, tex);
    RxD3D9InstanceDataRender(header, mesh);
}
