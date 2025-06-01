#include "StdInc.h"

#include "AEFireAudioEntity.h"
#include "AEAudioHardware.h"

uint8& CAEFireAudioEntity::m_snLastFireFrequencyIndex = *(uint8*)0xB612EC;
float gfFireFrequencyVariations[5] = { 1.16f, 1.08f, 1.0f, 0.92f, 0.84f }; // 0x8AE598

// 0x4DCF20
void CAEFireAudioEntity::Initialise(FxSystem_c* system) {
    m_FxSystem   = system;
    m_SoundLeft  = nullptr;
    m_SoundRight = nullptr;
}

// 0x5B9A90
void CAEFireAudioEntity::StaticInitialise() {
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_RAIN, SND_BANK_SLOT_WEATHER);
}

// 0x4DCF40
void CAEFireAudioEntity::Terminate() {
    AESoundManager.CancelSoundsOwnedByAudioEntity(this, 1);
    m_FxSystem = nullptr;
}

// 0x4DD3C0
void CAEFireAudioEntity::AddAudioEvent(eAudioEvents audioId, CVector& posn) {
    if (audioId == AE_FIRE_HYDRANT) {
        if (m_FxSystem && !m_SoundLeft && !m_SoundRight)
            PlayWaterSounds(AE_FIRE_HYDRANT, posn);
    } else if (audioId <= AE_FIRE_HYDRANT || audioId > AE_HELI_DUST) {
        if (m_FxSystem && !m_SoundLeft) {
            PlayFireSounds(audioId, posn);
        }
    }
}

// 0x4DD0D0
void CAEFireAudioEntity::PlayFireSounds(eAudioEvents audioId, CVector& posn) {
    m_snLastFireFrequencyIndex = (m_snLastFireFrequencyIndex + 1) % 5;
    const float volume = GetDefaultVolume(audioId);

    m_SoundLeft = AESoundManager.PlaySound({
        .BankSlotID      = SND_BANK_SLOT_EXPLOSIONS,
        .SoundID         = 0,
        .AudioEntity     = this,
        .Pos             = posn,
        .Volume          = volume,
        .RollOffFactor   = 2.0f,
        .Speed           = gfFireFrequencyVariations[m_snLastFireFrequencyIndex],
        .Flags           = SOUND_REQUEST_UPDATES,
    });

    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) {
        return;
    }

    AESoundManager.PlaySound({
        .BankSlotID      = SND_BANK_SLOT_VEHICLE_GEN,
        .SoundID         = 26,
        .AudioEntity     = this,
        .Pos             = posn,
        .Volume          = volume - 17.0f,
        .RollOffFactor   = 2.0f,
        .Speed           = gfFireFrequencyVariations[m_snLastFireFrequencyIndex] * 0.6f,
        .Doppler         = 1.0f,
        .FrameDelay      = 0,
        .Flags           = SOUND_REQUEST_UPDATES,
        .FrequencyVariance = 0.0f,
        .PlayTime        = 0,
        .EventID         = AE_FRONTEND_SELECT,
        .ClientVariable  = volume + 3.0f,
    });
}

// 0x4DD270
void CAEFireAudioEntity::PlayWaterSounds(eAudioEvents audioId, CVector& posn) {
    m_SoundLeft = AESoundManager.PlaySound({
         .BankSlotID        = SND_BANK_SLOT_COLLISIONS,
         .SoundID           = 3,
         .AudioEntity       = this,
         .Pos               = posn,
         .Volume            = GetDefaultVolume(audioId),
         .RollOffFactor     = 2.0f,
         .Speed             = 0.75f,
         .Doppler           = 0.6f,
         .FrameDelay        = 0,
         .Flags             = SOUND_REQUEST_UPDATES,
         .FrequencyVariance = 0.06f,
         .PlayTime          = 0,
         .EventID           = AE_FRONTEND_HIGHLIGHT,
    });

    m_SoundRight = AESoundManager.PlaySound({
        .BankSlotID        = SND_BANK_SLOT_FRONTEND_GAME,
        .SoundID           = 0,
        .AudioEntity       = this,
        .Pos               = posn,
        .Volume            = GetDefaultVolume(audioId) + 20.0f,
        .RollOffFactor     = 2.0f,
        .Speed             = 1.78f,
        .Doppler           = 0.6f,
        .FrameDelay        = 0,
        .Flags             = SOUND_REQUEST_UPDATES,
        .FrequencyVariance = 0.06f,
        .PlayTime          = 0,
        .EventID           = AE_FRONTEND_ERROR,
    });
}

// 0x4DCF60
void CAEFireAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (curPlayPos <= 0) {
        if (curPlayPos == -1) {
            if (sound == m_SoundLeft) {
                m_SoundLeft = nullptr;
            } else if (sound == m_SoundRight) {
                m_SoundRight = nullptr;
            }
        }
        return;
    }

    if (m_FxSystem) {
        RwMatrix matrix;
        m_FxSystem->GetCompositeMatrix(&matrix);
        sound->SetPosition(matrix.pos);
    }

    switch (sound->m_Event) {
    case AE_FRONTEND_SELECT:
        if (sound->m_Volume >= sound->m_ClientVariable) {
            sound->m_Event = AE_FRONTEND_BACK;
        } else {
            sound->m_Volume = std::min(sound->m_Volume + 2.0f, sound->m_ClientVariable);
        }
        break;
    case AE_FRONTEND_BACK:
        if (sound->m_Volume <= -30.0f)
            sound->StopSoundAndForget();
        else
            sound->m_Volume -= 0.75f;
        break;
    case AE_FRONTEND_ERROR:
        if (m_FxSystem && m_FxSystem->GetPlayStatus() == eFxSystemPlayStatus::FX_STOPPED) {
            sound->m_Volume = -100.0f;
        }
        break;
    case AE_FRONTEND_HIGHLIGHT:
        if (m_FxSystem && m_FxSystem->GetPlayStatus() == eFxSystemPlayStatus::FX_STOPPED) {
            if (sound->m_Volume > -100.0f) {
                sound->m_Volume -= 1.0f;
            }
        }
        break;
    }
}

void CAEFireAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEFireAudioEntity, 0x85AA94, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Initialise, 0x4DCF20);
    RH_ScopedInstall(StaticInitialise, 0x5B9A90);
    RH_ScopedInstall(Terminate, 0x4DCF40);
    RH_ScopedInstall(AddAudioEvent, 0x4DD3C0);
    RH_ScopedInstall(PlayFireSounds, 0x4DD0D0);
    RH_ScopedInstall(PlayWaterSounds, 0x4DD270);
    RH_ScopedVMTInstall(UpdateParameters, 0x4DCF60);
}
