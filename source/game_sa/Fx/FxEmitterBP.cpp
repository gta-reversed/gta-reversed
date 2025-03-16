#include "StdInc.h"

#include "FxEmitterBP.h"
#include "FxEmitter.h"
#include "FxPrimBP.h"
#include "FxEmitterPrt.h"
#include "FxInfo.h"
#include "FxInfoManager.h"

#include "Particle.h"
#include "FxTools.h"

void FxEmitterBP_c::InjectHooks() {
    RH_ScopedVirtualClass(FxEmitterBP_c, 0x85A788, 7);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x4A18D0);
    RH_ScopedInstall(RenderHeatHaze, 0x4A1940, {.reversed = false});
    RH_ScopedInstall(UpdateParticle, 0x4A21D0, {.reversed = false});
    RH_ScopedVMTInstall(CreateInstance, 0x4A2B40, {.reversed = false}); // bad
    RH_ScopedVMTInstall(Update, 0x4A2BC0, {.reversed = false});
    RH_ScopedVMTInstall(Load, 0x5C25F0, {.reversed = false});
    RH_ScopedVMTInstall(LoadTextures, 0x5C0A30, {.reversed = true});
    RH_ScopedVMTInstall(Render, 0x4A2C40, {.reversed = false});
    RH_ScopedVMTInstall(FreePrtFromPrim, 0x4A2510, {.reversed = false});
}

// 0x4A18D0
FxEmitterBP_c::FxEmitterBP_c() : FxPrimBP_c() {
    m_Type = 0;
}

// 0x4A1940
void FxEmitterBP_c::RenderHeatHaze(RwCamera* camera, uint32 txdHashKey, float brightness) {
    return plugin::CallMethod<0x4A1940, FxEmitterBP_c*, RwCamera*, uint32, float>(this, camera, txdHashKey, brightness);
}

// 0x4A21D0
bool FxEmitterBP_c::UpdateParticle(float deltaTime, FxEmitterPrt_c* emitter) {
    return plugin::CallMethodAndReturn<bool, 0x4A21D0, FxEmitterBP_c*, float, FxEmitterPrt_c*>(this, deltaTime, emitter);
}

// 0x4A2B40


FxPrim_c* FxEmitterBP_c::CreateInstance() {
    return new FxEmitter_c();
}

// 0x4A2BC0


void FxEmitterBP_c::Update(float deltaTime) {
    for (auto it = m_Particles.GetHead(); it; it = m_Particles.GetNext(it)) {
        if (it->m_System->m_nKillStatus == eFxSystemKillStatus::FX_3) {
            it->m_System->m_nKillStatus = eFxSystemKillStatus::FX_KILLED;
        }

        // wrong casts or smth
        if (it->m_System->m_nPlayStatus != eFxSystemPlayStatus::T2 && UpdateParticle(deltaTime, reinterpret_cast<FxEmitterPrt_c*>(it))) {
            m_Particles.RemoveItem(it);
            g_fxMan.ReturnParticle(reinterpret_cast<FxEmitterPrt_c*>(it));
        }
    }
}

// 0x5C25F0


bool FxEmitterBP_c::Load(FILESTREAM file, int32 version, FxName32_t* textureNames) {
    FxPrimBP_c::Load(file, version, textureNames);

    m_nLodStart = uint16(ReadField<float>(file, "LODSTART:") * 64.0f);
    m_nLodEnd   = uint16(ReadField<float>(file, "LODEND:") * 64.0f);

    return true;
}

// 0x5C0A30


bool FxEmitterBP_c::LoadTextures(FxName32_t* textureNames, int32 version) {
    assert(textureNames);

    const auto LoadTexture = [&](auto ind) -> RwTexture* {
        char mask[64];
        sprintf(mask, "%sm", textureNames[ind]);

        auto* texture = RwTextureRead(textureNames[ind], mask);
        return texture ? texture : RwTextureRead(textureNames[ind], nullptr);
    };

    const auto LoadTextureIfExists = [=](auto ind) -> RwTexture* {
        assert(&textureNames[ind]);
        return strncmp(textureNames[ind], "NULL", 5u) != 0 ? LoadTexture(ind) : nullptr;
    };

    m_apTextures[0] = LoadTexture(0);

    if (version > 101) {
        m_apTextures[1] = LoadTextureIfExists(1);
        m_apTextures[2] = LoadTextureIfExists(2);
        m_apTextures[3] = LoadTextureIfExists(3);
    }

    return true;
}

