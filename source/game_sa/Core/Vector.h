/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <numeric>
#include <span>
#include <rwplcore.h>
#include "Vector2D.h"

class CMatrix;

class CVector : public RwV3d {
public:
    constexpr CVector() = default;
    constexpr CVector(float X, float Y, float Z) : RwV3d{ X, Y, Z } {}
    constexpr CVector(RwV3d rwVec) { x = rwVec.x; y = rwVec.y; z = rwVec.z; }
    constexpr CVector(const CVector* rhs) { x = rhs->x; y = rhs->y; z = rhs->z; } // TODO: Remove
    constexpr explicit CVector(float value) { x = y = z = value; }
    explicit CVector(const CVector2D& v2, float z = 0.f);

public:
    static void InjectHooks();

    static CVector Random(float min, float max);
    static CVector Random(CVector min, CVector max);

    // Returns length of vector
    float Magnitude() const;

    // Returns length of 2d vector
    float Magnitude2D() const;

    // Normalises a vector in-place
    void Normalise();

    // Normalises a vector and returns length (in-place)
    float NormaliseAndMag();

    /// Get a normalized copy of this vector
    auto Normalized(float* outMag = nullptr) const -> CVector {
        CVector cpy = *this;
        const float mag = cpy.NormaliseAndMag();
        if (outMag) {
            *outMag = mag;
        }
        return cpy;
    }

    /// Perform a dot product with this and `o`, returning the result
    auto Dot(const CVector& o) const -> float;

    /*!
    * @notsa
    *
    * There's an SA function with the same name,
    * but don't get confused, that one stores the
    * result in-place.
    * 
    * @return The cross product of `*this` and `o`
    */
    auto Cross(const CVector& other) const -> CVector;

    /*!
    * @addr 0x70F890
    *
    * The original Cross function that stores the result in-place
    */
    void Cross_OG(const CVector& a, const CVector& b);

    // Adds left + right and stores result
    void Sum(const CVector& left, const CVector& right);

    // Subtracts left - right and stores result
    void Difference(const CVector& left, const CVector& right);

    CVector& operator=(const RwV3d& right) {
        x = right.x;
        y = right.y;
        z = right.z;
        return *this;
    }
    CVector& operator=(const CVector& right) {
        x = right.x;
        y = right.y;
        z = right.z;
        return *this;
    }
    void operator+=(const CVector& right);
    void operator-=(const CVector& right);
    void operator*=(const CVector& right);
    void operator*=(float multiplier);
    void operator/=(float divisor);

    // matrix * vector multiplication
    void FromMultiply(const CMatrix& matrix, const CVector& vector);
    void FromMultiply3x3(const CMatrix& matrix, const CVector& vector);

    inline void Set(float X, float Y, float Z) {
        x = X;
        y = Y;
        z = Z;
    }

    void Reset() {
        Set(0.f, 0.f, 0.f);
    }

    inline RwV3d ToRwV3d() {
        return { x, y, z };
    }

    inline void FromRwV3d(RwV3d& vec) {
        x = vec.x;
        y = vec.y;
        z = vec.z;
    }

    [[nodiscard]] inline float ComponentwiseSum() const {
        return x + y + z;
    }

    [[nodiscard]] inline float SquaredMagnitude() const {
        return x * x + y * y + z * z;
    }

    inline float SquaredMagnitude2D() const {
        return x * x + y * y;
    }

    [[nodiscard]] inline bool IsZero() const {
        return x == 0.0F && y == 0.0F && z == 0.0F;
    }

    float operator[](size_t i) const {
        return (&x)[i];
    }

    float& operator[](size_t i) {
        return (&x)[i];
    }

    //! Get a copy of `*this` vector projected onto `projectOnTo` (which is assumed to be unit length)
    //! The result will have a magnitude of `sqrt(abs(this->Dot(projectOnTo)))`
    CVector ProjectOnToNormal(const CVector& projectOnTo, float offset = 0.f) const {
        return projectOnTo * (Dot(projectOnTo) + offset);
    }

    //! Calculate the average position
    static CVector Average(const CVector* begin, const CVector* end);

    static CVector AverageN(const CVector* begin, size_t n) {
        return Average(begin, begin + n);
    }

    auto GetComponents() const {
        return std::span{ reinterpret_cast<const float*>(this), 3 };
    }

    //! Unit Z axis vector (0,0,1)
    static auto ZAxisVector() { return CVector{ 0.f, 0.f, 1.f }; }

    /*!
    * @param reMapRangeTo0To2Pi Return value will be in interval [0, 2pi] instead of [-pi, pi]
    * @return The heading of the vector in radians.
    */
    [[nodiscard]] float Heading(bool reMapRangeTo0To2Pi = false) const;

