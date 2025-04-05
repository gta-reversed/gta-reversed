/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "AEWeaponAudioEntity.h"
#include "AEAudioEnvironment.h"
#include "AESoundManager.h"
#include "AEAudioHardware.h"
#include "AEAudioUtility.h"

constexpr std::array<float, 2> gfWeaponPlaneFrequencyVariations{ 1.08f, 1.0f };

void CAEWeaponAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEWeaponAudioEntity, 0x86C2AC, 2);
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
    RH_ScopedInstall(PlayMiniGunFireSounds, 0x5047C0);
    RH_ScopedInstall(PlayCameraSound, 0x5046F0);
    RH_ScopedInstall(PlayWeaponLoopSound, 0x504610);
    RH_ScopedInstall(PlayFlameThrowerSounds, 0x504470);
    RH_ScopedInstall(PlayGunSounds, 0x503CE0);
    RH_ScopedInstall(ReportStealthKill, 0x503B20);
    RH_ScopedInstall(ReportChainsawEvent, 0x503910);
    RH_ScopedInstall(PlayFlameThrowerIdleGasLoop, 0x503870);
    RH_ScopedInstall(PlayGoggleSound, 0x503500);
    RH_ScopedInstall(StopFlameThrowerIdleGasLoop, 0x5034E0);
    RH_ScopedVMTInstall(UpdateParameters, 0x504B70);
}

