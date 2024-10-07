/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Placeable.h"

void CPlaceable::InjectHooks() {
    RH_ScopedVirtualClass(CPlaceable, 0x863C40, 1);
    RH_ScopedCategory("Entity");

    RH_ScopedOverloadedInstall(SetPosn, "xyz", 0x420B80, void(CPlaceable::*)(float, float, float));
    RH_ScopedOverloadedInstall(SetPosn, "vector", 0x4241C0, void(CPlaceable::*)(const CVector&));
    RH_ScopedOverloadedInstall(SetOrientation, "xyz", 0x439A80, void(CPlaceable::*)(float, float, float));
    RH_ScopedInstall(SetHeading, 0x43E0C0);
    RH_ScopedInstall(GetHeading, 0x441DB0);
    RH_ScopedInstall(GetRoll, 0x420B30);
    RH_ScopedOverloadedInstall(IsWithinArea, "xy", 0x54F200, bool(CPlaceable::*)(float, float, float, float) const);
    RH_ScopedOverloadedInstall(IsWithinArea, "xyz", 0x54F2B0, bool(CPlaceable::*)(float, float, float, float, float, float) const);
    RH_ScopedInstall(RemoveMatrix, 0x54F3B0);
    RH_ScopedInstall(AllocateStaticMatrix, 0x54F4C0);
    RH_ScopedInstall(AllocateMatrix, 0x54F560);
    RH_ScopedInstall(SetMatrix, 0x54F610);
    RH_ScopedInstall(GetMatrix, 0x411990);
    RH_ScopedInstall(ShutdownMatrixArray, 0x54EFD0);
    RH_ScopedInstall(InitMatrixArray, 0x54F3A0);
    RH_ScopedInstall(FreeStaticMatrix, 0x54F010);
}

CPlaceable::CPlaceable() : m_placement() {
    m_matrix = nullptr;
}

CPlaceable::~CPlaceable() {
    if (m_matrix) {
        CPlaceable::RemoveMatrix();
        --numMatrices;
    }

    m_matrix = reinterpret_cast<CMatrixLink*>(&gDummyMatrix);
}

CVector CPlaceable::GetRightVector() {
    if (m_matrix)
        return m_matrix->GetRight();
    return { std::cos(m_placement.m_fHeading), std::sin(m_placement.m_fHeading), 0.0f };
}

CVector CPlaceable::GetForwardVector() {
    if (m_matrix)
        return m_matrix->GetForward();
    return { -std::sin(m_placement.m_fHeading), std::cos(m_placement.m_fHeading), 0.0f };
}

CVector CPlaceable::GetUpVector() {
    if (m_matrix)
        return m_matrix->GetUp();
    return {0.0f, 0.0f, 1.0f};
}

void CPlaceable::SetPosn(float x, float y, float z) {
    auto& pos = GetPosition();
    pos.Set(x, y, z);
}

void CPlaceable::SetPosn(const CVector& posn) {
    auto& pos = GetPosition();
    pos = posn;
}

void CPlaceable::SetOrientation(float x, float y, float z) {
    if (!m_matrix) {
        m_placement.m_fHeading = z;
        return;
    }

    CVector vecPos = m_matrix->GetPosition();
    m_matrix->SetRotate(x, y, z);
    m_matrix->GetPosition() += vecPos;
}

void CPlaceable::SetHeading(float heading) {
    if (m_matrix)
        m_matrix->SetRotateZOnly(heading);
    else
        m_placement.m_fHeading = heading;
}

float CPlaceable::GetHeading() {
    if (!m_matrix) {
        return m_placement.m_fHeading;
    }

    const auto& fwd = m_matrix->GetForward();
    return std::atan2(-fwd.x, fwd.y);
}

// 0x420B30
float CPlaceable::GetRoll() const {
    if (!m_matrix) {
        return 0.f;
    }

    const auto& right = m_matrix->GetRight();
    const auto  xymag = CVector2D{ right }.SquaredMagnitude(); // NOTE: We're using sqmag here because it doesn't matter, and we save a sqrt this way.
    return std::atan2(right.z, m_matrix->GetUp().z < 0.f ? -xymag : xymag);
}

