#include "StdInc.h"

#include "AEAudioEnvironment.h"
#include "AEAmbienceTrackManager.h"
#include "AEAudioHardware.h"
#include "AEAudioUtility.h"

CAEAmbienceTrackManager& AEAmbienceTrackManager = *(CAEAmbienceTrackManager*)0x8AC15C;

void CAEAmbienceTrackManager::InjectHooks() {
    RH_ScopedClass(CAEAmbienceTrackManager);
    RH_ScopedCategory("Audio/Managers");

    RH_ScopedInstall(Reset, 0x4D6CA0);
    RH_ScopedInstall(IsAmbienceTrackActive, 0x4D6CF0);
    RH_ScopedInstall(IsAmbienceRadioActive, 0x4D6D40);
    RH_ScopedInstall(PlaySpecialMissionAmbienceTrack, 0x4D6D50);
    RH_ScopedInstall(StopSpecialMissionAmbienceTrack, 0x4D6D60);
    RH_ScopedInstall(UpdateAmbienceTrackAndVolume, 0x4D6E60, { .reversed = false }); // bad
    RH_ScopedInstall(Service, 0x4D76C0);
}

// 0x5B9660
bool CAEAmbienceTrackManager::Initialise(int32 hwClientHandle) {
    m_HwClientHandle       = hwClientHandle;
    m_RequestedSettings    = {
        .TrackID        = -1,
        .PlayingTrackID = -1,
        .TrackFlags     = 3
    };
    m_AmbienceStatus      = AmbienceStatus::STOPPED;
    m_Volume              = -100.0f;
    m_FreqFactor          = 1.0f;
    return true;
}

// 0x4D6CA0
void CAEAmbienceTrackManager::Reset() {
    m_SpecialMissionAmbienceTrackID = -1;
    m_OverrideRadio                 = false;
    m_LastAmbienceOverrodeRadio     = false;
    m_PrevAmbiencePlayTimeMs        = -1;
    m_PrevAmbienceStopTimeMs        = 0;
    m_StartAmbienceAtBeginning      = false;
    m_AmbienceRadioStation          = RADIO_INVALID;
}

// 0x4D6CF0
bool CAEAmbienceTrackManager::IsAmbienceTrackActive() const {
    return m_AmbienceStatus != AmbienceStatus::STOPPED;
}

// 0x4D6D40
bool CAEAmbienceTrackManager::IsAmbienceRadioActive() const {
    return m_AmbienceRadioStation != RADIO_INVALID;
}

// 0x4D6D50
void CAEAmbienceTrackManager::PlaySpecialMissionAmbienceTrack(eAudioEvents event) {
    m_SpecialMissionAmbienceTrackID = event;
}

