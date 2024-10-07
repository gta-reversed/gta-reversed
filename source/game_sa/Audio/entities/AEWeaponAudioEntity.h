/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "AEAudioEntity.h"
#include "AESound.h"

#include "eWeaponType.h"

class CPhysical;
class CPed;

class NOTSA_EXPORT_VTABLE CAEWeaponAudioEntity : public CAEAudioEntity {
public:
    bool       m_bPlayedMiniGunFireSound;
    bool       field_7D; // see PlayMiniGunFireSounds
    uint8      field_7E; // see PlayGunSounds, gfWeaponPlaneFrequencyVariations
    int8       m_nState;
    uint8      m_nChainsawSoundState;
    uint32     m_nFlameThrowerLastPlayedTime;
    uint32     m_nSpraycanLastPlayedTime;
    uint32     m_nExtinguisherLastPlayedTime;
    uint32     m_nMiniGunFireSoundPlayedTime;
    uint32     m_nTimeChainsaw;
    uint32     m_nTimeLastFired;
    CAESound*  m_FlameThrowerSound;

public:
    CAEWeaponAudioEntity();
    ~CAEWeaponAudioEntity() = default;

    void Initialise();
    void Terminate();
    void Reset();

    void WeaponFire(eWeaponType type, CPhysical* entity, eAudioEvents audioEventId);
    void WeaponReload(eWeaponType type, CPhysical* entity, eAudioEvents event);

    void PlayGunSounds(CPhysical* entity, int16 emptySfxId, int16 farSfxId2, int16 highPitchSfxId3, int16 lowPitchSfxId4, int16 echoSfxId5, int32 audioEventId, float volumeChange, float speed1, float speed2);
    void PlayGoggleSound(int16 sfxId, eAudioEvents event);

    void ReportStealthKill(eWeaponType type, CPhysical* entity, eAudioEvents event);

    void ReportChainsawEvent(CPhysical* entity, int32 audioEventId);
    void PlayChainsawStopSound(CPhysical* entity);

    void PlayWeaponLoopSound(CPhysical* entity, int16 sfxId, eAudioEvents startEvent, float audability, float speed, eAudioEvents endEvent);

    void PlayMiniGunFireSounds(CPhysical* entity, int32 audioEventId);
    void PlayMiniGunStopSound(CPhysical* entity);

    void PlayFlameThrowerSounds(CPhysical* entity, int16 sfx1, int16 sfx2, int32 audioEventId, float audability, float speed);
    void PlayFlameThrowerIdleGasLoop(CPhysical* entity);
    void StopFlameThrowerIdleGasLoop();

    void PlayCameraSound(CPhysical* entity, eAudioEvents event, float audability);

    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;

    void Clear() {
        m_nFlameThrowerLastPlayedTime = 0;
        m_nSpraycanLastPlayedTime     = 0;
        m_nExtinguisherLastPlayedTime = 0;
        m_FlameThrowerSound           = nullptr;
        field_7E                      = 0;
        m_nMiniGunFireSoundPlayedTime = 0;
        m_bPlayedMiniGunFireSound     = false;
        field_7D                      = false;
        m_nTimeChainsaw               = 0;
        m_nTimeLastFired              = 0;
    }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CAEWeaponAudioEntity* Constructor();
    CAEWeaponAudioEntity* Destructor();

};
VALIDATE_SIZE(CAEWeaponAudioEntity, 0xA0);
