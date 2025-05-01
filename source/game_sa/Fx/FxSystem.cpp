#include "StdInc.h"

#include "FxSystem.h"
#include "FxPrim.h"
#include "FxEmitter.h"
#include "FxEmitterBP.h"
#include "FxEmitterPrt.h"
#include "FxBox.h"
#include "Particle.h"

void FxSystem_c::InjectHooks() {
    RH_ScopedClass(FxSystem_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x4AAF00);
    RH_ScopedInstall(Destructor, 0x4AA260);
    RH_ScopedInstall(Init, 0x4AA750);
    RH_ScopedInstall(Exit, 0x4AA840);
    RH_ScopedInstall(Play, 0x4AA2F0);
    RH_ScopedInstall(Pause, 0x4AA370);
    RH_ScopedInstall(Stop, 0x4AA390);
    RH_ScopedInstall(PlayAndKill, 0x4AA3D0);
    RH_ScopedInstall(Kill, 0x4AA3F0);
    RH_ScopedInstall(AttachToBone, 0x4AA400);
    RH_ScopedOverloadedInstall(AddParticle, "v3d", 0x4AA440, void(FxSystem_c::*)(CVector*,CVector*,float,FxPrtMult_c*,float,float,float,bool), {.reversed = false});
    RH_ScopedOverloadedInstall(AddParticle, "mat", 0x4AA540, void(FxSystem_c::*)(RwMatrix*,CVector*,float,FxPrtMult_c*,float,float,float,bool), {.reversed = false});
    RH_ScopedInstall(EnablePrim, 0x4AA610);
    RH_ScopedInstall(SetMatrix, 0x4AA630);
    RH_ScopedInstall(SetOffsetPos, 0x4AA660);
    RH_ScopedInstall(AddOffsetPos, 0x4AA690);
    RH_ScopedInstall(CopyParentMatrix, 0x4AA890);
    RH_ScopedInstall(GetCompositeMatrix, 0x4AA8C0);
    RH_ScopedInstall(GetPlayStatus, 0x4AA900);
    RH_ScopedInstall(ForAllParticles, 0x4AA930);
    RH_ScopedInstall(UpdateBoundingBoxCB, 0x4AA9A0, {.reversed=false});
    RH_ScopedInstall(GetBoundingSphereWld, 0x4AAAD0);
    RH_ScopedInstall(GetBoundingSphereLcl, 0x4AAB50);
    RH_ScopedInstall(SetBoundingSphere, 0x4AAB80);
    RH_ScopedInstall(ResetBoundingSphere, 0x4AABF0);
    RH_ScopedInstall(SetConstTime, 0x4AA6C0);
    RH_ScopedInstall(SetRateMult, 0x4AA6F0);
    RH_ScopedInstall(SetTimeMult, 0x4AA710);
    RH_ScopedInstall(SetVelAdd, 0x4AA730);
    RH_ScopedInstall(SetLocalParticles, 0x4AA910);
    RH_ScopedInstall(SetZTestEnable, 0x4AAC50);
    RH_ScopedInstall(SetMustCreatePrts, 0x4AAC70);
    RH_ScopedInstall(DoFxAudio, 0x4AAC90);
    RH_ScopedInstall(IsVisible, 0x4AAF30);
    RH_ScopedInstall(Update, 0x4AAF70, {.reversed=false});
}
FxSystem_c* FxSystem_c::Constructor() { this->FxSystem_c::FxSystem_c(); return this; }
FxSystem_c* FxSystem_c::Destructor() { this->FxSystem_c::~FxSystem_c(); return this; }

// 0x4AAF00
FxSystem_c::FxSystem_c() : ListItem_c() {
    m_SystemBP        = nullptr;
    m_BoundingSphere  = nullptr;
    m_MustCreateParticles = false;
}

// 0x4AA260
FxSystem_c::~FxSystem_c() {
    m_SystemBP = nullptr;

    if (m_BoundingSphere) // looks pointless
        CMemoryMgr::Free(m_BoundingSphere);

    if (m_allocatedParentMat)
        RwMatrixDestroy(m_ParentMatrix);
}

