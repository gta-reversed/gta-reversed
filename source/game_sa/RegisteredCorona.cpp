#include "StdInc.h"

#include "RegisteredCorona.h"

void CRegisteredCorona::InjectHooks() {
    RH_ScopedClass(CRegisteredCorona);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Update, 0x6FABF0);
}

// 0x6FABF0
void CRegisteredCorona::Update() {
    if (!m_bRegisteredThisFrame) {
        m_Color.a = 0;
    }

    if (m_bAttached && !m_pAttachedTo) {
        m_dwId = 0;
        --CCoronas::NumCoronas;
        return;
    }

    float currentIntensity = m_FadedIntensity;
    float fadeStep = CTimer::GetTimeStep() * m_fFadeSpeed;
    const auto& camPos = TheCamera.GetPosition();
    if (!m_bCheckObstacles || ((!CCoronas::SunBlockedByClouds || m_dwId != 2)
        && CWorld::GetIsLineOfSightClear(m_vPosn, camPos, true, false, false, false, false, false, false))) {
        if (m_bOffScreen || (m_bOnlyFromBelow && camPos.z > m_vPosn.z)) {
            // Fade out when off-screen or camera is above
            currentIntensity = std::max(0.0f, currentIntensity - fadeStep);
            m_FadedIntensity = (uint8)currentIntensity;
        } else {
            // Fade towards target alpha
            if (m_FadedIntensity < m_Color.a) {
                currentIntensity = std::min((float)m_Color.a, currentIntensity + fadeStep);
            } else if (m_FadedIntensity > m_Color.a) {
                currentIntensity = std::max((float)m_Color.a, currentIntensity - fadeStep);
            }

            m_FadedIntensity = (uint8)currentIntensity;

            if (CCoronas::bChangeBrightnessImmediately) {
                m_FadedIntensity = m_Color.a;
            }

            if (m_dwId == 2) {
                CCoronas::LightsMult = std::max(0.6f, CCoronas::LightsMult - CTimer::GetTimeStep() * 0.06f);
            }
        }
    } else {
        // Fade out when blocked by obstacles
        currentIntensity = std::max(0.0f, currentIntensity - fadeStep);
        m_FadedIntensity = (uint8)currentIntensity;
    }

    if (!m_FadedIntensity && !m_bJustCreated) {
        m_dwId = 0;
        --CCoronas::NumCoronas;
    }

    m_bJustCreated = false;
    m_bRegisteredThisFrame = false;
}

//! Calculate the position to use for rendering
auto CRegisteredCorona::GetPosition() const -> CVector {
    if (!m_pAttachedTo) {
        return m_vPosn;
    }
    if (m_pAttachedTo->IsVehicle() && m_pAttachedTo->AsVehicle()->IsSubBike()) {
        return m_pAttachedTo->AsBike()->m_mLeanMatrix.TransformPoint(m_vPosn);
    }
    return m_pAttachedTo->GetMatrix().TransformPoint(m_vPosn);
}

auto CRegisteredCorona::CalculateIntensity(float scrZ, float farClip) const -> float {
    return std::clamp(invLerp(farClip, farClip / 2.f, scrZ), 0.f, 1.f);
}
