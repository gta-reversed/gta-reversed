#include "StdInc.h"

#include <Pipelines/PipelinesCommon.hpp>
#include "CustomCarEnvMapPipeline.h"
#include "app_light.h"
#include "CarFXRenderer.h"

enum MaterialFlags {
    MF_HAS_SHINE_CAM   = 1 << 0,
    MF_HAS_SHINE_WAVE  = 1 << 1,
    MF_HAS_SPECULARITY = 1 << 2,
};

void CCustomCarEnvMapPipeline::InjectHooks() {
    RH_ScopedClass(CCustomCarEnvMapPipeline);
    RH_ScopedCategory("Pipelines");

    RH_ScopedInstall(RegisterPlugin, 0x5DA450);
    RH_ScopedInstall(CreatePipe, 0x5DA020);
    RH_ScopedInstall(DestroyPipe, 0x5DA130);
    RH_ScopedInstall(PreRenderUpdate, 0x5D8870);
    RH_ScopedInstall(CustomPipeMaterialSetup, 0x5DA560);
    RH_ScopedInstall(CustomPipeAtomicSetup, 0x5DA610);
    RH_ScopedInstall(CreateCustomObjPipe, 0x5D9F80);
    RH_ScopedInstall(CustomPipeInstanceCB, 0x5D8490);
    RH_ScopedInstall(CustomPipeRenderCB, 0x5D9900);

    RH_ScopedInstall(pluginEnvAtmConstructorCB, 0x5D8D30);
    RH_ScopedInstall(pluginEnvAtmDestructorCB, 0x5D9730);
    RH_ScopedInstall(pluginEnvAtmCopyConstructorCB, 0x5D9780);
    RH_ScopedInstall(AllocEnvMapPipeAtomicData, 0x5D96F0);

    RH_ScopedInstall(pluginEnvMatConstructorCB, 0x5D8BD0);
    RH_ScopedInstall(pluginEnvMatDestructorCB, 0x5D95B0);
    RH_ScopedInstall(pluginEnvMatCopyConstructorCB, 0x5D9600);
    RH_ScopedInstall(pluginEnvMatStreamReadCB, 0x5D9660);
    RH_ScopedInstall(pluginEnvMatStreamWriteCB, 0x5D8CD0);
    RH_ScopedInstall(pluginEnvMatStreamGetSizeCB, 0x5D8D10);
    RH_ScopedInstall(GetFxEnvScaleX, 0x5D6F90);
    RH_ScopedInstall(GetFxEnvScaleY, 0x5D6FC0);
    RH_ScopedInstall(SetFxEnvScale, 0x5D6F40);
    RH_ScopedInstall(GetFxEnvTransSclX, 0x5D7040);
    RH_ScopedInstall(GetFxEnvTransSclY, 0x5D7070);
    RH_ScopedInstall(SetFxEnvTransScl, 0x5D6FF0);
    RH_ScopedInstall(GetFxEnvShininess, 0x5D8AD0);
    RH_ScopedInstall(SetFxEnvShininess, 0x5D70A0);
    RH_ScopedInstall(SetFxEnvTexture, 0x5DA230);
    RH_ScopedInstall(SetCustomEnvMapPipeMaterialDataDefaults, 0x5D8BB0);

    RH_ScopedInstall(pluginSpecMatConstructorCB, 0x5D8D40);
    RH_ScopedInstall(pluginSpecMatDestructorCB, 0x5D97D0);
    RH_ScopedInstall(pluginSpecMatCopyConstructorCB, 0x5D9830);
    RH_ScopedInstall(pluginSpecMatStreamReadCB, 0x5D9880);
    RH_ScopedInstall(pluginSpecMatStreamWriteCB, 0x5D8D60);
    RH_ScopedInstall(pluginSpecMatStreamGetSizeCB, 0x5D8DD0);
    RH_ScopedInstall(GetFxSpecSpecularity, 0x5D8B90);
    RH_ScopedInstall(GetFxSpecTexture, 0x5D8B50);
}

