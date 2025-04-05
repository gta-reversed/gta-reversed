#include "StdInc.h"

#include "FxSystemBP.h"
#include "FxPrimBP.h"
#include "FxEmitterBP.h"
#include "FxInfo.h"

#include "FxTools.h"

void FxSystemBP_c::InjectHooks() {
    RH_ScopedClass(FxSystemBP_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(operator new, 0x4AA100);
    RH_ScopedInstall(Constructor, 0x4AA0D0);
    RH_ScopedInstall(Destructor, 0x4AA0F0);
    RH_ScopedInstall(Load, 0x5C05F0);
    RH_ScopedInstall(Unload, 0x4AA120);
    RH_ScopedInstall(Update, 0x4AA130);
    RH_ScopedInstall(Render, 0x4AA160);
    RH_ScopedInstall(FreePrtFromSystem, 0x4AA1B0);
    RH_ScopedInstall(GetBoundingSphere, 0x4AA1F0);
    RH_ScopedInstall(SetBoundingSphere, 0x4AA200);
}

// 0x4AA100
void* FxSystemBP_c::operator new(unsigned size) {
    return g_fxMan.GetMemPool().GetMem(size);
}

// NOTSA
void FxSystemBP_c::operator delete(void* data) {
    CMemoryMgr::Free(data);
}

// 0x4AA0D0
FxSystemBP_c::FxSystemBP_c() : ListItem_c() {
    m_BoundingSphere = nullptr;
}

FxSystemBP_c* FxSystemBP_c::Constructor() {
    this->FxSystemBP_c::FxSystemBP_c();
    return this;
}

FxSystemBP_c* FxSystemBP_c::Destructor() {
    this->FxSystemBP_c::~FxSystemBP_c();
    return this;
}

// 0x5C05F0
void FxSystemBP_c::Load(Const char* filename, FILESTREAM file, int32 version) {
    assert(file);

    char line[256], field[32];

    ReadField<void>(file);

    if (version > 100) {
        char fname[128];
        ReadFieldImpl(file, fname, "FILENAME:");
    }

    char name[32];
    ReadFieldImpl(file, name, "NAME:");
    m_nNameKey = CKeyGen::GetUppercaseKey(name);

    m_fLength = ReadField<float>(file, "LENGTH:");

    if (version < 106) {
        m_fLoopIntervalMin = 0.0f;
        m_fLoopLength      = 0.0f;
    } else {
        m_fLoopIntervalMin = ReadField<float>(file);
        m_fLoopLength      = ReadField<float>(file);
    }

    m_nPlayMode = ReadField<uint8_t>(file, "PLAYMODE:");
    m_nCullDist = uint16(ReadField<float>(file, "CULLDIST:") * 256.0f);

    if (version > 103) {
        CVector vecCenter;
        float   fRadius;
        ReadLine(file, line, sizeof(line));
        RET_IGNORED(sscanf(line, "%s %f %f %f %f", field, &vecCenter.x, &vecCenter.y, &vecCenter.z, &fRadius));
        m_BoundingSphere = nullptr;
        SetBoundingSphere(vecCenter, fRadius);
    }

    m_nNumPrims = ReadField<uint8>(file, "NUM_PRIMS:");
    m_Prims = g_fxMan.Allocate<FxPrimBP_c*>(m_nNumPrims);

    FxName32_t aTextureNames[8][4];
    assert(m_nNumPrims < std::size(aTextureNames));

    for (auto i = 0; i < m_nNumPrims; i++) {
        ReadLine(file, line, sizeof(line));
        RET_IGNORED(sscanf(line, "%s", field));
        if (strcmp(field, "FX_PRIM_EMITTER_DATA:") != 0)
            continue;

        auto* emitter = new FxEmitterBP_c();
        assert(emitter);

        emitter->m_bHasInfoFlatData = false;
        emitter->m_bHasInfoHeatHazeData = false;

        ReadField<void>(file);

        emitter->Load(file, version, aTextureNames[i]);
        emitter->m_bHasInfoFlatData = emitter->IsFxInfoPresent(FX_INFO_FLAT_DATA);
        emitter->m_bHasInfoHeatHazeData = emitter->IsFxInfoPresent(FX_INFO_HEATHAZE_DATA);
        m_Prims[i] = emitter;
    }

    if (version >= 108) {
        ReadField<bool>(file, "OMITTEXTURES:"); // unused
    }

    if (version >= 109) {
        char txd[128];
        ReadFieldImpl(file, txd, "TXDNAME:"); // unused
    }

    for (auto i = 0; i < m_nNumPrims; i++) {
        m_Prims[i]->LoadTextures(&aTextureNames[i][0], version);
    }
}

// 0x4AA120
void FxSystemBP_c::Unload() {
    // NOP
}

// 0x4AA130
void FxSystemBP_c::Update(float arg0) {
    for (auto& prim : GetPrims()) {
        prim->Update(arg0);
    }
}

// 0x4AA160
void FxSystemBP_c::Render(RwCamera* camera, float dayNightBalance, bool bHeatHaze) {
    for (auto& prim : GetPrims()) {
        prim->Render(camera, 0, dayNightBalance, bHeatHaze);
    }
}

// 0x4AA1B0
bool FxSystemBP_c::FreePrtFromSystem(FxSystem_c* system) {
    if (m_nNumPrims <= 0)
        return false;

    for (auto& prim : GetPrims()) {
        if (prim->FreePrtFromPrim(system)) {
            return true;
        }
    }
    return false;
}

// 0x4AA1F0
FxSphere_c* FxSystemBP_c::GetBoundingSphere() const {
    return m_BoundingSphere;
}

// 0x4AA200
void FxSystemBP_c::SetBoundingSphere(const CVector& center, float radius) {
    if (!m_BoundingSphere && radius > 0.0f) {
        m_BoundingSphere = new (false) FxSphere_c();
        m_BoundingSphere->m_vecCenter = center;
        m_BoundingSphere->m_fRadius   = radius;
    }
}
