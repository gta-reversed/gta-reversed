#include "StdInc.h"

#include "AEFrontendAudioEntity.h"

#include "AETwinLoopSoundEntity.h"
#include "AEAudioHardware.h"
#include "AESoundManager.h"
#include "AEAudioUtility.h"
#include "AESound.h"

// 0x5B9AB0
void CAEFrontendAudioEntity::Initialise() {
    m_bAmplifierWakeUp = false;
    m_pAmplifierWakeUp = nullptr;

    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_FRONTEND_MENU, SND_BANK_SLOT_FRONTEND_MENU);
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME);
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_BULLET_PASS_1, SND_BANK_SLOT_BULLET_PASS);

    m_nbLoadingTuneSeed = CAEAudioUtility::GetRandomNumberInRange(0, 3);
    AEAudioHardware.LoadSound(SND_BANK_GENRL_LOADING, 2 * m_nbLoadingTuneSeed + 0, SND_BANK_SLOT_COLLISIONS);
    AEAudioHardware.LoadSound(SND_BANK_GENRL_LOADING, 2 * m_nbLoadingTuneSeed + 1, SND_BANK_SLOT_WEAPON_GEN);
}

// 0x4DD440
void CAEFrontendAudioEntity::Reset() {
    m_nLastFrameGeneral_or_nFrameCount = 0;
    m_nLastFrameMissionComplete        = 0;
    m_nLastFrameBulletPass             = 0;
    m_BulletPassCount                 = 0;
    m_f7E                              = -1;
    AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(AE_FRONTEND_SCANNER_NOISE_START, this);
}

