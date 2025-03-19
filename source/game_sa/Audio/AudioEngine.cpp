#include "StdInc.h"

#include "AudioEngine.h"

#include "AEAudioHardware.h"
#include "AEAmbienceTrackManager.h"
#include "AECutsceneTrackManager.h"
#include "AEUserRadioTrackManager.h"
#include "AEAudioUtility.h"
#include "AEWaterCannonAudioEntity.h"
#include "LoadingScreen.h"

CAudioEngine& AudioEngine = *(CAudioEngine*)0xB6BC90;

void CAudioEngine::InjectHooks() {
    RH_ScopedClass(CAudioEngine);
    RH_ScopedCategory("Audio");

    RH_ScopedInstall(Constructor, 0x507670);
    RH_ScopedInstall(Destructor, 0x506CD0);

    RH_ScopedInstall(Initialise, 0x5B9C60);
    RH_ScopedInstall(Shutdown, 0x507CB0);
    RH_ScopedInstall(Restart, 0x506DB0);
    RH_ScopedInstall(ResetStatistics, 0x506DA0);
    RH_ScopedInstall(ResetSoundEffects, 0x507C30);
    RH_ScopedInstall(Reset, 0x507A90);
    RH_ScopedInstall(IsAmbienceRadioActive, 0x507280);
    RH_ScopedInstall(DoesAmbienceTrackOverrideRadio, 0x507270);
    RH_ScopedInstall(StopAmbienceTrack, 0x507220);
    RH_ScopedInstall(IsAmbienceTrackActive, 0x507210);
    RH_ScopedInstall(PauseBeatTrack, 0x507200);
    RH_ScopedInstall(RetuneRadio, 0x507E10);
    RH_ScopedOverloadedInstall(StartRadio, "", 0x507DF0, void (CAudioEngine::*)(tVehicleAudioSettings*));
    RH_ScopedOverloadedInstall(StartRadio, "1", 0x507DC0, void (CAudioEngine::*)(eRadioID, int8));
    RH_ScopedInstall(ServiceLoadingTune, 0x5078A0);
    RH_ScopedInstall(ResumeAllSounds, 0x507440);
    RH_ScopedInstall(PauseAllSounds, 0x507430);
    RH_ScopedInstall(EnableEffectsLoading, 0x506E90);
    RH_ScopedInstall(DisableEffectsLoading, 0x506E80);
    RH_ScopedInstall(SetNonStreamFaderScalingFactor, 0x506E60);
    RH_ScopedInstall(SetEffectsFaderScalingFactor, 0x506E50);
    RH_ScopedInstall(SetMusicFaderScalingFactor, 0x506E40);
    RH_ScopedInstall(SetEffectsMasterVolume, 0x506E10);
    RH_ScopedInstall(SetMusicMasterVolume, 0x506DE0);
    RH_ScopedInstall(SetStreamFaderScalingFactor, 0x506E70);
    RH_ScopedInstall(SetRadioAutoRetuneOnOff, 0x506F80);
    RH_ScopedInstall(SetRadioBassSetting, 0x506FA0);
    RH_ScopedInstall(SetBassEnhanceOnOff, 0x506F90);
    RH_ScopedInstall(SetMissionAudioPosition, 0x507300);
    RH_ScopedInstall(GetBeatTrackStatus, 0x507170);
    RH_ScopedInstall(GetCutsceneTrackStatus, 0x507160);
    RH_ScopedInstall(IsCutsceneTrackActive, 0x507150);
    RH_ScopedInstall(PlayPreloadedCutsceneTrack, 0x507070);
    RH_ScopedInstall(IsVehicleRadioActive, 0x507050);
    RH_ScopedInstall(GetCurrentRadioStationID, 0x507040);
    RH_ScopedInstall(StopRadio, 0x506F70);
    RH_ScopedInstall(DisplayRadioStationName, 0x507030);
    RH_ScopedInstall(GetRadioStationNameKey, 0x507010);
    RH_ScopedInstall(GetRadioStationName, 0x507000);
    RH_ScopedInstall(HasRadioRetuneJustStarted, 0x506FE0);
    RH_ScopedInstall(IsRadioOn, 0x506FD0);
    RH_ScopedInstall(InitialiseRadioStationID, 0x506FC0);
    RH_ScopedInstall(GetRadioStationListenTimes, 0x507020);
    RH_ScopedInstall(AttachMissionAudioToPhysical, 0x507330);
    RH_ScopedInstall(AttachMissionAudioToObject, 0x507320);
    RH_ScopedInstall(AttachMissionAudioToPed, 0x507310);
    RH_ScopedInstall(ClearMissionAudio, 0x5072F0);
    RH_ScopedInstall(GetMissionAudioPosition, 0x5072E0);
    RH_ScopedInstall(GetMissionAudioEvent, 0x5072D0);
    RH_ScopedInstall(IsMissionAudioSampleFinished, 0x5072C0);
    RH_ScopedInstall(PlayLoadedMissionAudio, 0x5072B0);
    RH_ScopedInstall(GetMissionAudioLoadingStatus, 0x5072A0);
    RH_ScopedInstall(PreloadMissionAudio, 0x507290);
    RH_ScopedInstall(PreloadCutsceneTrack, 0x507E30);
    RH_ScopedInstall(InitialisePostLoading, 0x5078F0);
    RH_ScopedInstall(IsBeatInfoPresent, 0x5071D0);
    RH_ScopedInstall(StartLoadingTune, 0x507410);
    RH_ScopedInstall(SayPedless, 0x5073C0);
    RH_ScopedInstall(Service, 0x507750);
    RH_ScopedInstall(GetBeatInfo, 0x5071B0);
    RH_ScopedInstall(StopBeatTrack, 0x5071A0);
    RH_ScopedInstall(PlayPreloadedBeatTrack, 0x507180);
    RH_ScopedOverloadedInstall(ReportWaterSplash, "vec", 0x506F10, void (CAudioEngine::*)(CVector, float));
    RH_ScopedOverloadedInstall(ReportWaterSplash, "phs", 0x506F00, void (CAudioEngine::*)(CPhysical*, float, bool));
    RH_ScopedInstall(ReportGlassCollisionEvent, 0x506EE0);
    RH_ScopedInstall(ReportObjectDestruction, 0x506ED0);
    RH_ScopedInstall(ReportBulletHit, 0x506EC0);
    RH_ScopedInstall(ReportCollision, 0x506EB0);
    RH_ScopedInstall(ReportFrontendAudioEvent, 0x506EA0);
    RH_ScopedInstall(ReportWeaponEvent, 0x506F40);
    RH_ScopedInstall(ReportDoorMovement, 0x506F50);
    RH_ScopedOverloadedInstall(ReportMissionAudioEvent, "vec", 0x507340, void (CAudioEngine::*)(uint16, CVector&));
    RH_ScopedOverloadedInstall(ReportMissionAudioEvent, "obj", 0x507350, void (CAudioEngine::*)(uint16, CObject*));
    RH_ScopedOverloadedInstall(ReportMissionAudioEvent, "ped", 0x507370, void (CAudioEngine::*)(uint16, CPed*));
    RH_ScopedOverloadedInstall(ReportMissionAudioEvent, "veh", 0x507390, void (CAudioEngine::*)(uint16, CVehicle*));
    RH_ScopedOverloadedInstall(ReportMissionAudioEvent, "phs", 0x5073B0, void (CAudioEngine::*)(uint16, CPhysical*, float, float));
    RH_ScopedInstall(IsLoadingTuneActive, 0x506D90);
    RH_ScopedInstall(PreloadBeatTrack, 0x507F40);
    RH_ScopedInstall(StopCutsceneTrack, 0x507080);
    RH_ScopedInstall(IsRadioRetuneInProgress, 0x506FF0);
}