// 0x503450
void CAEWeaponAudioEntity::Initialise() {
    m_MiniGunState  = eMiniGunState::STOPPED;
    m_ChainsawState = eChainsawState::STOPPED;
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
void CAEWeaponAudioEntity::WeaponFire(eWeaponType weaponType, CPhysical* parent, eAudioEvents audioEvent) {
    if (!parent) {
        return;
    }

    switch (weaponType) {
    case WEAPON_PISTOL:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_EAGLE_DRYFIRE,
            SND_GENRL_WEAPONS_EAGLE_LOWS,
            SND_GENRL_WEAPONS_EAGLE_SHOT_L,
            SND_GENRL_WEAPONS_EAGLE_SHOT_R,
            SND_GENRL_WEAPONS_EAGLE_TAIL_L,
            audioEvent,
            0.0f,
            1.41421f
        );
    case WEAPON_PISTOL_SILENCED:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_SILENCED_DRYFIRE,
            SND_GENRL_WEAPONS_SILENCED_LOWS,
            SND_GENRL_WEAPONS_SILENCED_SHOT_L,
            SND_GENRL_WEAPONS_SILENCED_SHOT_L,
            SND_GENRL_WEAPONS_UNK,
            audioEvent,
            -7.f
        );
    case WEAPON_DESERT_EAGLE:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_EAGLE_DRYFIRE,
            SND_GENRL_WEAPONS_EAGLE_LOWS,
            SND_GENRL_WEAPONS_EAGLE_SHOT_L,
            SND_GENRL_WEAPONS_EAGLE_SHOT_R,
            SND_GENRL_WEAPONS_EAGLE_TAIL_L,
            audioEvent,
            0.f,
            0.94387001f
        );
    case WEAPON_SHOTGUN:
    case WEAPON_SPAS12_SHOTGUN:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_SHOTGUN_DRYFIRE,
            SND_GENRL_WEAPONS_SHOTGUN_LOWS,
            SND_GENRL_WEAPONS_SHOTGUN_SHOT_L,
            SND_GENRL_WEAPONS_SHOTGUN_SHOT_R,
            SND_GENRL_WEAPONS_SHOTGUN_TAIL_R,
            audioEvent,
            0.f
        );
    case WEAPON_SAWNOFF_SHOTGUN:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_SHOTGUN_DRYFIRE,
            SND_GENRL_WEAPONS_SHOTGUN_LOWS,
            SND_GENRL_WEAPONS_SHOTGUN_SHOT_L,
            SND_GENRL_WEAPONS_SHOTGUN_SHOT_R,
            SND_GENRL_WEAPONS_SHOTGUN_TAIL_R,
            audioEvent,
            0.0f,
            0.79369998f,
            0.93f
        );
    case WEAPON_MICRO_UZI:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_9MM1DRY,
            SND_GENRL_WEAPONS_9MM1LOWS,
            SND_GENRL_WEAPONS_9MM1SHOT_L,
            SND_GENRL_WEAPONS_9MM1SHOT_R,
            SND_GENRL_WEAPONS_9MM1TAIL,
            audioEvent
        );
    case WEAPON_MP5:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_9MM1DRY,
            SND_GENRL_WEAPONS_9MM1LOWS,
            SND_GENRL_WEAPONS_MP5SHOT_L,
            SND_GENRL_WEAPONS_MP5SHOT_R,
            SND_GENRL_WEAPONS_9MM1TAIL,
            audioEvent
        );
    case WEAPON_AK47:
    case WEAPON_M4:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_AK_DRYFIRE,
            SND_GENRL_WEAPONS_EAGLE_LOWS,
            SND_GENRL_WEAPONS_AK_SHOT_L,
            SND_GENRL_WEAPONS_AK_SHOT_R,
            SND_GENRL_WEAPONS_AK_TAIL_L,
            audioEvent
        );
    case WEAPON_TEC9:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_9MM1DRY,
            SND_GENRL_WEAPONS_9MM1LOWS,
            SND_GENRL_WEAPONS_9MM1SHOT_L,
            SND_GENRL_WEAPONS_9MM1SHOT_R,
            SND_GENRL_WEAPONS_9MM1TAIL,
            audioEvent,
            0.0f,
            1.25992f
        );
    case WEAPON_COUNTRYRIFLE:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_EAGLE_DRYFIRE,
            SND_GENRL_WEAPONS_EAGLE_LOWS,
            SND_GENRL_WEAPONS_SNIPER_SHOT_L,
            SND_GENRL_WEAPONS_SNIPER_SHOT_R,
            SND_GENRL_WEAPONS_SHOTGUN_TAIL_R,
            audioEvent,
            0.0f,
            0.89f
        );
    case WEAPON_SNIPERRIFLE:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_EAGLE_DRYFIRE,
            SND_GENRL_WEAPONS_EAGLE_LOWS,
            SND_GENRL_WEAPONS_SNIPER_SHOT_L,
            SND_GENRL_WEAPONS_SNIPER_SHOT_R,
            SND_GENRL_WEAPONS_SHOTGUN_TAIL_R,
            audioEvent
        );
    case WEAPON_FLAMETHROWER: {
        if (!std::exchange(m_LastFlameThrowerFireTimeMs, CTimer::GetTimeInMS())) {
            PlayFlameThrowerSounds(
                parent,
                SND_GENRL_WEAPONS_TRIGGER,
                SND_GENRL_WEAPONS_SNIPER_SHOT_L,
                audioEvent,
                -14.f,
                1.f
            );
        }
        break;
    }
    case WEAPON_MINIGUN:
        return PlayMiniGunFireSounds(parent, audioEvent);
    case WEAPON_DETONATOR:
        return PlayGunSounds(
            parent,
            SND_GENRL_WEAPONS_DETONATOR_BEEP,
            SND_GENRL_WEAPONS_UNK,
            SND_GENRL_WEAPONS_UNK,
            SND_GENRL_WEAPONS_UNK,
            SND_GENRL_WEAPONS_UNK,
            audioEvent,
            -14.0f
        );
    case WEAPON_SPRAYCAN:
        if (!std::exchange(m_LastSprayCanFireTimeMs, CTimer::GetTimeInMS())) {
            PlayWeaponLoopSound(
                parent,
                SND_GENRL_WEAPONS_SPRAY_PAINT,
                audioEvent,
                -20.0,
                1.0f,
                AE_WEAPON_SOUND_CAT_SPRAY
            );
        }
        break;
    case WEAPON_EXTINGUISHER:
        if (!std::exchange(m_LastFireExtFireTimeMs, CTimer::GetTimeInMS())) {
            PlayWeaponLoopSound(
                parent,
                SND_GENRL_WEAPONS_FIRE_EXT,
                audioEvent,
                -20.0,
                0.79369998f,
                AE_WEAPON_SOUND_CAT_EXT
            );
        }
        break;
    case WEAPON_CAMERA:
        return PlayCameraSound(parent, audioEvent, -14.0);
    case WEAPON_NIGHTVISION:
    case WEAPON_INFRARED:
        return PlayGoggleSound(SND_GENRL_WEAPONS_NIGHT_VISION, audioEvent);
    case WEAPON_BRASSKNUCKLE:
    case WEAPON_GOLFCLUB:
    case WEAPON_NIGHTSTICK:
    case WEAPON_KNIFE:
    case WEAPON_BASEBALLBAT:
    case WEAPON_SHOVEL:
    case WEAPON_POOL_CUE:
    case WEAPON_KATANA:
    case WEAPON_CHAINSAW:
    case WEAPON_DILDO1:
    case WEAPON_DILDO2:
    case WEAPON_VIBE1:
    case WEAPON_VIBE2:
    case WEAPON_FLOWERS:
    case WEAPON_CANE:
    case WEAPON_GRENADE:
    case WEAPON_TEARGAS:
    case WEAPON_MOLOTOV:
    case WEAPON_ROCKET:
    case WEAPON_ROCKET_HS:
    case WEAPON_FREEFALL_BOMB:
    case WEAPON_RLAUNCHER:
    case WEAPON_RLAUNCHER_HS:
    case WEAPON_REMOTE_SATCHEL_CHARGE:
    case WEAPON_PARACHUTE:
        return;
    default:
        NOTSA_UNREACHABLE();
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

    const auto PlaySound = [&](eSoundID soundID, float volumeOffsetdB = 0.f) {
        CAESound s;
        s.Initialise(
            5,
            soundID,
            this,
            entity->GetPosition(),
            GetDefaultVolume(event) + volumeOffsetdB,
            2.f / 3.f,
            1.0f,
            1.0f,
            0,
            SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY
        );
        s.RegisterWithPhysicalEntity(entity);
        AESoundManager.RequestNewSound(&s);
    };

    switch (type) {
    case WEAPON_PISTOL:
    case WEAPON_PISTOL_SILENCED:
        return PlaySound(event == AE_WEAPON_RELOAD_A ? 55 : 66);
    case WEAPON_DESERT_EAGLE:
        return PlaySound(event == AE_WEAPON_RELOAD_A ? 55 : 51);
    case WEAPON_SHOTGUN:
    case WEAPON_SPAS12_SHOTGUN:
        return PlaySound(event == AE_WEAPON_RELOAD_A ? 72 : 71);
    case WEAPON_SAWNOFF_SHOTGUN:
        return PlaySound(event == AE_WEAPON_RELOAD_A ? 70 : 69);
    case WEAPON_MICRO_UZI:
    case WEAPON_MP5:
    case WEAPON_TEC9:
        return PlaySound(event == AE_WEAPON_RELOAD_A ? 85 : 84);
    case WEAPON_AK47:
    case WEAPON_M4:
        return PlaySound(event == AE_WEAPON_RELOAD_A ? 32 : 31);
    case WEAPON_COUNTRYRIFLE:
        if (event == AE_WEAPON_RELOAD_A) {
            return PlaySound(32, -6.f);
        }
        break;
    case WEAPON_SNIPERRIFLE:
        return PlaySound(event == AE_WEAPON_RELOAD_A ? 55 : 32);
    }
}