// 0x5DA450
bool CCustomCarEnvMapPipeline::RegisterPlugin() {
    ms_envMapPluginOffset = RpMaterialRegisterPlugin(4, rwID_ENVMAPPLUGIN, pluginEnvMatConstructorCB, pluginEnvMatDestructorCB, pluginEnvMatCopyConstructorCB);
    if (ms_envMapPluginOffset < 0) {
        DEV_LOG("Failed to register Env Map Plugin");
        return false;
    }
    if (RpMaterialRegisterPluginStream(rwID_ENVMAPPLUGIN, pluginEnvMatStreamReadCB, pluginEnvMatStreamWriteCB, pluginEnvMatStreamGetSizeCB) < 0) {
        ms_envMapPluginOffset = -1;
        DEV_LOG("Failed to register Env Map Plugin");
        return false;
    }

    SetCustomEnvMapPipeMaterialDataDefaults(&fakeEnvMapPipeMatData);

    ms_envMapAtmPluginOffset = RpAtomicRegisterPlugin(4, rwID_ENVMAPATMPLUGIN, pluginEnvAtmConstructorCB, pluginEnvAtmDestructorCB, pluginEnvAtmCopyConstructorCB);
    if (ms_envMapAtmPluginOffset < 0) {
        DEV_LOG("Failed to register Env Map Atm Plugin");
        return false;
    }

    ms_specularMapPluginOffset = RpMaterialRegisterPlugin(4, rwID_SPECMAPATMPLUGIN, pluginSpecMatConstructorCB, pluginSpecMatDestructorCB, pluginSpecMatCopyConstructorCB);
    if (ms_specularMapPluginOffset < 0) {
        DEV_LOG("Failed to register Specular Map Plugin");
        return false;
    }
    if (RpMaterialRegisterPluginStream(rwID_SPECMAPATMPLUGIN, pluginSpecMatStreamReadCB, pluginSpecMatStreamWriteCB, pluginSpecMatStreamGetSizeCB) < 0) {
        ms_specularMapPluginOffset = -1;
        DEV_LOG("Failed to register Specular Map Plugin");
        return false;
    }

    return true;
}

// 0x5D8980
bool IsEnvironmentMappingSupported() {
    const auto caps = (D3DCAPS9*)RwD3D9GetCaps();
    return caps->MaxTextureBlendStages > 1 &&
        caps->MaxSimultaneousTextures >= 2 &&
        (caps->TextureOpCaps & D3DTEXOPCAPS_BLENDFACTORALPHA) &&
        (caps->TextureOpCaps & D3DTEXOPCAPS_MULTIPLYADD) &&
        (caps->ShadeCaps & D3DPSHADECAPS_SPECULARGOURAUDRGB);
}

// 0x5DA020
bool CCustomCarEnvMapPipeline::CreatePipe() {
    ObjPipeline = CreateCustomObjPipe();
    if (!ObjPipeline || !IsEnvironmentMappingSupported()) {
        return false;
    }
    std::memset(&g_GameLight, 0, sizeof(g_GameLight));
    m_gEnvMapPipeMatDataPool = new CustomEnvMapPipeMaterialDataPool(4096, "CustomEnvMapPipeMatDataPool");
    m_gEnvMapPipeAtmDataPool = new CustomEnvMapPipeAtomicDataPool(1024, "CustomEnvMapPipeAtmDataPool");
    m_gSpecMapPipeMatDataPool = new CustomSpecMapPipeMaterialDataPool(4096, "CustomSpecMapPipeMaterialDataPool");
    return true;
}

// 0x5DA130
void CCustomCarEnvMapPipeline::DestroyPipe() {
    delete std::exchange(m_gEnvMapPipeMatDataPool, nullptr);
    delete std::exchange(m_gEnvMapPipeAtmDataPool, nullptr);
    delete std::exchange(m_gSpecMapPipeMatDataPool, nullptr);

    if (auto* const pipe = std::exchange(ObjPipeline, nullptr)) {
        RxPipelineDestroy(pipe);
    }
}

// 0x5D8870
void CCustomCarEnvMapPipeline::PreRenderUpdate() {
    std::memset(&g_GameLight, 0, sizeof(g_GameLight));
    const auto dir = CVector{ *RwMatrixGetAt(RwFrameGetMatrix(RpLightGetFrame(pDirect))) }.Normalized();
    g_GameLight.Direction    = D3DVECTOR{ dir.x, dir.y, dir.z };
    g_GameLight.Type         = D3DLIGHT_DIRECTIONAL;
    g_GameLight.Diffuse      = { 0.25f, 0.25f, 0.25f, 1.0f };
    g_GameLight.Ambient      = { 0.75f, 0.75f, 0.75f, 1.0f };
    g_GameLight.Specular     = { 0.65f, 0.65f, 0.65f, 1.0f };
    g_GameLight.Range        = 1000.0f;
    g_GameLight.Falloff      = 0.0f;
    g_GameLight.Attenuation0 = 1.0f;
    g_GameLight.Attenuation1 = 0.0f;
    g_GameLight.Attenuation2 = 0.0f;
}

