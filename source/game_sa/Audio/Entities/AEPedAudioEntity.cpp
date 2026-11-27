#include "StdInc.h"

#include "AEPedAudioEntity.h"
#include "AEAudioHardware.h"
#include "AEAudioUtility.h"
#include "AECollisionAudioEntity.CollisionLookup.h"

// 0x5DE8D0
CAEPedAudioEntity::CAEPedAudioEntity() : CAEAudioEntity() {
    m_Ped           = nullptr;
    m_CanAddEvent   = false;

    m_JetPackSound0 = nullptr;
    m_JetPackSound1 = nullptr;
    m_JetPackSound2 = nullptr;
}

// (CEntity* entity)
// 0x4E0E80
void CAEPedAudioEntity::Initialise(CPed* ped) {
    m_Ped                       = ped;
    m_SwimSplashSoundID         = 0;
    m_LastSwimWakeTriggerTimeMs = 0;

    m_IsJetPackPlaying          = false;
    m_JetPackSound0 = nullptr;
    m_JetPackSound1 = nullptr;
    m_JetPackSound0Volume       = VOLUME_SILENCE;
    m_JetPackSound1Volume       = VOLUME_SILENCE;

    m_UnkSound                  = nullptr;
    m_UnkVolume                 = VOLUME_SILENCE;
    m_ShirtFlapVolume           = VOLUME_SILENCE;
    m_CanAddEvent               = true;
}

// 0x5B98A0
void CAEPedAudioEntity::StaticInitialise() {
    AEAudioHardware.LoadSoundBank(SND_BANK_FEET_GENERIC, SND_BANK_SLOT_FOOTSTEPS_GENERIC);
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_SWIMMING, SND_BANK_SLOT_SWIMMING);
}

// 0x4E1360
void CAEPedAudioEntity::Terminate() {
    m_CanAddEvent = false;
    m_Ped         = nullptr;
    StopJetPackSound();
    AESoundManager.CancelSoundsOwnedByAudioEntity(this, true);
    if (m_ShirtFlapSound.IsActive()) {
        m_ShirtFlapSound.StopSoundAndForget();
    }
}

// 0x4E2BB0
void CAEPedAudioEntity::AddAudioEvent(eAudioEvents event, float volume, float speed, CPhysical* entity, eSurfaceType surfaceId, int32 hitEvent, uint32 startDelayMs) {
    if (!m_CanAddEvent) {
        return;
    }

    if (!m_Ped) {
        return;
    }

    switch (event) {
    case AE_PED_FOOTSTEP_LEFT:
    case AE_PED_FOOTSTEP_RIGHT:
        HandleFootstepEvent(event, volume, speed, surfaceId);
        break;
    case AE_PED_SKATE_LEFT:
    case AE_PED_SKATE_RIGHT:
        HandleSkateEvent(event, volume, speed);
        break;
    case AE_PED_LAND_ON_FEET_AFTER_FALL:
    case AE_PED_COLLAPSE_AFTER_FALL:
        HandleLandingEvent(event);
        break;
    case AE_PED_SWING:
        HandlePedSwing(event, hitEvent, startDelayMs);
        break;
    case AE_PED_HIT_HIGH:
    case AE_PED_HIT_LOW:
    case AE_PED_HIT_GROUND:
    case AE_PED_HIT_GROUND_KICK:
    case AE_PED_HIT_HIGH_UNARMED:
    case AE_PED_HIT_LOW_UNARMED:
    case AE_PED_HIT_MARTIAL_PUNCH:
    case AE_PED_HIT_MARTIAL_KICK:
        HandlePedHit(event, entity, surfaceId, volume, startDelayMs);
        break;
    case AE_PED_JACKED_CAR_PUNCH:
    case AE_PED_JACKED_CAR_HEAD_BANG:
    case AE_PED_JACKED_CAR_KICK:
    case AE_PED_JACKED_BIKE:
    case AE_PED_JACKED_DOZER:
        HandlePedJacked(event);
        break;
    case AE_PED_SWIM_STROKE_SPLASH:
    case AE_PED_SWIM_DIVE_SPLASH:
        HandleSwimSplash(event);
        break;
    case AE_PED_SWIM_WAKE:
        HandleSwimWake(event);
        break;
    case AE_PED_CRUNCH: {
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS)) {
            break;
        }

        volume += GetDefaultVolume(AE_PED_CRUNCH);

        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_PED_CRUNCH, this)) {
            break;
        }
        AESoundManager.PlaySound({ .BankSlotID = SND_BANK_SLOT_COLLISIONS, .SoundID = 29, .AudioEntity = this, .Pos = entity->GetPosition(), .Volume = volume, .RollOffFactor = 1.5f, .Speed = speed, .Doppler = 1.0f, .FrameDelay = 0, .Flags = SOUND_DEFAULT, .FrequencyVariance = 0.06f, .PlayTime = 0, .EventID = AE_PED_CRUNCH });

        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_PED_KNOCK_DOWN, this) != 0) {
            break;
        }
        AESoundManager.PlaySound({ .BankSlotID = SND_BANK_SLOT_COLLISIONS, .SoundID = (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(47, 49)), .AudioEntity = this, .Pos = entity->GetPosition(), .Volume = volume, .RollOffFactor = 1.5f, .Speed = speed, .Doppler = 1.0f, .FrameDelay = 0, .Flags = SOUND_DEFAULT, .FrequencyVariance = 0.06f, .PlayTime = 0, .EventID = AE_PED_KNOCK_DOWN });
        break;
    }
    default:
        break;
    }
}