// 0x503CE0
void CAEWeaponAudioEntity::PlayGunSounds(
    CPhysical*   entity,
    int16        dryFireSfxID,
    int16        subSfxID,
    int16        mainLeftSfxID,
    int16        mainRightSfxID,
    int16        tailSfxID,
    eAudioEvents audioEvent,
    float        volumeOffsetdB,
    float        primarySpeed,
    float        tailFrequencyScalingFactor
) {
    if (!AEAudioHardware.IsSoundBankLoaded(143u, 5)) {
        if (!AudioEngine.IsLoadingTuneActive()) {
            AEAudioHardware.LoadSoundBank(143, 5);
        }
        return;
    }

    if (CTimer::GetTimeInMS() < m_LastGunFireTimeMs + 25) {
        return;
    }
    m_LastGunFireTimeMs = CTimer::GetTimeInMS();

    auto baseVolume = GetDefaultVolume(audioEvent) + volumeOffsetdB;
    auto [baseRollOffFactor, baseSpeed] = [&]() -> std::pair<float, float> {
        switch (audioEvent) {
        case AE_WEAPON_FIRE_PLANE: {
            m_LastWeaponPlaneFrequencyIndex = (m_LastWeaponPlaneFrequencyIndex + 1) % 2;
            return {1.6f, gfWeaponPlaneFrequencyVariations[m_LastWeaponPlaneFrequencyIndex] * primarySpeed * 0.7937f};
        }
        case AE_WEAPON_FIRE_MINIGUN_PLANE: {
            return {1.8f, primarySpeed * 0.7937f};
        }
        default: {
            return {1.f, primarySpeed};
        }
        }
    }();

    const auto PlaySound = [&](int16 sfxID, CVector pos, float volume, float rollOffFactor, float speed, uint32 flags, eAudioEvents audioEventOverride = AE_UNDEFINED) {
        CAESound s;
        s.Initialise(
            5,
            sfxID,
            this,
            pos,
            volume,
            rollOffFactor,
            speed,
            0.f,
            0,
            flags
        );
        if (flags & SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY) {
            s.RegisterWithPhysicalEntity(entity);
        }
        if (audioEventOverride == AE_UNDEFINED) {
            switch (audioEvent) {
            case AE_WEAPON_FIRE_MINIGUN_PLANE:
            case AE_WEAPON_FIRE_MINIGUN_AMMO:
                s.m_nEvent = AE_FRONTEND_PICKUP_WEAPON;
            }
        } else {
            s.m_nEvent = audioEventOverride;
        }
        AESoundManager.RequestNewSound(&s);
    };

    if (dryFireSfxID != -1) { // 0x503DFF
        PlaySound(
            dryFireSfxID,
            entity->GetPosition(),
            baseVolume,
            baseRollOffFactor * (2.f / 3.f),
            baseSpeed,
            SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES
        );
    }
    if (subSfxID != -1) { // 0x503E93
        PlaySound(
            subSfxID,
            entity->GetPosition(),
            baseVolume,
            baseRollOffFactor * 0.9f,
            baseSpeed,
            SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES
        );
    }

    // 0x503F21
    float frontEndVolume = -100.f;
    if (!notsa::contains({ AE_WEAPON_FIRE_PLANE, AE_WEAPON_FIRE_MINIGUN_PLANE }, audioEvent)) {
        const auto dist = CAEAudioEnvironment::GetPositionRelativeToCamera(entity).Magnitude() / (baseRollOffFactor * 1.25f);
        if (dist < (5.f / baseRollOffFactor)) { // Inverted
            baseVolume    -= 3.f;
            frontEndVolume = baseVolume + CAEAudioEnvironment::GetDistanceAttenuation(dist);
        } else if (dist < (12.f / baseRollOffFactor)) {
            const auto t   = ((12.f / baseRollOffFactor) - dist) / (12.f / baseRollOffFactor) - (5.f / baseRollOffFactor);
            frontEndVolume = baseVolume + CAEAudioEnvironment::GetDistanceAttenuation(dist) + std::log10f(t * (SQRT_2 / 2.f)) * 20.f;
            baseVolume    += std::log10f(((1.f - t) * 0.2929f) + (SQRT_2 / 2.f)) * 20.f;
        }
    }

    const auto PlayMainSound = [
        &,
        mainSoundSpeed = (CAEAudioUtility::GetRandomNumberInRange(-0.02f, 0.02f) + 1.f) * baseSpeed
    ](int16 sfxID, bool isRight) {
        PlaySound(
            sfxID,
            CVector{isRight ? 1.f : -1.f, 0.f, 0.f},
            frontEndVolume,
            baseRollOffFactor * 1.25f,
            mainSoundSpeed,
            SOUND_FORCED_FRONT | SOUND_REQUEST_UPDATES | SOUND_FRONT_END
        );
        PlaySound(
            sfxID,
            entity->GetPosition(),
            baseVolume,
            baseRollOffFactor * 1.25f,
            mainSoundSpeed,
            SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES
        );
    };
    if (mainLeftSfxID != -1) { // 0x5040DC + 0x5041DF
        PlayMainSound(mainLeftSfxID, false);
    }
    if (mainRightSfxID != -1) { // 0x504153 + 0x504218
        PlayMainSound(mainRightSfxID, true);
    }

    if (tailSfxID != -1 && !notsa::contains({AE_WEAPON_FIRE_PLANE, AE_WEAPON_FIRE_MINIGUN_PLANE}, audioEvent)) {
        const auto tailSoundRollOff    = baseRollOffFactor * 3.5f;
        const auto tailSoundVolume     = CAEAudioEnvironment::GetDistanceAttenuation(CAEAudioEnvironment::GetPositionRelativeToCamera(entity).Magnitude() / tailSoundRollOff) + baseVolume - 20.f;
        const auto tailSoundAudioEvent = audioEvent == AE_WEAPON_FIRE_MINIGUN_AMMO
            ? AE_FRONTEND_PICKUP_MONEY
            : AE_FRONTEND_SELECT;
        const auto PlayTailSound = [&](bool isRight, float speedMult) {
            PlaySound(
                tailSfxID,
                CVector{ isRight ? 1.f : -1, 0.f, 0.f },
                tailSoundVolume,
                tailSoundRollOff,
                tailFrequencyScalingFactor * speedMult,
                SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_REQUEST_UPDATES | SOUND_FRONT_END,
                tailSoundAudioEvent
            );
        };
        float speedMultA = 1.f,
              speedMultB = 1.1892101f;
        if (CAEAudioUtility::ResolveProbability(0.5f)) {
            std::swap(speedMultA, speedMultB);
        }
        PlayTailSound(false, speedMultA);
        PlayTailSound(true,  speedMultB);
    }
}