// 0x4DD4A0
void CAEFrontendAudioEntity::AddAudioEvent(eAudioEvents event, float fVolumeBoost, float fSpeed) {
    return plugin::CallMethod<0x4DD4A0, CAEFrontendAudioEntity*, int32, float, float>(this, event, fVolumeBoost, fSpeed);

    // untested
    CAESound sound;
    CVector XVECP = { +1.0f, 0.0f, 0.0f };
    CVector XVECM = { -1.0f, 0.0f, 0.0f };
    CVector YVECP = { +0.0f, 1.0f, 0.0f };

    const auto IncrementBulletHitsBank = [&]() {
        m_BulletPassBank = (eSoundBank)(m_BulletPassBank + 1);
        if (m_BulletPassBank > SND_BANK_GENRL_BULLET_PASS_LAST) {
            m_BulletPassBank = SND_BANK_GENRL_BULLET_PASS_FIRST;
        }
    };

    float volume = GetDefaultVolume(event) + fVolumeBoost;
    switch (event) {
    case AE_FRONTEND_START:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 25, this, XVECM, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);

            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 26, this, XVECP, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);
        }
        return;
    case AE_FRONTEND_SELECT:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_MENU, SND_BANK_SLOT_FRONTEND_MENU)) {
            sound.Initialise(SND_BANK_SLOT_FRONTEND_MENU, 6, this, XVECM, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);

            sound.Initialise(SND_BANK_SLOT_FRONTEND_MENU, 7, this, XVECP, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);
        }
        return;
    case AE_FRONTEND_BACK:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_MENU, SND_BANK_SLOT_FRONTEND_MENU)) {
            sound.Initialise(SND_BANK_SLOT_FRONTEND_MENU, 0, this, XVECM, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);

            sound.Initialise(SND_BANK_SLOT_FRONTEND_MENU, 1, this, XVECP, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);
        }
        return;
    case AE_FRONTEND_HIGHLIGHT:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_MENU, SND_BANK_SLOT_FRONTEND_MENU)) {
            sound.Initialise(SND_BANK_SLOT_FRONTEND_MENU, 4, this, XVECM, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);

            sound.Initialise(SND_BANK_SLOT_FRONTEND_MENU, 5, this, XVECP, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);
        }
        return;
    case AE_FRONTEND_ERROR:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_MENU, SND_BANK_SLOT_FRONTEND_MENU)) {
            sound.Initialise(SND_BANK_SLOT_FRONTEND_MENU, 2, this, XVECM, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);

            sound.Initialise(SND_BANK_SLOT_FRONTEND_MENU, 3, this, XVECP, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);
        }
        return;
    case AE_FRONTEND_NOISE_TEST:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_MENU, SND_BANK_SLOT_FRONTEND_MENU)) {
            sound.Initialise(SND_BANK_SLOT_FRONTEND_MENU, 8, this, YVECP, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;;
            AESoundManager.RequestNewSound(&sound);
        }
        return;
    case AE_FRONTEND_PICKUP_WEAPON:
    case AE_FRONTEND_CAR_FIT_BOMB_TIMED:
    case AE_FRONTEND_CAR_FIT_BOMB_BOOBY_TRAPPED:
    case AE_FRONTEND_CAR_FIT_BOMB_REMOTE_CONTROLLED:
    case AE_FRONTEND_PURCHASE_WEAPON:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME) && CTimer::GetFrameCounter() >= m_nLastFrameGeneral_or_nFrameCount + 5) {
            m_nLastFrameGeneral_or_nFrameCount = CTimer::GetFrameCounter();
            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 27, this, XVECM, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);

            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 28, this, XVECP, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);
        }
        return;
    case AE_FRONTEND_PICKUP_MONEY:
    case AE_FRONTEND_PICKUP_HEALTH:
    case AE_FRONTEND_PICKUP_ADRENALINE:
    case AE_FRONTEND_PICKUP_BODY_ARMOUR:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME) && CTimer::GetFrameCounter() >= m_nLastFrameGeneral_or_nFrameCount + 5) {
            m_nLastFrameGeneral_or_nFrameCount = CTimer::GetFrameCounter();
            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 16, this, XVECM, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);

            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 17, this, XVECP, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);
        }
        return;
    case AE_FRONTEND_PICKUP_INFO:
    case AE_FRONTEND_DISPLAY_INFO:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 14, this, XVECM, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);

            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 15, this, XVECP, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);
        }
        return;
    case AE_FRONTEND_PICKUP_DRUGS:
    case AE_FRONTEND_PICKUP_COLLECTABLE1:
    case AE_FRONTEND_PART_MISSION_COMPLETE:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME) && CTimer::GetFrameCounter() >= m_nLastFrameMissionComplete + 5) {
            m_nLastFrameMissionComplete = CTimer::GetFrameCounter();
            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 18, this, XVECM, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);

            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 19, this, XVECP, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);
        }
        return;
    case AE_FRONTEND_CAR_NO_CASH:
    case AE_FRONTEND_CAR_IS_HOT:
    case AE_FRONTEND_CAR_ALREADY_RIGGED:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME) && CTimer::GetFrameCounter() >= m_nLastFrameGeneral_or_nFrameCount + 5) {
            m_nLastFrameGeneral_or_nFrameCount = CTimer::GetFrameCounter();
            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 27, this, XVECM, volume, 1.0f, 0.841f);
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);

            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 28, this, XVECP, volume, 1.0f, 0.841f);
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);
        }
        return;
    case AE_FRONTEND_CAR_RESPRAY:
    {
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN))
            return;

        float speed0, speed1;
        if (CAEAudioUtility::ResolveProbability(0.5f)) {
            speed0 = 1.1892101f;
            speed1 = 1.0f;
        } else {
            speed0 = 1.0f;
            speed1 = 1.1892101f;
        }
        sound.Initialise(SND_BANK_SLOT_WEAPON_GEN, 28, this, XVECM, volume, 1.0f, speed0);
        sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_UNDUCKABLE | SOUND_PLAY_PHYSICALLY | SOUND_REQUEST_UPDATES | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
        sound.m_Event = AE_FRONTEND_CAR_RESPRAY;
        AESoundManager.RequestNewSound(&sound);

        sound.Initialise(SND_BANK_SLOT_WEAPON_GEN, 28, this, XVECP, volume, 1.0f, speed1);
        sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_UNDUCKABLE | SOUND_PLAY_PHYSICALLY | SOUND_REQUEST_UPDATES | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
        sound.m_Event = AE_FRONTEND_CAR_RESPRAY;
        AESoundManager.RequestNewSound(&sound);
        m_nLastTimeCarRespray = CTimer::GetTimeInMS();
        return;
    }
    case AE_FRONTEND_BULLET_PASS_LEFT_REAR:
        if (!AEAudioHardware.IsSoundBankLoaded(m_BulletPassBank, SND_BANK_SLOT_BULLET_PASS) || CTimer::GetFrameCounter() < m_nLastFrameBulletPass + 5)
            return;

        // inlined
        if (m_BulletPassCount > 10) {
            if (!AESoundManager.AreSoundsPlayingInBankSlot(SND_BANK_SLOT_BULLET_PASS)) {
                IncrementBulletHitsBank();
                AEAudioHardware.LoadSoundBank(m_BulletPassBank, SND_BANK_SLOT_BULLET_PASS);
                m_BulletPassCount = 0;
            }
            return;
        }

        m_nLastFrameBulletPass = CTimer::GetFrameCounter();
        sound.Initialise(SND_BANK_SLOT_BULLET_PASS, CAEAudioUtility::GetRandomNumberInRange(0, 2), this, { -0.1f, -1.0f, 0.0f }, volume);
        sound.m_Event = AE_FRONTEND_BULLET_PASS_LEFT_REAR;
        sound.m_SpeedVariance = 0.03125f;
        sound.m_nEnvironmentFlags = SOUND_REQUEST_UPDATES | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
        AESoundManager.RequestNewSound(&sound);
        ++m_BulletPassCount;
        return;
    case AE_FRONTEND_BULLET_PASS_LEFT_FRONT:
        if (!AEAudioHardware.IsSoundBankLoaded(m_BulletPassBank, SND_BANK_SLOT_BULLET_PASS) || CTimer::GetFrameCounter() < m_nLastFrameBulletPass + 5)
            return;

        // inlined
        if (m_BulletPassCount > 10) {
            if (!AESoundManager.AreSoundsPlayingInBankSlot(SND_BANK_SLOT_BULLET_PASS)) {
                IncrementBulletHitsBank();
                AEAudioHardware.LoadSoundBank(m_BulletPassBank, SND_BANK_SLOT_BULLET_PASS);
                m_BulletPassCount = 0;
            }
            return;
        }

        m_nLastFrameBulletPass = CTimer::GetFrameCounter();

        sound.Initialise(SND_BANK_SLOT_BULLET_PASS, CAEAudioUtility::GetRandomNumberInRange(0, 2), this, { -0.1f, +1.0f, 0.0f }, volume);
        sound.m_Event = AE_FRONTEND_BULLET_PASS_LEFT_FRONT;
        sound.m_SpeedVariance = 0.03125f;
        sound.m_nEnvironmentFlags = SOUND_REQUEST_UPDATES | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
        AESoundManager.RequestNewSound(&sound);
        ++m_BulletPassCount;
        return;
    case AE_FRONTEND_BULLET_PASS_RIGHT_REAR:
        if (!AEAudioHardware.IsSoundBankLoaded(m_BulletPassBank, SND_BANK_SLOT_BULLET_PASS) || CTimer::GetFrameCounter() < m_nLastFrameBulletPass + 5)
            return;

        if (m_BulletPassCount <= 10) {
            m_nLastFrameBulletPass = CTimer::GetFrameCounter();
            sound.Initialise(SND_BANK_SLOT_BULLET_PASS, CAEAudioUtility::GetRandomNumberInRange(0, 2), this, { 0.1f, -1.0f, 0.0f }, volume);
            sound.m_Event = AE_FRONTEND_BULLET_PASS_RIGHT_REAR;
            sound.m_SpeedVariance = 0.03125f;
            sound.m_nEnvironmentFlags = SOUND_REQUEST_UPDATES | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);
            ++m_BulletPassCount;
            return;
        }

        if (!AESoundManager.AreSoundsPlayingInBankSlot(SND_BANK_SLOT_BULLET_PASS)) {
            // inlined
            IncrementBulletHitsBank();
            AEAudioHardware.LoadSoundBank(m_BulletPassBank, SND_BANK_SLOT_BULLET_PASS);
            m_BulletPassCount = 0;
        }
        return;
    case AE_FRONTEND_BULLET_PASS_RIGHT_FRONT:
        if (AEAudioHardware.IsSoundBankLoaded(m_BulletPassBank, SND_BANK_SLOT_BULLET_PASS) && CTimer::GetFrameCounter() >= m_nLastFrameBulletPass + 5) {
            if (m_BulletPassCount <= 10) {
                m_nLastFrameBulletPass = CTimer::GetFrameCounter();
                sound.Initialise(SND_BANK_SLOT_BULLET_PASS, CAEAudioUtility::GetRandomNumberInRange(0, 2), this, { 0.1f, +1.0f, 0.0f }, volume);
                sound.m_Event = AE_FRONTEND_BULLET_PASS_RIGHT_FRONT;
                sound.m_SpeedVariance = 0.03125f;
                sound.m_nEnvironmentFlags = SOUND_REQUEST_UPDATES | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
                AESoundManager.RequestNewSound(&sound);
                ++m_BulletPassCount;
            } else {
                if (!AESoundManager.AreSoundsPlayingInBankSlot(SND_BANK_SLOT_BULLET_PASS)) {
                    // inlined
                    IncrementBulletHitsBank();
                    AEAudioHardware.LoadSoundBank(m_BulletPassBank, SND_BANK_SLOT_BULLET_PASS);
                    m_BulletPassCount = 0;
                }
            }
        }
        return;
    case AE_FRONTEND_WAKEUP_AMPLIFIER:
        if (!m_bAmplifierWakeUp && AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 8, this, YVECP, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_REQUEST_UPDATES | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            m_pAmplifierWakeUp = AESoundManager.RequestNewSound(&sound);
            m_bAmplifierWakeUp = m_pAmplifierWakeUp != nullptr;
        }
        return;
    case AE_FRONTEND_TIMER_COUNT:
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_FRONTEND_TIMER_COUNT, this)) {
            m_nLatestTimerCount = CTimer::GetTimeInMS();
            return;
        }

        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 4, this, XVECM, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_UNDUCKABLE | SOUND_PLAY_PHYSICALLY | SOUND_REQUEST_UPDATES | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            sound.m_Event = AE_FRONTEND_TIMER_COUNT;
            AESoundManager.RequestNewSound(&sound);

            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 5, this, XVECP, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_UNDUCKABLE | SOUND_PLAY_PHYSICALLY | SOUND_REQUEST_UPDATES | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            sound.m_Event = AE_FRONTEND_TIMER_COUNT;
            AESoundManager.RequestNewSound(&sound);
            m_nLatestTimerCount = CTimer::GetTimeInMS();
        }
        return;
    case AE_FRONTEND_RADIO_RETUNE_START:
    {
        auto flags0 = eSoundEnvironment(SOUND_UNDUCKABLE | SOUND_MUSIC_MASTERED | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END);
        auto flags1 = eSoundEnvironment(flags0 | SOUND_UNPAUSABLE);
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
            if (CTimer::GetIsPaused()) {
                if (!m_objRetunePaused.IsInUse()) {
                    m_objRetunePaused.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 2, 1, this, 200, 650, -1, -1);
                    m_objRetunePaused.PlayTwinLoopSound(YVECP, volume, 1.0f, 1.0f, 1.0f, flags1);
                }
            } else if (!m_objRetune.IsInUse()) {
                m_objRetune.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 2, 1, this, 200, 650, -1, -1);
                m_objRetune.PlayTwinLoopSound(YVECP, volume, 1.0f, 1.0f, 1.0f, flags0);
            }
        }
        return;
    }
    case AE_FRONTEND_RADIO_RETUNE_STOP:
        if (CTimer::GetIsPaused()) {
            if (m_objRetunePaused.IsInUse()) {
                m_objRetunePaused.StopSoundAndForget();
            }
        } else if (m_objRetune.IsInUse()) {
            m_objRetune.StopSoundAndForget();
        }
        return;
    case AE_FRONTEND_RADIO_RETUNE_STOP_PAUSED:
        if (m_objRetunePaused.IsInUse()) {
            m_objRetunePaused.StopSoundAndForget();
        }
        return;
    case AE_FRONTEND_RADIO_CLICK_ON:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 23, this, YVECP, volume);
            sound.m_nEnvironmentFlags = SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;;
            AESoundManager.RequestNewSound(&sound);
        }
        return;
    case AE_FRONTEND_RADIO_CLICK_OFF:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 23, this, YVECP, volume, 1.0f, 0.8909f);
            sound.m_nEnvironmentFlags = SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;;
            AESoundManager.RequestNewSound(&sound);
        }
        return;
    case AE_FRONTEND_FIRE_FAIL_SNIPERRIFFLE:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 10, this, YVECP, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FRONT_END | SOUND_UNCANCELLABLE;
            AESoundManager.RequestNewSound(&sound);
        }
        return;
    case AE_FRONTEND_FIRE_FAIL_ROCKET:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 11, this, YVECP, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FRONT_END | SOUND_UNCANCELLABLE;
            AESoundManager.RequestNewSound(&sound);
        }
        return;
    case AE_FRONTEND_RACE_321:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 6, this, XVECM, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);

            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 7, this, XVECP, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);
        }
        return;
    case AE_FRONTEND_RACE_GO:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 12, this, XVECM, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);

            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 13, this, XVECP, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            AESoundManager.RequestNewSound(&sound);
        }
        return;
    case AE_FRONTEND_BUY_CAR_MOD:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 9, this, YVECP, volume);
            sound.m_nEnvironmentFlags = SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;;
            sound.m_SpeedVariance = 0.0588f;
            AESoundManager.RequestNewSound(&sound);
        }
        return;
    case AE_FRONTEND_SCANNER_NOISE_START:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME) && !AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_FRONTEND_SCANNER_NOISE_START, this)) {
            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 3, this, YVECP, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_UNDUCKABLE | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            sound.m_Event = AE_FRONTEND_SCANNER_NOISE_START;
            AESoundManager.RequestNewSound(&sound);
        }
        return;
    case AE_FRONTEND_SCANNER_NOISE_STOP:
        AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(47, this);
        return;
    case AE_FRONTEND_SCANNER_CLICK:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME) && !AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_FRONTEND_SCANNER_CLICK, this)) {
            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 24, this, YVECP, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_UNDUCKABLE | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            sound.m_Event = AE_FRONTEND_SCANNER_CLICK;
            AESoundManager.RequestNewSound(&sound);
        }
        return;
    case AE_FRONTEND_LOADING_TUNE_START:
        if (   AEAudioHardware.IsSoundLoaded(SND_BANK_GENRL_LOADING, 2 * m_nbLoadingTuneSeed, SND_BANK_SLOT_COLLISIONS)
            && AEAudioHardware.IsSoundLoaded(SND_BANK_GENRL_LOADING, 2 * m_nbLoadingTuneSeed + 1, SND_BANK_SLOT_WEAPON_GEN)
            && !AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_FRONTEND_LOADING_TUNE_START, this)
        ) {
            sound.Initialise(SND_BANK_SLOT_WEAPON_GEN, 2 * m_nbLoadingTuneSeed, this, XVECM, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            sound.m_Event = AE_FRONTEND_LOADING_TUNE_START;
            AESoundManager.RequestNewSound(&sound);

            sound.Initialise(SND_BANK_SLOT_WEAPON_GEN, 2 * m_nbLoadingTuneSeed + 1, this, XVECP, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_FORCED_FRONT | SOUND_UNDUCKABLE | SOUND_UNPAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_UNCANCELLABLE | SOUND_FRONT_END;
            sound.m_Event = AE_FRONTEND_LOADING_TUNE_START;
            AESoundManager.RequestNewSound(&sound);
        }
        return;
    case AE_FRONTEND_LOADING_TUNE_STOP:
        AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(50, this);
        return;
    case AE_MISSILE_LOCK:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME) && !AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_MISSILE_LOCK, this)) {
            sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 13, this, YVECP, volume);
            sound.m_Speed = fSpeed;
            sound.m_nEnvironmentFlags = SOUND_UNDUCKABLE | SOUND_FRONT_END;
            sound.m_Event = AE_MISSILE_LOCK;
            AESoundManager.RequestNewSound(&sound);
        }
        return;
    default:
        return;
    }
}

