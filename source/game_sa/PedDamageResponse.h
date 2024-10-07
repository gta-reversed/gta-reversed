#pragma once

class CPedDamageResponse {
public:
    float m_fDamageHealth;
    float m_fDamageArmor;
    bool  m_bHealthZero;
    bool  m_bForceDeath;
    bool  m_bDamageCalculated;
    bool  m_bCheckIfAffectsPed;

    inline CPedDamageResponse() {
        m_fDamageHealth      = 0.0f;
        m_fDamageArmor       = 0.0f;
        m_bHealthZero        = false;
        m_bForceDeath        = false;
        m_bDamageCalculated  = false;
        m_bCheckIfAffectsPed = false;
    }

    //! Mark the damage as calculated
    void SetDamageAsCalculated() { m_bDamageCalculated = true; }

    //! Get the total damage done [Armor and HP]
    auto GetTotalDamage() const { return m_fDamageArmor + m_fDamageHealth; }
};
VALIDATE_SIZE(CPedDamageResponse, 0xC);

using CPedDamageResponseInfo = CPedDamageResponse;