// 0x5B9C60
bool CAudioEngine::Initialise() {
    CLoadingScreen::Pause();

    // NOTSA: Initialize AudioUtility before `AEAudioHardware` to avoid crash (Division by zero in `GetCurrentTimeInMS`)
    CAEAudioUtility::StaticInitialise();

    if (!AEAudioHardware.Initialise()) {
        NOTSA_LOG_ERR("Failed to initialise Audio Hardware");
        return false;
    }

    m_nBackgroundAudioChannel = AEAudioHardware.AllocateChannels(1);

    if (!AERadioTrackManager.Initialise(m_nBackgroundAudioChannel)) {
        NOTSA_LOG_ERR("Failed to initialise Radio Track Manager");
        return false;
    }

    if (!AECutsceneTrackManager.Initialise(m_nBackgroundAudioChannel)) {
        NOTSA_LOG_ERR("Failed to initialise Cutscene Track Manager");
        return false;
    }

    if (!AEAmbienceTrackManager.Initialise(m_nBackgroundAudioChannel)) {
        NOTSA_LOG_ERR("Failed to initialise Ambience Track Manager");
        return false;
    }

    if (!AESoundManager.Initialise()) {
        NOTSA_LOG_ERR("Failed to initialise Sound Manager");
        return false;
    }

    if (!CAEAudioEntity::StaticInitialise()) {
        return false;
    }

    m_FrontendAE.Initialise();
    CAudioEngine::SetEffectsFaderScalingFactor(0.0f);
    //CAEAudioUtility::StaticInitialise(); // Initialized above
    CAEPedAudioEntity::StaticInitialise();
    CAEPedSpeechAudioEntity::StaticInitialise();
    CAEVehicleAudioEntity::StaticInitialise();
    CAEExplosionAudioEntity::StaticInitialise();
    CAEWeatherAudioEntity::StaticInitialise();
    CAEDoorAudioEntity::StaticInitialise();
    CAEFireAudioEntity::StaticInitialise();
    CAEWaterCannonAudioEntity::StaticInitialise();
    CAEPoliceScannerAudioEntity::StaticInitialise();
    m_ScriptAE.Initialise();
    m_PedlessSpeechAE.Initialise();
    m_CollisionAE.Initialise();

    m_nCurrentRadioStationId = RADIO_INVALID;
    m_bPlayingMissionCompleteTrack = false;
    m_bStoppingMissionCompleteTrack = false;

    CLoadingScreen::Continue();

    return true;
}