// 0x4AA750
bool FxSystem_c::Init(FxSystemBP_c* systemBP, const RwMatrix& local, RwMatrix* parent) {
    m_SystemBP     = systemBP;
    m_LocalMatrix  = local;
    m_ParentMatrix = parent;
    m_fCurrentTime = 0;
    m_nPlayStatus  = eFxSystemPlayStatus::FX_STOPPED;
    m_nKillStatus  = eFxSystemKillStatus::FX_NOT_KILLED;
    m_UseConstTime = false;
    m_nConstTime   = 0;
    m_nRateMult    = 1000;
    m_nTimeMult    = 1000;
    m_VelAdd       = CVector();
    m_useZTest     = true;

    m_BoundingSphere = nullptr;
    if (m_SystemBP->m_BoundingSphere) {
        m_BoundingSphere = new (true) FxSphere_c(*systemBP->m_BoundingSphere);
    }

    m_Prims = new FxPrim_c * [m_SystemBP->m_nNumPrims];
    for (auto i = 0; i < m_SystemBP->m_nNumPrims; i++) {
        m_Prims[i] = m_SystemBP->m_Prims[i]->CreateInstance();
        m_Prims[i]->Init(m_SystemBP->m_Prims[i], this);
    }

    m_FireAE.Initialise(this);
    return true;
}

// 0x4AA840
void FxSystem_c::Exit() {
    for (auto& prim : GetPrims()) {
        delete prim;
    }
    delete[] m_Prims;
    m_FireAE.Terminate();
}

