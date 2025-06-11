#include "StdInc.h"

#include <common.h>

#include "AEWeatherAudioEntity.h"
#include "AEAudioHardware.h"
#include "AEAudioUtility.h"

enum class eWeatherEvent {
    THUNDER    = 1,
    CITY_NOISE = 3,
    UNK_4      = 4,
    UNK_5      = 5,
};


constexpr CVector DEFAULT_POS                   = { -0.906f, 0.f, 0.423f };

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
    //plugin::CallMethod<0x505A00, CAEWeatherAudioEntity*, CAESound*, int16>(this, sound, curPlayPos);

    if (curPlayPos <= 0) {
        return;
    }

    const auto FadeSoundOut = [sound] {
        if (sound->GetVolume() <= -50.f) {
            sound->StopSoundAndForget();
        } else {
            sound->SetVolume(std::max(-50.f, sound->GetVolume() - 0.6f));
        }
    };

    switch ((eWeatherEvent)(sound->m_Event)) {
    case eWeatherEvent::THUNDER: { // 0x505A7F
        const auto reqVolume  = CAEAudioUtility::AudioLog10(CWeather::LightningDuration * 0.0375f + 0.25f) * 20.f + GetDefaultVolume(AE_THUNDER);
        const auto reqPlayPos = 600 - 500 * (CWeather::LightningDuration / 20);
        if ((uint32)(curPlayPos) > reqPlayPos) { // 0x505AD9
            if ((uint32)(curPlayPos) > reqPlayPos + 200 || reqVolume <= 0.f) { // 0x505ACD
                sound->SetVolume(std::min(0.f, reqVolume));
            } else { // 0x505B4F
                const auto vol = std::pow(10.f, reqVolume / 20.f);
                sound->SetVolume(CAEAudioUtility::AudioLog10((float)(curPlayPos - reqPlayPos) * (1.f - vol) / 200.f + vol) * 20.f);
            }
        } else { // 0x505AF3
            sound->SetVolume(CAEAudioUtility::AudioLog10((float)(curPlayPos) / (float)(reqPlayPos)) * 20.f + reqVolume);
        }
        break;
    }
    case eWeatherEvent::CITY_NOISE: { // 0x505BE5 - Bro my head hurts what is this shit
        if (!CGame::CanSeeOutSideFromCurrArea()) {
            FadeSoundOut();
            return;
        }

        const CVector camPos = TheCamera.GetPosition();
        const CVector camRight = TheCamera.GetRightVector().Normalized();
        auto* const player = FindPlayerPed();

        CVector pov = camPos;
        if (player) {
            pov.z = player->GetPosition().z;
        }

        const auto Process = [&](CVector origin, CVector target, float& cityNoiseVolumeBoostResidue, CEntity*& lastEntity) {
            CEntity* hitEntity{};
            CColPoint hitCP{};
            CWorld::ProcessLineOfSight(
                origin,
                target,
                hitCP,
                hitEntity,
                true,
                true,
                false,
                true,
                true,
                false,
                false,
                false
            );
            const auto UpdateSound = [&](float speed, float volume) { // 0x505F56
                if (sound->GetVolume() < volume + GetDefaultVolume(AE_CITY_NOISE)) {
                    sound->SetVolume(std::min(volume + sound->GetVolume() + 0.3f, GetDefaultVolume(AE_CITY_NOISE) + volume));
                } else if (sound->GetVolume() > volume + GetDefaultVolume(AE_CITY_NOISE)) {
                    if (cityNoiseVolumeBoostResidue <= 0.f) {
                       sound->SetVolume(std::max(volume + sound->GetVolume() - 0.3f, GetDefaultVolume(AE_CITY_NOISE) + volume));
                    } else {
                        cityNoiseVolumeBoostResidue = std::min(1.3f, cityNoiseVolumeBoostResidue);
                        sound->SetVolume(std::max(sound->GetVolume() - 0.3f, GetDefaultVolume(AE_CITY_NOISE)) - cityNoiseVolumeBoostResidue);
                        //if (sound->GetVolume() - 0.3f - residue <= GetDefaultVolume(AE_CITY_NOISE) - residue) {
                        //} else {
                        //}
                    }
                }
                sound->SetSpeed(speed);
            };
            if (hitEntity && hitEntity != lastEntity && std::abs(camPos.z - pov.z) < 6.f) { // 0x505D50
                const CVector velocity = player && player->IsInVehicle()
                    ? player->m_pVehicle->GetMoveSpeed()
                    : CVector{};
                const CVector hitPhysicalPos = hitEntity->IsPhysical()
                    ? hitEntity->GetPosition()
                    : CVector{};
                const auto fwd = TheCamera.GetForwardVector().Normalized();
                const auto force = (velocity.ProjectOnToNormal(fwd) - hitPhysicalPos.ProjectOnToNormal(fwd)).Magnitude();
                if (force > 0.35f) { // 0x505E8D
                    const auto speed = (force - 0.35f) / 0.95f;
                    UpdateSound(speed * 1.75f + 1.75f, CAEAudioUtility::AudioLog10((1.f - (pov + camRight - hitCP.m_vecPoint).Magnitude() / 0.6f) * speed) * 20.f + 30.f);
                }
                lastEntity = hitEntity;
            } else {
                UpdateSound(1.f, 0.f);
            }
        };


        if (sound->m_CurrPos == DEFAULT_POS) {
            static auto& fCityNoiseVolumeBoostResidueRight = StaticRef<float>(0xB6BC70);
            static auto& pLastEntityRight = StaticRef<CEntity*>(0xB6BC74);
            Process(pov + camRight * 4.f, pov + camRight, fCityNoiseVolumeBoostResidueRight, pLastEntityRight);
        } else {
            static auto& fCityNoiseVolumeBoostResidueLeft = StaticRef<float>(0xB6BC68);
            static auto& pLastEntityLeft = StaticRef<CEntity*>(0xB6BC6C);
            Process(pov - camRight * 4.f, pov - camRight, fCityNoiseVolumeBoostResidueLeft, pLastEntityLeft);
        }

        break;
    }
    case eWeatherEvent::UNK_4:
    case eWeatherEvent::UNK_5: { // 0x50652F
        static auto& sbWindOffset = StaticRef<bool>(0x8CC2C0);
        static auto& sfWindOffset = StaticRef<float>(0xB6BAFC);
        static auto& sfWindFreq = StaticRef<float>(0xB6BAF8);
        static auto& sfOldFreqLeft = StaticRef<float>(0x8CC2C4);
        
        const auto zPosFactor = std::clamp(TheCamera.GetPosition().z / 500.f, 0.f, 1.f);
        const auto windRatio = lerp(
            CGeneral::GetPiecewiseLinear({
                { 0.0f, 0.0f },
                { 0.3f, 0.4f },
                { 1.0f, 0.5f }
            }, zPosFactor),
            CGeneral::GetPiecewiseLinear({
                { 0.0f, 0.5f },
                { 0.3f, 1.0f },
                { 1.0f, 1.0f }
            }, zPosFactor),
            std::clamp(CWeather::WindClipped, 0.f, 1.f)
        );
        if (CAEAudioUtility::ResolveProbability(0.07f)) { // 0x506642
            sbWindOffset = !sbWindOffset;
        }
        sbWindOffset = sbWindOffset // 0x50667E
            ? 21.f * windRatio
            : 0.f;
        sfWindFreq = sbWindOffset // 0x50668E
            ? 1.2f * windRatio
            : 0.f;
        if (!CGame::CanSeeOutSideFromCurrArea()) { // 0x506694
            FadeSoundOut();
        } else { // 0x5066A6
            sound->SetVolume(notsa::step_to(sound->GetVolume(), sfWindOffset - 33.f, 0.6f));

            sfOldFreqLeft = sfOldFreqLeft > sfWindFreq
                ? notsa::step_up_to(sfOldFreqLeft, sfWindFreq, 0.1f)                   // 0x506716
                : std::max(1.f, notsa::step_down_to(sfOldFreqLeft, sfWindFreq, 0.1f)); // 0x50677C
            sound->SetSpeed(sfOldFreqLeft);
        }
        break;
    }
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x5052F0
void CAEWeatherAudioEntity::Service() {
    if (CGame::CanSeeOutSideFromCurrArea()) { // 0x50538C
        const auto EnsureSoundForEventIsPlaying = [this](eSoundID sfx, eWeatherEvent event, int16 startPlayPercentage) {
            if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
                return;
            }
            if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(+event, this)) {
                return;
            }
            AESoundManager.PlaySound({
                .BankSlotID  = SND_BANK_SLOT_FRONTEND_GAME,
                .SoundID     = sfx,
                .AudioEntity = this,
                .Pos         = DEFAULT_POS,
                .Volume      = -50.f,
                .Flags       = SOUND_FORCED_FRONT | SOUND_START_PERCENTAGE | SOUND_REQUEST_UPDATES | SOUND_FRONT_END,
                .PlayTime    = startPlayPercentage,
                .EventID     = +event,
            });
        };

        // 0x50539E
        EnsureSoundForEventIsPlaying(0, eWeatherEvent::CITY_NOISE, 0); 
        EnsureSoundForEventIsPlaying(0, eWeatherEvent::CITY_NOISE, 50);

        // 0x505482
        EnsureSoundForEventIsPlaying(29, eWeatherEvent::UNK_4, 0);

        // 0x505509
        EnsureSoundForEventIsPlaying(30, eWeatherEvent::UNK_5, 50);
    }

    const auto UpdateRainSounds = [this](float targetVolume) {
        m_sfRainVolume = notsa::step_to(m_sfRainVolume, targetVolume, 0.5f);

        if (targetVolume == -100.f && m_sfRainVolume <= -50.f) { // 0x50566D
            m_sfRainVolume = -100.f;
            if (m_sRainSoundL.IsActive()) {
                m_sRainSoundL.StopSoundAndForget();
            }
            if (m_sRainSoundR.IsActive()) {
                m_sRainSoundR.StopSoundAndForget();
            }
        } else {
            const auto UpdateRainTwinLoopSound = [this](CAETwinLoopSoundEntity& sound, float posX) {
                if (sound.IsActive()) {
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
                            .Flags      = SOUND_IS_CANCELLABLE | SOUND_FRONT_END
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
                        (eSoundEnvironment)(SOUND_START_PERCENTAGE | SOUND_IS_CANCELLABLE | SOUND_FRONT_END)
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
    RH_ScopedVMTInstall(UpdateParameters, 0x505A00);
    RH_ScopedInstall(Service, 0x5052F0);
}
