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
    float      m_fMass;
    float      m_fTotalLength;
    CEntity*   m_pRopeHolder;
    CEntity*   m_pAttachedEntity;
    CEntity*   m_pRopeAttachObject;
    float      m_fSegmentLength;
    uint32     m_nTime;
    uint8      m_nSegments;
    eRopeType  m_nType;
    uint8      m_nFlags1; // *
    uint8      m_nFlags2; // ** leftover?

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