// 0x4AA2F0
void FxSystem_c::Play() {
    if (m_nPlayStatus != eFxSystemPlayStatus::T2) {
        m_fCurrentTime = 0;
        for (auto& prim : GetPrims()) {
            prim->Reset();
        }
    }

    m_nKillStatus = eFxSystemKillStatus::FX_NOT_KILLED;
    m_nPlayStatus = eFxSystemPlayStatus::FX_PLAYING;

    m_stopParticleCreation = false;
    m_prevCulled = false;

    m_LoopInterval = (float)(CGeneral::GetRandomNumber() % 10'000) / 10'000.0f * (m_SystemBP->m_fLoopLength - m_SystemBP->m_fLoopIntervalMin) + m_SystemBP->m_fLoopIntervalMin;
}

// 0x4AA370
void FxSystem_c::Pause() {
    if (m_nPlayStatus == eFxSystemPlayStatus::FX_STOPPED)
        return;

    if (m_nPlayStatus == eFxSystemPlayStatus::T2) {
        m_nPlayStatus = eFxSystemPlayStatus::FX_PLAYING;
    } else {
        m_nPlayStatus = eFxSystemPlayStatus::T2;
    }
}

// 0x4AA390
void FxSystem_c::Stop() {
    m_nPlayStatus = eFxSystemPlayStatus::FX_STOPPED;
    m_fCurrentTime = 0;

    for (auto& prim : GetPrims()) {
        prim->Reset();
    }
}

// 0x4AA3D0
void FxSystem_c::PlayAndKill() {
    Play();

    if (m_SystemBP->m_nPlayMode != 0)
        return;

    m_nKillStatus = FX_PLAY_AND_KILL;
}

// 0x4AA3F0
void FxSystem_c::Kill() {
    Stop();
    m_nKillStatus = FX_KILLED;
}

// 0x4AA400
void FxSystem_c::AttachToBone(CEntity* entity, eBoneTag boneId) {
    auto animHier = GetAnimHierarchyFromSkinClump(entity->m_pRwClump);
    auto index = RpHAnimIDGetIndex(animHier, boneId);
    m_ParentMatrix = &RpHAnimHierarchyGetMatrixArray(animHier)[index];
}

auto CanAddParticle() {
    switch (g_fx.GetFxQuality()) {
    case FX_QUALITY_LOW:
        return CGeneral::RandomBool(50.0f);
    case FX_QUALITY_MEDIUM:
        return CGeneral::RandomBool(75.0f);
    default:
        return true;
    }
}

// 0x4AA440
void FxSystem_c::AddParticle(CVector* pos, CVector* vel, float timeSince, FxPrtMult_c* fxMults, float rotZ, float lightMult, float lightMultLimit, bool createLocal) {
    if (CanAddParticle()) {
        auto brightness = lightMult < lightMultLimit ? 1.0f - lightMultLimit + lightMult : 1.0f;
        for (auto& prim : GetPrims()) {
            if (prim->m_bEnabled) {
                prim->AddParticle(pos, vel, timeSince, fxMults, rotZ, brightness, createLocal);
            }
        }
    }
}

// unused
// 0x4AA540
void FxSystem_c::AddParticle(RwMatrix* mat, CVector* vel, float timeSince, FxPrtMult_c* fxMults, float rotZ, float lightMult, float lightMultLimit, bool createLocal) {
    if (CanAddParticle()) {
        auto brightness = lightMult < lightMultLimit ? 1.0f - lightMultLimit + lightMult : 1.0f;
        for (auto& prim : GetPrims()) {
            prim->AddParticle(mat, vel, timeSince, fxMults, rotZ, brightness, createLocal);
        }
    }
}

// 0x4AA610
void FxSystem_c::EnablePrim(int32 primIndex, bool enable) {
    m_Prims[primIndex]->Enable(enable);
}

// 0x4AA630
void FxSystem_c::SetMatrix(RwMatrix* matrix) {
    if (m_allocatedParentMat)
        *m_ParentMatrix = *matrix;
    else
        m_ParentMatrix = matrix;
}

// 0x4AA660
void FxSystem_c::SetOffsetPos(const CVector& pos) {
    m_LocalMatrix.pos = *(RwV3d*)&pos;
    RwMatrixUpdate(&m_LocalMatrix);
}

// 0x4AA690
void FxSystem_c::AddOffsetPos(CVector* pos) {
    RwV3dAdd(&m_LocalMatrix.pos, &m_LocalMatrix.pos, pos);
    RwMatrixUpdate(&m_LocalMatrix);
}

// 0x4AA6C0
void FxSystem_c::SetConstTime(bool on, float time) {
    m_UseConstTime = on;
    m_nConstTime   = (uint16)(time * 256.0f);
}

// 0x4AA6F0
void FxSystem_c::SetRateMult(float mult) {
    m_nRateMult = (uint16)(mult * 1000.0f);
}

// 0x4AA710
void FxSystem_c::SetTimeMult(float mult) {
    m_nTimeMult = (uint16)(mult * 1000.0f);
}

// 0x4AA730
void FxSystem_c::SetVelAdd(CVector* velocity) {
    m_VelAdd = *velocity;
}

// 0x4AA910
void FxSystem_c::SetLocalParticles(bool enable) {
    m_createLocal = enable;
}

// 0x4AAC50
void FxSystem_c::SetZTestEnable(bool enable) {
    m_useZTest = enable;
}

// 0x4AAC70
void FxSystem_c::SetMustCreatePrts(bool enable) {
    m_MustCreateParticles = enable;
}

// 0x4AA890
void FxSystem_c::CopyParentMatrix() {
    RwMatrix* old = m_ParentMatrix;
    RwMatrix* allocated = RwMatrixCreate();
    m_ParentMatrix = allocated;
    *allocated = *old;
    m_allocatedParentMat = true;
}

// 0x4AA8C0
void FxSystem_c::GetCompositeMatrix(RwMatrix* out) {
    if (m_ParentMatrix)
        RwMatrixMultiply(out, &m_LocalMatrix, m_ParentMatrix);
    else
        *out = m_LocalMatrix;
}

// 0x4AA900
eFxSystemPlayStatus FxSystem_c::GetPlayStatus() const {
    return m_nPlayStatus;
}

// 0x4AA930
uint32 FxSystem_c::ForAllParticles(void(*callback)(Particle_c*, int32, FxBox_c**), FxBox_c* data) {
    auto count = 0;

    for (auto& prim : m_SystemBP->GetPrims()) {
        for (auto* particle = prim->m_Particles.GetHead(); particle; particle = prim->m_Particles.GetNext(particle)) {
            if (prim->m_Type && this == particle->m_System) {
                callback(particle, 0, &data);
                count++;
            }
        }
    }

    return count;
}

// 0x4AA9A0
void FxSystem_c::UpdateBoundingBoxCB(Particle_c* particle, int32 a2, FxBox_c** data) {
    ((void(__cdecl *)(Particle_c*, int32, FxBox_c**))0x4AA9A0)(particle, a2, data);
}

// 0x4AAA40
void FxSystem_c::GetBoundingBox(FxBox_c* out) {
    out->Reset();

    if (ForAllParticles(UpdateBoundingBoxCB, out) != 0) {
        return;
    }

    auto mat = g_fxMan.FxRwMatrixCreate();
    if (m_ParentMatrix) {
        RwMatrixMultiply(mat, &m_LocalMatrix, m_ParentMatrix);
    } else {
        *mat = m_LocalMatrix;
    }

    out->minX = out->maxX = mat->pos.x;
    out->minY = out->maxY = mat->pos.y;
    out->minZ = out->maxZ = mat->pos.z;
    g_fxMan.FxRwMatrixDestroy(mat);
}

// 0x4AAAD0
bool FxSystem_c::GetBoundingSphereWld(FxSphere_c* out) {
    if (!m_BoundingSphere)
        return false;

    auto mat = g_fxMan.FxRwMatrixCreate();
    GetCompositeMatrix(mat);

    RwV3dTransformPoints(reinterpret_cast<RwV3d*>(out), reinterpret_cast<const RwV3d*>(m_BoundingSphere), 1, mat);
    out->m_fRadius = m_BoundingSphere->m_fRadius;
    g_fxMan.FxRwMatrixDestroy(mat);
    return true;
}

// 0x4AAB50
bool FxSystem_c::GetBoundingSphereLcl(FxSphere_c* out) {
    if (!m_BoundingSphere)
        return false;

    *out = *m_BoundingSphere;
    return true;
}

// 0x4AAB80
void FxSystem_c::SetBoundingSphere(FxSphere_c* sphere) {
    if (sphere) {
        if (!m_BoundingSphere) {
            m_BoundingSphere = new (true) FxSphere_c();
            return;
        }
        m_BoundingSphere = sphere;
    } else if (m_BoundingSphere) {
        CMemoryMgr::Free(m_BoundingSphere);
        m_BoundingSphere = nullptr;
    }
}

// 0x4AABF0
void FxSystem_c::ResetBoundingSphere() {
    if (m_SystemBP->m_BoundingSphere) {
        if (!m_BoundingSphere) {
            m_BoundingSphere = new (true) FxSphere_c();
        }
        m_BoundingSphere = m_SystemBP->m_BoundingSphere;
    } else if (m_BoundingSphere) {
        CMemoryMgr::Free(m_BoundingSphere);
        m_BoundingSphere = nullptr;
    }
}

// 0x4AAC90
void FxSystem_c::DoFxAudio(CVector pos) {
    constexpr struct { const char* hash; eAudioEvents event; } mapping[] = {
        { "fire",           AE_FIRE               },
        { "fire_med",       AE_FIRE_MEDIUM        },
        { "fire_large",     AE_FIRE_LARGE         },
        { "fire_car",       AE_FIRE_CAR           },
        { "fire_bike",      AE_FIRE_BIKE          },
        { "Flame",          AE_FIRE_FLAME         },
        { "molotov_flame",  AE_FIRE_MOLOTOV_FLAME },
        { "water_hydrant",  AE_FIRE_HYDRANT       },
        { "water_fountain", AE_FIRE_HYDRANT       },
        { "water_fnt_tme",  AE_FIRE_HYDRANT       },
        { "smoke_flare",    AE_SMOKE_FLARE        },
        { "teargas",        AE_TEARGAS            },
        { "heli_dust",      AE_HELI_DUST          },
    };
    for (auto& [hash, event] : mapping) {
        if (m_SystemBP->GetNameKey() == CKeyGen::GetUppercaseKey(hash)) {
            m_FireAE.AddAudioEvent(event, pos);
        }
    }
}

// 0x4AAF30
bool FxSystem_c::IsVisible() {
    FxSphere_c sphere;
    if (GetBoundingSphereWld(&sphere)) {
        FxFrustumInfo_c* info = g_fxMan.GetFrustumInfo();
        if (!info->IsCollision(&sphere))
            return false;
    }
    return true;
}

// 0x4AAF70
bool FxSystem_c::Update(RwCamera* camera, float timeDelta) {
    return ((bool(__thiscall *)(FxSystem_c*, RwCamera*, float))0x4AAF70)(this, camera, timeDelta);
}

// NOTSA
std::span<FxPrim_c*> FxSystem_c::GetPrims() {
    return std::span{ m_Prims, m_SystemBP->m_nNumPrims };
}
