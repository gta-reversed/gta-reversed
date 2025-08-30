#include "StdInc.h"

#include "RegisteredCorona.h"

void CRegisteredCorona::InjectHooks() {
    RH_ScopedClass(CRegisteredCorona);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Update, 0x6FABF0);
}

void CRegisteredCorona::Update() {
    if (!m_bRegisteredThisFrame) {
        m_Color.a = 0;
    }

    // removing a bound crown without an object
    if (m_bAttached && !m_pAttachedTo) {
        m_dwId = 0;
        --CCoronas::NumCoronas;
        return;
    }

    uint8 step = uint8(CTimer::GetTimeStep() * m_fFadeSpeed);
    if (!m_bCheckObstacles || ((!CCoronas::SunBlockedByClouds || m_dwId != 2) && CWorld::GetIsLineOfSightClear(m_vPosn, TheCamera.GetPosition(), true, false, false, false, false, false, false))) {
        if (m_bOffScreen || (m_bOnlyFromBelow && TheCamera.GetPosition().z > m_vPosn.z)) {
            m_FadedIntensity = std::clamp<uint8>(m_FadedIntensity - step, 0, 255);
        } else {
            auto alpha = m_Color.a;

            if (alpha > m_FadedIntensity) {
                m_FadedIntensity = std::min<uint8>(alpha, m_FadedIntensity + step);
            } else {
                m_FadedIntensity = std::max<uint8>(0, m_FadedIntensity - step);
            }

            if (CCoronas::bChangeBrightnessImmediately) {
                m_FadedIntensity = alpha;
            }

            if (m_dwId == 2) {
                CCoronas::LightsMult = std::max(0.6f, CCoronas::LightsMult - CTimer::GetTimeStep() * 0.06f);
            }
        }
    } else {
        m_FadedIntensity = std::max<uint8>(0, m_FadedIntensity - step);
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
