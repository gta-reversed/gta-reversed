#pragma once
#include "extensions/FixedRect.hpp"

class CCopPed;
class CVehicle;

enum eSetPieceType : uint8 {
    /* http://gtaforums.com/topic/194990-gtasa-opcodes/?p=1057374595 */
    SETPIECE_NONE = 0,

    SETPIECE_2CARS_SLOW_SPEED = 1,
    SETPIECE_1CAR_QUICK_SPEED = 2,
    SETPIECE_1CAR_MEDIUM_SPEED = 3,
    SETPIECE_1PED = 4,
    SETPIECE_2PEDS = 5,
    SETPIECE_2CARS_QUICK_SPEED_BEFORE_HIT = 6,
    SETPIECE_2CARS_QUICK_SPEED = 7,
    SETPIECE_2CARS_MEDIUM_SPEED = 8
};

class CSetPiece {
public:
    int32                        m_nLastGenerationTime{};
    FixedRect<int16, 4.0f, true> m_AreaRect;
    FixedVector2D<int16, 4.0f>   m_SpawnCoord1{};
    FixedVector2D<int16, 4.0f>   m_SpawnCoord2{};
    FixedVector2D<int16, 4.0f>   m_TargetCoord1{};
    FixedVector2D<int16, 4.0f>   m_TargetCoord2{};
    eSetPieceType                m_nType{ SETPIECE_NONE };
    char  _pad1D[3];

public:
    static void InjectHooks();

    CSetPiece() = default; // 0x4993C0
    CCopPed* TryToGenerateCopPed(CVector2D posn);
    CVehicle* TryToGenerateCopCar(CVector2D posn, CVector2D target);
    void Update();

    // Inlined shit API
    CVector2D GetSpawnCoord1() const { return m_SpawnCoord1; }
    CVector2D GetSpawnCoord2() const { return m_SpawnCoord2; }
    CVector2D GetTargetCoord1() const { return m_TargetCoord1; }
    CVector2D GetTargetCoord2() const { return m_TargetCoord2; }
    void SetCornerA(CVector2D c) { m_AreaRect.x1 = c.x; m_AreaRect.y1 = c.y; }
    void SetCornerB(CVector2D c) { m_AreaRect.x2 = c.x; m_AreaRect.y2 = c.y; }
    void SetSpawnCoord1(CVector2D c) { m_SpawnCoord1 = c; }
    void SetSpawnCoord2(CVector2D c) { m_SpawnCoord2 = c; }
    void SetTargetCoord1(CVector2D c) { m_TargetCoord1 = c; }
    void SetTargetCoord2(CVector2D c) { m_TargetCoord2 = c; }
    float GetCornerAX() const { return m_AreaRect.x1; }
    float GetCornerAY() const { return m_AreaRect.y1; }
    float GetCornerBX() const { return m_AreaRect.x2; }
    float GetCornerBY() const { return m_AreaRect.y2; }
    float GetSpawnCoord1X() const { return m_SpawnCoord1.x; }
    float GetSpawnCoord1Y() const { return m_SpawnCoord1.y; }
    float GetSpawnCoord2X() const { return m_SpawnCoord2.x; }
    float GetSpawnCoord2Y() const { return m_SpawnCoord2.y; }
    float GetTargetCoord1X() const { return m_TargetCoord1.x; }
    float GetTargetCoord1Y() const { return m_TargetCoord1.y; }
    float GetTargetCoord2X() const { return m_TargetCoord2.x; }
    float GetTargetCoord2Y() const { return m_TargetCoord2.y; }
};
VALIDATE_SIZE(CSetPiece, 0x20);
