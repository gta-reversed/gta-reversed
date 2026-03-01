/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"
#include "Vector.h"

class CQuaternion {
public:
    union {
        struct {
            CVector imag;
            float real;
        };
        struct {
            float x, y, z, w;
        };
    };

public:
    static void InjectHooks();

    constexpr CQuaternion(const RtQuat& q) : imag{q.imag}, real{q.real} {}
    constexpr CQuaternion() : x{}, y{}, z{}, w{} {}
    constexpr CQuaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    // Quat to matrix
    void Get(RwMatrix* out) const;

    // Quat to euler angles
    void Get(float *x, float *y, float *z);

    // Quat to axis & angle
    void Get(RwV3d *axis, float *angle);

    // Stores result of quat multiplication
    void Multiply(const CQuaternion& a, const CQuaternion& b);

    // Spherical linear interpolation
    void Slerp(const CQuaternion& from, const CQuaternion& to, float halfTheta, float sinThetaInv, float t);

    // Quat from matrix
    void Set(const RwMatrix& m);

    // Quat from euler angles
    void Set(float x, float y, float z);

    // Quat from axis & angle
    void Set(RwV3d *axis, float angle);

    // Spherical linear interpolation
    void Slerp(const CQuaternion& from, const CQuaternion& to, float t);

    // Conjugate of a quat
    void Conjugate();
    CQuaternion Conjugated() const { CQuaternion c = *this; c.Conjugate(); return c; }

    // Squared length of a quat
    float GetLengthSquared() const;

    // Multiplies quat by a floating point value
    void Scale(float multiplier);

    // Copies value from other quat
    void Copy(const CQuaternion& from);

    // Gets a dot product for quats
    float Dot(const CQuaternion& a);

    // Normalises a quat
    void Normalise();

    // Add right to the quat 0x4D12F0
    void operator+=(const CQuaternion& right) {
        x += right.x;
        y += right.y;
        z += right.z;
        w += right.w;
    }

    // Substract right from the quat 0x4D1320
    void operator-=(const CQuaternion& right) {
        x -= right.x;
        y -= right.y;
        z -= right.z;
        w -= right.w;
    }

    // Assigns value from other quat 0x4D00C0
    CQuaternion& operator=(const CQuaternion& right) {
        x = right.x;
        y = right.y;
        z = right.z;
        w = right.w;
        return *this;
    }

    // Multiplies quat by a floating point value 0x4CF9B0
    void operator*=(float multiplier) {
        x *= multiplier;
        y *= multiplier;
        z *= multiplier;
        w *= multiplier;
    }

    CQuaternion operator-() const {
        return { -x, -y, -z, -w };
    }

    // NOTSA
    RtQuat* AsRtQuat() { return (RtQuat*)this; }

    operator RtQuat() const { return RtQuat{x, y, z, w}; }
};

VALIDATE_SIZE(CQuaternion, 0x10);

constexpr float DotProduct(const CQuaternion& q1, const CQuaternion& q2) {
    return q1.x * q2.x +
           q1.y * q2.y +
           q1.z * q2.z +
           q1.w * q2.w;
}

constexpr CQuaternion operator+(const CQuaternion& left, const CQuaternion& right) {
    return { left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w };
}

constexpr CQuaternion operator-(const CQuaternion& left, const CQuaternion& right) {
    return { left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w };
}

constexpr CQuaternion operator*(const CQuaternion& left, float right) {
    return { left.x * right, left.y * right, left.z * right, left.w * right };
}

constexpr CQuaternion operator*(float left, const CQuaternion& right) {
    return { left * right.x, left * right.y, left * right.z, left * right.w };
}

constexpr CQuaternion operator/(const CQuaternion& left, float right) {
    return { left.x / right, left.y / right, left.z / right, left.w / right };
}