// 0x5078F0
void CAudioEngine::InitialisePostLoading() {
    m_FrontendAE.AddAudioEvent(AE_FRONTEND_LOADING_TUNE_STOP);
    AudioEngine.Service();
    CAECollisionAudioEntity::InitialisePostLoading();

    m_GlobalWeaponAE = new CAEGlobalWeaponAudioEntity();
    m_GlobalWeaponAE->Initialise();

    while ( !AEAudioHardware.IsSoundBankLoaded(39, SLOT_LOADING_TUNE_LEFT)
           || !AEAudioHardware.IsSoundBankLoaded(27, 3)
           || !AEAudioHardware.IsSoundBankLoaded(138, 19)
           || !AEAudioHardware.IsSoundBankLoaded(0, 41)
           || !AEAudioHardware.IsSoundBankLoaded(BANK_EXTRAS, SLOT_EXTRAS)
           || !AEAudioHardware.IsSoundBankLoaded(52, 4)
           || !AEAudioHardware.IsSoundBankLoaded(143, SLOT_LOADING_TUNE_RIGHT)
           || !AEAudioHardware.IsSoundBankLoaded(51, 31)
           || !AEAudioHardware.IsSoundBankLoaded(105, 6)
           || !AEAudioHardware.IsSoundBankLoaded(74, 17)
           || !AEAudioHardware.IsSoundBankLoaded(128, 32)
           || !AEAudioHardware.IsSoundBankLoaded(13, 18)
   ) {
        AEAudioHardware.Service();
    }
}

// 0x507CB0
void CAudioEngine::Shutdown() {
    if (AERadioTrackManager.IsRadioOn()) {
        AERadioTrackManager.StopRadio(nullptr, true);
    } else if (AEAmbienceTrackManager.IsAmbienceTrackActive()) {
        StopAmbienceTrack(true);
    } else if (AECutsceneTrackManager.IsCutsceneTrackActive()) {
        AECutsceneTrackManager.StopCutsceneTrack();
    }
    m_CollisionAE.Reset();
    AERadioTrackManager.Reset();
    m_FrontendAE.AddAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP_PAUSED);
    m_FrontendAE.AddAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);
    m_FrontendAE.Reset();
    m_ScriptAE.Reset();
    CAEWeatherAudioEntity::StaticReset();
    CAEPedSpeechAudioEntity::Reset();
    delete m_GlobalWeaponAE;
    AESoundManager.Reset();
    AESoundManager.Terminate();
    AEAudioHardware.Terminate();
    CAEAudioEntity::Shutdown();
    AEUserRadioTrackManager.Shutdown();
}

// 0x506EB0
void CAudioEngine::ReportCollision(CEntity* entity1, CEntity* entity2, eSurfaceType surf1, eSurfaceType surf2, const CVector& point, const CVector* normal, float fCollisionImpact1, float fCollisionImpact2, bool playOnlyOneShotCollisionSound, bool unknown) {
    m_CollisionAE.ReportCollision(entity1, entity2, surf1, surf2, point, normal, fCollisionImpact1, fCollisionImpact2, playOnlyOneShotCollisionSound, unknown);
}

// 0x507350
void CAudioEngine::ReportMissionAudioEvent(uint16 eventId, CObject* object) {
    m_ScriptAE.ReportMissionAudioEvent(static_cast<eAudioEvents>(eventId), object, 0, 1.0f);
}

