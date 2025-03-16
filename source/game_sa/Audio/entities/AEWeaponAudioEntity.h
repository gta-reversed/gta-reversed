/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "AEAudioEntity.h"
#include "AESound.h"
#include <Audio/Enums/SoundIDs.h>

#include "eWeaponType.h"

class CPhysical;
class CPed;

class NOTSA_EXPORT_VTABLE CAEWeaponAudioEntity : public CAEAudioEntity {
    enum class eMiniGunState : int8 {
        SPINNING = 0x0,
        FIRING = 0x1,
        STOPPING = 0x2,
        STOPPED = 0x3,
    };

    enum eChainsawState : int8 {
        IDLE = 0x0,
        ACTIVE = 0x1,
        CUTTING = 0x2,
        STOPPING = 0x3,
        STOPPED = 0x4,
    };

    enum eWeaponSoundCategories {
        AE_WEAPON_SOUND_CAT_TAIL              = 1,  // 0x1
        AE_WEAPON_SOUND_CAT_FLAME             = 2,  // 0x2
        AE_WEAPON_SOUND_CAT_SPRAY             = 3,  // 0x3
        AE_WEAPON_SOUND_CAT_EXT               = 4,  // 0x4
        AE_WEAPON_SOUND_CAT_MINIGUN_SPIN      = 5,  // 0x5
        AE_WEAPON_SOUND_CAT_MINIGUN_FIRE      = 6,  // 0x6
        AE_WEAPON_SOUND_CAT_MINIGUN_TAIL      = 7,  // 0x7
        AE_WEAPON_SOUND_CAT_MINIGUN_STOP      = 8,  // 0x8
        AE_WEAPON_SOUND_CAT_CHAINSAW_IDLE     = 9,  // 0x9
        AE_WEAPON_SOUND_CAT_CHAINSAW_ACTIVE   = 10, // 0xA
        AE_WEAPON_SOUND_CAT_CHAINSAW_CUTTING  = 11, // 0xB
        AE_WEAPON_SOUND_CAT_CHAINSAW_STOP     = 12, // 0xC
        AE_WEAPON_SOUND_CAT_STEALTH_KNIFE_IN  = 13, // 0xD
        AE_WEAPON_SOUND_CAT_STEALTH_KNIFE_OUT = 14, // 0xE
    };

public:
    CAEWeaponAudioEntity() = default;
    ~CAEWeaponAudioEntity() = default;

    void Initialise();
    void Terminate();
    void Reset();

    void WeaponFire(eWeaponType type, CPhysical* entity, eAudioEvents audioEventId);
    void WeaponReload(eWeaponType type, CPhysical* entity, eAudioEvents event);

    void PlayGunSounds(
        CPhysical*   entity,
        int16        dryFireSfxID,
        int16        subSfxID,
        int16        mainLeftSfxID,
        int16        mainRightSfxID,
        int16        tailSfxID,
        eAudioEvents audioEvent,
        float        volumeOffsetDecibels = 0.f,
        float        primaryFrequencyScalingFactor = 1.f,
        float        tailFrequencyScalingFactor = 1.f
    );
    void PlayGoggleSound(int16 sfxId, eAudioEvents event);

    void ReportStealthKill(eWeaponType type, CPhysical* entity, eAudioEvents event);

    void ReportChainsawEvent(CPhysical* entity, eAudioEvents audioEvent);
    void PlayChainsawStopSound(CPhysical* entity);

    void PlayWeaponLoopSound(CPhysical* entity, int16 sfxId, eAudioEvents startEvent, float audability, float speed, eWeaponSoundCategories endEvent);

    void PlayMiniGunFireSounds(CPhysical* entity, eAudioEvents audioEventId);
    void PlayMiniGunStopSound(CPhysical* entity);

    void PlayFlameThrowerSounds(CPhysical* entity, int16 sfx1, int16 sfx2, eAudioEvents audioEventId, float audability, float speed);
    void PlayFlameThrowerIdleGasLoop(CPhysical* entity);
    void StopFlameThrowerIdleGasLoop();

    void PlayCameraSound(CPhysical* entity, eAudioEvents event, float audability);

    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;

    void Clear();

private:
    bool           m_IsMiniGunSpinActive{};
    bool           m_IsMiniGunFireActive{};
    uint8          m_LastWeaponPlaneFrequencyIndex{};
    eMiniGunState  m_MiniGunState{eMiniGunState::SPINNING};
    eChainsawState m_ChainsawState{eChainsawState::IDLE};
    uint32         m_LastFlameThrowerFireTimeMs{};
    uint32         m_LastSprayCanFireTimeMs{};
    uint32         m_LastFireExtFireTimeMs{};
    uint32         m_LastMiniGunFireTimeMs{};
    uint32         m_LastChainsawEventTimeMs{};
    uint32         m_LastGunFireTimeMs{};
    CAESound*      m_FlameThrowerIdleGasLoopSound{};

private: // Hook stuff
    friend void InjectHooksMain();
    static void InjectHooks();

    CAEWeaponAudioEntity* Constructor();
    CAEWeaponAudioEntity* Destructor();

};
VALIDATE_SIZE(CAEWeaponAudioEntity, 0xA0);
