#include "StdInc.h"

#include "AEPedAudioEntity.h"
#include "AEAudioHardware.h"
#include "AEAudioUtility.h"

void CAEPedAudioEntity::InjectHooks() {
    RH_ScopedClass(CAEPedAudioEntity);
    RH_ScopedCategory("Audio/Entities");

    //RH_ScopedInstall(Constructor, 0x5DE8D0, { .reversed = false });

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
    RH_ScopedInstall(Service, 0x4E2EE0, { .reversed = false });
}

// 0x5DE8D0
CAEPedAudioEntity::CAEPedAudioEntity() : CAEAudioEntity() {
    m_pPed = nullptr;
    m_bCanAddEvent = false;

    m_JetPackSound0 = nullptr;
    m_JetPackSound1 = nullptr;
    m_JetPackSound2 = nullptr;
}

// (CEntity* entity)
// 0x4E0E80
void CAEPedAudioEntity::Initialise(CPed* ped) {
    m_pPed = ped;
    m_nSfxId = 0;
    m_LastSwimWakeTriggerTimeMs = 0;

    m_bJetPackPlaying = false;
    m_JetPackSound0 = nullptr;
    m_JetPackSound1 = nullptr;
    m_fVolume1 = -100.0f;
    m_fVolume2 = -100.0f;

    field_150 = nullptr;
    field_154 = -100.0f;
    field_158 = -100.0f;
    m_bCanAddEvent = true;
}

// 0x5B98A0
void CAEPedAudioEntity::StaticInitialise() {
    AEAudioHardware.LoadSoundBank(SND_BANK_FEET_GENERIC, SND_BANK_SLOT_FOOTSTEPS_GENERIC);
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_SWIMMING, SND_BANK_SLOT_SWIMMING);
}

// 0x4E1360
void CAEPedAudioEntity::Terminate() {
    m_bCanAddEvent = false;
    m_pPed   = nullptr;
    StopJetPackSound();
    AESoundManager.CancelSoundsOwnedByAudioEntity(this, true);
    if (m_sTwinLoopSoundEntity.IsActive()) {
        m_sTwinLoopSoundEntity.StopSoundAndForget();
    }
}

// 0x4E2BB0
void CAEPedAudioEntity::AddAudioEvent(eAudioEvents event, float volume, float speed, CPhysical* ped, eSurfaceType surfaceId, int32 a7, uint32 maxVol) {
    if (!m_bCanAddEvent)
        return;

    if (!m_pPed)
        return;

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
        HandlePedSwing(event, a7, maxVol);
        break;
    case AE_PED_HIT_HIGH:
    case AE_PED_HIT_LOW:
    case AE_PED_HIT_GROUND:
    case AE_PED_HIT_GROUND_KICK:
    case AE_PED_HIT_HIGH_UNARMED:
    case AE_PED_HIT_LOW_UNARMED:
    case AE_PED_HIT_MARTIAL_PUNCH:
    case AE_PED_HIT_MARTIAL_KICK:
        HandlePedHit(event, ped, surfaceId, volume, maxVol);
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
        AESoundManager.PlaySound({
            .BankSlotID        = SND_BANK_SLOT_COLLISIONS,
            .SoundID           = 29,
            .AudioEntity       = this,
            .Pos               = ped->GetPosition(),
            .Volume            = volume,
            .RollOffFactor     = 1.5f,
            .Speed             = speed,
            .Doppler           = 1.0f,
            .FrameDelay        = 0,
            .Flags             = SOUND_DEFAULT,
            .FrequencyVariance = 0.06f,
            .PlayTime          = 0,
            .EventID           = AE_PED_CRUNCH
        });

        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_PED_KNOCK_DOWN, this) != 0) {
            break;
        }
        AESoundManager.PlaySound({
            .BankSlotID        = SND_BANK_SLOT_COLLISIONS,
            .SoundID           = (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(47, 49)),
            .AudioEntity       = this,
            .Pos               = ped->GetPosition(),
            .Volume            = volume,
            .RollOffFactor     = 1.5f,
            .Speed             = speed,
            .Doppler           = 1.0f,
            .FrameDelay        = 0,
            .Flags             = SOUND_DEFAULT,
            .FrequencyVariance = 0.06f,
            .PlayTime          = 0,
            .EventID           = AE_PED_KNOCK_DOWN
        });
        break;
    }
    default:
        break;
    }
}

