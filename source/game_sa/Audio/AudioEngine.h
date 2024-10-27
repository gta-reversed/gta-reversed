#pragma once

#include "AEFrontendAudioEntity.h"
#include "AEScriptAudioEntity.h"
#include "AERadioTrackManager.h"
#include "AECollisionAudioEntity.h"
#include "AEGlobalWeaponAudioEntity.h"
#include "AEPedlessSpeechAudioEntity.h"
#include "AETrackLoader.h"

class CEntity;
class CColPoint;
class CVector;

class tBeatInfo {
public:
    tTrackInfo::tBeat BeatWindow[20];
    int32             IsBeatInfoPresent;
    int32             BeatTypeThisFrame;
    int32             BeatNumber;
};
VALIDATE_SIZE(tBeatInfo, 0xAC);

class CAudioEngine {
public:
    bool                        m_bPlayingMissionCompleteTrack;
    bool                        m_bStoppingMissionCompleteTrack;
    eRadioID                    m_nCurrentRadioStationId;
    eRadioID                    m_nSavedRadioStationId;
    int32                       m_nBackgroundAudioChannel;
    tBeatInfo                   m_BeatInfo;
    CAEFrontendAudioEntity      m_FrontendAE;
    CAEScriptAudioEntity        m_ScriptAE;
    CAECollisionAudioEntity     m_CollisionAE;
    CAEGlobalWeaponAudioEntity* m_GlobalWeaponAE;
    CAEPedlessSpeechAudioEntity m_PedlessSpeechAE;
    CAEDoorAudioEntity          m_DoorAE;

public:
    static void InjectHooks();

    CAudioEngine() = default;  // 0x507670
    ~CAudioEngine() = default; // 0x506CD0

    bool Initialise();
    void InitialisePostLoading();
    void Shutdown();

    void Reset();
    void ResetStatistics();
    void ResetSoundEffects();

    void Restart();

    bool IsLoadingTuneActive();
    static bool IsRadioOn();
    bool IsRadioRetuneInProgress();
    bool IsVehicleRadioActive();
    static bool IsCutsceneTrackActive();
    bool IsBeatInfoPresent();
    static bool IsAmbienceTrackActive();
    static bool IsAmbienceRadioActive();
    bool IsMissionAudioSampleFinished(uint8 sampleId);

    void SetMusicMasterVolume(int8);
    void SetEffectsMasterVolume(int8);
    void SetMusicFaderScalingFactor(float);
    void SetEffectsFaderScalingFactor(float);
    void ServiceLoadingTune(float);
    void SetStreamFaderScalingFactor(float);
    void SetNonStreamFaderScalingFactor(float);

    static void EnableEffectsLoading();
    static void DisableEffectsLoading();

    void ReportCollision(CEntity* entity1, CEntity* entity2, eSurfaceType surf1, eSurfaceType surf2, CVector& point, CVector* normal, float fCollisionImpact1, float fCollisionImpact2, bool playOnlyOneShotCollisionSound, bool unknown);
    void ReportBulletHit(CEntity* entity, eSurfaceType surface, const CVector& posn, float angleWithColPointNorm);
    void ReportObjectDestruction(CEntity* entity);
    void ReportGlassCollisionEvent(eAudioEvents glassSoundType, Const CVector& posn);
    void ReportWaterSplash(CVector posn, float volume);
    void ReportWaterSplash(CPhysical* physical, float volume, bool forcePlaySplashSound);
    void ReportWeaponEvent(int32_t audioEvent, eWeaponType weaponType, CPhysical* physical);
    void ReportDoorMovement(CPhysical* physical);
    void ReportMissionAudioEvent(uint16 eventId, CVector& posn);
    void ReportMissionAudioEvent(uint16 eventId, CObject* object);
    void ReportMissionAudioEvent(uint16 eventId, CPed* ped);
    void ReportMissionAudioEvent(uint16 eventId, CVehicle* vehicle);
    void ReportMissionAudioEvent(uint16 eventId, CPhysical* physical, float a3, float a4);
    void ReportFrontendAudioEvent(eAudioEvents eventId, float volumeChange = 0.0f, float speed = 1.0f);