// 0x4E28A0
void CAEPedAudioEntity::TurnOnJetPack() {
    if (!m_Ped) {
        return;
    }

    if (m_IsJetPackPlaying || m_JetPackSound1 || m_JetPackSound0 || m_JetPackSound2) {
        return;
    }

    m_JetPackSound0Volume       = VOLUME_SILENCE;
    m_JetPackSound1Volume       = VOLUME_SILENCE;
    m_JetPackSound2Volume       = +2.000f;
    m_JetPackSoundSpeedMult = 0.400f;

    m_IsJetPackPlaying          = true;

    const auto PlayJetPackSound = [&](eSoundBankSlot slot, eSoundID soundID) {
        return AESoundManager.PlaySound({
            .BankSlotID  = slot,
            .SoundID     = soundID,
            .AudioEntity = this,
            .Pos         = m_Ped->GetPosition(),
            .Volume      = -100.f,
            .Flags       = SOUND_REQUEST_UPDATES,
        });
    };
    m_JetPackSound0 = PlayJetPackSound(SND_BANK_SLOT_VEHICLE_GEN, 26);
    m_JetPackSound1 = PlayJetPackSound(SND_BANK_SLOT_WEAPON_GEN, 10);
    m_JetPackSound2 = PlayJetPackSound(SND_BANK_SLOT_FRONTEND_GAME, 0);
}

// 0x4E2A70
void CAEPedAudioEntity::TurnOffJetPack() {
    StopJetPackSound();
    m_IsJetPackPlaying = false;
}

// 0x4E1120
void CAEPedAudioEntity::StopJetPackSound() {
    if (m_JetPackSound0) {
        m_JetPackSound0->StopSoundAndForget();
        m_JetPackSound0 = nullptr;
    }

    if (m_JetPackSound1) {
        m_JetPackSound1->StopSoundAndForget();
        m_JetPackSound1 = nullptr;
    }

    if (m_JetPackSound2) {
        m_JetPackSound2->StopSoundAndForget();
        m_JetPackSound2 = nullptr;
    }
}

// 0x4E0EE0
void CAEPedAudioEntity::UpdateJetPack(float thrustFwd, float thrustAngle) {
    if (!m_IsJetPackPlaying || !m_JetPackSound1 || !m_JetPackSound0 || !m_JetPackSound2) {
        return;
    }

    if (thrustFwd <= 0.5f) { // flying
        m_JetPackSound0Volume   = std::max(m_JetPackSound0Volume - 5.0f, VOLUME_SILENCE);
        m_JetPackSound1Volume   = std::min(m_JetPackSound1Volume + 6.0f, -17.0f);
        m_JetPackSound2Volume   = std::max(m_JetPackSound2Volume - 0.3f, 2.0f);
        m_JetPackSoundSpeedMult = std::max(m_JetPackSoundSpeedMult - 0.031f, 0.4f);
    } else { // idle
        m_JetPackSound0Volume   = std::min(m_JetPackSound0Volume + 15.0f, -15.0f);
        m_JetPackSound1Volume   = std::max(m_JetPackSound1Volume - 7.1f, VOLUME_SILENCE);
        m_JetPackSound2Volume   = std::min(m_JetPackSound2Volume + 0.3f, 11.0f);
        m_JetPackSoundSpeedMult = std::min(m_JetPackSoundSpeedMult + 0.031f, 0.71f);
    }

    const auto angle = std::sin(thrustAngle);
    const float speed = angle < 0.0f ? -angle : angle; // maybe wrong

    // 0.0f == 0xB61384 (uninitialized)
    m_JetPackSound0->m_Volume = m_JetPackSound0Volume + 0.0f;
    m_JetPackSound0->m_Speed = speed * -0.07f + 1.0f;

    m_JetPackSound1->m_Volume = m_JetPackSound1Volume + 0.0f;
    m_JetPackSound1->m_Speed = 0.56f;

    m_JetPackSound2->m_Volume = m_JetPackSound2Volume + 0.0f;
    m_JetPackSound2->m_Speed = (speed / 5.0f + 1.0f) * m_JetPackSoundSpeedMult;
}

// 0x4E1170
void CAEPedAudioEntity::PlayWindRush(float, float) {
    // NOP
}

// 0x4E1180
void CAEPedAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (sound == m_JetPackSound1) {
        if (curPlayPos == -1) {
            m_JetPackSound1 = nullptr;
            return;
        }
        if (m_Ped) {
            sound->SetPosition(m_Ped->GetPosition());
        }
        return;
    }

    if (sound == m_JetPackSound0) {
        if (curPlayPos == -1) {
            m_JetPackSound0 = nullptr;
            return;
        }
        if (m_Ped) {
            sound->SetPosition(m_Ped->GetPosition());
        }
        return;
    }

    if (sound == m_JetPackSound2) {
        if (curPlayPos == -1) {
            m_JetPackSound2 = nullptr;
            return;
        }
        if (m_Ped) {
            sound->SetPosition(m_Ped->GetPosition());
        }
        return;
    }

    // shit
    if (sound == m_UnkSound) {
        if (curPlayPos == -1) {
            m_UnkSound = nullptr;
        }
        return;
    }

    if (curPlayPos < 0)
        return;

    switch (sound->m_Event) {
    case AE_PED_SWING:
    case AE_PED_HIT_HIGH:
    case AE_PED_HIT_LOW:
    case AE_PED_HIT_GROUND:
    case AE_PED_HIT_GROUND_KICK:
    case AE_PED_HIT_HIGH_UNARMED:
    case AE_PED_HIT_LOW_UNARMED:
    case AE_PED_HIT_MARTIAL_PUNCH:
    case AE_PED_HIT_MARTIAL_KICK:
    case AE_PED_JACKED_CAR_PUNCH:
    case AE_PED_JACKED_CAR_HEAD_BANG:
    case AE_PED_JACKED_CAR_KICK:
    case AE_PED_JACKED_BIKE:
    case AE_PED_JACKED_DOZER:
        if (CTimer::GetTimeInMS() < (uint32)sound->m_ClientVariable)
            return;
        sound->m_Speed = 1.0f;
        return;
    case AE_PED_SWIM_WAKE: {
        const auto volume = GetDefaultVolume(AE_PED_SWIM_WAKE);

        if (CTimer::GetTimeInMS() <= m_LastSwimWakeTriggerTimeMs + 100) {
            if (sound->m_Volume >= volume) {
                return;
            }

            if (sound->m_Volume + 0.6f >= volume) {
                sound->m_Volume = volume;
                return;
            }
            sound->m_Volume += 0.6f;
        } else {
            auto vol = volume - 20.0f;
            if (sound->m_Volume <= vol) {
                sound->StopSoundAndForget();
                m_LastSwimWakeTriggerTimeMs = 0;
                return;
            }
            sound->m_Volume = std::max(sound->m_Volume - 0.6f, vol);
        }

        return;
    }
    default:
        return;
    }
}

