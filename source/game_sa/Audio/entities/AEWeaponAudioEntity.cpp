/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "AEWeaponAudioEntity.h"

#include "AESoundManager.h"
#include "AEAudioHardware.h"
#include "AEAudioUtility.h"

// 0x507560
CAEWeaponAudioEntity::CAEWeaponAudioEntity() {
    Clear();
}

// 0x503450
void CAEWeaponAudioEntity::Initialise() {
    m_nState = 3;
    m_nChainsawSoundState = 4;
    if (!AudioEngine.IsLoadingTuneActive()) {
        AEAudioHardware.LoadSoundBank(143, 5);
    }
}

// 0x503480
void CAEWeaponAudioEntity::Terminate() {
    AESoundManager.CancelSoundsOwnedByAudioEntity(this, true);
}

// 0x503490
void CAEWeaponAudioEntity::Reset() {
    AESoundManager.CancelSoundsOwnedByAudioEntity(this, true);
    Clear();
}

// 0x504F80
void CAEWeaponAudioEntity::WeaponFire(eWeaponType type, CPhysical* entity, eAudioEvents audioEventId) {
    if (!entity)
        return;

    switch (type) {
    case WEAPON_PISTOL:
        return PlayGunSounds(entity, 52, 53, 6, 7, 8, audioEventId, 0.0f, 1.41421f, 1.0f);

    case WEAPON_PISTOL_SILENCED:
        return PlayGunSounds(entity, 76, 77, 24, 24, -1, audioEventId, -7.0f, 1.0f, 1.0f);

    case WEAPON_DESERT_EAGLE:
        return PlayGunSounds(entity, 52, 53, 6, 7, 8, audioEventId, 0.0f, 0.94387001f, 1.0f);

    case WEAPON_SHOTGUN:
    case WEAPON_SPAS12_SHOTGUN:
        return PlayGunSounds(entity, 73, 74, 21, 22, 23, audioEventId, 0.0f, 1.0f, 1.0f);

    case WEAPON_SAWNOFF_SHOTGUN:
        return PlayGunSounds(entity, 73, 74, 21, 22, 23, audioEventId, 0.0f, 0.79369998f, 0.93f);

    case WEAPON_MICRO_UZI:
        return PlayGunSounds(entity, 29, 30, 0, 1, 2, audioEventId, 0.0f, 1.0f, 1.0f);

    case WEAPON_MP5:
        return PlayGunSounds(entity, 29, 30, 17, 18, 2, audioEventId, 0.0f, 1.0f, 1.0f);

    case WEAPON_AK47:
    case WEAPON_M4:
        return PlayGunSounds(entity, 33, 53, 3, 4, 5, audioEventId, 0.0f, 1.0f, 1.0f);

    case WEAPON_TEC9:
        return PlayGunSounds(entity, 29, 30, 0, 1, 2, audioEventId, 0.0f, 1.25992f, 1.0f);

    case WEAPON_COUNTRYRIFLE:
        return PlayGunSounds(entity, 52, 53, 26, 27, 23, audioEventId, 0.0f, 0.89f, 1.0f);

    case WEAPON_SNIPERRIFLE:
        return PlayGunSounds(entity, 52, 53, 26, 27, 23, audioEventId, 0.0f, 1.0f, 1.0f);

    case WEAPON_FLAMETHROWER:
        if (!m_nFlameThrowerLastPlayedTime)
            PlayFlameThrowerSounds(entity, 83, 26, audioEventId, -14.0f, 1.0f);
        m_nFlameThrowerLastPlayedTime = CTimer::GetTimeInMS();
        break;

    case WEAPON_MINIGUN:
        return PlayMiniGunFireSounds(entity, audioEventId);

    case WEAPON_DETONATOR:
        return PlayGunSounds(entity, 49, -1, -1, -1, -1, audioEventId, -14.0f, 1.0f, 1.0f);

    case WEAPON_SPRAYCAN:
        if (!m_nSpraycanLastPlayedTime)
            PlayWeaponLoopSound(entity, 28, audioEventId, -20.0f, 1.0f, AE_FRONTEND_HIGHLIGHT);

        m_nSpraycanLastPlayedTime = CTimer::GetTimeInMS();
        break;

    case WEAPON_EXTINGUISHER:
        if (!m_nExtinguisherLastPlayedTime)
            PlayWeaponLoopSound(entity, 9, audioEventId, -20.0f, 0.79369998f, AE_FRONTEND_ERROR);

        m_nExtinguisherLastPlayedTime = CTimer::GetTimeInMS();
        break;

    case WEAPON_CAMERA:
        return PlayCameraSound(entity, audioEventId, -14.0);

    case WEAPON_NIGHTVISION:
    case WEAPON_INFRARED:
        return PlayGoggleSound(64, audioEventId);

    default:
        return;
    }
}

