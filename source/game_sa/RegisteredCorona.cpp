#include "StdInc.h"

#include "RegisteredCorona.h"

//! Calculate the position to use for rendering
auto CRegisteredCorona::GetPosition() const -> CVector {
    if (!m_pAttachedTo) {
        return m_vPosn;
    }
    if (m_pAttachedTo->GetType() == ENTITY_TYPE_VEHICLE && m_pAttachedTo->AsVehicle()->IsSubBike()) {
        return m_pAttachedTo->AsBike()->m_mLeanMatrix.TransformPoint(m_vPosn);
    }
    return m_pAttachedTo->GetMatrix().TransformPoint(m_vPosn);
}

auto CRegisteredCorona::CalculateIntensity(float scrZ, float farClip) const -> float {
    return std::clamp(invLerp(farClip, farClip / 2.f, scrZ), 0.f, 1.f);
}
