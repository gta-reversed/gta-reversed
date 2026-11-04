#pragma once

#include "AEAudioEntity.h"

class NOTSA_EXPORT_VTABLE CAEDoorAudioEntity : public CAEAudioEntity {
    uint32 m_nTime;
    uint32 m_nMovingDoorTime;
    uint32 m_nField84;

public:
    CAEDoorAudioEntity();

    static void StaticInitialise();
    void Reset();

    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;

    void AddAudioEvent(eAudioEvents event, CPhysical* physical);
    void AddAudioEvent(eAudioEvents event, CVector& posn, float volume = 0.0f, float speed = 1.0f);
    void PlayDoorSound(int16 sfxId, eAudioEvents event, CVector& posn, float volumeDelta = 0.0f, float speed = 1.0f);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

};

VALIDATE_SIZE(CAEDoorAudioEntity, 0x88);
