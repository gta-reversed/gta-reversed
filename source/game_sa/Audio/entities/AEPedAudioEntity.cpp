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
    RH_ScopedInstall(HandleFootstepEvent, 0x4E13A0, { .reversed = false });
    RH_ScopedInstall(HandleSkateEvent, 0x4E17E0);
    RH_ScopedInstall(HandleLandingEvent, 0x4E18E0);
    RH_ScopedInstall(HandlePedSwing, 0x4E1A40, { .reversed = false });
    RH_ScopedInstall(HandlePedHit, 0x4E1CC0, { .reversed = false });
    RH_ScopedInstall(HandlePedJacked, 0x4E2350, { .reversed = false });
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
    m_nTimeInMS = 0;

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
    AEAudioHardware.LoadSoundBank(0, 41);
    AEAudioHardware.LoadSoundBank(128, 32);
}

// 0x4E1360
void CAEPedAudioEntity::Terminate() {
    m_bCanAddEvent = false;
    m_pPed   = nullptr;
    StopJetPackSound();
    AESoundManager.CancelSoundsOwnedByAudioEntity(this, true);
    if (m_sTwinLoopSoundEntity.m_bIsInitialised) {
        m_sTwinLoopSoundEntity.StopSoundAndForget();
    }
}

// 0x4E2BB0
void CAEPedAudioEntity::AddAudioEvent(eAudioEvents event, float volume, float speed, CPhysical* ped, uint8 surfaceId, int32 a7, uint32 maxVol) {
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
        if (!AEAudioHardware.IsSoundBankLoaded(39u, 2) || AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_PED_CRUNCH, this))
            break;

        const auto vol = GetDefaultVolume(AE_PED_CRUNCH) + volume;
        CAESound sound;
        sound.Initialise(2, 29, this, ped->GetPosition(), vol, 1.0f, 1.0f, 1.0f, 0, SOUND_DEFAULT, 0.0f, 0);
        sound.m_fSpeed = speed;
        sound.m_fSpeedVariability = 0.06f;
        sound.m_fSoundDistance = 1.5f;
        sound.m_nEvent = AE_PED_CRUNCH;
        AESoundManager.RequestNewSound(&sound);

        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_PED_KNOCK_DOWN, this) != 0)
            break;

        auto RandomNumberInRange = CAEAudioUtility::GetRandomNumberInRange(47, 49);
        sound.Initialise(2, RandomNumberInRange, this, ped->GetPosition(), vol, 1.0f, 1.0f, 1.0f, 0, SOUND_DEFAULT, 0.0f, 0);
        sound.m_fSpeed = speed;
        sound.m_fSpeedVariability = 0.06f;
        sound.m_fSoundDistance = 1.5f;
        sound.m_nEvent = AE_PED_KNOCK_DOWN;
        AESoundManager.RequestNewSound(&sound);
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

    m_tempSound.Initialise(19, 26, this, m_pPed->GetPosition(), -100.0f, 1.0f, 1.0f, 1.0f, 0, SOUND_DEFAULT, 0.0f, 0);
    m_tempSound.m_fSpeed = 1.0f;
    m_tempSound.m_nEnvironmentFlags = SOUND_REQUEST_UPDATES;
    m_JetPackSound0 = AESoundManager.RequestNewSound(&m_tempSound);

    m_tempSound.Initialise(5, 10, this, m_pPed->GetPosition(), -100.0f, 1.0f, 1.0f, 1.0f, 0, SOUND_DEFAULT, 0.0f, 0);
    m_tempSound.m_fSpeed = 1.0f;
    m_tempSound.m_nEnvironmentFlags = SOUND_REQUEST_UPDATES;
    m_JetPackSound1 = AESoundManager.RequestNewSound(&m_tempSound);

    m_tempSound.Initialise(0, 0, this, m_pPed->GetPosition(), -100.0f, 1.0f, 1.0f, 1.0f, 0, SOUND_DEFAULT, 0.0f, 0);
    m_tempSound.m_fSpeed = 1.0f;
    m_tempSound.m_nEnvironmentFlags = SOUND_REQUEST_UPDATES;
    m_JetPackSound2 = AESoundManager.RequestNewSound(&m_tempSound);
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
    m_JetPackSound0->m_fVolume = m_fVolume1 + 0.0f;
    m_JetPackSound0->m_fSpeed = speed * -0.07f + 1.0f;

    m_JetPackSound1->m_fVolume = m_fVolume2 + 0.0f;
    m_JetPackSound1->m_fSpeed = 0.56f;

    m_JetPackSound2->m_fVolume = m_fVolume3 + 0.0f;
    m_JetPackSound2->m_fSpeed = (speed / 5.0f + 1.0f) * m_JetPackSoundSpeedMult;
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

    switch (sound->m_nEvent) {
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
        sound->m_fSpeed = 1.0f;
        return;
    case AE_PED_SWIM_WAKE: {
        const auto volume = GetDefaultVolume(AE_PED_SWIM_WAKE);

        if (CTimer::GetTimeInMS() <= m_nTimeInMS + 100) {
            if (sound->m_fVolume >= volume) {
                return;
            }

            if (sound->m_fVolume + 0.6f >= volume) {
                sound->m_fVolume = volume;
                return;
            }
        } else {
            auto vol = volume - 20.0f;
            if (sound->m_fVolume <= vol) {
                sound->StopSoundAndForget();
                m_nTimeInMS = 0;
                return;
            }
            sound->m_fVolume = std::max(sound->m_fVolume - 0.6f, vol);
        }

        return;
    }
    default:
        return;
    }
}

