#include "StdInc.h"

#include "SetPieces.h"

void CSetPieces::InjectHooks() {
    RH_ScopedClass(CSetPieces);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(AddOne, 0x499500);
    RH_ScopedInstall(Init, 0x4994F0);
    RH_ScopedInstall(Load, 0x5D3CA0);
    RH_ScopedInstall(Save, 0x5D3C70);
    RH_ScopedInstall(Update, 0x49AA00);
}

// 0x499500
void CSetPieces::AddOne(eSetPieceType type, CVector2D cornerA, CVector2D cornerB, CVector2D spawnCoord1, CVector2D targetCoord1, CVector2D spawnCoord2, CVector2D targetCoord2) {
    if (NumSetPieces >= MAX_SET_PIECES) {
        return;
    }

    const auto [left, right] = std::minmax(cornerA.x, cornerB.x);
    const auto [bottom, top] = std::minmax(cornerA.y, cornerB.y);

    auto& sp = aSetPieces[NumSetPieces++];
    sp.m_nType        = type;
    sp.m_AreaRect     = CRect{ left, bottom, right, top };
    sp.m_SpawnCoord1  = spawnCoord1;
    sp.m_SpawnCoord2  = spawnCoord2;
    sp.m_TargetCoord1 = targetCoord1;
    sp.m_TargetCoord2 = targetCoord2;
}

// 0x4994F0
void CSetPieces::Init() {
    NumSetPieces = 0;
    bDebug       = false;
}

// 0x5D3CA0
bool CSetPieces::Load() {
    LoadDataFromWorkBuffer(NumSetPieces);
    LoadDataFromWorkBuffer(aSetPieces);

    for (auto& sp : aSetPieces) {
        volatile CRect _{ sp.m_AreaRect };
    }
    return true;
}

// 0x5D3C70
bool CSetPieces::Save() {
    SaveDataToWorkBuffer(NumSetPieces);
    SaveDataToWorkBuffer(aSetPieces);
    return true;
}

// 0x49AA00
void CSetPieces::Update() {
    ZoneScoped;

    const auto f = CTimer::GetFrameCounter() % 8;
    const auto beg = f * NumSetPieces / 8;
    const auto end = (f + 1) * NumSetPieces / 8;

    for (auto i = beg; i < end; i++) {
        aSetPieces[i].Update();
    }
}
