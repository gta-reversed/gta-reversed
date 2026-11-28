#include "StdInc.h"

#include "FxEmitter.h"
#include "EmissionInfo.h"
#include "FxEmitterPrt.h"
#include "FxPrimBP.h"
#include "FxEmitterBP.h"

// 0x4A2550
bool FxEmitter_c::Init(FxPrimBP_c* primBP, FxSystem_c* system) {
    m_PrimBP            = primBP;
    m_System            = system;
    m_EmissionIntensity = 0.0f;
    return true;
}

// 0x4A4460
void FxEmitter_c::Update(float currentTime, float deltaTime) {
    if (m_bEnabled && !m_System->m_stopParticleCreation) {
        CreateParticles(currentTime, deltaTime);
    }
}

// 0x4A2570
void FxEmitter_c::Reset() {
    m_EmissionIntensity = 0.0f;
}

// 0x4A4050
void FxEmitter_c::AddParticle(const RwMatrix& mat, const CVector& vel, float timeSince, const FxPrtMult_c& fxMults, float rotZ, float brightness, bool createLocal) {
    auto* const local = g_fxMan.FxRwMatrixCreate();
    *local            = mat;
    RwMatrixUpdate(local);
    EmitParticle(*local, vel, timeSince, fxMults, rotZ, brightness, createLocal);
    g_fxMan.FxRwMatrixDestroy(local);
}

// 0x4A3EA0
void FxEmitter_c::AddParticle(const CVector& pos, const CVector& vel, float timeSince, const FxPrtMult_c& fxMults, float rotZ, float brightness, bool createLocal) {
    auto* const local = g_fxMan.FxRwMatrixCreate();
    RwMatrixSetIdentity(local);
    *RwMatrixGetPos(local) = pos;
    RwMatrixUpdate(local);
    EmitParticle(*local, vel, timeSince, fxMults, rotZ, brightness, createLocal);
    g_fxMan.FxRwMatrixDestroy(local);
}

// 0x4A41E0
void FxEmitter_c::CreateParticles(float currentTime, float deltaTime) {
    EmissionInfo_t emission;
    m_PrimBP->m_FxInfoManager.ProcessEmissionInfo(currentTime, deltaTime, m_System->m_SystemBP->m_fLength, m_System->m_UseConstTime, &emission);

    const auto  lodStart = (float)m_PrimBP->m_FxInfoManager.m_nLodStart / 64.0f;
    const auto  lodEnd   = (float)m_PrimBP->m_FxInfoManager.m_nLodEnd / 64.0f;
    const float camDist  = m_System->m_fCameraDistance;
    const auto  lodMult  = camDist < lodStart ? 1.0f
        : camDist > lodEnd                    ? 0.0f
                                              : 1.0f - (camDist - lodStart) / (lodEnd - lodStart);
    m_EmissionIntensity += emission.m_fCount * lodMult * ((float)m_System->m_nRateMult / 1000.0f);

    if (CWeather::Wind < emission.m_fMinWind || CWeather::Wind > emission.m_fMaxWind) {
        return;
    }
    if (CWeather::Rain < emission.m_fMinRain || CWeather::Rain > emission.m_fMaxRain) {
        return;
    }
    if (m_EmissionIntensity < 1.0f) {
        return;
    }

    auto* const wldMat = g_fxMan.FxRwMatrixCreate();
    RwMatrixUpdate(&m_System->m_LocalMatrix);
    GetEmitterMatrix(*wldMat, m_System->m_LocalMatrix);

    for (auto i = 0; i < (int32)m_EmissionIntensity; i++) {
        CreateParticle(emission, *wldMat, nullptr, (float)i / m_EmissionIntensity * deltaTime, FxPrtMult_c{}, 1.2f, m_System->m_createLocal);
    }
    m_EmissionIntensity -= (float)(int32)m_EmissionIntensity;

    g_fxMan.FxRwMatrixDestroy(wldMat);
}

