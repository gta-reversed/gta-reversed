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

//TODO: Remove once the cause of occasional nan here is found
#define NANCHECK() if (isnan(m_CurrPos.x)) { assert(false); }

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
    RH_ScopedInstall(NewVPSLEntry, 0x4EF7A0);
    RH_ScopedInstall(RegisterWithPhysicalEntity, 0x4EF820);
    RH_ScopedInstall(StopSoundAndForget, 0x4EF850);
    RH_ScopedInstall(SetPosition, 0x4EF880);
    RH_ScopedInstall(CalculateVolume, 0x4EFA10);
    RH_ScopedInstall(Initialise, 0x4EFE50);
    RH_ScopedInstall(UpdateParameters, 0x4EFF50);
    RH_ScopedInstall(SoundHasFinished, 0x4EFFD0);
}

CAESound::CAESound(CAESound& sound) {
    m_BankSlot = sound.m_BankSlot;
    m_SoundID = sound.m_SoundID;
    m_pBaseAudio = sound.m_pBaseAudio;
    m_Event = sound.m_Event;
    m_ClientVariable = sound.m_ClientVariable;
    m_Volume = sound.m_Volume;
    m_RollOffFactor = sound.m_RollOffFactor;
    m_Speed = sound.m_Speed;
    m_SpeedVariance = sound.m_SpeedVariance;
    m_CurrPos = sound.m_CurrPos;
    m_PrevPos = sound.m_PrevPos;
    m_LastFrameUpdateMs = sound.m_LastFrameUpdateMs;
    m_CurrTimeUpdateMs = sound.m_CurrTimeUpdateMs;
    m_PrevTimeUpdateMs = sound.m_PrevTimeUpdateMs;
    m_CurrCamDist = sound.m_CurrCamDist;
    m_PrevCamDist = sound.m_PrevCamDist;
    m_Doppler = sound.m_Doppler;
    m_FrameDelay = sound.m_FrameDelay;
    m_nEnvironmentFlags = sound.m_nEnvironmentFlags;
    m_IsInUse = sound.m_IsInUse;
    m_PlayTime = sound.m_PlayTime;
    m_IsPhysicallyPlaying = sound.m_IsPhysicallyPlaying;
    m_ListenerVolume = sound.m_ListenerVolume;
    m_ListenerSpeed = sound.m_ListenerSpeed;
    m_HasRequestedStopped = sound.m_HasRequestedStopped;
    m_Headroom = sound.m_Headroom;
    m_Length = sound.m_Length;
    m_pPhysicalEntity = nullptr;

    if (sound.m_pPhysicalEntity) {
        m_pPhysicalEntity = sound.m_pPhysicalEntity;
        m_pPhysicalEntity->RegisterReference(&m_pPhysicalEntity);
    }

    NANCHECK()
}

CAESound::CAESound(eSoundBankSlot bankSlotId, eSoundID sfxId, CAEAudioEntity* baseAudio, CVector posn, float volume, float fDistance, float speed, float timeScale,
                   uint8 ignoredServiceCycles, eSoundEnvironment environmentFlags, float speedVariability) {
    m_BankSlot = bankSlotId;
    m_SoundID = sfxId;
    m_pBaseAudio = baseAudio;
    m_PrevPos = CVector(0.0f, 0.0f, 0.0f);
    m_pPhysicalEntity = nullptr;
    m_ClientVariable = -1.0F;
    m_Event = AE_UNDEFINED;
    m_LastFrameUpdateMs = 0;

    SetPosition(posn);

    m_Volume = volume;
    m_RollOffFactor = fDistance;
    m_Speed = speed;
    m_Doppler = timeScale;
    m_FrameDelay = ignoredServiceCycles;
    m_nEnvironmentFlags = environmentFlags;
    m_IsPhysicallyPlaying = 0;
    m_PlayTime = 0;
    m_Headroom = 0.0F;
    m_SpeedVariance = speedVariability;
    m_IsInUse = 1;
    m_ListenerVolume = -100.0F;
    m_ListenerSpeed = 1.0F;
    m_Length = -1;
}

CAESound::~CAESound() {
    UnregisterWithPhysicalEntity();
}