// 0x4E28A0
void CAEPedAudioEntity::TurnOnJetPack() {
    if (!m_pPed)
        return;

    if (m_bJetPackPlaying || m_JetPackSound1 || m_JetPackSound0 || m_JetPackSound2)
        return;

    m_fVolume1 = -100.0f;
    m_fVolume2 = -100.0f;
    m_fVolume3 = +2.000f;
    m_JetPackSoundSpeedMult = 0.400f;

    m_bJetPackPlaying = true;

    const auto PlayJetPackSound = [&](eSoundBankSlot slot, eSoundID soundID) {
        return AESoundManager.PlaySound({
            .BankSlotID  = slot,
            .SoundID     = soundID,
            .AudioEntity = this,
            .Pos         = m_pPed->GetPosition(),
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
    m_bJetPackPlaying = false;
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
    if (!m_bJetPackPlaying || !m_JetPackSound1 || !m_JetPackSound0 || !m_JetPackSound2) {
        return;
    }

    if (thrustFwd <= 0.5f) { // flying
        m_fVolume1 = std::max(m_fVolume1 - 5.0f, -100.0f);
        m_fVolume2 = std::min(m_fVolume2 + 6.0f, -17.0f);
        m_fVolume3 = std::max(m_fVolume3 - 0.3f, 2.0f);
        m_JetPackSoundSpeedMult = std::max(m_JetPackSoundSpeedMult - 0.031f, 0.4f);
    } else { // idle
        m_fVolume1 = std::min(m_fVolume1 + 15.0f, -15.0f);
        m_fVolume2 = std::max(m_fVolume2 - 7.1f, -100.0f);
        m_fVolume3 = std::min(m_fVolume3 + 0.3f, 11.0f);
        m_JetPackSoundSpeedMult = std::min(m_JetPackSoundSpeedMult + 0.031f, 0.71f);
    }

    const auto angle = std::sin(thrustAngle);
    const float speed = angle < 0.0f ? -angle : angle; // maybe wrong

    // 0.0f == 0xB61384 (uninitialized)
    m_JetPackSound0->m_Volume = m_fVolume1 + 0.0f;
    m_JetPackSound0->m_Speed = speed * -0.07f + 1.0f;

    m_JetPackSound1->m_Volume = m_fVolume2 + 0.0f;
    m_JetPackSound1->m_Speed = 0.56f;

    m_JetPackSound2->m_Volume = m_fVolume3 + 0.0f;
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
        if (m_pPed) {
            sound->SetPosition(m_pPed->GetPosition());
        }
        return;
    }

    if (sound == m_JetPackSound0) {
        if (curPlayPos == -1) {
            m_JetPackSound0 = nullptr;
            return;
        }
        if (m_pPed) {
            sound->SetPosition(m_pPed->GetPosition());
        }
        return;
    }

    if (sound == m_JetPackSound2) {
        if (curPlayPos == -1) {
            m_JetPackSound2 = nullptr;
            return;
        }
        if (m_pPed) {
            sound->SetPosition(m_pPed->GetPosition());
        }
        return;
    }

    // shit
    if (sound == field_150) {
        if (curPlayPos == -1) {
            field_150 = nullptr;
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

    if (m_pPed->bIsInTheAir) {
        return;
    }

    const auto PlayFootstepSound = [&] (eSoundBank bank, eSoundBankSlot slot, eSoundID sfx, int16 playTime = 0) {
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_FEET_GENERIC, slot)) {
            AESoundManager.PlaySound({
                .BankSlotID         = slot,
                .SoundID            = sfx,
                .AudioEntity        = this,
                .Pos                = m_pPed->GetPosition(),
                .Volume             = volume,
                .Speed              = speed,
                .Flags              = SOUND_START_PERCENTAGE,
                .FrequencyVariance  = 0.0588f,
                .PlayTime           = playTime,
                .RegisterWithEntity = m_pPed,
            });
        } else {
            AEAudioHardware.LoadSoundBank(bank, slot);
        }
    };

    const auto PlayRandomGenericFootstepSound = [&] () {
        PlayFootstepSound(SND_BANK_FEET_GENERIC, SND_BANK_SLOT_FOOTSTEPS_GENERIC, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(1, 5)));
    };

    if (FindPlayerPed(-1) != m_pPed) {
        PlayRandomGenericFootstepSound();
    } else {
        const auto TryPlayPlayerFootstepSound = [&] (eSoundBank bank, eSoundBankSlot slot, eSoundID sfx, bool needCancelSoundsInSlot = false, int16 playTime = 0) {
            if (AEAudioHardware.IsSoundBankLoaded(bank, slot)) {
                PlayFootstepSound(bank, slot, sfx);
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
        if (g_surfaceInfos.IsAudioWater(forcedSurfaceType) || g_surfaceInfos.IsAudioWater(m_pPed->m_nContactSurface)) { // 0x4E1703
            TryPlayPlayerFootstepSound(SND_BANK_GENRL_SWIMMING, SND_BANK_SLOT_SWIMMING, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(0, 4)), false, 50);
        } else if (g_surfaceInfos.IsAudioConcrete(m_pPed->m_nContactSurface)) { // 0x4E145E
            PlayRandomGenericFootstepSound();
        } else if (g_surfaceInfos.IsAudioGrass(m_pPed->m_nContactSurface) || g_surfaceInfos.IsAudioLongGrass(m_pPed->m_nContactSurface)) { // 0x4E16BA
            TryPlayPlayerFootstepSound(SND_BANK_FEET_GRASS, SND_BANK_SLOT_FOOTSTEPS_PLAYER, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(0, 4)), true);
        } else if (g_surfaceInfos.IsAudioSand(m_pPed->m_nContactSurface)) { // 0x4E14B7
            TryPlayPlayerFootstepSound(SND_BANK_FEET_SAND, SND_BANK_SLOT_FOOTSTEPS_PLAYER, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(0, 3)), true);
        } else if (g_surfaceInfos.IsAudioGravel(m_pPed->m_nContactSurface)) { // 0x4E1531
            TryPlayPlayerFootstepSound(SND_BANK_FEET_GRAVEL, SND_BANK_SLOT_FOOTSTEPS_PLAYER, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(0, 4)), true);
        } else if (g_surfaceInfos.IsAudioWood(m_pPed->m_nContactSurface)) { // 0x4E157D
            TryPlayPlayerFootstepSound(SND_BANK_FEET_WOOD, SND_BANK_SLOT_FOOTSTEPS_PLAYER, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(0, 4)), true);
        } else if (g_surfaceInfos.IsAudioTile(m_pPed->m_nContactSurface)) { // 0x4E15DB
            TryPlayPlayerFootstepSound(SND_BANK_FEET_TILE, SND_BANK_SLOT_FOOTSTEPS_PLAYER, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(0, 4)), true);
        } else if (g_surfaceInfos.IsAudioMetal(m_pPed->m_nContactSurface)) { // 0x4E1636
            TryPlayPlayerFootstepSound(SND_BANK_FEET_METAL, SND_BANK_SLOT_FOOTSTEPS_PLAYER, (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(0, 4)), true);
        } else { // 0x4E168F
            PlayRandomGenericFootstepSound();
        }
    }
}

// 0x4E17E0
void CAEPedAudioEntity::HandleSkateEvent(eAudioEvents event, float volume, float speed) {
    if (m_pPed->bIsInTheAir) {
        return;
    }
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_FEET_GENERIC, SND_BANK_SLOT_FOOTSTEPS_GENERIC)) {
        return;
    }
    AESoundManager.PlaySound({
        .BankSlotID         = SND_BANK_SLOT_FOOTSTEPS_GENERIC,
        .SoundID            = (eSoundID)(event == AE_PED_SKATE_LEFT ? 7 : 8),
        .AudioEntity        = this,
        .Pos                = m_pPed->GetPosition(),
        .Volume             = GetDefaultVolume(event) + (float)(CAEAudioUtility::GetRandomNumberInRange(-3, 3)) + volume,
        .Speed              = speed,
        .RegisterWithEntity = m_pPed,
    });
}