// 0x506F40
void CAudioEngine::ReportWeaponEvent(int32 audioEvent, eWeaponType weaponType, CPhysical* physical) {
    m_GlobalWeaponAE->AddAudioEvent(static_cast<eAudioEvents>(audioEvent), weaponType, physical);
}

// 0x506F50
void CAudioEngine::ReportDoorMovement(CPhysical* physical) {
    m_DoorAE.AddAudioEvent(AE_ENTRY_EXIT_DOOR_MOVING, physical);
}

// 0x506ED0
void CAudioEngine::ReportObjectDestruction(CEntity* entity) {
    m_CollisionAE.ReportObjectDestruction(entity);
}

// 0x506F70
void CAudioEngine::StopRadio(tVehicleAudioSettings* settings, bool bDuringPause) {
    AERadioTrackManager.StopRadio(settings, bDuringPause);
}

// 0x507150
bool CAudioEngine::IsCutsceneTrackActive() {
    return AECutsceneTrackManager.IsCutsceneTrackActive();
}

// 0x507E10
void CAudioEngine::RetuneRadio(eRadioID radioId) {
    if (!GetBeatTrackStatus())
        AERadioTrackManager.RetuneRadio(radioId);
}

// 0x5072A0
int8 CAudioEngine::GetMissionAudioLoadingStatus(uint8 sampleId) {
    return m_ScriptAE.GetMissionAudioLoadingStatus(sampleId);
}

// 0x5072C0
bool CAudioEngine::IsMissionAudioSampleFinished(uint8 sampleId) {
    return m_ScriptAE.IsMissionAudioSampleFinished(sampleId);
}

// 0x507F40
void CAudioEngine::PreloadBeatTrack(int16 trackId) {
    if (AERadioTrackManager.IsRadioOn()) {
        m_nCurrentRadioStationId = AERadioTrackManager.GetCurrentRadioStationID();
        tVehicleAudioSettings* settings = CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio();
        AERadioTrackManager.StopRadio(settings, true);
        m_FrontendAE.AddAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);
        while (AERadioTrackManager.IsRadioOn()) {
            AERadioTrackManager.Service(AEAudioHardware.GetTrackPlayTime());
            AEAudioHardware.Service();
        }
    } else if (AEAmbienceTrackManager.IsAmbienceTrackActive()) {
        StopAmbienceTrack(true);
    } else {
        if (AECutsceneTrackManager.IsCutsceneTrackActive())
            AECutsceneTrackManager.StopCutsceneTrack();

        while (AECutsceneTrackManager.IsCutsceneTrackActive()) {
            AECutsceneTrackManager.Service(AEAudioHardware.GetTrackPlayTime());
            AEAudioHardware.Service();
        }
    }
    return AECutsceneTrackManager.PreloadBeatTrack(trackId, false);
}

// 0x507200
void CAudioEngine::PauseBeatTrack(bool pause) {
    AECutsceneTrackManager.PauseTrack(pause);
}

bool CAudioEngine::IsLoadingTuneActive() {
    return m_FrontendAE.IsLoadingTuneActive();
}

// 0x506DA0
void CAudioEngine::ResetStatistics() {
    CAERadioTrackManager::ResetStatistics();
}

// 0x506DB0
void CAudioEngine::Restart() {
    AESoundManager.PauseManually(false);
}

// 0x506DE0
void CAudioEngine::SetMusicMasterVolume(int8 volume) {
    AEAudioHardware.SetMusicMasterScalingFactor(static_cast<float>(volume) / 64.0f);
}

// 0x506E10
void CAudioEngine::SetEffectsMasterVolume(int8 volume) {
    AEAudioHardware.SetEffectsMasterScalingFactor(static_cast<float>(volume) / 64.0f);
}

// 0x506E40
void CAudioEngine::SetMusicFaderScalingFactor(float factor) {
    AEAudioHardware.SetMusicFaderScalingFactor(factor);
}

void CAudioEngine::SetEffectsFaderScalingFactor(float factor) {
    AEAudioHardware.SetEffectsFaderScalingFactor(factor);
}

// 0x506E60
void CAudioEngine::SetNonStreamFaderScalingFactor(float factor) {
    AEAudioHardware.SetNonStreamFaderScalingFactor(factor);
}

// 0x506E70
void CAudioEngine::SetStreamFaderScalingFactor(float factor) {
    AEAudioHardware.SetStreamFaderScalingFactor(factor);
}
// 0x506E90
void CAudioEngine::EnableEffectsLoading() {
    AEAudioHardware.EnableEffectsLoading();
}