// 0x4EF680
CAESound& CAESound::operator=(const CAESound& sound) {
    if (this == &sound)
        return *this;

    CAESound::UnregisterWithPhysicalEntity();
    m_BankSlot           = sound.m_BankSlot;
    m_SoundID        = sound.m_SoundID;
    m_pBaseAudio            = sound.m_pBaseAudio;
    m_Event                = sound.m_Event;
    m_ClientVariable            = sound.m_ClientVariable;
    m_Volume               = sound.m_Volume;
    m_RollOffFactor        = sound.m_RollOffFactor;
    m_Speed                = sound.m_Speed;
    m_SpeedVariance     = sound.m_SpeedVariance;
    m_CurrPos           = sound.m_CurrPos;
    m_PrevPos           = sound.m_PrevPos;
    m_LastFrameUpdateMs      = sound.m_LastFrameUpdateMs;
    m_CurrTimeUpdateMs       = sound.m_CurrTimeUpdateMs;
    m_PrevTimeUpdateMs       = sound.m_PrevTimeUpdateMs;
    m_CurrCamDist          = sound.m_CurrCamDist;
    m_PrevCamDist          = sound.m_PrevCamDist;
    m_Doppler            = sound.m_Doppler;
    m_FrameDelay = sound.m_FrameDelay;
    m_nEnvironmentFlags     = sound.m_nEnvironmentFlags;
    m_IsInUse               = sound.m_IsInUse;
    m_PlayTime  = sound.m_PlayTime;
    m_IsPhysicallyPlaying           = sound.m_IsPhysicallyPlaying;
    m_ListenerVolume          = sound.m_ListenerVolume;
    m_ListenerSpeed            = sound.m_ListenerSpeed;
    m_HasRequestedStopped         = sound.m_HasRequestedStopped;
    m_Headroom        = sound.m_Headroom;
    m_Length          = sound.m_Length;
    m_pPhysicalEntity       = nullptr;
    RegisterWithPhysicalEntity(sound.m_pPhysicalEntity);

    NANCHECK()
    return *this;
}

// 0x4EF1A0
void CAESound::UnregisterWithPhysicalEntity() {
    CEntity::ClearReference(m_pPhysicalEntity);
}

// 0x4EF1C0
void CAESound::StopSound() {
    m_HasRequestedStopped = eSoundState::SOUND_STOPPED;
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
    m_Length = soundLength;
    if (m_IsPhysicallyPlaying)
        return;

    if (m_HasRequestedStopped != eSoundState::SOUND_ACTIVE) {
        m_PlayTime = -1;
        return;
    }

    m_PlayTime += static_cast<int16>(static_cast<float>(playProgress) * m_ListenerSpeed);
    if (m_PlayTime < soundLength)
        return;

    if (loopStartTime == -1) {
        m_PlayTime = -1;
        return;
    }

    // Avoid division by 0
    // This seems to have been fixed the same way in Android
    // The cause is/can be missing audio files, but I'm lazy to fix it, so this is gonna be fine for now
    m_PlayTime = !notsa::IsFixBugs() || soundLength > 0
        ? loopStartTime + (m_PlayTime % soundLength)
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
    *outVec = GetFrontEnd() ? m_CurrPos : CAEAudioEnvironment::GetPositionRelativeToCamera(m_CurrPos);
}

// 0x4EF390
void CAESound::CalculateFrequency() {
    m_ListenerSpeed = m_SpeedVariance <= 0.0F || m_SpeedVariance >= m_Speed
        ? m_Speed
        : m_Speed + CAEAudioUtility::GetRandomNumberInRange(-m_SpeedVariance, m_SpeedVariance);
}

// 0x4EF3E0
void CAESound::UpdateFrequency() {
    if (m_SpeedVariance == 0.0F) {
        m_ListenerSpeed = m_Speed;
    }
}

// 0x4EF400
float CAESound::GetRelativePlaybackFrequencyWithDoppler() const {
    return GetFrontEnd()
        ? m_ListenerSpeed
        : m_ListenerSpeed * CAEAudioEnvironment::GetDopplerRelativeFrequency(m_PrevCamDist, m_CurrCamDist, m_PrevTimeUpdateMs, m_CurrTimeUpdateMs, m_Doppler);
}

// 0x4EF440
float CAESound::GetSlowMoFrequencyScalingFactor() const {
    return GetUnpausable() || !CTimer::GetIsSlowMotionActive() || CCamera::GetActiveCamera().m_nMode == eCamMode::MODE_CAMERA
        ? 1.f
        : fSlowMoFrequencyScalingFactor;
}

// 0x4EF7A0
void CAESound::NewVPSLEntry() {
    m_IsPhysicallyPlaying = 0;
    m_HasRequestedStopped = eSoundState::SOUND_ACTIVE;
    m_IsAudioHardwareAware = 0;
    m_IsInUse = 1;
    m_Headroom = AEAudioHardware.GetSoundHeadroom(m_SoundID, m_BankSlot);
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
    m_RequestUpdates = false;
    m_pBaseAudio = nullptr;
    StopSound();
}

