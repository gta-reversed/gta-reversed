#pragma once

#include "Base.h"
#include "Vector2D.h"
#include "Vector.h"

struct CActiveOccluderLine {
    CVector2D Origin;
    CVector2D Dir;
    float     Length;
};
VALIDATE_SIZE(CActiveOccluderLine, 0x14);

class CActiveOccluder {
public:
    static void InjectHooks();

    bool IsPointWithinOcclusionArea(CVector2D pt, float radius = 0.f) const;
    bool IsPointBehindOccluder(CVector pt, float radius = 0.f) const;
    auto GetLines() const { return std::span{ m_Lines, m_LinesUsed }; }
    float GetDistToCam() const { return (float)(m_DistToCam); }
public:
    CActiveOccluderLine m_Lines[6];
    uint16              m_DistToCam;
    uint8               m_LinesUsed;
    uint8               m_NumFaces;
    CVector             m_FaceNormals[3];
    float               m_FaceOffsets[3];
};
VALIDATE_SIZE(CActiveOccluder, 0xAC);
