#pragma once

#include "Skidmark.h"

constexpr auto SKIDMARKS_COUNT{ 32u };

class CSkidmarks {
public:
    static inline auto& m_aIndices = StaticRef<std::array<RxVertexIndex, 96>>(0xC799C8);
    static inline auto& m_pTexture = StaticRef<RwTexture*>(0xC79A88);
    static inline auto& m_aSkidmarks = StaticRef<CSkidmark[SKIDMARKS_COUNT]>(0xC79AA8);

public:
    static void InjectHooks();

    static void Init();
    static void Shutdown();
    static void Clear();
    static void Update();
    static void Render();
    static void RegisterOne(uint32 index, const CVector& posn, float dirX, float dirY, eSkidmarkType type, bool* bloodState, float length);
    static void RegisterOne(uint32 index, const CVector& posn, float dirX, float dirY, bool* isSandy, bool* bloodState, float length);

    static CSkidmark* FindById(uint32 id);
    static CSkidmark* GetNextFree(bool forceFree);
};
