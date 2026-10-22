/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "Rope.h"
#include "Ropes.h"

void CRopes::InjectHooks() {
    RH_ScopedClass(CRopes);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x555DC0);
    RH_ScopedInstall(Shutdown, 0x556B10);
    RH_ScopedInstall(Update, 0x558D70);
    RH_ScopedInstall(Render, 0x556AE0);
    RH_ScopedInstall(RegisterRope, 0x556B40);
    RH_ScopedInstall(FindPickupHeight, 0x556760);
    RH_ScopedInstall(FindRope, 0x556000);
    RH_ScopedInstall(FindCoorsAlongRope, 0x555E40);
    RH_ScopedInstall(CreateRopeForSwatPed, 0x558D10);
    RH_ScopedInstall(IsCarriedByRope, 0x555F80);
    RH_ScopedInstall(SetSpeedOfTopNode, 0x555DF0);
}

// 0x555DC0
void CRopes::Init() {
    for (auto& rope : aRopes) {
        rope.m_nType = eRopeType::NONE;
    }
    PlayerControlsCrane = eControlledCrane::NONE;
}

// 0x556B10
void CRopes::Shutdown() {
    for (auto& rope : aRopes) {
        if (rope.m_nType == eRopeType::NONE)
            continue;

        rope.Remove();
    }
}

// 0x558D70
void CRopes::Update() {
    ZoneScoped;

    if (CReplay::Mode == MODE_PLAYBACK)
        return;

    for (auto& rope : aRopes) {
        if (rope.m_nType != eRopeType::NONE)
            rope.Update();
    }
}

// 0x556AE0
void CRopes::Render() {
    ZoneScoped;

    for (auto& rope : aRopes) {
        if (rope.m_nType != eRopeType::NONE)
            rope.Render();
    }
}

// Must be used in loop to make attached to holder
// 0x556B40
bool CRopes::RegisterRope(uint32 ropeID, uint32 ropeType, CVector startPos, bool bExpires, uint8 segmentCount, uint8 flags, CPhysical* holder, uint32 timeExpire) {
    assert(segmentCount < NUM_ROPE_SEGMENTS);

    // 0x556B4C - Already registered, just move it
    if (const auto idx = FindRope(ropeID); idx >= 0) {
        auto& rope = aRopes[idx];
        rope.m_nFlags2 |= 0x1;
        rope.m_nSegments = segmentCount;
        for (auto i = 0u; i <= rope.m_nSegments; i++) {
            rope.m_aSegments[i] = startPos;
            rope.m_aSpeed[i]    = CVector{};
        }
        rope.CreateHookObjectForRope();
        return true;
    }

    // 0x556C11 - Otherwise create a new one
    const auto it = rng::find(aRopes, eRopeType::NONE, &CRope::m_nType);
    if (it == aRopes.end()) {
        return false;
    }
    auto& rope = *it;

    rope.m_nId                = ropeID;
    rope.m_aSegments[0]       = startPos;
    rope.m_aSpeed[0]          = CVector{};
    rope.m_nSegments          = segmentCount;
    rope.m_nFlags2            = (uint8)((rope.m_nFlags2 & ~0x6) | 0x1 | ((flags & 1) << 2));
    rope.m_nFlags1            = 0;
    rope.m_fGroundZ           = 0.f;
    rope.m_pAttachedEntity    = nullptr;
    rope.m_pRopeAttachObject  = nullptr;
    rope.m_fMass              = holder && holder->GetIsTypeVehicle() ? 0.9f : 0.5f;
    rope.m_nType              = (eRopeType)ropeType;
    rope.m_nTime              = bExpires ? CTimer::GetTimeInMS() + timeExpire : 0u;
    rope.m_pRopeHolder        = holder;
    CEntity::SafeRegisterRef(rope.m_pRopeHolder);

    rope.m_fTotalLength = [&] {
        switch (rope.m_nType) {
        case eRopeType::MAGNET:           return 10.f;
        case eRopeType::CRANE_MAGNO:      return 50.f;
        case eRopeType::WRECKING_BALL:
        case eRopeType::QUARRY_CRANE_ARM:
        case eRopeType::CRANE_TROLLEY:    return 68.f;
        default:                          return 20.f;
        }
    }();
    rope.m_fSegmentLength = rope.m_fTotalLength / (float)(NUM_ROPE_SEGMENTS - 1);

    // 0x556CFB - Lay out the segments
    switch (rope.m_nType) {
    case eRopeType::CRANE_MAGNO:
    case eRopeType::WRECKING_BALL:
    case eRopeType::QUARRY_CRANE_ARM:
    case eRopeType::CRANE_TROLLEY: { // Hanging straight down
        for (auto i = 1u; i < NUM_ROPE_SEGMENTS; i++) {
            rope.m_aSegments[i] = rope.m_aSegments[i - 1] - CVector{ 0.f, 0.f, rope.m_fSegmentLength }; // NOTE: Speeds aren't reset here
        }
        break;
    }
    default: { // Zig-zag along the X axis
        for (auto i = 1u; i < NUM_ROPE_SEGMENTS; i++) {
            rope.m_aSegments[i] = rope.m_aSegments[i - 1] + CVector{ i % 2 ? rope.m_fSegmentLength : -rope.m_fSegmentLength, 0.f, 0.f };
            rope.m_aSpeed[i]    = CVector{};
        }
        break;
    }
    }

    rope.CreateHookObjectForRope();
    return true;
}