// 0x5DA560
RpMaterial* CCustomCarEnvMapPipeline::CustomPipeMaterialSetup(RpMaterial* material, void* data) {
    uint32 flags{};
    if (RpMatFXMaterialGetEffects(material) == rpMATFXEFFECTENVMAP) {
        SetFxEnvTexture(material, nullptr);
    }
    if (GetFxEnvShininess(material) != 0.0f) {
        if (auto* const tex = GetFxEnvTexture(material)) {
            flags |= RwTextureGetName(tex)[0] == 'x'
                ? MF_HAS_SHINE_WAVE
                : MF_HAS_SHINE_CAM;
        }
    }
    if (GetFxSpecSpecularity(material) != 0.0f && GetFxSpecTexture(material)) {
        flags |= MF_HAS_SPECULARITY;
    }
    material->surfaceProps.specular = std::bit_cast<float>(flags); // According to the docs, `specular` isn't used, so I guess that explains it?
    return material;
}

// 0x5DA610
RpAtomic* CCustomCarEnvMapPipeline::CustomPipeAtomicSetup(RpAtomic* atomic) {
    RpGeometryForAllMaterials(atomic->geometry, CustomPipeMaterialSetup, nullptr);
    atomic->pipeline = ObjPipeline;
    SetPipelineID(atomic, CUSTOM_CAR_ENV_MAP_PIPELINE_PLUGIN_ID);
    return atomic;
}

// Android - CreateCustomOpenGLObjPipe
// 0x5D9F80
RxPipeline* CCustomCarEnvMapPipeline::CreateCustomObjPipe() {
    auto* const pipe = RxPipelineCreate();
    if (!pipe) {
        return nullptr;
    }

    auto* const lpipe = RxPipelineLock(pipe);
    if (lpipe) {
        auto* const nodeDef = RxNodeDefinitionGetD3D9AtomicAllInOne();
        RxLockedPipeAddFragment(lpipe, nullptr, nodeDef);
        if (RxLockedPipeUnlock(lpipe)) {
            auto* const node = RxPipelineFindNodeByName(pipe, nodeDef->name, nullptr, nullptr);
            RxD3D9AllInOneSetInstanceCallBack(node, RxD3D9AllInOneGetInstanceCallBack(node));
            RxD3D9AllInOneSetReinstanceCallBack(node, CustomPipeInstanceCB);
            RxD3D9AllInOneSetRenderCallBack(node, CustomPipeRenderCB);
            pipe->pluginId = CUSTOM_CAR_ENV_MAP_PIPELINE_PLUGIN_ID;
            pipe->pluginData = CUSTOM_CAR_ENV_MAP_PIPELINE_PLUGIN_ID;
            return pipe;
        }
    }

    RxPipelineDestroy(pipe);

    return nullptr;
}

// todo: check signature
// 0x5D8490
RwBool CCustomCarEnvMapPipeline::CustomPipeInstanceCB(void* object, RwResEntry* resEntry, RxD3D9AllInOneInstanceCallBack instanceCallback) {
    if (instanceCallback) {
        auto entry = *reinterpret_cast<RwResEntrySA*>(resEntry);
        return instanceCallback(object, &entry.header, true) != 0;
    }
    return true;
}

