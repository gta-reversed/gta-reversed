#include "StdInc.h"
#include "BouncingPanel.h"

float& CBouncingPanel::BOUNCE_SPRING_DAMP_MULT = *(float*)0x8D3954;
float& CBouncingPanel::BOUNCE_SPRING_RETURN_MULT = *(float*)0x8D3958;
float& CBouncingPanel::BOUNCE_VEL_CHANGE_LIMIT = *(float*)0x8D395C;
float& CBouncingPanel::BOUNCE_HANGING_DAMP_MULT = *(float*)0x8D3960;
float& CBouncingPanel::BOUNCE_HANGING_RETURN_MULT = *(float*)0x8D3964;

void CBouncingPanel::InjectHooks() {
    RH_ScopedClass(CBouncingPanel);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ResetPanel, 0x6F4910);
    RH_ScopedInstall(SetPanel, 0x6F4920);
    RH_ScopedInstall(ProcessPanel, 0x6F49A0);
}

// 0x6F4910
void CBouncingPanel::ResetPanel() {
    m_nFrameId = -1;
}

// 0x6F4920
void CBouncingPanel::SetPanel(int16 frameId, int16 axis, float angleLimit) {
    m_nFrameId = frameId;
    m_nAxis = axis;
    m_fAngleLimit = angleLimit;
    m_vecRotation = CVector();
    m_vecPos = CVector();
}

float CBouncingPanel::GetAngleChange(float velocity) const {
    return std::clamp(velocity, -BOUNCE_VEL_CHANGE_LIMIT, BOUNCE_VEL_CHANGE_LIMIT) * m_fAngleLimit;
}

// 0x6F49A0
void CBouncingPanel::ProcessPanel(CVehicle* vehicle, RwFrame* frame, CVector moveForce, CVector turnForce, float fReturnMultiplier, float fDampMultiplier) {
    CMatrix  frameMat{RwFrameGetMatrix(frame)};
    CMatrix& vehMat = vehicle->GetMatrix();
    CVector  compPos = vehMat.TransformVector(frameMat.GetPosition());
    CVector  angularVelocity = vehMat.InverseTransformVector(vehicle->GetSpeed(compPos) - moveForce + CrossProduct(compPos, turnForce));

    switch (m_nAxis) {
    case 0: {
        m_vecPos.y += GetAngleChange(angularVelocity.z);
        m_vecPos.z += GetAngleChange(angularVelocity.y);
        break;
    }
    case 1: {
        m_vecPos.x += GetAngleChange(angularVelocity.z);
        m_vecPos.z += GetAngleChange(angularVelocity.y);
        break;
    }
    case 2: {
        m_vecPos.x += GetAngleChange(angularVelocity.y);
        m_vecPos.y += GetAngleChange(angularVelocity.x);
        break;
    }
    }

    const bool areMultsSet = fReturnMultiplier > -1.0f && fDampMultiplier > -1.0f;
    {
        const float rtnMult = areMultsSet
            ? fReturnMultiplier
            : m_nAxis == 2 ? BOUNCE_HANGING_RETURN_MULT : BOUNCE_SPRING_RETURN_MULT;

        m_vecPos -= rtnMult * m_vecRotation * CTimer::ms_fTimeStep;
    }
    {
        const float dampMult = areMultsSet
            ? fDampMultiplier
            : m_nAxis == 2 ? BOUNCE_HANGING_DAMP_MULT : BOUNCE_SPRING_DAMP_MULT;

        m_vecPos *= pow(dampMult, CTimer::ms_fTimeStep);
    }
    m_vecRotation += m_vecPos * CTimer::ms_fTimeStep;

    switch (m_nAxis) {
    case 0: {
        CVector& right = frameMat.GetRight();
        right.y = m_vecRotation.z;
        right.z = m_vecRotation.y;
        break;
    }
    case 1: {
        CVector& fwd = frameMat.GetForward();
        fwd.x = m_vecRotation.z;
        fwd.z = m_vecRotation.x;
        break;
    }
    case 2: {
        CVector& up = frameMat.GetUp();
        up.y = m_vecRotation.x;
        up.x = m_vecRotation.y;
        break;
    }
    }

    frameMat.UpdateRW();
}