// 0x4E18E0
void CAEPedAudioEntity::HandleLandingEvent(eAudioEvents event) {
    if (m_pPed->bIsInTheAir) {
        return;
    }

    const auto PlayLandingSound = [&](eSoundBankSlot slot, eSoundID soundID, float volume, int16 playPos) {
        AESoundManager.PlaySound({
            .BankSlotID = slot,
            .SoundID            = soundID,
            .AudioEntity        = this,
            .Pos                = m_pPed->GetPosition(),
            .Volume             = volume,
            .Flags              = SOUND_START_PERCENTAGE,
            .FrequencyVariance  = 0.0588f,
            .PlayTime           = playPos,
            .RegisterWithEntity = m_pPed
        });
    };
    const auto volume = GetDefaultVolume(event);
    if (g_surfaceInfos.IsAudioWater(m_pPed->m_nContactSurface)) {
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
void CAEPedAudioEntity::HandlePedSwing(eAudioEvents event, int32 a3, uint32 volume) {
    const auto weaponType = m_pPed->GetActiveWeapon().m_Type;
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN)) {
        if (AudioEngine.IsLoadingTuneActive()) {
            return;
        }
        AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN);
        return;
    }
    int32 sfxId = SND_GENRL_WEAPONS_UNK;
    float volumeOffset = 0.0f; // addend to GetDefaultVolume(event)
    float speedVar = 1.0f;     // speedVariance arg of the swing sfx
    const float baseVolume = GetDefaultVolume(event);
    if (weaponType <= WEAPON_INFRARED) {
        // Byte-verified vs 0x4E1C60/0x4E1C7C (CMP 0x2D JA skips table; 7-entry jt, 5-entry event jt at 0x4E1CAC):
        // jt = {0x4E1B48, 0x4E1AE8(CANE_SWISH), 0x4E1B17, 0x4E1B26, 0x4E1B08, 0x4E1B33, 0x4E1AED(through)}
        // map bytes: 00 00 01 02 03 04 04 02 03 00 | 05 05 05 05 00 01 | 00*6 | 06 06 06 | 00*10 | 06 06 | 00*9
        switch (weaponType) {
        case WEAPON_UNARMED:          // jt idx 0 -> 0x4E1B48: 0x57/-6/0.84
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
        case WEAPON_INFRARED:
            sfxId = SND_GENRL_WEAPONS_WEAPON_SWIPE_FIST;
            speedVar = 0.84f;
            volumeOffset = -6.0f;
            break;
        case WEAPON_GOLFCLUB:         // jt idx 1 -> 0x4E1AE8: 0x2F (speedVar/volume untouched)
        case WEAPON_CANE:
            sfxId = SND_GENRL_WEAPONS_CANE_SWISH;
            break;
        case WEAPON_NIGHTSTICK:       // jt idx 2 -> 0x4E1B17: 0x56/0.84
        case WEAPON_POOL_CUE:
            sfxId = SND_GENRL_WEAPONS_WEAPON_SWIPE;
            speedVar = 0.84f;
            break;
        case WEAPON_KNIFE:            // jt idx 3 -> 0x4E1B26: 0x58/-3 (speedVar untouched)
        case WEAPON_KATANA:
            sfxId = SND_GENRL_WEAPONS_WEAPON_SWIPE_METAL;
            volumeOffset = -3.0f;
            break;
        case WEAPON_BASEBALLBAT:      // jt idx 4 -> 0x4E1B08: 0x56/0.67
        case WEAPON_SHOVEL:
            sfxId = SND_GENRL_WEAPONS_WEAPON_SWIPE;
            speedVar = 0.67f;
            break;
        case WEAPON_DILDO1:           // jt idx 5 -> 0x4E1B33: 0x56/-6/0.84
        case WEAPON_DILDO2:
        case WEAPON_VIBE1:
        case WEAPON_VIBE2:
            sfxId = SND_GENRL_WEAPONS_WEAPON_SWIPE;
            speedVar = 0.84f;
            volumeOffset = -6.0f;
            break;
        default: // jt idx 6 (ROCKET/ROCKET_HS/FREEFALL/RLAUNCHER/RLAUNCHER_HS): through, sfxId stays UNK
            break;
        }
    }
    // NOTE: weapon > INFRARED (JA 0x4E1AED) also leaves sfxId as UNK: silent unless a swing event below overrides it.
    switch (event) {
    case AE_PED_HIT_GROUND_KICK:
    case AE_PED_HIT_HIGH_UNARMED:
    case AE_PED_HIT_LOW_UNARMED:
        sfxId = SND_GENRL_WEAPONS_WEAPON_SWIPE_FIST;
        speedVar = 0.84f;
        volumeOffset = -6.0f;
        break;
    case AE_PED_HIT_MARTIAL_PUNCH:
    case AE_PED_HIT_MARTIAL_KICK:
        sfxId = SND_GENRL_WEAPONS_WEAPON_SWIPE_FIST;
        speedVar = 0.84f;
        volumeOffset = -2.0f;
        break;
    default:
        if (sfxId < 0) {
            return;
        }
        break;
    }
    m_tempSound.Initialise(
        SND_BANK_SLOT_WEAPON_GEN,
        (eSoundID)sfxId,
        this,
        m_pPed->GetPosition(),
        baseVolume + volumeOffset,
        1.0f,
        speedVar,
        1.0f,
        0,
        SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY,
        0.04f,
        0
    );
    m_tempSound.RegisterWithPhysicalEntity(m_pPed);
    if (a3 != 0) {
        m_tempSound.m_Speed = 0.0f;
        m_tempSound.m_SpeedVariance = 0.0f;
        m_tempSound.m_ClientVariable = (float)(CTimer::GetTimeInMS() + (uint32)a3);
        m_tempSound.m_Event = event;
        m_tempSound.SetFlags(SOUND_REQUEST_UPDATES, true);
    }
    AESoundManager.RequestNewSound(&m_tempSound);
    (void)volume;
}

