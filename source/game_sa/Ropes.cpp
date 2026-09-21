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
// Binary verb (DIS_556B40.txt, 402 insns): re-registering an existing rope id only
// rewrites its segments/speeds; a fresh rope fills the type/length/holder/expire
// fields and lays out all 32 segments. `segmentCount` seeds m_nSegments, `flags`
// bit 0 lands in m_nFlags1 bit 2 (AND-masked), and odd/even segment pairs zigzag
// by m_fSegmentLength in x.
bool CRopes::RegisterRope(uint32 ropeID, uint32 ropeType, CVector startPos, bool bExpires, uint8 segmentCount, uint8 flags, CPhysical* holder, uint32 timeExpire) {
    for (auto i = 0u; i < MAX_NUM_ROPES; i++) {
        auto& rope = GetRope(i);
        if (rope.m_nType == eRopeType::NONE || rope.m_nId != ropeID)
            continue;
        rope.m_aSegments[0] = startPos;
        rope.m_aSpeed[0] = CVector{};
        rope.m_nFlags1 |= 1;
        rope.m_nSegments = segmentCount;
        for (auto s = 0u; s <= segmentCount; s++) {
            rope.m_aSegments[s + 1] = startPos;
            rope.m_aSpeed[s + 1] = CVector{};
        }
        rope.CreateHookObjectForRope();
        return true;
    }
    auto* rope = [&]() -> CRope* {
        for (auto& r : aRopes) {
            if (r.m_nType == eRopeType::NONE)
                return &r;
        }
        return nullptr;
    }();
    if (!rope)
        return false;
    rope->m_nId = ropeID;
    rope->m_aSegments[0] = startPos;
    rope->m_aSpeed[0] = CVector{};
    rope->m_nSegments = segmentCount;
    rope->m_nFlags1 = (rope->m_nFlags1 & 0xF9) | ((flags & 1) << 2) | 1;
    rope->m_fGroundZ = 0.0f;
    rope->m_fMass = 0.0f;
    rope->m_fTotalLength = 0.0f;
    rope->m_fSegmentLength = (holder && (holder->GetStatus() & 7) == 2) ? 0.9f : 0.5f;
    rope->m_nFlags2 = 0;
    rope->m_pRopeHolder = holder;
    rope->m_nType = static_cast<eRopeType>(ropeType);
    if (holder)
        holder->RegisterReference(reinterpret_cast<CEntity**>(&rope->m_pRopeHolder));
    rope->m_nTime = bExpires ? CTimer::GetTimeInMS() + timeExpire : 0;
    switch (rope->m_nType) {
    case eRopeType::CRANE_MAGNO:
        rope->m_fTotalLength = 50.0f;
        rope->m_fSegmentLength = 1.6129032f; // 50/31
        break;
    case eRopeType::WRECKING_BALL:
    case eRopeType::QUARRY_CRANE_ARM:
    case eRopeType::CRANE_TROLLEY:
        rope->m_fTotalLength = 68.0f;
        rope->m_fSegmentLength = 2.1935484f; // 68/31
        break;
    case eRopeType::MAGNET:
        rope->m_fTotalLength = 20.0f;
        rope->m_fSegmentLength = 0.64516127f; // 20/31
        break;
    default:
        rope->m_fTotalLength = 10.0f;
        rope->m_fSegmentLength = 0.32258064f; // 10/31
        break;
    }
    if (rope->m_nType >= eRopeType::CRANE_MAGNO && rope->m_nType <= eRopeType::SWAT) {
        // Wrecking-ball/crane-arm/trolley ropes hang straight down by m_fSegmentLength;
        // every other type zigzags in x around the start position.
        for (auto s = 1u; s < NUM_ROPE_SEGMENTS; s++)
            rope->m_aSegments[s] = rope->m_aSegments[s - 1] - CVector{ 0.0f, 0.0f, rope->m_fSegmentLength };
    } else {
        auto odd = false;
        for (auto s = 1u; s < NUM_ROPE_SEGMENTS; s++) {
            const auto& prev = rope->m_aSegments[s - 1];
            rope->m_aSegments[s] = CVector{
                odd ? prev.x - rope->m_fSegmentLength : prev.x + rope->m_fSegmentLength,
                prev.y,
                prev.z,
            };
            odd = !odd;
        }
    }
    for (auto& speed : rope->m_aSpeed)
        speed = CVector{};
    rope->CreateHookObjectForRope();
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
// fDistAlongRope is a fraction along the rope (callers pass e.g. m_fCoorAlongRope
// advanced by CTimer::GetTimeStep() * 0.003f); the binary clamps it into [0, 0.999].
bool CRopes::FindCoorsAlongRope(uint32 ropeId, float fDistAlongRope, CVector* outPosn, CVector* outSpeed) {
    const auto idx = FindRope(ropeId);
    if (idx < 0)
        return false;
    auto& rope = GetRope(idx);
    const float t = std::clamp(fDistAlongRope, 0.0f, 0.999f);
    const float segFloat = t * 31.0f;
    const auto seg = static_cast<uint32>(segFloat); // binary truncates via float->int conversion
    const float frac = segFloat - static_cast<float>(seg);
    *outPosn = rope.m_aSegments[seg] * (1.0f - frac) + rope.m_aSegments[seg + 1] * frac;
    if (outSpeed)
        *outSpeed = rope.m_aSpeed[seg + 1];
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
