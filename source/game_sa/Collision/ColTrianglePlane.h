/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "CompressedVector.h"
#include <extensions/FixedFloat.hpp>
#include "Vector.h"

class CStoredCollPoly;
class CColTriangle;
    
class CColTrianglePlane {
public:
    enum class Orientation : uint8 {
        POS_X,
        NEG_X,
        POS_Y,
        NEG_Y,
        POS_Z,
        NEG_Z,
    };
public:
    CompressedUnitVector     m_normal{};       //< Surface normal
    FixedFloat<int16, 128.f> m_normalOffset{}; //< vA.Dot(m_normal)
    Orientation              m_orientation{};  //< The planes orientation (Calculated from the normal)

public:
    static void InjectHooks();

    CColTrianglePlane(const CStoredCollPoly& poly);
    CColTrianglePlane(const CColTriangle& tri, const CompressedVector* vertices);
    CColTrianglePlane(const CVector& a, const CVector& b, const CVector& c);

    void GetNormal(CVector& out);
    CVector GetNormal() const noexcept { return m_normal; }
    void Set(const CompressedVector* vertices, const CColTriangle& triangle);

    float GetPtDotNormal(const CVector& pt) const { return pt.Dot(m_normal) - m_normalOffset; }
};
VALIDATE_SIZE(CColTrianglePlane, 0xA);