// 0x4E13A0
void CAEPedAudioEntity::HandleFootstepEvent(eAudioEvents event, float volume, float speed, eSurfaceType forcedSurfaceType) {
    volume += GetDefaultVolume(event);

    if (m_Ped->bIsInTheAir) {
        return;
    }
    volume += (float)(CAEAudioUtility::GetRandomNumberInRange(-3, 3));

    const auto PlayFootstepSound = [&] (eSoundBank bank, eSoundBankSlot slot, eSoundID sfx, int16 playTime = 0) {
        if (AEAudioHardware.EnsureSoundBankIsLoaded(bank, slot)) {
            AESoundManager.PlaySound({
                .BankSlotID         = slot,
                .SoundID            = sfx,
                .AudioEntity        = this,
                .Pos                = m_Ped->GetPosition(),
                .Volume             = volume,
                .Speed              = speed,
                .Flags              = SOUND_START_PERCENTAGE,
                .FrequencyVariance  = 0.0588f,
                .PlayTime           = playTime,
                .RegisterWithEntity = m_Ped,
            });
        }
    };

    const auto PlayRandomGenericFootstepSound = [&] () {
        PlayFootstepSound(SND_BANK_FEET_GENERIC, SND_BANK_SLOT_FOOTSTEPS_GENERIC, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(1, 5)));
    };

    if (FindPlayerPed(-1) != m_Ped) {
        volume -= 3.f;
        PlayRandomGenericFootstepSound();
    } else {
        const auto TryPlayPlayerFootstepSound = [&] (eSoundBank bank, eSoundBankSlot slot, eSoundID sfx, bool needCancelSoundsInSlot = false, int16 playTime = 0) {
            if (AEAudioHardware.IsSoundBankLoaded(bank, slot)) {
                PlayFootstepSound(bank, slot, sfx, playTime);
                return true;
            } else {
                if (needCancelSoundsInSlot) {
                    if (AESoundManager.AreSoundsPlayingInBankSlot(slot) - 1 <= 1) {
                        AESoundManager.CancelSoundsInBankSlot(slot, false);
                    }
                }
                AEAudioHardware.LoadSoundBank(bank, slot);
                PlayRandomGenericFootstepSound();
                return false;
            }
        };
        if (g_surfaceInfos.IsAudioWater(forcedSurfaceType) || g_surfaceInfos.IsAudioWater(m_Ped->m_nContactSurface)) { // 0x4E1703
            TryPlayPlayerFootstepSound(SND_BANK_GENRL_SWIMMING, SND_BANK_SLOT_SWIMMING, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(0, 4)), false, 50);
        } else if (g_surfaceInfos.IsAudioConcrete(m_Ped->m_nContactSurface)) { // 0x4E145E
            PlayRandomGenericFootstepSound();
        } else if (g_surfaceInfos.IsAudioGrass(m_Ped->m_nContactSurface) || g_surfaceInfos.IsAudioLongGrass(m_Ped->m_nContactSurface)) { // 0x4E16BA
            TryPlayPlayerFootstepSound(SND_BANK_FEET_GRASS, SND_BANK_SLOT_FOOTSTEPS_PLAYER, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(0, 4)), true);
        } else if (g_surfaceInfos.IsAudioSand(m_Ped->m_nContactSurface)) { // 0x4E14B7
            TryPlayPlayerFootstepSound(SND_BANK_FEET_SAND, SND_BANK_SLOT_FOOTSTEPS_PLAYER, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(0, 3)), true);
        } else if (g_surfaceInfos.IsAudioGravel(m_Ped->m_nContactSurface)) { // 0x4E1531
            TryPlayPlayerFootstepSound(SND_BANK_FEET_GRAVEL, SND_BANK_SLOT_FOOTSTEPS_PLAYER, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(0, 4)), true);
        } else if (g_surfaceInfos.IsAudioWood(m_Ped->m_nContactSurface)) { // 0x4E157D
            TryPlayPlayerFootstepSound(SND_BANK_FEET_WOOD, SND_BANK_SLOT_FOOTSTEPS_PLAYER, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(0, 4)), true);
        } else if (g_surfaceInfos.IsAudioTile(m_Ped->m_nContactSurface)) { // 0x4E15DB
            TryPlayPlayerFootstepSound(SND_BANK_FEET_TILE, SND_BANK_SLOT_FOOTSTEPS_PLAYER, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(0, 4)), true);
        } else if (g_surfaceInfos.IsAudioMetal(m_Ped->m_nContactSurface)) { // 0x4E1636
            TryPlayPlayerFootstepSound(SND_BANK_FEET_METAL, SND_BANK_SLOT_FOOTSTEPS_PLAYER, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(0, 4)), true);
        } else { // 0x4E168F
            PlayRandomGenericFootstepSound();
        }
    }
}

