#include "StdInc.h"

float& CPedDamageResponseCalculator::ms_damageFactor = *(float*)0x8A6260; // 5555.5498

void CPedDamageResponseCalculator::InjectHooks()
{
    RH_ScopedClass(CPedDamageResponseCalculator);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x4AD3F0);
}

CPedDamageResponseCalculator::CPedDamageResponseCalculator(CEntity* pEntity, float fDamage, eWeaponType weaponType, ePedPieceTypes bodyPart, bool bSpeak)
{
    m_pDamager = pEntity;
    m_fDamageFactor = fDamage;
    m_bodyPart = bodyPart;
    m_weaponType = weaponType;
    m_bSpeak = bSpeak;
}

CPedDamageResponseCalculator::~CPedDamageResponseCalculator()
{
    // nothing here
}

// 0x4AD3F0
CPedDamageResponseCalculator* CPedDamageResponseCalculator::Constructor(CEntity * pEntity, float fDamage, eWeaponType weaponType, ePedPieceTypes bodyPart, bool bSpeak)
{
    this->CPedDamageResponseCalculator::CPedDamageResponseCalculator(pEntity, fDamage, weaponType, bodyPart, bSpeak);
    return this;
}

void CPedDamageResponseCalculator::ComputeDamageResponse(CPed * pPed, CPedDamageResponse * pDamageResponse, bool bSpeak)
{
    plugin::CallMethod<0x4B5AC0, CPedDamageResponseCalculator*, CPed *, CPedDamageResponse *, bool>(this, pPed, pDamageResponse, bSpeak);
}
