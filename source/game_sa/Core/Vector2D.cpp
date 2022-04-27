/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Vector2D.h"

void CVector2D::InjectHooks() {
    RH_ScopedClass(CVector2D);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(Magnitude, 0x420860);
    RH_ScopedInstall(Normalise, 0x44E480);
    RH_ScopedInstall(operator=, 0x43E110);
}

void CVector2D::Normalise() {
    auto len = Magnitude();
    if (len > 0.0f) {
        auto recip = 1.0F / len;
        x *= recip;
        y *= recip;
    } else {
        x = 1.0f;
    }
}