// 0x4DD480
bool CAEFrontendAudioEntity::IsRadioTuneSoundActive() {
    return CTimer::GetIsPaused()
        ? m_objRetunePaused.IsInUse()
        : m_objRetune.IsInUse();
}

// 0x4DD470
bool CAEFrontendAudioEntity::IsLoadingTuneActive() {
    return AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_FRONTEND_LOADING_TUNE_START, this);
}

// 0x4DEDA0
void CAEFrontendAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    return plugin::CallMethod<0x4DEDA0, CAEFrontendAudioEntity*, CAESound*, int16>(this, sound, curPlayPos);

    // untested
    if (!sound)
        return;

    const float curPos = (float)curPlayPos / 350.f + (float)curPlayPos / 350.f;
    const float rear = curPos - 1.0f;
    const float front = 1.0f - curPos;

    const auto BulletSound = [=](const float x, const float y) {
        if (curPlayPos >= 0 && curPlayPos <= 350) {
            sound->SetPosition({ x, y, 0.0f });
        }
    };

    switch (sound->m_Event) {
    case AE_FRONTEND_CAR_RESPRAY:
        if (curPlayPos > 0 && CTimer::GetTimeInMS() > m_nLastTimeCarRespray + 1900) {
            sound->StopSoundAndForget();
        }
        break;
    case AE_FRONTEND_BULLET_PASS_LEFT_REAR:
        BulletSound(-0.1f, rear);
        break;
    case AE_FRONTEND_BULLET_PASS_LEFT_FRONT:
        BulletSound(-0.1f, front);
        break;
    case AE_FRONTEND_BULLET_PASS_RIGHT_REAR:
        BulletSound(+0.1f, rear);
        break;
    case AE_FRONTEND_BULLET_PASS_RIGHT_FRONT:
        BulletSound(+0.1f, front);
        break;
    case AE_FRONTEND_TIMER_COUNT:
        if (curPlayPos > 0 && CTimer::GetTimeInMS() > m_nLatestTimerCount + 100) {
            sound->StopSoundAndForget();
        }
        break;
    default:
        break;
    }

    if (sound == m_pAmplifierWakeUp && curPlayPos == -1) {
        m_pAmplifierWakeUp = nullptr;
        m_bAmplifierWakeUp = false;
    }
}

void CAEFrontendAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEFrontendAudioEntity, 0x862E54, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Initialise, 0x5B9AB0);
    RH_ScopedInstall(Reset, 0x4DD440);
    RH_ScopedInstall(AddAudioEvent, 0x4DD4A0, { .reversed = false });
    RH_ScopedInstall(IsRadioTuneSoundActive, 0x4DD480);
    RH_ScopedInstall(IsLoadingTuneActive, 0x4DD470);
    RH_ScopedVMTInstall(UpdateParameters, 0x4DEDA0, { .reversed = false });
}