// 0x4E17E0
void CAEPedAudioEntity::HandleSkateEvent(eAudioEvents event, float volume, float speed) {
    if (m_Ped->bIsInTheAir) {
        return;
    }
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_FEET_GENERIC, SND_BANK_SLOT_FOOTSTEPS_GENERIC)) {
        return;
    }
    AESoundManager.PlaySound({
        .BankSlotID         = SND_BANK_SLOT_FOOTSTEPS_GENERIC,
        .SoundID            = (eSoundID)(event == AE_PED_SKATE_LEFT ? 7 : 8),
        .AudioEntity        = this,
        .Pos                = m_Ped->GetPosition(),
        .Volume             = GetDefaultVolume(event) + (float)(CAEAudioUtility::GetRandomNumberInRange(-3, 3)) + volume,
        .Speed              = speed,
        .FrequencyVariance  = 0.0588f,
        .RegisterWithEntity = m_Ped,
    });
}

// 0x4E18E0
void CAEPedAudioEntity::HandleLandingEvent(eAudioEvents event) {
    if (m_Ped->bIsInTheAir) {
        return;
    }

    const auto PlayLandingSound = [&](eSoundBankSlot slot, eSoundID soundID, float volume, int16 playPos) {
        AESoundManager.PlaySound({ .BankSlotID = slot, .SoundID = soundID, .AudioEntity = this, .Pos = m_Ped->GetPosition(), .Volume = volume, .Flags = SOUND_START_PERCENTAGE, .FrequencyVariance = 0.0588f, .PlayTime = playPos, .RegisterWithEntity = m_Ped });
    };
    const auto volume = GetDefaultVolume(event);
    if (g_surfaceInfos.IsAudioWater(m_Ped->m_nContactSurface)) {
        if (AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_SWIMMING, SND_BANK_SLOT_SWIMMING)) {
            PlayLandingSound(
                SND_BANK_SLOT_SWIMMING,
                CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_SWIMMING_SWIM1, SND_GENRL_SWIMMING_SWIM5),
                std::max(0.f, volume),
                50
            );
        }
    } else {
        if (AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_FEET_GENERIC, SND_BANK_SLOT_FOOTSTEPS_GENERIC)) {
            PlayLandingSound(
                SND_BANK_SLOT_FOOTSTEPS_GENERIC,
                event == AE_PED_LAND_ON_FEET_AFTER_FALL ? 6 : 0,
                volume,
                0
            );
        }
    }
}

// 0x4E1A40
void CAEPedAudioEntity::HandlePedSwing(eAudioEvents event, int32 hitEvent, uint32 startDelayMs) {
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN, true)) {
        return;
    }

    const auto PlaySwingSound = [&](eSoundID soundID, float volumeOffset, float speed) {
        PlayFightSound(SND_BANK_SLOT_WEAPON_GEN, soundID, event, GetDefaultVolume(event) + volumeOffset, speed, 0.04f, startDelayMs);
    };

    switch (hitEvent) {
    case AE_PED_HIT_GROUND_KICK:
    case AE_PED_HIT_HIGH_UNARMED:
    case AE_PED_HIT_LOW_UNARMED: // 0x4E1B5D
        return PlaySwingSound(SND_GENRL_WEAPONS_WEAPON_SWIPE_FIST, -6.f, 0.84f);
    case AE_PED_HIT_MARTIAL_PUNCH:
    case AE_PED_HIT_MARTIAL_KICK: // 0x4E1B74
        return PlaySwingSound(SND_GENRL_WEAPONS_WEAPON_SWIPE_FIST, -2.f, 0.84f);
    }

    switch (m_Ped->GetActiveWeapon().GetType()) {
    case WEAPON_GOLFCLUB:
    case WEAPON_CANE: // 0x4E1AE8
        return PlaySwingSound(SND_GENRL_WEAPONS_CANE_SWISH, 0.f, 1.f);
    case WEAPON_NIGHTSTICK:
    case WEAPON_POOL_CUE: // 0x4E1B17
        return PlaySwingSound(SND_GENRL_WEAPONS_WEAPON_SWIPE, 0.f, 0.84f);
    case WEAPON_KNIFE:
    case WEAPON_KATANA: // 0x4E1B26
        return PlaySwingSound(SND_GENRL_WEAPONS_WEAPON_SWIPE_METAL, -3.f, 1.f);
    case WEAPON_BASEBALLBAT:
    case WEAPON_SHOVEL: // 0x4E1B08
        return PlaySwingSound(SND_GENRL_WEAPONS_WEAPON_SWIPE, 0.f, 0.67f);
    case WEAPON_DILDO1:
    case WEAPON_DILDO2:
    case WEAPON_VIBE1:
    case WEAPON_VIBE2: // 0x4E1B33
        return PlaySwingSound(SND_GENRL_WEAPONS_WEAPON_SWIPE, -6.f, 0.84f);
    case WEAPON_UNARMED:
    case WEAPON_BRASSKNUCKLE:
    case WEAPON_CHAINSAW:
    case WEAPON_FLOWERS:
    case WEAPON_GRENADE:
    case WEAPON_TEARGAS:
    case WEAPON_MOLOTOV:
    case WEAPON_PISTOL:
    case WEAPON_PISTOL_SILENCED:
    case WEAPON_DESERT_EAGLE:
    case WEAPON_SHOTGUN:
    case WEAPON_SAWNOFF_SHOTGUN:
    case WEAPON_SPAS12_SHOTGUN:
    case WEAPON_MICRO_UZI:
    case WEAPON_MP5:
    case WEAPON_AK47:
    case WEAPON_M4:
    case WEAPON_TEC9:
    case WEAPON_COUNTRYRIFLE:
    case WEAPON_SNIPERRIFLE:
    case WEAPON_FLAMETHROWER:
    case WEAPON_MINIGUN:
    case WEAPON_REMOTE_SATCHEL_CHARGE:
    case WEAPON_DETONATOR:
    case WEAPON_SPRAYCAN:
    case WEAPON_EXTINGUISHER:
    case WEAPON_CAMERA:
    case WEAPON_NIGHTVISION:
    case WEAPON_INFRARED: // 0x4E1B48
        return PlaySwingSound(SND_GENRL_WEAPONS_WEAPON_SWIPE_FIST, -6.f, 0.84f);
    case WEAPON_ROCKET:
    case WEAPON_ROCKET_HS:
    case WEAPON_FREEFALL_BOMB:
    case WEAPON_RLAUNCHER:
    case WEAPON_RLAUNCHER_HS:
    default: // 0x4E1B8B
        return;
    }
}