// 0x4A2C40


void FxEmitterBP_c::Render(RwCamera* camera, uint32 txdHashKey, float brightness, bool doHeatHaze) {
    return plugin::CallMethod<0x4A2C40, FxEmitterBP_c*, RwCamera*, uint32, float, bool>(this, camera, txdHashKey, brightness, doHeatHaze);

    /*
    static constexpr RwBlendFunction g_BlendFunctions[] = {
        rwBLENDZERO,      rwBLENDONE,          rwBLENDSRCCOLOR,  rwBLENDINVSRCCOLOR,  rwBLENDSRCALPHA, rwBLENDINVSRCALPHA,
        rwBLENDDESTALPHA, rwBLENDINVDESTALPHA, rwBLENDDESTCOLOR, rwBLENDINVDESTCOLOR, rwBLENDSRCALPHASAT
    };

    if (doHeatHaze) {
        if (m_FxInfoManager.m_bHasHeatHazeParticleEmitter)
            RenderHeatHaze(camera, txdHashKey, brightness);
        return;
    }

    if (IsFxInfoPresent(FX_INFO_HEATHAZE_DATA)) {
        if (m_Particles.GetNumItems())
            g_fxMan.m_bHeatHazeEnabled = true;
        return;
    }

    if (!m_Particles.GetNumItems())
        return;

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(m_bAlphaOn));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(g_BlendFunctions[m_bAlphaOn ? m_nSrcBlendId : 1]));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(g_BlendFunctions[m_bAlphaOn ? m_nDstBlendId : 0]));

    auto* raster = RwTextureGetRaster(m_apTextures[0]);
    RenderBegin(raster, nullptr, rwIM3D_VERTEXUV);

    for (auto* prt = (FxEmitterPrt_c*)m_Particles.GetHead(); prt; prt = (FxEmitterPrt_c*)m_Particles.GetNext(prt)) {
        const auto pos = [prt] {
            if (prt->m_bLocalToSystem) {
                // Get updated matrix position.
                CVector out{};
                auto* mat = g_fxMan.FxRwMatrixCreate();
                prt->m_System->GetCompositeMatrix(mat);
                RwV3dTransformPoint(&out, &prt->m_Pos, mat); // SA: RwV3dTransformPoints(...,...,1,...)
                g_fxMan.FxRwMatrixDestroy(mat);
                return out;
            } else {
                return prt->m_Pos;
            }
        }();

        RenderInfo_t renderInfo{};
        m_FxInfoManager.ProcessRenderInfo(
            prt->m_System->m_fCurrentTime,
            prt->m_fCurrentLife / prt->m_fTotalLife,
            0.0f,
            prt->m_System->m_SystemBP->m_fLength,
            false,
            &renderInfo
        );

        if (renderInfo.m_SmokeType > -1) {
            // RenderSmoke();
            CGeneral::GetRandomNumberInRange(0.0f, 1.0f) *
        }
    }

    RenderEnd();
    */
}

// 0x4A2510

// 0x0
bool FxEmitterBP_c::FreePrtFromPrim(FxSystem_c* system) {
    return plugin::CallMethodAndReturn<bool, 0x4A2510, FxEmitterBP_c*, FxSystem_c*>(this, system);
}

// todo: eFxInfo
// 0x4A24D0
bool FxEmitterBP_c::IsFxInfoPresent(eFxInfoType type) const {
    if (m_FxInfoManager.m_nNumInfos <= 0)
        return false;

    for (auto& info : m_FxInfoManager.GetInfos()) {
        if (info->m_nType == type) {
            return true;
        }
    }
    return false;
}