// 0x4D6E60
void CAEAmbienceTrackManager::UpdateAmbienceTrackAndVolume() {
    const auto prevAmbienceRadioStation = m_AmbienceRadioStation;
    auto       specialTrackId           = -1;
    auto       volume                   = -6.f;
    auto       isSandstorm              = false;

    m_OverrideRadio            = false;
    m_StartAmbienceAtBeginning = 0;
    m_AmbienceRadioStation     = RADIO_INVALID;

    if (m_SpecialMissionAmbienceTrackID < 0) { // 0x4D6E97
        if (CWeather::Sandstorm > 0.f) {
            if (CGame::CanSeeOutSideFromCurrArea() && !CCullZones::PlayerNoRain() && !CCullZones::CamNoRain()) {
                specialTrackId = 167;
                volume         = std::log10f(2) * std::log2f(CWeather::Sandstorm);
            }
            isSandstorm = true;
        }
    } else { // 0x4D6E97
        specialTrackId             = m_SpecialMissionAmbienceTrackID;
        m_StartAmbienceAtBeginning = true;
        m_OverrideRadio            = true;
    }

    if ((CWeather::Sandstorm <= 0.f && m_SpecialMissionAmbienceTrackID < 0) || (m_SpecialMissionAmbienceTrackID >= 0 && specialTrackId == -1)) {
        auto    activeAuZoIdx = -1;
        CVector activeAuZoPos{};

        if (CAudioZones::m_NumActiveSpheres) { // 0x4D6F0D
            const auto& sp = CAudioZones::m_aSpheres[CAudioZones::m_aActiveSpheres[0]];
            activeAuZoIdx = sp.m_nAudioZone;
            activeAuZoPos = sp.m_Sphere.m_vecCenter;
        } else if (CAudioZones::m_NumActiveBoxes) {
            const auto& box = CAudioZones::m_aBoxes[CAudioZones::m_aActiveBoxes[0]];
            activeAuZoIdx = box.m_nAudioZone;
        }

        const auto GetVolumeByAuZoDist = [&] {
            return CAEAudioEnvironment::GetDistanceAttenuation(CAEAudioEnvironment::GetPositionRelativeToCamera(activeAuZoPos).Magnitude() * 0.2f) - 6.f;
        };

        switch (activeAuZoIdx) {
        case -1:
            break;
        case 4: {
            specialTrackId = 143;
            const auto camZ = TheCamera.GetPosition().z;
            volume = camZ <= 1372.f // TODO: Magic number
                ? -6.f - 9.f * (1372.f - camZ)
                : -6.f;
            break;
        }
        case 5: {
            specialTrackId = 140;
            volume = GetVolumeByAuZoDist();
            break;
        }
        case 8: {
            specialTrackId = 165;
            break;
        }
        case 10: {
            specialTrackId = 139;
            volume = GetVolumeByAuZoDist();
            break;
        }
        case 12:
            specialTrackId = 168;
            break;
        case 13:
            specialTrackId = 157;
            m_OverrideRadio = true;
            break;
        case 15:
            specialTrackId = 164;
            break;
        case 17:
            specialTrackId = 146;
            break;
        case 19:
            specialTrackId = 138;
            break;
        case 20:
            specialTrackId = 136;
            break;
        case 21:
            specialTrackId = 135;
            break;
        case 23:
            specialTrackId = 148;
            break;
        case 24:
            specialTrackId = 159;
            break;
        case 25:
            specialTrackId = 158;
            break;
        case 26:
            specialTrackId = 154;
            break;
        case 28:
        case 29:
            specialTrackId = 147;
            break;
        case 30:
            m_AmbienceRadioStation = CStats::FindMostFavoriteRadioStation();
            break;
        case 34:
            specialTrackId = 162;
            break;
        case 36:
            specialTrackId = 155;
            break;
        case 37:
            specialTrackId = 144;
            m_StartAmbienceAtBeginning = 1;
            break;
        case 39:
            specialTrackId = 163;
            break;
        case 41:
            specialTrackId = 169;
            m_OverrideRadio = 1;
            break;
        case 44:
            specialTrackId = 152;
            break;
        case 48:
            specialTrackId = 137;
            break;
        case 50:
            specialTrackId = 173;
            break;
        case 51:
            specialTrackId = 156;
            break;
        case 52:
            m_AmbienceRadioStation = RADIO_CLASSIC_HIP_HOP;
            break;
        case 53:
            m_AmbienceRadioStation = RADIO_COUNTRY;
            break;
        case 54:
            m_AmbienceRadioStation = RADIO_CLASSIC_ROCK;
            break;
        case 55:
            m_AmbienceRadioStation = RADIO_DISCO_FUNK;
            break;
        case 56:
            m_AmbienceRadioStation = RADIO_HOUSE_CLASSICS;
            break;
        case 57:
            m_AmbienceRadioStation = RADIO_MODERN_HIP_HOP;
            break;
        case 58:
            m_AmbienceRadioStation = RADIO_MODERN_ROCK;
            break;
        case 59:
            m_AmbienceRadioStation = RADIO_NEW_JACK_SWING;
            break;
        case 60:
            m_AmbienceRadioStation = RADIO_REGGAE;
            break;
        case 61:
            m_AmbienceRadioStation = RADIO_RARE_GROOVE;
            break;
        case 62:
            m_AmbienceRadioStation = RADIO_TALK;
            break;
        case 64:
            specialTrackId = 151;
            break;
        case 66:
            specialTrackId = 170;
            break;
        case 67:
            specialTrackId = 171;
            break;
        default:
            break;
        }
    }
    const auto FadeOutAndStop = [this] {
        if (m_Volume > -40.f) {
            m_Volume -= 0.4f;
        } else {
            StopAmbienceTrack();
        }
    };
    if (m_AmbienceRadioStation != RADIO_INVALID) { // 0x4D718C
        if (AudioEngine.IsRadioOn()) { // Inverted
            if (AudioEngine.GetCurrentRadioStationID() != m_AmbienceRadioStation) {
                AudioEngine.StopRadio(nullptr, false);
            }
        } else {
            AudioEngine.StartRadio(m_AmbienceRadioStation, 2);
        }
    } else if (prevAmbienceRadioStation != RADIO_INVALID) { //>0x4D71E5 - Radio was running previously, but isn't anymore, so stop it
        if (AudioEngine.IsRadioOn()) {
            AudioEngine.StopRadio(0, 0);
        }
    } else if (AudioEngine.IsRadioOn()) { // 0x4D720A
        if (m_OverrideRadio) {
            AudioEngine.StopRadio(nullptr, false);
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);
        }
        m_LastAmbienceOverrodeRadio = m_OverrideRadio;
    } else if (specialTrackId != -1) {
        volume = CAEAudioEnvironment::GetDistanceAttenuation(std::sqrt(sq(TheCamera.m_fDistanceToWater) + sq(TheCamera.GetPosition().z - TheCamera.m_fHeightOfNearestWater)) / SQRT_3) - 12.f;
        if (CWeather::UnderWaterness < 0.5f) {
            if (volume <= -34.f) { // 0x4D74A7
                if (CGameLogic::LaRiotsActiveHere() || !CGame::CanSeeOutSideFromCurrArea()) {
                    if (m_AmbienceStatus != 8) { // 0x4D7529
                        if (m_RequestedSettings.TrackID == 174) {
                            if (volume < -40.f) {
                                StopAmbienceTrack();
                            }
                            m_Volume     = -100.f;
                            m_FreqFactor = 1.f;
                        } else if (isSandstorm && m_Volume > -40.f) { // 0x4D756C
                            m_FreqFactor =  1.f;
                            m_Volume     -= 0.4f;
                        } else {
                            StopAmbienceTrack();
                            if (m_LastAmbienceOverrodeRadio || AudioEngine.IsVehicleRadioActive()) {
                                AudioEngine.StartRadio(AudioEngine.GetCurrentRadioStationID(), 0);
                            }
                            m_FreqFactor = 1.f;
                        }
                    }
                } else if (m_AmbienceStatus == 8) { // 0x4D74A9
                    m_AmbienceStatus            = AmbienceStatus::STARTING;
                    m_RequestedSettings.TrackID = 166;
                    m_Volume                    = -34.f;
                    m_FreqFactor                = 1.f;
                } else if (m_RequestedSettings.TrackID == 166) { // 0x4D7500
                    m_Volume = std::min(-12.f, m_Volume + 1.f);
                } else {
                    FadeOutAndStop();
                }
                return;
            }
        } else {
            volume = 14.f;
        }

        if (m_AmbienceStatus == 8) { // 0x4D7370
            m_RequestedSettings.TrackID = 174;
            m_AmbienceStatus = AmbienceStatus::STARTING;
            m_Volume = -34.f;
            m_FreqFactor = CWeather::UnderWaterness < 0.5
                ? 1.f
                : 0.0625f;
        } else if (m_RequestedSettings.TrackID != 174) { // 0x4D73AE
            FadeOutAndStop();
        } else { // 0x4D73E7
            m_Volume = notsa::step_to(m_Volume, volume, 1.f);
        }
        m_FreqFactor = notsa::step_to(m_FreqFactor, CWeather::UnderWaterness < 0.5f ? 1.f : 0.0625f, 0.25f);   
    } else if (specialTrackId != m_SpecialMissionAmbienceTrackID) { // 0x4D7250
        if (m_AmbienceStatus == 8) {
            m_RequestedSettings.TrackID = specialTrackId;
            m_AmbienceStatus            = AmbienceStatus::STARTING;
            m_Volume                    = -34.f;
            m_FreqFactor                = 1.f;
        } else if (m_RequestedSettings.TrackID == specialTrackId) {
            m_Volume = std::min(m_Volume + 1.f, m_Volume);
        } else {
            FadeOutAndStop();
        }
    } else if (m_AmbienceStatus == 8) { // 0x4D725A
        m_RequestedSettings.TrackID = specialTrackId;
        m_AmbienceStatus            = AmbienceStatus::STARTING;
        m_Volume                    = -2.f;
        m_FreqFactor                = 1.f;
    } else if (m_RequestedSettings.TrackID == specialTrackId) { // 0x4D7278
        StopAmbienceTrack();
    }
}