// 0x4E1CC0
void CAEPedAudioEntity::HandlePedHit(eAudioEvents event, CPhysical* hitEntity, eSurfaceType hitSurface, float volume, uint32 startDelayMs) {
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN, true)) {
        return;
    }

    const auto baseVolume = GetDefaultVolume(event) + volume;
    const auto weaponType = m_Ped->GetActiveWeapon().GetType();

    // The sound of the weapon itself
    const auto [weaponSoundID, weaponSpeedVariance] = [weaponType]() -> std::pair<eSoundID, float> {
        switch (weaponType) {
        case WEAPON_UNARMED:
        case WEAPON_NIGHTVISION:
        case WEAPON_INFRARED: // 0x4E1D86
            return { SND_GENRL_WEAPONS_KNUCKLE, 0.0588f };
        case WEAPON_GOLFCLUB: // 0x4E1D99
            return { SND_GENRL_WEAPONS_CLUB_STRIKE, 0.0588f };
        case WEAPON_NIGHTSTICK: // 0x4E1DA3
            return { SND_GENRL_WEAPONS_BAT_STRIKE, 0.0588f };
        case WEAPON_KNIFE: // 0x4E1DB7
            return { SND_GENRL_WEAPONS_KNIFE, 0.03f };
        case WEAPON_BASEBALLBAT: // 0x4E1DC9
            return { SND_GENRL_WEAPONS_BASEBALL_BAT, 0.0588f };
        case WEAPON_SHOVEL: // 0x4E1DD3
            return { SND_GENRL_WEAPONS_SHOVEL, 0.0588f };
        case WEAPON_POOL_CUE: // 0x4E1DAD
            return { SND_GENRL_WEAPONS_POOLCUE, 0.0588f };
        case WEAPON_KATANA: // 0x4E1DDD
            return { SND_GENRL_WEAPONS_KATANA_SWORD, 0.0588f };
        case WEAPON_DILDO1:
        case WEAPON_DILDO2:
        case WEAPON_VIBE1:
        case WEAPON_VIBE2: // 0x4E1DF1
            return { SND_GENRL_WEAPONS_DILDO, 0.0588f };
        case WEAPON_FLOWERS: // 0x4E1DFB
            return { SND_GENRL_WEAPONS_FLOWER_STRIKE, 0.07f };
        case WEAPON_CANE: // 0x4E1DE7
            return { SND_GENRL_WEAPONS_CANE_STRIKE, 0.0588f };
        case WEAPON_BRASSKNUCKLE:
        case WEAPON_GRENADE:
        case WEAPON_TEARGAS:
        case WEAPON_MOLOTOV:
        case WEAPON_PISTOL:
        case WEAPON_PISTOL_SILENCED:
        case WEAPON_DESERT_EAGLE:
        case WEAPON_SHOTGUN:
        case WEAPON_SAWNOFF_SHOTGUN:
        case WEAPON_SPAS12_SHOTGUN:
        case WEAPON_MICRO_UZI:
        case WEAPON_MP5:
        case WEAPON_AK47:
        case WEAPON_M4:
        case WEAPON_TEC9:
        case WEAPON_COUNTRYRIFLE:
        case WEAPON_SNIPERRIFLE:
        case WEAPON_FLAMETHROWER:
        case WEAPON_MINIGUN:
        case WEAPON_REMOTE_SATCHEL_CHARGE:
        case WEAPON_DETONATOR:
        case WEAPON_SPRAYCAN:
        case WEAPON_EXTINGUISHER:
        case WEAPON_CAMERA: // 0x4E1D8F
            return { SND_GENRL_WEAPONS_BRASS_KNUCKLES, 0.0588f };
        case WEAPON_CHAINSAW:
        case WEAPON_ROCKET:
        case WEAPON_ROCKET_HS:
        case WEAPON_FREEFALL_BOMB:
        case WEAPON_RLAUNCHER:
        case WEAPON_RLAUNCHER_HS:
        default: // 0x4E1E0D
            return { SND_GENRL_WEAPONS_UNK, 0.0588f };
        }
    }();

    const auto PlayWeaponSound = [&](eSoundID soundID, float volumeOffset) {
        if (soundID >= 0) {
            PlayFightSound(SND_BANK_SLOT_WEAPON_GEN, soundID, event, baseVolume + volumeOffset, 1.f, weaponSpeedVariance, startDelayMs);
        }
    };
    const auto PlayImpactSound = [&](eSoundID soundID, float volumeOffset, eSoundBankSlot slot = SND_BANK_SLOT_WEAPON_GEN) {
        PlayFightSound(slot, soundID, event, baseVolume + volumeOffset, 1.f, 0.0588f, startDelayMs);
    };
    const auto PlayWeaponAndImpactSound = [&](float weaponVolumeOffset, eSoundID impactSoundID, float impactVolumeOffset) {
        PlayWeaponSound(weaponSoundID, weaponVolumeOffset);
        PlayImpactSound(impactSoundID, impactVolumeOffset);
    };
    const auto PlayChainsawImpactSound = [&] { // 0x4E1F17
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_BULLET_HITS, SND_BANK_SLOT_BULLET_HITS)) {
            PlayImpactSound(CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_BULLET_HITS_BULPED1L, SND_GENRL_BULLET_HITS_BULPED3L), -3.f, SND_BANK_SLOT_BULLET_HITS);
        }
    };
    const auto ReportCollisionWithHitEntity = [&] {
        AudioEngine.ReportCollision(m_Ped, hitEntity, SURFACE_PED, hitSurface, m_Ped->GetPosition(), nullptr, 1.f, 1.f, true, false);
    };

    if (!hitEntity || !hitEntity->GetIsTypePed()) { // 0x4E1FCA
        switch (event) {
        case AE_PED_HIT_HIGH:
        case AE_PED_HIT_LOW:
        case AE_PED_HIT_GROUND: { // 0x4E1FE0
            const auto surfaceVolume = GetHitSurfaceVolume(hitSurface);
            ReportCollisionWithHitEntity();
            return PlayWeaponSound(weaponSoundID, surfaceVolume);
        }
        case AE_PED_HIT_GROUND_KICK:
        case AE_PED_HIT_MARTIAL_KICK: // 0x4E202B
            return ReportCollisionWithHitEntity();
        case AE_PED_HIT_HIGH_UNARMED:
        case AE_PED_HIT_LOW_UNARMED:
        case AE_PED_HIT_MARTIAL_PUNCH: { // 0x4E206E
            const auto surfaceVolume = GetHitSurfaceVolume(hitSurface);
            ReportCollisionWithHitEntity();
            return PlayWeaponSound(SND_GENRL_WEAPONS_KNUCKLE, surfaceVolume);
        }
        default: // 0x4E1EB4
            return PlayWeaponSound(weaponSoundID, 0.f);
        }
    }

    switch (event) {
    case AE_PED_HIT_HIGH:  // 0x4E1E43
    case AE_PED_HIT_LOW:  { // 0x4E1E88
        const auto GetImpactSoundID = [event] {
            return event == AE_PED_HIT_HIGH
                ? CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_WEAPONS_BOX_FACE1, SND_GENRL_WEAPONS_BOX_FACE4)
                : CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_WEAPONS_BODY_PUNCH1, SND_GENRL_WEAPONS_BODY_PUNCH4);
        };
        switch (weaponType) {
        case WEAPON_KNIFE:
        case WEAPON_KATANA: // 0x4E1F05
            return PlayImpactSound(SND_GENRL_WEAPONS_STAB, -9.f);
        case WEAPON_CHAINSAW:
            return PlayChainsawImpactSound();
        case WEAPON_FLOWERS: // 0x4E1E59
            return PlayWeaponSound(weaponSoundID, 0.f);
        case WEAPON_BRASSKNUCKLE: // 0x4E1E6C
            return PlayWeaponAndImpactSound(1.f, GetImpactSoundID(), -3.f);
        case WEAPON_BASEBALLBAT: // 0x4E1E5E
            return PlayWeaponAndImpactSound(-10.f, GetImpactSoundID(), -3.f);
        default: // 0x4E1E7A
            return PlayWeaponAndImpactSound(-15.f, GetImpactSoundID(), -3.f);
        }
    }
    case AE_PED_HIT_GROUND: { // 0x4E1EEF
        switch (weaponType) {
        case WEAPON_KNIFE:
        case WEAPON_KATANA: // 0x4E1F05
            return PlayImpactSound(SND_GENRL_WEAPONS_STAB, -9.f);
        case WEAPON_CHAINSAW:
            return PlayChainsawImpactSound();
        case WEAPON_FLOWERS: // 0x4E1E59
            return PlayWeaponSound(weaponSoundID, 0.f);
        case WEAPON_BASEBALLBAT: // 0x4E1F43
            return PlayWeaponAndImpactSound(-10.f, SND_GENRL_WEAPONS_STOMP1, -6.f);
        default: // 0x4E1F5D
            return PlayWeaponAndImpactSound(-15.f, SND_GENRL_WEAPONS_STOMP1, -6.f);
        }
    }
    case AE_PED_HIT_GROUND_KICK: // 0x4E1F77
        return PlayImpactSound(SND_GENRL_WEAPONS_STOMP1, -6.f);
    case AE_PED_HIT_HIGH_UNARMED:
    case AE_PED_HIT_MARTIAL_PUNCH: { // 0x4E1F89
        const auto impactSoundID = CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_WEAPONS_BOX_FACE1, SND_GENRL_WEAPONS_BOX_FACE4);
        PlayWeaponSound(SND_GENRL_WEAPONS_KNUCKLE, 0.f);
        return PlayImpactSound(impactSoundID, -3.f);
    }
    case AE_PED_HIT_LOW_UNARMED: { // 0x4E1FA0
        const auto impactSoundID = CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_WEAPONS_BODY_PUNCH1, SND_GENRL_WEAPONS_BODY_PUNCH4);
        PlayWeaponSound(SND_GENRL_WEAPONS_KNUCKLE, 0.f);
        return PlayImpactSound(impactSoundID, -3.f);
    }
    case AE_PED_HIT_MARTIAL_KICK: // 0x4E1FB7
        return PlayImpactSound(CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_WEAPONS_MARTIAL_STRIKE1, SND_GENRL_WEAPONS_MARTIAL_STRIKE4), -3.f);
    default: // 0x4E1E59
        return PlayWeaponSound(weaponSoundID, 0.f);
    }
}

