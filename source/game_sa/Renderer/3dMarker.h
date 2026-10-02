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
    MARKER3D_CYLINDER = 1,
    MARKER3D_TUBE = 2,
    MARKER3D_ARROW2 = 3,
    MARKER3D_TORUS = 4,
    MARKER3D_CONE = 5,
    MARKER3D_CONE_NO_COLLISION = 6,

    // Add stuff above this
    MARKER3D_COUNT,

    // Sentinel value (Used for markers not in use)
    MARKER3D_NA = 257
};

class C3dMarker {
public:
    static void InjectHooks();

    C3dMarker() = default;  // 0x720F60
    ~C3dMarker() = default; // 0x720F70

    bool AddMarker(uint32 id, e3dMarkerType type, float size, uint8 red, uint8 green, uint8 blue, uint8 alpha, uint16 pulsePeriod, float pulseFraction, int16 rotateRate);
    void DeleteMarkerObject();
    bool IsZCoordinateUpToDate();
    void Render();
    void SetZCoordinateIfNotUpToDate(float coordinate);
    void UpdateZCoordinate(CVector point, float zDistance);
    void DeleteIfHasAtomic();

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
    uint16        m_LastMapReadX{ 30'000 }; // float casted to uint16
    uint16        m_LastMapReadY{ 30'000 }; // float casted to uint16
    float         m_LastMapReadResultZ{};
    float         m_RoofHeight{ 65535.0f };
    CVector       m_LastPosition{};
    uint32        m_OnScreenTestTime{};
};

VALIDATE_SIZE(C3dMarker, 0xA0);
