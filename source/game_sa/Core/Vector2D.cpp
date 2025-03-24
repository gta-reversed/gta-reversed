/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"
#include "Vector2D.h"
#include "Vector.h"
#include "General.h"

void CVector2D::InjectHooks() {
    RH_ScopedClass(CVector2D);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(Magnitude, 0x420860);
    //RH_ScopedInstall(Normalise, 0x44E480); - Added extra parameter, must unhook
    RH_ScopedInstall(operator=, 0x43E110);
}

CVector2D::CVector2D(const CVector& v3) :
    CVector2D{v3.x, v3.y}
{
}

void CVector2D::Normalise(float* outMag) {
    auto mag = Magnitude();
    if (mag > 0.0f) {
        auto recip = 1.0F / mag;
        x *= recip;
        y *= recip;
    } else {
        x = 1.0f;
    }
    if (outMag) {
        *outMag = mag;
    }
}

uint32 CVector2D::NodeHeading() const {
    return CGeneral::GetNodeHeadingFromVector(x, y);
}

CVector2D CVector2D::RotatedBy(float rad) const {
    const auto s = std::sin(rad), c = std::cos(rad);
    return {
        x * c - y * s,
        x * s - y * c,
    };
}

bool CVector2D::ApproxEqualTo(CVector2D o, float epsilon) {
    return approxEqual(x, o.x, epsilon) && approxEqual(y, o.y, epsilon);
}
