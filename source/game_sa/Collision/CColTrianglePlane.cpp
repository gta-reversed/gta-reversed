#include "StdInc.h"

void CColTrianglePlane::InjectHooks()
{
    ReversibleHooks::Install("CColTrianglePlane", "GetNormal", 0x411610, &CColTrianglePlane::GetNormal);
    ReversibleHooks::Install("CColTrianglePlane", "Set", 0x411660, &CColTrianglePlane::Set);
}

void CColTrianglePlane::GetNormal(CVector& out)
{
    out = UncompressUnitVector(m_normal);
}

void CColTrianglePlane::Set(CompressedVector const* vertices, CColTriangle& triangle)
{
    const auto vecA = UncompressVector(vertices[triangle.m_nVertA]);
    const auto vecB = UncompressVector(vertices[triangle.m_nVertB]);
    const auto vecC = UncompressVector(vertices[triangle.m_nVertC]);

    const auto vecBsubA = vecB - vecA;
    const auto vecCsubA = vecC - vecA;

    auto vecCross = CrossProduct(vecCsubA, vecBsubA);
    vecCross.Normalise();
    m_normal = CompressUnitVector(vecCross);
    m_nDistance = static_cast<uint16>(DotProduct(vecA, vecCross) * 128.0F);

    const auto normXOrient = fabs(vecCross.x);
    const auto normYOrient = fabs(vecCross.y);
    const auto normZOrient = fabs(vecCross.z);

    if (normXOrient <= normYOrient || normXOrient <= normZOrient)
    {
        if (normYOrient <= normZOrient)
        {
            if (vecCross.z <= 0.0F)
                m_nOrientation = eTrianglePlaneOrientation::NEG_Z;
            else
                m_nOrientation = eTrianglePlaneOrientation::POS_Z;
        }
        else if (vecCross.y <= 0.0F)
            m_nOrientation = eTrianglePlaneOrientation::NEG_Y;
        else
            m_nOrientation = eTrianglePlaneOrientation::POS_Y;
    }
    else
    {
        if (vecCross.x <= 0.0F)
            m_nOrientation = eTrianglePlaneOrientation::NEG_X;
        else
            m_nOrientation = eTrianglePlaneOrientation::POS_X;
    }
}
