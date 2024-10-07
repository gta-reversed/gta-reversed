#include "StdInc.h"

#include "FxEmitter.h"
#include "EmissionInfo.h"
#include "FxEmitterPrt.h"
#include "FxPrimBP.h"
#include "FxEmitterBP.h"
#include "FxEmitterPrt.h"

void FxEmitter_c::InjectHooks() {
    RH_ScopedVirtualClass(FxEmitter_c, 0x85A7A4, 6);
    RH_ScopedCategory("Fx");

    // RH_ScopedInstall(Init_Reversed, 0x4A2550);
    // RH_ScopedInstall(Update_Reversed, 0x4A4460); // bad
    // RH_ScopedInstall(Reset_Reversed, 0x4A2570);
    // RH_ScopedOverloadedInstall(AddParticle_Reversed, "matrix", 0x4A3EA0, void(FxEmitter_c::*)(RwMatrix*, CVector*, float, FxPrtMult_c*, float, float, bool));
    // RH_ScopedOverloadedInstall(AddParticle_Reversed, "", 0x4A4050, void(FxEmitter_c::*)(CVector*, CVector*, float, FxPrtMult_c*, float, float, bool));
    // RH_ScopedInstall(CreateParticles, 0x4A41E0);
    // RH_ScopedInstall(CreateParticle, 0x4A2580);
}


// 0x4A2550
bool FxEmitter_c::Init(FxPrimBP_c* primBP, FxSystem_c* system) {
    m_PrimBP             = primBP;
    m_System             = system;
    m_fEmissionIntensity = 0.0f;
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
    m_fEmissionIntensity = 0.0f;
}

// 0x4A3EA0
void FxEmitter_c::AddParticle(CVector* pos, CVector* vel, float timeSince, FxPrtMult_c* fxMults, float rotZ, float brightness, bool createLocal) {
    return plugin::CallMethod<0x4A3EA0, FxEmitter_c*, CVector*, CVector*, float, FxPrtMult_c*, float, float, bool>(this, pos, vel, timeSince, fxMults, rotZ, brightness,
                                                                                                                   createLocal);

    // todo:
    EmissionInfo_t emission;
    m_PrimBP->m_FxInfoManager.ProcessEmissionInfo(0.0f, 0.0f, m_System->m_SystemBP->m_fLength, m_System->m_UseConstTime, &emission);

    auto mat1 = g_fxMan.FxRwMatrixCreate();
    auto mat0 = g_fxMan.FxRwMatrixCreate();
    auto mat  = g_fxMan.FxRwMatrixCreate();

    RwMatrixSetIdentity(mat);
    RwV3dAssign(RwMatrixGetPos(mat), pos);

    RwMatrixUpdate(mat);
    if (m_System->m_ParentMatrix) {
        RwMatrixMultiply(mat0, mat, m_System->m_ParentMatrix);
    }
    else {
        mat0 = mat;
    }

    auto mat2 = g_fxMan.FxRwMatrixCreate();
    m_PrimBP->GetRWMatrix(*mat2);
    RwMatrixMultiply(mat1, mat2, mat0);
    g_fxMan.FxRwMatrixDestroy(mat2);

    if (auto* particle = CreateParticle(&emission, mat1, vel, timeSince, fxMults, brightness, createLocal)) {
        if (rotZ >= 0.0f) {
            particle->m_RotZ = (uint8)(rotZ / 2.0f); // todo: int8/uint8 conversion
        }
    }

    g_fxMan.FxRwMatrixDestroy(mat);
    g_fxMan.FxRwMatrixDestroy(mat0);
    g_fxMan.FxRwMatrixDestroy(mat1);
}

// 0x4A4050
void FxEmitter_c::AddParticle(RwMatrix* mat, CVector* vel, float timeSince, FxPrtMult_c* fxMults, float rotZ, float brightness, bool createLocal) {
    return plugin::CallMethod<0x4A4050, FxEmitter_c*, RwMatrix*, CVector*, float, FxPrtMult_c*, float, float, bool>(this, mat, vel, timeSince, fxMults, rotZ, brightness,
                                                                                                                    createLocal);

    // todo:
    EmissionInfo_t emission;
    m_PrimBP->m_FxInfoManager.ProcessEmissionInfo(0.0f, 0.0f, m_System->m_SystemBP->m_fLength, m_System->m_UseConstTime, &emission);
    auto mat2 = g_fxMan.FxRwMatrixCreate();
    auto mat1 = g_fxMan.FxRwMatrixCreate();
    auto mat0 = g_fxMan.FxRwMatrixCreate();
    RwMatrixSetIdentity(mat0);
    mat0 = mat;

    RwMatrixUpdate(mat0);
    if (m_System->m_ParentMatrix) {
        RwMatrixMultiply(mat1, mat0, m_System->m_ParentMatrix);
    } else {
        mat1 = mat0;
    }

    auto mat3 = g_fxMan.FxRwMatrixCreate();
    m_PrimBP->GetRWMatrix(*mat3);
    RwMatrixMultiply(mat2, mat3, mat1);
    g_fxMan.FxRwMatrixDestroy(mat3);

    if (auto* particle = CreateParticle(&emission, mat2, vel, timeSince, fxMults, brightness, createLocal)) {
        if (rotZ >= 0.0f)
            particle->m_RotZ = (uint8)(rotZ * 2.0f); // todo:: see above
    }

    g_fxMan.FxRwMatrixDestroy(mat0);
    g_fxMan.FxRwMatrixDestroy(mat1);
    g_fxMan.FxRwMatrixDestroy(mat2);
}