// 0x506E80
void CAudioEngine::DisableEffectsLoading() {
    AEAudioHardware.DisableEffectsLoading();
}

// 0x506EA0
void CAudioEngine::ReportFrontendAudioEvent(eAudioEvents eventId, float volumeChange, float speed) {
    m_FrontendAE.AddAudioEvent(eventId, volumeChange, speed);
}

// 0x506EC0
void CAudioEngine::ReportBulletHit(CEntity* entity, eSurfaceType surface, const CVector& posn, float angleWithColPointNorm) {
    m_CollisionAE.ReportBulletHit(entity, surface, posn, angleWithColPointNorm);
}

// 0x506EE0
void CAudioEngine::ReportGlassCollisionEvent(eAudioEvents glassSoundType, Const CVector& posn) {
    m_CollisionAE.ReportGlassCollisionEvent(glassSoundType, posn, 0);
}

// 0x506F10
void CAudioEngine::ReportWaterSplash(CVector posn, float volume) {
    m_CollisionAE.ReportWaterSplash(posn, volume);
}

// 0x506F00
void CAudioEngine::ReportWaterSplash(CPhysical* physical, float volume, bool forcePlaySplashSound) {
    m_CollisionAE.ReportWaterSplash(physical, volume, forcePlaySplashSound);
}

// 0x507340
void CAudioEngine::ReportMissionAudioEvent(uint16 eventId, CVector& posn) {
    m_ScriptAE.ReportMissionAudioEvent(static_cast<eAudioEvents>(eventId), posn);
}

// 0x507370
void CAudioEngine::ReportMissionAudioEvent(uint16 eventId, CPed* ped) {
    m_ScriptAE.ReportMissionAudioEvent(static_cast<eAudioEvents>(eventId), ped, 0, 1.0f);
}

// 0x507390
void CAudioEngine::ReportMissionAudioEvent(uint16 eventId, CVehicle* vehicle) {
    m_ScriptAE.ReportMissionAudioEvent(static_cast<eAudioEvents>(eventId), vehicle);
}

// 0x5073B0
void CAudioEngine::ReportMissionAudioEvent(uint16 eventId, CPhysical* physical, float a3, float a4) {
    m_ScriptAE.ReportMissionAudioEvent(static_cast<eAudioEvents>(eventId), physical, a3, a4);
}

// 0x507050
bool CAudioEngine::IsVehicleRadioActive() {
    return AERadioTrackManager.IsVehicleRadioActive();
}

// Used in III
// 0x507060
void CAudioEngine::PlayRadioAnnouncement(uint32 a1) {
    // NOP
}

void CAudioEngine::PreloadCutsceneTrack(int16 trackId, bool wait) {
    if (AERadioTrackManager.IsRadioOn()) {
        m_nCurrentRadioStationId = AERadioTrackManager.GetCurrentRadioStationID();
        tVehicleAudioSettings* settings = CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio();
        AERadioTrackManager.StopRadio(settings, true);
        m_FrontendAE.AddAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);
        while (AERadioTrackManager.IsRadioOn()) {
            AERadioTrackManager.Service(AEAudioHardware.GetTrackPlayTime());
            AEAudioHardware.Service();
        }
    } else {
        if (AEAmbienceTrackManager.IsAmbienceTrackActive()) {
            StopAmbienceTrack(true);
            AECutsceneTrackManager.PreloadCutsceneTrack(trackId, wait);
            return;
        }

        if (AECutsceneTrackManager.IsCutsceneTrackActive())
            AECutsceneTrackManager.StopCutsceneTrack();

        while (AECutsceneTrackManager.IsCutsceneTrackActive()) {
            AECutsceneTrackManager.Service(AEAudioHardware.GetTrackPlayTime());
            AEAudioHardware.Service();
        }
    }

    AECutsceneTrackManager.PreloadCutsceneTrack(trackId, wait);
}

// 0x507070
void CAudioEngine::PlayPreloadedCutsceneTrack() {
    AECutsceneTrackManager.PlayPreloadedCutsceneTrack();
}