// 0x503B20
void CAEWeaponAudioEntity::ReportStealthKill(eWeaponType type, CPhysical* entity, eAudioEvents event) {
    if (type != WEAPON_KNIFE) {
        return;
    }

    const auto PlayStealthKillSound = [&](int16 bankSlotID, eSoundID soundID, float volumeOffsetdB, int32 eventID) {
        AESoundManager.PlaySound({
            .BankSlotID         = bankSlotID,
            .SoundID            = soundID,
            .AudioEntity        = this,
            .Pos                = entity->GetPosition(),
            .Volume             = GetDefaultVolume(event) + volumeOffsetdB,
            .Speed              = 0.0f, // ???
            .Flags              = SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES,
            .RegisterWithEntity = entity,
            .EventID            = eventID,
            .ClientVariable     = (float)CTimer::GetTimeInMS(),
        });
    };

    if (!AEAudioHardware.IsSoundBankLoaded(143, 5)) {
        if (!AudioEngine.IsLoadingTuneActive()) {
            AEAudioHardware.LoadSoundBank(143, 5);
        }
        return;
    }
    PlayStealthKillSound(5, 81, -6.f, AE_WEAPON_SOUND_CAT_STEALTH_KNIFE_IN);

    if (!AEAudioHardware.IsSoundBankLoaded(39, 2)) {
        if (!AudioEngine.IsLoadingTuneActive()) {
            AEAudioHardware.LoadSoundBank(39, 2);
        }
        return;
    }
    PlayStealthKillSound(2, 471, 0.f, AE_WEAPON_SOUND_CAT_STEALTH_KNIFE_OUT);
}