// 0x4A41E0
void FxEmitter_c::CreateParticles(float currentTime, float deltaTime) {
    return plugin::CallMethod<0x4A41E0, FxEmitter_c*, float, float>(this, currentTime, deltaTime);

    EmissionInfo_t emission;
    m_PrimBP->m_FxInfoManager.ProcessEmissionInfo(currentTime, deltaTime, m_System->m_SystemBP->m_fLength, m_System->m_UseConstTime, &emission);

    auto lodStart = (float)m_PrimBP->m_FxInfoManager.m_nLodStart / 64.0f;
    auto lodEnd   = (float)m_PrimBP->m_FxInfoManager.m_nLodEnd   / 64.0f;

    float visibility;
    if (m_System->m_fCameraDistance >= lodStart) {
        if (m_System->m_fCameraDistance <= lodEnd) {
            visibility = 1.0f - (m_System->m_fCameraDistance - lodStart) / (lodEnd - lodStart);
        } else {
            visibility = 0.0f;
        }
    } else {
        visibility = 1.0f;
    }

    m_fEmissionIntensity += emission.m_fCount * visibility * ((float)m_System->m_nRateMult * 1000.0f);
    if (   CWeather::Wind >= emission.m_fMinWind
        && CWeather::Wind <= emission.m_fMaxWind
        && CWeather::Rain >= emission.m_fMinRain
        && CWeather::Rain <= emission.m_fMaxRain
        && m_fEmissionIntensity >= 1.0f
    ) {
        auto mat = g_fxMan.FxRwMatrixCreate();
        auto mat1 = g_fxMan.FxRwMatrixCreate();
        RwMatrixUpdate(&m_System->m_LocalMatrix);
        if (m_System->m_ParentMatrix) {
            RwMatrixMultiply(mat1, &m_System->m_LocalMatrix, m_System->m_ParentMatrix);
        } else {
            *mat1 = m_System->m_LocalMatrix;
        }

        auto mat2 = g_fxMan.FxRwMatrixCreate();
        m_PrimBP->GetRWMatrix(*mat2);
        RwMatrixMultiply(mat, mat2, mat1);
        g_fxMan.FxRwMatrixDestroy(mat2);

        auto counter = 0u;
        for (auto step = 0u; counter < (uint32)m_fEmissionIntensity; step = counter) {
            FxPrtMult_c prtMult;
            auto timeSince = (float)step / m_fEmissionIntensity * deltaTime;
            CreateParticle(&emission, mat, nullptr, timeSince, &prtMult, 1.2f, m_System->m_createLocal);
            counter++;
        }
        m_fEmissionIntensity -= m_fEmissionIntensity; // OG shit

        g_fxMan.FxRwMatrixDestroy(mat1);
        g_fxMan.FxRwMatrixDestroy(mat);
    }
}

