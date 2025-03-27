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
    CMatrix       m_mat;
    RpAtomic*     m_pAtomic;
    RpMaterial*   m_pMaterial;
    e3dMarkerType m_nType;
    bool          m_bIsUsed;
    bool          m_bMustBeRenderedThisFrame;
    uint32        m_nIdentifier;
    CRGBA         m_colour;
    uint16        m_nPulsePeriod;
    int16         m_nRotateRate;
    uint32        m_nStartTime;
    float         m_fPulseFraction;
    float         m_fStdSize;
    float         m_fSize;
    float         m_fBrightness;
    float         m_DistToCam2D;
    CVector       m_vecNormal;
    uint16        m_nLastMapReadX; // float casted to uint16
    uint16        m_nLastMapReadY; // float casted to uint16
    float         m_fLastMapReadResultZ;
    float         m_fRoofHeight;
    CVector       m_vecLastPosition;
    uint32        m_nOnScreenTestTime;

public:
    static void InjectHooks();

    C3dMarker() = default;  // 0x720F60
    ~C3dMarker() = default; // 0x720F70

    void Init() { // NOTSA
        m_vecLastPosition = CVector();
        m_pAtomic = nullptr;
        m_nType = MARKER3D_NA;
        m_bIsUsed = false;
        m_bMustBeRenderedThisFrame = false;
        m_nIdentifier = 0;
        m_colour = CRGBA(255, 255, 255, 255);
        m_nPulsePeriod = 1024;
        m_nRotateRate = 5;
        m_nStartTime = 0;
        m_fPulseFraction = 0.25f;
        m_fStdSize = 1.0f;
        m_fSize = 1.0f;
        m_fBrightness = 1.0f;
        m_DistToCam2D = 0.0f;
        m_vecNormal = CVector();
        m_nLastMapReadX = 30000;
        m_fRoofHeight = 65535.0f;
    }

    bool AddMarker(uint32 id, e3dMarkerType type, float size, uint8 red, uint8 green, uint8 blue, uint8 alpha, uint16 pulsePeriod, float pulseFraction, int16 rotateRate);
    void DeleteMarkerObject();
    bool IsZCoordinateUpToDate();
    void Render();
    void SetZCoordinateIfNotUpToDate(float coordinate);
    void UpdateZCoordinate(CVector point, float zDistance);
    void DeleteIfHasAtomic();
};

VALIDATE_SIZE(C3dMarker, 0xA0);