// 0x4EF880
void CAESound::SetPosition(CVector pos) {
    if (!m_LastFrameUpdateMs) {
        m_PrevPos = pos;
        m_CurrPos = pos;

        auto const fCamDist = DistanceBetweenPoints(pos, TheCamera.GetPosition());
        m_CurrCamDist = fCamDist;
        m_PrevCamDist = fCamDist;

        m_LastFrameUpdateMs = CTimer::GetFrameCounter();
        m_CurrTimeUpdateMs = CTimer::GetTimeInMS();
        m_PrevTimeUpdateMs = CTimer::GetTimeInMS();
    } else if (m_LastFrameUpdateMs == CTimer::GetFrameCounter()) {
        m_CurrPos = pos;
        m_CurrCamDist = DistanceBetweenPoints(pos, TheCamera.GetPosition());
        m_CurrTimeUpdateMs = CTimer::GetTimeInMS();
    } else {
        m_PrevPos = m_CurrPos;
        m_PrevCamDist = m_CurrCamDist;
        m_PrevTimeUpdateMs = m_CurrTimeUpdateMs;

        m_CurrPos = pos;
        m_CurrCamDist = DistanceBetweenPoints(pos, TheCamera.GetPosition());
        m_CurrTimeUpdateMs = CTimer::GetTimeInMS();
        m_LastFrameUpdateMs = CTimer::GetFrameCounter();
    }

    NANCHECK()
}

// 0x4EFA10
void CAESound::CalculateVolume() {
    if (!GetFrontEnd()) {
        m_ListenerVolume = CAEAudioEnvironment::GetDirectionalMikeAttenuation(CAEAudioEnvironment::GetPositionRelativeToCamera(m_CurrPos))
            + CAEAudioEnvironment::GetDistanceAttenuation(CAEAudioEnvironment::GetPositionRelativeToCamera(m_CurrPos).Magnitude() / m_RollOffFactor);
    }
    m_ListenerVolume += m_Volume - m_Headroom;
}

// 0x4EFE50
void CAESound::Initialise(
    eSoundBankSlot  bankSlotId,
    eSoundID        soundID,
    CAEAudioEntity* audioEntity,
    CVector         pos,
    float           volume,
    float           rollOff,
    float           speed,
    float           doppler,
    uint8           frameDelay,
    uint32          flags,
    float           frequencyVariance,
    int16           playTime
) {
    assert((!(flags & SOUND_REQUEST_UPDATES) || audioEntity) && "SOUND_REQUEST_UPDATES flag requires `audioEntity` to be set!");

    UnregisterWithPhysicalEntity();

    m_SoundID        = soundID;
    m_BankSlot           = bankSlotId;
    m_pBaseAudio            = audioEntity;
    m_Volume               = volume;
    m_RollOffFactor        = rollOff;
    m_Speed                = speed;
    m_SpeedVariance     = frequencyVariance;
    m_PrevPos           .Set(0.0F, 0.0F, 0.0F);
    m_Event                = AE_UNDEFINED;
    m_ClientVariable            = -1.0F;
    m_LastFrameUpdateMs      = 0;

    SetPosition(pos);

    m_Doppler               = doppler;
    m_Length          = -1;
    m_IsPhysicallyPlaying           = 0;
    m_HasRequestedStopped         = eSoundState::SOUND_ACTIVE;
    m_Headroom        = 0.0F;
    m_FrameDelay = frameDelay;
    m_nEnvironmentFlags     = flags;
    m_IsInUse               = 1;
    m_PlayTime  = playTime;
    m_ListenerVolume          = -100.0F;
    m_ListenerSpeed            = 1.0F;
}

// 0x4EFF50
void CAESound::UpdateParameters(int16 curPlayPos) {
    if (GetLifespanTiedToPhysicalEntity()) {
        if (!m_pPhysicalEntity)
            m_HasRequestedStopped = eSoundState::SOUND_STOPPED;
        else
            SetPosition(m_pPhysicalEntity->GetPosition());
    }

    if (GetRequestUpdates() && m_pBaseAudio) {
        m_pBaseAudio->UpdateParameters(this, curPlayPos);
        if (m_SpeedVariance == 0.0F)
            m_ListenerSpeed = m_Speed;
    }
}

// 0x4EFFD0
void CAESound::SoundHasFinished() {
    UpdateParameters(-1);
    UnregisterWithPhysicalEntity();
    m_IsInUse = 0;
    m_IsPhysicallyPlaying = 0;
    m_PlayTime = 0;
}
