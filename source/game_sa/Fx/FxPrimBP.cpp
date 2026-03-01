#include "StdInc.h"

#include "FxPrimBP.h"
#include "FxTools.h"

void FxPrimBP_c::InjectHooks() {
    RH_ScopedClass(FxPrimBP_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(GetRWMatrix, 0x4A9DC0);
    RH_ScopedInstall(Load, 0x5C2010, { .reversed = false });
}

// 0x4A9CF0
FxPrimBP_c::FxPrimBP_c() {
    m_apTextures.fill(nullptr);
}

// 0x4A9D20
FxPrimBP_c::~FxPrimBP_c() {
    // FIX_BUGS: They forgot destroy third texture (texture 3 destroyed twice)
    for (auto& texture : m_apTextures) {
        RwTextureDestroy(texture);
        texture = nullptr;
    }
}

// 0x4A9DC0
void FxPrimBP_c::GetRWMatrix(RwMatrix& outMatrix) {
    if (m_pMatrixBuffered) {
        m_pMatrixBuffered->CopyToRwMatrix(outMatrix);
    } else {
        RwMatrixSetIdentity(&outMatrix);
    }
}

// 0x5C2010
bool FxPrimBP_c::Load(FILESTREAM file, int32 version, FxName32_t* textureNames) {
    return plugin::CallMethodAndReturn<bool, 0x5C2010, FxPrimBP_c*, FILESTREAM, int32, FxName32_t*>(this, file, version, textureNames);

    char line[256], field[128];

    ReadFieldImpl(file, field, "FX_PRIM_BASE_DATA:");
    ReadFieldImpl(file, field, "NAME:");
    
    CVector mat[4];
    ReadLine(file, line, sizeof(line));
    VERIFY(sscanf(
        line,
        "%s %f %f %f %f %f %f %f %f %f %f %f %f",
        field,
        &mat[0].x, &mat[0].y, &mat[0].z, // right
        &mat[1].x, &mat[1].y, &mat[1].z, // up
        &mat[2].x, &mat[2].y, &mat[2].z, // at
        &mat[3].x, &mat[3].y, &mat[3].z  // pos
    ) == 13);

    // if it's identity matrix, don't allocate it
    if (mat[0] == CVector{1.0f, 0.0f, 0.0f} &&
        mat[1] == CVector{0.0f, 1.0f, 0.0f} &&
        mat[2] == CVector{0.0f, 0.0f, 1.0f} &&
        mat[3] == CVector{0.0f, 0.0f, 0.0f}
    ) {
        m_pMatrixBuffered = nullptr;
    } else {
        m_pMatrixBuffered = g_fxMan.Allocate<FxBufferedMatrix>(1);
        assert(m_pMatrixBuffered);

        m_pMatrixBuffered->CopyFromVectors(mat[0], mat[1], mat[2], mat[3]);
    }

    assert(textureNames);
    ReadFieldImpl(file, textureNames[0], "TEXTURE:");

    if (version > 101) {
        ReadFieldImpl(file, textureNames[1], "TEXTURE2:");
        ReadFieldImpl(file, textureNames[2], "TEXTURE3:");
        ReadFieldImpl(file, textureNames[3], "TEXTURE4:");
    }

    m_bAlphaOn    = ReadField<bool>(file, "ALPHAON:");
    m_nSrcBlendId = ReadField<uint8>(file, "SRCBLENDID:");
    m_nDstBlendId = ReadField<uint8>(file, "DSTBLENDID:");

    m_FxInfoManager.Load(file, version);
    return true;
}
