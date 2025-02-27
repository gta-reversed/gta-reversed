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
    RH_ScopedInstall(SetFlags, 0x4EF2B0);
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

CAESound::CAESound(
    eSoundBankSlot  bankSlot,
    eSoundID        sfxId,
    CAEAudioEntity* audioEntity,
    CVector         pos,
    float           volume,
    float           rollOff,
    float           speed,
    float           doppler,
    uint8           frameDelay,
    uint16          flags,
    float           speedVariance
) :
    m_BankSlot{ bankSlot },
    m_SoundID{ sfxId },
    m_AudioEntity{ audioEntity },
    m_Volume{ volume },
    m_RollOffFactor{ rollOff },
    m_Speed{ speed },
    m_Doppler{ doppler },
    m_FrameDelay{ frameDelay },
    m_Flags{ flags },
    m_SpeedVariance{ speedVariance }
{
    assert((!(flags & SOUND_REQUEST_UPDATES) || audioEntity) && "SOUND_REQUEST_UPDATES flag requires `audioEntity` to be set!");

    SetPosition(pos);
}

CAESound::~CAESound() {
    UnregisterWithPhysicalEntity();
}

// 0x4EFE50
void CAESound::Initialise(
    eSoundBankSlot  bankSlot,
    eSoundID        sfxId,
    CAEAudioEntity* audioEntity,
    CVector         pos,
    float           volume,
    float           rollOff,
    float           speed,
    float           doppler,
    uint8           frameDelay,
    uint16          flags,
    float           speedVariance,
    int16           playTime
) {
    *this = CAESound{
        bankSlot,
        sfxId,
        audioEntity,
        pos,
        volume,
        rollOff,
        speed,
        doppler,
        frameDelay,
        (eSoundEnvironment)(flags),
        speedVariance,
    };
    m_PlayTime = playTime;
}

// 0x4EF1A0
void CAESound::UnregisterWithPhysicalEntity() {
    m_PhysicalEntity = nullptr;
}

// 0x4EF1C0
void CAESound::StopSound() {
    m_HasRequestedStopped = true;
    UnregisterWithPhysicalEntity();
}

// 0x4EF2B0
void CAESound::SetFlags(uint16 envFlag, uint16 bEnabled) {
    if (bEnabled)
        m_Flags |= envFlag;
    else
        m_Flags &= ~envFlag;
}

// 0x4EF2E0
void CAESound::UpdatePlayTime(int16 soundLength, int16 loopStartTime, int16 playProgress) {
    m_Length = soundLength;
    if (m_IsPhysicallyPlaying) {
        return;
    }

    if (m_HasRequestedStopped) {
        m_PlayTime = -1;
        return;
    }

    m_PlayTime += static_cast<int16>(static_cast<float>(playProgress) * m_ListenerSpeed);
    if (m_PlayTime < soundLength) {
        return;
    }

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
    *outVec = IsFrontEnd() ? m_CurrPos : CAEAudioEnvironment::GetPositionRelativeToCamera(m_CurrPos);
}

// 0x4EF390
void CAESound::CalculateFrequency() {
    m_ListenerSpeed = m_SpeedVariance <= 0.f || m_SpeedVariance >= m_Speed
        ? m_Speed
        : m_Speed + CAEAudioUtility::GetRandomNumberInRange(-m_SpeedVariance, m_SpeedVariance);
}

// 0x4EF3E0
void CAESound::UpdateFrequency() {
    if (m_SpeedVariance == 0.f) {
        m_ListenerSpeed = m_Speed;
    }
}

// 0x4EF400
float CAESound::GetRelativePlaybackFrequencyWithDoppler() const {
    return IsFrontEnd()
        ? m_ListenerSpeed
        : m_ListenerSpeed * CAEAudioEnvironment::GetDopplerRelativeFrequency(m_PrevCamDist, m_CurrCamDist, m_PrevTimeUpdateMs, m_CurrTimeUpdateMs, m_Doppler);
}

// 0x4EF440
float CAESound::GetSlowMoFrequencyScalingFactor() const {
    return IsPausable() || !CTimer::GetIsSlowMotionActive() || CCamera::GetActiveCamera().m_nMode == eCamMode::MODE_CAMERA
        ? 1.f
        : fSlowMoFrequencyScalingFactor;
}

// 0x4EF7A0
void CAESound::NewVPSLEntry() {
    m_IsPhysicallyPlaying  = true;
    m_HasRequestedStopped  = false;
    m_IsAudioHardwareAware = false;
    m_IsInUse              = true;
    m_Headroom             = AEAudioHardware.GetSoundHeadroom(m_SoundID, m_BankSlot);
    CalculateFrequency();
}

// 0x4EF820
void CAESound::RegisterWithPhysicalEntity(CEntity* entity) {
    CAESound::UnregisterWithPhysicalEntity();
    if (entity) {
        m_PhysicalEntity = entity;
    }
}

// 0x4EF850
void CAESound::StopSoundAndForget() {
    m_RequestUpdates = false;
    m_AudioEntity    = nullptr;
    StopSound();
}

// 0x4EF880
void CAESound::SetPosition(CVector pos) {
    const auto dist = DistanceBetweenPoints(pos, TheCamera.GetPosition());
    if (!m_LastFrameUpdatedAt) { // 0x4EF898
        m_PrevCamDist       = dist;
        m_PrevPos           = pos;
        m_PrevTimeUpdateMs  = CTimer::GetTimeInMS();
    } else if (m_LastFrameUpdatedAt != CTimer::GetFrameCounter()) {
        m_PrevPos           = m_CurrPos;
        m_PrevCamDist       = m_CurrCamDist;
        m_PrevTimeUpdateMs  = m_CurrTimeUpdateMs;
    }
    m_LastFrameUpdatedAt = CTimer::GetFrameCounter();
    m_CurrCamDist        = dist;
    m_CurrPos            = pos;
    m_CurrTimeUpdateMs   = CTimer::GetTimeInMS();
}

// 0x4EFA10
void CAESound::CalculateVolume() {
    if (!IsFrontEnd()) {
        m_ListenerVolume = CAEAudioEnvironment::GetDirectionalMikeAttenuation(CAEAudioEnvironment::GetPositionRelativeToCamera(m_CurrPos))
            + CAEAudioEnvironment::GetDistanceAttenuation(CAEAudioEnvironment::GetPositionRelativeToCamera(m_CurrPos).Magnitude() / m_RollOffFactor);
    }
    m_ListenerVolume += m_Volume - m_Headroom;
}

// 0x4EFF50
void CAESound::UpdateParameters(int16 curPlayPos) {
    if (IsLifespanTiedToPhysicalEntity()) {
        if (m_PhysicalEntity) {
            SetPosition(m_PhysicalEntity->GetPosition());
        } else {
            m_HasRequestedStopped = true;
        }
    }
    if (GetRequestUpdates()) {
        if (m_AudioEntity) { // NB: References are clearable, so must check!
            m_AudioEntity->UpdateParameters(this, curPlayPos);
            if (m_SpeedVariance == 0.0F) {
                m_ListenerSpeed = m_Speed;
            }
        }
    }
}

// 0x4EFFD0
void CAESound::SoundHasFinished() {
    UpdateParameters(-1);
    UnregisterWithPhysicalEntity();
    m_IsInUse             = false;
    m_IsPhysicallyPlaying = false;
    m_PlayTime            = 0;
}
