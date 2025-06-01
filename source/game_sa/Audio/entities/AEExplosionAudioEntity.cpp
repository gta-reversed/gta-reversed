#include "StdInc.h"

#include "AEExplosionAudioEntity.h"
#include "AEAudioHardware.h"
#include "AEAudioEnvironment.h"
#include "AEAudioUtility.h"

float gfExplosionFrequencyVariations[] = { 1.12f, 1.0f, 0.88f }; // 0x8AE58C
constexpr auto NUM_VARIATIONS = std::size(gfExplosionFrequencyVariations);

// 0x5074B0
CAEExplosionAudioEntity::CAEExplosionAudioEntity() {
    m_Speed = 0;
}

void CAEExplosionAudioEntity::StaticInitialise() {
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_EXPLOSIONS, SND_BANK_SLOT_EXPLOSIONS);
}

// 0x4DCBE0
void CAEExplosionAudioEntity::AddAudioEvent(eAudioEvents audioEvent, CVector& posn, float volume) {
    volume += GetDefaultVolume(AE_EXPLOSION);

    if (audioEvent != AE_EXPLOSION) {
        return;
    }

    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_EXPLOSIONS, SND_BANK_SLOT_EXPLOSIONS)) {
        return;
    }

    {
        m_Speed = (m_Speed + 1) % NUM_VARIATIONS;
        const auto PlayExplosionSound = [&](eSoundID soundID, float rollOff) {
            AESoundManager.PlaySound({
                .BankSlotID        = SND_BANK_SLOT_EXPLOSIONS,
                .SoundID           = soundID,
                .AudioEntity       = this,
                .Pos               = posn,
                .Volume            = volume,
                .RollOffFactor     = rollOff,
                .Speed             = gfExplosionFrequencyVariations[m_Speed],
                .Flags             = SOUND_REQUEST_UPDATES,
                .FrequencyVariance = 0.06f,
            });
        };
        PlayExplosionSound(4, 2.f);
        PlayExplosionSound(3, 4.f);
        PlayExplosionSound(2, 7.5f);
    }
    {
        const auto PlayExplosionSound = [&](CVector pos, bool variance) {
            auto speed = gfExplosionFrequencyVariations[m_Speed];
            if (variance) {
                speed *= sqrt(sqrt(2.0f));
            }
            AESoundManager.PlaySound({
                .BankSlotID        = SND_BANK_SLOT_EXPLOSIONS,
                .SoundID           = 1,
                .AudioEntity       = this,
                .Pos               = posn,
                .Volume            = volume + CAEAudioEnvironment::GetDistanceAttenuation(CAEAudioEnvironment::GetPositionRelativeToCamera(posn).Magnitude() / 12.0f) - 3.0f,
                .RollOffFactor     = 12.f,
                .Speed             = gfExplosionFrequencyVariations[m_Speed],
                .Flags             = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_REQUEST_UPDATES | SOUND_FRONT_END,
                .FrequencyVariance = 0.06f,
            });
        };
        const auto variance = CAEAudioUtility::ResolveProbability(0.5f);
        PlayExplosionSound({ -1.0f, 0.0f, 0.0f }, variance);
        PlayExplosionSound({ 1.0f, 0.0f, 0.0f }, !variance);
    }
}

// 0x4DCB90
void CAEExplosionAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (curPlayPos > 0) {
        if (sound->m_Volume > 0.0f) {
            sound->m_Volume = std::max(sound->m_Volume - 1.0f, 0.0f);
        }
    }
}

void CAEExplosionAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEExplosionAudioEntity, 0x862E60, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(StaticInitialise, 0x5B9A60);
    RH_ScopedInstall(AddAudioEvent, 0x4DCBE0);
    RH_ScopedVMTInstall(UpdateParameters, 0x4DCB90);
}