// 0x507080
void CAudioEngine::StopCutsceneTrack(bool bWaitForFinish) {
    AECutsceneTrackManager.StopCutsceneTrack();

    if (!bWaitForFinish) {
        if (!m_bPlayingMissionCompleteTrack)
            return;

        m_bStoppingMissionCompleteTrack = true;
        m_bPlayingMissionCompleteTrack = false;
        return;
    }

    while (AECutsceneTrackManager.IsCutsceneTrackActive()) {
        AECutsceneTrackManager.Service(AEAudioHardware.GetTrackPlayTime());
        AEAudioHardware.Service();
    }
    if (m_nCurrentRadioStationId < 0) {
        if (AERadioTrackManager.IsVehicleRadioActive()) {
            tVehicleAudioSettings* settings = CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio();
            AERadioTrackManager.StartRadio(settings);
            m_bPlayingMissionCompleteTrack = false;
            return;
        }
        m_bPlayingMissionCompleteTrack = false;
        return;
    }
    if (AERadioTrackManager.IsVehicleRadioActive())
        AERadioTrackManager.StartRadio(m_nCurrentRadioStationId, 0, 0, 0);
    m_nCurrentRadioStationId = RADIO_INVALID;
    m_bPlayingMissionCompleteTrack = false;
}

// 0x507180
void CAudioEngine::PlayPreloadedBeatTrack(bool a2) {
    AECutsceneTrackManager.PlayPreloadedCutsceneTrack();
    m_bPlayingMissionCompleteTrack = a2;
}

// 0x5071A0
void CAudioEngine::StopBeatTrack() {
    StopCutsceneTrack(true);
}

// 0x5071B0
tBeatInfo* CAudioEngine::GetBeatInfo() {
    AEAudioHardware.GetBeatInfo(&m_BeatInfo);
    return &m_BeatInfo;
}

// win32
// 0x5071D0
bool CAudioEngine::IsBeatInfoPresent() {
    auto beatInfo = GetBeatInfo();
    return beatInfo && beatInfo->IsBeatInfoPresent;
}

// 0x507210
bool CAudioEngine::IsAmbienceTrackActive() {
    return AEAmbienceTrackManager.IsAmbienceTrackActive();
}

// 0x507220
void CAudioEngine::StopAmbienceTrack(bool a1) {
    AEAmbienceTrackManager.StopAmbienceTrack();
    if (a1 && AEAmbienceTrackManager.IsAmbienceTrackActive()) {
        do {
            AEAmbienceTrackManager.Service(AEAudioHardware.GetTrackPlayTime());
            AEAudioHardware.Service();
        } while (AEAmbienceTrackManager.IsAmbienceTrackActive());
    }
}

// 0x507270
bool CAudioEngine::DoesAmbienceTrackOverrideRadio() {
    return AEAmbienceTrackManager.m_OverrideRadio;
}

// 0x507280
bool CAudioEngine::IsAmbienceRadioActive() {
    return AEAmbienceTrackManager.IsAmbienceRadioActive();
}

// 0x507290
void CAudioEngine::PreloadMissionAudio(uint8 slotId, int32 sampleId) {
    m_ScriptAE.PreloadMissionAudio(slotId, sampleId);
}

// 0x5072B0
void CAudioEngine::PlayLoadedMissionAudio(uint8 slotId) {
    m_ScriptAE.PlayLoadedMissionAudio(slotId);
}

// 0x5072D0
int32 CAudioEngine::GetMissionAudioEvent(uint8 sampleId) {
    return m_ScriptAE.GetMissionAudioEvent(sampleId);
}

// 0x5072E0
CVector* CAudioEngine::GetMissionAudioPosition(uint8 sampleId) {
    return m_ScriptAE.GetMissionAudioPosition(sampleId);
}

// 0x5072F0
void CAudioEngine::ClearMissionAudio(uint8 sampleId) {
    m_ScriptAE.ClearMissionAudio(sampleId);
}

// 0x507300
void CAudioEngine::SetMissionAudioPosition(uint8 sampleId, CVector& posn) {
    m_ScriptAE.SetMissionAudioPosition(sampleId, posn);
}

// 0x507310
CVector* CAudioEngine::AttachMissionAudioToPed(uint8 sampleId, CPed* ped) {
    return m_ScriptAE.AttachMissionAudioToPhysical(sampleId, ped);
}

// 0x507320
CVector* CAudioEngine::AttachMissionAudioToObject(uint8 sampleId, CObject* object) {
    return m_ScriptAE.AttachMissionAudioToPhysical(sampleId, object);
}

// 0x507330
CVector* CAudioEngine::AttachMissionAudioToPhysical(uint8 sampleId, CPhysical* physical) {
    return m_ScriptAE.AttachMissionAudioToPhysical(sampleId, physical);
}

// 0x5073C0
void CAudioEngine::SayPedless(eAudioEvents audioEvent, eGlobalSpeechContext gCtx, CEntity* attachTo, uint32 startTimeDelayMs, float probability, bool overrideSilence, bool isForceAudible, bool isFrontEnd) {
    m_PedlessSpeechAE.AddSayEvent(audioEvent, gCtx, attachTo, startTimeDelayMs, probability, overrideSilence, isForceAudible, isFrontEnd);
}

