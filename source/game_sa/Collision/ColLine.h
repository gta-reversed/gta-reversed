/*
        Plugin-SDK file
        Authors: GTA Community. See more here
        https://github.com/DK22Pac/plugin-sdk
        Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"

class CColLine {
public:
    static void InjectHooks();

    CColLine() = default;
    CColLine(const CVector& start, const CVector& end);

    void Set(const CVector& start, const CVector& end);

    /*!
    * @notsa
    * @brief Distance to a point from this line (Uses `CCollision::DistToLine`)
    */
    float DistTo(CVector pos) const;

    /*!
    * @addr notsa
    * @brief Render the line in the 3D world (Be sure to call from a place where 3D stuff is rendered, if called from elsewhere you won't see the lines!)
    *
    * @param transform Transformation matrix to be used
    * @param color     Color of the lines used
    */
    void DrawWireFrame(CRGBA color, const CMatrix& transform) const;

    //! Is the line vertical
    bool IsVertical() const { return CVector2D{ m_vecStart } == CVector2D{ m_vecEnd }; }

    friend auto TransformObject(const CColLine& ln, const CMatrix& transform) -> CColLine {
        return { transform.TransformPoint(ln.m_vecStart), transform.TransformPoint(ln.m_vecEnd) };
    }

public:
    CVector m_vecStart;
    float   m_fStartSize;
    CVector m_vecEnd;
    float   m_fEndSize;

};

VALIDATE_SIZE(CColLine, 0x20);
