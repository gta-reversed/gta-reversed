/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RGBA.h"
#include "Vector.h"

/*!
 * @brief Checkpoint types. Original enum had prefix of `CHECKPOINT_`
*/
enum class eCheckpointType : uint32 {
    TUBE         = 0x0,
    ENDTUBE      = 0x1,
    EMPTYTUBE    = 0x2,
    TORUS        = 0x3,
    TORUS_NOFADE = 0x4,
    TORUSROT     = 0x5,
    TORUSTHROUGH = 0x6,
    TORUS_UPDOWN = 0x7,
    TORUS_DOWN   = 0x8,

    //
    // Add above this
    //
    NUM = 0x9,   ///< Count of different checkpoint types
    NA  = 0x101, ///< Sentinel value (Used for markers not in use)
};
using eCheckpointTypeU16 = notsa::WEnumU16<eCheckpointType>;

class CCheckpoint {
public:
    void Init() {
        *this = CCheckpoint{};
    }

    static void InjectHooks();

    /*!
     * @brief Render this checkpoint. Checkpoints are basically just markers, so what we're really doing here is placing markers each time.
    */
    void Render();

    /*!
     * @brief Update heading, position, etc as necessary (Based on 0x7229C0)
    */
    void Update();

    /*!
     * @brief Change the position
     * @param pos The new position. Z axis is ignored for the type `TORUS_DOWN`.
    */
    void SetPosition(const CVector& pos);

    /*!
     * @brief Mark this checkpoint as not used. (See 0x722FC0 aka `CCheckpoints::DeleteCP`)
    */
    void MarkAsDeleted();

    /*!
     * @brief Set the pointing direction
     * @param heading The new pointing direction in degrees
    */
    void SetHeading(float headingDeg);

public:
    eCheckpointTypeU16 m_Type{ eCheckpointType::NA };
    bool               m_IsUsed{ false };
    bool               m_RotFlag{ true };
    int32              m_ID{ 0 };
    CRGBA              m_Colour{ 255, 255, 255, 255 };
    uint16             m_PulsePeriod{ 1'024 };
    int16              m_RotateRate{ 5 };
    CVector            m_Pos{};
    CVector            m_Fwd{}; ///< Pointing direction
    float              m_PulseFraction{ 0.25f };
    float              m_Size{ 1.f };
    float              m_DistToCam2D{ 0.f }; ///< (AKA CameraRange) - Distance to player's camera at the moment it's placed
    float              m_MultiSize{ 0.f };

};
VALIDATE_SIZE(CCheckpoint, 0x38);