// 0x556760
float CRopes::FindPickupHeight(CEntity* entity) {
    return CModelInfo::GetModelInfo(entity->m_nModelIndex)->GetColModel()->GetBoundingBox().m_vecMax.z;
}

// Returns id to array
// 0x556000
int32 CRopes::FindRope(uint32 id) {
    for (auto ropeId = 0; ropeId < MAX_NUM_ROPES; ropeId++) {
        if (aRopes[ropeId].m_nType != eRopeType::NONE && aRopes[ropeId].m_nId == id)
            return ropeId;
    }
    return -1;
}

// a4 always nullptr
// 0x555E40
bool CRopes::FindCoorsAlongRope(uint32 ropeId, float fDistAlongRope, CVector* outPosn, CVector* outSpeed) {
    const auto idx = FindRope(ropeId);
    if (idx < 0) {
        return false;
    }
    const auto& rope = aRopes[idx];

    // `fDistAlongRope` is in segments
    const auto segment = (size_t)fDistAlongRope;
    const auto t       = fDistAlongRope - (float)segment;
    assert(segment + 1 < NUM_ROPE_SEGMENTS);

    *outPosn = rope.m_aSegments[segment] * (1.f - t) + rope.m_aSegments[segment + 1] * t;
    if (outSpeed) {
        *outSpeed = rope.m_aSpeed[segment + 1];
    }
    return true;
}

// 0x558D10
int32 CRopes::CreateRopeForSwatPed(const CVector& startPos) {
    int32 newRopeId = m_nRopeIdCreationCounter + 100;
    if (RegisterRope(newRopeId, static_cast<uint32>(eRopeType::SWAT), startPos, true, 0, 0, nullptr, 4000)) {
        return -1;
    }

    m_nRopeIdCreationCounter += 1;
    return newRopeId;
}

// 0x555F80
bool CRopes::IsCarriedByRope(CPhysical* entity) {
    if (!entity)
        return false;

    for (auto& rope : aRopes) {
        if (rope.m_nType != eRopeType::NONE && rope.m_pRopeAttachObject == entity)
            return true;
    }
    return false;
}

// 0x555DF0
void CRopes::SetSpeedOfTopNode(uint32 ropeId, CVector dirSpeed) {
    for (auto& rope : aRopes) {
        if (rope.m_nType != eRopeType::NONE && rope.m_nId == ropeId) {
            rope.m_aSpeed[0] = dirSpeed;
            return;
        }
    }
}