// 0x4D76C0
void CAEAmbienceTrackManager::Service(int32 trackPlayTime) {
    if (   !AudioEngine.IsCutsceneTrackActive()
        && !CTimer::GetIsPaused()
        && !FrontEndMenuManager.m_bMainMenuSwitch
    ) {
        UpdateAmbienceTrackAndVolume();
    }

    switch (m_AmbienceStatus) {
    case AmbienceStatus::STARTING: // 0x4D770B
        AEAudioHardware.PlayTrack(
            m_RequestedSettings.TrackID,
            -1,
            m_StartAmbienceAtBeginning // Pirulax: I'm so sorry I couldn't resist writing this!
                ? 0
                : m_PrevAmbiencePlayTimeMs < 0
                    ? CAEAudioUtility::GetRandomNumberInRange(0, 600'000)
                    : m_PrevAmbiencePlayTimeMs + std::max(
                        m_PrevAmbienceStopTimeMs ? (int32)CTimer::GetTimeInMS() - m_PrevAmbienceStopTimeMs : 0,
                        7000
                    ),
            m_RequestedSettings.TrackFlags,
            false,
            false
        );
        m_AmbienceStatus = AmbienceStatus::WAITING_TO_PLAY;
        break;
    case AmbienceStatus::WAITING_TO_PLAY: // 0x4D7779
        if (trackPlayTime == -2) {
            m_AmbienceStatus = READY_TO_PLAY;
        }
        break;
    case AmbienceStatus::READY_TO_PLAY: // 0x4D778D
        StartTrackPlayback();
        m_AmbienceStatus = PLAYING;
        break;
    case AmbienceStatus::PLAYING: // 0x4D77A0
        CheckForPause();
        AEAudioHardware.SetChannelVolume(m_HwClientHandle, 0, m_Volume, 0);
        break;
    case AmbienceStatus::STOPPING: // 0x4D77C3
        if (m_RequestedSettings.TrackID == m_SpecialMissionAmbienceTrackID) {
            m_PrevAmbiencePlayTimeMs = -1;
            m_PrevAmbienceStopTimeMs = 0;
        } else {
            m_PrevAmbiencePlayTimeMs = trackPlayTime;
            m_PrevAmbienceStopTimeMs = CTimer::GetTimeInMS();
        }
        m_Volume = -100.0f;
        [[fallthrough]];
    case AmbienceStatus::STOPPING_CHANNELS_STOPPED: // Moved here to allow fallthrough
        AEAudioHardware.StopTrack();
        m_AmbienceStatus = WAITING_TO_STOP;
        break;
    case AmbienceStatus::STOPPING_SILENCED:
        AEAudioHardware.StopSound(m_HwClientHandle, 0);
        m_AmbienceStatus = STOPPING_CHANNELS_STOPPED;
        break;
    case AmbienceStatus::WAITING_TO_STOP:
        if (trackPlayTime == -6) {
            m_AmbienceStatus = STOPPED;
        } else if (trackPlayTime == -7 || trackPlayTime == -2) {
            AEAudioHardware.StopTrack();
        }
        break;
    default:
        return;
    }
}

// 0x4D6D60
void CAEAmbienceTrackManager::StopSpecialMissionAmbienceTrack() {
    ChangeStatusToStopped();
    Reset();
}

// 0x4D6CC0
void CAEAmbienceTrackManager::StopAmbienceTrack() {
    ChangeStatusToStopped();
    m_OverrideRadio             = false;
    m_LastAmbienceOverrodeRadio = false;
    m_StartAmbienceAtBeginning  = false;
}

// 0x4D6DA0
void CAEAmbienceTrackManager::StartTrackPlayback() const {
    AEAudioHardware.DisableBassEq();
    AEAudioHardware.SetChannelVolume(m_HwClientHandle, 0, m_Volume, 0);
    AEAudioHardware.SetChannelFlags(m_HwClientHandle, 0, 33);
    AEAudioHardware.StartTrackPlayback();
}

//  0x4D6DF0
void CAEAmbienceTrackManager::CheckForPause() const {
    float freqFactor = CTimer::GetIsPaused() || FrontEndMenuManager.m_bMainMenuSwitch ? 0.0f : m_FreqFactor;
    AEAudioHardware.SetChannelFrequencyScalingFactor(m_HwClientHandle, 0, freqFactor);
    AEAudioHardware.SetChannelFrequencyScalingFactor(m_HwClientHandle, 0, freqFactor);
}

// NOTSA
void CAEAmbienceTrackManager::ChangeStatusToStopped() {
    m_AmbienceStatus = [this] {
        using enum AmbienceStatus;
        switch (m_AmbienceStatus) {
        case STARTING:
        case WAITING_TO_PLAY:
        case READY_TO_PLAY:
        case PLAYING:
            return STOPPING;
        }
        return m_AmbienceStatus;
    }();
}
