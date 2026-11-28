#pragma once

#include "AEAudioEntity.h"
#include "AETwinLoopSoundEntity.h"

class NOTSA_EXPORT_VTABLE CAEFrontendAudioEntity : public CAEAudioEntity {
public:
    void Initialise();
    void Reset();

    void AddAudioEvent(eAudioEvents event, float volumeBoost = 0.0f, float speed = 1.0f);

    bool IsRadioTuneSoundActive();
    bool IsLoadingTuneActive();

    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;

    static void InjectHooks();

private:
    bool                   m_IsAmplifierWakeUpPlaying{};
    uint8                  m_LoadingTuneSeed{};
    int16                  m_Unused7E{};
    int16                  m_BulletPassCount{};
    eSoundBank             m_BulletPassBank{};
    uint32                 m_LastFrameGeneral{};
    uint32                 m_LastFrameMissionComplete{};
    uint32                 m_LastFrameBulletPass{};
    uint32                 m_LastTimeCarRespray{};
    uint32                 m_LastTimeTimerCount{};
    CAESound*              m_AmplifierWakeUpSound{};
    CAETwinLoopSoundEntity m_RetuneSound{};
    CAETwinLoopSoundEntity m_RetunePausedSound{};
};

VALIDATE_SIZE(CAEFrontendAudioEntity, 0x1EC);
