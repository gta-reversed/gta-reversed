#include "StdInc.h"

CGangInfo::CGangInfo() {
    rng::fill(m_nGangWeapons, WEAPON_UNARMED);
}

// NOTSA
size_t CGangInfo::GetNumOfWeaponChoices() const {
    size_t n{};
    for (; m_nGangWeapons[n] != WEAPON_UNARMED && ++n < m_nGangWeapons.size(););
    return n;
}

// NOTSA
eWeaponType CGangInfo::GetRandomWeapon() const {
    if (const auto nchoices = GetNumOfWeaponChoices()) {
        return CGeneral::RandomChoice(m_nGangWeapons | rng::views::take(nchoices));
    }
    return WEAPON_UNARMED; // No choices
}
