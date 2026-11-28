/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Rope.h"

// NOTSA
enum class eControlledCrane : int32 {
    NONE,
    MAGNO_CRANE,
    WRECKING_BALL,
    LAS_VEGAS_CRANE,
    QUARRY_CRANE
};

class CRopes {
public:
    static constexpr auto MAX_NUM_ROPES{ 8u };

public:
    static void Init();
    static void Shutdown();
    static void Update();
    static void Render();

    static bool RegisterRope(uint32 ropeId, uint32 ropeType, CVector startPos, bool expires, uint8 segmentCount, uint8 flags, CPhysical* holder, uint32 timeExpire);

    static float FindPickupHeight(CEntity* entity);
    static int32 FindRope(uint32 id);
    static bool  FindCoorsAlongRope(uint32 ropeId, float distAlongRope, CVector* outPos, CVector* outSpeed);

    static int32 CreateRopeForSwatPed(const CVector& startPos);
    static bool  IsCarriedByRope(CPhysical* entity);
    static void  SetSpeedOfTopNode(uint32 ropeId, CVector dirSpeed);

    static CRope& GetRope(int32 index) { return aRopes[index]; }

    static void InjectHooks();

public:
    static inline auto& aRopes              = StaticRef<std::array<CRope, MAX_NUM_ROPES>>(0xB768B8); // Access using CRopes::GetRope()
    static inline auto& PlayerControlsCrane = StaticRef<eControlledCrane>(0xB76898);
    static inline auto& RopeIdCreationCounter = StaticRef<uint32>(0xB781F8);
};
