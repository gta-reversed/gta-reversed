#pragma once

#include "AEWeaponAudioEntity.h"

class NOTSA_EXPORT_VTABLE CAEGlobalWeaponAudioEntity : public CAEWeaponAudioEntity {
public:
    CAEGlobalWeaponAudioEntity();
    ~CAEGlobalWeaponAudioEntity();

    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;

    void AddAudioEvent(eAudioEvents audioId, eWeaponType weaponType, CPhysical* entity);
    void ProjectileFire(eWeaponType weaponType, CPhysical* physical, eAudioEvents event);
    void ServiceAmbientGunFire();
    void PickAmbientGunFire(int32& gunShots, eWeaponType& weaponType, uint32& delay);

private:
    CPhysical* m_Physical;
    int8       m_FrequencyVariation;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CAEGlobalWeaponAudioEntity* Constructor();
    CAEGlobalWeaponAudioEntity* Destructor();
};
VALIDATE_SIZE(CAEGlobalWeaponAudioEntity, 0xA8);
