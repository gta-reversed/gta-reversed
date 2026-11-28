#include "StdInc.h"

#include "AEFrontendAudioEntity.h"

#include "AETwinLoopSoundEntity.h"
#include "AEAudioHardware.h"
#include "AESoundManager.h"
#include "AEAudioUtility.h"
#include "AESound.h"

// 0x5B9AB0
void CAEFrontendAudioEntity::Initialise() {
    m_LastFrameGeneral         = 0;
    m_LastFrameMissionComplete = 0;
    m_LastFrameBulletPass      = 0;
    m_BulletPassCount          = 0;
    m_BulletPassBank           = SND_BANK_GENRL_BULLET_PASS_1;
    m_Unused7E                 = -1;
    m_LastTimeCarRespray       = 0;
    m_LastTimeTimerCount       = 0;
    m_IsAmplifierWakeUpPlaying = false;
    m_AmplifierWakeUpSound     = nullptr;

    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_FRONTEND_MENU, SND_BANK_SLOT_FRONTEND_MENU);
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME);
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_BULLET_PASS_1, SND_BANK_SLOT_BULLET_PASS);

    m_LoadingTuneSeed = CAEAudioUtility::GetRandomNumberInRange(0, 3);
    AEAudioHardware.LoadSound(SND_BANK_GENRL_LOADING, 2 * m_LoadingTuneSeed + 0, SND_BANK_SLOT_COLLISIONS);
    AEAudioHardware.LoadSound(SND_BANK_GENRL_LOADING, 2 * m_LoadingTuneSeed + 1, SND_BANK_SLOT_WEAPON_GEN);
}

// 0x4DD440
void CAEFrontendAudioEntity::Reset() {
    m_LastFrameGeneral         = 0;
    m_LastFrameMissionComplete = 0;
    m_LastFrameBulletPass      = 0;
    m_BulletPassCount          = 0;
    m_Unused7E                 = -1;
    AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(AE_FRONTEND_SCANNER_NOISE_START, this);
}

