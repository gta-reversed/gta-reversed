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
    AEAudioHardware.LoadSoundBank(52, 4);
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

    CAESound sound;
    sound.Initialise(
        4,
        0,
        this,
        posn,
        volume,
        2.0f,
        gfFireFrequencyVariations[m_snLastFireFrequencyIndex],
        1.0f,
        0,
        SOUND_REQUEST_UPDATES,
        0.0f,
        0
    );
    m_SoundLeft = AESoundManager.RequestNewSound(&sound);

    if (!AEAudioHardware.IsSoundBankLoaded(138, 19)) {
        AEAudioHardware.LoadSoundBank(138, 19);
        return;
    }

    sound.Initialise(
        19,
        26,
        this,
        posn,
        volume - 17.0f,
        2.0f,
        gfFireFrequencyVariations[m_snLastFireFrequencyIndex] * 0.6f,
        1.0f,
        0,
        SOUND_REQUEST_UPDATES,
        0.0f,
        0
    );
    sound.m_fMaxVolume = volume + 3.0f;
    sound.m_nEvent = AE_FRONTEND_SELECT;
    AESoundManager.RequestNewSound(&sound);
}

// 0x4DD270
void CAEFireAudioEntity::PlayWaterSounds(eAudioEvents audioId, CVector& posn) {
    CAESound sound;
    sound.Initialise(2, 3, this, posn, GetDefaultVolume(audioId), 2.0f, 0.75f, 0.6f, 0, SOUND_REQUEST_UPDATES, 0.0f, 0);
    sound.m_nEvent = AE_FRONTEND_HIGHLIGHT;
    sound.m_fSpeedVariability = 0.06f;
    m_SoundLeft = AESoundManager.RequestNewSound(&sound);

    sound.Initialise(0, 0, this, posn, GetDefaultVolume(audioId) + 20.0f, 2.0f, 1.78f, 0.6f, 0, SOUND_REQUEST_UPDATES, 0.0f, 0);
    sound.m_fSpeedVariability = 0.06f;
    sound.m_nEvent = AE_FRONTEND_ERROR;
    m_SoundRight = AESoundManager.RequestNewSound(&sound);
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

    switch (sound->m_nEvent) {
    case AE_FRONTEND_SELECT:
        if (sound->m_fVolume >= sound->m_fMaxVolume) {
            sound->m_nEvent = AE_FRONTEND_BACK;
        } else {
            sound->m_fVolume = std::min(sound->m_fVolume + 2.0f, sound->m_fMaxVolume);
        }
        break;
    case AE_FRONTEND_BACK:
        if (sound->m_fVolume <= -30.0f)
            sound->StopSoundAndForget();
        else
            sound->m_fVolume -= 0.75f;
        break;
    case AE_FRONTEND_ERROR:
        if (m_FxSystem && m_FxSystem->GetPlayStatus() == eFxSystemPlayStatus::FX_STOPPED) {
            sound->m_fVolume = -100.0f;
        }
        break;
    case AE_FRONTEND_HIGHLIGHT:
        if (m_FxSystem && m_FxSystem->GetPlayStatus() == eFxSystemPlayStatus::FX_STOPPED) {
            if (sound->m_fVolume > -100.0f) {
                sound->m_fVolume -= 1.0f;
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