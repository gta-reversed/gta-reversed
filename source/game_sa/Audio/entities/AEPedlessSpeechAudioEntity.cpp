#include "StdInc.h"

#include "AEPedlessSpeechAudioEntity.h"


void CAEPedlessSpeechAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEPedlessSpeechAudioEntity, 0x85F330, 8);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Constructor, 0x4E6070);
    RH_ScopedInstall(Initialise, 0x4E4E10);
    RH_ScopedInstall(StopCurrentSpeech, 0x4E4E80);
    RH_ScopedInstall(AddSayEvent, 0x4E60D0);
    RH_ScopedVMTInstall(IsPedFemaleForAudio, 0x4E60C0);
    RH_ScopedVMTInstall(GetPedType, 0x4E60B0);
    RH_ScopedVMTInstall(UpdateParameters, 0x4E4D10);
    RH_ScopedVMTInstall(Terminate, 0x4E6300);
    RH_ScopedVMTInstall(PlayLoadedSound, 0x4E6380);
    RH_ScopedVMTInstall(GetAllocatedVoice, 0x4E6090);
    RH_ScopedVMTInstall(WillPedChatAboutTopic, 0x4E60A0);
}

// 0x4E4E10
void CAEPedlessSpeechAudioEntity::Initialise() {
    *this = CAEPedlessSpeechAudioEntity{PEDTYPE_GFD};
}

// 0x4E4E80
void CAEPedlessSpeechAudioEntity::StopCurrentSpeech() {
    if (!m_IsInitialized || !m_IsPlayingSpeech) {
        return;
    }
    auto* const speech = GetCurrentSpeech();
    if (speech->Status == CAEPedSpeechSlot::eStatus::PLAYING) {
        if (const auto s = std::exchange(m_Sound, nullptr)) {
            s->StopSoundAndForget();
        }
    }
    *speech = {};
}

// 0x4E60D0
int16 CAEPedlessSpeechAudioEntity::AddSayEvent(eAudioEvents audioEvent, eGlobalSpeechContext gCtx, CEntity* attachTo, uint32 startTimeDelayMs, float probability, bool overrideSilence, bool isForceAudible, bool isFrontEnd) {
    if (!attachTo) {
        return -1;
    }
    m_AttachedTo = attachTo;
    return I_AddSayEvent<true>(attachTo->GetPosition(), audioEvent, gCtx, startTimeDelayMs, probability, overrideSilence, isForceAudible, isFrontEnd);
}

// 0x4E4D10
void CAEPedlessSpeechAudioEntity::UpdateParameters(CAESound* sound, int16 playTime) {
    return I_UpdateParameters<true>(sound, playTime);
}

// 0x4E6300
void CAEPedlessSpeechAudioEntity::Terminate() {
    StopCurrentSpeech();
    *this = CAEPedlessSpeechAudioEntity{};
}

// 0x4E6380
void CAEPedlessSpeechAudioEntity::PlayLoadedSound() {
    if (m_AttachedTo) {
        I_PlayLoadedSound<true>(m_AttachedTo);
        m_AttachedTo = nullptr;
    } else {
        StopCurrentSpeech();
    }
}

CAEPedlessSpeechAudioEntity* CAEPedlessSpeechAudioEntity::Constructor() {
    this->CAEPedlessSpeechAudioEntity::CAEPedlessSpeechAudioEntity();
    return this;
}

CAEPedlessSpeechAudioEntity* CAEPedlessSpeechAudioEntity::Destructor() {
    this->CAEPedlessSpeechAudioEntity::~CAEPedlessSpeechAudioEntity();
    return this;
}
