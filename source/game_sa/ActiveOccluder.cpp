#include "StdInc.h"

#include "ActiveOccluder.h"

void CActiveOccluder::InjectHooks()
{
    RH_ScopedClass(CActiveOccluder);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(IsPointWithinOcclusionArea, 0x71E580);
    RH_ScopedInstall(IsPointBehindOccluder, 0x71FA40);
}

// 0x71E580
bool CActiveOccluder::IsPointWithinOcclusionArea(float fX, float fY, float fRadius)
{
    if (m_LinesUsed <= 0)
        return true;

    for (auto i = 0; i < m_LinesUsed; ++i) {
        auto& line = m_Lines[i];
        if (!IsPointInsideLine(line.Origin.x, line.Origin.y, line.Delta.x, line.Delta.y, fX, fY, fRadius))
            return false;
    }

    return true;
}

// 0x71FA40
bool CActiveOccluder::IsPointBehindOccluder(CVector vecPos, float fRadius)
{
    if (m_NumFaces <= 0)
        return true;

    for (auto i = 0; i < m_NumFaces; ++i) {
        const auto& vecCamPos = TheCamera.GetPosition();

        auto fPosDotVec = DotProduct(vecPos, m_FaceNormals[i]) - m_FaceOffsets[i];
        auto fCamDotVec = DotProduct(vecCamPos, m_FaceNormals[i]) - m_FaceOffsets[i];

        if (fCamDotVec * fPosDotVec >= 0.0F || fabs(fPosDotVec) < fRadius)
            return false;
    }

    return true;
}
