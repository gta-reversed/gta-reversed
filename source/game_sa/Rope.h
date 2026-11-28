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
class CPhysical;

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
    void ReleasePickedUpObject();
    bool DoControlsApply() const;
    void CreateHookObjectForRope();
    bool UpdateWeightInRope(CVector weightPos, float unused, CVector* outWeightPos);
    void Remove();
    void Render();
    void PickUpObject(CEntity* obj);
    void Update();

    static void InjectHooks();

private:
    bool                 IsCrane() const;                                              // NOTSA
    void                 ProcessControls();                                            // NOTSA
    std::optional<float> ProcessWeight();                                              // NOTSA
    void                 ProcessPickUp(float staleZ);                                  // NOTSA
    bool                 PickUpVehicle(const CVector& lastSegment, bool onlyRcTiger);  // NOTSA
    void                 PickUpPed(const CVector& lastSegment, float staleZ);          // NOTSA
    void                 PickUpRopeObject(const CVector& lastSegment, eRopeType type); // NOTSA
    void                 LetDownForPickUp(float heightAbovePickUpPoint);               // NOTSA

public:
    // NOTE: Some names are off (see the comments), they are kept because other files use them
    std::array<CVector, NUM_ROPE_SEGMENTS> m_aSegments{};
    std::array<CVector, NUM_ROPE_SEGMENTS> m_aSpeed{};
    uintptr                                m_nId{};
    float                                  m_fGroundZ{};
    float                                  m_fMass{};             //!< The length of the whole rope
    float                                  m_fTotalLength{};      //!< The length of one segment (`m_fMass / (NUM_ROPE_SEGMENTS - 1)`)
    CEntity*                               m_pRopeHolder{};       //!< The crane / vehicle the rope hangs from
    CEntity*                               m_pAttachedEntity{};   //!< The hook object at the end of the rope (magnet, harness, wrecking ball)
    CEntity*                               m_pRopeAttachObject{}; //!< The entity the rope carries
    float                                  m_fSegmentLength{};    //!< How far the rope is let down (0.01 .. 0.9), `CObject::GetRopeHeight`
    uint32                                 m_nTime{};             //!< Time (ms) after which the rope falls down if it isn't registered again (0 = at once)
    uint8                                  m_nSegments{};         //!< Number of segments from the top that are held in place (by `CRopes::RegisterRope`)
    eRopeType                              m_nType{};
    uint8                                  m_nFlags1{}; //!< Frames left until something can be picked up again
    uint8                                  m_nFlags2{}; //!< 0x1 - Registered this frame, 0x4 - Collides with the ground
};

VALIDATE_SIZE(CRope, 0x328);