bool CPlaceable::IsWithinArea(float x1, float y1, float x2, float y2) const {
    const auto& vecPos = GetPosition();
    if (x1 > x2)
        std::swap(x1, x2);

    if (y1 > y2)
        std::swap(y1, y2);

    return vecPos.x >= x1 && vecPos.x <= x2 && vecPos.y >= y1 && vecPos.y <= y2;
}


bool CPlaceable::IsWithinArea(float x1, float y1, float z1, float x2, float y2, float z2) const {
    const auto& vecPos = GetPosition();
    if (x1 > x2)
        std::swap(x1, x2);

    if (y1 > y2)
        std::swap(y1, y2);

    if (z1 > z2)
        std::swap(z1, z2);

    return vecPos.x >= x1
        && vecPos.x <= x2
        && vecPos.y >= y1
        && vecPos.y <= y2
        && vecPos.z >= z1
        && vecPos.z <= z2;
}

void CPlaceable::RemoveMatrix() {
    const auto& vecForward = m_matrix->GetForward();
    auto fHeading = std::atan2(-vecForward.x, vecForward.y);

    m_placement.m_vPosn = m_matrix->GetPosition();
    m_placement.m_fHeading = fHeading;

    m_matrix->m_pOwner = nullptr;
    gMatrixList.MoveToFreeList(m_matrix);
    m_matrix = nullptr;
}

void CPlaceable::AllocateStaticMatrix() {
    if (m_matrix)
        return gMatrixList.MoveToList2(m_matrix);

    if (gMatrixList.IsFull())
        gMatrixList.GetOldestLink()->m_pOwner->RemoveMatrix();

    m_matrix = gMatrixList.AddToList2();
    m_matrix->m_pOwner = this;
}

void CPlaceable::AllocateMatrix() {
    if (m_matrix)
        return;

    if (gMatrixList.IsFull())
        gMatrixList.GetOldestLink()->m_pOwner->RemoveMatrix();

    m_matrix = gMatrixList.AddToList1();
    m_matrix->m_pOwner = this;
}

void CPlaceable::SetMatrix(CMatrix& matrix) {
    if (!m_matrix) {
        if (matrix.GetUp().z == 1.0F) {
            auto& vecForward = matrix.GetForward();
            auto fHeading = std::atan2(-vecForward.x, vecForward.y);

            m_placement.m_vPosn = matrix.GetPosition();
            m_placement.m_fHeading = fHeading;
            return;
        }
        CPlaceable::AllocateMatrix();
    }

    *static_cast<CMatrix*>(m_matrix) = matrix;
}

// NOTSA
bool CPlaceable::IsPointInRange(const CVector& point, float range) {
    return DistanceBetweenPointsSquared(point, GetPosition()) <= sq(range);
}

CMatrix& CPlaceable::GetMatrix() {
    if (!m_matrix) {
        CPlaceable::AllocateMatrix();
        m_placement.UpdateMatrix(m_matrix);
    }

    return *m_matrix;
}

void CPlaceable::ShutdownMatrixArray() {
    gMatrixList.Shutdown();
}

void CPlaceable::InitMatrixArray() {
    ZoneScoped;

    gMatrixList.Init(CPlaceable::NUM_MATRICES_TO_CREATE);
}

void CPlaceable::FreeStaticMatrix() {
    gMatrixList.MoveToList1(m_matrix);
}

void CPlaceable::GetOrientation(float& x, float& y, float& z) {
    if (!m_matrix) {
        z = m_placement.m_fHeading;
        return;
    }

    x = asinf(GetForward().z);

    float cosx = std::cosf(x);
    float cosy = GetUp().z / cosx;
    y = std::acosf(cosy);

    float cosz = GetForward().y / cosx;
    z = std::acosf(cosz);
}
