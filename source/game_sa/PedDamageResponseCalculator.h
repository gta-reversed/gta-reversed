#pragma once

#include "eWeaponType.h"

class CEntity;
class CPed;
class CPedDamageResponse;

class CPedDamageResponseCalculator {
public:
    const CEntity* m_pDamager;
    float          m_fDamageFactor;
    ePedPieceTypes m_bodyPart;
    eWeaponType    m_weaponType;
    bool           m_bSpeak;     // refers to a CPed::Say call (the dying scream?)

    static inline auto& ms_damageFactor = StaticRef<float>(0x8A6260);

public:
    CPedDamageResponseCalculator(const CEntity* entity, float fDamage, eWeaponType weaponType, ePedPieceTypes bodyPart, bool bSpeak);
    ~CPedDamageResponseCalculator() = default; // 0x4AD420

    void AccountForPedDamageStats(CPed* ped, CPedDamageResponse& response);
    void AccountForPedArmour(CPed* ped, CPedDamageResponse& response);
    void AdjustPedDamage(CPed* ped);
    bool ComputeWillForceDeath(CPed* ped, CPedDamageResponse& response);
    void ComputeWillKillPed(CPed* ped, CPedDamageResponse& response, bool bSpeak);
    bool IsBleedingWeapon(CPed* ped) const;
    void ComputeDamageResponse(CPed* ped, CPedDamageResponse& response, bool bSpeak);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CPedDamageResponseCalculator* Constructor(CEntity* entity, float fDamage, eWeaponType weaponType, ePedPieceTypes bodyPart, bool bSpeak);
};

VALIDATE_SIZE(CPedDamageResponseCalculator, 0x14);
