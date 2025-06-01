#include "StdInc.h"

#include <common.h>

#include "AEWeatherAudioEntity.h"
#include "AEAudioHardware.h"
#include "AEAudioUtility.h"


float& CAEWeatherAudioEntity::m_sfRainVolume = *(float*)0x8CC30C; // -100.0f

auto& m_snLastRainDropSoundID = StaticRef<int32>(0x8CC310); // TODO: Use `eSoundID`
CAETwinLoopSoundEntity& m_sRainSoundL = *(CAETwinLoopSoundEntity*)0xB6BB18;  // dunno about names
CAETwinLoopSoundEntity& m_sRainSoundR = *(CAETwinLoopSoundEntity*)0xB6BBC0;

// 0x72A620
CAEWeatherAudioEntity::CAEWeatherAudioEntity() : CAEAudioEntity() {
    m_nThunderFrequencyVariationCounter = 0;
}

// 0x5B9A70
void CAEWeatherAudioEntity::StaticInitialise() {
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_RAIN, SND_BANK_SLOT_WEATHER);
}

// 0x5052B0
void CAEWeatherAudioEntity::StaticReset() {
    m_sfRainVolume = -100.0f;

    if (m_sRainSoundL.IsActive()) {
        m_sRainSoundL.StopSoundAndForget();
    }

    if (m_sRainSoundR.IsActive()) {
        m_sRainSoundR.StopSoundAndForget();
    }
}

// 0x506800, see discord gists channel
void CAEWeatherAudioEntity::AddAudioEvent(eAudioEvents event) {
    plugin::CallMethod<0x506800, CAEWeatherAudioEntity*, eAudioEvents>(this, event);
}

// 0x505A00
void CAEWeatherAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    plugin::CallMethod<0x505A00, CAEWeatherAudioEntity*, CAESound*, int16>(this, sound, curPlayPos);
}

// 0x5052F0, see discord gists channel
void CAEWeatherAudioEntity::Service() {
    if (CGame::CanSeeOutSideFromCurrArea()) { // 0x50538C
        const auto PlaySoundForEvent = [this](eSoundID sfx, eAudioEvents ae, int16 playPercentage) {
            AESoundManager.PlaySound({
                .BankSlotID  = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID     = sfx,
                .AudioEntity = this,
                .Pos         = CVector{ -0.906f, 0.f, 0.423f },
                .Volume      = -50.f,
                .Flags       = SOUND_FORCED_FRONT | SOUND_START_PERCENTAGE | SOUND_REQUEST_UPDATES | SOUND_FRONT_END,
                .PlayTime    = playPercentage,
                .EventID     = ae,
            });
        };

        if (!AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_FRONTEND_HIGHLIGHT, this)) { // 0x50539E
            if (AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) { // 0x5053B5
                PlaySoundForEvent(0, AE_FRONTEND_HIGHLIGHT, 0);
                PlaySoundForEvent(0, AE_FRONTEND_HIGHLIGHT, 50);
            }
        }

        if (!AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_FRONTEND_ERROR, this)) { // 0x505482
            if (AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) { // 0x505495
                PlaySoundForEvent(29, AE_FRONTEND_ERROR, 0);
            }
        }

        if (!AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_FRONTEND_NOISE_TEST, this)) { // 0x505509
            if (AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) { // 0x50551C
                PlaySoundForEvent(30, AE_FRONTEND_NOISE_TEST, 50);
            }
        }
    }

    const auto UpdateRainSounds = [this](float targetVolume) {
        m_sfRainVolume = step_to(m_sfRainVolume, targetVolume, 0.5f);

        if (targetVolume == -100.f && m_sfRainVolume <= -50.f) { // 0x50566D
            m_sfRainVolume = -100.f;
            if (m_sRainSoundL.m_bIsInitialised) {
                m_sRainSoundL.StopSoundAndForget();
            }
            if (m_sRainSoundR.m_bIsInitialised) {
                m_sRainSoundR.StopSoundAndForget();
            }
        } else {
            const auto UpdateRainTwinLoopSound = [this](CAETwinLoopSoundEntity& sound, float posX) {
                if (sound.m_bIsInitialised) {
                    sound.UpdateTwinLoopSound({posX, 0.f, 0.423f}, m_sfRainVolume, 1.f);
                    if (sound.DoSoundsSwitchThisFrame()) {
                        m_snLastRainDropSoundID = m_snLastRainDropSoundID + 1 <= 11
                            ? m_snLastRainDropSoundID + 1
                            : 2;
                        AESoundManager.PlaySound({
                            .BankSlotID = SND_BANK_SLOT_WEATHER,
                            .SoundID    = (eSoundID)(m_snLastRainDropSoundID),
                            .Pos        = CVector{ posX, CAEAudioUtility::ResolveProbability(0.5f) ? 0.423f : -0.423f, 0.f },
                            .Volume     = CAEAudioUtility::GetRandomNumberInRange(-6.f, 6.f) + m_sfRainVolume - 15.f,
                            .Flags      = SOUND_UNCANCELLABLE | SOUND_FRONT_END
                        });
                    }
                } else if (AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_RAIN, SND_BANK_SLOT_WEATHER)) {
                    sound.Initialise(
                        SND_BANK_SLOT_WEATHER,
                        1,
                        0,
                        this,
                        65u,
                        350u
                    );
                    sound.PlayTwinLoopSound(
                        { posX, 0.423f, 0.f },
                        m_sfRainVolume,
                        1.f,
                        1.f,
                        1.f,
                        (eSoundEnvironment)(SOUND_START_PERCENTAGE | SOUND_UNCANCELLABLE | SOUND_FRONT_END)
                    );
                }
            };
            UpdateRainTwinLoopSound(m_sRainSoundL, -0.906f);
            UpdateRainTwinLoopSound(m_sRainSoundR, 0.906f);
        }
    };
    if (CWeather::Rain <= 0.f || CCullZones::PlayerNoRain() || CCullZones::CamNoRain() || !CGame::CanSeeOutSideFromCurrArea()) { // 0x5055A3
        UpdateRainSounds(-100.0f);
    } else { // 0x5055D5
        m_sfRainVolume = std::max(m_sfRainVolume, -50.f);
        UpdateRainSounds(GetDefaultVolume(AE_RAIN) + CAEAudioUtility::AudioLog10(std::max(CWeather::Rain - 0.2f, 0.f) / 0.8f) * 20.f);
    }
}

void CAEWeatherAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEWeatherAudioEntity, 0x872A74, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(StaticInitialise, 0x5B9A70);
    RH_ScopedInstall(StaticReset, 0x5052B0);
    RH_ScopedInstall(AddAudioEvent, 0x506800, { .reversed = false });
    RH_ScopedVMTInstall(UpdateParameters, 0x505A00, { .reversed = false });
    RH_ScopedInstall(Service, 0x5052F0);
}
