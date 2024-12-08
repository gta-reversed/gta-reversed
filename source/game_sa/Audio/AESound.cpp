/*
    Plugin-SDK file
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
    RH_ScopedClass(CAESound);
    RH_ScopedCategory("Audio");

    RH_ScopedInstall(operator=, 0x4EF680);
    RH_ScopedInstall(UnregisterWithPhysicalEntity, 0x4EF1A0);
    RH_ScopedInstall(StopSound, 0x4EF1C0);
    RH_ScopedInstall(SetIndividualEnvironment, 0x4EF2B0);
    RH_ScopedInstall(UpdatePlayTime, 0x4EF2E0);
    RH_ScopedOverloadedInstall(GetRelativePosition, "orginal", 0x4EF350, void(CAESound::*)(CVector*) const);
    RH_ScopedInstall(CalculateFrequency, 0x4EF390);
    RH_ScopedInstall(UpdateFrequency, 0x4EF3E0);
    RH_ScopedInstall(GetRelativePlaybackFrequencyWithDoppler, 0x4EF400);
    RH_ScopedInstall(GetSlowMoFrequencyScalingFactor, 0x4EF440);
    RH_ScopedInstall(NewVPSLentry, 0x4EF7A0);
    RH_ScopedInstall(RegisterWithPhysicalEntity, 0x4EF820);
    RH_ScopedInstall(StopSoundAndForget, 0x4EF850);
    RH_ScopedInstall(SetPosition, 0x4EF880);
    RH_ScopedInstall(CalculateVolume, 0x4EFA10);
    RH_ScopedInstall(Initialise, 0x4EFE50);
    RH_ScopedInstall(UpdateParameters, 0x4EFF50);
    RH_ScopedInstall(SoundHasFinished, 0x4EFFD0);
}

CAESound::CAESound(CAESound& sound) {
    m_nBankSlotId = sound.m_nBankSlotId;
    m_nSoundIdInSlot = sound.m_nSoundIdInSlot;
    m_pBaseAudio = sound.m_pBaseAudio;
    m_nEvent = sound.m_nEvent;
    m_ClientVariable = sound.m_ClientVariable;
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
    m_vecPrevPosn = CVector(0.0f, 0.0f, 0.0f);
    m_pPhysicalEntity = nullptr;
    m_ClientVariable = -1.0F;
    m_nEvent = AE_UNDEFINED;
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

// 0x4EF680
CAESound& CAESound::operator=(const CAESound& sound) {
    if (this == &sound)
        return *this;

    CAESound::UnregisterWithPhysicalEntity();
    m_nBankSlotId           = sound.m_nBankSlotId;
    m_nSoundIdInSlot        = sound.m_nSoundIdInSlot;
    m_pBaseAudio            = sound.m_pBaseAudio;
    m_nEvent                = sound.m_nEvent;
    m_ClientVariable            = sound.m_ClientVariable;
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

// 0x4EF1A0
void CAESound::UnregisterWithPhysicalEntity() {
    CEntity::ClearReference(m_pPhysicalEntity);
}

// 0x4EF1C0
void CAESound::StopSound() {
    m_nPlayingState = eSoundState::SOUND_STOPPED;
    UnregisterWithPhysicalEntity();
}

// 0x4EF2B0
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

    // Avoid division by 0
    // This seems to have been fixed the same way in Android
    // The cause is/can be missing audio files, but I'm lazy to fix it, so this is gonna be fine for now
    m_nCurrentPlayPosition = !notsa::IsFixBugs() || soundLength > 0
        ? loopStartTime + (m_nCurrentPlayPosition % soundLength)
        : loopStartTime;
}

// NOTSA: Simplified calling convention
CVector CAESound::GetRelativePosition() const {
    CVector outVec;
    GetRelativePosition(&outVec);
    return outVec;
}

// 0x4EF350 - Matches the original calling convention, to be used by reversible hooks, use the version returning CVector instead in our code
void CAESound::GetRelativePosition(CVector* outVec) const {
    *outVec = GetFrontEnd() ? m_vecCurrPosn : CAEAudioEnvironment::GetPositionRelativeToCamera(m_vecCurrPosn);
}

// 0x4EF390
void CAESound::CalculateFrequency() {
    m_fFrequency = m_fSpeedVariability <= 0.0F || m_fSpeedVariability >= m_fSpeed
        ? m_fSpeed
        : m_fSpeed + CAEAudioUtility::GetRandomNumberInRange(-m_fSpeedVariability, m_fSpeedVariability);
}

// 0x4EF3E0
void CAESound::UpdateFrequency() {
    if (m_fSpeedVariability == 0.0F) {
        m_fFrequency = m_fSpeed;
    }
}

// 0x4EF400
float CAESound::GetRelativePlaybackFrequencyWithDoppler() const {
    return GetFrontEnd()
        ? m_fFrequency
        : m_fFrequency * CAEAudioEnvironment::GetDopplerRelativeFrequency(m_fPrevCamDist, m_fCurrCamDist, m_nPrevTimeUpdate, m_nCurrTimeUpdate, m_fTimeScale);
}

// 0x4EF440
float CAESound::GetSlowMoFrequencyScalingFactor() const {
    return GetUnpausable() || !CTimer::GetIsSlowMotionActive() || CCamera::GetActiveCamera().m_nMode == eCamMode::MODE_CAMERA
        ? 1.f
        : fSlowMoFrequencyScalingFactor;
}

// 0x4EF7A0
void CAESound::NewVPSLentry() {
    m_nHasStarted = 0;
    m_nPlayingState = eSoundState::SOUND_ACTIVE;
    m_bWasServiced = 0;
    m_nIsUsed = 1;
    m_fSoundHeadRoom = AEAudioHardware.GetSoundHeadroom(m_nSoundIdInSlot, m_nBankSlotId);
    CalculateFrequency();
}

// 0x4EF820
void CAESound::RegisterWithPhysicalEntity(CEntity* entity) {
    CAESound::UnregisterWithPhysicalEntity();
    if (entity) {
        m_pPhysicalEntity = entity;
        entity->RegisterReference(&m_pPhysicalEntity);
    }
}

// 0x4EF850
void CAESound::StopSoundAndForget() {
    m_bRequestUpdates = false;
    m_pBaseAudio = nullptr;
    StopSound();
}

// 0x4EF880
void CAESound::SetPosition(CVector pos) {
    if (!m_nLastFrameUpdate) {
        m_vecPrevPosn = pos;
        m_vecCurrPosn = pos;

        auto const fCamDist = DistanceBetweenPoints(pos, TheCamera.GetPosition());
        m_fCurrCamDist = fCamDist;
        m_fPrevCamDist = fCamDist;

        m_nLastFrameUpdate = CTimer::GetFrameCounter();
        m_nCurrTimeUpdate = CTimer::GetTimeInMS();
        m_nPrevTimeUpdate = CTimer::GetTimeInMS();
    } else if (m_nLastFrameUpdate == CTimer::GetFrameCounter()) {
        m_vecCurrPosn = pos;
        m_fCurrCamDist = DistanceBetweenPoints(pos, TheCamera.GetPosition());
        m_nCurrTimeUpdate = CTimer::GetTimeInMS();
    } else {
        m_vecPrevPosn = m_vecCurrPosn;
        m_fPrevCamDist = m_fCurrCamDist;
        m_nPrevTimeUpdate = m_nCurrTimeUpdate;

        m_vecCurrPosn = pos;
        m_fCurrCamDist = DistanceBetweenPoints(pos, TheCamera.GetPosition());
        m_nCurrTimeUpdate = CTimer::GetTimeInMS();
        m_nLastFrameUpdate = CTimer::GetFrameCounter();
    }
}

// 0x4EFA10
void CAESound::CalculateVolume() {
    if (GetFrontEnd())
        m_fFinalVolume = m_fVolume - m_fSoundHeadRoom;
    else {
        const auto relativePos = CAEAudioEnvironment::GetPositionRelativeToCamera(m_vecCurrPosn);
        const auto fDist = CAEAudioEnvironment::GetPositionRelativeToCamera(m_vecCurrPosn).Magnitude() / m_fSoundDistance;
        const auto fAttenuation = CAEAudioEnvironment::GetDistanceAttenuation(fDist);
        m_fFinalVolume = CAEAudioEnvironment::GetDirectionalMikeAttenuation(relativePos) + fAttenuation + m_fVolume - m_fSoundHeadRoom;
    }
}

// 0x4EFE50
void CAESound::Initialise(
    int16 bankSlotId, int16 soundID, CAEAudioEntity* audioEntity, CVector pos, float volume, float rollOffFactor, float relativeFrequency, float doppler,
                          uint8 frameDelay, uint32 flags, float frequencyVariance, int16 playTime)
{
    UnregisterWithPhysicalEntity();

    m_nSoundIdInSlot        = soundID;
    m_nBankSlotId           = bankSlotId;
    m_pBaseAudio            = audioEntity;
    m_fVolume               = volume;
    m_fSoundDistance        = rollOffFactor;
    m_fSpeed                = relativeFrequency;
    m_fSpeedVariability     = frequencyVariance;
    m_vecPrevPosn           .Set(0.0F, 0.0F, 0.0F);
    m_nEvent                = AE_UNDEFINED;
    m_ClientVariable            = -1.0F;
    m_nLastFrameUpdate      = 0;

    SetPosition(pos);

    m_fTimeScale            = doppler;
    m_nSoundLength          = -1;
    m_nHasStarted           = 0;
    m_nPlayingState         = eSoundState::SOUND_ACTIVE;
    m_fSoundHeadRoom        = 0.0F;
    m_nIgnoredServiceCycles = frameDelay;
    m_nEnvironmentFlags     = flags;
    m_nIsUsed               = 1;
    m_nCurrentPlayPosition  = playTime;
    m_fFinalVolume          = -100.0F;
    m_fFrequency            = 1.0F;
}

// 0x4EFF50
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

// 0x4EFFD0
void CAESound::SoundHasFinished() {
    UpdateParameters(-1);
    UnregisterWithPhysicalEntity();
    m_nIsUsed = 0;
    m_nHasStarted = 0;
    m_nCurrentPlayPosition = 0;
}
