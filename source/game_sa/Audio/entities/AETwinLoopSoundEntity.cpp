#include "StdInc.h"

#include "AEAudioUtility.h"
#include "AESoundManager.h"

// 0x4F2AE0
CAETwinLoopSoundEntity::CAETwinLoopSoundEntity(
    eSoundBankSlot  bankSlot,
    eSoundID        soundA,
    eSoundID        soundB,
    CAEAudioEntity* parent,
    uint16          swapTimeMin,
    uint16          swapTimeMax,
    int16           playPercentageA,
    int16           playPercentageB
) :
    m_IsPlayingFirstSound{ true },
    m_IsInUse{ true },
    m_SwapTimeMin{ swapTimeMin },
    m_SwapTimeMax{ swapTimeMax },
    m_BankSlot{ bankSlot },
    m_AudioEntity{ parent },
    m_SoundIDs{ soundA, soundB },
    m_SoundPlayPercentages{ playPercentageA, playPercentageB }
{
    rng::fill(m_SoundsLengths, -1);
}

// 0x4F2B80
CAETwinLoopSoundEntity::~CAETwinLoopSoundEntity() {
    if (m_IsInUse) {
        StopSoundAndForget();
    }
    m_IsInUse = false;
}

// 0x4F28A0
void CAETwinLoopSoundEntity::Initialise(
    eSoundBankSlot  bank,
    eSoundID        soundA,
    eSoundID        soundB,
    CAEAudioEntity* parent,
    uint16          swapTimeMin,
    uint16          swapTimeMax,
    int16           playPercentageA,
    int16           playPercentageB
) {
    assert(!m_IsInUse);

    *this = CAETwinLoopSoundEntity{
        bank,
        soundA,
        soundB,
        parent,
        swapTimeMin,
        swapTimeMax,
        playPercentageA,
        playPercentageB,
    };
}

// 0x4F29A0
void CAETwinLoopSoundEntity::UpdateTwinLoopSound(CVector pos, float volume, float speed) {
    for (auto&& [i, sound] : notsa::enumerate(m_Sounds)) {
        if (!sound) {
            continue;
        }
        sound->SetPosition(pos);
        if (i == (m_IsPlayingFirstSound ? 0 : 1)) {
            sound->SetVolume(volume);
        }
        sound->SetSpeed(speed);
    }
}

// 0x4F2E90
void CAETwinLoopSoundEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (curPlayPos == -1) {
        for (auto*& s : m_Sounds) {
            if (s == sound) {
                s = nullptr;
            }
        }
    }
    if (DoSoundsSwitchThisFrame()) {
        if (sound == m_Sounds[0]) {
            SwapSounds();
        }
    }
}

// 0x4F2C10
void CAETwinLoopSoundEntity::SwapSounds() {
    auto* curr = m_Sounds[m_IsPlayingFirstSound ? 0 : 1];
    auto* next = m_Sounds[m_IsPlayingFirstSound ? 1 : 0];
    if (!curr || !next) {
        return;
    }
    next->m_fVolume = std::exchange(curr->m_fVolume, -100.f);
    m_IsPlayingFirstSound = !m_IsPlayingFirstSound;
    ReCalculateSwapTime();
}

// 0x4F2930
void CAETwinLoopSoundEntity::StopSound() {
    for (auto&& s : m_Sounds) {
        if (s) {
            s->StopSound();
        }
    }
}

// 0x4F2960
void CAETwinLoopSoundEntity::StopSoundAndForget() {
    for (auto&& sound : m_Sounds) {
        if (auto* const s = std::exchange(sound, nullptr)) {
            s->StopSoundAndForget();
        }
    }
    m_IsInUse = false;
}

// unused
// 0x4F2A80
float CAETwinLoopSoundEntity::GetEmittedVolume() const {
    auto* const s = m_Sounds[m_IsPlayingFirstSound ? 0 : 1];
    return s
        ? s->GetVolume()
        : -100.f;
}

// unused
// 0x4F2A40
void CAETwinLoopSoundEntity::SetEmittedVolume(float volume) const {
    auto* const s = m_Sounds[m_IsPlayingFirstSound ? 0 : 1];
    s->SetVolume(volume);
}

// unused
// 0x4F2AC0
bool CAETwinLoopSoundEntity::IsTwinLoopPlaying() const {
    return rng::any_of(m_Sounds, notsa::NotIsNull{});
}

// 0x4F2CA0
bool CAETwinLoopSoundEntity::DoSoundsSwitchThisFrame() const {
    return CTimer::GetTimeInMSPauseMode() > m_TimeToSwapSoundsMs;
}

// 0x4F2CB0
void CAETwinLoopSoundEntity::PlayTwinLoopSound(CVector pos, float volume, float speed, float rollOffFactor, float doppler, eSoundEnvironment flags) {
    assert(m_IsInUse && "Must initialize sound first using `Initialise()`");

    StopSoundAndForget();
    for (int32 i = 0; i < NUM_SOUNDS; i++) {
        assert(!m_Sounds[i]);
        m_Sounds[i] = AESoundManager.PlaySound({
            .BankSlot      = m_BankSlot,
            .SoundID       = m_SoundIDs[i],
            .AudioEntity   = this,
            .Pos           = pos,
            .Volume        = i == 0 ? volume : -100.f,
            .RollOffFactor = rollOffFactor,
            .Speed         = speed,
            .Doppler       = doppler,
            .Flags         = (eSoundEnvironment)(flags | SOUND_START_PERCENTAGE | SOUND_REQUEST_UPDATES),
            .PlayTime      = m_SoundPlayPercentages[i] != -1
                ? m_SoundPlayPercentages[i]
                : CAEAudioUtility::GetRandomNumberInRange<int16>(0, 99),
        });
    }
    m_IsInUse             = true;
    m_IsPlayingFirstSound = true;
    ReCalculateSwapTime();
}

void CAETwinLoopSoundEntity::ReCalculateSwapTime() {
    m_TimeToSwapSoundsMs = CTimer::GetTimeInMSPauseMode() + CAEAudioUtility::GetRandomNumberInRange(m_SwapTimeMin, m_SwapTimeMax);
}

void CAETwinLoopSoundEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAETwinLoopSoundEntity, 0x85F438, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Initialise, 0x4F28A0);
    RH_ScopedInstall(UpdateTwinLoopSound, 0x4F29A0);
    RH_ScopedVMTInstall(UpdateParameters, 0x4F2E90);
    RH_ScopedInstall(SwapSounds, 0x4F2C10);
    RH_ScopedInstall(StopSoundAndForget, 0x4F2960);
    RH_ScopedInstall(PlayTwinLoopSound, 0x4F2CB0);
    RH_ScopedInstall(DoSoundsSwitchThisFrame, 0x4F2CA0);
}
