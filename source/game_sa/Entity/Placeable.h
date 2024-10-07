/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "SimpleTransform.h"
#include "MatrixLink.h"

class CBox;
class CRect;

class NOTSA_EXPORT_VTABLE CPlaceable {
public:
    CSimpleTransform m_placement;
    CMatrixLink *m_matrix;

public:
    static void InjectHooks();

    CPlaceable();
    virtual ~CPlaceable();

    CMatrix& GetMatrix();

    static void ShutdownMatrixArray();
    static void InitMatrixArray();

    CVector GetRightVector();
    CVector GetForwardVector();
    CVector GetUpVector();

    void FreeStaticMatrix();
    void SetPosn(float x, float y, float z);
    void SetPosn(const CVector& posn);
    void SetOrientation(float x, float y, float z);
    void SetOrientation(CVector radians) { SetOrientation(radians.x, radians.y, radians.z); } // TODO: Replace method above with this
    void GetOrientation(float& x, float& y, float& z);
    void SetHeading(float heading);
    float GetHeading();
    float GetRoll() const;
    bool IsWithinArea(float x1, float y1, float x2, float y2) const;
    bool IsWithinArea(float x1, float y1, float z1, float x2, float y2, float z2) const;
    void RemoveMatrix();
    void AllocateStaticMatrix();
    void AllocateMatrix();
    void SetMatrix(CMatrix& matrix);

    // NOTSA
    bool IsPointInRange(const CVector& point, float range);
    bool IsEntityInRange(const CPlaceable* entity, float range) { return IsPointInRange(entity->GetPosition(), range); }
public:
    static constexpr uint32 NUM_MATRICES_TO_CREATE = 900;

    inline CVector& GetRight() const { return m_matrix->GetRight(); }
    inline CVector& GetForward() const { return m_matrix->GetForward(); }
    inline CVector& GetUp() const { return m_matrix->GetUp(); }
    inline const CVector& GetPosition() const { return m_matrix ? m_matrix->GetPosition() : m_placement.m_vPosn; }
    inline CVector& GetPosition() { return m_matrix ? m_matrix->GetPosition() : m_placement.m_vPosn; }
    inline CVector2D GetPosition2D() { return { GetPosition() }; }
};

VALIDATE_SIZE(CPlaceable, 0x18);
