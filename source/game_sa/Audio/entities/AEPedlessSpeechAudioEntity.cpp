#include "StdInc.h"

#include "AEPedlessSpeechAudioEntity.h"

// 0x4E6070
CAEPedlessSpeechAudioEntity::CAEPedlessSpeechAudioEntity() : CAEPedSpeechAudioEntity() {
    // NOP
}

// 0x4E4E10
void CAEPedlessSpeechAudioEntity::Initialise() {
    m_pEntity                  = nullptr;
    m_Entity                   = nullptr;
    m_pSound                   = nullptr;
    m_fVoiceVolume             = -100.0f;
    m_nCurrentPhraseId         = -1;
    m_bTalking                 = false;
    m_bSpeechDisabled          = false;
    m_bSpeechForScriptsDisabled= false;
    m_nVocalEnableFlag         = 0;
    field_9C                   = 0;
    m_nVoiceGender             = 0;
    m_nVoiceType               = 4;
    f90                        = true;

    memset(field_B4, 0, sizeof(field_B4));

}

// 0x4E4E80
void CAEPedlessSpeechAudioEntity::StopCurrentSpeech() {
    plugin::CallMethod<0x4E4E80, CAEPedlessSpeechAudioEntity*>(this);
}

// 0x4E60D0
int16 CAEPedlessSpeechAudioEntity::AddSayEvent(int32 a1, int16 a2, CEntity* entity, uint32 playOffset, float a6, bool a3, bool a8, bool a9) {
    return plugin::CallMethodAndReturn<int16, 0x4E60D0, CAEPedlessSpeechAudioEntity*, int32, int16, CEntity*, uint32, float, bool, bool, bool>(this, a1, a2, entity, playOffset, a6, a3, a8, a9);
}

// 0x4E4D10
void CAEPedlessSpeechAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    plugin::CallMethod<0x4E4D10, CAEPedlessSpeechAudioEntity*, CAESound*, int16>(this, sound, curPlayPos);
}

// 0x4E6300
void CAEPedlessSpeechAudioEntity::Terminate() {
    plugin::CallMethod<0x4E6300, CAEPedlessSpeechAudioEntity*>(this);
}

// 0x4E6380
void CAEPedlessSpeechAudioEntity::PlayLoadedSound() {
    plugin::CallMethod<0x4E6380, CAEPedlessSpeechAudioEntity*>(this);
}

// 0x4E6090
int16 CAEPedlessSpeechAudioEntity::GetAllocatedVoice() {
    return -1;
}

// 0x4E60A0
bool CAEPedlessSpeechAudioEntity::WillPedChatAboutTopic(int16) {
    return false;
}

// 0x4E60B0
int16 CAEPedlessSpeechAudioEntity::GetPedType() {
    return -1;
}

// 0x4E60C0
bool CAEPedlessSpeechAudioEntity::IsPedFemaleForAudio() {
    return false;
}

void CAEPedlessSpeechAudioEntity::InjectHooks() {
    RH_ScopedClass(CAEPedlessSpeechAudioEntity);
    RH_ScopedCategory("Audio/Entities");

    // RH_ScopedInstall(Constructor, 0x4E6070);
    RH_ScopedInstall(GetPedType, 0x4E60B0);
    RH_ScopedInstall(IsPedFemaleForAudio, 0x4E60C0);
    // RH_ScopedInstall(Initialise, 0x4E4E10);
    // RH_ScopedInstall(StopCurrentSpeech, 0x4E4E80);
    // RH_ScopedInstall(AddSayEvent, 0x4E60D0);
    // RH_ScopedInstall(UpdateParameters_Reversed, 0x4E4D10);
    // RH_ScopedInstall(Terminate_Reversed, 0x4E6300);
    // RH_ScopedInstall(PlayLoadedSound_Reversed, 0x4E6380);
    RH_ScopedInstall(GetAllocatedVoice_Reversed, 0x4E6090);
    RH_ScopedInstall(WillPedChatAboutTopic_Reversed, 0x4E60A0);
}

// 0x4E6070
CAEPedlessSpeechAudioEntity* CAEPedlessSpeechAudioEntity::Constructor() {
    this->CAEPedlessSpeechAudioEntity::CAEPedlessSpeechAudioEntity();
    return this;
}

//
CAEPedlessSpeechAudioEntity::~CAEPedlessSpeechAudioEntity() {}

//
CAEPedlessSpeechAudioEntity* CAEPedlessSpeechAudioEntity::Destructor() {
    this->CAEPedlessSpeechAudioEntity::~CAEPedlessSpeechAudioEntity();
    return this;
}

// 0x4E4D10
void CAEPedlessSpeechAudioEntity::UpdateParameters_Reversed(CAESound* sound, int16 curPlayPos) {
    CAEPedlessSpeechAudioEntity::UpdateParameters(sound, curPlayPos);
}

// 0x4E6300
void CAEPedlessSpeechAudioEntity::Terminate_Reversed() {
    CAEPedlessSpeechAudioEntity::Terminate();
}

// 0x4E6380
void CAEPedlessSpeechAudioEntity::PlayLoadedSound_Reversed() {
    CAEPedlessSpeechAudioEntity::PlayLoadedSound();
}

// 0x4E6090
int16 CAEPedlessSpeechAudioEntity::GetAllocatedVoice_Reversed() {
    return CAEPedlessSpeechAudioEntity::GetAllocatedVoice();
}

// 0x4E60A0
bool CAEPedlessSpeechAudioEntity::WillPedChatAboutTopic_Reversed(int16 topic) {
    return CAEPedlessSpeechAudioEntity::WillPedChatAboutTopic(topic);
}
