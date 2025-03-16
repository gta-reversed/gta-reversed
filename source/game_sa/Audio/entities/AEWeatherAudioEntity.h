/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "AEAudioEntity.h"

class NOTSA_EXPORT_VTABLE CAEWeatherAudioEntity : public CAEAudioEntity {
public:
    uint8 m_nThunderFrequencyVariationCounter;

    static float& m_sfRainVolume;

public:
    CAEWeatherAudioEntity();
    ~CAEWeatherAudioEntity() = default; // 0x72A400

    static void StaticInitialise();
    static void StaticReset();

    void AddAudioEvent(eAudioEvents event);
    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;
    void Service();

private:
    friend void InjectHooksMain();
    static void InjectHooks();

};
VALIDATE_SIZE(CAEWeatherAudioEntity, 0x80);