// 0x503690
void CAEWeaponAudioEntity::WeaponReload(eWeaponType type, CPhysical* entity, eAudioEvents event) {
    if (!entity)
        return;

    if (!AEAudioHardware.IsSoundBankLoaded(143, 5)) {
        if (!AudioEngine.IsLoadingTuneActive()) {
            AEAudioHardware.LoadSoundBank(143, 5);
        }
        return;
    }

    int16 soundType;
    float volumeOffset = 0.0f;

    switch (type) {
    case WEAPON_PISTOL:
    case WEAPON_PISTOL_SILENCED:
        soundType = event != AE_WEAPON_RELOAD_A ? 66 : 55;
        break;
    case WEAPON_DESERT_EAGLE:
        soundType = 4 * (event == AE_WEAPON_RELOAD_A) + 51;
        break;
    case WEAPON_SHOTGUN:
    case WEAPON_SPAS12_SHOTGUN:
        soundType = (event != AE_WEAPON_RELOAD_A) + 71;
        break;
    case WEAPON_SAWNOFF_SHOTGUN:
        soundType = (event != AE_WEAPON_RELOAD_A) + 69;
        break;
    case WEAPON_MICRO_UZI:
    case WEAPON_MP5:
    case WEAPON_TEC9:
        soundType = (event != AE_WEAPON_RELOAD_A) + 84;
        break;
    case WEAPON_AK47:
    case WEAPON_M4:
        soundType = (event != AE_WEAPON_RELOAD_A) + 31;
        break;
    case WEAPON_COUNTRYRIFLE:
        if (event != AE_WEAPON_RELOAD_A)
            return;
        soundType = 32;
        volumeOffset = -6.0f;
        break;
    case WEAPON_SNIPERRIFLE:
        soundType = event != AE_WEAPON_RELOAD_A ? 32 : 55;
        break;
    default:
        return;
    }

    const auto volume = GetDefaultVolume(event) + volumeOffset;

    m_tempSound.Initialise(5, soundType, this, entity->GetPosition(), volume, 0.66f, 1.0f, 1.0f, 0, SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY, 0.0f, 0);
    m_tempSound.RegisterWithPhysicalEntity(entity);
    AESoundManager.RequestNewSound(&m_tempSound);
}

