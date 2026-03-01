#include "StdInc.h"

#include "Skidmarks.h"

RxVertexIndex (&CSkidmarks::m_aIndices)[96] = *reinterpret_cast<uint16 (*)[96]>(0xC799C8);
RwTexture*& CSkidmarks::m_pTexture = *reinterpret_cast<RwTexture**>(0xC79A88);
CSkidmark (&CSkidmarks::m_aSkidmarks)[SKIDMARKS_COUNT] = *reinterpret_cast<CSkidmark (*)[SKIDMARKS_COUNT]>(0xC79AA8);

void CSkidmarks::InjectHooks() {
    RH_ScopedClass(CSkidmarks);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x7204E0);
    RH_ScopedInstall(Shutdown, 0x720570);
    RH_ScopedInstall(Clear, 0x720590);
    RH_ScopedInstall(Update, 0x7205C0);
    RH_ScopedInstall(Render, 0x720640);
    RH_ScopedOverloadedInstall(RegisterOne, "EC0", 0x720EC0, void (*)(uint32, const CVector&, float, float, bool*, bool*, float));
    RH_ScopedOverloadedInstall(RegisterOne, "", 0x720930, void (*)(uint32, const CVector&, float, float, eSkidmarkType, bool*, float));
}

// 0x7204E0
void CSkidmarks::Init() {
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("particle"));
    m_pTexture = RwTextureRead("particleskid", nullptr);
    CTxdStore::PopCurrentTxd();

    Clear();

    // Pirulax: todo fix
    // Izzotop: Same as original.
    for (unsigned part = 0; part < SKIDMARK_NUM_PARTS; part++) {
        constexpr uint32 indices[] = { 0, 2, 1, 1, 2, 3 };
        for (unsigned v = 0; v < 6; v++) {
            m_aIndices[6 * part + v] = (RxVertexIndex)(2 * part + indices[v]);
        }
    }
}

// 0x720570
void CSkidmarks::Shutdown() {
    RwTextureDestroy(m_pTexture);
    m_pTexture = nullptr;
}

// 0x720590
void CSkidmarks::Clear() {
    for (CSkidmark& mark : m_aSkidmarks) {
        mark.m_nState = eSkidmarkState::INACTIVE;
        mark.m_bActive = false;
    }
}

// 0x7205C0
void CSkidmarks::Update() {
    ZoneScoped;

    for (CSkidmark& mark : m_aSkidmarks) {
        mark.Update();
    }
}

// 0x720640
void CSkidmarks::Render() {
    ZoneScoped;

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(RwTextureGetRaster(m_pTexture)));

    for (CSkidmark& mark : m_aSkidmarks) {
        mark.Render();
    }

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
}

// 0x720930
void CSkidmarks::RegisterOne(uint32 index, const CVector& posn, float dirX, float dirY, eSkidmarkType type, bool* bloodState, float length) {
    if (CReplay::Mode == MODE_PLAYBACK)
        return;

    if (CSkidmark* markById = FindById(index)) {
        markById->RegisterNewPart(posn, { dirX, dirY }, length, bloodState);
    } else if (CSkidmark* mark = GetNextFree(true)) {
        mark->Init(index, posn, type, bloodState);
    }
}

// 0x720EC0
// unused
void CSkidmarks::RegisterOne(uint32 index, const CVector& posn, float dirX, float dirY, bool* isSandy, bool* bloodState, float length) {
    if (bloodState)
        RegisterOne(index, posn, dirX, dirY, eSkidmarkType::BLOODY, bloodState, length);
    else
        RegisterOne(index, posn, dirX, dirY, isSandy ? eSkidmarkType::SANDY : eSkidmarkType::DEFAULT, bloodState, length);
}

// NOTSA
CSkidmark* CSkidmarks::FindById(uint32 id) {
    for (CSkidmark& mark : m_aSkidmarks)
        if (mark.m_nState == eSkidmarkState::JUST_UPDATED && mark.m_nId == id)
            return &mark;
    return nullptr;
}

// Try get an unsued slot/forcefully reuse one that isn't visible
// NOTSA
CSkidmark* CSkidmarks::GetNextFree(bool forceFree) {
    for (CSkidmark& mark : m_aSkidmarks)
        if (mark.m_nState == eSkidmarkState::INACTIVE)
            return &mark;

    if (forceFree) {
        auto mostRecentlyUpdatedMs = std::numeric_limits<uint32>::max();
        CSkidmark* mostRecentlyUpdatedMark{};
        for (CSkidmark& mark : m_aSkidmarks) {
            if (mark.m_bActive)
                continue;
            if (mark.m_lastDisappearTimeUpdateMs >= mostRecentlyUpdatedMs)
                continue;
            if (!TheCamera.IsSphereVisible(mark.GetBoundingSphere())) {
                mostRecentlyUpdatedMs = mark.m_lastDisappearTimeUpdateMs;
                mostRecentlyUpdatedMark = &mark;
            }
        }
        return mostRecentlyUpdatedMark;
    }

    return nullptr;
}