// unused
// 0x5073D0
void CAudioEngine::DisablePoliceScanner(uint8 a1, uint8 a2) {
    CAEPoliceScannerAudioEntity::DisableScanner(a1, a2);
}

// unused
// 0x5073F0
void CAudioEngine::EnablePoliceScanner() {
    CAEPoliceScannerAudioEntity::EnableScanner();
}

// 0x507400
void CAudioEngine::StopPoliceScanner(uint8 a1) {
    CAEPoliceScannerAudioEntity::StopScanner(a1);
}

// 0x507410
void CAudioEngine::StartLoadingTune() {
    m_FrontendAE.AddAudioEvent(AE_FRONTEND_LOADING_TUNE_START);
    AESoundManager.Service();
}

// 0x507430
void CAudioEngine::PauseAllSounds() {
    AEAudioHardware.PauseAllSounds();
}

// 0x507440
void CAudioEngine::ResumeAllSounds() {
    AEAudioHardware.ResumeAllSounds();
}

// 0x507750
void CAudioEngine::Service() {
    ZoneScoped;

    m_FrontendAE.AddAudioEvent(AE_FRONTEND_WAKEUP_AMPLIFIER);
    if (!CTimer::GetIsPaused())
        m_FrontendAE.AddAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP_PAUSED);

    int32 trackPlayTime = AEAudioHardware.GetTrackPlayTime();
    //AEAudioHardware.GetChannelPlayTimes(m_nBackgroundAudioChannel, nullptr); // Does nothing
    if (m_bPlayingMissionCompleteTrack && trackPlayTime == -4) {
        m_bStoppingMissionCompleteTrack = true;
    } else if (m_bStoppingMissionCompleteTrack) {
        if (trackPlayTime == -6) {
            m_bStoppingMissionCompleteTrack = false;
            if (CAERadioTrackManager::IsVehicleRadioActive()) {
                AERadioTrackManager.StartRadio(CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio());
            }
        }
    }

    AECutsceneTrackManager.Service(trackPlayTime);
    AEAmbienceTrackManager.Service(trackPlayTime);
    AERadioTrackManager.Service(trackPlayTime);
    if (FindPlayerPed(0))
        m_GlobalWeaponAE->ServiceAmbientGunFire();

    m_CollisionAE.Service();

    if (CTimer::GetIsPaused()) {
        AEAudioHardware.SetMusicFaderScalingFactor(1.0f);
        AEAudioHardware.SetEffectsFaderScalingFactor(1.0f);
        AEAudioHardware.SetStreamFaderScalingFactor(1.0f);
        m_ScriptAE.Service();
        CAEPedSpeechAudioEntity::Service();
        AESoundManager.Service();
        CAEPoliceScannerAudioEntity::Service();
        return;
    }

    if (!CCutsceneMgr::ms_running) {
        AEAudioHardware.SetStreamFaderScalingFactor(1.0f);
        m_ScriptAE.Service();
        CAEPedSpeechAudioEntity::Service();
        AESoundManager.Service();
        CAEPoliceScannerAudioEntity::Service();
        return;
    }

    AEAudioHardware.SetStreamFaderScalingFactor(0.0f);
    m_ScriptAE.Service();
    CAEPedSpeechAudioEntity::Service();
    AESoundManager.Service();
    CAEPoliceScannerAudioEntity::Service();
}

// 0x5078A0
void CAudioEngine::ServiceLoadingTune(float fScalingFactor) {
    float fCurrentScalingFactor = AEAudioHardware.GetEffectsFaderScalingFactor();
    if (fScalingFactor > fCurrentScalingFactor) {
        float fNewScalingFactor = fCurrentScalingFactor + 0.005f;
        if (fNewScalingFactor < fScalingFactor)
            fScalingFactor = fNewScalingFactor;
    }
    AEAudioHardware.SetEffectsFaderScalingFactor(fScalingFactor);
    AESoundManager.Service();
}


