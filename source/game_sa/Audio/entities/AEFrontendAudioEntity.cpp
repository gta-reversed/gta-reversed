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
    m_BulletPassCount                  = 0;
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
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 25,
                .AudioEntity     = this,
                .Pos             = XVECM,
                .Volume          = volume,
                .Speed           = fSpeed,
                .Flags           = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END
            });
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 26,
                .AudioEntity     = this,
                .Pos             = XVECP,
                .Volume          = volume,
                .Speed           = fSpeed,
                .Flags           = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END
            });
        }
        return;
    case AE_FRONTEND_SELECT:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_MENU, SND_BANK_SLOT_FRONTEND_MENU)) {
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_MENU,
                .SoundID         = 6,
                .AudioEntity     = this,
                .Pos             = XVECM,
                .Volume          = volume,
                .Speed           = fSpeed,
                .Flags           = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END
            });
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_MENU,
                .SoundID         = 7,
                .AudioEntity     = this,
                .Pos             = XVECP,
                .Volume          = volume,
                .Speed           = fSpeed,
                .Flags           = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END
            });
        }
        return;
    case AE_FRONTEND_BACK:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_MENU, SND_BANK_SLOT_FRONTEND_MENU)) {
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_MENU,
                .SoundID         = 0,
                .AudioEntity     = this,
                .Pos             = XVECM,
                .Volume          = volume,
                .Speed           = fSpeed,
                .Flags           = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END
            });
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_MENU,
                .SoundID         = 1,
                .AudioEntity     = this,
                .Pos             = XVECP,
                .Volume          = volume,
                .Speed           = fSpeed,
                .Flags           = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END
            });
        }
        return;
    case AE_FRONTEND_HIGHLIGHT:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_MENU, SND_BANK_SLOT_FRONTEND_MENU)) {
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_MENU,
                .SoundID         = 4,
                .AudioEntity     = this,
                .Pos             = XVECM,
                .Volume          = volume,
                .Speed           = fSpeed,
                .Flags           = SOUND_FORCED_FRONT | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END
            });
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_MENU,
                .SoundID         = 5,
                .AudioEntity     = this,
                .Pos             = XVECP,
                .Volume          = volume,
                .Speed           = fSpeed,
                .Flags           = SOUND_FORCED_FRONT | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END
            });
        }
        return;
    case AE_FRONTEND_ERROR:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_MENU, SND_BANK_SLOT_FRONTEND_MENU)) {
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_MENU,
                .SoundID         = 2,
                .AudioEntity     = this,
                .Pos             = XVECM,
                .Volume          = volume,
                .Speed           = fSpeed,
                .Flags           = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END
            });
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_MENU,
                .SoundID         = 3,
                .AudioEntity     = this,
                .Pos             = XVECP,
                .Volume          = volume,
                .Speed           = fSpeed,
                .Flags           = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END
            });
        }
        return;
    case AE_FRONTEND_NOISE_TEST:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_MENU, SND_BANK_SLOT_FRONTEND_MENU)) {
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_MENU,
                .SoundID         = 8,
                .AudioEntity     = this,
                .Pos             = YVECP,
                .Volume          = volume,
                .Speed           = fSpeed,
                .Flags           = SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END
            });
        }
        return;
    case AE_FRONTEND_PICKUP_WEAPON:
    case AE_FRONTEND_CAR_FIT_BOMB_TIMED:
    case AE_FRONTEND_CAR_FIT_BOMB_BOOBY_TRAPPED:
    case AE_FRONTEND_CAR_FIT_BOMB_REMOTE_CONTROLLED:
    case AE_FRONTEND_PURCHASE_WEAPON:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME) && CTimer::GetFrameCounter() >= m_nLastFrameGeneral_or_nFrameCount + 5) {
            m_nLastFrameGeneral_or_nFrameCount = CTimer::GetFrameCounter();
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 27,
                .AudioEntity     = this,
                .Pos             = XVECM,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = fSpeed,
                .Doppler         = 1.0f,
                .FrameDelay      = 0,
                .Flags           = SOUND_FORCED_FRONT | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
                .FrequencyVariance = 0.0f,
                .PlayTime        = 0
            });

            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 28,
                .AudioEntity     = this,
                .Pos             = XVECP,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = fSpeed,
                .Doppler         = 1.0f,
                .FrameDelay      = 0,
                .Flags           = SOUND_FORCED_FRONT | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
                .FrequencyVariance = 0.0f,
                .PlayTime        = 0
            });
        }
        return;
    case AE_FRONTEND_PICKUP_MONEY:
    case AE_FRONTEND_PICKUP_HEALTH:
    case AE_FRONTEND_PICKUP_ADRENALINE:
    case AE_FRONTEND_PICKUP_BODY_ARMOUR:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME) && CTimer::GetFrameCounter() >= m_nLastFrameGeneral_or_nFrameCount + 5) {
            m_nLastFrameGeneral_or_nFrameCount = CTimer::GetFrameCounter();
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 16,
                .AudioEntity     = this,
                .Pos             = XVECM,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = fSpeed,
                .Doppler         = 1.0f,
                .FrameDelay      = 0,
                .Flags           = SOUND_FORCED_FRONT | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
                .FrequencyVariance = 0.0f,
                .PlayTime        = 0
            });

            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 17,
                .AudioEntity     = this,
                .Pos             = XVECP,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = fSpeed,
                .Doppler         = 1.0f,
                .FrameDelay      = 0,
                .Flags           = SOUND_FORCED_FRONT | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
                .FrequencyVariance = 0.0f,
                .PlayTime        = 0
            });
        }
        return;
    case AE_FRONTEND_PICKUP_INFO:
    case AE_FRONTEND_DISPLAY_INFO:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 14,
                .AudioEntity     = this,
                .Pos             = XVECM,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = fSpeed,
                .Doppler         = 1.0f,
                .FrameDelay      = 0,
                .Flags           = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
                .FrequencyVariance = 0.0f,
                .PlayTime        = 0
            });

            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 15,
                .AudioEntity     = this,
                .Pos             = XVECP,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = fSpeed,
                .Doppler         = 1.0f,
                .FrameDelay      = 0,
                .Flags           = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
                .FrequencyVariance = 0.0f,
                .PlayTime        = 0
            });
        }
        return;
    case AE_FRONTEND_PICKUP_DRUGS:
    case AE_FRONTEND_PICKUP_COLLECTABLE1:
    case AE_FRONTEND_PART_MISSION_COMPLETE:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME) && CTimer::GetFrameCounter() >= m_nLastFrameMissionComplete + 5) {
            m_nLastFrameMissionComplete = CTimer::GetFrameCounter();
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 18,
                .AudioEntity     = this,
                .Pos             = XVECM,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = fSpeed,
                .Doppler         = 1.0f,
                .FrameDelay      = 0,
                .Flags           = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
                .FrequencyVariance = 0.0f,
                .PlayTime        = 0
            });

            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 19,
                .AudioEntity     = this,
                .Pos             = XVECP,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = fSpeed,
                .Doppler         = 1.0f,
                .FrameDelay      = 0,
                .Flags           = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
                .FrequencyVariance = 0.0f,
                .PlayTime        = 0
            });
        }
        return;
    case AE_FRONTEND_CAR_NO_CASH:
    case AE_FRONTEND_CAR_IS_HOT:
    case AE_FRONTEND_CAR_ALREADY_RIGGED:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME) && CTimer::GetFrameCounter() >= m_nLastFrameGeneral_or_nFrameCount + 5) {
            m_nLastFrameGeneral_or_nFrameCount = CTimer::GetFrameCounter();
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 27,
                .AudioEntity     = this,
                .Pos             = XVECM,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = 1.0f,
                .Doppler         = 0.841f,
                .FrameDelay      = 0,
                .Flags           = SOUND_FORCED_FRONT | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
                .FrequencyVariance = 0.0f,
                .PlayTime        = 0
            });

            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 28,
                .AudioEntity     = this,
                .Pos             = XVECP,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = 1.0f,
                .Doppler         = 0.841f,
                .FrameDelay      = 0,
                .Flags           = SOUND_FORCED_FRONT | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
                .FrequencyVariance = 0.0f,
                .PlayTime        = 0
            });
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
        AESoundManager.PlaySound({
            .BankSlotID      = SND_BANK_SLOT_WEAPON_GEN,
            .SoundID         = 28,
            .AudioEntity     = this,
            .Pos             = XVECM,
            .Volume          = volume,
            .RollOffFactor   = 1.0f,
            .Speed           = 1.0f,
            .Doppler         = speed0,
            .FrameDelay      = 0,
            .Flags           = SOUND_FORCED_FRONT | SOUND_IS_DUCKABLE | SOUND_PLAY_PHYSICALLY | SOUND_REQUEST_UPDATES | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
            .FrequencyVariance = 0.0f,
            .PlayTime        = 0
        });

        AESoundManager.PlaySound({
            .BankSlotID      = SND_BANK_SLOT_WEAPON_GEN,
            .SoundID         = 28,
            .AudioEntity     = this,
            .Pos             = XVECP,
            .Volume          = volume,
            .RollOffFactor   = 1.0f,
            .Speed           = 1.0f,
            .Doppler         = speed1,
            .FrameDelay      = 0,
            .Flags           = SOUND_FORCED_FRONT | SOUND_IS_DUCKABLE | SOUND_PLAY_PHYSICALLY | SOUND_REQUEST_UPDATES | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
            .FrequencyVariance = 0.0f,
            .PlayTime        = 0
        });
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
        AESoundManager.PlaySound({
            .BankSlotID      = SND_BANK_SLOT_BULLET_PASS,
            .SoundID         = CAEAudioUtility::GetRandomNumberInRange<eSoundID>(0, 2),
            .AudioEntity     = this,
            .Pos             = { -0.1f, -1.0f, 0.0f },
            .Volume          = volume,
            .RollOffFactor   = 1.0f,
            .Speed           = 1.0f,
            .Doppler         = 1.0f,
            .FrameDelay      = 0,
            .Flags           = SOUND_REQUEST_UPDATES | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
            .FrequencyVariance = 0.03125f,
            .PlayTime        = 0
        });
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
        AESoundManager.PlaySound({
            .BankSlotID      = SND_BANK_SLOT_BULLET_PASS,
            .SoundID         = CAEAudioUtility::GetRandomNumberInRange<eSoundID>(0, 2),
            .AudioEntity     = this,
            .Pos             = { -0.1f, +1.0f, 0.0f },
            .Volume          = volume,
            .RollOffFactor   = 1.0f,
            .Speed           = 1.0f,
            .Doppler         = 1.0f,
            .FrameDelay      = 0,
            .Flags           = SOUND_REQUEST_UPDATES | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
            .FrequencyVariance = 0.03125f,
            .PlayTime        = 0
        });
        ++m_BulletPassCount;
        return;
    case AE_FRONTEND_BULLET_PASS_RIGHT_REAR:
        if (!AEAudioHardware.IsSoundBankLoaded(m_BulletPassBank, SND_BANK_SLOT_BULLET_PASS) || CTimer::GetFrameCounter() < m_nLastFrameBulletPass + 5)
            return;

        if (m_BulletPassCount <= 10) {
            m_nLastFrameBulletPass = CTimer::GetFrameCounter();
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_BULLET_PASS,
                .SoundID         = CAEAudioUtility::GetRandomNumberInRange<eSoundID>(0, 2),
                .AudioEntity     = this,
                .Pos             = { 0.1f, -1.0f, 0.0f },
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = 1.0f,
                .Doppler         = 1.0f,
                .FrameDelay      = 0,
                .Flags           = SOUND_REQUEST_UPDATES | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
                .FrequencyVariance = 0.03125f,
                .PlayTime        = 0
            });
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
                AESoundManager.PlaySound({
                    .BankSlotID      = SND_BANK_SLOT_BULLET_PASS,
                    .SoundID         = CAEAudioUtility::GetRandomNumberInRange<eSoundID>(0, 2),
                    .AudioEntity     = this,
                    .Pos             = { 0.1f, +1.0f, 0.0f },
                    .Volume          = volume,
                    .RollOffFactor   = 1.0f,
                    .Speed           = 1.0f,
                    .Doppler         = 1.0f,
                    .FrameDelay      = 0,
                    .Flags           = SOUND_REQUEST_UPDATES | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
                    .FrequencyVariance = 0.03125f,
                    .PlayTime        = 0
                });
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
            m_pAmplifierWakeUp = AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 8,
                .AudioEntity     = this,
                .Pos             = YVECP,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = fSpeed,
                .Doppler         = 1.0f,
                .FrameDelay      = 0,
                .Flags           = SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_REQUEST_UPDATES | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
                .FrequencyVariance = 0.0f,
                .PlayTime        = 0
            });
            m_bAmplifierWakeUp = m_pAmplifierWakeUp != nullptr;
        }
        return;
    case AE_FRONTEND_TIMER_COUNT:
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_FRONTEND_TIMER_COUNT, this)) {
            m_nLatestTimerCount = CTimer::GetTimeInMS();
            return;
        }

        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 4,
                .AudioEntity     = this,
                .Pos             = XVECM,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = fSpeed,
                .Doppler         = 1.0f,
                .FrameDelay      = 0,
                .Flags           = SOUND_FORCED_FRONT | SOUND_IS_DUCKABLE | SOUND_PLAY_PHYSICALLY | SOUND_REQUEST_UPDATES | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
                .FrequencyVariance = 0.0f,
                .PlayTime        = 0
            });

            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 5,
                .AudioEntity     = this,
                .Pos             = XVECP,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = fSpeed,
                .Doppler         = 1.0f,
                .FrameDelay      = 0,
                .Flags           = SOUND_FORCED_FRONT | SOUND_IS_DUCKABLE | SOUND_PLAY_PHYSICALLY | SOUND_REQUEST_UPDATES | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
                .FrequencyVariance = 0.0f,
                .PlayTime        = 0
            });
            m_nLatestTimerCount = CTimer::GetTimeInMS();
        }
        return;
    case AE_FRONTEND_RADIO_CLICK_ON:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 23,
                .AudioEntity     = this,
                .Pos             = YVECP,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = 1.0f,
                .Doppler         = 1.0f,
                .FrameDelay      = 0,
                .Flags           = SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
                .FrequencyVariance = 0.0f,
                .PlayTime        = 0
            });
        }
        return;
    case AE_FRONTEND_RADIO_CLICK_OFF:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 23,
                .AudioEntity     = this,
                .Pos             = YVECP,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = 1.0f,
                .Doppler         = 0.8909f,
                .FrameDelay      = 0,
                .Flags           = SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
                .FrequencyVariance = 0.0f,
                .PlayTime        = 0
            });
        }
        return;
    case AE_FRONTEND_FIRE_FAIL_SNIPERRIFFLE:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 10,
                .AudioEntity     = this,
                .Pos             = YVECP,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = fSpeed,
                .Doppler         = 1.0f,
                .FrameDelay      = 0,
                .Flags           = SOUND_FRONT_END | SOUND_IS_CANCELLABLE,
                .FrequencyVariance = 0.0f,
                .PlayTime        = 0
            });
        }
        return;
    case AE_FRONTEND_FIRE_FAIL_ROCKET:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 11,
                .AudioEntity     = this,
                .Pos             = YVECP,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = fSpeed,
                .Doppler         = 1.0f,
                .FrameDelay      = 0,
                .Flags           = SOUND_FRONT_END | SOUND_IS_CANCELLABLE,
                .FrequencyVariance = 0.0f,
                .PlayTime        = 0
            });
        }
        return;
    case AE_FRONTEND_BUY_CAR_MOD:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 9,
                .AudioEntity     = this,
                .Pos             = YVECP,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = 1.0f,
                .Doppler         = 1.0f,
                .FrameDelay      = 0,
                .Flags           = SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
                .FrequencyVariance = 0.0588f,
                .PlayTime        = 0
            });
        }
        return;
    case AE_FRONTEND_SCANNER_NOISE_START:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME) && !AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_FRONTEND_SCANNER_NOISE_START, this)) {
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 3,
                .AudioEntity     = this,
                .Pos             = YVECP,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = fSpeed,
                .Doppler         = 1.0f,
                .FrameDelay      = 0,
                .Flags           = SOUND_IS_DUCKABLE | SOUND_IS_CANCELLABLE | SOUND_FRONT_END
            });
        }
        return;
    case AE_FRONTEND_SCANNER_CLICK:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME) && !AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_FRONTEND_SCANNER_CLICK, this)) {
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 24,
                .AudioEntity     = this,
                .Pos             = YVECP,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = fSpeed,
                .Doppler         = 1.0f,
                .FrameDelay      = 0,
                .Flags           = SOUND_IS_DUCKABLE | SOUND_IS_CANCELLABLE | SOUND_FRONT_END
            });
        }
        return;
    case AE_FRONTEND_LOADING_TUNE_START:
        if (   AEAudioHardware.IsSoundLoaded(SND_BANK_GENRL_LOADING, 2 * m_nbLoadingTuneSeed, SND_BANK_SLOT_COLLISIONS)
            && AEAudioHardware.IsSoundLoaded(SND_BANK_GENRL_LOADING, 2 * m_nbLoadingTuneSeed + 1, SND_BANK_SLOT_WEAPON_GEN)
            && !AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_FRONTEND_LOADING_TUNE_START, this)
        ) {
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_WEAPON_GEN,
                .SoundID         = 2 * m_nbLoadingTuneSeed,
                .AudioEntity     = this,
                .Pos             = XVECM,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = fSpeed,
                .Doppler         = 1.0f,
                .FrameDelay      = 0,
                .Flags           = SOUND_FORCED_FRONT | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
                .FrequencyVariance = 0.0f,
                .PlayTime        = 0
            });

            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_WEAPON_GEN,
                .SoundID         = 2 * m_nbLoadingTuneSeed + 1,
                .AudioEntity     = this,
                .Pos             = XVECP,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = fSpeed,
                .Doppler         = 1.0f,
                .FrameDelay      = 0,
                .Flags           = SOUND_FORCED_FRONT | SOUND_IS_DUCKABLE | SOUND_IS_PAUSABLE | SOUND_PLAY_PHYSICALLY | SOUND_IS_CANCELLABLE | SOUND_FRONT_END,
                .FrequencyVariance = 0.0f,
                .PlayTime        = 0
            });
        }
        return;
    case AE_MISSILE_LOCK:
        if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME) && !AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_MISSILE_LOCK, this)) {
            AESoundManager.PlaySound({
                .BankSlotID      = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID         = 13,
                .AudioEntity     = this,
                .Pos             = YVECP,
                .Volume          = volume,
                .RollOffFactor   = 1.0f,
                .Speed           = fSpeed,
                .Doppler         = 1.0f,
                .FrameDelay      = 0,
                .Flags           = SOUND_IS_DUCKABLE | SOUND_FRONT_END
            });
        }
        return;
    default:
        return;
    }
}

// 0x4DD480
bool CAEFrontendAudioEntity::IsRadioTuneSoundActive() {
    return CTimer::GetIsPaused()
        ? m_objRetunePaused.IsActive()
        : m_objRetune.IsActive();
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
