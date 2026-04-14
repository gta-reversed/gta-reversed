#pragma once

#include "Vector.h"
#include "Pool.h"
#include "BoundingBox.h"

enum class eJumpState : uint8 {
    START_POINT_INTERSECTED,
    IN_FLIGHT,
    END_POINT_INTERSECTED,
};

struct CStuntJump {
    CBoundingBox start;
    CBoundingBox end;
    CVector      camera;
    int32        reward;
    bool         done;
    bool         found;
};

VALIDATE_SIZE(CStuntJump, 0x44);

typedef CPool<CStuntJump> CStuntJumpsPool;

class CStuntJumpManager {
public:
    static inline auto& mp_poolStuntJumps = StaticRef<CStuntJumpsPool*>(0xA9A888);
    static inline auto& mp_Active = StaticRef<CStuntJump*>(0xA9A88C); // nullptr
    static inline auto& m_bActive = StaticRef<bool>(0xA9A890);
    static inline auto& m_bHitReward = StaticRef<bool>(0xA9A891);
    static inline auto& m_iTimer = StaticRef<uint32>(0xA9A894);
    static inline auto& m_jumpState = StaticRef<eJumpState>(0xA9A898);
    static inline auto& m_iNumJumps = StaticRef<int32>(0xA9A89C);
    static inline auto& m_iNumCompleted = StaticRef<uint32>(0xA9A8A0);

public:
    static void InjectHooks();

    static void Init();
    static void Shutdown();
    static void ShutdownForRestart();
    static bool Save();
    static bool Load();
    static void AddOne(const CBoundingBox& start, const CBoundingBox& end, const CVector& cameraPosn, int32 reward);
    static void Update();

    static void SetActive(bool active);
    static void Render();
};

extern void StuntJumpTestCode();