// 0x5D9900
void CCustomCarEnvMapPipeline::CustomPipeRenderCB(RwResEntry* resEntry, void* object, RwUInt8 type, RwUInt32 rxGeoFlags) {
    assert(type == rpATOMIC);

    const auto specIntensity = gSpecIntensity * 1.85f;

    const auto atomic = (RpAtomic*)(object);
    const auto geo = RpAtomicGetGeometry(atomic);

    _rwD3D9EnableClippingIfNeeded(atomic, type);

    const auto atmFlags = CVisibilityPlugins::GetAtomicId(atomic);
    const auto noReflections = (atmFlags & (ATOMIC_IS_BLOWN_UP | ATOMIC_DISABLE_REFLECTIONS)) != 0;

    // Fixed blown up car rendering ("DarkVehiclesFix")
    // Credit: SilentPatch
    const auto isBlownUp = notsa::IsFixBugs() && (atmFlags & ATOMIC_IS_BLOWN_UP) && !(RpLightGetFlags(pDirect) & rpLIGHTLIGHTATOMICS);

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

    auto* const clump = RpAtomicGetClump(atomic);
    auto* const frame = clump
        ? RpClumpGetFrame(clump)
        : RpAtomicGetFrame(atomic);
    const auto* const ltm = RwFrameGetLTM(frame);
    const auto* const pos = RwMatrixGetPos(ltm);

    for (RwUInt32 i = 0; i < header->numMeshes; i++) { // 0x5D99E9
        auto* const mesh  = &meshes[i];
        auto* const mat   = mesh->material;
        const auto  matFlags = std::bit_cast<RwUInt32>(mat->surfaceProps.specular); // I guess they reuse this for storing the flags?

        // Calculate spec and power & set render states
        const auto useSpecular = isLightingEnabled && (matFlags & 4) && (g_fx.GetFxQuality() >= FX_QUALITY_HIGH || !noReflections); // Specular lighting enabled?
        float spec, power;
        if (useSpecular && !isBlownUp) { // 0x5D9A7F - 0x5D9B25 (Inverted if)
            const auto specularity = GetFxSpecSpecularity(mat);
            spec  = std::min(1.f, specIntensity * specularity * 2.f);
            power = specularity * 100.f;

            // 0x5D9A89
            RwD3D9SetLight(1, &g_GameLight);
            RwD3D9EnableLight(1, TRUE);

            // 0x5D9ACB
            RwD3D9SetRenderState(D3DRS_LOCALVIEWER, FALSE);
            RwD3D9SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
        } else {  // 0x5D9A4E - 0x5D9A74
            spec  = 0.f;
            power = 0.f;
        }

        // 0x5D9B0D + 0x5D9AC5
        RwD3D9SetRenderState(D3DRS_SPECULARENABLE, useSpecular);


        // 0x5D9B2B
        RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
        RwD3D9SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);

        // Can't have both at the same time
        assert(!((matFlags & MF_HAS_SHINE_CAM) && (matFlags & MF_HAS_SHINE_WAVE)));

        // 0x5D9B54 (EnvCam)
        if ((matFlags & MF_HAS_SHINE_CAM) && !noReflections && !isBlownUp) {
            RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, RWRSTATE(D3DTADDRESS_WRAP));

            // Set transform
            {
                // The formula `-fmod(p, s) / s` is creating normalized UV coordinates in the [0,1]
                // range that will properly wrap/tile the texture based on the object's
                // world position and the tiling scale (s).
                const auto CalculatePos = [](float p, float s) { // @ 0x5D84C0
                    return std::fmod(p, s) / s; // (p - floor(p / s) * s) / s
                };

                // Tex coords u, v
                const auto u = -CalculatePos(pos->x, GetFxEnvTransSclX(mat) * 50.f),
                           v = -CalculatePos(pos->y, GetFxEnvTransSclY(mat) * 50.f);

                // Scale x, y
                const auto sx = GetFxEnvScaleX(mat),
                           sy = GetFxEnvScaleY(mat);

                // Now set the texture transform matrix
                const D3DMATRIX transform{.m = {
                    { sx,  0.f, 0.f, 0.f },
                    { 0.f, sy,  0.f, 0.f },
                    { u,   v,   1.f, 0.f },
                    { 0.f, 0.f, 0.f, 1.f },
                }};
                RwD3D9SetTransform(D3DTS_TEXTURE1, &transform);
            }

            // 0x5D9BE0
            RwD3D9SetTexture(GetFxEnvTexture(mat), 1);

            // 0x5D9C04
            const auto c = std::min(0xFFu, (uint32)(GetFxEnvShininess(mat) * specIntensity * 254.f));
            RwD3D9SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(0xFF, c, c, c));

            // 0x5D9C38
            RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MULTIPLYADD);
            RwD3D9SetTextureStageState(1, D3DTSS_COLORARG0, D3DTA_CURRENT);
            RwD3D9SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            RwD3D9SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TFACTOR);
            RwD3D9SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
            RwD3D9SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            RwD3D9SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
            RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL | 1);
            RwD3D9SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_PROJECTED | D3DTTFF_COUNT3);

            RwD3D9SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
        }

        // 0x5D9CB2 (EnvWave - The wavy texture on the sides of vehicles)
        if ((matFlags & MF_HAS_SHINE_WAVE) && !noReflections && (geo->flags & rpGEOMETRYTEXTURED2) && !isBlownUp) {
            RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, RWRSTATE(D3DTADDRESS_WRAP));

            auto* const envMapAtmData = AllocEnvMapPipeAtomicData(atomic);
            assert(envMapAtmData);

            // Calculate and set texture UV transform
            {
                // Inline begin (@ 0x5D8590)
                auto* const env_map_atm = EnvMapAtmPlGetData(atomic);
                auto* const env_map_mat = EnvMapPlGetData(mat);

                const auto tsx = GetFxEnvTransSclX(mat) * 50.f,
                           tsy = GetFxEnvTransSclY(mat) * 50.f;

                if (env_map_mat->RenderFrame != RWSRCGLOBAL(renderFrame)) { // 0x5D85F5
                    env_map_mat->RenderFrame = RWSRCGLOBAL(renderFrame);

                    const auto CalcPos = [](float p, float s) {
                        const auto n = (int32)(p / s); // floor(p / s)
                        const auto t = std::abs((p - (float)(n) * s) / s); // = std::abs(std::modf(p, s) / s) = std::abs((p - floor(p / s) * s) / s)
                        return n % 2 == 0
                            ? 1.f - t
                            : t;
                    };

                    // 0x5D8639
                    const CVector2D currUV{
                        CalcPos(pos->x, tsx),
                        CalcPos(pos->y, tsy)
                    };

                    // 0x5D86B9
                    const CVector2D prevUV{
                        CalcPos(env_map_atm->PrevPosX, tsx),
                        CalcPos(env_map_atm->PrevPosY, tsy)
                    };

                    // 0x5D8719 - Very much simplified
                    const auto dir = (CVector2D{*pos} - CVector2D{env_map_atm->PrevPosX, env_map_atm->PrevPosY}).Dot(CVector2D{ ltm->up });
                    env_map_atm->OffsetU = notsa::wrap(
                        env_map_atm->OffsetU + std::copysignf(prevUV.CWSum() - currUV.CWSum(), dir),
                        0.f, 1.f
                    );

                    // 0x5D87E4
                    env_map_atm->PrevPosX = pos->x;
                    env_map_atm->PrevPosY = pos->y;
                }

                // 0x5D87EA
                auto v = std::clamp(ltm->at.x + ltm->at.y, 0.f, 0.1f);
                if (ltm->at.z < 0.f) {
                    v = 1.f - v;
                }
                const auto u = -env_map_atm->OffsetU;
                // Inline end

                // 0x5D9D08 - Now set the texture transform matrix
                D3DMATRIX transform{.m = {
                    { 1.f, 0.f, 0.f, 0.f },
                    { 0.f, 1.f, 0.f, 0.f },
                    { u,   v,   1.f, 0.f },
                    { 0.f, 0.f, 0.f, 1.f },
                }};
                RwD3D9SetTransform(D3DTS_TEXTURE1, &transform);
            }

            // 0x5D9D69
            RwD3D9SetTexture(GetFxEnvTexture(mat), 1);

            // 0x5D9D7B
            const auto a = std::min(255u, (uint32)(specIntensity * 24.f));
            RwD3D9SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(a, 0xFF, 0xFF, 0xFF));

            // 0x5D9DA6
            RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_BLENDFACTORALPHA);
            RwD3D9SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            RwD3D9SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
            RwD3D9SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
            RwD3D9SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            RwD3D9SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
            RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 1);
            RwD3D9SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

            RwD3D9SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
        }

        // 0x5D9E0D
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(mesh->vertexAlpha || mesh->material->color.alpha != 0xFF));

        if (geoHasNoLighting) { // 0x5D9E31 - Render without lighting
            RxD3D9InstanceDataRender(header, mesh);
        } else {
            if (isLightingEnabled) { // 0x5D9E3D
                // Some car materials are initially painted over, so for those we use black (0, 0, 0) instead
                const auto isSpecialColor = notsa::contains<uint32>({
                    0xAF00FF, // @ 0x5D9E6B
                    0x00FFB9, // @ 0x5D9E8D
                    0x00FF3C, // @ 0x5D9E84...
                    0x003CFF,
                    0x00AFFF,
                    0xC8FF00, // @ 0x5D9E9B...
                    0xFF00FF,
                    0xFFFF00,
                }, CRGBA{mat->color}.ToIntRGB());
                const auto color = isSpecialColor
                    ? CRGBA{0, 0, 0, mat->color.alpha}
                    : CRGBA{mat->color};

                // 0x5DA790 - [Inlined] Create and set material using the calculated
                {
                    D3DMATERIAL9 m;

                    m.Specular = { spec, spec, spec };
                    m.Power = power;

                    const auto isPrelit = (rxGeoFlags & rxGEOMETRY_PRELIT) != 0;
                    if (!(rxGeoFlags & rxGEOMETRY_MODULATE) || color == CRGBA{0xFF, 0xFF, 0xFF, 0xFF}) { // 0x5DA7A9
                        const auto d = mat->surfaceProps.diffuse;
                        m.Diffuse = { d, d, d, 1.f };

                        // 0x5DA9DC
                        RwD3D9SetRenderState(D3DRS_AMBIENT, 0xFFFFFFFF);
                        RwD3D9SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);

                        // 0x5DA9F3
                        if (!isPrelit) {
                            m.Emissive = { 0.f, 0.f, 0.f, 0.f };
                        }
                        RwD3D9SetRenderState(D3DRS_COLORVERTEX, isPrelit);
                        RwD3D9SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, isPrelit ? D3DMCS_COLOR1 : D3DMCS_MATERIAL);

                        // 0x5DAA4F - Skipping useless `ambient == 1` optimization
                        m.Ambient.r = AmbientSaturated.red * mat->surfaceProps.ambient;
                        m.Ambient.g = AmbientSaturated.green * mat->surfaceProps.ambient;
                        m.Ambient.b = AmbientSaturated.blue * mat->surfaceProps.ambient;
                    } else { // 0x5DA7E2
                        const auto d = mat->surfaceProps.diffuse;
                        m.Diffuse = {
                            (float)(color.r) / 255.f * d,
                            (float)(color.g) / 255.f * d,
                            (float)(color.b) / 255.f * d,
                            (float)(color.a) / 255.f
                        };

                        RwD3D9SetRenderState(D3DRS_AMBIENT, isPrelit ? color.ToIntARGB() : 0xFFFFFFFF);
                        RwD3D9SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, isPrelit ? D3DMCS_COLOR1 : D3DMCS_MATERIAL);

                        RwD3D9SetRenderState(D3DRS_COLORVERTEX, isPrelit);
                        RwD3D9SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);

                        const auto amb = mat->surfaceProps.ambient;
                        (isPrelit ? m.Ambient : m.Emissive) = {0.f, 0.f, 0.f};
                        (isPrelit ? m.Emissive : m.Ambient) = {
                            (float)(color.r) / 255.f * AmbientSaturated.red * amb,
                            (float)(color.g) / 255.f * AmbientSaturated.green * amb,
                            (float)(color.b) / 255.f * AmbientSaturated.blue * amb,
                        };
                    }
                    RwD3D9SetMaterial(&m);
                }
            }
            RxD3D9InstanceDataRenderLighting(header, mesh, rxGeoFlags, mesh->material->texture); // 0x5D9EEB
        }

        if (useSpecular) {
            RwD3D9SetRenderState(D3DRS_SPECULARENABLE, FALSE);
            RwD3D9EnableLight(1, FALSE);
        }
    }

    constexpr RwSurfaceProperties D3D9SurfaceProps{ 1.f, 0.f, 0.f };
    constexpr RwRGBA              D3D9RGBA{ 0xFF, 0xFF, 0xFF, 0xFF };
    RwD3D9SetSurfaceProperties(&D3D9SurfaceProps, &D3D9RGBA, rxGEOMETRY_LIGHT);

    RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    RwD3D9SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 1);
    RwD3D9SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, NULL);
}
