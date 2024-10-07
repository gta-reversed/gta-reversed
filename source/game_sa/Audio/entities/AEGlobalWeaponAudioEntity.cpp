#include "StdInc.h"

#include "AEGlobalWeaponAudioEntity.h"
#include "AEAudioHardware.h"

CAESound* (&pWaterfall)[3] = *(CAESound * (*)[3])0xB612F0;
CAESound*& pFogHorn = *(CAESound**)0xB612FC;

// 0x5075B0
CAEGlobalWeaponAudioEntity::CAEGlobalWeaponAudioEntity() : CAEWeaponAudioEntity() {
    m_Physical = new CPhysical();
}

// 0x506C50
CAEGlobalWeaponAudioEntity::~CAEGlobalWeaponAudioEntity() {
    delete m_Physical;
}

// 0x4DEF90
void CAEGlobalWeaponAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (curPlayPos != -1)
        return;

    if (sound == pFogHorn) {
        pFogHorn = nullptr;
        return;
    }

    for (auto& waterfall : pWaterfall) {
        if (sound == waterfall) {
            waterfall = nullptr;
        }
    }
}

// 0x4DFAA0
void CAEGlobalWeaponAudioEntity::AddAudioEvent(eAudioEvents audioId, eWeaponType weaponType, CPhysical* entity) {
    if (!entity)
        return;

    switch (audioId) {
    case AE_WEAPON_FIRE:
    case AE_WEAPON_FIRE_PLANE:
        WeaponFire(weaponType, entity, audioId);
        break;
    case AE_WEAPON_RELOAD_A:
    case AE_WEAPON_RELOAD_B:
        WeaponReload(weaponType, entity, audioId);
        break;
    case AE_PROJECTILE_FIRE:
        ProjectileFire(weaponType, entity, audioId);
        break;
    default:
        return;
    }
}

// 0x4DF060
void CAEGlobalWeaponAudioEntity::ProjectileFire(eWeaponType weaponType, CPhysical* physical, eAudioEvents event) {
    switch (weaponType) {
    case WEAPON_ROCKET:
    case WEAPON_ROCKET_HS:
        break;
    default:
        return;
    }

    static float gfRocketFrequencyVariations[] = { 1.08f, 1.0f, 0.92f, 3.3f } ;// 0x8AE5AC
    m_FrequencyVariation = (m_FrequencyVariation + 1) % 3;

    if (!AEAudioHardware.IsSoundBankLoaded(143, 5) && !AudioEngine.IsLoadingTuneActive()) {
        AEAudioHardware.LoadSoundBank(143, 5);
    }

    const auto volume = GetDefaultVolume(event);
    const auto speed0 = gfRocketFrequencyVariations[m_FrequencyVariation];
    const auto speed1 = speed0 * 1.25f;

    m_tempSound.Initialise(5, 68, this, physical->GetPosition(), volume - 8.0f, 3.0f, speed0, 1.0f, 0, SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY, 0.02f);
    m_tempSound.RegisterWithPhysicalEntity(physical);
    AESoundManager.RequestNewSound(&m_tempSound);

    if (!AEAudioHardware.IsSoundBankLoaded(138, 19)) {
        AEAudioHardware.LoadSoundBank(138, 19);
        return;
    }
    m_tempSound.Initialise(19, 26, this, physical->GetPosition(), volume, 3.0f, speed1, 1.0f, 0, SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY, 0.02f);
    m_tempSound.RegisterWithPhysicalEntity(physical);
    AESoundManager.RequestNewSound(&m_tempSound);
}

// 0x4DF210
void CAEGlobalWeaponAudioEntity::ServiceAmbientGunFire() {
    plugin::CallMethod<0x4DF210, CAEGlobalWeaponAudioEntity*>(this);
}

void CAEGlobalWeaponAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEGlobalWeaponAudioEntity, 0x862E5C, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Constructor, 0x5075B0);
    RH_ScopedInstall(Destructor, 0x506C50);
    RH_ScopedInstall(UpdateParameters, 0x4DEF90);
    RH_ScopedInstall(AddAudioEvent, 0x4DFAA0);
    RH_ScopedInstall(ProjectileFire, 0x4DF060);
    RH_ScopedInstall(ServiceAmbientGunFire, 0x4DF210, { .reversed = false });
}

CAEGlobalWeaponAudioEntity* CAEGlobalWeaponAudioEntity::Constructor() {
    this->CAEGlobalWeaponAudioEntity::CAEGlobalWeaponAudioEntity();
    return this;
}
CAEGlobalWeaponAudioEntity* CAEGlobalWeaponAudioEntity::Destructor() { this->CAEGlobalWeaponAudioEntity::~CAEGlobalWeaponAudioEntity(); return this; }