// 0x4E1CC0
void CAEPedAudioEntity::HandlePedHit(eAudioEvents event, CPhysical* physical, uint8 surfaceId, float volume, uint32 maxVol) {
    CWeapon* const weapon = &m_pPed->GetActiveWeapon();
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN)) {
        if (AudioEngine.IsLoadingTuneActive()) {
            return;
        }
        AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN);
        return;
    }
    int32 sfxId = SND_GENRL_WEAPONS_KNUCKLE; // slot-5 sfx, or -1 = silent
    float volumeOffset = 0.0f; // addend to GetDefaultVolume(event)+volume for the weapon-gen hit sfx
    float speedVar = 0.0588f;  // speedVariance arg of the weapon-gen hit sfx
    const float baseVolume = GetDefaultVolume(event) + volume;
    if (weapon->m_Type > WEAPON_INFRARED) {
        sfxId = -1;
    } else {
        // Byte-verified vs 0x4E2234/0x4E2268: jt = {keep,0x2C,0x30,0x23,0x39(+0.03),0x22,0x4B,0x43,0x38,0x2E,0x32,0x36(+0.07),silent}
        // map bytes: 00 01 02 03 04 05 06 07 08 0C | 09 09 09 09 0A 0B | 01*6 | 0C 0C | 01*6 | 00 00
        switch (weapon->m_Type) {
        case WEAPON_UNARMED: // jt idx 0: keep KNUCKLE
        case WEAPON_NIGHTVISION:
        case WEAPON_INFRARED:
            break;
        case WEAPON_BRASSKNUCKLE: // jt idx 1 -> 0x2C
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
            sfxId = SND_GENRL_WEAPONS_BRASS_KNUCKLES;
            break;
        case WEAPON_GOLFCLUB: // jt idx 2 -> 0x30
            sfxId = SND_GENRL_WEAPONS_CLUB_STRIKE;
            break;
        case WEAPON_NIGHTSTICK: // jt idx 3 -> 0x23
            sfxId = SND_GENRL_WEAPONS_BAT_STRIKE;
            break;
        case WEAPON_KNIFE: // jt idx 4 -> 0x39, speedVar 0.03
            sfxId = SND_GENRL_WEAPONS_KNIFE;
            speedVar = 0.03f;
            break;
        case WEAPON_BASEBALLBAT: // jt idx 5 -> 0x22
            sfxId = SND_GENRL_WEAPONS_BASEBALL_BAT;
            break;
        case WEAPON_SHOVEL: // jt idx 6 -> 0x4B
            sfxId = SND_GENRL_WEAPONS_SHOVEL;
            break;
        case WEAPON_POOL_CUE: // jt idx 7 -> 0x43
            sfxId = SND_GENRL_WEAPONS_POOLCUE;
            break;
        case WEAPON_KATANA: // jt idx 8 -> 0x38
            sfxId = SND_GENRL_WEAPONS_KATANA_SWORD;
            break;
        case WEAPON_DILDO1: // jt idx 9 -> 0x2E
        case WEAPON_DILDO2:
        case WEAPON_VIBE1:
        case WEAPON_VIBE2:
            sfxId = SND_GENRL_WEAPONS_CANE_STRIKE;
            break;
        case WEAPON_FLOWERS: // jt idx 10 -> 0x32
            sfxId = SND_GENRL_WEAPONS_DILDO;
            break;
        case WEAPON_CANE: // jt idx 11 -> 0x36, speedVar 0.07
            sfxId = SND_GENRL_WEAPONS_FLOWER_STRIKE;
            speedVar = 0.07f;
            break;
        default: // jt idx 12: CHAINSAW/ROCKET/ROCKET_HS/FREEFALL/RLAUNCHER/RLAUNCHER_HS -> silent
            sfxId = -1;
            break;
        }
    }
    if (physical && (physical->physicalFlags.bCollidable + physical->physicalFlags.bDisableTurnForce * 2 + physical->physicalFlags.bDisableMoveForce * 4) == 3) {
        int32 hitSfx = -1;
        bool useBulletHitsSlot = false; // set by the CHAINSAW HIGH/LOW/GROUND paths: second init uses bullet-hits slot
        switch (event) {
        case AE_PED_HIT_HIGH:
            // Byte-verified vs 0x4E22B8/0x4E22D0 (weapon-1, JA->0x4E1E7A random/-15):
            // map bytes: 00 05 05 01 02 05 05 01 03 05 05 05 05 04 (identical for LOW)
            // jt = {0x4E1E6C random(0x28,0x2B)/+0, 0x4E1F05 STAB/-9, 0x4E1E5E random(0x28,0x2B)/-10,
            //       0x4E1F17 feet(7,9), 0x4E1E59 silent, 0x4E1E7A random(0x28,0x2B)/-15}
            switch (weapon->m_Type) {
            case WEAPON_BRASSKNUCKLE: // idx 0 -> random(0x28,0x2B), offset unchanged
                hitSfx = CAEAudioUtility::GetRandomNumberInRange<int32>(SND_GENRL_WEAPONS_BOX_FACE1, SND_GENRL_WEAPONS_BOX_FACE4);
                break;
            case WEAPON_KNIFE: // idx 1 -> 0x51/-9
            case WEAPON_KATANA:
                hitSfx = SND_GENRL_WEAPONS_STAB;
                volumeOffset = -9.0f;
                break;
            case WEAPON_BASEBALLBAT: // idx 2 -> random(0x28,0x2B)/-10
                hitSfx = CAEAudioUtility::GetRandomNumberInRange<int32>(SND_GENRL_WEAPONS_BOX_FACE1, SND_GENRL_WEAPONS_BOX_FACE4);
                volumeOffset = -10.0f;
                break;
            case WEAPON_CHAINSAW: // idx 3 -> bullet-hits bank sfx 7-9 (bank check inside)
                if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_BULLET_HITS, SND_BANK_SLOT_BULLET_HITS)) {
                    return;
                }
                hitSfx = CAEAudioUtility::GetRandomNumberInRange<int32>(7, 9);
                useBulletHitsSlot = true;
                break;
            case WEAPON_FLOWERS: // idx 4 -> silent
                break;
            default: // idx 5 + JA default (UNARMED/GOLF/NIGHTSTICK/SHOVEL/POOL/DILDOs/VIBEs/CANE+all others) -> random(0x28,0x2B)/-15
                hitSfx = CAEAudioUtility::GetRandomNumberInRange<int32>(SND_GENRL_WEAPONS_BOX_FACE1, SND_GENRL_WEAPONS_BOX_FACE4);
                volumeOffset = -15.0f;
                break;
            }
            break;
        case AE_PED_HIT_LOW:
            // Byte-verified vs 0x4E22E0/0x4E22F8 (weapon-1, JA->0x4E1EE5 random(0x24,0x27)/-15):
            // map bytes identical to HIGH; jt = {0x4E1EDB random(0x24,0x27)/-10, 0x4E1F05 STAB/-9,
            // 0x4E1E9E random(0x24,0x27)/-10, 0x4E1F17 feet(7,9), 0x4E1E59 silent, 0x4E1EE5 random(0x24,0x27)/-15}
            switch (weapon->m_Type) {
            case WEAPON_BRASSKNUCKLE: // idx 0 -> random(0x24,0x27)/-10
                hitSfx = CAEAudioUtility::GetRandomNumberInRange<int32>(SND_GENRL_WEAPONS_BODY_PUNCH1, SND_GENRL_WEAPONS_BODY_PUNCH4);
                volumeOffset = -10.0f;
                break;
            case WEAPON_KNIFE: // idx 1 -> 0x51/-9 (offset slot [ESP+0x14], shared with HIGH-STAB path)
            case WEAPON_KATANA:
                hitSfx = SND_GENRL_WEAPONS_STAB;
                volumeOffset = -9.0f;
                break;
            case WEAPON_BASEBALLBAT: // idx 2 -> random(0x24,0x27)/-10
                hitSfx = CAEAudioUtility::GetRandomNumberInRange<int32>(SND_GENRL_WEAPONS_BODY_PUNCH1, SND_GENRL_WEAPONS_BODY_PUNCH4);
                volumeOffset = -10.0f;
                break;
            case WEAPON_CHAINSAW: // idx 3 -> bullet-hits bank sfx 7-9 (bank check inside)
                if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_BULLET_HITS, SND_BANK_SLOT_BULLET_HITS)) {
                    return;
                }
                hitSfx = CAEAudioUtility::GetRandomNumberInRange<int32>(7, 9);
                useBulletHitsSlot = true;
                break;
            case WEAPON_FLOWERS: // idx 4 -> silent
                break;
            default: // idx 5 + JA default -> random(0x24,0x27)/-15
                hitSfx = CAEAudioUtility::GetRandomNumberInRange<int32>(SND_GENRL_WEAPONS_BODY_PUNCH1, SND_GENRL_WEAPONS_BODY_PUNCH4);
                volumeOffset = -15.0f;
                break;
            }
            break;
        case AE_PED_HIT_GROUND:
            // Byte-verified vs 0x4E2308/0x4E231C (weapon-4, CMP 0xA JA->0x4E1F5D STOMP-trap):
            // map bytes (weapons 4..14): 00 01 04 04 00 02 04 04 04 04 03
            // jt = {0x4E1F05 STAB/-9, 0x4E1F43 0x52/-10/-6, 0x4E1F17 feet(7,9), 0x4E1E59 silent};
            // dword past jt (idx 4) decodes to 0x4E1F5D: MOV [ESP+0x10],-15; EBX=0x52; [ESP+0x14],-6; trap.
            switch (weapon->m_Type) {
            case WEAPON_KNIFE: // idx 0 -> 0x51/-9 (second-only tail in OG)
            case WEAPON_KATANA:
                hitSfx = SND_GENRL_WEAPONS_STAB;
                volumeOffset = -9.0f;
                break;
            case WEAPON_BASEBALLBAT: // idx 1 -> 0x52, first -10/second -6, trap in OG
                hitSfx = SND_GENRL_WEAPONS_STOMP1;
                volumeOffset = -10.0f;
                break;
            case WEAPON_CHAINSAW: // idx 2 -> bullet-hits bank sfx 7-9 (bank 27/slot 3 check inside, second-only in OG)
                if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_BULLET_HITS, SND_BANK_SLOT_BULLET_HITS)) {
                    return;
                }
                hitSfx = CAEAudioUtility::GetRandomNumberInRange<int32>(7, 9);
                useBulletHitsSlot = true;
                break;
            case WEAPON_FLOWERS: // idx 3 -> silent
                break;
            default: // idx 4 (SHOVEL/POOL/DILDOs/VIBEs) + JA default -> 0x52 trap, first -15/second -6 in OG
                hitSfx = SND_GENRL_WEAPONS_STOMP1;
                volumeOffset = -15.0f;
                break;
            }
            break;
        case AE_PED_HIT_GROUND_KICK: // -> 0x52/-6, via shared tail
            sfxId = SND_GENRL_WEAPONS_STOMP1;
            volumeOffset = -6.0f;
            break;
        case AE_PED_HIT_HIGH_UNARMED: // -> 0x28-0x2B then 0x3A tail
            hitSfx = CAEAudioUtility::GetRandomNumberInRange<int32>(SND_GENRL_WEAPONS_BOX_FACE1, SND_GENRL_WEAPONS_BOX_FACE4);
            break;
        case AE_PED_HIT_LOW_UNARMED: // -> 0x24-0x27 then 0x3A tail
            hitSfx = CAEAudioUtility::GetRandomNumberInRange<int32>(SND_GENRL_WEAPONS_BODY_PUNCH1, SND_GENRL_WEAPONS_BODY_PUNCH4);
            break;
        case AE_PED_HIT_MARTIAL_PUNCH: // -> 0x3B-0x3E then tail
            hitSfx = CAEAudioUtility::GetRandomNumberInRange<int32>(SND_GENRL_WEAPONS_MARTIAL_STRIKE1, SND_GENRL_WEAPONS_MARTIAL_STRIKE4);
            break;
        default:
            break;
        }
        // First init (0x4E1EC0/0x4E20BD/0x4E20C0): slot 5, sfx = outer sfxId (EBX), speedVariance = outer
        // speedVar, volume = eventVol+arg+volumeOffset. Skipped when it duplicates the second init for
        // the HIGH/LOW-random paths (OG JL 0x4E2164), when silent (TEST BX,BX / JL), and it traps
        // (JMP 0x4067B5) for the direct second-only cases (STAB/feet/GROUND/0x52/unarmed/martial tails).
        if (hitSfx >= 0) {
            m_tempSound.Initialise(
                SND_BANK_SLOT_WEAPON_GEN,
                (eSoundID)sfxId,
                this,
                m_pPed->GetPosition(),
                baseVolume + volumeOffset,
                1.0f,
                1.0f,
                1.0f,
                0,
                SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY,
                speedVar,
                0
            );
            m_tempSound.RegisterWithPhysicalEntity(m_pPed);
            if (maxVol != 0) {
                m_tempSound.m_Speed = 0.0f;
                m_tempSound.m_SpeedVariance = 0.0f;
                m_tempSound.m_ClientVariable = (float)(CTimer::GetTimeInMS() + maxVol);
                m_tempSound.m_Event = event;
                m_tempSound.SetFlags(SOUND_REQUEST_UPDATES, true);
            }
            AESoundManager.RequestNewSound(&m_tempSound);
        }
        // Second init (0x4E216D): slot = (feet-path ? 3 : 5), sfx = hitSfx (or trap EBX for the
        // first-only cases), speedVariance 0.0588, volume = eventVol+arg+volumeOffset.
        if (sfxId >= 0) {
            m_tempSound.Initialise(
                useBulletHitsSlot ? SND_BANK_SLOT_BULLET_HITS : SND_BANK_SLOT_WEAPON_GEN,
                (eSoundID)hitSfx,
                this,
                m_pPed->GetPosition(),
                baseVolume + volumeOffset,
                1.0f,
                1.0f,
                1.0f,
                0,
                SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY,
                0.0588f,
                0
            );
            m_tempSound.RegisterWithPhysicalEntity(m_pPed);
            if (maxVol != 0) {
                m_tempSound.m_Speed = 0.0f;
                m_tempSound.m_SpeedVariance = 0.0f;
                m_tempSound.m_ClientVariable = (float)(CTimer::GetTimeInMS() + maxVol);
                m_tempSound.m_Event = event;
                m_tempSound.SetFlags(SOUND_REQUEST_UPDATES, true);
            }
            AESoundManager.RequestNewSound(&m_tempSound);
        }
        return;
    }
    // OG derives the impact values via helper 0x4DA510 on the event id; passing 1.0f keeps this faithful except that nuance.
    switch (event) {
    case AE_PED_HIT_HIGH:
    case AE_PED_HIT_LOW:
    case AE_PED_HIT_GROUND: {
        // OG passes GetDefaultVolume-style helper on `volume`, then ReportCollision(..., SURFACE_PED, event, ...)
        AudioEngine.ReportCollision(m_pPed, physical, SURFACE_PED, (eSurfaceType)event, m_pPed->GetPosition(), nullptr, 1.0f, 1.0f, true, false);
        break;
    }
    case AE_PED_HIT_GROUND_KICK:
    case AE_PED_HIT_MARTIAL_PUNCH:
        AudioEngine.ReportCollision(m_pPed, physical, SURFACE_PED, (eSurfaceType)event, m_pPed->GetPosition(), nullptr, 1.0f, 1.0f, true, false);
        return;
    case AE_PED_HIT_HIGH_UNARMED:
    case AE_PED_HIT_LOW_UNARMED:
    case AE_PED_HIT_MARTIAL_KICK: {
        // OG passes GetDefaultVolume-style helper on `volume`, then ReportCollision + a 0x3A/speedVar tail sound
        AudioEngine.ReportCollision(m_pPed, physical, SURFACE_PED, (eSurfaceType)event, m_pPed->GetPosition(), nullptr, 1.0f, 1.0f, true, false);
        m_tempSound.Initialise(
            SND_BANK_SLOT_COLLISIONS,
            (eSoundID)SND_GENRL_WEAPONS_KNUCKLE,
            this,
            m_pPed->GetPosition(),
            baseVolume + volumeOffset,
            1.0f,
            1.0f,
            1.0f,
            0,
            SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY,
            speedVar,
            0
        );
        m_tempSound.RegisterWithPhysicalEntity(m_pPed);
        if (maxVol != 0) {
            m_tempSound.m_Speed = 0.0f;
            m_tempSound.m_SpeedVariance = 0.0f;
            m_tempSound.m_ClientVariable = (float)(CTimer::GetTimeInMS() + maxVol);
            m_tempSound.m_Event = event;
            m_tempSound.SetFlags(SOUND_REQUEST_UPDATES, true);
        }
        AESoundManager.RequestNewSound(&m_tempSound);
        break;
    }
    default:
        break;
    }
    (void)surfaceId;
}