// 0x4E2350
void CAEPedAudioEntity::HandlePedJacked(eAudioEvents event) {
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN, true)) {
        return;
    }

    const auto volume = GetDefaultVolume(event);
    const auto now    = CTimer::GetTimeInMS();

    // The sounds start silent (0 speed), `UpdateParameters` gives them speed 1 once `startTimeMs` has passed
    const auto PlayJackedSound = [&](eSoundBankSlot slot, eSoundID soundID, uint32 startTimeMs) {
        AESoundManager.PlaySound({ .BankSlotID = slot, .SoundID = soundID, .AudioEntity = this, .Pos = m_Ped->GetPosition(), .Volume = volume, .RollOffFactor = 2.f, .Speed = 0.f, .Flags = SOUND_REQUEST_UPDATES, .RegisterWithEntity = m_Ped, .EventID = event, .ClientVariable = (float)(startTimeMs) });
    };
    const auto PlayHitSounds = [&](eSoundID strikeSoundID, eSoundID impactSoundID, uint32 startTimeMs) {
        PlayJackedSound(SND_BANK_SLOT_WEAPON_GEN, strikeSoundID, startTimeMs);
        PlayJackedSound(SND_BANK_SLOT_WEAPON_GEN, impactSoundID, startTimeMs);
    };

    const auto fallTimeMs = [&]() -> uint32 {
        switch (event) {
        case AE_PED_JACKED_CAR_PUNCH: // 0x4E23D1
            PlayHitSounds(SND_GENRL_WEAPONS_KNUCKLE, CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_WEAPONS_BOX_FACE1, SND_GENRL_WEAPONS_BOX_FACE4), now + 500);
            return now + 2'133;
        case AE_PED_JACKED_CAR_HEAD_BANG: // 0x4E2415
            PlayHitSounds(SND_GENRL_WEAPONS_DILDO, CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_WEAPONS_BODY_PUNCH1, SND_GENRL_WEAPONS_BODY_PUNCH4), now + 933);
            return now + 2'633;
        case AE_PED_JACKED_CAR_KICK: // 0x4E243E
            PlayJackedSound(SND_BANK_SLOT_WEAPON_GEN, CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_WEAPONS_BOX_FACE1, SND_GENRL_WEAPONS_BOX_FACE4), now + 900);
            return now + 2'800;
        case AE_PED_JACKED_BIKE: // 0x4E2466
            PlayHitSounds(SND_GENRL_WEAPONS_KNUCKLE, CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_WEAPONS_BOX_FACE1, SND_GENRL_WEAPONS_BOX_FACE4), now + 33);
            return now + 366;
        case AE_PED_JACKED_DOZER: // 0x4E248F
            PlayJackedSound(SND_BANK_SLOT_WEAPON_GEN, CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_WEAPONS_BODY_PUNCH1, SND_GENRL_WEAPONS_BODY_PUNCH4), now + 266);
            return now + 833;
        default: // 0x4E25C0
            return now;
        }
    }();

    // 0x4E25C0 - The body hitting the ground
    if (AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_FEET_GENERIC, SND_BANK_SLOT_FOOTSTEPS_GENERIC)) {
        PlayJackedSound(SND_BANK_SLOT_FOOTSTEPS_GENERIC, 0, fallTimeMs);
    }
}