// 0x503910
void CAEWeaponAudioEntity::ReportChainsawEvent(CPhysical* entity, eAudioEvents audioEvent) {
    if (!AEAudioHardware.IsSoundBankLoaded(36, 40)) {
        if (AESoundManager.AreSoundsPlayingInBankSlot(40)) {
            AESoundManager.CancelSoundsInBankSlot(40, false);
        }
        AEAudioHardware.LoadSoundBank(36, 40);
        return;
    }

    const auto PlaySoundIfNotPlaying = [&](int16 sfxID, float rollOffFactor, eWeaponSoundCategories soundCat) {
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(soundCat, this) != SOUND_NOT_PLAYING) {
            return;
        }
        CAESound s;
        s.Initialise(
            40,
            sfxID,
            this,
            entity->GetPosition(),
            GetDefaultVolume(audioEvent),
            rollOffFactor,
            1.f,
            0.f,
            0,
            SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES
        );
        s.m_nEvent = soundCat;
        s.RegisterWithPhysicalEntity(entity);
        AESoundManager.RequestNewSound(&s);
    };

    switch (audioEvent) {
    case AE_WEAPON_CHAINSAW_IDLE: { // 0x503A6E
        switch (m_ChainsawState) {
        case eChainsawState::STOPPED:
        case eChainsawState::IDLE: {
            PlaySoundIfNotPlaying(1, 2.f / 3.f, AE_WEAPON_SOUND_CAT_CHAINSAW_IDLE);

            m_ChainsawState           = eChainsawState::IDLE;
            m_LastChainsawEventTimeMs = CTimer::GetTimeInMS();
        }
        }
        break;
    }
    case AE_WEAPON_CHAINSAW_ACTIVE: { // 0x5039C0
        switch (m_ChainsawState) {
        case eChainsawState::STOPPING:
        case eChainsawState::ACTIVE:
        case eChainsawState::IDLE: {
            PlaySoundIfNotPlaying(0, 1.25f, AE_WEAPON_SOUND_CAT_CHAINSAW_ACTIVE);

            m_ChainsawState           = eChainsawState::ACTIVE;
            m_LastChainsawEventTimeMs = CTimer::GetTimeInMS();
        }
        }
        break;
    }
    case AE_WEAPON_CHAINSAW_CUTTING: { // 0x503996
        switch (m_ChainsawState) {
        case eChainsawState::ACTIVE:
        case eChainsawState::CUTTING: {
            m_ChainsawState           = eChainsawState::CUTTING;
            m_LastChainsawEventTimeMs = CTimer::GetTimeInMS();
        }
        }
        break;
    }
    }
}

// 0x504610
void CAEWeaponAudioEntity::PlayWeaponLoopSound(CPhysical* entity, int16 sfxId, eAudioEvents startEvent, float volumeOffsetdB, float speed, eWeaponSoundCategories soundCat) {
    if (!AEAudioHardware.IsSoundBankLoaded(143u, 5)) {
        if (!AudioEngine.IsLoadingTuneActive()) {
            AEAudioHardware.LoadSoundBank(143, 5);
        }
        return;
    }
    AESoundManager.PlaySound({
        .BankSlotID         = 5,
        .SoundID            = sfxId,
        .AudioEntity        = this,
        .Pos                = entity->GetPosition(),
        .Volume             = GetDefaultVolume(startEvent) + volumeOffsetdB,
        .Speed              = speed,
        .Flags              = SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_START_PERCENTAGE | SOUND_REQUEST_UPDATES,
        .RegisterWithEntity = entity,
        .EventID            = soundCat,
    });
}

// 0x504960
void CAEWeaponAudioEntity::PlayMiniGunStopSound(CPhysical* entity) {
    if (!entity) {
        m_MiniGunState = eMiniGunState::STOPPED;
        return;
    }

    if (m_MiniGunState == eMiniGunState::STOPPING) {
        return;
    }

    if (!AEAudioHardware.IsSoundBankLoaded(143u, 5)) {
        if (!AudioEngine.IsLoadingTuneActive()) {
            AEAudioHardware.LoadSoundBank(143, 5);
        }
        return;
    }

    const auto PlayMiniGunFireStopSound = [&](float speed, float maxDist) {
        CAESound s;
        s.Initialise(
            5,
            63,
            this,
            entity->GetPosition(),
            GetDefaultVolume(AE_WEAPON_FIRE_MINIGUN_STOP),
            maxDist * 2.f / 3.f, // *0.66f....f
            speed,
            1.0f,
            0,
            SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES
        );
        s.RegisterWithPhysicalEntity(entity);
        s.m_nEvent = AE_FRONTEND_PICKUP_HEALTH; // ???
        AESoundManager.RequestNewSound(&s);
    };
    if (entity->IsVehicle() && entity->AsVehicle()->IsSubPlane()) {
        PlayMiniGunFireStopSound(1.8f, 0.7937f);
    } else {
        PlayMiniGunFireStopSound(1.f, 1.f);
    }
    m_MiniGunState = eMiniGunState::STOPPING;
}

