#include "StdInc.h"

#include "AEDoorAudioEntity.h"

#include "AEAudioHardware.h"
#include "AESoundManager.h"

// 0x447030
CAEDoorAudioEntity::CAEDoorAudioEntity() : CAEAudioEntity() {
    m_nTime = 0;
    m_nMovingDoorTime = 0;
    m_nField84 = 0;
}

// 0x5B9A80
void CAEDoorAudioEntity::StaticInitialise() {
    AEAudioHardware.LoadSoundBank(51, 31);
}

// 0x4DC6B0
void CAEDoorAudioEntity::Reset() {
    m_nTime = 0;
    m_nMovingDoorTime = 0;
    AESoundManager.CancelSoundsOwnedByAudioEntity(this, true);
}

// 0x4DC9F0
void CAEDoorAudioEntity::AddAudioEvent(eAudioEvents event, CPhysical* physical) {
    if (event == AE_ENTRY_EXIT_DOOR_MOVING && !AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_ENTRY_EXIT_DOOR_MOVING, this) &&
        CTimer::GetTimeInMS() > m_nMovingDoorTime + 4000) {
        PlayDoorSound(1, AE_ENTRY_EXIT_DOOR_MOVING, physical->GetPosition());
        m_nMovingDoorTime = CTimer::GetTimeInMS();
    }
}

// 0x4DC860
void CAEDoorAudioEntity::AddAudioEvent(eAudioEvents event, CVector& posn, float volume, float speed) {
    switch (event) {
    case AE_GARAGE_DOOR_OPENING:
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(event, this)) {
            m_nTime = CTimer::GetTimeInMS();
            break;
        }

        AESoundManager.CancelSoundsOwnedByAudioEntity(this, true);
        if (!(CTimer::GetTimeInMS() <= m_nTime + 300))
            PlayDoorSound(2, event, posn, volume, 0.79f);

        PlayDoorSound(0, event, posn, volume, speed);
        m_nTime = CTimer::GetTimeInMS();
        break;
    case AE_GARAGE_DOOR_OPENED:
        if (!AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(event, this) && AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_GARAGE_DOOR_OPENING, this)) {
            AESoundManager.CancelSoundsOwnedByAudioEntity(this, true);
            PlayDoorSound(2, event, posn, volume, 1.0f);
        }

        m_nTime = CTimer::GetTimeInMS();
        break;
    case AE_GARAGE_DOOR_CLOSING:
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(event, this)) {
            m_nTime = CTimer::GetTimeInMS();
            break;
        }

        if (!(CTimer::GetTimeInMS() <= m_nTime + 300))
            PlayDoorSound(2, event, posn, volume, 1.0f);

        PlayDoorSound(0, event, posn, volume, speed);
        m_nTime = CTimer::GetTimeInMS();
        break;
    case AE_GARAGE_DOOR_CLOSED:
        if (!AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(event, this) && AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_GARAGE_DOOR_CLOSING, this)) {
            AESoundManager.CancelSoundsOwnedByAudioEntity(this, true);
            PlayDoorSound(2, event, posn, volume, 0.79f);
        }
        m_nTime = CTimer::GetTimeInMS();
        break;
    default:
        return;
    }
}

// 0x4DC6D0
void CAEDoorAudioEntity::PlayDoorSound(int16 sfxId, eAudioEvents event, CVector& posn, float volumeDelta, float speed) {
    if (AEAudioHardware.IsSoundBankLoaded(51, 31)) {
        CVector position;
        bool    enabled = false;
        if (posn.x == -1000.0f && posn.y == -1000.0f && posn.z == -1000.0f || posn.x == 0.0f && posn.y == 0.0f && posn.z == 0.0f) {
            position.Set(0.0f, 1.0f, 0.0f);
            enabled = true;
        } else {
            position = posn;
        }

        const float eventVolume = GetDefaultVolume(event);
        const float volume = eventVolume + volumeDelta;
        CAESound    sound;
        sound.Initialise(31, sfxId, this, position, volume, 2.0f, speed, 1.0f, 0, SOUND_REQUEST_UPDATES, 0.0f, 0);
        sound.SetIndividualEnvironment(SOUND_FRONT_END, enabled);
        sound.m_nEvent = event;
        AESoundManager.RequestNewSound(&sound);
    } else {
        StaticInitialise();
    }
}

