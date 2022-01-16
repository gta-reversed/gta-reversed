/*
Plugin-SDK (Grand Theft Auto San Andreas) source file
Authors: GTA Community. See more here
https://github.com/DK22Pac/plugin-sdk
Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "AESound.h"

#include "AEAudioEnvironment.h"
#include "AEAudioHardware.h"
#include "AEAudioUtility.h"

void CAESound::InjectHooks() {
    ReversibleHooks::Install("CAESound", "operator=", 0x4EF680, &CAESound::operator=);
    ReversibleHooks::Install("CAESound", "UnregisterWithPhysicalEntity", 0x4EF1A0, &CAESound::UnregisterWithPhysicalEntity);
    ReversibleHooks::Install("CAESound", "StopSound", 0x4EF1C0, &CAESound::StopSound);
    ReversibleHooks::Install("CAESound", "SetIndividualEnvironment", 0x4EF2B0, &CAESound::SetIndividualEnvironment);
    ReversibleHooks::Install("CAESound", "UpdatePlayTime", 0x4EF2E0, &CAESound::UpdatePlayTime);
    ReversibleHooks::Install("CAESound", "GetRelativePosition", 0x4EF350, &CAESound::GetRelativePosition);
    ReversibleHooks::Install("CAESound", "CalculateFrequency", 0x4EF390, &CAESound::CalculateFrequency);
    ReversibleHooks::Install("CAESound", "UpdateFrequency", 0x4EF3E0, &CAESound::UpdateFrequency);
    ReversibleHooks::Install("CAESound", "GetRelativePlaybackFrequencyWithDoppler", 0x4EF400, &CAESound::GetRelativePlaybackFrequencyWithDoppler);
    ReversibleHooks::Install("CAESound", "GetSlowMoFrequencyScalingFactor", 0x4EF440, &CAESound::GetSlowMoFrequencyScalingFactor);
    ReversibleHooks::Install("CAESound", "NewVPSLentry", 0x4EF7A0, &CAESound::NewVPSLentry);
    ReversibleHooks::Install("CAESound", "RegisterWithPhysicalEntity", 0x4EF820, &CAESound::RegisterWithPhysicalEntity);
    ReversibleHooks::Install("CAESound", "StopSoundAndForget", 0x4EF850, &CAESound::StopSoundAndForget);
    ReversibleHooks::Install("CAESound", "SetPosition", 0x4EF880, &CAESound::SetPosition);
    ReversibleHooks::Install("CAESound", "CalculateVolume", 0x4EFA10, &CAESound::CalculateVolume);
    ReversibleHooks::Install("CAESound", "Initialise", 0x4EFE50, &CAESound::Initialise);
    ReversibleHooks::Install("CAESound", "UpdateParameters", 0x4EFF50, &CAESound::UpdateParameters);
    ReversibleHooks::Install("CAESound", "SoundHasFinished", 0x4EFFD0, &CAESound::SoundHasFinished);
}

CAESound::CAESound(CAESound& sound) {
    m_nBankSlotId = sound.m_nBankSlotId;
    m_nSoundIdInSlot = sound.m_nSoundIdInSlot;
    m_pBaseAudio = sound.m_pBaseAudio;
    m_nEvent = sound.m_nEvent;
    m_fMaxVolume = sound.m_fMaxVolume;
    m_fVolume = sound.m_fVolume;
    m_fSoundDistance = sound.m_fSoundDistance;
    m_fSpeed = sound.m_fSpeed;
    m_fSpeedVariability = sound.m_fSpeedVariability;
    m_vecCurrPosn = sound.m_vecCurrPosn;
    m_vecPrevPosn = sound.m_vecPrevPosn;
    m_nLastFrameUpdate = sound.m_nLastFrameUpdate;
    m_nCurrTimeUpdate = sound.m_nCurrTimeUpdate;
    m_nPrevTimeUpdate = sound.m_nPrevTimeUpdate;
    m_fCurrCamDist = sound.m_fCurrCamDist;
    m_fPrevCamDist = sound.m_fPrevCamDist;
    m_fTimeScale = sound.m_fTimeScale;
    m_nIgnoredServiceCycles = sound.m_nIgnoredServiceCycles;
    m_nEnvironmentFlags = sound.m_nEnvironmentFlags;
    m_nIsUsed = sound.m_nIsUsed;
    m_nCurrentPlayPosition = sound.m_nCurrentPlayPosition;
    m_nHasStarted = sound.m_nHasStarted;
    m_fFinalVolume = sound.m_fFinalVolume;
    m_fFrequency = sound.m_fFrequency;
    m_nPlayingState = sound.m_nPlayingState;
    m_fSoundHeadRoom = sound.m_fSoundHeadRoom;
    m_nSoundLength = sound.m_nSoundLength;
    m_pPhysicalEntity = nullptr;

    if (sound.m_pPhysicalEntity) {
        m_pPhysicalEntity = sound.m_pPhysicalEntity;
        m_pPhysicalEntity->RegisterReference(&m_pPhysicalEntity);
    }
}

CAESound::CAESound(int16 bankSlotId, int16 sfxId, CAEAudioEntity* baseAudio, CVector posn, float volume, float fDistance, float speed, float timeScale,
                   uint8 ignoredServiceCycles, eSoundEnvironment environmentFlags, float speedVariability) {
    m_nBankSlotId = bankSlotId;
    m_nSoundIdInSlot = sfxId;
    m_pBaseAudio = baseAudio;
    m_vecPrevPosn.x = 0.0;
    m_vecPrevPosn.y = 0.0;
    m_vecPrevPosn.z = 0.0;
    m_pPhysicalEntity = nullptr;
    m_fMaxVolume = -1.0F;
    m_nEvent = -1;
    m_nLastFrameUpdate = 0;

    SetPosition(posn);

    m_fVolume = volume;
    m_fSoundDistance = fDistance;
    m_fSpeed = speed;
    m_fTimeScale = timeScale;
    m_nIgnoredServiceCycles = ignoredServiceCycles;
    m_nEnvironmentFlags = environmentFlags;
    m_nHasStarted = 0;
    m_nCurrentPlayPosition = 0;
    m_fSoundHeadRoom = 0.0F;
    m_fSpeedVariability = speedVariability;
    m_nIsUsed = 1;
    m_fFinalVolume = -100.0F;
    m_fFrequency = 1.0F;
    m_nSoundLength = -1;
}

CAESound::~CAESound() {
    UnregisterWithPhysicalEntity();
}

CAESound& CAESound::operator=(const CAESound& sound) {
    if (this == &sound)
        return *this;

    CAESound::UnregisterWithPhysicalEntity();
    m_nBankSlotId           = sound.m_nBankSlotId;
    m_nSoundIdInSlot        = sound.m_nSoundIdInSlot;
    m_pBaseAudio            = sound.m_pBaseAudio;
    m_nEvent                = sound.m_nEvent;
    m_fMaxVolume            = sound.m_fMaxVolume;
    m_fVolume               = sound.m_fVolume;
    m_fSoundDistance        = sound.m_fSoundDistance;
    m_fSpeed                = sound.m_fSpeed;
    m_fSpeedVariability     = sound.m_fSpeedVariability;
    m_vecCurrPosn           = sound.m_vecCurrPosn;
    m_vecPrevPosn           = sound.m_vecPrevPosn;
    m_nLastFrameUpdate      = sound.m_nLastFrameUpdate;
    m_nCurrTimeUpdate       = sound.m_nCurrTimeUpdate;
    m_nPrevTimeUpdate       = sound.m_nPrevTimeUpdate;
    m_fCurrCamDist          = sound.m_fCurrCamDist;
    m_fPrevCamDist          = sound.m_fPrevCamDist;
    m_fTimeScale            = sound.m_fTimeScale;
    m_nIgnoredServiceCycles = sound.m_nIgnoredServiceCycles;
    m_nEnvironmentFlags     = sound.m_nEnvironmentFlags;
    m_nIsUsed               = sound.m_nIsUsed;
    m_nCurrentPlayPosition  = sound.m_nCurrentPlayPosition;
    m_nHasStarted           = sound.m_nHasStarted;
    m_fFinalVolume          = sound.m_fFinalVolume;
    m_fFrequency            = sound.m_fFrequency;
    m_nPlayingState         = sound.m_nPlayingState;
    m_fSoundHeadRoom        = sound.m_fSoundHeadRoom;
    m_nSoundLength          = sound.m_nSoundLength;
    m_pPhysicalEntity       = nullptr;
    RegisterWithPhysicalEntity(sound.m_pPhysicalEntity);

    return *this;
}

void CAESound::UnregisterWithPhysicalEntity() {
    if (!m_pPhysicalEntity)
        return;

    m_pPhysicalEntity->CleanUpOldReference(&m_pPhysicalEntity);
    m_pPhysicalEntity = nullptr;
}

void CAESound::StopSound() {
    m_nPlayingState = eSoundState::SOUND_STOPPED;
    UnregisterWithPhysicalEntity();
}

void CAESound::SetIndividualEnvironment(uint16 envFlag, uint16 bEnabled) {
    if (bEnabled)
        m_nEnvironmentFlags |= envFlag;
    else
        m_nEnvironmentFlags &= ~envFlag;
}

// 0x4EF2E0
void CAESound::UpdatePlayTime(int16 soundLength, int16 loopStartTime, int16 playProgress) {
    m_nSoundLength = soundLength;
    if (m_nHasStarted)
        return;

    if (m_nPlayingState != eSoundState::SOUND_ACTIVE) {
        m_nCurrentPlayPosition = -1;
        return;
    }

    m_nCurrentPlayPosition += static_cast<int16>(static_cast<float>(playProgress) * m_fFrequency);
    if (m_nCurrentPlayPosition < soundLength)
        return;

    if (loopStartTime == -1) {
        m_nCurrentPlayPosition = -1;
        return;
    }

    m_nCurrentPlayPosition = loopStartTime + (m_nCurrentPlayPosition % soundLength);
}

void CAESound::GetRelativePosition(CVector* outPosn) {
    if (!GetFrontEnd())
        return CAEAudioEnvironment::GetPositionRelativeToCamera(outPosn, &m_vecCurrPosn);

    *outPosn = m_vecCurrPosn;
}

void CAESound::CalculateFrequency() {
    if (m_fSpeedVariability <= 0.0F || m_fSpeedVariability >= m_fSpeed)
        m_fFrequency = m_fSpeed;
    else
        m_fFrequency = m_fSpeed + CAEAudioUtility::GetRandomNumberInRange(-m_fSpeedVariability, m_fSpeedVariability);
}

void CAESound::UpdateFrequency() {
    if (m_fSpeedVariability == 0.0F)
        m_fFrequency = m_fSpeed;
}

float CAESound::GetRelativePlaybackFrequencyWithDoppler() {
    if (GetFrontEnd())
        return m_fFrequency;

    return m_fFrequency * CAEAudioEnvironment::GetDopplerRelativeFrequency(m_fPrevCamDist, m_fCurrCamDist, m_nPrevTimeUpdate, m_nCurrTimeUpdate, m_fTimeScale);
}

float CAESound::GetSlowMoFrequencyScalingFactor() {
    if (GetUnpausable() || !CTimer::GetIsSlowMotionActive() || CCamera::GetActiveCamera().m_nMode == eCamMode::MODE_CAMERA) {
        return 1.0F;
    }

    return fSlowMoFrequencyScalingFactor;
}

void CAESound::NewVPSLentry() {
    m_nHasStarted = 0;
    m_nPlayingState = eSoundState::SOUND_ACTIVE;
    m_bWasServiced = 0;
    m_nIsUsed = 1;
    m_fSoundHeadRoom = AEAudioHardware.GetSoundHeadroom(m_nSoundIdInSlot, m_nBankSlotId);
    CalculateFrequency();
}

void CAESound::RegisterWithPhysicalEntity(CEntity* entity) {
    CAESound::UnregisterWithPhysicalEntity();
    if (entity) {
        m_pPhysicalEntity = entity;
        entity->RegisterReference(&m_pPhysicalEntity);
    }
}

void CAESound::StopSoundAndForget() {
    m_bRequestUpdates = false;
    m_pBaseAudio = nullptr;
    StopSound();
}

void CAESound::SetPosition(CVector vecPos) {
    if (!m_nLastFrameUpdate) {
        m_vecPrevPosn = vecPos;
        m_vecCurrPosn = vecPos;

        auto const fCamDist = DistanceBetweenPoints(vecPos, TheCamera.GetPosition());
        m_fCurrCamDist = fCamDist;
        m_fPrevCamDist = fCamDist;

        m_nLastFrameUpdate = CTimer::GetFrameCounter();
        m_nCurrTimeUpdate = CTimer::GetTimeInMS();
        m_nPrevTimeUpdate = CTimer::GetTimeInMS();
    } else if (m_nLastFrameUpdate == CTimer::GetFrameCounter()) {
        m_vecCurrPosn = vecPos;
        m_fCurrCamDist = DistanceBetweenPoints(vecPos, TheCamera.GetPosition());
        m_nCurrTimeUpdate = CTimer::GetTimeInMS();
    } else {
        m_vecPrevPosn = m_vecCurrPosn;
        m_fPrevCamDist = m_fCurrCamDist;
        m_nPrevTimeUpdate = m_nCurrTimeUpdate;

        m_vecCurrPosn = vecPos;
        m_fCurrCamDist = DistanceBetweenPoints(vecPos, TheCamera.GetPosition());
        m_nCurrTimeUpdate = CTimer::GetTimeInMS();
        m_nLastFrameUpdate = CTimer::GetFrameCounter();
    }
}

void CAESound::CalculateVolume() {
    if (GetFrontEnd())
        m_fFinalVolume = m_fVolume - m_fSoundHeadRoom;
    else {
        CVector relativePos;
        CAEAudioEnvironment::GetPositionRelativeToCamera(&relativePos, &m_vecCurrPosn);
        const auto fDist = relativePos.Magnitude() / m_fSoundDistance;
        const auto fAttenuation = CAEAudioEnvironment::GetDistanceAttenuation(fDist);
        m_fFinalVolume = CAEAudioEnvironment::GetDirectionalMikeAttenuation(relativePos) + fAttenuation + m_fVolume - m_fSoundHeadRoom;
    }
}

void CAESound::Initialise(int16 bankSlotId, int16 sfxId, CAEAudioEntity* baseAudio, CVector posn, float volume, float maxDistance, float speed, float timeScale,
                          uint8 ignoredServiceCycles, eSoundEnvironment environmentFlags, float speedVariability, int16 currPlayPosn)
{
    UnregisterWithPhysicalEntity();

    m_nSoundIdInSlot        = sfxId;
    m_nBankSlotId           = bankSlotId;
    m_pBaseAudio            = baseAudio;
    m_fVolume               = volume;
    m_fSoundDistance        = maxDistance;
    m_fSpeed                = speed;
    m_fSpeedVariability     = speedVariability;
    m_vecPrevPosn           .Set(0.0F, 0.0F, 0.0F);
    m_nEvent                = -1;
    m_fMaxVolume            = -1.0F;
    m_nLastFrameUpdate      = 0;

    SetPosition(posn);

    m_fTimeScale            = timeScale;
    m_nSoundLength          = -1;
    m_nHasStarted           = 0;
    m_nPlayingState         = eSoundState::SOUND_ACTIVE;
    m_fSoundHeadRoom        = 0.0F;
    m_nIgnoredServiceCycles = ignoredServiceCycles;
    m_nEnvironmentFlags     = environmentFlags;
    m_nIsUsed               = 1;
    m_nCurrentPlayPosition  = currPlayPosn;
    m_fFinalVolume          = -100.0F;
    m_fFrequency            = 1.0F;
}

void CAESound::UpdateParameters(int16 curPlayPos) {
    if (GetLifespanTiedToPhysicalEntity()) {
        if (!m_pPhysicalEntity)
            m_nPlayingState = eSoundState::SOUND_STOPPED;
        else
            SetPosition(m_pPhysicalEntity->GetPosition());
    }

    if (GetRequestUpdates() && m_pBaseAudio) {
        m_pBaseAudio->UpdateParameters(this, curPlayPos);
        if (m_fSpeedVariability == 0.0F)
            m_fFrequency = m_fSpeed;
    }
}

void CAESound::SoundHasFinished() {
    UpdateParameters(-1);
    UnregisterWithPhysicalEntity();
    m_nIsUsed = 0;
    m_nHasStarted = 0;
    m_nCurrentPlayPosition = 0;
}