// 0x5047C0
void CAEWeaponAudioEntity::PlayMiniGunFireSounds(CPhysical* entity, eAudioEvents audioEvent) {
    const auto PlayMiniGunFireSound = [&](eAudioEvents gunFireAE) {
        m_MiniGunState = eMiniGunState::FIRING;
        if (!std::exchange(m_IsMiniGunFireActive, true)) {
            PlayGunSounds(entity, 15, 16, 11, 12, 13, gunFireAE, 0.f, 1.f, 1.f);
        }
    };
    switch (audioEvent) {
    case AE_WEAPON_FIRE:
    case AE_WEAPON_FIRE_MINIGUN_AMMO: { // 0x504912
        PlayMiniGunFireSound(AE_WEAPON_FIRE_MINIGUN_AMMO);
        break;
    }
    case AE_WEAPON_FIRE_PLANE: { // 0x5047E6
        PlayMiniGunFireSound(AE_WEAPON_FIRE_MINIGUN_PLANE);
        break;
    }
    case AE_WEAPON_FIRE_MINIGUN_NO_AMMO: { // 0x5048FC - Minigun is just spinning
        if (!m_IsMiniGunSpinActive) {
            if (!AEAudioHardware.IsSoundBankLoaded(143, 5)) { // SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN
                if (!AudioEngine.IsLoadingTuneActive()) {
                    AEAudioHardware.LoadSoundBank(143, 5);
                }
                break;
            }
            AESoundManager.PlaySound({
                .BankSlotID         = 5,
                .SoundID            = 14,
                .AudioEntity        = this,
                .Pos                = entity->GetPosition(),
                .Volume             = GetDefaultVolume(AE_WEAPON_FIRE_MINIGUN_NO_AMMO),
                .RollOffFactor      = 2.f / 3.f,
                .Speed              = 1.f,
                .Doppler            = 0.f,
                .Flags              = SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES,
                .RegisterWithEntity = entity,
                .EventID            = AE_WEAPON_SOUND_CAT_MINIGUN_SPIN
            });
            m_IsMiniGunSpinActive = true;
        }
        m_MiniGunState = eMiniGunState::SPINNING;
        break;
    }
    }
    m_LastMiniGunFireTimeMs = CTimer::GetTimeInMS();
}

// 0x503500
void CAEWeaponAudioEntity::PlayGoggleSound(int16 sfxId, eAudioEvents event) {
    if (!AEAudioHardware.IsSoundBankLoaded(143u, 5)) {
        if (!AudioEngine.IsLoadingTuneActive()) {
            AEAudioHardware.LoadSoundBank(143, 5);
        }
        return;
    }
    const auto PlaySound = [&](float offsetX, float speed) {
        CAESound s;
        s.Initialise(5, sfxId, this, { offsetX, 0.0f, 0.0f }, GetDefaultVolume(event) - 9.f, 1.0f, speed, 1.0f, 0, SOUND_DEFAULT);
        s.m_nEnvironmentFlags = SOUND_FRONT_END | SOUND_FORCED_FRONT;
        AESoundManager.RequestNewSound(&s);
    };
    const auto r = CAEAudioUtility::ResolveProbability(0.5f);
    PlaySound(-1.0f, r ? 1.1892101f : 1.f);
    PlaySound(+1.0f, r ? 1.f : 1.1892101f);
}

// 0x504470
void CAEWeaponAudioEntity::PlayFlameThrowerSounds(CPhysical* entity, int16 dryFireSfxID, int16 flameSfxID, eAudioEvents audioEvent, float volumeOffsetdB, float speed) {
    const auto PlaySound = [&](int16 bankSlotID, int16 sfxID, float sfxVolumeOffsetdB, float rollOffFactor, float sfxSpeed, float sfxFreqVariance, eAudioEvents sfxAE) {
        CAESound s;
        s.Initialise(
            bankSlotID,
            sfxID,
            this,
            entity->GetPosition(),
            GetDefaultVolume(audioEvent) + volumeOffsetdB + sfxVolumeOffsetdB,
            rollOffFactor,
            sfxSpeed,
            1.f,
            0,
            SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY|SOUND_REQUEST_UPDATES,
            sfxFreqVariance
        );
        s.m_nEvent = sfxAE;
        s.RegisterWithPhysicalEntity(entity);
        AESoundManager.RequestNewSound(&s);
    };

    if (!AEAudioHardware.IsSoundBankLoaded(143u, 5)) {
        if (!AudioEngine.IsLoadingTuneActive()) {
            AEAudioHardware.LoadSoundBank(143, 5);
        }
        return;
    }
    PlaySound(
        5,
        dryFireSfxID,
        -6.f,
        2.f / 3.f,
        speed,
        0.02f,
        AE_UNDEFINED
    );

    if (!AEAudioHardware.IsSoundBankLoaded(138, 19)) {
        AEAudioHardware.LoadSoundBank(138, 19);
        return;
    }
    PlaySound(
        19,
        flameSfxID,
        -20.f,
        2.f,
        speed * 0.79369998f,
        0.f,
        AE_FRONTEND_BACK
    );
}