// 0x4DCA60
void CAEDoorAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (curPlayPos <= 0)
        return;

#ifdef ANDROID_CODE
    if (sound->m_nEvent == AE_GARAGE_DOOR_OPENING) {
        if (m_nTime + 10000 >= CTimer::GetTimeInMS()) {
            auto playing = AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_GARAGE_DOOR_OPENED, this);
            if (playing)
                return sound->StopSoundAndForget();

            return;
        }
        AddAudioEvent(AE_GARAGE_DOOR_OPENED, sound->m_vecCurrPosn);
        return sound->StopSoundAndForget();
    }

    if (sound->m_nEvent != AE_GARAGE_DOOR_CLOSING)
        return;

    if (m_nTime + 10000 < CTimer::GetTimeInMS()) {
        AddAudioEvent(AE_GARAGE_DOOR_CLOSED, sound->m_vecCurrPosn, 0.0f, 1.0f);
        return sound->StopSoundAndForget();
    }

    auto playing = AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_GARAGE_DOOR_CLOSED, this);
    if (playing)
        return sound->StopSoundAndForget();
#else
    eAudioEvents event;
    if (sound->m_nEvent == AE_GARAGE_DOOR_OPENING) {
        event = AE_GARAGE_DOOR_OPENED;
        if (CTimer::GetTimeInMS() > (m_nTime + 10000)) {
            auto playing = (!AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_GARAGE_DOOR_OPENED, this) &&
                            AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_GARAGE_DOOR_OPENING, this));
            if (playing) {
                AESoundManager.CancelSoundsOwnedByAudioEntity(this, 1);
                PlayDoorSound(2, AE_GARAGE_DOOR_OPENED, sound->m_vecCurrPosn, 0.0f, 1.0f);
            }
            m_nTime = CTimer::GetTimeInMS();
            sound->StopSoundAndForget();
            return;
        }
    } else {
        if (sound->m_nEvent != AE_GARAGE_DOOR_CLOSING)
            return;

        event = AE_GARAGE_DOOR_CLOSED;
        if (CTimer::GetTimeInMS() > (m_nTime + 10000)) {
            auto playing = (!AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_GARAGE_DOOR_CLOSED, this) &&
                            AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_GARAGE_DOOR_CLOSING, this));
            if (playing) {
                AESoundManager.CancelSoundsOwnedByAudioEntity(this, 1);
                PlayDoorSound(2, AE_GARAGE_DOOR_CLOSED, sound->m_vecCurrPosn, 0.0f, 0.79f);
            }
            m_nTime = CTimer::GetTimeInMS();
            sound->StopSoundAndForget();
            return;
        }
    }

    if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(event, this))
        sound->StopSoundAndForget();
#endif
}

void CAEDoorAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEDoorAudioEntity, 0x85998C, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(StaticInitialise, 0x5B9A80);
    RH_ScopedInstall(Reset, 0x4DC6B0);
    RH_ScopedOverloadedInstall(AddAudioEvent, "1", 0x4DC9F0, void (CAEDoorAudioEntity::*)(eAudioEvents, CPhysical*));
    RH_ScopedOverloadedInstall(AddAudioEvent, "2", 0x4DC860, void (CAEDoorAudioEntity::*)(eAudioEvents, CVector&, float, float));
    RH_ScopedInstall(PlayDoorSound, 0x4DC6D0);
    RH_ScopedVMTInstall(UpdateParameters, 0x4DCA60);
}