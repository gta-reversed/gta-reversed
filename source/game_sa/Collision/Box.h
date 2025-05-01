/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"
#include "RGBA.h"

class CBox {
public:
    CVector m_vecMin{}, m_vecMax{};

    constexpr CBox() = default;
    constexpr CBox(CVector min, CVector max) : m_vecMin(min), m_vecMax(max) {}

    void Set(const CVector& vecMin, const CVector& vecMax);

    //! Updates box corners, like (if left>right then swap(left, right))
    void Recalc();

    CVector GetSize()   const { return m_vecMax - m_vecMin; }
    float   GetWidth()  const { return m_vecMax.x - m_vecMin.x; }
    float   GetLength() const { return m_vecMax.y - m_vecMin.y; }
    float   GetHeight() const { return m_vecMax.z - m_vecMin.z; }
    CVector GetCenter() const { return (m_vecMax + m_vecMin) / 2.f; }

    //! Check is point within the box
    bool IsPointInside(const CVector& point) const;

    //! Stretch the bounding box so that the point `pt` will be inside it
    void StretchToPoint(const CVector& pt);

    /*!
    * @addr notsa
    * @brief Render the box in the 3D world (Be sure to call from a place where 3D stuff is rendered, if called from elsewhere you won't see the lines!)
    *
    * @param transform Transformation matrix to be used
    * @param color     Color of the lines used
    */
    void DrawWireFrame(CRGBA color, const CMatrix& transform = CMatrix::Unity()) const;

public:
    static void InjectHooks();
};

VALIDATE_SIZE(CBox, 0x18);
