/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "FxManager.h"
#include "FxEmitterPrt.h"
#include "CustomBuildingDNPipeline.h"
#include "FxTools.h"
#include "FxSystem.h"
#include "FxSystemBP.h"
#include "FxPrimBP.h"
#include "Particle.h"

FxManager_c& g_fxMan = *(FxManager_c*)0xA9AE80;

void FxManager_c::InjectHooks() {
    RH_ScopedClass(FxManager_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x4A9470);
    RH_ScopedInstall(Destructor, 0x4A90A0);
    RH_ScopedInstall(Init, 0x4A98E0);
    RH_ScopedInstall(Exit, 0x4A9A10);
    RH_ScopedInstall(DestroyFxSystem, 0x4A9810);
    RH_ScopedInstall(DestroyAllFxSystems, 0x4A98B0, {.reversed = false}); // <-- broken for some reason?
    RH_ScopedInstall(Update, 0x4A9A80);
    RH_ScopedInstall(LoadFxProject, 0x5C2420);
    RH_ScopedInstall(UnloadFxProject, 0x4A9AE0);
    RH_ScopedInstall(LoadFxSystemBP, 0x5C1F50);
    RH_ScopedInstall(FindFxSystemBP, 0x4A9360);
    RH_ScopedInstall(GetFrustumInfo, 0x4A9130);
    RH_ScopedInstall(CalcFrustumInfo, 0x4A9140);
    RH_ScopedInstall(Render, 0x4A92A0);
    RH_ScopedInstall(ReturnParticle, 0x4A93B0);
    RH_ScopedInstall(GetParticle, 0x4A93C0);
    RH_ScopedInstall(FreeUpParticle, 0x4A9400);
    RH_ScopedInstall(SetWindData, 0x4A93E0);
    RH_ScopedInstall(FxRwMatrixCreate, 0x4A9440);
    RH_ScopedInstall(FxRwMatrixDestroy, 0x4A9460);
    RH_ScopedInstall(ShouldCreate, 0x4A9500);
    RH_ScopedOverloadedInstall(CreateFxSystem, "2", 0x4A9BB0, FxSystem_c* (FxManager_c::*)(const char*, const RwMatrix&, RwMatrix*, bool));
    RH_ScopedOverloadedInstall(CreateFxSystem, "1", 0x4A95C0, FxSystem_c* (FxManager_c::*)(FxSystemBP_c*, const RwMatrix&, RwMatrix*, bool));
    RH_ScopedOverloadedInstall(CreateFxSystem, "4", 0x4A9BE0, FxSystem_c* (FxManager_c::*)(const char*, const CVector&, RwMatrix*, bool));
    RH_ScopedOverloadedInstall(CreateFxSystem, "3", 0x4A96B0, FxSystem_c* (FxManager_c::*)(FxSystemBP_c*, const CVector&, RwMatrix*, bool));
}

// 0x4A9470
FxManager_c::FxManager_c() {
    m_FxEmitters = nullptr;
}

FxManager_c* FxManager_c::Constructor() {
    this->FxManager_c::FxManager_c();
    return this;
}

FxManager_c* FxManager_c::Destructor() {
    this->FxManager_c::~FxManager_c();
    return this;
}

// 0x4A98E0
bool FxManager_c::Init() {
    m_Pool.Init();

    m_nCurrentMatrix = 0;
    m_apMatrices.fill(RwMatrixCreate());

    m_FxEmitters = new FxEmitterPrt_c[FX_MANAGER_NUM_EMITTERS];
    for (auto i = 0; i < FX_MANAGER_NUM_EMITTERS; i++) {
        m_FxEmitterParticles.AddItem(&m_FxEmitters[i]);
    }

    return true;
}

// 0x4A9A10
void FxManager_c::Exit() {
    DestroyAllFxSystems();
    m_FxSystemBPs.RemoveAll();
    m_FxEmitters = nullptr;
    m_FxEmitterParticles.RemoveAll();
    CTxdStore::RemoveTxdSlot(m_nFxTxdIndex);
    std::ranges::for_each(m_apMatrices, RwMatrixDestroy);
    m_Pool.Exit();
}

