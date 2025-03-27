#pragma once

#include "AEAudioEntity.h"

class CVector;
class CWaterCannon;

class NOTSA_EXPORT_VTABLE CAEWaterCannonAudioEntity : public CAEAudioEntity {
public:
    bool      m_bInitialized;
    bool      m_bSplashInfoEnabled;
    CVector   m_vecPosn;
    float     m_fSplashMagnitude;
    CAESound* m_Sounds[4];

public:
    CAEWaterCannonAudioEntity();
    ~CAEWaterCannonAudioEntity();

    void Initialise(CWaterCannon* waterCannon);
    void Terminate();
    static void StaticInitialise();

    void AddAudioEvent(eAudioEvents event, CVector& posn);
    void SetSplashInfo(CVector posn, float magnitude);
    void ClearSplashInfo();
    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;
    void UpdateGenericWaterCannonSound(bool splashInfoEnabled, int16 id, int16 bankSlotId, int16 sfxId, float speed, float volume, CVector posn, float soundDistance);
    void Service();

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CAEWaterCannonAudioEntity* Constructor();

};

VALIDATE_SIZE(CAEWaterCannonAudioEntity, 0xA0);
