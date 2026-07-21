#pragma once

#include "Vector.h"
#include "extensions/FixedVector.hpp"

class CCompressedMatrixNotAligned {
public:
    CVector                   m_vecPos;
    FixedVector<int8, 127.0f> m_vecRight;
    FixedVector<int8, 127.0f> m_vecForward;

public:
    static void InjectHooks();

    void DecompressIntoFullMatrix(CMatrix& matrix) const;
    void CompressFromFullMatrix(const CMatrix& matrix);

    // @notsa
    static auto Compress(const CMatrix& matrix) {
        CCompressedMatrixNotAligned compMatrix{};
        compMatrix.CompressFromFullMatrix(matrix);
        return compMatrix;
    }

    static auto Decompress(const CCompressedMatrixNotAligned& compMatrix) {
        CMatrix matrix{};
        compMatrix.DecompressIntoFullMatrix(matrix);
        return matrix;
    }
};
VALIDATE_SIZE(CCompressedMatrixNotAligned, 20);
