#pragma once

#include "AEAudioEntity.h"
#include "AETwinLoopSoundEntity.h"

class NOTSA_EXPORT_VTABLE CAEFrontendAudioEntity : public CAEAudioEntity {
protected:
    bool                   m_bAmplifierWakeUp;
    uint8                  m_nbLoadingTuneSeed;
    int16                  m_f7E; // unused
    int16                  m_nNumBulletSounds;
    int16                  m_nBankId;
    uint32                 m_nLastFrameGeneral_or_nFrameCount;
    uint32                 m_nLastFrameMissionComplete;
    uint32                 m_nLastFrameBulletPass;
    uint32                 m_nLastTimeCarRespray;
    uint32                 m_nLatestTimerCount;
    CAESound*              m_pAmplifierWakeUp;
    CAETwinLoopSoundEntity m_objRetune;
    CAETwinLoopSoundEntity m_objRetunePaused;

public:
    void Initialise();
    void Reset();

    void AddAudioEvent(eAudioEvents event, float fVolumeBoost = 0.0f, float fSpeed = 1.0f);

    bool IsRadioTuneSoundActive();
    bool IsLoadingTuneActive();

    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

};

VALIDATE_SIZE(CAEFrontendAudioEntity, 0x1EC);