// 0x4E13A0
void CAEPedAudioEntity::HandleFootstepEvent(eAudioEvents event, float volume, float speed, uint8 surfaceId) {
    plugin::CallMethod<0x4E13A0, CAEPedAudioEntity*, int32, float, float, uint8>(this, event, volume, speed, surfaceId);
}

// 0x4E17E0
void CAEPedAudioEntity::HandleSkateEvent(eAudioEvents event, float volume, float speed) {
    if (m_pPed->bIsInTheAir)
        return;

    if (!AEAudioHardware.IsSoundBankLoaded(0, 41)) {
        AEAudioHardware.LoadSoundBank(0, 41);
        return;
    }

    const auto vol = (
          (float)CAEAudioUtility::GetRandomNumberInRange(-3, 3)
        + GetDefaultVolume(event)
        + volume
    );
    const auto sfxId = (event != AE_PED_SKATE_LEFT) + 7; // what?
    m_tempSound.Initialise(41, sfxId, this, m_pPed->GetPosition(), vol, 1.0f, speed, 1.0f, 0, SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY, 0.0588f, 0);
    m_tempSound.RegisterWithPhysicalEntity(m_pPed);
    AESoundManager.RequestNewSound(&m_tempSound);
}

// 0x4E18E0
void CAEPedAudioEntity::HandleLandingEvent(eAudioEvents event) {
    if (m_pPed->bIsInTheAir)
        return;

    int16 bankSlotId;
    int16 sfxId;
    int16 playPosn;
    auto volume = GetDefaultVolume(event);
    if (g_surfaceInfos.IsAudioWater(m_pPed->m_nContactSurface)) {
        if (!AEAudioHardware.IsSoundBankLoaded(128u, 32)) {
            AEAudioHardware.LoadSoundBank(128, 32);
            return;
        }

        bankSlotId = 32;
        sfxId = CAEAudioUtility::GetRandomNumberInRange(0, 4);
        playPosn = 50;
        if (volume <= 0.0f) {
            volume = 0.0f;
        }
    } else {
        if (!AEAudioHardware.IsSoundBankLoaded(0, 41)) {
            AEAudioHardware.LoadSoundBank(0, 41);
            return;
        }

        bankSlotId = 41;
        sfxId = event != 58 ? 0 : 6;
        playPosn = 0;
    }

    const auto flags = (eSoundEnvironment)(SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_START_PERCENTAGE);
    m_tempSound.Initialise(bankSlotId, sfxId, this, m_pPed->GetPosition(), volume, 1.0f, 1.0f, 1.0f, 0, flags, 0.0588f, playPosn);
    m_tempSound.RegisterWithPhysicalEntity(m_pPed);
    AESoundManager.RequestNewSound(&m_tempSound);
}