    void InitialiseRadioStationID(eRadioID id);
    void StartRadio(tVehicleAudioSettings* settings);
    void StartRadio(eRadioID id, int8 bassValue);
    void StopRadio(tVehicleAudioSettings* settings, bool bDuringPause);
    void SetRadioAutoRetuneOnOff(bool);
    void SetBassEnhanceOnOff(bool enable);
    void SetRadioBassSetting(int8);
    bool HasRadioRetuneJustStarted();
    const GxtChar* GetRadioStationName(eRadioID id);
    void GetRadioStationNameKey(eRadioID id, char* outStr);
    int32* GetRadioStationListenTimes();
    void DisplayRadioStationName();
    eRadioID GetCurrentRadioStationID();
    void PlayRadioAnnouncement(uint32);
    void RetuneRadio(eRadioID id);

    void PreloadCutsceneTrack(int16 trackId, bool wait);
    static void PlayPreloadedCutsceneTrack();
    void StopCutsceneTrack(bool);
    int8 GetCutsceneTrackStatus();
    int8 GetBeatTrackStatus();

    void PlayPreloadedBeatTrack(bool a2);
    void StopBeatTrack();
    tBeatInfo* GetBeatInfo();
    void PauseBeatTrack(bool pause);

    void PreloadBeatTrack(int16 trackId);
    void StopAmbienceTrack(bool a1);
    static bool DoesAmbienceTrackOverrideRadio();
    void PreloadMissionAudio(uint8 slotId, int32 sampleId);
    int8 GetMissionAudioLoadingStatus(uint8 sampleId);
    void PlayLoadedMissionAudio(uint8 slotId);
    int32 GetMissionAudioEvent(uint8 sampleId);
    CVector* GetMissionAudioPosition(uint8 sampleId);
    void ClearMissionAudio(uint8 sampleId);
    void SetMissionAudioPosition(uint8 sampleId, CVector& posn);

    CVector* AttachMissionAudioToPed(uint8 sampleId, CPed* ped);
    CVector* AttachMissionAudioToObject(uint8 sampleId, CObject* object);
    CVector* AttachMissionAudioToPhysical(uint8 sampleId, CPhysical* physical);

    void SayPedless(eAudioEvents audioEvent, eGlobalSpeechContext gCtx, CEntity* attachTo, uint32 startTimeDelayMs, float probability, bool overrideSilence, bool isForceAudible, bool isFrontEnd);

    void EnablePoliceScanner();
    void DisablePoliceScanner(uint8, uint8);

    void StopPoliceScanner(uint8);
    void StartLoadingTune();

    static void PauseAllSounds();
    static void ResumeAllSounds();

    void Service();

#ifdef ANDROID
    void Save();
    void Load();
#endif
private: // Wrappers for hooks

    // 0x507670
    CAudioEngine* Constructor() {
        this->CAudioEngine::CAudioEngine();
        return this;
    }
    // 0x506CD0
    CAudioEngine* Destructor() {
        this->CAudioEngine::~CAudioEngine();
        return this;
    }
};

VALIDATE_SIZE(CAudioEngine, 0x1FD8);
VALIDATE_OFFSET(CAudioEngine, m_FrontendAE, 0x0B4);
VALIDATE_OFFSET(CAudioEngine, m_ScriptAE, 0x2A0);
VALIDATE_OFFSET(CAudioEngine, m_CollisionAE, 0x4BC);
VALIDATE_OFFSET(CAudioEngine, m_GlobalWeaponAE, 0x1E34);
VALIDATE_OFFSET(CAudioEngine, m_PedlessSpeechAE, 0x1E38);
VALIDATE_OFFSET(CAudioEngine, m_DoorAE, 0x1F50);

extern CAudioEngine& AudioEngine;
