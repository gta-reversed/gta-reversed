#include "StdInc.h"

#include "CarFXRenderer.h"
#include "CustomCarEnvMapPipeline.h"
#include "ClothesBuilder.h"

void CCarFXRenderer::InjectHooks() {
    RH_ScopedClass(CCarFXRenderer);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(RegisterPlugins, 0x5D5B00);
    RH_ScopedInstall(Initialise, 0x5D5AC0);
    RH_ScopedInstall(InitialiseDirtTexture, 0x5D5BC0);
    RH_ScopedInstall(Shutdown, 0x5D5AD0);
    RH_ScopedInstall(PreRenderUpdate, 0x5D5B10);
    RH_ScopedInstall(IsCCPCPipelineAttached, 0x5D5B80);
    RH_ScopedInstall(CustomCarPipeAtomicSetup, 0x5D5B20);
    RH_ScopedInstall(CustomCarPipeClumpSetup, 0x5D5B40);
    RH_ScopedInstall(SetCustomFXAtomicRenderPipelinesVMICB, 0x5D5B60);
    RH_ScopedInstall(SetFxEnvMapLightMult, 0x5D5BA0);
}

// 0x5D5B00
bool CCarFXRenderer::RegisterPlugins() {
    return CCustomCarEnvMapPipeline::RegisterPlugin() != 0;
}

// 0x5D5AC0
bool CCarFXRenderer::Initialise() {
    return CCustomCarEnvMapPipeline::CreatePipe() != 0;
}

// 0x5D5BC0
void CCarFXRenderer::InitialiseDirtTexture() {
    CTxdStore::ScopedTXDSlot txd{ "vehicle" };

    auto* const grunge = RwTextureRead("vehiclegrunge256", nullptr);
    RwTextureSetFilterMode(grunge, rwFILTERLINEAR);

    const auto* const grungeRaster = RwTextureGetRaster(grunge);
    const auto        numPixels    = (size_t)(RwRasterGetWidth(grungeRaster) * RwRasterGetHeight(grungeRaster));

    // Make copies of it with increasing levels of dirt: The first one is (almost) white, the last one is (almost) the original texture
    for (auto&& [level, dirtTex] : rngv::enumerate(ms_aDirtTextures)) {
        dirtTex = CClothesBuilder::CopyTexture(grunge);
        RwTextureSetName(dirtTex, "vehiclegrunge256");

        auto* const raster    = RwTextureGetRaster(dirtTex);
        auto* const pixels    = RwRasterLock(raster, 0, rwRASTERLOCKWRITE | rwRASTERLOCKREAD);
        const auto  whiteness = (int32)(NUM_DIRT_TEXTURES - level) * 255 / NUM_DIRT_TEXTURES;
        for (auto&& pixel : std::span{ pixels, numPixels * 4 } | rngv::chunk(4)) {
            for (auto& channel : pixel | rngv::take(3)) { // Leave alpha alone
                channel = (uint8)(channel * (int32)level / NUM_DIRT_TEXTURES + whiteness);
            }
        }
        RwRasterUnlock(raster);
    }
}

// 0x5D5AD0
void CCarFXRenderer::Shutdown() {
    for (auto& texture : ms_aDirtTextures) {
        RwTextureDestroy(texture);
    }
    CCustomCarEnvMapPipeline::DestroyPipe();
}

// 0x5D5B10
void CCarFXRenderer::PreRenderUpdate() {
    CCustomCarEnvMapPipeline::PreRenderUpdate();
}

// 0x5D5B80
bool CCarFXRenderer::IsCCPCPipelineAttached(RpAtomic* atomic) {
    return GetPipelineID(atomic) == 0x53F2009A;
}

// 0x5D5B20
void CCarFXRenderer::CustomCarPipeAtomicSetup(RpAtomic* atomic) {
    CCustomCarEnvMapPipeline::CustomPipeAtomicSetup(atomic);
}

// 0x5D5B40
void CCarFXRenderer::CustomCarPipeClumpSetup(RpClump* clump) {
    RpClumpForAllAtomics(clump, SetCustomFXAtomicRenderPipelinesVMICB, nullptr);
}

// 0x5D5B60
RpAtomic* CCarFXRenderer::SetCustomFXAtomicRenderPipelinesVMICB(RpAtomic* atomic, void* data) {
    CCustomCarEnvMapPipeline::CustomPipeAtomicSetup(atomic);
    return atomic;
}

// 0x5D5BA0
void CCarFXRenderer::SetFxEnvMapLightMult(float multiplier) {
    gSpecIntensity = multiplier;
}
