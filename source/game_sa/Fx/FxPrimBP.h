/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"
#include "List_c.h"
#include "FxInfoManager.h"
#include "extensions/FixedVector.hpp"

class FxPrim_c;
class FxPrimBP_c;

// struct FxName32_t {
//    char value[32];
// };
using FxName32_t = char[32];

struct FxBufferedMatrix {
    static constexpr float FX_RECIPROCAL = std::numeric_limits<int16>::max();

    FixedVector<int16, FX_RECIPROCAL> right;
    FixedVector<int16, FX_RECIPROCAL> up;
    FixedVector<int16, FX_RECIPROCAL> at;
    FixedVector<int16, FX_RECIPROCAL> pos;

    void CopyFromRwMatrix(const RwMatrix& mat) {
        right = static_cast<CVector>(mat.right);
        up    = static_cast<CVector>(mat.up);
        at    = static_cast<CVector>(mat.at);
        pos   = static_cast<CVector>(mat.pos);
    }

    void CopyFromVectors(const CVector& r, const CVector& u, const CVector& a, const CVector& p) {
        right = r; up = u; at = a; pos = p;
    }

    void CopyToRwMatrix(RwMatrix& out) {
        out.right = right;
        out.up    = up;
        out.at    = at;
        out.pos   = pos;
    }
};
VALIDATE_SIZE(FxBufferedMatrix, 0x18);

class FxPrimBP_c {
public:
    int8                      m_Type;
    uint8                     m_nSrcBlendId;
    uint8                     m_nDstBlendId;
    bool                      m_bAlphaOn;
    FxBufferedMatrix*         m_pMatrixBuffered; // int16* m_matVals;
    std::array<RwTexture*, 4> m_apTextures;
    int32 m_TxdIndex;
    TList_c<Particle_c>       m_Particles;
    FxInfoManager_c           m_FxInfoManager;

public:
    static void InjectHooks();

    FxPrimBP_c();
    virtual ~FxPrimBP_c() = 0;
    virtual bool Load(FILESTREAM file, int32 version, FxName32_t* textureNames) = 0;
    virtual bool LoadTextures(FxName32_t* textureNames, int32 version) = 0;
    virtual FxPrim_c* CreateInstance() = 0;
    virtual void Update(float a1) = 0;
    virtual void Render(RwCamera* camera, uint32 a2, float a3, bool bCanRenderHeatHaze) = 0;
    virtual bool FreePrtFromPrim(FxSystem_c* system) = 0;

    void GetRWMatrix(RwMatrix& outMatrix);
};

VALIDATE_SIZE(FxPrimBP_c, 0x40);
