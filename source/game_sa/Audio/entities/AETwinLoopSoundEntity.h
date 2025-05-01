/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

/* This class is used to control 2 sounds as one. There are 2 sounds
   created with PlayTwinLoopSound(), and one of them is playing when second
   is simply muted. When time m_nTimeToSwapSounds is reached, these sounds
   are switched: second starts playing, first begins being muted.
   The time m_nTimeToSwapSounds is calculated as:
   CAEAudioUtility::GetRandomNumberInRange(m_nPlayTimeMin, m_nPlayTimeMax);
   Notice these sounds must be situated in same bank slot.
   m_pBaseAudio is a pointer to audio which created twin sound. For example,
   this could be CAEVehicleAudioEntity for playing skid sounds.              */

#include "AEAudioEntity.h"

class NOTSA_EXPORT_VTABLE CAETwinLoopSoundEntity : public CAEAudioEntity {
public:
    int16           m_nBankSlotId;
    int16           m_nSoundId1;
    int16           m_nSoundId2;
    CAEAudioEntity* m_pBaseAudio;
    int16           m_bIsInitialised;
    int16           unused_field_8A;
    int16           unused_field_8C;
    uint16          m_nPlayTimeMin;
    uint16          m_nPlayTimeMax;
    uint32          m_nTimeToSwapSounds;
    bool            m_bPlayingFirstSound;
    int16           m_nSoundPlayStart1; // 0 - 99 percentage
    int16           m_nSoundPlayStart2; // 0 - 99 percentage
    CAESound*       m_pSound1;
    CAESound*       m_pSound2;

public:
    CAETwinLoopSoundEntity();
    CAETwinLoopSoundEntity(int16 bank, int16 soundType1, int16 soundType2, CAEAudioEntity* audio, uint16 minTime, uint16 maxTime, int16 sfxPlayStart1, int16 sfxPlayStart2);

    ~CAETwinLoopSoundEntity();

    void Initialise(int16 bank, int16 sfx1, int16 sfx2, CAEAudioEntity* audio, uint16 minTime, uint16 maxTime, int16 sfxPlayStart1, int16 sfxPlayStart2);

    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;

    void UpdateTwinLoopSound(CVector posn, float volume, float speed);
    void SwapSounds();
    void StopSound();
    void StopSoundAndForget();

    float GetEmittedVolume();
    void  SetEmittedVolume(float volume);

    bool IsTwinLoopPlaying();
    bool DoSoundsSwitchThisFrame();

    void PlayTwinLoopSound(CVector posn, float volume, float speed, float maxDistance, float timeScale, eSoundEnvironment flags);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

};

VALIDATE_SIZE(CAETwinLoopSoundEntity, 0xA8);