// 0x4E26A0
void CAEPedAudioEntity::HandleSwimSplash(eAudioEvents event) {
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_SWIMMING, SND_BANK_SLOT_SWIMMING)) {
        return;
    }
    if (++m_SwimSplashSoundID > 4) {
        m_SwimSplashSoundID = 0;
    }
    AESoundManager.PlaySound({
        .BankSlotID         = SND_BANK_SLOT_SWIMMING,
        .SoundID            = (eSoundID)(m_SwimSplashSoundID),
        .AudioEntity        = this,
        .Pos                = m_Ped->GetPosition(),
        .Volume             = GetDefaultVolume(event),
        .FrequencyVariance  = 0.0588f,
        .RegisterWithEntity = m_Ped,
    });
}

// 0x4E2790
void CAEPedAudioEntity::HandleSwimWake(eAudioEvents event) {
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS, true)) {
        return;
    }
    if (!AESoundManager.AreSoundsOfThisEventPlayingForThisEntityAndPhysical(event, this, m_Ped)) {
        AESoundManager.PlaySound({
            .BankSlotID         = SND_BANK_SLOT_COLLISIONS,
            .SoundID            = 3,
            .AudioEntity        = this,
            .Pos                = m_Ped->GetPosition(),
            .Volume             = GetDefaultVolume(event) - 20.0f,
            .RollOffFactor      = 1.f,
            .Speed              = 0.75f,
            .Doppler            = 1.f,
            .FrameDelay         = 0,
            .Flags              = SOUND_REQUEST_UPDATES | SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY,
            .RegisterWithEntity = m_Ped,
            .EventID            = event,
        });
    }
    m_LastSwimWakeTriggerTimeMs = CTimer::GetTimeInMS();
}

