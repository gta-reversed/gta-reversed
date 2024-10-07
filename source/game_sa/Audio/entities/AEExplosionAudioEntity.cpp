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
    AEAudioHardware.LoadSoundBank(52, 4);
}

// 0x4DCBE0
void CAEExplosionAudioEntity::AddAudioEvent(eAudioEvents audioEvent, CVector& posn, float volume) {
    if (audioEvent != AE_EXPLOSION)
        return;

    if (!AEAudioHardware.IsSoundBankLoaded(52, 4)) {
        AEAudioHardware.LoadSoundBank(52, 4);
        return;
    }

    float vol0 = GetDefaultVolume(AE_EXPLOSION) + volume;
    m_Speed = (m_Speed + 1) % NUM_VARIATIONS;

    CAESound sound;

    sound.Initialise(4, 4, this, posn, vol0, 2.0f, gfExplosionFrequencyVariations[m_Speed], 1.0f, 0, SOUND_REQUEST_UPDATES, 0.06f, 0);
    AESoundManager.RequestNewSound(&sound);

    sound.Initialise(4, 3, this, posn, vol0, 4.0f, gfExplosionFrequencyVariations[m_Speed], 1.0f, 0, SOUND_REQUEST_UPDATES, 0.06f, 0);
    AESoundManager.RequestNewSound(&sound);

    sound.Initialise(4, 2, this, posn, vol0, 7.5f, gfExplosionFrequencyVariations[m_Speed], 1.0f, 0, SOUND_REQUEST_UPDATES, 0.06f, 0);
    AESoundManager.RequestNewSound(&sound);

    float speed_a, speed_b;
    if (CAEAudioUtility::ResolveProbability(0.5f)) {
        speed_a = gfExplosionFrequencyVariations[m_Speed] * sqrt(sqrt(2.0f));
        speed_b = gfExplosionFrequencyVariations[m_Speed];
    } else {
        speed_a = gfExplosionFrequencyVariations[m_Speed];
        speed_b = gfExplosionFrequencyVariations[m_Speed] * sqrt(sqrt(2.0f));
    }

    auto vol1 = CAEAudioEnvironment::GetDistanceAttenuation(CAEAudioEnvironment::GetPositionRelativeToCamera(posn).Magnitude() * (1.0f / 12.0f)) + vol0 - 3.0f;
    auto flags = static_cast<eSoundEnvironment>(SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_REQUEST_UPDATES | SOUND_FRONT_END);

    sound.Initialise(4, 1, this, { -1.0f, 0.0f, 0.0f }, vol1, 12.0f, speed_a, 1.0f, 0, flags, 0.0f, 0);
    AESoundManager.RequestNewSound(&sound);

    sound.Initialise(4, 1, this, { 1.0f, 0.0f, 0.0f }, vol1, 12.0f, speed_b, 1.0f, 0, flags, 0.0f, 0);
    AESoundManager.RequestNewSound(&sound);
}

// 0x4DCB90
void CAEExplosionAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (curPlayPos > 0) {
        if (sound->m_fVolume > 0.0f) {
            sound->m_fVolume = std::max(sound->m_fVolume - 1.0f, 0.0f);
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