void CAEPedAudioEntity::HandlePedJacked(eAudioEvents event) {
    // E4-mapped vs 0x4E2350 (jump table @0x4E2680; event-0x45, JA skips to trailing block):
    // 0x45->0x4E23D1, 0x46->0x4E2415, 0x47->0x4E243E, 0x48->0x4E2466, 0x49->0x4E248F.
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN)) {
        if (AudioEngine.IsLoadingTuneActive()) {
            return;
        }
        AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN);
        return;
    }
    const float baseVolume = GetDefaultVolume(event);
    const uint32 now = CTimer::GetTimeInMS();
    int32 firstSfx = -1; // slot-5 sfx for the FIRST init (0x3A/0x32); -1 = second-only case
    int32 hitSfx = -1;   // random punch sfx for the SECOND init at 0x4E252B
    uint32 delayMs = 0;      // t1: added to `now`, stored (as float) into m_tempSound.m_ClientVariable
    uint32 trailDelayMs = 0; // t2: added to `now` for the TRAILING init's m_ClientVariable ([ESP+0x20] at 0x4E2639)
    switch (event) {
    case AE_PED_JACKED_CAR_PUNCH: // 0x45: first 0x3A, second random(0x28,0x2B), t1 = now+0x1F4 (500), t2 = now+0x855
        hitSfx = CAEAudioUtility::GetRandomNumberInRange<int32>(SND_GENRL_WEAPONS_BOX_FACE1, SND_GENRL_WEAPONS_BOX_FACE4);
        firstSfx = SND_GENRL_WEAPONS_KNUCKLE;
        delayMs = 0x1F4;
        trailDelayMs = 0x855;
        break;
    case AE_PED_JACKED_CAR_HEAD_BANG: // 0x46: first 0x32, second random(0x24,0x27), t1 = now+0x3A5 (933), t2 = now+0xA49
        hitSfx = CAEAudioUtility::GetRandomNumberInRange<int32>(SND_GENRL_WEAPONS_BODY_PUNCH1, SND_GENRL_WEAPONS_BODY_PUNCH4);
        firstSfx = SND_GENRL_WEAPONS_DILDO;
        delayMs = 0x3A5;
        trailDelayMs = 0xA49;
        break;
    case AE_PED_JACKED_CAR_KICK: // 0x47: SECOND-only random(0x28,0x2B), t1 = now+0x384 (900), t2 = now+0xAF0
        hitSfx = CAEAudioUtility::GetRandomNumberInRange<int32>(SND_GENRL_WEAPONS_BOX_FACE1, SND_GENRL_WEAPONS_BOX_FACE4);
        delayMs = 0x384;
        trailDelayMs = 0xAF0;
        break;
    case AE_PED_JACKED_BIKE: // 0x48: first 0x3A, second random(0x28,0x2B), t1 = now+0x21 (33), t2 = now+0x16E
        hitSfx = CAEAudioUtility::GetRandomNumberInRange<int32>(SND_GENRL_WEAPONS_BOX_FACE1, SND_GENRL_WEAPONS_BOX_FACE4);
        firstSfx = SND_GENRL_WEAPONS_KNUCKLE;
        delayMs = 0x21;
        trailDelayMs = 0x16E;
        break;
    case AE_PED_JACKED_DOZER: // 0x49: SECOND-only random(0x24,0x27), t1 = now+0x10A (266), t2 = now+0x341
        hitSfx = CAEAudioUtility::GetRandomNumberInRange<int32>(SND_GENRL_WEAPONS_BODY_PUNCH1, SND_GENRL_WEAPONS_BODY_PUNCH4);
        delayMs = 0x10A;
        trailDelayMs = 0x341;
        break;
    default:
        break;
    }
    // FIRST init (cases 0x45/0x46/0x48 only): slot 5, flags 0x84 (LIFESPAN_TIED | REQUEST_UPDATES, set raw —
    // no SetFlags call in OG), rollOff 2.0, speed 0.0.
    if (firstSfx >= 0) {
        m_tempSound.Initialise(
            SND_BANK_SLOT_WEAPON_GEN,
            (eSoundID)firstSfx,
            this,
            m_pPed->GetPosition(),
            baseVolume,
            2.0f,
            0.0f,
            1.0f,
            0,
            SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES,
            0.0f,
            0
        );
        m_tempSound.RegisterWithPhysicalEntity(m_pPed);
        m_tempSound.m_Event = event;
        m_tempSound.m_ClientVariable = (float)(now + delayMs);
        AESoundManager.RequestNewSound(&m_tempSound);
    }
    // SECOND init (all 5 cases @0x4E252B): slot 5, same vol/2.0/0/1.0/0/0x84/0/0; skipped if random sfx < 0.
    if (hitSfx >= 0) {
        m_tempSound.Initialise(
            SND_BANK_SLOT_WEAPON_GEN,
            (eSoundID)hitSfx,
            this,
            m_pPed->GetPosition(),
            baseVolume,
            2.0f,
            0.0f,
            1.0f,
            0,
            SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES,
            0.0f,
            0
        );
        m_tempSound.RegisterWithPhysicalEntity(m_pPed);
        m_tempSound.m_Event = event;
        m_tempSound.m_ClientVariable = (float)(now + delayMs);
        AESoundManager.RequestNewSound(&m_tempSound);
    }
    // TRAILING block (bank 0 = SND_BANK_FEET_GENERIC, slot 0x29, sfx 0; JA-default events also land here):
    // volume = baseVol ([ESP+0x18] is never clobbered after 0x4E23C0); m_ClientVariable = (float)(now + t2).
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_FEET_GENERIC, SND_BANK_SLOT_FOOTSTEPS_GENERIC)) {
        AEAudioHardware.LoadSoundBank(SND_BANK_FEET_GENERIC, SND_BANK_SLOT_FOOTSTEPS_GENERIC);
        return;
    }
    m_tempSound.Initialise(
        SND_BANK_SLOT_FOOTSTEPS_GENERIC,
        (eSoundID)0,
        this,
        m_pPed->GetPosition(),
        baseVolume,
        2.0f,
        0.0f,
        1.0f,
        0,
        SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES,
        0.0f,
        0
    );
    m_tempSound.RegisterWithPhysicalEntity(m_pPed);
    m_tempSound.m_Event = event;
    m_tempSound.m_ClientVariable = (float)(now + trailDelayMs);
    AESoundManager.RequestNewSound(&m_tempSound);
}