// 0x4E1A40
void CAEPedAudioEntity::HandlePedSwing(eAudioEvents event, int32 a3, uint32 volume) {
    plugin::CallMethod<0x4E1A40, CAEPedAudioEntity*, int32, int32, uint32>(this, event, a3, volume);
}

// 0x4E1CC0
void CAEPedAudioEntity::HandlePedHit(eAudioEvents event, CPhysical* physical, uint8 surfaceId, float volume, uint32 maxVol) {
    plugin::CallMethod<0x4E1CC0, CAEPedAudioEntity*, int32, CPhysical*, uint8, float, uint32>(this, event, physical, surfaceId, volume, maxVol);
}

// 0x4E2350
void CAEPedAudioEntity::HandlePedJacked(eAudioEvents event) {
    plugin::CallMethod<0x4E2350, CAEPedAudioEntity*, int32>(this, event);
}

// 0x4E26A0
void CAEPedAudioEntity::HandleSwimSplash(eAudioEvents event) {
    if (!AEAudioHardware.IsSoundBankLoaded(128, 32)) {
        AEAudioHardware.LoadSoundBank(128, 32);
        return;
    }

    const auto volume = GetDefaultVolume(event);
    m_nSfxId = (m_nSfxId + 1) % 4;
    m_tempSound.Initialise(32, m_nSfxId, this, m_pPed->GetPosition(), volume, 1.0f, 1.0f, 1.0f, 0, SOUND_DEFAULT, 0.0f, 0);
    m_tempSound.m_fSpeedVariability = 0.0588f;
    m_tempSound.SetIndividualEnvironment(SOUND_PLAY_PHYSICALLY | SOUND_START_PERCENTAGE | SOUND_UNDUCKABLE, true);
    m_tempSound.RegisterWithPhysicalEntity(m_pPed);
    AESoundManager.RequestNewSound(&m_tempSound);
}

// 0x4E2790
void CAEPedAudioEntity::HandleSwimWake(eAudioEvents event) {
    if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntityAndPhysical(event, this, m_pPed)) {
        m_nTimeInMS = CTimer::GetTimeInMS();
        return;
    }

    if (AEAudioHardware.IsSoundBankLoaded(39u, 2)) {
        auto volume = GetDefaultVolume(event) - 20.0f;
        m_tempSound.Initialise(2, 3, this, m_pPed->GetPosition(), volume, 1.0f, 1.0f, 1.0f, 0, SOUND_DEFAULT, 0.0f, 0);
        m_tempSound.m_fSpeed = 0.75f;
        m_tempSound.m_nEnvironmentFlags = SOUND_REQUEST_UPDATES | SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY;
        m_tempSound.m_nEvent = event;
        m_tempSound.RegisterWithPhysicalEntity(m_pPed);
        AESoundManager.RequestNewSound(&m_tempSound);
        m_nTimeInMS = CTimer::GetTimeInMS();
        return;
    }

    if (!AudioEngine.IsLoadingTuneActive()) {
        AEAudioHardware.LoadSoundBank(39, 2);
    }
}

// 0x4E2A90
void CAEPedAudioEntity::PlayShirtFlap(float volume, float speed) {
    if (m_sTwinLoopSoundEntity.m_bIsInitialised) {
        m_sTwinLoopSoundEntity.UpdateTwinLoopSound(m_pPed->GetPosition(), volume, speed);
    } else {
        m_sTwinLoopSoundEntity.Initialise(5, 19, 20, this, 200, 1000, -1, -1);
        m_sTwinLoopSoundEntity.PlayTwinLoopSound(m_pPed->GetPosition(), volume, speed, 2.0f, 1.0f, SOUND_DEFAULT);
    }
}

// 0x4E2EE0
void CAEPedAudioEntity::Service() {
    plugin::CallMethod<0x4E2EE0, CAEPedAudioEntity*>(this);
}