// 0x4A9810
void FxManager_c::DestroyFxSystem(FxSystem_c* system) {
    assert(system);
    assert(system->m_SystemBP);

    for (auto i = 0; i < system->m_SystemBP->m_nNumPrims; i++) {
        auto& prim      = system->m_SystemBP->m_Prims[i];
        auto& particles = prim->m_Particles;
        for (Particle_c* it = particles.GetHead(); it;) {
            auto* const prt = it;
            it = particles.GetNext(it); // Iterator will be invalidated, so get next here immediately

            if (prt->m_System == system) {
                prim->m_Particles.RemoveItem(prt);
                m_FxEmitterParticles.AddItem(prt);
            }
        }
    }

    m_FxSystems.RemoveItem(system);
    system->Exit();
    delete system;
}

// 0x4A98B0
void FxManager_c::DestroyAllFxSystems() {
    for (FxSystem_c* it = m_FxSystems.GetHead(); it; it = m_FxSystems.GetNext(it)) {
        DestroyFxSystem(it);
    }
}

// 0x5C2420
bool FxManager_c::LoadFxProject(const char* path) {
    char txdPath[256]; // to be set to "models\\effectsPC.txd"
    strcpy(txdPath, path);
    strcpy(&txdPath[strlen(path) - strlen(".fxp")], "PC.txd");

    // NOTSA: sanity check
    if (!fs::exists(txdPath)) {
        NOTSA_LOG_ERR("Fx TXD file '{}' doesn't exists. Game will crash after trying to render a fx!", txdPath);
    }

    m_nFxTxdIndex = CTxdStore::AddTxdSlot("fx");
    CTxdStore::LoadTxd(m_nFxTxdIndex, txdPath);
    CTxdStore::AddRef(m_nFxTxdIndex);
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(m_nFxTxdIndex);

    auto* file = CFileMgr::OpenFile(path, "r");
    if (!file)
        return false;

    char line[256], buffer[128];

    ReadField<void>(file, "FX_PROJECT_DATA:"); // NOTSA

    while (true) {
        ReadField<void>(file);
        ReadLine(file, line, sizeof(line));
        RET_IGNORED(sscanf(line, "%s", buffer));
        if (strncmp(buffer, "FX_SYSTEM_DATA:", 16u))
            break;

        LoadFxSystemBP(path, file);
    }

    CFileMgr::CloseFile(file);
    CTxdStore::PopCurrentTxd();
    GetMemPool().Optimise();
    return true;
}

// 0x4A9AE0
void FxManager_c::UnloadFxProject() {
    // ((void(__thiscall*)(FxManager_c*))0x4A9AE0)(this);

    DestroyAllFxSystems();
    m_FxSystemBPs.RemoveAll();
    GetMemPool().Reset();
    m_FxEmitterParticles.RemoveAll();

    m_FxEmitters = new FxEmitterPrt_c[FX_MANAGER_NUM_EMITTERS];
    for (auto i = 0; i < FX_MANAGER_NUM_EMITTERS; i++) {
        m_FxEmitterParticles.AddItem(&m_FxEmitters[i]);
    }
}

// 0x5C1F50
void FxManager_c::LoadFxSystemBP(Const char* filename, FILESTREAM file) {
    // return ((FxSystemBP_c * (__thiscall*)(FxManager_c*, char*, FILESTREAM))0x5C1F50)(this, filename, file);

    int32 version;
    char line[256];
    ReadLine(file, line, sizeof(line));
    (void)sscanf(line, "%d", &version);

    auto system = new FxSystemBP_c();
    system->Load(filename, file, version);
    m_FxSystemBPs.AddItem(system);
}

