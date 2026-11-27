#pragma once

#include "eAudioEvents.h"
#include "AEExplosionAudioEntity.h"
#include "AEDoorAudioEntity.h"

class CEntity;
class CVector;

struct CAudioLink {
    //! Position of a link that has none (it's played as a frontend sound then)
    static constexpr CVector UNSET_POSITION{ -1000.0f, -1000.0f, -1000.0f };

    CVector       Position{};
    CEntity*      Entity{};     //!< If set, the sound follows this entity
    CAESound*     Sound{};      //!< The playing sound (Only for the two single-sound links)
    int32         AudioEvent{}; //!< The script slot event that was loaded
    eSoundBankS32 BankID{};
    int32         SoundID{}; //!< Sound in the bank, negative if the whole bank was loaded

    void Init() {
        AudioEvent = AE_UNDEFINED;
        BankID     = SND_BANK_UNK;
        SoundID    = -1;
        Entity     = nullptr;
        Sound      = nullptr;
        Position   = UNSET_POSITION;
    }

    //! NOTSA
    bool HasPosition() const { return Position != UNSET_POSITION && !Position.IsZero(); }
};

VALIDATE_SIZE(CAudioLink, 0x20);

class NOTSA_EXPORT_VTABLE CAEScriptAudioEntity : public CAEAudioEntity {
public:
    CAEScriptAudioEntity() = default; // 0x5074D0

    void Initialise();
    void Reset();

    void AddAudioEvent(int32);
    void AttachMissionAudioToPhysical(uint8 sampleId, CPhysical* physical);

    void ClearMissionAudio(uint8 sampleId);

    bool     IsMissionAudioSampleFinished(uint8 sampleId);
    bool     GetMissionAudioLoadingStatus(uint8 sampleId);
    int32    GetMissionAudioEvent(uint8 sampleId);
    void     SetMissionAudioPosition(uint8 sampleId, const CVector& posn);
    CVector* GetMissionAudioPosition(uint8 sampleId);

    void PlayMissionBankSound(eAudioEvents eventId, const CVector& posn, CPhysical* physical, int16 sfxId, uint8 linkId, bool onlyIfNotPlaying = false, float volume = 0.0f, float maxDistance = 2.0f, float speed = 1.0f);
    void PlayResidentSoundEvent(eSoundBankSlot slot, eSoundBank bank, eSoundID sfx, eAudioEvents event, const CVector& posn, CPhysical* physical, float vol, float speed = 1.0f, int16 playPosn = 0, float maxDistance = 1.0f);
    void PlayLoadedMissionAudio(uint8 sampleId);
    void PreloadMissionAudio(uint8 sampleId, eAudioEvents scriptId);

    void ProcessMissionAudioEvent(eAudioEvents eventId, const CVector& posn, CPhysical* physical, float volume = 0.0f, float speed = 1.0f);
    void ReportMissionAudioEvent(eAudioEvents eventId, CPhysical* physical, float volume = 0.0f, float speed = 1.0f);
    void ReportMissionAudioEvent(eAudioEvents eventId, const CVector& posn);

    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;
    void Service();

    static void InjectHooks();

private:
    //! NOTSA
    void UpdateMiniGameSound(CAESound* sound);

private:
    static constexpr auto  MISSION_AUDIO_COUNT = 4;
    static constexpr uint8 BANK_LINK_A         = 2; //!< The first of the two links that hold a whole bank (The two before it hold one sound each)
    static constexpr uint8 BANK_LINK_B         = 3;

    //! Values of `m_MissionSoundEnvelopePhase`
    static constexpr auto ENVELOPE_FADE_IN  = 1.0f;
    static constexpr auto ENVELOPE_FADE_OUT = 2.0f;

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
};

VALIDATE_SIZE(CAEScriptAudioEntity, 0x21C);
