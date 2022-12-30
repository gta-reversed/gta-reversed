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
    * @addr notsa
    * @brief Render the line in the 3D world (Be sure to call from a place where 3D stuff is rendered, if called from elsewhere you won't see the lines!)
    *
    * @param transform Transformation matrix to be used
    * @param color     Color of the lines used
    */
    void DrawWireFrame(CRGBA color, const CMatrix& transform) const;
public:
    CVector m_vecStart;
    float   m_fStartSize;
    CVector m_vecEnd;
    float   m_fEndSize;

};

VALIDATE_SIZE(CColLine, 0x20);