// 0x4DD4A0
void CAEFrontendAudioEntity::AddAudioEvent(eAudioEvents event, float volumeBoost, float speed) {
    const CVector left{ -1.0f, 0.0f, 0.0f };
    const CVector right{ +1.0f, 0.0f, 0.0f };
    const CVector front{ 0.0f, 1.0f, 0.0f };

    const float volume   = GetDefaultVolume(event) + volumeBoost;

    const auto PlaySound = [&](eSoundBankSlot slot, eSoundID sfx, const CVector& pos, float soundSpeed, uint32 flags, eAudioEvents eventId = AE_UNDEFINED, float frequencyVariance = 0.0f) {
        return AESoundManager.PlaySound({
            .BankSlotID        = slot,
            .SoundID           = sfx,
            .AudioEntity       = this,
            .Pos               = pos,
            .Volume            = volume,
            .Speed             = soundSpeed,
            .Flags             = flags,
            .FrequencyVariance = frequencyVariance,
            .EventID           = eventId,
        });
    };

    // The sound `sfx` on the left, `sfx + 1` on the right
    const auto PlayStereo = [&](eSoundBankSlot slot, eSoundID sfx, float soundSpeed, uint32 flags, eAudioEvents eventId = AE_UNDEFINED) {
        PlaySound(slot, sfx, left, soundSpeed, flags, eventId);
        PlaySound(slot, (eSoundID)(sfx + 1), right, soundSpeed, flags, eventId);
    };

    const auto IsFrontendGameBankLoaded = [] {
        return AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME);
    };
    const auto IsFrontendMenuBankLoaded = [] {
        return AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_MENU, SND_BANK_SLOT_FRONTEND_MENU);
    };

    // At most one sound every 5 frames
    const auto CheckFrameDelay = [](uint32& lastFrame) {
        if (CTimer::GetFrameCounter() < lastFrame + 5) {
            return false;
        }
        lastFrame = CTimer::GetFrameCounter();
        return true;
    };

    const auto PlayBulletPass = [&](const CVector& pos) {
        if (!AEAudioHardware.IsSoundBankLoaded(m_BulletPassBank, SND_BANK_SLOT_BULLET_PASS) || CTimer::GetFrameCounter() < m_LastFrameBulletPass + 5) {
            return;
        }
        if (m_BulletPassCount > 10) { // Switch to the next bank once the sounds of this one are done
            if (!AESoundManager.AreSoundsPlayingInBankSlot(SND_BANK_SLOT_BULLET_PASS)) {
                m_BulletPassBank = (eSoundBank)(m_BulletPassBank + 1);
                if (m_BulletPassBank > SND_BANK_GENRL_BULLET_PASS_LAST) {
                    m_BulletPassBank = SND_BANK_GENRL_BULLET_PASS_FIRST;
                }
                AEAudioHardware.LoadSoundBank(m_BulletPassBank, SND_BANK_SLOT_BULLET_PASS);
                m_BulletPassCount = 0;
            }
            return;
        }
        m_LastFrameBulletPass = CTimer::GetFrameCounter();
        PlaySound(
            SND_BANK_SLOT_BULLET_PASS,
            (eSoundID)(CAEAudioUtility::GetRandomNumberInRange(0, 2)),
            pos,
            1.0f,
            SOUND_REQUEST_UPDATES | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
            event,
            0.03125f
        );
        m_BulletPassCount++;
    };

    const auto PlayRetune = [&](CAETwinLoopSoundEntity& retune, uint32 flags) {
        if (retune.IsActive()) {
            return;
        }
        retune.Initialise(SND_BANK_SLOT_FRONTEND_GAME, 2, 1, this, 200, 650);
        retune.PlayTwinLoopSound(front, volume, 1.0f, 1.0f, 1.0f, (eSoundEnvironment)(flags));
    };
    const auto StopRetune = [](CAETwinLoopSoundEntity& retune) {
        if (retune.IsActive()) {
            retune.StopSoundAndForget();
        }
    };

    constexpr uint32 FLAGS_ROLLED_OFF_FRONT = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END;
    constexpr uint32 FLAGS_FORCED_FRONT     = SOUND_FORCED_FRONT | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END;
    constexpr uint32 FLAGS_FRONT            = SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END;
    constexpr uint32 FLAGS_UPDATED_FRONT    = SOUND_FORCED_FRONT | SOUND_IS_DUCKABLE | SOUND_PLAY_PHYSICALLY | SOUND_REQUEST_UPDATES | SOUND_IS_CANCELLABLE | SOUND_FRONT_END;

    switch (event) {
    case AE_FRONTEND_START: // 0x4DD5DB
        if (IsFrontendGameBankLoaded()) {
            PlayStereo(SND_BANK_SLOT_FRONTEND_GAME, 25, speed, FLAGS_ROLLED_OFF_FRONT);
        }
        return;
    case AE_FRONTEND_SELECT: // 0x4DD712
        if (IsFrontendMenuBankLoaded()) {
            PlayStereo(SND_BANK_SLOT_FRONTEND_MENU, 6, speed, FLAGS_ROLLED_OFF_FRONT);
        }
        return;
    case AE_FRONTEND_BACK: // 0x4DD7F0
        if (IsFrontendMenuBankLoaded()) {
            PlayStereo(SND_BANK_SLOT_FRONTEND_MENU, 0, speed, FLAGS_ROLLED_OFF_FRONT);
        }
        return;
    case AE_FRONTEND_HIGHLIGHT: // 0x4DD88E
        if (IsFrontendMenuBankLoaded()) {
            PlayStereo(SND_BANK_SLOT_FRONTEND_MENU, 4, speed, FLAGS_FORCED_FRONT);
        }
        return;
    case AE_FRONTEND_ERROR: // 0x4DD9EA
        if (IsFrontendMenuBankLoaded()) {
            PlayStereo(SND_BANK_SLOT_FRONTEND_MENU, 2, speed, FLAGS_ROLLED_OFF_FRONT);
        }
        return;
    case AE_FRONTEND_NOISE_TEST: // 0x4DD96C
        if (IsFrontendMenuBankLoaded()) {
            PlaySound(SND_BANK_SLOT_FRONTEND_MENU, 8, front, speed, FLAGS_FRONT);
        }
        return;
    case AE_FRONTEND_PICKUP_WEAPON: // 0x4DDA89
    case AE_FRONTEND_CAR_FIT_BOMB_TIMED:
    case AE_FRONTEND_CAR_FIT_BOMB_BOOBY_TRAPPED:
    case AE_FRONTEND_CAR_FIT_BOMB_REMOTE_CONTROLLED:
    case AE_FRONTEND_PURCHASE_WEAPON:
        if (IsFrontendGameBankLoaded() && CheckFrameDelay(m_LastFrameGeneral)) {
            PlayStereo(SND_BANK_SLOT_FRONTEND_GAME, 27, speed, FLAGS_FORCED_FRONT);
        }
        return;
    case AE_FRONTEND_PICKUP_MONEY: // 0x4DDB40
    case AE_FRONTEND_PICKUP_HEALTH:
    case AE_FRONTEND_PICKUP_ADRENALINE:
    case AE_FRONTEND_PICKUP_BODY_ARMOUR:
        if (IsFrontendGameBankLoaded() && CheckFrameDelay(m_LastFrameGeneral)) {
            PlayStereo(SND_BANK_SLOT_FRONTEND_GAME, 16, speed, FLAGS_FORCED_FRONT);
        }
        return;
    case AE_FRONTEND_PICKUP_INFO: // 0x4DD676
    case AE_FRONTEND_DISPLAY_INFO:
        if (IsFrontendGameBankLoaded()) {
            PlayStereo(SND_BANK_SLOT_FRONTEND_GAME, 14, speed, FLAGS_ROLLED_OFF_FRONT);
        }
        return;
    case AE_FRONTEND_PICKUP_DRUGS: // 0x4DE6A6
    case AE_FRONTEND_PICKUP_COLLECTABLE1:
    case AE_FRONTEND_PART_MISSION_COMPLETE:
        if (IsFrontendGameBankLoaded() && CheckFrameDelay(m_LastFrameMissionComplete)) {
            PlayStereo(SND_BANK_SLOT_FRONTEND_GAME, 18, speed, FLAGS_ROLLED_OFF_FRONT);
        }
        return;
    case AE_FRONTEND_CAR_NO_CASH: // 0x4DDD27
    case AE_FRONTEND_CAR_IS_HOT:
    case AE_FRONTEND_CAR_ALREADY_RIGGED:
        if (IsFrontendGameBankLoaded() && CheckFrameDelay(m_LastFrameGeneral)) {
            PlayStereo(SND_BANK_SLOT_FRONTEND_GAME, 27, 0.841f, FLAGS_FORCED_FRONT);
        }
        return;
    case AE_FRONTEND_CAR_RESPRAY: { // 0x4DDBF7
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN)) {
            return;
        }
        const bool isLeftHigher = CAEAudioUtility::ResolveProbability(0.5f);
        PlaySound(SND_BANK_SLOT_WEAPON_GEN, SND_GENRL_WEAPONS_SPRAY_PAINT, left, isLeftHigher ? 1.1892101f : 1.0f, FLAGS_UPDATED_FRONT, event);
        PlaySound(SND_BANK_SLOT_WEAPON_GEN, SND_GENRL_WEAPONS_SPRAY_PAINT, right, isLeftHigher ? 1.0f : 1.1892101f, FLAGS_UPDATED_FRONT, event);
        m_LastTimeCarRespray = CTimer::GetTimeInMS();
        return;
    }
    case AE_FRONTEND_BUY_CAR_MOD: // 0x4DDE08
        if (IsFrontendGameBankLoaded()) {
            PlaySound(SND_BANK_SLOT_FRONTEND_GAME, 9, front, 1.0f, FLAGS_FRONT, AE_UNDEFINED, 0.0588f);
        }
        return;
    case AE_FRONTEND_BULLET_PASS_LEFT_REAR: // 0x4DDE81
        PlayBulletPass({ -0.1f, -1.0f, 0.0f });
        return;
    case AE_FRONTEND_BULLET_PASS_LEFT_FRONT: // 0x4DDF69
        PlayBulletPass({ -0.1f, +1.0f, 0.0f });
        return;
    case AE_FRONTEND_BULLET_PASS_RIGHT_REAR: // 0x4DE010
        PlayBulletPass({ +0.1f, -1.0f, 0.0f });
        return;
    case AE_FRONTEND_BULLET_PASS_RIGHT_FRONT: // 0x4DE0BC
        PlayBulletPass({ +0.1f, +1.0f, 0.0f });
        return;
    case AE_FRONTEND_WAKEUP_AMPLIFIER: // 0x4DE1D8
        if (!m_IsAmplifierWakeUpPlaying && IsFrontendGameBankLoaded()) {
            m_AmplifierWakeUpSound     = PlaySound(SND_BANK_SLOT_FRONTEND_GAME, 8, front, speed, FLAGS_FRONT | SOUND_REQUEST_UPDATES);
            m_IsAmplifierWakeUpPlaying = m_AmplifierWakeUpSound != nullptr;
        }
        return;
    case AE_FRONTEND_TIMER_COUNT: // 0x4DE26D
        if (!AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_FRONTEND_TIMER_COUNT, this)) {
            if (!IsFrontendGameBankLoaded()) {
                return;
            }
            PlayStereo(SND_BANK_SLOT_FRONTEND_GAME, 4, speed, FLAGS_UPDATED_FRONT, event);
        }
        m_LastTimeTimerCount = CTimer::GetTimeInMS();
        return;
    case AE_FRONTEND_RADIO_RETUNE_START: // 0x4DE381
        if (IsFrontendGameBankLoaded()) {
            if (CTimer::GetIsPaused()) {
                PlayRetune(m_RetunePausedSound, SOUND_IS_DUCKABLE | SOUND_MUSIC_MASTERED | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END);
            } else {
                PlayRetune(m_RetuneSound, SOUND_IS_DUCKABLE | SOUND_MUSIC_MASTERED | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END);
            }
        }
        return;
    case AE_FRONTEND_RADIO_RETUNE_STOP: // 0x4DE480
        StopRetune(CTimer::GetIsPaused() ? m_RetunePausedSound : m_RetuneSound);
        return;
    case AE_FRONTEND_RADIO_RETUNE_STOP_PAUSED: // 0x4DE4AF
        StopRetune(m_RetunePausedSound);
        return;
    case AE_FRONTEND_RADIO_CLICK_ON: // 0x4DE4CC
        if (IsFrontendGameBankLoaded()) {
            PlaySound(SND_BANK_SLOT_FRONTEND_GAME, 23, front, 1.0f, FLAGS_FRONT);
        }
        return;
    case AE_FRONTEND_RADIO_CLICK_OFF: // 0x4DE53D
        if (IsFrontendGameBankLoaded()) {
            PlaySound(SND_BANK_SLOT_FRONTEND_GAME, 23, front, 0.8909f, FLAGS_FRONT);
        }
        return;
    case AE_FRONTEND_FIRE_FAIL_SNIPERRIFFLE: // 0x4DE5AE
        if (IsFrontendGameBankLoaded()) {
            PlaySound(SND_BANK_SLOT_FRONTEND_GAME, 10, front, speed, SOUND_IS_CANCELLABLE | SOUND_FRONT_END);
        }
        return;
    case AE_FRONTEND_FIRE_FAIL_ROCKET: // 0x4DE62A
        if (IsFrontendGameBankLoaded()) {
            PlaySound(SND_BANK_SLOT_FRONTEND_GAME, 11, front, speed, SOUND_IS_CANCELLABLE | SOUND_FRONT_END);
        }
        return;
    case AE_FRONTEND_RACE_321: // 0x4DE79D
        if (IsFrontendGameBankLoaded()) {
            PlayStereo(SND_BANK_SLOT_FRONTEND_GAME, 6, speed, FLAGS_ROLLED_OFF_FRONT);
        }
        return;
    case AE_FRONTEND_RACE_GO: // 0x4DE878
        if (IsFrontendGameBankLoaded()) {
            PlayStereo(SND_BANK_SLOT_FRONTEND_GAME, 12, speed, FLAGS_ROLLED_OFF_FRONT);
        }
        return;
    case AE_FRONTEND_SCANNER_NOISE_START: // 0x4DE9ED
    case AE_FRONTEND_SCANNER_CLICK:       // 0x4DEA8F
        if (IsFrontendGameBankLoaded() && !AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(event, this)) {
            PlaySound(SND_BANK_SLOT_FRONTEND_GAME, event == AE_FRONTEND_SCANNER_CLICK ? 24 : 3, front, speed, SOUND_IS_DUCKABLE | SOUND_IS_CANCELLABLE | SOUND_FRONT_END, event);
        }
        return;
    case AE_FRONTEND_SCANNER_NOISE_STOP: // 0x4DEA87
        AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(AE_FRONTEND_SCANNER_NOISE_START, this);
        return;
    case AE_FRONTEND_LOADING_TUNE_START: { // 0x4DEB29
        const auto sfxLeft  = (eSoundID)(2 * m_LoadingTuneSeed);
        const auto sfxRight = (eSoundID)(2 * m_LoadingTuneSeed + 1);
        if (!AEAudioHardware.IsSoundLoaded(SND_BANK_GENRL_LOADING, sfxLeft, SND_BANK_SLOT_COLLISIONS) || !AEAudioHardware.IsSoundLoaded(SND_BANK_GENRL_LOADING, sfxRight, SND_BANK_SLOT_WEAPON_GEN)) {
            return;
        }
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_FRONTEND_LOADING_TUNE_START, this)) {
            return;
        }
        PlaySound(SND_BANK_SLOT_COLLISIONS, sfxLeft, left, speed, FLAGS_FORCED_FRONT, event);
        PlaySound(SND_BANK_SLOT_WEAPON_GEN, sfxRight, right, speed, FLAGS_FORCED_FRONT, event);
        return;
    }
    case AE_FRONTEND_LOADING_TUNE_STOP: // 0x4DEC64
        AESoundManager.CancelSoundsOfThisEventPlayingForThisEntity(AE_FRONTEND_LOADING_TUNE_START, this);
        return;
    case AE_MISSILE_LOCK: // 0x4DE953
        if (IsFrontendGameBankLoaded() && !AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_MISSILE_LOCK, this)) {
            PlaySound(SND_BANK_SLOT_FRONTEND_GAME, 13, front, speed, SOUND_IS_DUCKABLE | SOUND_FRONT_END, event);
        }
        return;
    case AE_FRONTEND_FRENZY_STARTED: // 0x4DEC71
    case AE_FRONTEND_FRENZY_ONGOING:
    case AE_FRONTEND_FRENZY_PASSED:
    case AE_FRONTEND_FRENZY_FAILED:
    case AE_FRONTEND_FRENZY_REGISTER_KILL_PED:
    case AE_FRONTEND_FRENZY_REGISTER_KILL_CAR:
    case AE_FRONTEND_PAGER:
    default: // Not frontend events
        return;
    }
}

