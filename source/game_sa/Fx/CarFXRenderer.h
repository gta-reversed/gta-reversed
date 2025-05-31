#pragma once

static inline auto& gSpecIntensity = StaticRef<float>(0x8D12D0); // 1.0f

class CCarFXRenderer {
public:
    static constexpr int32 NUM_DIRT_TEXTURES = 16;
    static RwTexture* (&ms_aDirtTextures)[NUM_DIRT_TEXTURES];

public:
    static void InjectHooks();

    static bool RegisterPlugins();

    static bool Initialise();
    static void InitialiseDirtTexture();
    static void Shutdown();
    static void PreRenderUpdate();

    static bool IsCCPCPipelineAttached(RpAtomic* atomic);

    static void CustomCarPipeAtomicSetup(RpAtomic* atomic);
    static void CustomCarPipeClumpSetup(RpClump* clump);

    static RpMaterial* MaterialRemapDirtCB(RpMaterial*, void*);
    static RpAtomic* AtomicRemapDirtCB(RpAtomic*, void*);
    static RpAtomic* SetCustomFXAtomicRenderPipelinesVMICB(RpAtomic* atomic, void* data);

    static float GetFxEnvMapLightMult();
    static void SetFxEnvMapLightMult(float multiplier);
};