// 0x4A2580
FxEmitterPrt_c* FxEmitter_c::CreateParticle(const EmissionInfo_t& emissionInfo, RwMatrix& wldMat, const CVector* velOverride, float timeSince, const FxPrtMult_c& fxMults, float brightness, bool createLocal) {
    auto* particle = g_fxMan.GetParticle(0);
    if (!particle) {
        if (!m_System->m_MustCreateParticles) {
            return nullptr;
        }
        g_fxMan.FreeUpParticle();
        particle = g_fxMan.GetParticle(0);
        if (!particle) {
            return nullptr;
        }
    }
    auto* const prt        = particle->AsFxEmitterPrt();

    prt->m_fCurrentLife    = 0.0f;
    prt->m_fTotalLife      = (CGeneral::GetRandomNumberInRange(-1.0f, 1.0f) * emissionInfo.m_fLifeBias + emissionInfo.m_fLife) * fxMults.m_fLife;
    prt->m_System          = m_System;

    prt->m_MultColor       = CRGBA{ fxMults.m_Color };
    prt->m_MultSize        = fxMults.m_fSize;
    prt->m_MultRot         = fxMults.m_Rot;
    prt->m_bLocalToSystem  = createLocal;

    prt->m_RandR           = CGeneral::GetRandomNumberInRange(0, 256);
    prt->m_RandG           = CGeneral::GetRandomNumberInRange(0, 256);
    prt->m_RandB           = CGeneral::GetRandomNumberInRange(0, 256);
    prt->m_Brightness      = brightness;

    prt->m_RotZ            = -1;
    prt->m_CurrentRotation = CGeneral::GetRandomNumberInRange(emissionInfo.m_fRotationMinAngle, emissionInfo.m_fRotationMaxAngle);

    if (createLocal) {
        m_PrimBP->GetRWMatrix(wldMat);
    }

    const auto offset = [&] {
        if (std::abs(emissionInfo.m_fRadius) < 0.001f) { // 0x4A283C - In a box
            return CVector{
                CGeneral::GetRandomNumberInRange(emissionInfo.m_SizeMin.x, emissionInfo.m_SizeMax.x),
                CGeneral::GetRandomNumberInRange(emissionInfo.m_SizeMin.y, emissionInfo.m_SizeMax.y),
                CGeneral::GetRandomNumberInRange(emissionInfo.m_SizeMin.z, emissionInfo.m_SizeMax.z),
            };
        }

        // 0x4A2759 - In a sphere: on its surface for a negative radius, inside it otherwise
        const CVector dir{
            CGeneral::GetRandomNumberInRange(-1.0f, 1.0f),
            CGeneral::GetRandomNumberInRange(-1.0f, 1.0f),
            CGeneral::GetRandomNumberInRange(-1.0f, 1.0f),
        };
        const auto invDist = 1.0f / dir.Magnitude();
        const auto scale   = emissionInfo.m_fRadius < 0.0f
            ? invDist
            : CGeneral::GetRandomNumberInRange(0.0f, 1.0f) * invDist;
        return dir * (scale * emissionInfo.m_fRadius);
    }() + emissionInfo.m_Pos;

    prt->m_Pos = offset.z * wldMat.at + offset.y * wldMat.up + offset.x * wldMat.right + wldMat.pos;

    if (velOverride) {
        prt->m_Velocity = *velOverride;
    } else {
        const auto    heading = CGeneral::GetRandomNumberInRange(0.0f, TWO_PI);
        const auto    pitch   = CGeneral::GetRandomNumberInRange(DegreesToRadians(emissionInfo.m_fAngleMin), DegreesToRadians(emissionInfo.m_fAngleMax));
        const CVector dirInCone{
            CMaths::GetCosFast(heading) * CMaths::GetSinFast(pitch),
            CMaths::GetCosFast(pitch),
            CMaths::GetSinFast(heading) * CMaths::GetSinFast(pitch)
        };

        // NOTE: A direction with x > 10 means "away from the origin": the original then uses the particle's world position as the direction
        const auto coneAxis = emissionInfo.m_Direction.x <= 10.0f
            ? emissionInfo.m_Direction.Normalized()
            : prt->m_Pos;
        CVector    coneAxisWld;
        RwV3dTransformVectors(&coneAxisWld, &coneAxis, 1, &wldMat);

        CVector dir;
        RotateVecIntoVec(dir, dirInCone, coneAxisWld);
        prt->m_Velocity = dir * (CGeneral::GetRandomNumberInRange(-1.0f, 1.0f) * emissionInfo.m_fSpeedBias + emissionInfo.m_Speed);
    }
    prt->m_Velocity += m_System->m_VelAdd;

    static_cast<FxEmitterBP_c*>(m_PrimBP)->UpdateParticle(timeSince, prt);
    m_PrimBP->m_Particles.AddItem(prt);

    return prt;
}

// NOTSA - `local` placed in the system's parent space, then transformed by the prim's own matrix
void FxEmitter_c::GetEmitterMatrix(RwMatrix& out, const RwMatrix& local) const {
    auto* const systemMat = g_fxMan.FxRwMatrixCreate();
    if (m_System->m_ParentMatrix) {
        RwMatrixMultiply(systemMat, &local, m_System->m_ParentMatrix);
    } else {
        *systemMat = local;
    }

    auto* const primMat = g_fxMan.FxRwMatrixCreate();
    m_PrimBP->GetRWMatrix(*primMat);
    RwMatrixMultiply(&out, primMat, systemMat);
    g_fxMan.FxRwMatrixDestroy(primMat);

    g_fxMan.FxRwMatrixDestroy(systemMat);
}

// NOTSA - The common part of both `AddParticle`s
void FxEmitter_c::EmitParticle(const RwMatrix& local, const CVector& vel, float timeSince, const FxPrtMult_c& fxMults, float rotZ, float brightness, bool createLocal) {
    EmissionInfo_t emission;
    m_PrimBP->m_FxInfoManager.ProcessEmissionInfo(0.0f, 0.0f, m_System->m_SystemBP->m_fLength, m_System->m_UseConstTime, &emission);

    auto* const wldMat = g_fxMan.FxRwMatrixCreate();
    GetEmitterMatrix(*wldMat, local);
    if (auto* const prt = CreateParticle(emission, *wldMat, &vel, timeSince, fxMults, brightness, createLocal)) {
        if (rotZ >= 0.0f) {
            prt->m_RotZ = (int8)(int32)(rotZ / 2.0f);
        }
    }
    g_fxMan.FxRwMatrixDestroy(wldMat);
}

void FxEmitter_c::InjectHooks() {
    RH_ScopedVirtualClass(FxEmitter_c, 0x85A7A4, 6);
    RH_ScopedCategory("Fx");

    RH_ScopedVMTInstall(Init, 0x4A2550);
    RH_ScopedVMTInstall(Update, 0x4A4460);
    RH_ScopedVMTInstall(Reset, 0x4A2570);
    RH_ScopedVMTOverloadedInstall(AddParticle, "mat", 0x4A4050, void (FxEmitter_c::*)(const RwMatrix&, const CVector&, float, const FxPrtMult_c&, float, float, bool));
    RH_ScopedVMTOverloadedInstall(AddParticle, "pos", 0x4A3EA0, void (FxEmitter_c::*)(const CVector&, const CVector&, float, const FxPrtMult_c&, float, float, bool));
    RH_ScopedInstall(CreateParticles, 0x4A41E0);
    RH_ScopedInstall(CreateParticle, 0x4A2580);
}