// 0x503B20
void CAEWeaponAudioEntity::ReportStealthKill(eWeaponType type, CPhysical* entity, eAudioEvents event) {
    if (type != WEAPON_KNIFE)
        return;

    const auto vol = GetDefaultVolume(event);
    const auto flags = static_cast<eSoundEnvironment>(SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES);

    if (!AEAudioHardware.IsSoundBankLoaded(143, 5)) {
        if (!AudioEngine.IsLoadingTuneActive()) {
            AEAudioHardware.LoadSoundBank(143, 5);
        }
        return;
    }
    m_tempSound.Initialise(5, 81, this, entity->GetPosition(), vol - 6.0f, 1.0f, 0.0f, 1.0f, 0);
    m_tempSound.RegisterWithPhysicalEntity(entity);
    m_tempSound.m_nEvent = AE_FRONTEND_PICKUP_COLLECTABLE1;
    m_tempSound.m_fMaxVolume = (float)CTimer::m_snTimeInMilliseconds;
    AESoundManager.RequestNewSound(&m_tempSound);

    if (!AEAudioHardware.IsSoundBankLoaded(39, 2)) {
        if (!AudioEngine.IsLoadingTuneActive()) {
            AEAudioHardware.LoadSoundBank(39, 2);
        }
        return;
    }
    m_tempSound.Initialise(2, 47, this, entity->GetPosition(), vol, 1.0f, 0.0f, 1.0f, 0, flags);
    m_tempSound.RegisterWithPhysicalEntity(entity);
    m_tempSound.m_nEvent = AE_FRONTEND_CAR_NO_CASH;
    m_tempSound.m_fMaxVolume = (float)CTimer::GetTimeInMS();
    AESoundManager.RequestNewSound(&m_tempSound);
}

// 0x503910
void CAEWeaponAudioEntity::ReportChainsawEvent(CPhysical* entity, int32 audioEventId) {
    plugin::CallMethod<0x503910, CAEWeaponAudioEntity*, CPhysical*, int32>(this, entity, audioEventId);
}

// 0x504610
void CAEWeaponAudioEntity::PlayWeaponLoopSound(CPhysical* entity, int16 sfxId, eAudioEvents startEvent, float audability, float speed, eAudioEvents endEvent) {
    if (!AEAudioHardware.IsSoundBankLoaded(143u, 5)) {
        if (!AudioEngine.IsLoadingTuneActive()) {
            AEAudioHardware.LoadSoundBank(143, 5);
        }
        return;
    }

    const auto volume = GetDefaultVolume(startEvent) + audability;
    const auto flags = static_cast<eSoundEnvironment>(SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_START_PERCENTAGE | SOUND_REQUEST_UPDATES);
    m_tempSound.Initialise(5, sfxId, this, entity->GetPosition(), volume, 1.0f, speed, 1.0f, 0, flags);
    m_tempSound.RegisterWithPhysicalEntity(entity);
    m_tempSound.m_nEvent = endEvent;
    AESoundManager.RequestNewSound(&m_tempSound);
}

// 0x504960
void CAEWeaponAudioEntity::PlayMiniGunStopSound(CPhysical* entity) {
    if (!entity) {
        m_nState = 3;
        return;
    }

    if (m_nState == 2) { // todo: figure out what is that
        m_nState = 2;
        return;
    }

    if (!AEAudioHardware.IsSoundBankLoaded(143u, 5)) {
        if (!AudioEngine.IsLoadingTuneActive()) {
            AEAudioHardware.LoadSoundBank(143, 5);
        }
        return;
    }

    const auto [distMult, speed] = [&] {
         if (entity->IsVehicle() && entity->AsVehicle()->IsSubPlane()) {
             return std::make_pair(1.8f, 0.7937f);
         } else {
             return std::make_pair(1.0f, 1.0f);
         }
    }();
    const auto dist = distMult * 0.66f;
    const auto volume = CAEAudioEntity::GetDefaultVolume(AE_WEAPON_FIRE_MINIGUN_STOP);
    const auto flags = static_cast<eSoundEnvironment>(SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES);
    m_tempSound.Initialise(5, 63, this, entity->GetPosition(), volume, dist, speed, 1.0f, 0, flags);
    m_tempSound.RegisterWithPhysicalEntity(entity);
    m_tempSound.m_nEvent = AE_FRONTEND_PICKUP_HEALTH;
    AESoundManager.RequestNewSound(&m_tempSound);
    m_nState = 2;
}