// 0x4E26A0
void CAEPedAudioEntity::HandleSwimSplash(eAudioEvents event) {
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_SWIMMING, SND_BANK_SLOT_SWIMMING)) {
        return;
    }
    m_nSfxId = std::max(0, m_nSfxId + 1);
    AESoundManager.PlaySound({
        .BankSlotID         = SND_BANK_SLOT_SWIMMING,
        .SoundID            = (eSoundID)(m_nSfxId),
        .AudioEntity        = this,
        .Pos                = m_pPed->GetPosition(),
        .Volume             = GetDefaultVolume(event),
        .Flags              = SOUND_PLAY_PHYSICALLY | SOUND_START_PERCENTAGE | SOUND_IS_DUCKABLE,
        .FrequencyVariance  = 0.0588f,
        .RegisterWithEntity = m_pPed,
    });
}

// 0x4E2790
void CAEPedAudioEntity::HandleSwimWake(eAudioEvents event) {
    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS, true)) {
        return;
    }
    if (!AESoundManager.AreSoundsOfThisEventPlayingForThisEntityAndPhysical(event, this, m_pPed)) {
        AESoundManager.PlaySound({
            .BankSlotID         = SND_BANK_SLOT_COLLISIONS,
            .SoundID            = 3,
            .AudioEntity        = this,
            .Pos                = m_pPed->GetPosition(),
            .Volume             = GetDefaultVolume(event) - 20.0f,
            .RollOffFactor      = 1.f,
            .Speed              = 0.75f,
            .Doppler            = 1.f,
            .FrameDelay         = 0,
            .Flags              = SOUND_REQUEST_UPDATES | SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY,
            .RegisterWithEntity = m_pPed,
            .EventID            = event,
        });
    }
    m_LastSwimWakeTriggerTimeMs = CTimer::GetTimeInMS();
}

