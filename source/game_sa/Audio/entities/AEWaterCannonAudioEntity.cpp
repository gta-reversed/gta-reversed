#include "StdInc.h"

#include "AEWaterCannonAudioEntity.h"
#include "AEAudioHardware.h"
#include "WaterCannon.h"

// 0x728A90
CAEWaterCannonAudioEntity::CAEWaterCannonAudioEntity() : CAEAudioEntity() {
    m_bInitialized = false;
}

// 0x728AB0
CAEWaterCannonAudioEntity::~CAEWaterCannonAudioEntity() {
    Terminate();
}

// 0x503060
void CAEWaterCannonAudioEntity::Initialise(CWaterCannon* waterCannon) {
    m_pEntity = reinterpret_cast<CEntity*>(waterCannon); // bruh
    ClearSplashInfo();
    m_bInitialized = true;
}

// 0x5030A0
void CAEWaterCannonAudioEntity::Terminate() {
    for (auto& sound : m_Sounds) {
        if (sound) {
            sound->StopSound();
        }
    }
}

// ANDROID IDB 0x3D2F00
void CAEWaterCannonAudioEntity::StaticInitialise() {
    // NOP
}

// unused
// 0x
void CAEWaterCannonAudioEntity::AddAudioEvent(eAudioEvents event, CVector& posn) {
    // NOP
}

// 0x502F50
void CAEWaterCannonAudioEntity::SetSplashInfo(CVector posn, float magnitude) {
    m_vecPosn = posn;
    m_bSplashInfoEnabled = true;
    m_fSplashMagnitude = magnitude;
}

// 0x502F20
void CAEWaterCannonAudioEntity::ClearSplashInfo() {
    m_vecPosn.Set(0.0f, 0.0f, 0.0f);
    m_bSplashInfoEnabled = false;
    m_fSplashMagnitude = 0.0f;
}

// 0x502EF0
void CAEWaterCannonAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (curPlayPos == -1) {
        for (auto& tsound : m_Sounds) {
            if (tsound == sound) {
                tsound = nullptr;
            }
        }
    }
}

// 0x502F80
void CAEWaterCannonAudioEntity::UpdateGenericWaterCannonSound(bool splashInfoEnabled, int16 id, int16 bankSlotId, int16 sfxId, float speed, float volume, CVector posn, float soundDistance) {
    auto& sound = m_Sounds[id];
    if (splashInfoEnabled) {
        if (sound) {
            sound->m_fVolume = volume;
            sound->m_fSpeed = speed;
            sound->SetPosition(posn);
        } else {
            m_tempSound.Initialise(bankSlotId, sfxId, this, posn, volume, 1.0f, 1.0f, 1.0f, 0, SOUND_DEFAULT, 0.0f, 0);
            m_tempSound.m_fVolume = volume;
            m_tempSound.m_fSoundDistance = soundDistance;
            m_tempSound.m_fSpeed = speed;
            m_tempSound.m_nEnvironmentFlags = SOUND_REQUEST_UPDATES;
            sound = AESoundManager.RequestNewSound(&m_tempSound);
        }

        return;
    }

    if (sound) {
        sound->StopSound();
    }
}

// 0x5030D0
void CAEWaterCannonAudioEntity::Service() {
    const auto speed0 = +1.78f;
    const auto speed1 = +0.50f;
    const auto speed2 = +1.00f;
    const auto speed3 = speed2 / 2.0f;

    const auto volume0 = +6.0f;
    const auto volume1 = -9.0f;
    const auto volume2 = -3.0f;
    const auto volume3 = volume2 - 6.0f;

    if (!m_bInitialized)
        return;
    if (!AEAudioHardware.IsSoundBankLoaded(59, 0) || !AEAudioHardware.IsSoundBankLoaded(39, 2))
        return;

    bool enabled = false;
    auto* cannon = reinterpret_cast<CWaterCannon*>(m_pEntity);
    if (cannon->m_abUsed[cannon->m_nSectionsCount]) {
        if (cannon->m_nId) {
            enabled = true;
        }
    }

    const auto& sectionLastPoint = cannon->m_sectionPoint[cannon->m_nSectionsCount];
    UpdateGenericWaterCannonSound(enabled,              0, 0, 0, speed0, volume0, sectionLastPoint, 2.0f);
    UpdateGenericWaterCannonSound(enabled,              1, 2, 3, speed1, volume1, sectionLastPoint, 2.0f);
    UpdateGenericWaterCannonSound(m_bSplashInfoEnabled, 2, 2, 3, speed2, volume2, m_vecPosn,        2.0f);
    UpdateGenericWaterCannonSound(m_bSplashInfoEnabled, 3, 2, 3, speed3, volume3, m_vecPosn,        2.0f);
}

void CAEWaterCannonAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEWaterCannonAudioEntity, 0x872A60, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Constructor, 0x728A90);
    RH_ScopedInstall(Initialise, 0x503060);
    RH_ScopedInstall(Terminate, 0x5030A0);
    RH_ScopedInstall(Service, 0x5030D0);
    RH_ScopedInstall(SetSplashInfo, 0x502F50);
    RH_ScopedInstall(ClearSplashInfo, 0x502F20);
    RH_ScopedVMTInstall(UpdateParameters, 0x502EF0);
}

CAEWaterCannonAudioEntity* CAEWaterCannonAudioEntity::Constructor() {
    this->CAEWaterCannonAudioEntity::CAEWaterCannonAudioEntity();
    return this;
}