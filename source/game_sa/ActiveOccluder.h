#pragma once

#include "Base.h"
#include "Vector2D.h"
#include "Vector.h"

struct COcclusionLine {
    CVector2D Base, Delta;
    float     Length;
};

VALIDATE_SIZE(COcclusionLine, 0x14);

class CActiveOccluder {
public:
    bool IsPointWithinOcclusionArea(CVector2D pt, float radius = 0.f) const;
    bool IsPointBehindOccluder(CVector pt, float radius = 0.f) const;

public:
    COcclusionLine m_Lines[6];
    uint16         m_DistToCam;
    uint8          m_LinesUsed;

    uint8   m_NumFaces;
    CVector m_FaceNormals[3];
    float   m_FaceOffsets[3];

private:
    friend void InjectHooksMain();
    static void InjectHooks();

public:
    auto GetLines() const { return std::span{ m_Lines, m_LinesUsed }; }
    float GetDistToCam() const { return (float)(m_DistToCam); }
};

VALIDATE_SIZE(CActiveOccluder, 0xAC);
