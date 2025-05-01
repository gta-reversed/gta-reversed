/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"
#include "List_c.h"

#include "FxFrustumInfo.h"
#include "FxMemoryPool.h"
#include "FxSystemBP.h"
#include "FxSystem.h"

class FxEmitterPrt_c;

static constexpr auto FX_MANAGER_NUM_EMITTERS = 1000;

class FxManager_c {
public:
    TList_c<FxSystemBP_c>    m_FxSystemBPs;
    TList_c<FxSystem_c>      m_FxSystems;
    FxEmitterPrt_c*          m_FxEmitters;
    TList_c<Particle_c>      m_FxEmitterParticles;
    int32                    m_nFxTxdIndex;
    CVector*                 m_pWindDir;
    float*                   m_pfWindSpeed;
    FxFrustumInfo_c          m_Frustum;
    uint32                   m_nCurrentMatrix;
    std::array<RwMatrix*, 8> m_apMatrices;
    FxMemoryPool_c           m_Pool;
    bool                     m_bHeatHazeEnabled;

public:
    static void InjectHooks();

    FxManager_c();
    ~FxManager_c() = default; // 0x4A90A0
    FxManager_c* Constructor();
    FxManager_c* Destructor();

    bool Init();
    void Exit();
    void DestroyFxSystem(FxSystem_c* system);
    void DestroyAllFxSystems();

    bool LoadFxProject(const char* path);
    void UnloadFxProject();

    void LoadFxSystemBP(Const char* filename, FILESTREAM file);
    FxSystemBP_c* FindFxSystemBP(const char* name);

    FxFrustumInfo_c* GetFrustumInfo() { return &m_Frustum; } // 0x4A9130
    void CalcFrustumInfo(RwCamera* camera);

    void ReturnParticle(FxEmitterPrt_c* emitter);
    Particle_c* GetParticle(int8 primType);
    void FreeUpParticle();

    void Update(RwCamera* camera, float timeDelta);
    void Render(RwCamera* camera, bool bHeatHaze);

    void SetWindData(CVector* dir, float* speed);

    RwMatrix* FxRwMatrixCreate();
    void FxRwMatrixDestroy(RwMatrix* matrix);

    bool ShouldCreate(FxSystemBP_c* system, const RwMatrix& transform, RwMatrix* objectMatrix, bool ignoreBoundingChecks = false);
    FxSystem_c* CreateFxSystem(Const char* name, const RwMatrix& transform, RwMatrix* objectMatrix, bool ignoreBoundingChecks = false);
    FxSystem_c* CreateFxSystem(Const char* name, const CVector& point, RwMatrix* objectMatrix, bool ignoreBoundingChecks = false);
    FxSystem_c* CreateFxSystem(FxSystemBP_c* systemBP, const CVector& point, RwMatrix* objectMatrix, bool ignoreBoundingChecks = false);
    FxSystem_c* CreateFxSystem(FxSystemBP_c* systemBP, const RwMatrix& transform, RwMatrix* objectMatrix, bool ignoreBoundingChecks = false);

    FxMemoryPool_c& GetMemPool() { return m_Pool; }

    template <typename Type>
    Type* Allocate(int32 count) {
        const auto size = sizeof(Type) * count;
        const auto align = std::min(sizeof(Type), sizeof(int32));
        return (Type*)GetMemPool().GetMem(size, align);
    }
};

VALIDATE_SIZE(FxManager_c, 0xBC);

extern FxManager_c& g_fxMan;
