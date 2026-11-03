/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"

class CEntity;
class CObject;

// todo: Better names, see CreateHookObjectForRope
enum class eRopeType : uint8 {
    NONE             = 0,
    CRANE_MAGNET1    = 1,
    CRANE_HARNESS    = 2,
    MAGNET           = 3,
    CRANE_MAGNO      = 4, // see ModelIndices::MI_MAGNOCRANE
    WRECKING_BALL    = 5,
    QUARRY_CRANE_ARM = 6,
    CRANE_TROLLEY    = 7,
    SWAT             = 8
};

constexpr auto NUM_ROPE_SEGMENTS{ 32u };

class CRope {
public:
    std::array<CVector, NUM_ROPE_SEGMENTS> m_aSegments;
    std::array<CVector, NUM_ROPE_SEGMENTS> m_aSpeed;
    uintptr    m_nId;
    float      m_fGroundZ;
    float      m_fTotalLength;   //!< 0x308 - Length of the whole rope
    float      m_fSegmentLength; //!< 0x30C - `m_fTotalLength / (NUM_ROPE_SEGMENTS - 1)`
    CEntity*   m_pRopeHolder;
    CEntity*   m_pAttachedEntity;
    CEntity*   m_pRopeAttachObject;
    float      m_fMass;          //!< 0x31C - TODO: Guessed name (`0.9` if held by a vehicle, `0.5` otherwise)
    uint32     m_nTime;
    uint8      m_nSegments;
    eRopeType  m_nType;
    uint8      m_nFlags1; // *
    uint8      m_nFlags2; // ** 0x1 - Set when (re-)registered, 0x4 - `flags & 1` passed to `CRopes::RegisterRope`

public:
    static void InjectHooks();

    void ReleasePickedUpObject();
    bool DoControlsApply() const;
    void CreateHookObjectForRope();
    int8 UpdateWeightInRope(float a2, float a3, float a4, int32 a5, float* a6);
    void Remove();
    void Render();
    void PickUpObject(CEntity* obj);
    void Update();
};

VALIDATE_SIZE(CRope, 0x328);
