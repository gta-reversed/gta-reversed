/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <extensions/FixedVector.hpp>
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
    FixedVector<int16, 4096.f> m_normal{};       //< Surface normal
    FixedFloat<int16, 128.f>   m_normalOffset{}; //< vA.Dot(m_normal)
    Orientation                m_orientation{};  //< The planes orientation (Calculated from the normal)

public:
    static void InjectHooks();

    CColTrianglePlane(const CStoredCollPoly& poly);
    CColTrianglePlane(const CColTriangle& tri, const FixedVector<int16, 128.0f>* vertices);
    CColTrianglePlane(const CVector& a, const CVector& b, const CVector& c);

    void GetNormal(CVector& out);
    CVector GetNormal() const noexcept { return m_normal; }
    void Set(const FixedVector<int16, 128.0f>* vertices, const CColTriangle& triangle);

    float GetPtDotNormal(const CVector& pt) const { return pt.Dot(m_normal) - m_normalOffset; }
};
VALIDATE_SIZE(CColTrianglePlane, 0xA);
