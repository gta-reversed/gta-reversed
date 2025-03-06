/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Matrix.h"

class CPlaceable;

class CMatrixLink : public CMatrix {
public:
    CMatrixLink() = default;
    CMatrixLink(const CMatrix& matrix) : CMatrix(matrix) {} // NOTSA
    CMatrixLink(float fScale) : CMatrix() { SetScale(fScale); }

    inline void Insert(CMatrixLink* pWhere);
    inline void Remove();

    // NOTSA
    operator CMatrix&() { return *this; }
    operator const CMatrix&() const { return *this; }

public:
    CPlaceable*  m_pOwner{};
    CMatrixLink* m_pPrev{};
    CMatrixLink* m_pNext{};

private:
    friend void InjectHooksMain();
    static void InjectHooks();
};

VALIDATE_SIZE(CMatrixLink, 0x54);
