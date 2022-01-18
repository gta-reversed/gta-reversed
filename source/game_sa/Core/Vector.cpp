/*
Plugin-SDK (Grand Theft Auto San Andreas) source file
Authors: GTA Community. See more here
https://github.com/DK22Pac/plugin-sdk
Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

void CVector::InjectHooks()
{
    ReversibleHooks::Install("CVector", "Magnitude", 0x4082C0, &CVector::Magnitude);
    ReversibleHooks::Install("CVector", "Magnitude2D", 0x406D50, &CVector::Magnitude2D);
    ReversibleHooks::Install("CVector", "Normalise", 0x59C910, &CVector::Normalise);
    ReversibleHooks::Install("CVector", "NormaliseAndMag", 0x59C970, &CVector::NormaliseAndMag);
    ReversibleHooks::Install("CVector", "Cross", 0x70F890, &CVector::Cross);
    ReversibleHooks::Install("CVector", "Sum", 0x40FDD0, &CVector::Sum);
    ReversibleHooks::Install("CVector", "Difference", 0x40FE00, &CVector::Difference);
    ReversibleHooks::Install("CVector", "FromMultiply", 0x59C670, &CVector::FromMultiply);
    ReversibleHooks::Install("CVector", "FromMultiply3x3", 0x59C6D0, &CVector::FromMultiply3x3);
    ReversibleHooks::Install("CVector", "global_CrossProduct_out", 0x59C730, static_cast<CVector*(*)(CVector*, CVector*, CVector*)>(&CrossProduct));
    ReversibleHooks::Install("CVector", "global_DotProduct_vec*vec*", 0x59C6D0, static_cast<float(*)(CVector*, CVector*)>(&DotProduct));
}

CVector CVector::Random(float min, float max) {
    const auto Get = [=] { return CGeneral::GetRandomNumberInRange(min, max); };
    return { Get(), Get(), Get() };
}

CVector::CVector()
{
    x = 0.0f; y = 0.0f; z = 0.0f;
}

CVector::CVector(float X, float Y, float Z)
{
    x = X;
    y = Y;
    z = Z;
}


// Returns length of vector
float CVector::Magnitude() const
{
    return sqrt(x * x + y * y + z * z);
}

float CVector::Magnitude2D()
{
    return sqrt(x * x + y * y);
}

// Normalises a vector
void CVector::Normalise()
{
    NormaliseAndMag();
}

// Normalises a vector and returns length
float CVector::NormaliseAndMag()
{
    const auto fDot = x * x + y * y + z * z;
    if (fDot <= 0.0F)
    {
        x = 1.0F;
        return 1.0F;
    }

    const auto fRecip = 1.0F / sqrt(fDot);
    x *= fRecip;
    y *= fRecip;
    z *= fRecip;

    return 1.0F / fRecip;
}

// Performs cross calculation
void CVector::Cross(const CVector& left, const CVector &right)
{
    x = left.y * right.z - left.z * right.y;
    y = left.z * right.x - left.x * right.z;
    z = left.x * right.y - left.y * right.x;
}

// Adds left + right and stores result
void CVector::Sum(const CVector& left, const CVector &right)
{
    x = left.x + right.x;
    y = left.y + right.y;
    z = left.z + right.z;
}

// Subtracts left - right and stores result
void CVector::Difference(const CVector& left, const CVector &right)
{
    x = left.x - right.x;
    y = left.y - right.y;
    z = left.z - right.z;
}

// Assigns value from other vector
void CVector::operator= (const CVector& right)
{
    x = right.x;
    y = right.y;
    z = right.z;
}

// Adds value from the second vector.
void CVector::operator+=(const CVector& right)
{
    x += right.x;
    y += right.y;
    z += right.z;
}

void CVector::operator-=(const CVector& right)
{
    x -= right.x;
    y -= right.y;
    z -= right.z;
}

void CVector::operator*=(const CVector& right)
{
    x *= right.x;
    y *= right.y;
    z *= right.z;
}

// Multiplies vector by a floating point value
void CVector::operator *= (float multiplier)
{
    x *= multiplier;
    y *= multiplier;
    z *= multiplier;
}

// Divides vector by a floating point value
void CVector::operator /= (float divisor)
{
    x /= divisor;
    y /= divisor;
    z /= divisor;
}

void CVector::FromMultiply(CMatrix const& matrix, CVector const& vector)
{
    x = matrix.m_pos.x + matrix.m_right.x * vector.x + matrix.m_forward.x * vector.y + matrix.m_up.x * vector.z;
    y = matrix.m_pos.y + matrix.m_right.y * vector.x + matrix.m_forward.y * vector.y + matrix.m_up.y * vector.z;
    z = matrix.m_pos.z + matrix.m_right.z * vector.x + matrix.m_forward.z * vector.y + matrix.m_up.z * vector.z;
}

void CVector::FromMultiply3x3(CMatrix const& matrix, CVector const& vector)
{
    x = matrix.m_right.x * vector.x + matrix.m_forward.x * vector.y + matrix.m_up.x * vector.z;
    y = matrix.m_right.y * vector.x + matrix.m_forward.y * vector.y + matrix.m_up.y * vector.z;
    z = matrix.m_right.z * vector.x + matrix.m_forward.z * vector.y + matrix.m_up.z * vector.z;
}

CVector* CrossProduct(CVector* out, CVector* a, CVector* b)
{
    out->Cross(*a, *b);
    return out;
}

CVector CrossProduct(const CVector& a, const CVector& b)
{
    CVector result;
    result.Cross(a, b);
    return result;
}

float DotProduct(CVector* v1, CVector* v2)
{
    return v1->z * v2->z + v1->y * v2->y + v1->x * v2->x;
}

float DotProduct(const CVector& v1, const CVector& v2)
{
    return v1.z * v2.z + v1.y * v2.y + v1.x * v2.x;
}

float DotProduct2D(const CVector& v1, const CVector& v2)
{
    return v1.y * v2.y + v1.x * v2.x;
}
