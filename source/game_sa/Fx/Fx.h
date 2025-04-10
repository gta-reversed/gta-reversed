/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"
#include "FxManager.h"

class CVehicle;

enum FxQuality_e : uint32 {
    FX_QUALITY_LOW = 0,
    FX_QUALITY_MEDIUM = 1,
    FX_QUALITY_HIGH = 2,
    FX_QUALITY_VERY_HIGH = 3,
};

enum eSparkType : uint8 {
    SPARK_PARTICLE_SPARK2 = 0,
    SPARK_PARTICLE_SPARK  = 1
};

// original name unknown
struct FxEntitySystem : public ListItem_c<FxEntitySystem> {
    FxSystem_c* m_System;
    CEntity*    m_Entity;
};

class Fx_c {
public:
    FxSystem_c*             m_Blood;
    FxSystem_c*             m_BoatSplash;
    FxSystem_c*             m_Bubble;
    FxSystem_c*             m_Cardebris;
    FxSystem_c*             m_CollisionSmoke;
    FxSystem_c*             m_GunShell;
    FxSystem_c*             m_Sand;
    FxSystem_c*             m_Sand2;
    FxSystem_c*             m_SmokeHuge;
    FxSystem_c*             m_SmokeII3expand;
    FxSystem_c*             m_Spark;
    FxSystem_c*             m_Spark2;
    FxSystem_c*             m_Splash;
    FxSystem_c*             m_Wake;
    FxSystem_c*             m_WaterSplash;
    FxSystem_c*             m_WheelDirt;
    FxSystem_c*             m_Glass;
    TList_c<FxEntitySystem> m_FxEntities;
    uint32                  m_Randomizer;
    FxQuality_e             m_FxQuality;
    uint32                  m_nVerticesCount2;
    uint32                  m_nVerticesCount;
    uint32                  m_nTransformRenderFlags;
    RwRaster*               m_pRasterToRender;
    RwMatrix*               m_pTransformLTM;
    RxObjSpace3DVertex*     m_pVerts;

public:
    static void InjectHooks();
    Fx_c* Constructor();
    Fx_c* Destructor();

    Fx_c() = default; // 0x49E620
    ~Fx_c() = default; // 0x49E630

    void InitStaticSystems();
    void ExitStaticSystems();

    void InitEntitySystems();
    void ExitEntitySystems();

    void Init();
    void Exit();
    void Reset();

    void CreateEntityFx(CEntity* entity, const char* fxName, const CVector& pos, RwMatrix* transform);
    void DestroyEntityFx(CEntity* entity);

    void Update(RwCamera* camera, float timeDelta);
    void Render(RwCamera* camera, bool heatHaze);
    void CreateMatFromVec(RwMatrix* out, const CVector* origin, const CVector* direction);
    void SetFxQuality(FxQuality_e quality);
    [[nodiscard]] FxQuality_e GetFxQuality() const;

    void AddBlood(const CVector& origin, const CVector& direction, int32 amount, float arg3);
    void AddWood(CVector& origin, CVector& direction, int32 amount, float arg3);
    void AddSparks(const CVector& origin, const CVector& direction, float force, int32 amount, CVector across, eSparkType sparksType, float spread, float life);
    void AddTyreBurst(const CVector& posn, const CVector& velocity);
    void AddBulletImpact(const CVector& posn, const CVector& direction, int32 bulletFxType, int32 amount, float arg4);
    void AddPunchImpact(CVector& posn, CVector& velocity, int32 arg2);
    void AddDebris(CVector& posn, RwRGBA& color, float scale, int32 amount);
    void AddGlass(CVector& posn, RwRGBA& color, float scale, int32 amount);
    void AddWheelSpray(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, bool bInWater, float lightMult);
    void AddWheelGrass(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult);
    void AddWheelGravel(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult);
    void AddWheelMud(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult);
    void AddWheelSand(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult);
    void AddWheelDust(CVehicle* vehicle, CVector pos, bool bWheelsSpinning, float lightMult);
    void TriggerWaterHydrant(CVector& posn);
    void TriggerGunshot(CEntity* entity, const CVector& origin, const CVector& target, bool doGunflash);
    void TriggerTankFire(CVector& origin, CVector& target);
    void TriggerWaterSplash(CVector& posn);
    void TriggerBulletSplash(CVector& posn);
    void TriggerFootSplash(CVector& posn);
};
VALIDATE_SIZE(Fx_c, 0x70);

void RenderBegin(RwRaster* raster, RwMatrix* transform, uint32 transformRenderFlags);
void RenderAddTri_(
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3,
    float u1, float v1,
    float u2, float v2,
    float u3, float v3,
    int32 r1, int32 g1, int32 b1, int32 a1,
    int32 r2, int32 g2, int32 b2, int32 a2,
    int32 r3, int32 g3, int32 b3, int32 a3
);

void RenderAddTri(
    CVector pos1, CVector pos2, CVector pos3,
    RwTexCoords coord1, RwTexCoords coord2, RwTexCoords coord3,
    const CRGBA& color1, const CRGBA& color2, const CRGBA& color3
);
void RenderEnd();
void RotateVecIntoVec(RwV3d* vecRes, RwV3d* vec, RwV3d* vecAlign);
void RotateVecAboutVec(RwV3d* vecRes, RwV3d* vec, RwV3d* axis, float angle);

extern Fx_c& g_fx;
