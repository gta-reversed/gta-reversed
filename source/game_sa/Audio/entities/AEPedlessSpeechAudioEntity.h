#pragma once

#include "AEPedSpeechAudioEntity.h"

class NOTSA_EXPORT_VTABLE CAEPedlessSpeechAudioEntity final : public CAEPedSpeechAudioEntity {
public:
    char         gap100[20];
    CEntity::Ref m_AttachedTo{};

public:
    using CAEPedSpeechAudioEntity::CAEPedSpeechAudioEntity;
    CAEPedlessSpeechAudioEntity() = default; // 0x4E6070
    ~CAEPedlessSpeechAudioEntity() = default;

    void Initialise();
    void StopCurrentSpeech(); // OG moment: Function 'StopCurrentSpeech' hides a non-virtual function from class 'CAEPedSpeechAudioEntity'
    int16 AddSayEvent(eAudioEvents audioEvent, eGlobalSpeechContext gCtx, CEntity* attachTo, uint32 startTimeDelayMs, float probability, bool overrideSilence, bool isForceAudible, bool isFrontEnd);

    void UpdateParameters(CAESound* sound, int16 playTime) override;
    void AddScriptSayEvent(eAudioEvents audioEvent, eAudioEvents scriptID, bool overrideSilence, bool isForceAudible, bool isFrontEnd) override { /* NOP */ } // 0x4E6080
    void Terminate() override;
    void PlayLoadedSound() override;
    ePedSpeechVoiceS16 GetAllocatedVoice() override { return VOICE_UNK; } // // 0x4E6090
    bool WillPedChatAboutTopic(int16 topic) override { return false; } // 0x4E60A0
    eAudioPedType GetPedType() override { return PED_TYPE_UNK; } // 0x4E60B0
    bool IsPedFemaleForAudio() override { return false; } // 0x4E60C0

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CAEPedlessSpeechAudioEntity* Constructor();
    CAEPedlessSpeechAudioEntity* Destructor();
};
VALIDATE_SIZE(CAEPedlessSpeechAudioEntity, 0x118);