// 0x503870
void CAEWeaponAudioEntity::PlayFlameThrowerIdleGasLoop(CPhysical* entity) {
    if (m_FlameThrowerIdleGasLoopSound != nullptr) {
        return;
    }
    m_FlameThrowerIdleGasLoopSound = AESoundManager.PlaySound({
        .BankSlotID         = 5,
        .SoundID            = 10,
        .AudioEntity        = this,
        .Pos                = entity->GetPosition(),
        .Volume             = GetDefaultVolume(AE_FLAMETHROWER_IDLE),
        .RollOffFactor      = 2.f / 3.f,
        .Flags              = SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES,
        .RegisterWithEntity = entity
    });
}

// 0x5034E0
void CAEWeaponAudioEntity::StopFlameThrowerIdleGasLoop() {
    if (auto* const s = std::exchange(m_FlameThrowerIdleGasLoopSound, nullptr)) {
        s->StopSoundAndForget();
    }
}

// 0x504AA0
void CAEWeaponAudioEntity::PlayChainsawStopSound(CPhysical* entity) {
    if (entity && AEAudioHardware.IsSoundBankLoaded(36, 40)) {
        if (m_ChainsawState != eChainsawState::STOPPING) {
            AESoundManager.PlaySound({
                .BankSlotID         = 40,
                .SoundID            = 2,
                .AudioEntity        = this,
                .Pos                = entity->GetPosition(),
                .Volume             = GetDefaultVolume(AE_WEAPON_CHAINSAW_ACTIVE),
                .RollOffFactor      = 2.f / 3.f,
                .Flags              = SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES,
                .RegisterWithEntity = entity,
                .EventID            = AE_WEAPON_SOUND_CAT_CHAINSAW_STOP
            });
        }
        m_ChainsawState = eChainsawState::STOPPING;
    } else {
        m_ChainsawState = eChainsawState::STOPPED;
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
    AESoundManager.PlaySound({
        .BankSlotID         = 5,
        .SoundID            = 45,
        .AudioEntity        = this,
        .Pos                = entity->GetPosition(),
        .Volume             = GetDefaultVolume(event),
        .RollOffFactor      = 2.f / 3.f,
        .Flags              = SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_UNPAUSABLE, // NOTE/BUG: `UNPAUSABLE` typo?
        .RegisterWithEntity = entity,
        .EventID            = AE_WEAPON_SOUND_CAT_CHAINSAW_STOP
    });
}

// 0x504B70
void CAEWeaponAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (curPlayPos == -1) {
        if (sound == m_FlameThrowerIdleGasLoopSound) {
            m_FlameThrowerIdleGasLoopSound = nullptr;
        } else if (sound->m_nEvent == AE_WEAPON_SOUND_CAT_MINIGUN_STOP && m_MiniGunState == eMiniGunState::STOPPING) {
            m_MiniGunState = eMiniGunState::STOPPED;
        }
        return;
    }
     
    switch (sound->m_nEvent) {
    case AE_WEAPON_SOUND_CAT_TAIL: { // 0x504BAA
        if (!CGame::CanSeeOutSideFromCurrArea()) {
            sound->m_fVolume -= 1.f;
        }
        break;
    }
    case AE_WEAPON_SOUND_CAT_FLAME: { // 0x504BC3
        if (m_LastFlameThrowerFireTimeMs + 300 >= CTimer::GetTimeInMS()) {
            sound->m_fVolume = std::max(GetDefaultVolume(AE_WEAPON_FIRE), sound->m_fVolume + 2.f); // TODO: Use TimeStep
        } else {
            sound->StopSoundAndForget();
            m_LastFlameThrowerFireTimeMs = 0;
        }
        break;
    }
    case AE_WEAPON_SOUND_CAT_SPRAY: { // 0x504C38
        if (m_LastSprayCanFireTimeMs + 300 < CTimer::GetTimeInMS()) {
            sound->StopSoundAndForget();
            m_LastSprayCanFireTimeMs = 0;
        }
        break;
    }
    case AE_WEAPON_SOUND_CAT_EXT: { // 0x504C5F
        if (m_LastFireExtFireTimeMs + 300 >= CTimer::GetTimeInMS()) {
            sound->m_fSpeed = (sound->m_fSpeed < 0.85f) ? std::min(sound->m_fSpeed + 0.01f, 0.85f) : sound->m_fSpeed; // TODO: Use TimeStep
        } else {
            sound->StopSoundAndForget();
            m_LastFireExtFireTimeMs = 0;
        }
        break;
    }
    case AE_WEAPON_SOUND_CAT_MINIGUN_SPIN: { // 0x504CBC
        if (m_LastMiniGunFireTimeMs + 300 < CTimer::GetTimeInMS()) {
            PlayMiniGunStopSound(sound->m_pPhysicalEntity->AsPhysical());
        }
        switch (m_MiniGunState) {
        case eMiniGunState::STOPPING:
        case eMiniGunState::STOPPED: {
            sound->StopSoundAndForget();
            m_IsMiniGunSpinActive = 0;
        }
        }
        break;
    }
    case AE_WEAPON_SOUND_CAT_MINIGUN_FIRE: { // 0x504CFB
        if (m_LastMiniGunFireTimeMs + 300 < CTimer::GetTimeInMS()) {
            PlayMiniGunStopSound(sound->m_pPhysicalEntity->AsPhysical());
        }
        if (m_MiniGunState != eMiniGunState::FIRING) {
            sound->StopSoundAndForget();
            m_IsMiniGunFireActive = 0;
        }
        break;
    }
    case AE_WEAPON_SOUND_CAT_MINIGUN_TAIL: { // 0x504D31
        if (m_MiniGunState != eMiniGunState::FIRING) {
            if (sound->m_fVolume <= -30.0) {
                sound->StopSoundAndForget();
            } else {
                sound->m_fVolume -= 1.5f;
            }
        }
        break;
    }
    case AE_WEAPON_SOUND_CAT_CHAINSAW_IDLE: { // 0x504D6B
        if (m_LastChainsawEventTimeMs + 300 < CTimer::GetTimeInMS()) {
            m_ChainsawState = eChainsawState::STOPPED;
        }
        if (m_ChainsawState != eChainsawState::IDLE) {
            sound->StopSoundAndForget();
        }
        break;
    }
    case AE_WEAPON_SOUND_CAT_CHAINSAW_ACTIVE: { // 0x504D9B
        switch (m_ChainsawState) {
        case eChainsawState::CUTTING: { // 0x504DA5
            if (m_LastChainsawEventTimeMs + 400 >= CTimer::m_snTimeInMilliseconds) {
                sound->m_fSpeed = std::max(0.85f, sound->m_fSpeed - 0.15f); // TODO: Use TimeStep
            } else {
                m_LastChainsawEventTimeMs = CTimer::m_snTimeInMilliseconds;
                m_ChainsawState           = eChainsawState::ACTIVE;
            }
            break;
        }
        case eChainsawState::ACTIVE: { // 0x504E0E
            sound->m_fSpeed = std::min(1.f, sound->m_fSpeed + 0.03f); // TODO: Use TimeStep
            if (m_LastChainsawEventTimeMs + 300 < CTimer::GetTimeInMS()) {
                PlayChainsawStopSound(sound->m_pPhysicalEntity->AsPhysical());
            }
            break;
        }
        default: {
            sound->StopSoundAndForget();
            break;
        }
        }
        break;
    }
    case AE_WEAPON_SOUND_CAT_CHAINSAW_STOP: { // 0x504E66
        if (m_ChainsawState == eChainsawState::STOPPING && curPlayPos > 1'000) {
            m_ChainsawState = eChainsawState::IDLE;
        }
        break;
    }
    case AE_WEAPON_SOUND_CAT_STEALTH_KNIFE_IN: { // 0x504E8B
        if ((uint32)sound->m_ClientVariable + 820 < CTimer::GetTimeInMS()) {
            sound->m_fSpeed = 0.84f;
        }
        break;
    }
    case AE_WEAPON_SOUND_CAT_STEALTH_KNIFE_OUT: { // 0x504EBF
        if ((uint32)sound->m_ClientVariable + 2200 < CTimer::GetTimeInMS()) {
            sound->m_fSpeed = 1.f;
        }
        break;
    }
    default: { // 0x504ECD
        sound->m_fVolume = std::max(0.f, sound->m_fVolume - 2.5f);
        break;
    }
    }
}

void CAEWeaponAudioEntity::Clear() {
    *this = CAEWeaponAudioEntity{};
}

CAEWeaponAudioEntity* CAEWeaponAudioEntity::Constructor() {
    CAEWeaponAudioEntity::CAEWeaponAudioEntity();
    return this;
}

CAEWeaponAudioEntity* CAEWeaponAudioEntity::Destructor() {
    CAEWeaponAudioEntity::~CAEWeaponAudioEntity();
    return this;
}