// 0x4A9360
FxSystemBP_c* FxManager_c::FindFxSystemBP(const char* name) {
    // return ((FxSystemBP_c * (__thiscall*)(FxManager_c*, const char*))0x4A9360)(this, name);

    const auto key = CKeyGen::GetUppercaseKey(name);
    for (FxSystemBP_c* it = m_FxSystemBPs.GetHead(); it; it = m_FxSystemBPs.GetNext(it)) {
        if (it->m_nNameKey == key) {
            return it;
        }
    }

    NOTSA_LOG_ERR("Couldn't find Fx system bp '{}'", name);
    return nullptr;
}

// 0x4A9140
void FxManager_c::CalcFrustumInfo(RwCamera* camera) {
    const auto* matrix     = RwFrameGetMatrix(RwCameraGetFrame(camera));
    const auto* viewWindow = RwCameraGetViewWindow(camera);
    const auto farClip     = RwCameraGetFarClipPlane(camera);

    const auto dist  = RwV2dLength(viewWindow);
    const auto angle = RadiansToDegrees(std::atan2(dist, 1.0f));
    const auto radius = std::sqrt(sq(dist) + 1.0f) * farClip / std::sin(DegreesToRadians(180.0f - 2.0f * angle)) * std::sin(DegreesToRadians(angle));

    m_Frustum.m_Sphere = {CVector{matrix->pos} + CVector{matrix->at} * radius, radius};

    m_Frustum.m_Planes[0] = camera->frustumPlanes[2].plane;
    m_Frustum.m_Planes[1] = camera->frustumPlanes[3].plane;
    m_Frustum.m_Planes[2] = camera->frustumPlanes[4].plane;
    m_Frustum.m_Planes[3] = camera->frustumPlanes[5].plane;
}

// 0x4A9A80
void FxManager_c::Update(RwCamera* camera, float timeDelta) {
    // ((void(__thiscall*)(FxManager_c*, RwCamera*, float))0x4A9A80)(this, camera, timeDelta);

    assert(camera);
    CalcFrustumInfo(camera);

    for (FxSystemBP_c* it = m_FxSystemBPs.GetHead(); it; it = m_FxSystemBPs.GetNext(it)) {
        it->Update(timeDelta);
    }

    for (FxSystem_c* it = m_FxSystems.GetHead(); it; it = m_FxSystems.GetNext(it)) {
        if (it->Update(camera, timeDelta)) {
            DestroyFxSystem(it);
        }
    }
}

// 0x4A92A0
void FxManager_c::Render(RwCamera* camera, bool bHeatHaze) {
    // ((void(__thiscall*)(FxManager_c*, RwCamera*, uint8))0x4A92A0)(this, camera, bHeatHaze);

    auto balance = 1.0f - CCustomBuildingDNPipeline::m_fDNBalanceParam;
    m_bHeatHazeEnabled = false;
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(0));
    for (FxSystemBP_c* it = m_FxSystemBPs.GetHead(); it; it = m_FxSystemBPs.GetNext(it)) {
        it->Render(camera, balance * 0.6f + 0.4f, bHeatHaze);
    }
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(TRUE));
}

// 0x4A93B0
void FxManager_c::ReturnParticle(FxEmitterPrt_c* emitter) {
    m_FxEmitterParticles.AddItem(emitter);
}

// 0x4A93C0 -- has no xref with primType != 0.
Particle_c* FxManager_c::GetParticle(int8 primType) {
    assert(!primType);

    return !primType ? m_FxEmitterParticles.RemoveHead() : nullptr;
}

// 0x4A9400
void FxManager_c::FreeUpParticle() {
    // ((void(__thiscall*)(FxManager_c*))0x4A9400)(this);

    FxSystem_c* system;
    do {
        do {
            auto numItems = m_FxSystems.GetNumItems();
            system = m_FxSystems.GetItemOffset(true, CGeneral::GetRandomNumber() % numItems);
        } while (system->m_MustCreateParticles);
    } while (!system->m_SystemBP->FreePrtFromSystem(system));
}

