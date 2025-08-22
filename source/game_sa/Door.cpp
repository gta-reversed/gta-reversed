/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Door.h"

float& CDoor::DOOR_SPEED_MAX_CAPPED = *(float*)0x8D3950;

// 0x6F4040
bool CDoor::Process(CVehicle* vehicle, CVector& arg1, CVector& arg2, Const CVector& thisDoorPos) {
    return plugin::CallMethodAndReturn<bool, 0x6F4040, CDoor*, CVehicle*, CVector&, CVector&, CVector&>(this, vehicle, arg1, arg2, const_cast<CVector&>(thisDoorPos));
}

// 0x6F4540
bool CDoor::ProcessImpact(CVehicle* vehicle, CVector& arg1, CVector& arg2, Const CVector& thisDoorPos) {
    return plugin::CallMethodAndReturn<bool, 0x6F4540, CDoor*, CVehicle*, CVector&, CVector&, CVector&>(this, vehicle, arg1, arg2, const_cast<CVector&>(thisDoorPos));
}

// 0x6F4790
void CDoor::Open(float openRatio) {
    plugin::CallMethod<0x6F4790, CDoor*, float>(this, openRatio);
}

// 0x6F47E0
float CDoor::GetAngleOpenRatio() {
    return plugin::CallMethodAndReturn<float, 0x6F47E0, CDoor*>(this);
}

// 0x6F4800
bool CDoor::IsClosed() const {
    return m_fClosedAngle == m_fAngle;
}

// 0x6F4820
bool CDoor::IsFullyOpen() const {
    return fabs(m_fOpenAngle) - 0.5f <= fabs(m_fAngle);
}

CVector CDoor::GetRotation() const {
    CVector rotation{ 0.f, 0.f, 0.f };
    rotation[m_nAxis] = m_fAngle;
    return rotation;
}

void CDoor::UpdateFrameMatrix(CMatrix& mat) {
    mat.SetRotateKeepPos(GetRotation());
    mat.UpdateRW();
}
