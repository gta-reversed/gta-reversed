/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Base.h"

#include "RGBA.h"
#include "Vector.h"
#include "Matrix.h"

struct RpAtomic;
struct RpMaterial;

enum e3dMarkerType : uint16 {
    MARKER3D_ARROW = 0,
    MARKER3D_CYLINDER,          // 0x1
    MARKER3D_TUBE,              // 0x2
    MARKER3D_ARROW2,            // 0x3
    MARKER3D_TORUS,             // 0x4
    MARKER3D_CONE,              // 0x5
    MARKER3D_CONE_NO_COLLISION, // 0x6

    // Add stuff above this
    MARKER3D_COUNT, // 0x7

    // Sentinel value (Used for markers not in use)
    MARKER3D_NA = 257
};

class C3dMarker {
public:
    static void InjectHooks();

    C3dMarker() = default;  // 0x720F60
    ~C3dMarker() = default; // 0x720F70

    bool AddMarker(uint32 id, e3dMarkerType type, float size,
        uint8 r, uint8 g, uint8 b, uint8 a, uint16 pulsePeriod, float pulseFraction, int16 rotateRate);
    void DeleteMarkerObject();
    void Render();
    void DeleteIfHasAtomic();
    void UpdateZCoordinate(CVector tempPlayerPos, float size);
    void SetZCoordinateIfNotUpToDate(float newZ);
    bool IsZCoordinateUpToDate();

public:
    CMatrix       m_Mat{};
    RpAtomic*     m_Atomic{};
    RpMaterial*   m_Material{};

    e3dMarkerType m_Type{ MARKER3D_NA };
    bool          m_IsInUse{};
    bool          m_IsActive{};

    uint32        m_ID{};

    CRGBA         m_Color{ 255, 255, 255, 255 };
    uint16        m_PulsePeriod{ 1'024 };
    int16         m_RotateRate{ 5 };
    uint32        m_StartTime{};
    float         m_PulseFraction{ 0.25f };
    float         m_StdSize{ 1.f };
    float         m_Size{ 1.f };
    float         m_Brightness{ 1.f };
    float         m_DistToCam2D{};
    CVector       m_Normal{};

    uint16        m_LastMapReadX{ 30'000 }, m_LastMapReadY{ 30'000 }; // float casted to uint16
    float         m_LastMapReadResultZ{};
    float         m_RoofHeight{ 65535.0f };
    CVector       m_LastPosition{};
    uint32        m_OnScreenTestTime{};
};

VALIDATE_SIZE(C3dMarker, 0xA0);
