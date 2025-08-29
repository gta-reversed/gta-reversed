/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Door.h"

void CDoor::InjectHooks() {
    RH_ScopedClass(CDoor);
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(Process, 0x6F4040, { .reversed = false });
    RH_ScopedGlobalInstall(ProcessImpact, 0x6F4540, { .reversed = false });
    RH_ScopedGlobalInstall(Open, 0x6F4790);
    RH_ScopedGlobalInstall(GetAngleOpenRatio, 0x6F47E0);
    RH_ScopedGlobalInstall(IsClosed, 0x6F4800);
    RH_ScopedGlobalInstall(IsFullyOpen, 0x6F4820);
}

// 0x6F4040
bool CDoor::Process(CVehicle* vehicle, CVector& oldMoveSpeed, CVector& oldTurnSpeed, Const CVector& offset) {
    return plugin::CallMethodAndReturn<bool, 0x6F4040, CDoor*, CVehicle*, CVector&, CVector&, CVector&>(this, vehicle, oldMoveSpeed, oldTurnSpeed, const_cast<CVector&>(offset));
}

// 0x6F4540
bool CDoor::ProcessImpact(CVehicle* vehicle, CVector& oldMoveSpeed, CVector& oldTurnSpeed, Const CVector& offset) {
    return plugin::CallMethodAndReturn<bool, 0x6F4540, CDoor*, CVehicle*, CVector&, CVector&, CVector&>(this, vehicle, oldMoveSpeed, oldTurnSpeed, const_cast<CVector&>(offset));
}

// 0x6F4790
void CDoor::Open(float angRatio) {
    m_fPrevAngle = m_fAngle;
    if (angRatio >= 1.0f) {
        m_nDirn  = m_nDirn;
        m_fAngle = m_fOpenAngle;
        if (m_nDirn >= 0) {
            m_nDoorState = DOOR_HIT_MAX_END;
        }
    } else {
        m_fAngle = angRatio * m_fOpenAngle;
        if (m_fAngle == 0.0f) {
            m_fAngVel = 0.0f;
        }
    }
}

// 0x6F47E0
float CDoor::GetAngleOpenRatio() const {
    if (m_fOpenAngle == 0.0f) {
        return 0.0f;
    } else {
        return m_fAngle / m_fOpenAngle;
    }
}

// 0x6F4800
bool CDoor::IsClosed() const {
    return m_fClosedAngle == m_fAngle;
}

// 0x6F4820
bool CDoor::IsFullyOpen() const {
    return fabs(m_fOpenAngle) - 0.5f <= fabs(m_fAngle);
}

// NOTSA
CVector CDoor::GetRotation() const {
    CVector rotation{ 0.f, 0.f, 0.f };
    rotation[m_nAxis] = m_fAngle;
    return rotation;
}

// NOTSA
void CDoor::UpdateFrameMatrix(CMatrix& mat) {
    mat.SetRotateKeepPos(GetRotation());
    mat.UpdateRW();
}