// 0x4A93E0
void FxManager_c::SetWindData(CVector* dir, float* speed) {
    m_pWindDir    = dir;
    m_pfWindSpeed = speed;
}

// 0x4A9440
RwMatrix* FxManager_c::FxRwMatrixCreate() {
    return m_apMatrices[m_nCurrentMatrix++];
}

// 0x4A9460
void FxManager_c::FxRwMatrixDestroy(RwMatrix* matrix) {
    m_nCurrentMatrix--;
}

// 0x4A9BB0
FxSystem_c* FxManager_c::CreateFxSystem(const char* name, const RwMatrix& transform, RwMatrix* objectMatrix, bool ignoreBoundingChecks) {
    FxSystemBP_c* systemBP = FindFxSystemBP(name);
    assert(systemBP);
    return CreateFxSystem(systemBP, transform, objectMatrix, ignoreBoundingChecks);
}

// 0x4A9BE0
FxSystem_c* FxManager_c::CreateFxSystem(Const char* name, const CVector& point, RwMatrix* objectMatrix, bool ignoreBoundingChecks) {
    FxSystemBP_c* systemBP = FindFxSystemBP(name);
    assert(systemBP);
    return CreateFxSystem(systemBP, point, objectMatrix, ignoreBoundingChecks);
}

// 0x4A96B0
FxSystem_c* FxManager_c::CreateFxSystem(FxSystemBP_c* systemBP, const CVector& point, RwMatrix* objectMatrix, bool ignoreBoundingChecks) {
    if (systemBP) {
        auto* pointMatrix = FxRwMatrixCreate();
        RwMatrixSetIdentity(pointMatrix);
        RwV3dAssign(RwMatrixGetPos(pointMatrix), &point);
        RwMatrixUpdate(pointMatrix);

        FxSystem_c* system = CreateFxSystem(systemBP, *pointMatrix, objectMatrix, ignoreBoundingChecks);

        FxRwMatrixDestroy(pointMatrix);
        return system;
    }
    return nullptr;
}

// 0x4A9500
bool FxManager_c::ShouldCreate(FxSystemBP_c* system, const RwMatrix& transform, RwMatrix* objectMatrix, bool ignoreBoundingChecks) {
    if (ignoreBoundingChecks)
        return true;

    if (!system->m_BoundingSphere)
        return true;

    auto* curr = FxRwMatrixCreate();
    if (objectMatrix)
        RwMatrixMultiply(curr, &transform, objectMatrix);
    else
        *curr = transform;

    FxSphere_c pointsOut;
    RwV3dTransformPoints(&pointsOut.m_vecCenter, reinterpret_cast<const RwV3d*>(system->m_BoundingSphere), 1, curr);
    pointsOut.m_fRadius = system->m_BoundingSphere->m_fRadius;
    FxRwMatrixDestroy(curr);
    return GetFrustumInfo()->IsCollision(&pointsOut) != 0;
}

// 0x4A95C0
FxSystem_c* FxManager_c::CreateFxSystem(FxSystemBP_c* systemBP, const RwMatrix& transform, RwMatrix* objectMatrix, bool ignoreBoundingChecks) {
    if (systemBP && ShouldCreate(systemBP, transform, objectMatrix, ignoreBoundingChecks)) {
        auto* fx = new FxSystem_c();
        fx->Init(systemBP, transform, objectMatrix);
        m_FxSystems.AddItem(fx);

        const auto quality = g_fx.GetFxQuality();
        switch (quality) {
        case FX_QUALITY_LOW:
            fx->SetRateMult(0.5f);
            break;
        case FX_QUALITY_MEDIUM:
            fx->SetRateMult(0.75f);
            break;
        case FX_QUALITY_HIGH:
        case FX_QUALITY_VERY_HIGH:
            fx->SetRateMult(1.0f);
            break;
        };

        return fx;
    }
    return nullptr;
}
