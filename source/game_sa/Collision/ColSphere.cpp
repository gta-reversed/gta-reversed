#include "StdInc.h"

#include "ColSphere.h"

void CColSphere::InjectHooks() {
    RH_ScopedClass(CColSphere);
    RH_ScopedCategory("Collision");

    RH_ScopedInstall(Set, 0x40FD10);
    RH_ScopedInstall(IntersectRay, 0x40FF20);
    RH_ScopedInstall(IntersectEdge, 0x4100E0);
    RH_ScopedInstall(IntersectPoint, 0x410040);
    RH_ScopedInstall(IntersectSphere, 0x410090);
}

// 0x40FD10
void CColSphere::Set(float radius, const CVector& center, eSurfaceType material, uint8 pieceType, tColLighting lighting) {
    m_fRadius = radius;
    m_vecCenter = center;
    m_Surface.m_nMaterial = material;
    m_Surface.m_nPiece = pieceType;
    m_Surface.m_nLighting = lighting;
}

// 0x40FF20
bool CColSphere::IntersectRay(const CVector& rayOrigin, const CVector& direction, CVector& intersectPoint1, CVector& intersectPoint2) {
    CVector distance = rayOrigin - m_vecCenter;
    float b = 2.0f * DotProduct(direction, distance);
    float c = DotProduct(distance, distance) - m_fRadius * m_fRadius;
    float t0 = 0.0f, t1 = 0.0f;
    if (CGeneral::SolveQuadratic(1.0f, b, c, t0, t1)) {
        intersectPoint1 = (t0 * direction) + rayOrigin;
        intersectPoint2 = (t1 * direction) + rayOrigin;
        return true;
    }
    return false;
}

// 0x4100E0
bool CColSphere::IntersectEdge(const CVector& startPoint, const CVector& endPoint, CVector& intersectPoint1, CVector& intersectPoint2) {
    CVector originCenterDistance = startPoint - m_vecCenter;
    CVector rayDirection = endPoint - startPoint;
    float rayLength = rayDirection.Magnitude();
    rayDirection.Normalise();
    float a = DotProduct(rayDirection, rayDirection);
    float b = 2.0f * DotProduct(originCenterDistance, rayDirection);
    float c = DotProduct(originCenterDistance, originCenterDistance) - m_fRadius * m_fRadius;
    float discriminant = b * b - 4.0f * a * c; // discriminant = b^2-4ac
    // discriminant == 0: the ray intersects one point on the sphere
    // discriminant > 0: the ray intersects two points on the sphere
    if (discriminant < 0.0f) {
        return false;
    }

    float discriminantSquareRoot = sqrt(discriminant);
    float numerator1 = (-b - discriminantSquareRoot) * 0.5f;
    float numerator2 = (discriminantSquareRoot - b) * 0.5f;
    if (numerator1 > rayLength || numerator2 < 0.0f) {
        return false;
    }

    intersectPoint2 = endPoint;
    if (numerator2 < rayLength) {
        intersectPoint2 = (rayDirection * numerator2) + startPoint;
    }

    intersectPoint1 = startPoint;
    if (numerator1 > 0.0f) {
        intersectPoint1 = (rayDirection * numerator1) + startPoint;
    }
    return true;
}

// 0x410040
bool CColSphere::IntersectPoint(const CVector& point) {
    CVector distance = m_vecCenter - point;
    return m_fRadius * m_fRadius > distance.SquaredMagnitude();
}

// 0x410090
bool CColSphere::IntersectSphere(const CColSphere& right) const {
    CVector distance = m_vecCenter - right.m_vecCenter;
    return std::powf(m_fRadius + right.m_fRadius, 2.0f) > distance.SquaredMagnitude();
}

auto TransformObject(const CColSphere& sp, const CMatrix& transform) -> CColSphere {
    return CColSphere{
        TransformObject(static_cast<const CSphere&>(sp), transform),
        sp.m_Surface
    };
}