// 0x4DD480
bool CAEFrontendAudioEntity::IsRadioTuneSoundActive() {
    return CTimer::GetIsPaused()
        ? m_RetunePausedSound.IsActive()
        : m_RetuneSound.IsActive();
}

// 0x4DD470
bool CAEFrontendAudioEntity::IsLoadingTuneActive() {
    return AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_FRONTEND_LOADING_TUNE_START, this);
}

// 0x4DEDA0
void CAEFrontendAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (!sound) {
        return;
    }

    const float curPos = (float)curPlayPos / 350.f + (float)curPlayPos / 350.f;
    const float rear = curPos - 1.0f;
    const float front = 1.0f - curPos;

    // The original moves the sound along Z, Y stays 0
    const auto BulletSound = [=](const float x, const float z) {
        if (curPlayPos >= 0 && curPlayPos <= 350) {
            sound->SetPosition({ x, 0.0f, z });
        }
    };

    switch (sound->m_Event) {
    case AE_FRONTEND_CAR_RESPRAY:
        if (curPlayPos > 0 && CTimer::GetTimeInMS() > m_LastTimeCarRespray + 1'900) {
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
        if (curPlayPos > 0 && CTimer::GetTimeInMS() > m_LastTimeTimerCount + 100) {
            sound->StopSoundAndForget();
        }
        break;
    default:
        break;
    }

    if (sound == m_AmplifierWakeUpSound && curPlayPos == -1) {
        m_AmplifierWakeUpSound     = nullptr;
        m_IsAmplifierWakeUpPlaying = false;
    }
}

void CAEFrontendAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEFrontendAudioEntity, 0x862E54, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Initialise, 0x5B9AB0);
    RH_ScopedInstall(Reset, 0x4DD440);
    RH_ScopedInstall(AddAudioEvent, 0x4DD4A0);
    RH_ScopedInstall(IsRadioTuneSoundActive, 0x4DD480);
    RH_ScopedInstall(IsLoadingTuneActive, 0x4DD470);
    RH_ScopedVMTInstall(UpdateParameters, 0x4DEDA0);
}
