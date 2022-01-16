/*
    Plugin-SDK (Grand Theft Auto San Andreas) source file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

void CRect::InjectHooks()
{
    ReversibleHooks::Install("CRect", "IsFlipped", 0x404190, &CRect::IsFlipped);
    ReversibleHooks::Install("CRect", "Restrict", 0x404200, &CRect::Restrict);
    ReversibleHooks::Install("CRect", "Resize", 0x404260, &CRect::Resize);
    ReversibleHooks::Install("CRect", "IsPointInside", 0x404290, (bool(CRect::*)(CVector2D const&) const)&CRect::IsPointInside);
    ReversibleHooks::Install("CRect", "IsPointInside_tolerance", 0x4042D0, (bool(CRect::*)(CVector2D const&, float) const)&CRect::IsPointInside);
    ReversibleHooks::Install("CRect", "SetFromCenter", 0x43E020, &CRect::SetFromCenter);
    ReversibleHooks::Install("CRect", "GetCenter", 0x43E050, (void(CRect::*)(float*, float*) const)&CRect::GetCenter);
    ReversibleHooks::Install("CRect", "StretchToPoint", 0x5327F0, &CRect::StretchToPoint);
}

CRect::CRect(float fLeft, float fTop, float fRight, float fBottom)
{
    left = fLeft;
    top = fTop;
    right = fRight;
    bottom = fBottom;
}

CRect::CRect()
{
    left = 1000000.0F;
    top = 1000000.0F;
    right = -1000000.0F;
    bottom = -1000000.0F;
}

inline bool CRect::IsFlipped() const
{
    return left > right || top > bottom;
}

inline void CRect::Restrict(CRect const& restriction)
{
    if (restriction.left < left)
        left = restriction.left;

    if (restriction.right > right)
        right = restriction.right;

    if (restriction.top < top)
        top = restriction.top;

    if (restriction.bottom > bottom)
        bottom = restriction.bottom;
}

inline void CRect::Resize(float resizeX, float resizeY)
{
    left -= resizeX;
    right += resizeX;
    top -= resizeY;
    bottom += resizeY;
}

inline bool CRect::IsPointInside(CVector2D const& point) const
{
    return left <= point.x && right >= point.x
        && top <= point.y && bottom >= point.y;
}

inline bool CRect::IsPointInside(CVector2D const& point, float tolerance) const
{
    return left - tolerance <= point.x && right + tolerance >= point.x
        && top - tolerance <= point.y && bottom + tolerance >= point.y;
}

inline void CRect::SetFromCenter(float x, float y, float size)
{
    left = x - size;
    right = x + size;
    top = y - size;
    bottom = y + size;
}

inline void CRect::GetCenter(float* x, float* y) const
{
    *x = (right + left) * 0.5F;
    *y = (top + bottom) * 0.5F;
}

inline void CRect::StretchToPoint(float x, float y)
{
    if (x < left)
        left = x;

    if (x > right)
        right = x;

    if (y < top)
        top = y;

    if (y > bottom)
        bottom = y;
}
