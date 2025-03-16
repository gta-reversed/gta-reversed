/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "AEAudioEntity.h"

class FxSystem_c;

class NOTSA_EXPORT_VTABLE CAEFireAudioEntity : public CAEAudioEntity {
public:
    CAESound*   m_SoundLeft;
    CAESound*   m_SoundRight;
    FxSystem_c* m_FxSystem;

    static uint8& m_snLastFireFrequencyIndex;

public:
    static void InjectHooks();

    void Initialise(FxSystem_c* system);
    static void StaticInitialise();
    void Terminate();

    void AddAudioEvent(eAudioEvents audioId, CVector& posn);
    void PlayFireSounds(eAudioEvents audioId, CVector& posn);
    void PlayWaterSounds(eAudioEvents audioId, CVector& posn);
    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;

};

VALIDATE_SIZE(CAEFireAudioEntity, 0x88);