// 0x507A90
void CAudioEngine::Reset() {
    AESoundManager.Service();

    if (AERadioTrackManager.IsRadioOn())
    {
        AERadioTrackManager.StopRadio(nullptr, true);
        while (AERadioTrackManager.IsRadioOn()) {
            AERadioTrackManager.Service(AEAudioHardware.GetTrackPlayTime());
            AEAudioHardware.Service();
        }
    }
    else if (AEAmbienceTrackManager.IsAmbienceTrackActive())
    {
        StopAmbienceTrack(true);
        AEAmbienceTrackManager.StopSpecialMissionAmbienceTrack();
    }
    else if (AECutsceneTrackManager.IsCutsceneTrackActive())
    {
        AECutsceneTrackManager.StopCutsceneTrack();
        while (AECutsceneTrackManager.IsCutsceneTrackActive()) {
            AECutsceneTrackManager.Service(AEAudioHardware.GetTrackPlayTime());
            AEAudioHardware.Service();
        }
    }

    m_CollisionAE.Reset();
    AERadioTrackManager.Reset();
    AEAmbienceTrackManager.Reset();
    m_FrontendAE.AddAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP_PAUSED);
    m_FrontendAE.AddAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);
    m_FrontendAE.Reset();
    m_ScriptAE.Reset();
    if (m_GlobalWeaponAE)
        m_GlobalWeaponAE->Reset();
    CAEWeatherAudioEntity::StaticReset();
    CAEPedSpeechAudioEntity::Reset();
    CAEPoliceScannerAudioEntity::Reset();
    AESoundManager.Reset();
    AESoundManager.Service();
    AESoundManager.PauseManually(true);
    m_nCurrentRadioStationId = RADIO_INVALID;
    m_bPlayingMissionCompleteTrack = false;
    m_bStoppingMissionCompleteTrack = false;
}

// 0x507C30
void CAudioEngine::ResetSoundEffects() {
    AESoundManager.Service();
    m_CollisionAE.Reset();
    m_FrontendAE.AddAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP_PAUSED);
    m_FrontendAE.AddAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);
    m_FrontendAE.Reset();
    m_ScriptAE.Reset();
    CAEWeatherAudioEntity::StaticReset();
    CAEPedSpeechAudioEntity::Reset();
    AESoundManager.Reset();
    AESoundManager.Service();
    AESoundManager.PauseManually(true);
}

// 0x507160
int8 CAudioEngine::GetCutsceneTrackStatus() {
    return AECutsceneTrackManager.GetCutsceneTrackStatus();
}

// 0x507170
int8 CAudioEngine::GetBeatTrackStatus() {
    return AECutsceneTrackManager.GetCutsceneTrackStatus();
}

// 0x507DC0
void CAudioEngine::StartRadio(eRadioID id, int8 bassValue) {
    if (!GetBeatTrackStatus())
        AERadioTrackManager.StartRadio(id, bassValue, 0, 0);
}

// 0x507DF0
void CAudioEngine::StartRadio(tVehicleAudioSettings* settings) {
    if (!GetBeatTrackStatus())
        AERadioTrackManager.StartRadio(settings);
}

// 0x506F80
void CAudioEngine::SetRadioAutoRetuneOnOff(bool enable) {
    AERadioTrackManager.SetRadioAutoRetuneOnOff(enable);
}

// 0x506F90
void CAudioEngine::SetBassEnhanceOnOff(bool enable) {
    AERadioTrackManager.SetBassEnhanceOnOff(enable);
}

// 0x506FA0
void CAudioEngine::SetRadioBassSetting(int8 nBassSet) {
    AERadioTrackManager.SetBassSetting(nBassSet, 1.0f);
}

// 0x506FC0
void CAudioEngine::InitialiseRadioStationID(eRadioID id) {
    AERadioTrackManager.InitialiseRadioStationID(id);
}

// 0x506FD0
bool CAudioEngine::IsRadioOn() {
    return AERadioTrackManager.IsRadioOn();
}

// 0x506FE0
bool CAudioEngine::HasRadioRetuneJustStarted() {
    return AERadioTrackManager.HasRadioRetuneJustStarted();
}

// 0x506FF0
bool CAudioEngine::IsRadioRetuneInProgress() {
    return m_FrontendAE.IsRadioTuneSoundActive();
}

// 0x507000
const GxtChar* CAudioEngine::GetRadioStationName(eRadioID id) {
    return AERadioTrackManager.GetRadioStationName(id);
}

// 0x507010
void CAudioEngine::GetRadioStationNameKey(eRadioID id, char* outStr) {
    AERadioTrackManager.GetRadioStationNameKey(id, outStr);
}

// 0x507020
int32* CAudioEngine::GetRadioStationListenTimes() {
    return AERadioTrackManager.GetRadioStationListenTimes();
}

// 0x507040
eRadioID CAudioEngine::GetCurrentRadioStationID() {
    return AERadioTrackManager.GetCurrentRadioStationID();
}

// 0x507030
void CAudioEngine::DisplayRadioStationName() {
    AERadioTrackManager.DisplayRadioStationName();
}