// 0x5047C0
void CAEWeaponAudioEntity::PlayMiniGunFireSounds(CPhysical* entity, int32 audioEventId) {
    plugin::CallMethod<0x5047C0, CAEWeaponAudioEntity*, CPhysical*, int32>(this, entity, audioEventId);
}

// 0x503CE0
void CAEWeaponAudioEntity::PlayGunSounds(CPhysical* entity, int16 emptySfxId, int16 farSfxId2, int16 highPitchSfxId3, int16 lowPitchSfxId4, int16 echoSfxId5, int32 audioEventId, float volumeChange, float speed1, float speed2) {
    plugin::CallMethod<0x503CE0, CAEWeaponAudioEntity*, CPhysical*, int16, int16, int16, int16, int16, int32, float, float, float>(
        this, entity, emptySfxId, farSfxId2, highPitchSfxId3, lowPitchSfxId4, echoSfxId5, audioEventId, volumeChange, speed1, speed2);
}

// 0x503500
void CAEWeaponAudioEntity::PlayGoggleSound(int16 sfxId, eAudioEvents event) {
    const auto volume = GetDefaultVolume(event);
    if (!AEAudioHardware.IsSoundBankLoaded(143u, 5)) {
        if (!AudioEngine.IsLoadingTuneActive()) {
            AEAudioHardware.LoadSoundBank(143, 5);
        }
        return;
    }

    const auto [speed0, speed1] = [] {
        if (CAEAudioUtility::ResolveProbability(0.5f)) {
            return std::make_pair(1.1892101f, 1.0f);
        } else {
            return std::make_pair(1.0f, 1.1892101f);
        }
    }();
    const auto vol = volume - 9.0f;

    m_tempSound.Initialise(5, sfxId, this, { -1.0f, 0.0f, 0.0f }, vol, 1.0f, speed0, 1.0f, 0, SOUND_DEFAULT);
    m_tempSound.m_nEnvironmentFlags = SOUND_FRONT_END | SOUND_FORCED_FRONT;
    AESoundManager.RequestNewSound(&m_tempSound);

    m_tempSound.Initialise(5, sfxId, this, { +1.0f, 0.0f, 0.0f }, vol, 1.0f, speed1, 1.0f, 0, SOUND_DEFAULT);
    m_tempSound.m_nEnvironmentFlags = SOUND_FRONT_END | SOUND_FORCED_FRONT;
    AESoundManager.RequestNewSound(&m_tempSound);
}

// 0x504470
void CAEWeaponAudioEntity::PlayFlameThrowerSounds(CPhysical* entity, int16 sfx1, int16 sfx2, int32 audioEventId, float audability, float speed) {
    plugin::CallMethod<0x504470, CAEWeaponAudioEntity*, CPhysical*, int16, int16, int32, float, float>(this, entity, sfx1, sfx2, audioEventId, audability, speed);
}

// 0x503870
void CAEWeaponAudioEntity::PlayFlameThrowerIdleGasLoop(CPhysical* entity) {
    if (m_FlameThrowerSound != nullptr)
        return;

    const auto volume = GetDefaultVolume(AE_FLAMETHROWER_IDLE);
    const auto flags = static_cast<eSoundEnvironment>(SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES);
    m_tempSound.Initialise(5, 10, this, entity->GetPosition(), volume, 0.66f, 1.0f, 1.0f, 0, flags);
    m_tempSound.RegisterWithPhysicalEntity(entity);
    m_FlameThrowerSound = AESoundManager.RequestNewSound(&m_tempSound);
}

// 0x5034E0
void CAEWeaponAudioEntity::StopFlameThrowerIdleGasLoop() {
    if (m_FlameThrowerSound) {
        m_FlameThrowerSound->StopSoundAndForget();
        m_FlameThrowerSound = nullptr;
    }
}