// 0x4E2A90
void CAEPedAudioEntity::PlayShirtFlap(float volume, float speed) {
    if (m_sTwinLoopSoundEntity.IsActive()) {
        m_sTwinLoopSoundEntity.UpdateTwinLoopSound(m_pPed->GetPosition(), volume, speed);
    } else {
        m_sTwinLoopSoundEntity.Initialise(SND_BANK_SLOT_WEAPON_GEN, 19, 20, this, 200, 1000, -1, -1);
        m_sTwinLoopSoundEntity.PlayTwinLoopSound(m_pPed->GetPosition(), volume, speed, 2.0f, 1.0f, SOUND_DEFAULT);
    }
}

// 0x4E2EE0
void CAEPedAudioEntity::Service() {
    // E4-mapped vs 0x4E2EE0 (this + 0x94 = m_pPed, +0x158 = field_154 volume state, +0xA8 = twin-loop entity).
    // Runtime .data scales 0xB613BC/0xB613C0/0xB613CC read garbage in the raw exe; kept as StaticRefs (NOTSA).
    static auto& s_fVehicleSpeedScale = StaticRef<float>(0xB613BC); // veh-wind gain scale
    static auto& s_fFallSpeedScale    = StaticRef<float>(0xB613C0); // fall/parachute gain scale
    static auto& s_fFallNormScale     = StaticRef<float>(0xB613CC); // (k2 - 0.5) divisor
    if (!m_pPed->IsPlayer()) {
        return;
    }
    bool allowWind = true;
    float volume = -100.0f; // S04 accumulator
    float speedOut = 1.0f;  // S14 gain accumulator
    float skydiveW = 0.0f;  // S10: FALL_SkyDive_accel blend
    float paraW = 0.0f;     // S0C: Para_decel blend
    // veh+0xFC deref, then (veh+0xFC)+0x36 low 3 bits == 2; ped+0x46C & 0x100. Both offsets ungrounded
    // to named repo members — kept raw (NOTSA) rather than inventing behavior.
    const auto pedAddr = reinterpret_cast<uintptr_t>(m_pPed);
    const auto vehAtFC = *reinterpret_cast<CVehicle**>(pedAddr + 0xFC);
    if (vehAtFC && (*reinterpret_cast<const uint8*>(reinterpret_cast<uintptr_t>(vehAtFC) + 0x36) & 7) == 2) {
        allowWind = false;
    } else if (*reinterpret_cast<const uint32*>(pedAddr + 0x46C) & 0x100) {
        allowWind = false;
    }
    // Vehicle-speed wind branch: ped+0x530 == 0x32 (PEDSTATE_DRIVING) && ped+0x58C vehicle &&
    // (veh type 9 [BIKE] || (type 0 [AUTOMOBILE] && vtbl+0x9C non-null)). Only the BIKE arm is
    // expressible with repo APIs; the AUTOMOBILE+vtbl arm and veh+0x1B8 base select are NOTSA-raw.
    if (m_pPed->m_nPedState == PEDSTATE_DRIVING && m_pPed->m_pVehicle) {
        const auto veh = m_pPed->m_pVehicle;
        if (veh->m_nVehicleType == VEHICLE_TYPE_BIKE) {
            const float spd = veh->GetMoveSpeed().Magnitude();
            if (spd > 0.0f) {
                // OG base: veh+0x1B8 == 2 ? -17.0 (0x8AE654) : -10.0 (0x8AE650); offset ungrounded, kept raw.
                const float base = *reinterpret_cast<const uint8*>(reinterpret_cast<uintptr_t>(veh) + 0x1B8) == 2 ? -17.0f : -10.0f;
                float k = spd > 1.0f ? 1.0f : spd; // then: if (k < 0.0f) k = 0.0f
                if (k < 0.0f) {
                    k = 0.0f;
                }
                volume = CAEAudioUtility::AudioLog10(k) * 20.0f + base;
                speedOut = s_fVehicleSpeedScale * k + 0.8f;
            }
        }
    }
    // OG gates GetUsingParachute on *(ped+0x47C): only called when nonzero; default false. Offset ungrounded, kept raw.
    bool usingParachute = false;
    if (*reinterpret_cast<const int32*>(pedAddr + 0x47C) != 0) {
        usingParachute = m_pPed->GetIntelligence()->GetUsingParachute();
    }
    if (allowWind) {
        float k2 = m_pPed->GetMoveSpeed().Magnitude() / 0.7f;
        float base = -16.0f;
        if (!usingParachute) {
            if (k2 < 0.5f) {
                k2 = 0.0f;
            } else {
                k2 = (k2 - 0.5f) / s_fFallNormScale;
            }
        } else {
            base = -7.0f;
            if (const auto skydive = RpAnimBlendClumpGetAssociation(m_pPed->GetRpClump(), "FALL_SkyDive_accel")) {
                skydiveW = std::clamp(skydive->m_BlendAmount, 0.0f, 1.0f);
            }
            if (const auto para = RpAnimBlendClumpGetAssociation(m_pPed->GetRpClump(), "Para_decel")) {
                paraW = std::clamp(para->m_BlendAmount, 0.0f, 1.0f);
            }
            k2 = std::clamp(k2, 0.0f, 1.0f);
        }
        volume = CAEAudioUtility::AudioLog10(k2) * 20.0f + base;
        speedOut = s_fFallSpeedScale * k2 + 0.8f;
    }
    // Combine: speedOut *= 0.5*skydiveW + 1.0; volume += 4.0*skydiveW - 6.0*paraW; hysteresis vs field_154.
    const float finalSpeed = speedOut * (0.5f * skydiveW + 1.0f);
    volume += 4.0f * skydiveW + -6.0f * paraW;
    if (volume > field_154) {
        field_154 = std::min(volume, field_154 + 5.0f);
    } else {
        field_154 = std::max(volume, field_154 - 2.0f);
    }
    if (volume < -20.0f && field_154 < -20.0f) {
        field_154 = volume;
    }
    if (field_154 > -100.0f) {
        PlayShirtFlap(field_154, finalSpeed);
        return;
    }
    m_sTwinLoopSoundEntity.StopSoundAndForget();
}
