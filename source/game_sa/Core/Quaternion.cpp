/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "Quaternion.h"

void CQuaternion::InjectHooks() {
    RH_ScopedClass(CQuaternion);
    RH_ScopedCategory("Core");

    RH_ScopedOverloadedInstall(Get, "", 0x59C080, void (CQuaternion::*)(RwMatrix*) const);
}

// Quat to matrix
template<>
RwMatrix CQuaternion::GetAs() const {
    auto ii  = imag + imag;

    const auto x2x = ii.x * imag.x;
    const auto y2x = ii.y * imag.x;
    const auto z2x = ii.z * imag.x;

    const auto y2y = ii.y * imag.y;
    const auto z2y = ii.z * imag.y;
    const auto z2z = ii.z * imag.z;

    const auto x2r = ii.x * real;
    const auto y2r = ii.y * real;
    const auto z2r = ii.z * real;

    RwMatrix out;

    CVector right{ 1.0F - (z2z + y2y), z2r + y2x, z2x - y2r };
    RwV3dAssign(RwMatrixGetRight(&out), &right);

    CVector up{ y2x - z2r, 1.0F - (z2z + x2x), x2r + z2y };
    RwV3dAssign(RwMatrixGetUp(&out), &up);

    CVector at{ y2r + z2x, z2y - x2r, 1.0F - (y2y + x2x) };
    RwV3dAssign(RwMatrixGetAt(&out), &at);

    return out;
}

// Quat to euler angles
void CQuaternion::Get(float* x, float* y, float* z) {
    ((void(__thiscall*)(CQuaternion*, float*, float*, float*))0x59C160)(this, x, y, z);
}

// Quat to axis & angle
void CQuaternion::Get(RwV3d* axis, float* angle) {
    ((void(__thiscall*)(CQuaternion*, RwV3d*, float*))0x59C230)(this, axis, angle);
}

// Stores result of quat multiplication
void CQuaternion::Multiply(const CQuaternion& a, const CQuaternion& b) {
    ((void(__thiscall*)(CQuaternion*, const CQuaternion&, const CQuaternion&))0x59C270)(this, a, b);
}

// Spherical linear interpolation
void CQuaternion::Slerp(const CQuaternion& from, const CQuaternion& to, float halftheta, float sintheta_inv, float t) {
    ((void(__thiscall*)(CQuaternion*, const CQuaternion&, const CQuaternion&, float, float, float))0x59C300)(this, from, to, halftheta, sintheta_inv, t);
}

// Quat from matrix
void CQuaternion::Set(const RwMatrix& m) {
    ((void(__thiscall*)(CQuaternion*, const RwMatrix&))0x59C3E0)(this, m);
}

// Quat from euler angles
void CQuaternion::Set(float x, float y, float z) {
    ((void(__thiscall*)(CQuaternion*, float, float, float))0x59C530)(this, x, y, z);
}

// Quat from axis & angle
void CQuaternion::Set(RwV3d* axis, float angle) {
    ((void(__thiscall*)(CQuaternion*, RwV3d*, float))0x59C600)(this, axis, angle);
}

// Spherical linear interpolation
void CQuaternion::Slerp(const CQuaternion& from, const CQuaternion& to, float t) {
    ((void(__thiscall*)(CQuaternion*, const CQuaternion&, const CQuaternion&, float))0x59C630)(this, from, to, t);
}

// Conjugate of a quat
void CQuaternion::Conjugate() {
    ((void(__thiscall*)(CQuaternion*))0x4D37D0)(this);
}

// Squared length of a quat
float CQuaternion::GetLengthSquared() const {
    // Originally NOP.
    return sq(x) + sq(y) + sq(z) + sq(w);
}

// Multiplies quat by a floating point value
void CQuaternion::Scale(float multiplier) {
    ((void(__thiscall*)(CQuaternion*, float))0x4CF9B0)(this, multiplier);
}

// Copies value from other quat
void CQuaternion::Copy(const CQuaternion& from) {
    ((void(__thiscall*)(CQuaternion*, const CQuaternion&))0x4CF9E0)(this, from);
}

// Gets a dot product for quats
float CQuaternion::Dot(const CQuaternion& rhs) {
    return this->w * rhs.w + this->z * rhs.z + this->y * rhs.y + this->x * rhs.x;
}

// Normalises a quat
void CQuaternion::Normalise() {
    const auto sqMag = GetLengthSquared();
    if (sqMag == 0.f) {
        w = 1.0;
    } else {
        *this = *this / std::sqrt(sqMag);
    }
}
