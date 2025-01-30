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

    bool IsPointWithinOcclusionArea(float fX, float fY, float fRadius) const;
    bool IsPointBehindOccluder(CVector vecPos, float fRadius) const;
    auto GetLines() const { return std::span{ m_Lines, m_LinesUsed }; }

public:
    CActiveOccluderLine m_Lines[6];
    uint16              m_DistToCam;
    uint8               m_LinesUsed;
    uint8               m_NumFaces;
    CVector             m_FaceNormals[3];
    float               m_FaceOffsets[3];
};
VALIDATE_SIZE(CActiveOccluder, 0xAC);