// 0x4A2580
FxEmitterPrt_c* FxEmitter_c::CreateParticle(EmissionInfo_t* emissionInfo, RwMatrix* wldMat, CVector* velOverride, float timeSince, FxPrtMult_c* fxMults, float brightness, bool createLocal) {
    //return plugin::CallMethodAndReturn<FxEmitterPrt_c*, 0x4A2580, FxEmitter_c*, EmissionInfo_t*, RwMatrix*, CVector*, float, FxPrtMult_c*, float, bool>(this, emissionInfo, wldMat, velOverride, timeSince, fxMults, brightness, createLocal);

    // todo:
    auto* particle = [&]() -> FxEmitterPrt_c* {
        auto* prt = g_fxMan.GetParticle(0);
        if (!prt) {
            if (!m_System->m_MustCreateParticles)
                return nullptr;

            g_fxMan.FreeUpParticle();
            prt = g_fxMan.GetParticle(0);
        }
        return reinterpret_cast<FxEmitterPrt_c*>(prt);
    }();

    particle->m_fCurrentLife = 0.0f;
    particle->m_fTotalLife = (((float)(CGeneral::GetRandomNumber() % 10'000) / 5'000.0f - 1.0f) * emissionInfo->m_fLifeBias + emissionInfo->m_fLife) * fxMults->m_fLife;
    particle->m_System = m_System;

    particle->m_MultColor = CRGBA{fxMults->m_Color};

    particle->m_MultSize = fxMults->m_fSize;
    particle->m_MultRot = fxMults->m_Rot;

    particle->m_bLocalToSystem = createLocal;

    particle->m_RandR = CGeneral::GetRandomNumberInRange(0, 256);
    particle->m_RandG = CGeneral::GetRandomNumberInRange(0, 256);
    particle->m_RandB = CGeneral::GetRandomNumberInRange(0, 256);
    particle->m_Brightness = brightness;

    particle->m_RotZ = -1;
    particle->m_CurrentRotation = CGeneral::GetRandomNumberInRange(0.0f, 1.0f) * (emissionInfo->m_fRotationMaxAngle - emissionInfo->m_fRotationMinAngle) + emissionInfo->m_fRotationMinAngle;

    if (createLocal) {
        m_PrimBP->GetRWMatrix(*wldMat);
    }

    CVector vec;
    if (approxEqual(emissionInfo->m_fRadius, 0.0f, 0.001f)) {
        vec = CVector{
            CGeneral::GetRandomNumberInRange(0.0f, 1.0f) * (emissionInfo->m_SizeMax.x - emissionInfo->m_SizeMin.x) + emissionInfo->m_SizeMin.x,
            CGeneral::GetRandomNumberInRange(0.0f, 1.0f) * (emissionInfo->m_SizeMax.y - emissionInfo->m_SizeMin.y) + emissionInfo->m_SizeMin.y,
            CGeneral::GetRandomNumberInRange(0.0f, 1.0f) * (emissionInfo->m_SizeMax.z - emissionInfo->m_SizeMin.z) + emissionInfo->m_SizeMin.z,
        };
    } else {
        vec = CVector{
            CGeneral::GetRandomNumberInRange(0.0f, 2.0f) - 1.0f,
            CGeneral::GetRandomNumberInRange(0.0f, 2.0f) - 1.0f,
            CGeneral::GetRandomNumberInRange(0.0f, 2.0f) - 1.0f,
        };

        float radius;
        auto invDist = 1.0f / vec.Magnitude();
        if (emissionInfo->m_fRadius < 0.0f) { // todo: check comp.
            radius = invDist * emissionInfo->m_fRadius;
        } else {
            radius = (invDist * CGeneral::GetRandomNumberInRange(0.0f, 1.0f)) * emissionInfo->m_fRadius;
        }
        vec *= radius;
    }
    vec += emissionInfo->m_Pos;

    particle->m_Pos = vec.z * wldMat->at + vec.y * wldMat->up + vec.x * wldMat->right + wldMat->pos;

    if (velOverride) {
        particle->m_Velocity = *velOverride;
    } else {
        auto index = CGeneral::GetRandomNumberInRange(0.0f, TWO_PI);
        auto minAngle = DegreesToRadians(emissionInfo->m_fAngleMin);
        auto maxAngle = DegreesToRadians(emissionInfo->m_fAngleMax);

        auto sinY = (CGeneral::GetRandomNumberInRange(0.0f, 1.0f) * (maxAngle - minAngle) + minAngle) * 40.743664f;

        CVector v37;
        v37.x = CMaths::ms_SinTable[(uint8)((index * 40.743664f) + 64.0f) + 1] * CMaths::ms_SinTable[(uint8)sinY + 1];
        v37.y = CMaths::ms_SinTable[(uint8)(sinY + 64.0f) + 1];
        v37.z = CMaths::ms_SinTable[(uint8)(index * 40.743664f) + 1] * CMaths::ms_SinTable[(uint8)sinY + 1];

        CVector vectorsIn;
        CVector vectorsOut;
        if (emissionInfo->m_Direction.x <= 10.0f) {
            vectorsIn = emissionInfo->m_Direction;
            vectorsIn.Normalise();
        } else {
            vectorsIn = particle->m_Pos;
        }
        RwV3dTransformVectors(&vectorsOut, &vectorsIn, 1, wldMat);
        CVector v38;
        RotateVecIntoVec(&v38, &v37, &vectorsOut);
        particle->m_Velocity = v38 * ((CGeneral::GetRandomNumberInRange(0.0f, 2.0f) - 1.0f) * emissionInfo->m_fSpeedBias + emissionInfo->m_fSpeed);
    }

    particle->m_Velocity += m_System->m_VelAdd;
    static_cast<FxEmitterBP_c*>(m_PrimBP)->UpdateParticle(timeSince, particle);
    m_PrimBP->m_Particles.AddItem(particle);

    return particle;
}
