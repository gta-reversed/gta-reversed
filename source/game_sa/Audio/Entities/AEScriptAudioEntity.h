#pragma once

#include "eAudioEvents.h"
#include "AEExplosionAudioEntity.h"
#include "AEDoorAudioEntity.h"

class CEntity;
class CVector;

struct CAudioLink {
    CVector    m_vPosition;
    CEntity*   m_pEntity;
    CAESound*  m_Sound;
    int32      m_nAudioEvent;
    eSoundBank m_nBankId;
    int32      m_nBankSlotId;

    void Init() {
        m_nAudioEvent = -1;
        m_nBankId     = eSoundBank::SND_BANK_UNK;
        m_nBankSlotId = -1;
        m_pEntity     = nullptr;
        m_Sound       = nullptr;
        m_vPosition   = CVector(-1000.0f, -1000.0f, -1000.0f);
    }
};

VALIDATE_SIZE(CAudioLink, 0x20);

class NOTSA_EXPORT_VTABLE CAEScriptAudioEntity : public CAEAudioEntity {
private:
    static constexpr auto MISSION_AUDIO_COUNT = 4;
    static constexpr auto WAVLINK_UNK_2       = 2;

public:
    bool                                        m_MiniGameOneShotTriggered{};
    bool                                        m_InACrane{};
    int8                                        m_MeshGateTriggerCount{};
    uint32                                      m_LastMiniGameLoopTriggerTimeMs{};
    float                                       m_CraneVolume{};
    float                                       m_CraneFrequencyScaling{ 1.0f };
    float                                       m_MissionSoundEnvelopePhase{};
    std::array<CAudioLink, MISSION_AUDIO_COUNT> m_WavLinks{};
    CPhysical*                                  m_Crane{};
    CAEDoorAudioEntity                          m_DoorAudioEntity{};
    CAEExplosionAudioEntity                     m_ExplosionAudioEntity{};

public:
    CAEScriptAudioEntity() = default; // 0x5074D0

    void Initialise();
    void Reset();

    void AddAudioEvent(int32);
    void AttachMissionAudioToPhysical(uint8 sampleId, CPhysical* physical);

    void ClearMissionAudio(uint8 id);

    bool IsMissionAudioSampleFinished(uint8 sampleId);
    bool GetMissionAudioLoadingStatus(uint8 sampleId);
    int32 GetMissionAudioEvent(uint8 sampleId);
    void SetMissionAudioPosition(uint8 sampleId, const CVector& posn);
    CVector* GetMissionAudioPosition(uint8 sampleId);

    void PlayMissionBankSound(eAudioEvents eventId, const CVector& posn, CPhysical* physical, int16 sfxId, uint8 linkId, uint8 a7 = 0, float volume = 0.0f, float maxDistance = 2.0f, float speed = 1.0f);
    void PlayResidentSoundEvent(eSoundBankSlot slot, eSoundBank bank, eSoundID sfx, eAudioEvents event, const CVector& posn, CPhysical* physical, float vol, float speed = 1.0f, int16 playPosn = 0, float maxDistance = 1.0f);
    void PlayLoadedMissionAudio(uint8);
    void PreloadMissionAudio(uint8 slotId, eAudioEvents scriptId);

    void ProcessMissionAudioEvent(eAudioEvents eventId, const CVector& posn, CPhysical* physical, float volume = 0.0f, float speed = 1.0f);
    void ReportMissionAudioEvent(eAudioEvents eventId, CPhysical* physical, float volume = 0.0f, float speed = 1.0f);
    void ReportMissionAudioEvent(eAudioEvents eventId, const CVector& posn);

    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;
    void Service();

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CAEScriptAudioEntity* Constructor();
};

VALIDATE_SIZE(CAEScriptAudioEntity, 0x21C);
