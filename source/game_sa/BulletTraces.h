#pragma once

#include <extensions/utility.hpp>

#include "Enums/eWeaponType.h"

class CVector;
class CEntity;
class CBulletTrace;

class CBulletTraces {
public:
    static inline auto& aTraces = StaticRef<std::array<CBulletTrace, 16>>(0xC7C748);

public:
    static void InjectHooks();

    static void Init();
    static void Update();
    static void AddTrace(const CVector& from, const CVector& to, float radius, uint32 disappearTime, uint8 alpha);
    static void AddTrace(const CVector& from, const CVector& to, eWeaponType weaponType, CEntity* fromEntity);
    static void Render();

    static CBulletTrace* GetFree(); // Inlined function (Kinda NOTSA)
    static size_t        GetTraceIndex( CBulletTrace* pTrace) { return notsa::array_indexof(aTraces, pTrace); } // NOTSA
};
