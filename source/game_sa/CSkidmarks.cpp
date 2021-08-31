#include "StdInc.h"

RxVertexIndex (&CSkidmarks::m_aIndices)[96] = *reinterpret_cast<unsigned short (*)[96]>(0xC799C8);
RwTexture*& CSkidmarks::m_pTexture = *reinterpret_cast<RwTexture**>(0xC79A88);
CSkidmark (&CSkidmarks::m_aSkidmarks)[SKIDMARKS_COUNT] = *reinterpret_cast<CSkidmark (*)[SKIDMARKS_COUNT]>(0xC79AA8);

void CSkidmarks::InjectHooks() {
    ReversibleHooks::Install("CSkidmarks", "Clear", 0x720590, &CSkidmarks::Clear);
    ReversibleHooks::Install("CSkidmarks", "Init", 0x7204E0, &CSkidmarks::Init);
    ReversibleHooks::Install("CSkidmarks", "RegisterOne_iVffbbf", 0x720EC0, (void (*)(unsigned int, const CVector&, float, float, const bool*, bool*, float))(&CSkidmarks::RegisterOne));
    // ReversibleHooks::Install("CSkidmarks", "RegisterOne_iVffEbf", 0x720930, (void (*)(unsigned int, const CVector&, float, float, eSurfaceType, bool*, float))(&CSkidmarks::RegisterOne));
    // ReversibleHooks::Install("CSkidmarks", "Render", 0x720640, &CSkidmarks::Render);
    ReversibleHooks::Install("CSkidmarks", "Shutdown", 0x720570, &CSkidmarks::Shutdown);
    // ReversibleHooks::Install("CSkidmarks", "Update", 0x7205C0, &CSkidmarks::Update);
}

// 0x720590
void CSkidmarks::Clear() {
    for (CSkidmark& mark : m_aSkidmarks) {
        mark.m_nState = 0;
        mark.byte157 = 0;
    }
}

// 0x7204E0
void CSkidmarks::Init() {
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("particle"));
    m_pTexture = RwTextureRead("particleskid", 0);
    CTxdStore::PopCurrentTxd();

    Clear();

    for (unsigned i = 0; i < 16; i++) {
        constexpr unsigned indicesRelative[] = { 0, 2, 1, 1, 2, 3 };
        for (unsigned v = 0; v < 6; v++) {
            m_aIndices[i + v] = (RxVertexIndex)(2 * i + indicesRelative[v]);
        }
    }
}

// 0x720EC0
void CSkidmarks::RegisterOne(unsigned int index, const CVector& posn, float dirX, float dirY, const bool* surfaceType, bool* bloodState, float length) {
    eSurfaceType surface = SURFACE_DEFAULT;
    if (bloodState) {
        CSkidmarks::RegisterOne(index, posn, dirX, dirY, SURFACE_TARMAC_REALLYFUCKED, bloodState, length);
        return;
    }
    if (surfaceType) {
        surface = SURFACE_TARMAC;
    }
    CSkidmarks::RegisterOne(index, posn, dirX, dirY, surface, bloodState, length);
}

// 0x720930
void CSkidmarks::RegisterOne(unsigned int index, const CVector& posn, float dirX, float dirY, eSurfaceType surfaceType, bool* bloodState, float length) {
    plugin::Call<0x720930, unsigned int, const CVector&, float, float, eSurfaceType, bool*, float>(index, posn, dirX, dirY, surfaceType, bloodState, length);
}

// 0x720640
void CSkidmarks::Render() {
    plugin::Call<0x720640>();
}

// 0x720570
void CSkidmarks::Shutdown() {
    RwTextureDestroy(CSkidmarks::m_pTexture);
    CSkidmarks::m_pTexture = nullptr;
}

// 0x7205C0
void CSkidmarks::Update() {
    plugin::Call<0x7205C0>();
}