    /*!
    * @notsa
    * @return Make all component's values absolute (positive).
    */
    static friend CVector abs(CVector vec) {
        return { std::abs(vec.x), std::abs(vec.y), std::abs(vec.z) };
    }

    static friend CVector pow(CVector vec, float power) {
        return { std::pow(vec.x, power), std::pow(vec.y, power), std::pow(vec.z, power) };
    }
    
    friend constexpr CVector operator*(const CVector& vec, float multiplier) {
        return { vec.x * multiplier, vec.y * multiplier, vec.z * multiplier };
    }

#ifdef _DEBUG
    bool HasNanOrInf() const {
        for (auto i = 0; i < 3; i++) {
            const auto v = (*this)[i];
            if (std::isnan(v) || std::isinf(v)) {
                return true;
            }
        }
        return false;
    }
#endif
};
VALIDATE_SIZE(CVector, 0xC);

constexpr inline CVector operator-(const CVector& vecOne, const CVector& vecTwo) {
    return { vecOne.x - vecTwo.x, vecOne.y - vecTwo.y, vecOne.z - vecTwo.z };
}

constexpr inline CVector operator+(const CVector& vecOne, const CVector& vecTwo) {
    return { vecOne.x + vecTwo.x, vecOne.y + vecTwo.y, vecOne.z + vecTwo.z };
}

constexpr inline CVector operator*(const CVector& vecOne, const CVector& vecTwo) {
    return { vecOne.x * vecTwo.x, vecOne.y * vecTwo.y, vecOne.z * vecTwo.z };
}

constexpr inline bool operator!=(const CVector& vecOne, const CVector& vecTwo) {
    return vecOne.x != vecTwo.x || vecOne.y != vecTwo.y || vecOne.z != vecTwo.z;
}

constexpr inline bool operator!=(const CVector& vec, float notEqualTo) {
    return vec.x != notEqualTo || vec.y != notEqualTo || vec.z != notEqualTo;
}

constexpr inline bool operator==(const CVector& vec, float equalTo) {
    return vec.x == equalTo && vec.y == equalTo && vec.z == equalTo;
}

constexpr inline bool operator==(const CVector& vecLeft, const CVector& vecRight) {
    return vecLeft.x == vecRight.x && vecLeft.y == vecRight.y && vecLeft.z == vecRight.z;
}


constexpr inline CVector operator/(const CVector& vec, float dividend) {
    return { vec.x / dividend, vec.y / dividend, vec.z / dividend };
}

constexpr inline CVector operator*(float multiplier, const CVector& vec) {
    return { vec.x * multiplier, vec.y * multiplier, vec.z * multiplier };
}

constexpr inline CVector operator-(const CVector& vec) {
    return { -vec.x, -vec.y, -vec.z };
}

inline float DistanceBetweenPoints(const CVector& pointOne, const CVector& pointTwo) {
    return (pointTwo - pointOne).Magnitude();
}

inline float DistanceBetweenPointsSquared(const CVector& pointOne, const CVector& pointTwo) {
    return (pointTwo - pointOne).SquaredMagnitude();
}

inline CVector Lerp(const CVector& vecOne, const CVector& vecTwo, float fProgress) {
    return vecOne * (1.0F - fProgress) + vecTwo * fProgress;
}

//! Component-wise clamp of values
inline CVector Clamp(CVector val, CVector min, CVector max) {
    for (auto i = 0; i < 3; i++) {
        val[i] = std::clamp(val[i], min[i], max[i]);
    }
    return val;
}

inline CVector Pow(const CVector& vec, float fPow) {
    return { pow(vec.x, fPow), pow(vec.y, fPow), pow(vec.z, fPow) };
}

CVector* CrossProduct(CVector* out, CVector* a, CVector* b);
float DotProduct(CVector* v1, CVector* v2);
CVector CrossProduct(const CVector& a, const CVector& b);
float DotProduct(const CVector& v1, const CVector& v2);
float DotProduct2D(const CVector& v1, const CVector& v2);
static CVector Normalized(CVector v) { v.Normalise(); return v; }
static CVector ProjectVector(const CVector& what, const CVector& onto) {
    return onto * (DotProduct(what, onto) / onto.SquaredMagnitude());
}

[[deprecated]] inline CVector Multiply3x3_MV(const CMatrix& m, const CVector& v) { NOTSA_UNREACHABLE("Use `m.TransformVector(v)` instead"); }
[[deprecated]] inline CVector Multiply3x3_VM(const CVector& v, const CMatrix& m) { NOTSA_UNREACHABLE("Use `m.InverseTransformVector(v)` m"); }
[[deprecated]] inline CVector MultiplyMatrixWithVector(const CMatrix& mat, const CVector& vec) { NOTSA_UNREACHABLE("Use `m.TransformPoint(v)` instead"); }
