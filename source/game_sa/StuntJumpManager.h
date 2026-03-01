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
    static inline CStuntJumpsPool*& mp_poolStuntJumps = *(CStuntJumpsPool**)0xA9A888;
    static inline CStuntJump*&      mp_Active         = *(CStuntJump**)0xA9A88C; // nullptr
    static inline bool&             m_bActive         = *(bool*)0xA9A890;
    static inline bool&             m_bHitReward      = *(bool*)0xA9A891;
    static inline uint32&           m_iTimer          = *(uint32*)0xA9A894;
    static inline eJumpState&       m_jumpState       = *(eJumpState*)0xA9A898;
    static inline int32&            m_iNumJumps       = *(int32*)0xA9A89C;
    static inline uint32&           m_iNumCompleted   = *(uint32*)0xA9A8A0;

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