// 0x4E2A90
void CAEPedAudioEntity::PlayShirtFlap(float volume, float speed) {
    if (m_ShirtFlapSound.IsActive()) {
        m_ShirtFlapSound.UpdateTwinLoopSound(m_Ped->GetPosition(), volume, speed);
    } else {
        m_ShirtFlapSound.Initialise(SND_BANK_SLOT_WEAPON_GEN, 19, 20, this, 200, 1'000, -1, -1);
        m_ShirtFlapSound.PlayTwinLoopSound(m_Ped->GetPosition(), volume, speed, 2.0f, 1.0f, SOUND_DEFAULT);
    }
}

// 0x4E2EE0
void CAEPedAudioEntity::Service() {
    if (!m_Ped->IsPlayer()) {
        return;
    }

    const auto* const attachedTo = m_Ped->m_pAttachedTo;
    const auto        isOnFoot   = !(attachedTo && attachedTo->GetIsTypeVehicle()) && !m_Ped->bInVehicle; // NOTE: Not `IsInVehicle()`, the original doesn't check `m_pVehicle`

    // Wind in the face when driving a bike or an open top car
    float volume = -100.f;
    float speed  = 1.f;
    if (auto* const veh = m_Ped->m_pVehicle; m_Ped->IsStateDriving() && veh && (veh->IsBike() || veh->IsAutomobile() && veh->IsOpenTopCar())) {
        if (const auto vehSpeed = std::abs(veh->GetMoveSpeed().Magnitude()); vehSpeed > 0.f) { // 0x4E2FB9
            const auto t = std::clamp(vehSpeed, 0.f, 1.f);
            volume       = CAEAudioUtility::AudioLog10(t) * 20.f + (veh->m_vehicleAudio.m_AuSettings.VehicleAudioType == AE_BMX ? -17.f : -10.f);
            speed        = (1.2f - 0.8f) * t + 0.8f;
        }
    }

    const auto isUsingParachute = m_Ped->GetIntelligence() && m_Ped->GetIntelligence()->GetUsingParachute();

    // Wind when walking/running/falling
    float skyDiveAccel = 0.f, paraDecel = 0.f;
    if (isOnFoot) { // 0x4E3086
        auto  t = m_Ped->GetMoveSpeed().Magnitude() / 0.7f;
        float baseVolume;
        if (!isUsingParachute) {
            baseVolume = -16.f;
            t          = t >= 0.5f
                ? std::clamp((t - 0.5f) / (1.f - 0.5f), 0.f, 1.f)
                : 0.f;
        } else { // 0x4E30FC
            baseVolume = -7.f;
            if (const auto* const a = RpAnimBlendClumpGetAssociation(m_Ped->GetRpClump(), "FALL_SkyDive_accel")) {
                skyDiveAccel = std::clamp(a->GetBlendAmount(), 0.f, 1.f);
            }
            if (const auto* const a = RpAnimBlendClumpGetAssociation(m_Ped->GetRpClump(), "Para_decel")) {
                paraDecel = std::clamp(a->GetBlendAmount(), 0.f, 1.f);
            }
            t = std::clamp(t, 0.f, 1.f);
        }
        volume = CAEAudioUtility::AudioLog10(t) * 20.f + baseVolume;
        speed  = (1.6f - 0.8f) * t + 0.8f;
    }

    // 0x4E3242
    const auto targetVolume = volume + 4.f * skyDiveAccel - 6.f * paraDecel;
    m_ShirtFlapVolume       = notsa::step_to(m_ShirtFlapVolume, targetVolume, 5.f, 2.f); // NOTE: Framerate dependent
    if (targetVolume < -20.f && m_ShirtFlapVolume < -20.f) {
        m_ShirtFlapVolume = targetVolume;
    }
    if (m_ShirtFlapVolume > -100.f) {
        PlayShirtFlap(m_ShirtFlapVolume, (0.5f * skyDiveAccel + 1.f) * speed);
    } else {
        m_ShirtFlapSound.StopSoundAndForget();
    }
}

// NOTSA - Sound of a swing/hit, a delayed one starts silent (0 speed) and `UpdateParameters` gives it speed 1 once the delay has passed
void CAEPedAudioEntity::PlayFightSound(eSoundBankSlot slot, eSoundID soundID, eAudioEvents event, float volume, float speed, float speedVariance, uint32 startDelayMs) {
    const auto isDelayed = startDelayMs != 0;
    AESoundManager.PlaySound({
        .BankSlotID         = slot,
        .SoundID            = soundID,
        .AudioEntity        = this,
        .Pos                = m_Ped->GetPosition(),
        .Volume             = volume,
        .Speed              = isDelayed ? 0.f : speed,
        .Flags              = isDelayed ? SOUND_REQUEST_UPDATES : SOUND_DEFAULT,
        .FrequencyVariance  = isDelayed ? 0.f : speedVariance,
        .RegisterWithEntity = m_Ped,
        .EventID            = isDelayed ? event : AE_UNDEFINED,
        .ClientVariable     = isDelayed ? (float)(CTimer::GetTimeInMS() + startDelayMs) : -1.f,
    });
}

// 0x4DA510
float CAEPedAudioEntity::GetHitSurfaceVolume(eSurfaceType surface) {
    // NOTE: The original's x87 `FYL2X` keeps the logarithm in full precision, a `float` one is 1 ulp off for some of the table's values
    return (float)(std::log10((double)((float)(gCollisionLookup[surface].ParamD) * 0.01f)) * 20.f);
}

void CAEPedAudioEntity::InjectHooks() {
    RH_ScopedClass(CAEPedAudioEntity);
    RH_ScopedCategory("Audio/Entities");

    //RH_ScopedInstall(Constructor, 0x5DE8D0, { .Reversed = false });

    RH_ScopedInstall(Initialise, 0x4E0E80);
    RH_ScopedInstall(StaticInitialise, 0x5B98A0);
    RH_ScopedInstall(Terminate, 0x4E1360);
    RH_ScopedInstall(AddAudioEvent, 0x4E2BB0);
    RH_ScopedInstall(TurnOnJetPack, 0x4E28A0);
    RH_ScopedInstall(TurnOffJetPack, 0x4E2A70);
    RH_ScopedInstall(StopJetPackSound, 0x4E1120);
    RH_ScopedInstall(UpdateJetPack, 0x4E0EE0);
    RH_ScopedInstall(PlayWindRush, 0x4E1170);
    RH_ScopedInstall(UpdateParameters, 0x4E1180);
    RH_ScopedInstall(HandleFootstepEvent, 0x4E13A0);
    RH_ScopedInstall(HandleSkateEvent, 0x4E17E0);
    RH_ScopedInstall(HandleLandingEvent, 0x4E18E0);
    RH_ScopedInstall(HandlePedSwing, 0x4E1A40);
    RH_ScopedInstall(HandlePedHit, 0x4E1CC0);
    RH_ScopedInstall(HandlePedJacked, 0x4E2350);
    RH_ScopedInstall(HandleSwimSplash, 0x4E26A0);
    RH_ScopedInstall(HandleSwimWake, 0x4E2790);
    RH_ScopedInstall(PlayShirtFlap, 0x4E2A90);
    RH_ScopedInstall(Service, 0x4E2EE0);
    RH_ScopedInstall(GetHitSurfaceVolume, 0x4DA510);
}