// 0x504AA0
void CAEWeaponAudioEntity::PlayChainsawStopSound(CPhysical* entity) {
    if (entity && AEAudioHardware.IsSoundBankLoaded(0x24u, 40)) {
        if (m_nChainsawSoundState != 3) {
            const auto volume = GetDefaultVolume(AE_WEAPON_CHAINSAW_ACTIVE);
            const auto flags = static_cast<eSoundEnvironment>(SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES);
            m_tempSound.Initialise(40, 2, this, entity->GetPosition(), volume, 0.66f, 1.0f, 1.0f, 0, flags);
            m_tempSound.RegisterWithPhysicalEntity(entity);
            m_tempSound.m_nEvent = AE_FRONTEND_PICKUP_DRUGS;
            AESoundManager.RequestNewSound(&m_tempSound);
        }
        m_nChainsawSoundState = 3;
    } else {
        m_nChainsawSoundState = 4;
    }
}

// 0x5046F0
void CAEWeaponAudioEntity::PlayCameraSound(CPhysical* entity, eAudioEvents event, float audability) {
    if (!AEAudioHardware.IsSoundBankLoaded(143, 5)) {
        if (!AudioEngine.IsLoadingTuneActive()) {
            AEAudioHardware.LoadSoundBank(143, 5);
        }
        return;
    }

    const auto vol = GetDefaultVolume(event) + audability;
    const auto flags = static_cast<eSoundEnvironment>(SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_UNPAUSABLE);
    m_tempSound.Initialise(5, 45, this, entity->GetPosition(), vol, 0.66f, 1.0f, 1.0f, 0, flags);
    m_tempSound.RegisterWithPhysicalEntity(entity);
    AESoundManager.RequestNewSound(&m_tempSound);
}

// 0x504B70
void CAEWeaponAudioEntity::UpdateParameters(CAESound *sound, int16 curPlayPos) {
    plugin::CallMethod<0x504B70, CAEWeaponAudioEntity*, CAESound*, int16>(this, sound, curPlayPos);
}

void CAEWeaponAudioEntity::InjectHooks() {
    RH_ScopedClass(CAEWeaponAudioEntity);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Constructor, 0x5DE990);
    RH_ScopedInstall(Destructor, 0x507560);
    RH_ScopedInstall(Initialise, 0x503450);
    RH_ScopedInstall(Reset, 0x503490);
    RH_ScopedInstall(Terminate, 0x503480);
    RH_ScopedInstall(WeaponFire, 0x504F80);
    RH_ScopedInstall(WeaponReload, 0x503690);
    RH_ScopedInstall(PlayChainsawStopSound, 0x504AA0);
    RH_ScopedInstall(PlayMiniGunStopSound, 0x504960);
    RH_ScopedInstall(PlayMiniGunFireSounds, 0x5047C0, { .reversed = false });
    RH_ScopedInstall(PlayCameraSound, 0x5046F0);
    RH_ScopedInstall(PlayWeaponLoopSound, 0x504610);
    RH_ScopedInstall(PlayFlameThrowerSounds, 0x504470, { .reversed = false });
    RH_ScopedInstall(PlayGunSounds, 0x503CE0, { .reversed = false });
    RH_ScopedInstall(ReportStealthKill, 0x503B20);
    RH_ScopedInstall(ReportChainsawEvent, 0x503910, { .reversed = false });
    RH_ScopedInstall(PlayFlameThrowerIdleGasLoop, 0x503870);
    RH_ScopedInstall(PlayGoggleSound, 0x503500);
    RH_ScopedInstall(StopFlameThrowerIdleGasLoop, 0x5034E0);
    RH_ScopedInstall(UpdateParameters, 0x504B70, { .reversed = false });

}

CAEWeaponAudioEntity* CAEWeaponAudioEntity::Constructor() {
    this->CAEWeaponAudioEntity::CAEWeaponAudioEntity();
    return this;
}

CAEWeaponAudioEntity* CAEWeaponAudioEntity::Destructor() {
    this->CAEWeaponAudioEntity::~CAEWeaponAudioEntity();
    return this;
}

// 0x504B70