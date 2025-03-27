#pragma once

#include "eAudioEvents.h"
#include "AEExplosionAudioEntity.h"
#include "AEDoorAudioEntity.h"

class CEntity;
class CVector;

struct CAudioLink {
    CVector  m_vPosition;
    CEntity* m_pEntity;
    CAESound*m_Sound;
    int32    m_nAudioEvent;
    int32    m_nBankId;
    int32    m_nBankSlotId;

    void Init() {
        m_nAudioEvent = -1;
        m_nBankId     = -1;
        m_nBankSlotId = -1;
        m_pEntity     =  nullptr;
        m_Sound       =  nullptr;
        m_vPosition   = CVector(-1000.0f, -1000.0f, -1000.0f);
    }
};

VALIDATE_SIZE(CAudioLink, 0x20);

class NOTSA_EXPORT_VTABLE CAEScriptAudioEntity : public CAEAudioEntity {
public:
    static constexpr auto MISSION_AUDIO_COUNT = 4;

public:
    uint8                   field_7C;
    uint8                   field_7D;
    uint8                   field_7E;
    uint8                   field_7F;
    uint32                  m_nLastTimeHornPlayed;
    float                   m_fVolume;
    float                   m_fSpeed;
    float                   field_8C;
    CAudioLink              wavLinks[MISSION_AUDIO_COUNT];
    CPhysical*              m_Physical;
    CAEDoorAudioEntity      m_GarageAudio;
    CAEExplosionAudioEntity m_ExplosionAudio;

public:
    CAEScriptAudioEntity();

    void Initialise();
    void Reset();

    void AddAudioEvent(int32);
    CVector* AttachMissionAudioToPhysical(uint8 sampleId, CPhysical* physical);

    void ClearMissionAudio(uint8 id);

    bool IsMissionAudioSampleFinished(uint8 sampleId);
    int8 GetMissionAudioLoadingStatus(uint8 sampleId);
    int32 GetMissionAudioEvent(uint8 sampleId);
    void SetMissionAudioPosition(uint8 sampleId, CVector& posn);
    CVector* GetMissionAudioPosition(uint8);

    void PlayMissionBankSound(uint8 sampleId, CVector& posn, CPhysical* physical, int16 sfxId, uint8 linkId, uint8 a7 = 0, float volume = 0.0f, float maxDistance = 2.0f, float speed = 1.0f);
    void PlayResidentSoundEvent(int16 bankSlotId, int16 bankId, int16 sfxId, uint16 event, CVector& posn, CPhysical* physical, float vol, float speed = 1.0f, int16 playPosn = 0, float maxDistance = 1.0f);
    void PlayLoadedMissionAudio(uint8);
    void PreloadMissionAudio(uint8 slotId, int32 sampleId);

    void ProcessMissionAudioEvent(eAudioEvents eventId, CVector& posn, CPhysical* physical, float volume = 0.0f, float speed = 1.0f);
    void ReportMissionAudioEvent(eAudioEvents eventId, CPhysical* physical, float volume = 0.0f, float speed = 1.0f);
    void ReportMissionAudioEvent(eAudioEvents eventId, CVector& posn);

    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;
    void Service();

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CAEScriptAudioEntity* Constructor();
};

VALIDATE_SIZE(CAEScriptAudioEntity, 0x21C);
