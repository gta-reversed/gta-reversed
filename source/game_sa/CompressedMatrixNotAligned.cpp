#include "StdInc.h"

#include "CompressedMatrixNotAligned.h"

void CCompressedMatrixNotAligned::InjectHooks()
{
    RH_ScopedClass(CCompressedMatrixNotAligned);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(DecompressIntoFullMatrix, 0x59B9F0);
    RH_ScopedInstall(CompressFromFullMatrix, 0x59BAD0);
}

void CCompressedMatrixNotAligned::DecompressIntoFullMatrix(CMatrix& matrix) const
{
    matrix.GetRight() = m_vecRight;
    matrix.GetForward() = m_vecForward;
    matrix.GetUp() = CrossProduct(matrix.GetRight(), matrix.GetForward());
    matrix.GetPosition() = m_vecPos;
    matrix.Reorthogonalise();
}

void CCompressedMatrixNotAligned::CompressFromFullMatrix(const CMatrix& matrix)
{
    m_vecRight = matrix.GetRight();
    m_vecForward = matrix.GetForward();
    m_vecPos = matrix.GetPosition();
}

