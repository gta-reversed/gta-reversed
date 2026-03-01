/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

/* This class is used to control 2 sounds as one. There are 2 sounds
   created with `PlayTwinLoopSound()`, and one of them is playing when second
   is simply muted. When time m_TimeToSwapSoundsMs is reached, these sounds
   are switched: second starts playing, first begins being muted.
   The time m_TimeToSwapSoundsMs is calculated as:
   `CAEAudioUtility::GetRandomNumberInRange(m_SwapTimeMin, m_SwapTimeMax)`
   Notice these sounds must be situated in same bank slot.
   `m_AudioEntity` is a pointer to audio which created twin sound. For example,
   this could be CAEVehicleAudioEntity for playing skid sounds.              */

#include "AEAudioEntity.h"

#include <Enums/SoundIDs.h>
#include <Enums/eSoundBankSlot.h>

class NOTSA_EXPORT_VTABLE CAETwinLoopSoundEntity : public CAEAudioEntity {
    static constexpr uint32 NUM_SOUNDS = 2;
public:
    CAETwinLoopSoundEntity() = default;
    CAETwinLoopSoundEntity(
        eSoundBankSlot  bank,
        eSoundID        soundA,
        eSoundID        soundB,
        CAEAudioEntity* parent,
        uint16          swapTimeMin,
        uint16          swapTimeMax,
        int16           playPercentageA,
        int16           playPercentageB
    );

    ~CAETwinLoopSoundEntity();

    void Initialise(
        eSoundBankSlot  bank,
        eSoundID        soundA,
        eSoundID        soundB,
        CAEAudioEntity* parent,
        uint16          swapTimeMin,
        uint16          swapTimeMax,
        int16           playPercentageA = -1,
        int16           playPercentageB = -1
    );

    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;

    void UpdateTwinLoopSound(CVector pos, float volume, float speed);
    void SwapSounds();
    void StopSound();
    void StopSoundAndForget();

    float GetEmittedVolume() const;
    void  SetEmittedVolume(float volume) const;

    bool IsTwinLoopPlaying() const;
    bool DoSoundsSwitchThisFrame() const;

    void PlayTwinLoopSound(CVector pos, float volume, float speed, float rollOffFactor, float doppler = 1.f, eSoundEnvironment flags = SOUND_DEFAULT);

    bool IsActive() const noexcept { return m_IsInUse; }
private:
    void ReCalculateSwapTime();

private:
    friend void InjectHooksMain();
    static void InjectHooks();

private:
    eSoundBankSlotS16 m_BankSlot{};
    eSoundID          m_SoundIDs[NUM_SOUNDS]{};
    CAEAudioEntity*   m_AudioEntity{};
    int16             m_IsInUse{};
    int16             m_SoundsLengths[NUM_SOUNDS]{};
    uint16            m_SwapTimeMin{};
    uint16            m_SwapTimeMax{};
    uint32            m_TimeToSwapSoundsMs{};
    bool              m_IsPlayingFirstSound{};
    int16             m_SoundPlayPercentages[NUM_SOUNDS]{}; // 0 - 99%
    CAESound*         m_Sounds[NUM_SOUNDS]{};
};
VALIDATE_SIZE(CAETwinLoopSoundEntity, 0xA8);
