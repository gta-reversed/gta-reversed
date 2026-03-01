#pragma once

#include "Enums/eWeaponType.h"

class CVector;
class CEntity;
class CBulletTrace;

class CBulletTraces {
public:
    static CBulletTrace(&aTraces)[16];

public:
    static void InjectHooks();

    static void Init();
    static void Update();
    static void AddTrace(const CVector& from, const CVector& to, float radius, uint32 disappearTime, uint8 alpha);
    static void AddTrace(const CVector& from, const CVector& to, eWeaponType weaponType, CEntity* fromEntity);
    static void Render();

    static CBulletTrace* GetFree(); // Inlined function (Kinda NOTSA)
    static size_t        GetTraceIndex(CBulletTrace* pTrace) { return std::distance(std::begin(aTraces), pTrace); } // NOTSA
};
