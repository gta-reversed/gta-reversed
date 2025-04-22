#include "StdInc.h"
#include <bitset>
#include "AEPedSpeechAudioEntity.h"
#include "AEPedlessSpeechAudioEntity.h"
#include "AEAudioUtility.h"
#include "PedClothesDesc.h"
#include <AEAudioHardware.h>
#include <Audio/Enums/eSoundBankSlot.h>

#include "PedSpeechAudioEntityLUTs.inc.h"

void CAEPedSpeechAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEPedSpeechAudioEntity, 0x85F310, 8);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Constructor, 0x4E4F10);
    RH_ScopedInstall(IsGlobalContextImportantForInterupting, 0x4E4600);
    RH_ScopedInstall(IsGlobalContextUberImportant, 0x4E46F0);
    RH_ScopedInstall(GetNextMoodToUse, 0x4E4700);
    RH_ScopedInstall(GetVoiceForMood, 0x4E4760);
    RH_ScopedInstall(CanWePlayScriptedSpeech, 0x4E4950);
    RH_ScopedInstall(GetSpeechContextVolumeOffset, 0x4E4AE0);
    RH_ScopedInstall(RequestPedConversation, 0x4E50E0);
    RH_ScopedInstall(ReleasePedConversation, 0x4E52A0);
    RH_ScopedInstall(GetCurrentCJMood, 0x4E53B0);
    RH_ScopedInstall(StaticInitialise, 0x5B98C0);
    RH_ScopedInstall(GetSpecificSpeechContext, 0x4E4470);
    RH_ScopedInstall(Service, 0x4E3710);
    RH_ScopedInstall(Reset, 0x4E37B0);
    RH_ScopedInstall(ReservePedConversationSpeechSlots, 0x4E37F0);
    RH_ScopedInstall(ReservePlayerConversationSpeechSlot, 0x4E3870);
    RH_ScopedInstall(RequestPlayerConversation, 0x4E38C0);
    RH_ScopedInstall(ReleasePlayerConversation, 0x4E3960);
    RH_ScopedInstall(SetUpConversation, 0x4E3A00);
    RH_ScopedInstall(GetAudioPedType, 0x4E3C60);
    RH_ScopedInstall(GetVoice, 0x4E3CD0);
    RH_ScopedInstall(DisableAllPedSpeech, 0x4E3EB0);
    RH_ScopedInstall(IsGlobalContextPain, 0x4E44F0);
    RH_ScopedInstall(SetCJMood, 0x4E3ED0);
    RH_ScopedInstall(EnableAllPedSpeech, 0x4E3EC0);
    RH_ScopedInstall(IsCJDressedInForGangSpeech, 0x4E4270);
    RH_ScopedInstall(GetSexForSpecialPed, 0x4E4260);
    RH_ScopedInstall(IsGlobalContextImportantForWidescreen, 0x4E46B0);
    RH_ScopedInstall(GetRepeatTime, 0x4E47E0);
    RH_ScopedInstall(LoadAndPlaySpeech, 0x4E4840);
    RH_ScopedInstall(GetNumSlotsPlayingContext, 0x4E49B0);
    RH_ScopedInstall(GetNextPlayTime, 0x4E49E0);
    RH_ScopedInstall(SetNextPlayTime, 0x4E4A20);
    RH_ScopedInstall(DisablePedSpeech, 0x4E56D0);
    RH_ScopedInstall(DisablePedSpeechForScriptSpeech, 0x4E5700);
    RH_ScopedInstall(CanPedSayGlobalContext, 0x4E5730);
    RH_ScopedInstall(GetVoiceAndTypeFromModel, 0x4E58C0);
    RH_ScopedInstall(GetSoundAndBankIDs, 0x4E5920);
    RH_ScopedInstall(CanWePlayGlobalSpeechContext, 0x4E5F10);
    RH_ScopedInstall(AddSayEvent, 0x4E6550);
    RH_ScopedInstall(Initialise, 0x4E68D0);
    RH_ScopedInstall(CanPedHoldConversation, 0x4E69E0);
    RH_ScopedInstall(IsGlobalContextImportantForStreaming, 0x4E4510);
    RH_ScopedInstall(EnablePedSpeech, 0x4E3F70);
    RH_ScopedInstall(EnablePedSpeechForScriptSpeech, 0x4E3F90);
    RH_ScopedInstall(StopCurrentSpeech, 0x4E3FB0);
    RH_ScopedInstall(GetSoundAndBankIDsForScriptedSpeech, 0x4E4400);
    RH_ScopedInstall(GetSexFromModel, 0x4E4200);
    RH_ScopedInstall(GetPedTalking, 0x4E3F50);
    RH_ScopedInstall(GetVoiceAndTypeForSpecialPed, 0x4E4170);
    RH_ScopedVMTInstall(UpdateParameters, 0x4E3520);
    RH_ScopedVMTInstall(AddScriptSayEvent, 0x4E4F70);
    RH_ScopedVMTInstall(Terminate, 0x4E5670);
    RH_ScopedVMTInstall(PlayLoadedSound, 0x4E5CD0);
    RH_ScopedVMTInstall(GetAllocatedVoice, 0x4E4120);
    RH_ScopedVMTInstall(WillPedChatAboutTopic, 0x4E5800);
    RH_ScopedVMTInstall(GetPedType, 0x4E4130);
    RH_ScopedVMTInstall(IsPedFemaleForAudio, 0x4E4150);
}

CAEPedSpeechAudioEntity::CAEPedSpeechAudioEntity(eAudioPedType pt) noexcept :
    m_PedAudioType{pt}
{
    if (pt != PED_TYPE_UNK) {
        m_IsInitialized = true;
    }
    m_NextTimeCanSayPain.fill(0);
}

// For 0x4E68D0
CAEPedSpeechAudioEntity::CAEPedSpeechAudioEntity(CPed* ped) noexcept :
    CAEPedSpeechAudioEntity{}
{
    m_pEntity = ped;
    if (ped->GetModelID() != MODEL_INVALID) {
        auto* const mi = ped->GetPedModelInfo();
        m_PedAudioType = mi->m_nPedAudioType;
        switch (m_PedAudioType) {
        case PED_TYPE_UNK:
            return;
        case PED_TYPE_SPC: {
            if (!GetVoiceAndTypeForSpecialPed(mi->m_nKey)) {
                return;
            }
            break;
        }
        default: {
            m_VoiceID = mi->m_nVoiceId;
            if (m_VoiceID == -1) {
                return;
            }
            mi->IncrementVoice();
            break;
        }
        }
        VERIFY(GetSexFromModel(ped->GetModelID()));
        m_IsInitialized = true;
    }
}

// 0x4E4950
tPedSpeechSlotID CAEPedSpeechAudioEntity::CanWePlayScriptedSpeech() {
    return GetFreeSpeechSlot();
}

// 0x4E4AE0
float CAEPedSpeechAudioEntity::GetSpeechContextVolumeOffset(eGlobalSpeechContextS16 gctx) {
    const auto CalculateStrainVolumeOffset = [](float base) {
        const auto t = std::clamp(CStats::GetStatValue(STAT_FAT) - CStats::GetStatValue(STAT_MUSCLE), -1.f, 1.f);
        return (t + 1.f) * 0.5f * base + base;
    };
    switch (gctx) {
    case CTX_GLOBAL_STOMACH_RUMBLE:        return -6.f; // 0x4E4CBB
    case CTX_GLOBAL_BREATHING:             return -12.f; // 0x4E4CC7
    case CTX_GLOBAL_PAIN_CJ_STRAIN:        return CalculateStrainVolumeOffset(-18.f); // 0x4E4B08
    case CTX_GLOBAL_PAIN_CJ_STRAIN_EXHALE: return CalculateStrainVolumeOffset(-21.f); // 0x4E4BE2
    case CTX_GLOBAL_PAIN_CJ_DROWNING:      return 3.f; // 0x4E4CBE
    default:                               return 0.f; // 0x4E4CD0
    }
}

// 0x4E50E0
bool CAEPedSpeechAudioEntity::RequestPedConversation(CPed* pedA, CPed* pedB) {
    if (s_bAllSpeechDisabled || s_bPedConversationHappening || s_bPlayerConversationHappening) {
        return false;
    }
    if (pedA->GetSpeechAE().GetAllocatedVoice() == pedB->GetSpeechAE().GetAllocatedVoice()) {
        return false;
    }
    const auto CheckCanPedTalk = [](CPed* p) {
        return !p->GetSpeechAE().IsAllSpeechDisabled()
            && p->CanPedHoldConversation()
            && !p->GetPedTalking()
            && CVector::DistSqr(TheCamera.GetPosition(), p->GetPosition()) <= sq(40.f);
    };
    if (!CheckCanPedTalk(pedA) || !CheckCanPedTalk(pedB)) {
        return false;
    }
    if (!ReservePedConversationSpeechSlots()) {
        return false;
    }

    s_bPedConversationHappening = true;
    s_pConversationPed1         = pedA;
    s_pConversationPed2         = pedB;

    SetUpConversation();

    return true;
}

// 0x4E52A0
void CAEPedSpeechAudioEntity::ReleasePedConversation() {
    if (!s_bPedConversationHappening) {
        return;
    }
    if (s_pConversationPedSlot1 == -1 || s_pConversationPedSlot2 == -1) {
        return;
    }

    const auto ReleaseConversationFromSlot = [](CPed*& convoPed, auto& slotID) {
        auto& ss = s_PedSpeechSlots[slotID];
        switch (ss.Status) {
        case CAEPedSpeechSlot::eStatus::FREE:
        case CAEPedSpeechSlot::eStatus::RESERVED:
            break;
        default:
            ss.AudioEntity->StopCurrentSpeech();
        }
        ss       = {}; // Reset slot
        slotID   = -1;
        convoPed = nullptr;
    };
    ReleaseConversationFromSlot(s_pConversationPed1, s_pConversationPedSlot1);
    ReleaseConversationFromSlot(s_pConversationPed2, s_pConversationPedSlot2);

    s_bPedConversationHappening = false;
}

// 0x5B98C0
void CAEPedSpeechAudioEntity::StaticInitialise() {
    rng::fill(s_PedSpeechSlots, CAEPedSpeechSlot{});
    rng::fill(s_PhraseMemory, tPhraseMemory{});

    Reset();

    s_pConversationPed1            = nullptr;
    s_pConversationPed2            = nullptr;
    s_pConversationPedSlot1        = 0;
    s_pConversationPedSlot2        = 0;
    s_pPlayerConversationPed       = nullptr;
    s_bPedConversationHappening    = false;
    s_bPlayerConversationHappening = false;
    rng::fill(s_Conversation, CTX_GLOBAL_UNK);

    s_nCJBasicMood        = MOOD_UNK;
    s_nCJGangBanging      = -1;
    s_nCJFat              = -1;
    s_nCJWellDressed      = -1;
    s_nCJMoodOverrideTime = 0;

    s_NextSpeechSlot     = 0;
    s_bAllSpeechDisabled = false;
    s_bAPlayerSpeaking   = false;
    s_bForceAudible      = false;
}

// 0x4E3710
void CAEPedSpeechAudioEntity::Service() { // static
    s_bForceAudible = false;
    for (auto&& [i, speech] : rngv::enumerate(s_PedSpeechSlots)) {
        // Waiting for sound to load, and has loaded?
        if (speech.Status == CAEPedSpeechSlot::eStatus::LOADING && AEAudioHardware.IsSoundLoaded(speech.SoundBankID, speech.SoundID, SND_BANK_SLOT_SPEECH1 + i)) {
            speech.Status = CAEPedSpeechSlot::eStatus::WAITING;
        }

        // Sound is now loaded, waiting to be played
        if (speech.Status == CAEPedSpeechSlot::eStatus::WAITING) {
            if (CTimer::GetTimeInMS() >= speech.StartPlaybackTime) {
                if (const auto ae = speech.AudioEntity) {
                    ae->PlayLoadedSound();
                } else {
                    speech.Status = CAEPedSpeechSlot::eStatus::FREE;
                }
            }
        }

        // `PlayLoadedSound` above might've modified the status, must check it again
        switch (speech.Status) {
        case CAEPedSpeechSlot::eStatus::REQUESTED:
        case CAEPedSpeechSlot::eStatus::PLAYING: {
            s_bForceAudible |= speech.ForceAudible;
        }
        }
    }
}

// 0x4E37B0
void CAEPedSpeechAudioEntity::Reset() {
    rng::generate(gGlobalSpeechContextNextPlayTime, []{
        return CTimer::GetTimeInMS() + CAEAudioUtility::GetRandomNumberInRange(3000, 7000);
    }); 
    s_nCJMoodOverrideTime = 0;
}

// 0x4E37F0
bool CAEPedSpeechAudioEntity::ReservePedConversationSpeechSlots() {
    const auto slotA = GetFreeSpeechSlot();
    const auto slotB = GetFreeSpeechSlot();

    if (slotA == -1 || slotB == -1) {
        return false;
    }

    const auto SetupSlot = [](int32 slot) {
        auto* const ss                   = &s_PedSpeechSlots[slot];
        ss->IsReservedForPedConversation = true;
        ss->Status                       = CAEPedSpeechSlot::eStatus::RESERVED;
    };
    SetupSlot(s_pConversationPedSlot1 = slotA);
    SetupSlot(s_pConversationPedSlot2 = slotB);

    return true;
}

// 0x4E3870
bool CAEPedSpeechAudioEntity::ReservePlayerConversationSpeechSlot() {
    const auto slot = GetFreeSpeechSlot();
    if (slot == -1) {
        return false;
    }
    auto* const ss                      = &s_PedSpeechSlots[slot];
    ss->Status                          = CAEPedSpeechSlot::eStatus::RESERVED;
    ss->IsReservedForPlayerConversation = true;
    s_pConversationPedSlot1             = slot;
    return true;
}

// 0x4E38C0
bool CAEPedSpeechAudioEntity::RequestPlayerConversation(CPed* ped) {
    if (s_bAllSpeechDisabled) {
        return false;
    }

    if (ped->GetSpeechAE().m_IsSpeechForScriptsDisabled || ped->GetSpeechAE().m_IsSpeechDisabled) {
        return false;
    }

    const auto player = FindPlayerPed();
    if (!player || player->GetSpeechAE().m_IsSpeechDisabled || player->GetSpeechAE().m_IsSpeechForScriptsDisabled) {
        return false;
    }

    if (   s_bPedConversationHappening
        || s_bPlayerConversationHappening
        || ped->GetPedTalking()
        || !ReservePlayerConversationSpeechSlot()
    ) {
        return false;
    }

    s_pPlayerConversationPed       = ped;
    s_bPlayerConversationHappening = true;

    return true;
}

// 0x4E3960
void CAEPedSpeechAudioEntity::ReleasePlayerConversation() {
    if (!s_bPlayerConversationHappening) {
        return;
    }
    s_bPlayerConversationHappening = false;
    if (s_pConversationPedSlot1 < 0) {
        return;
    }
    auto* const ss = &s_PedSpeechSlots[s_pConversationPedSlot1];
    switch (ss->Status) {
    case CAEPedSpeechSlot::eStatus::FREE:
    case CAEPedSpeechSlot::eStatus::RESERVED: {
        *ss = {};
    }
    }
    s_PedSpeechSlots[s_pConversationPedSlot1] = {};
    s_pConversationPedSlot1                   = -1;
    s_pPlayerConversationPed                  = nullptr;
}

// 0x4E3A00
void CAEPedSpeechAudioEntity::SetUpConversation() {
    rng::fill(s_Conversation, CTX_GLOBAL_NO_SPEECH);

    auto PushConvo = [i = 0](eGlobalSpeechContext gctx) mutable {
        s_Conversation[i++] = gctx;
    };

    const auto PushConvoForPeds = [&](eGlobalSpeechContext ifFemale, eGlobalSpeechContext ifMale) {
        const auto PushConvoForPed = [&](const CPed* p) {
            switch (p->m_nPedType) {
            case PED_TYPE_PROSTITUTE:
            case PED_TYPE_CIVFEMALE: PushConvo(ifFemale); break;
            default:                 PushConvo(ifMale);   break;
            }
        };
        PushConvoForPed(s_pConversationPed2); // yes, ped 2, then ped 1
        PushConvoForPed(s_pConversationPed1);
    };

    // Greeting
    PushConvoForPeds(CTX_GLOBAL_PCONV_GREET_FEM, CTX_GLOBAL_PCONV_GREET_MALE);

    // Other stuff
    const auto r = CAEAudioUtility::GetRandomNumberInRange(1, 10);
    if (r <= 8) { // Combined cases 1, -> 4 + 5, -> 8
        const auto hasInitialState = r <= 4;

        s_ConversationLength = hasInitialState ? 3 : 2;

        if (hasInitialState) { // For cases 1 -> 4 only
            const auto isGood = CGeneral::DoCoinFlip();
            PushConvo(isGood ?  CTX_GLOBAL_PCONV_STATE_GOOD :  CTX_GLOBAL_PCONV_STATE_BAD);
            PushConvo(isGood ?  CTX_GLOBAL_PCONV_AGREE_GOOD :  CTX_GLOBAL_PCONV_AGREE_BAD);
        }

        // A question
        PushConvo(CTX_GLOBAL_PCONV_QUESTION);

        // Whenever they agree with the answer (?)
        switch (CAEAudioUtility::GetRandomNumberInRange(0, 2)) {
        case 0: PushConvo(CTX_GLOBAL_PCONV_AGREE_GOOD); break;
        case 1: PushConvo(CTX_GLOBAL_PCONV_AGREE_BAD);  break;
        case 2: PushConvo(CTX_GLOBAL_PCONV_ANS_NO);     break;
        }

        // Part away (?)
        PushConvoForPeds(CTX_GLOBAL_PCONV_PART_FEM, CTX_GLOBAL_PCONV_PART_MALE);
    } else if (r <= 10) { // cases 9, 10
        s_ConversationLength = 1;

        PushConvo(CGeneral::DoCoinFlip() ?  CTX_GLOBAL_PCONV_STATE_GOOD :  CTX_GLOBAL_PCONV_STATE_BAD);
        PushConvo(CTX_GLOBAL_PCONV_DISMISS);
    } else {
        NOTSA_UNREACHABLE();
    }
}

// 0x4E3C60
eAudioPedType CAEPedSpeechAudioEntity::GetAudioPedType(const char* name) {
    static const auto mapping = notsa::make_mapping<std::string_view, eAudioPedType>({
        {"PED_TYPE_GEN",    PED_TYPE_GEN   },
        {"PED_TYPE_EMG",    PED_TYPE_EMG   },
        {"PED_TYPE_PLAYER", PED_TYPE_PLAYER},
        {"PED_TYPE_GANG",   PED_TYPE_GANG  },
        {"PED_TYPE_GFD",    PED_TYPE_GFD   },
        {"PED_TYPE_SPC",    PED_TYPE_SPC   },
    });
    return notsa::find_value_or(mapping, name, PED_TYPE_UNK);
}

// 0x4E3CD0
ePedSpeechVoiceS16 CAEPedSpeechAudioEntity::GetVoice(const char* name, eAudioPedTypeS16 type) {
    const auto DoLookUpIn = [name](auto&& mapping) {
        return notsa::find_value_or(mapping, name, VOICE_UNK);
    };
    switch (type) {
    case PED_TYPE_GEN:   return DoLookUpIn(gGenSpeechVoiceLookup);
    case PED_TYPE_EMG:   return DoLookUpIn(gEmgSpeechVoiceLookup);
    case PED_TYPE_PLAYER:return DoLookUpIn(gPlySpeechVoiceLookup);
    case PED_TYPE_GANG:  return DoLookUpIn(gGngSpeechVoiceLookup);
    case PED_TYPE_GFD:   return DoLookUpIn(gGfdSpeechVoiceLookup);
    case PED_TYPE_SPC:   return VOICE_UNK;
    default:             NOTSA_UNREACHABLE();
    }
}

// 0x4E3EB0
void CAEPedSpeechAudioEntity::DisableAllPedSpeech() {
    s_bAllSpeechDisabled = true;
}

// 0x4E3ED0
void CAEPedSpeechAudioEntity::SetCJMood(eCJMood basicMood, uint32 overrideTimeMS, int16 isGangBanging, int16 isFat, int16 isWellDressed) {
    s_nCJMoodOverrideTime = CTimer::GetTimeInMS() + overrideTimeMS;
    s_nCJBasicMood  = [=]{
        switch (basicMood) {
        case MOOD_AR:
        case MOOD_CR:
        case MOOD_PR:
        case MOOD_WR: return basicMood;
        default:      return MOOD_UNK;
        }
    }();
    s_nCJGangBanging = isGangBanging;
    s_nCJFat         = isFat;
    s_nCJWellDressed = isWellDressed;
}


// 0x4E53B0
eCJMood CAEPedSpeechAudioEntity::GetCurrentCJMood() {
    const auto* const plyr = FindPlayerPed();
    if (!plyr) {
        return MOOD_CR;
    }

    const auto isMoodOverrideActive = s_nCJMoodOverrideTime >= CTimer::GetTimeInMS();
    
    const auto CheckCJIsWellDressed = [&] { // 0x4E53DF
        if (isMoodOverrideActive && s_nCJWellDressed != -1) {
            return s_nCJWellDressed != 0;
        }
        return CStats::GetStatValue(STAT_CLOTHES_RESPECT) >= 650.f
            && CStats::GetStatValue(STAT_FASHION_BUDGET) >= 10'000.f;
    };

    const auto CheckCJIsGangBanging = [&] { // 0x4E5440
        if (isMoodOverrideActive && s_nCJGangBanging != -1) {
            return s_nCJGangBanging != 0;
        }
        if (IsCJDressedInForGangSpeech()) {
            return true;
        }
        auto& plyrGrp = plyr->GetPlayerGroup();
        switch (plyrGrp.GetMembership().CountMembersExcludingLeader()) {
        case 0: // No other members, just the player alone
            return false;
        case 1: { // Exactly one
            const auto& mem = plyrGrp.GetMembership().GetMembers().front(); // The one-and-only member (This isnt the same as `GetMember(0)`!!!)
            if (mem->m_nPedType == PED_TYPE_GANG2) {
                return true;
            }
            auto& memSpeech = mem->GetSpeechAE();
            return memSpeech.m_PedAudioType == PED_TYPE_GANG
                && notsa::contains({ VOICE_GNG_RYDER, VOICE_GNG_SWEET, VOICE_GNG_SMOKE }, (eGngSpeechVoices)memSpeech.m_VoiceID);
        }
        default: // 2 or more
            return true;
        }
    };

    const auto CheckCJIsFat = [&] { // 0x4E54E0
        if (isMoodOverrideActive && s_nCJFat != -1) {
            return s_nCJFat != 0;
        }
        return CStats::GetStatValue(STAT_FAT) >= 600.f
            && CStats::GetStatValue(STAT_FAT) - 200.f > CStats::GetStatValue(STAT_MUSCLE);
    };

    const auto DeriveMood = [&](eCJMood basicMood) { // 0x4E55CC
        if (isMoodOverrideActive && s_nCJBasicMood != MOOD_UNK) { // 0x4E5592
            basicMood = s_nCJBasicMood;
        }
        switch (basicMood) {
        case MOOD_AR:
            return CheckCJIsGangBanging()
                ? MOOD_AG
                : MOOD_AR;
        case MOOD_CR: {
            if (CheckCJIsGangBanging()) {
                return MOOD_CG;
            }
            if (CheckCJIsFat()) {
                return MOOD_CF;
            }
            return CheckCJIsWellDressed()
                ? MOOD_CD
                : MOOD_CR;
        }
        case MOOD_PR:
            return CheckCJIsGangBanging()
                ? MOOD_PG
                : MOOD_PR;
        case MOOD_WR:
            return CheckCJIsGangBanging()
                ? MOOD_WG
                : MOOD_WR;
        default:
            return MOOD_CR;
        }
    };

    if (FindPlayerWanted()->GetWantedLevel() > 3) { // 0x4E5537
        return DeriveMood(MOOD_PR);
    }

    if (FindPlayerWanted()->GetWantedLevel() > 1) { // 0x4E554B
        return DeriveMood(MOOD_AR);
    }

    if (CTheScripts::LastMissionPassedTime != -1) { // 0x4E555C
        if (CTimer::GetTimeInMS() >= CTheScripts::LastMissionPassedTime) {
            return CTimer::GetTimeInMS() < CTheScripts::LastMissionPassedTime + 180'000
                ? DeriveMood(MOOD_WR)
                : DeriveMood(MOOD_CR);
        } else {
            CTheScripts::LastMissionPassedTime = CTimer::GetTimeInMS();
        }
    }

    return DeriveMood(MOOD_CR); // 0x4E5579
}

// 0x4E4700
eCJMood __stdcall CAEPedSpeechAudioEntity::GetNextMoodToUse(eCJMood currMood) {
    switch (currMood) {
    case MOOD_AG:
    case MOOD_PR: return MOOD_AR;
    case MOOD_PG: return MOOD_AG;
    case MOOD_WG: return MOOD_CG;
    default:      return MOOD_CR;
    }
}

// 0x4E4760
ePedSpeechVoiceS16 __stdcall CAEPedSpeechAudioEntity::GetVoiceForMood(eCJMood mood) {
    const auto b = CAEAudioUtility::GetRandomNumberInRange(0, 1) != 0;
    switch (mood) {
    case MOOD_AG: return b ? VOICE_PLY_AG : VOICE_PLY_AG2;
    case MOOD_AR: return b ? VOICE_PLY_AR : VOICE_PLY_AR2;
    case MOOD_CD: return b ? VOICE_PLY_CD : VOICE_PLY_CD2;
    case MOOD_CF: return b ? VOICE_PLY_CF : VOICE_PLY_CF2;
    case MOOD_CG: return b ? VOICE_PLY_CG : VOICE_PLY_CG2;
    case MOOD_CR: return b ? VOICE_PLY_CR : VOICE_PLY_CR2;
    case MOOD_PG: return b ? VOICE_PLY_PG : VOICE_PLY_PG2;
    case MOOD_PR: return b ? VOICE_PLY_PR : VOICE_PLY_PR2;
    case MOOD_WG: return b ? VOICE_PLY_WG : VOICE_PLY_WG2;
    case MOOD_WR: return b ? VOICE_PLY_WR : VOICE_PLY_WR2;
    default:      NOTSA_UNREACHABLE();
    }
}

// 0x4E3EC0
void CAEPedSpeechAudioEntity::EnableAllPedSpeech() {
    s_bAllSpeechDisabled = false;
}

// 0x4E4270
bool CAEPedSpeechAudioEntity::IsCJDressedInForGangSpeech() {
    static constexpr struct {
        eClothesTexturePart ClothesPart;
        const char*         TexName;
    } GANG_SPEECH_CLOTHES[] = {
        { CLOTHES_TEXTURE_TORSO,   "hoodyagreen" },
        { CLOTHES_TEXTURE_TORSO,   "shirtbgang"  },
        { CLOTHES_TEXTURE_LEGS,    "tracktrgang" },
        { CLOTHES_TEXTURE_LEGS,    "denimsgang"  },
        { CLOTHES_TEXTURE_HATS,    "bandgang"    },
        { CLOTHES_TEXTURE_HATS,    "hockey"      },
        { CLOTHES_TEXTURE_HATS,    "capgang"     },
        { CLOTHES_TEXTURE_HATS,    "capgangback" },
        { CLOTHES_TEXTURE_HATS,    "capgangside" },
        { CLOTHES_TEXTURE_HATS,    "capgangover" },
        { CLOTHES_TEXTURE_HATS,    "capgangup"   },
        { CLOTHES_TEXTURE_GLASSES, "bandred3"    },
        { CLOTHES_TEXTURE_GLASSES, "bandblue3"   },
        { CLOTHES_TEXTURE_GLASSES, "bandgang3"   },
        { CLOTHES_TEXTURE_GLASSES, "bandblack3"  },
    };

    if (!FindPlayerPed(PED_TYPE_PLAYER1)) {
        return false;
    }

    if (CGameLogic::FindCityClosestToPoint(FindPlayerCoors()) != LEVEL_NAME_LOS_SANTOS) {
        return false;
    }

    return rng::any_of(GANG_SPEECH_CLOTHES, [pcd = FindPlayerPed()->GetClothesDesc()](auto& gc) {
        return pcd->m_anTextureKeys[gc.ClothesPart] == CKeyGen::GetUppercaseKey(gc.TexName);
    });
}

// 0x4E4600
bool __stdcall CAEPedSpeechAudioEntity::IsGlobalContextImportantForInterupting(eGlobalSpeechContext gCtx) {
    const auto ctxi = GetGlobalSpeechContextInfo(gCtx);
    return ctxi && ctxi->IsImportantForInterrupting; // NOTSA: Use context info lookup instead of static switch
    /*
    switch (gCtx) {
    case CTX_GLOBAL_ARREST:
    case CTX_GLOBAL_ARRESTED:
    case CTX_GLOBAL_JOIN_GANG_NO:
    case CTX_GLOBAL_JOIN_GANG_YES:
    case CTX_GLOBAL_JOIN_ME_ASK:
        return true;
    }
    return false;
    */
}

// 0x4E46F0 - unused
bool CAEPedSpeechAudioEntity::IsGlobalContextUberImportant(int16 gCtx) {
    return false;
}

// 0x4E44F0
bool __stdcall CAEPedSpeechAudioEntity::IsGlobalContextPain(eGlobalSpeechContext gCtx) {
    const auto ctxi = GetGlobalSpeechContextInfo(gCtx);
    return ctxi && ctxi->IsPain;
}

// 0x4E46B0
bool CAEPedSpeechAudioEntity::IsGlobalContextImportantForWidescreen(eGlobalSpeechContext gCtx) {
    switch (m_PedAudioType) {
    case PED_TYPE_GFD:
    case PED_TYPE_PLAYER:
        return true;
    }
    const auto ctxi = GetGlobalSpeechContextInfo(gCtx);
    return ctxi && ctxi->IsImportantForWidescreen; // NOTSA: Use context info lookup instead of static switch
    /*
    switch (gCtx) {
    case CTX_GLOBAL_ARREST:
    case CTX_GLOBAL_ARRESTED:
    case CTX_GLOBAL_HAVING_SEX:
        return true;
    }
    return false;
    */
}

// 0x4E4510
bool CAEPedSpeechAudioEntity::IsGlobalContextImportantForStreaming(eGlobalSpeechContext gCtx) {
    switch (m_PedAudioType) {
    case PED_TYPE_GFD:
    case PED_TYPE_PLAYER:
        return true;
    }
    const auto ctxi = GetGlobalSpeechContextInfo(gCtx);
    return ctxi && ctxi->IsImportantForStreaming; // NOTSA: Use context info lookup instead of static switch
    /*
    switch (gCtx) {
    case CTX_GLOBAL_ARREST:
    case CTX_GLOBAL_ARRESTED:
    case CTX_GLOBAL_CONV_DISL_CAR:
    case CTX_GLOBAL_CONV_DISL_CLOTHES:
    case CTX_GLOBAL_CONV_DISL_HAIR:
    case CTX_GLOBAL_CONV_DISL_PHYS:
    case CTX_GLOBAL_CONV_DISL_SHOES:
    case CTX_GLOBAL_CONV_DISL_SMELL:
    case CTX_GLOBAL_CONV_DISL_TATTOO:
    case CTX_GLOBAL_CONV_DISL_WEATHER:
    case CTX_GLOBAL_CONV_IGNORED:
    case CTX_GLOBAL_CONV_LIKE_CAR:
    case CTX_GLOBAL_CONV_LIKE_CLOTHES:
    case CTX_GLOBAL_CONV_LIKE_HAIR:
    case CTX_GLOBAL_CONV_LIKE_PHYS:
    case CTX_GLOBAL_CONV_LIKE_SHOES:
    case CTX_GLOBAL_CONV_LIKE_SMELL:
    case CTX_GLOBAL_CONV_LIKE_TATTOO:
    case CTX_GLOBAL_CONV_LIKE_WEATHER:
    case CTX_GLOBAL_JOIN_GANG_NO:
    case CTX_GLOBAL_JOIN_GANG_YES:
    case CTX_GLOBAL_JOIN_ME_ASK:
    case CTX_GLOBAL_PCONV_AGREE_BAD:
    case CTX_GLOBAL_PCONV_AGREE_GOOD:
    case CTX_GLOBAL_PCONV_ANS_NO:
    case CTX_GLOBAL_PCONV_DISMISS:
    case CTX_GLOBAL_PCONV_GREET_FEM:
    case CTX_GLOBAL_PCONV_GREET_MALE:
    case CTX_GLOBAL_PCONV_PART_FEM:
    case CTX_GLOBAL_PCONV_PART_MALE:
    case CTX_GLOBAL_PCONV_QUESTION:
    case CTX_GLOBAL_PCONV_STATE_BAD:
    case CTX_GLOBAL_PCONV_STATE_GOOD:
    return true;
    }
    return false;
    */
}

// 0x4E47E0
int16 CAEPedSpeechAudioEntity::GetRepeatTime(eGlobalSpeechContext gCtx) const {
    if (const auto* const ctxi = GetGlobalSpeechContextInfo(gCtx)) {
        return ctxi->RepeatTime;
    }
    return 0;
}

// 0x4E4840
void CAEPedSpeechAudioEntity::LoadAndPlaySpeech(uint32 playbackTimeOffsetMS) {
    auto* const speech = GetCurrentSpeech();
    switch (speech->Status) {
    case CAEPedSpeechSlot::eStatus::FREE:
    case CAEPedSpeechSlot::eStatus::RESERVED:
        break;
    default:
        return;
    }

    AEAudioHardware.LoadSound(m_BankID, m_SoundID, SND_BANK_SLOT_SPEECH1 + m_PedSpeechSlotID);
    *speech = CAEPedSpeechSlot{
        .Status            = CAEPedSpeechSlot::eStatus::LOADING,
        .AudioEntity       = this,
        .SoundID           = m_SoundID,
        .SoundBankID       = m_BankID,
        .StartPlaybackTime = CTimer::GetTimeInMS() + playbackTimeOffsetMS,
        .GCtx              = m_LastGCtx,
        .PedAudioType      = m_PedAudioType,
        .ForceAudible      = m_IsForcedAudible,
    };
}

// 0x4E49B0
int32 CAEPedSpeechAudioEntity::GetNumSlotsPlayingContext(eGlobalSpeechContext gCtx) {
    return rng::count_if(s_PedSpeechSlots, [&](const CAEPedSpeechSlot& speech) {
        return speech.Status != CAEPedSpeechSlot::eStatus::FREE && speech.GCtx == gCtx;
    });
}

// 0x4E49E0
uint32 CAEPedSpeechAudioEntity::GetNextPlayTime(eGlobalSpeechContext gCtx) const {
    return GetNextPlayTimeRef(gCtx);
}

// 0x4E4A20
void CAEPedSpeechAudioEntity::SetNextPlayTime(eGlobalSpeechContext gCtx) {
    assert(gCtx < CTX_GLOBAL_NUM); // OG: `return;`

    if (const auto* const ctxi = GetGlobalSpeechContextInfo(gCtx)) {
        GetNextPlayTimeRef(gCtx) = CTimer::GetTimeInMS() + ctxi->RepeatTime + CAEAudioUtility::GetRandomNumberInRange(1, 1000);
    }
}

// 0x4E56D0
void CAEPedSpeechAudioEntity::DisablePedSpeech(int16 a1) {
    if (m_IsInitialized) {
        m_IsSpeechDisabled = true;
        if (a1)
            StopCurrentSpeech();
    }
}

// 0x4E5700
void CAEPedSpeechAudioEntity::DisablePedSpeechForScriptSpeech(int16 a1) {
    if (m_IsInitialized) {
        m_IsSpeechForScriptsDisabled = true;
        if (a1)
            StopCurrentSpeech();
    }
}

// 0x4E5730
bool CAEPedSpeechAudioEntity::CanPedSayGlobalContext(eGlobalSpeechContext gCtx) const {
    if (!m_IsInitialized) {
        return false;
    }
    const auto sCtx = GetSpecificSpeechContext(gCtx, m_PedAudioType);
    if (sCtx == -1) {
        return false;
    }
    if (const auto* const ctxi = GetSpecificSpeechContextInfo(sCtx, gCtx, m_PedAudioType, m_VoiceID)) {
        return ctxi->IsUseable();
    }
    return false;
}

// 0x4E58C0
bool CAEPedSpeechAudioEntity::GetVoiceAndTypeFromModel(eModelID modelId) {
    auto* const mi = CModelInfo::GetModelInfo(modelId)->AsPedModelInfoPtr();

    switch (mi->m_nPedAudioType) {
    case PED_TYPE_UNK: return false;
    case PED_TYPE_SPC: return GetVoiceAndTypeForSpecialPed(mi->m_nKey);
    }

    m_VoiceID = mi->m_nVoiceId;
    if (m_VoiceID == -1) {
        return false;
    }

    mi->IncrementVoice();

    return true;
}

// 0x4E5920
int16 CAEPedSpeechAudioEntity::GetSoundAndBankIDs(eGlobalSpeechContext gCtx, eSpecificSpeechContext& outSpecificSpeechContext) {
    // Left out some ifs error checking ifs as they aren't necessary (They're gonna get caught in debug anyways)

    const auto sCtx = GetSpecificSpeechContext(gCtx, m_PedAudioType);
    if (sCtx == -1) {
        return -1;
    }
    outSpecificSpeechContext = sCtx;

    const auto voiceID = [&]() -> ePedSpeechVoiceS32 {
        if (IsGlobalContextPain(gCtx)) {
            return GetPainVoice();
        } else if (m_PedAudioType == PED_TYPE_PLAYER) {
            eCJMood mood = GetCurrentCJMood();
            NOTSA_LOG_DEBUG("GetCurrentCJMood: 0x{:x}", (int)mood);
            for (; ;mood = GetNextMoodToUse(mood)) {
                const auto voiceID = GetVoiceForMood(mood);
                if (mood == MOOD_CR || GetSpecificSpeechContextInfo(sCtx, gCtx, m_PedAudioType, voiceID)->IsUseable()) {
                    return voiceID;
                }
            }
        } else {
            return m_VoiceID;
        }
    }();
    if (voiceID == -1) {
        return -1;
    }
    m_BankID = GetVoiceSoundBank(gCtx, m_PedAudioType, voiceID);

    const auto* const ctx = GetSpecificSpeechContextInfo(sCtx, gCtx, m_PedAudioType, voiceID);
    if (!ctx || !ctx->IsUseable()) {
        return -1;
    }
    assert(ctx->FirstSoundID <= ctx->LastSoundID);

    const size_t numSounds = ctx->GetNumSounds();
    assert(numSounds > 0);

    // Find sound ID to use
    // If all possible sounds are in the PhraseMemory,
    // the least recently used one will be picked
    // NOTE: Below is a better version of what they did (without using an intermediary array)

    const auto GetPhraseIndexInMemory = [this](int32 soundID) -> int16 {
        for (auto&& [i, p] : rngv::enumerate(s_PhraseMemory)) {
            if (p.SoundID == soundID && p.BankID == m_BankID) {
                return i;
            }
        }
        return -1;
    };

    int16      maxPhraseIMemdx = -1;
    const auto rndOffset    = CAEAudioUtility::GetRandomNumberInRange(0u, numSounds);
    for (size_t i = 0; i < std::min(20u, numSounds); i++) { // NOTE: Here you can tune the maximum tries (Default: 20)
        const auto soundIDCurr  = ctx->FirstSoundID + (int16)((rndOffset + i) % numSounds);
        const auto phraseMemIdx = GetPhraseIndexInMemory(soundIDCurr);

        // Not in memory at all? Good, pick this!
        if (phraseMemIdx == -1) {
            m_SoundID = soundIDCurr;
            break;
        }

        // Maybe this was used a longer time ago than the previous one?
        if (maxPhraseIMemdx < phraseMemIdx) {
            m_SoundID = soundIDCurr;
            maxPhraseIMemdx = phraseMemIdx;
        }
    }
    assert(m_SoundID != -1);

    // Insert into front of `s_PhraseMemory`
    rng::shift_right(s_PhraseMemory, 1);
    s_PhraseMemory[0] = {m_SoundID, m_BankID};

    return m_SoundID - ctx->FirstSoundID;
}

// 0x4E5F10
int16 CAEPedSpeechAudioEntity::CanWePlayGlobalSpeechContext(eGlobalSpeechContext gCtx) {
    if (!IsGlobalContextImportantForInterupting(gCtx) && !IsGlobalContextPain(gCtx)) {
        const auto CheckSlot = [](auto slot) {
            return s_PedSpeechSlots[slot].Status == CAEPedSpeechSlot::eStatus::RESERVED
                ? slot
                : -1;
        };
        if (s_bPedConversationHappening) {
            if (this == &s_pConversationPed1->GetSpeechAE()) {
                return CheckSlot(s_pConversationPedSlot1);
            }
            if (this == &s_pConversationPed2->GetSpeechAE()) {
                return CheckSlot(s_pConversationPedSlot2);
            }
        }
        if (s_bPlayerConversationHappening) {
            if (this == &s_pPlayerConversationPed->GetSpeechAE()) {
                return CheckSlot(s_pConversationPedSlot1);
            }    
        }
    }
    return CTimer::GetTimeInMS() >= GetNextPlayTime(gCtx)
        ? GetFreeSpeechSlot() // s_NextSpeechSlot is set in `GetFreeSpeechSlot`
        : -1;
}

// 0x4E6550
int16 CAEPedSpeechAudioEntity::AddSayEvent(eAudioEvents audioEvent, eGlobalSpeechContext gCtx, uint32 startTimeDelayMs, float probability, bool overrideSilence, bool isForceAudible, bool isFrontEnd) {
    return I_AddSayEvent<false>(m_pEntity->GetPosition(), audioEvent, gCtx, startTimeDelayMs, probability, overrideSilence, isForceAudible, isFrontEnd);
}

// 0x4E68D0
void CAEPedSpeechAudioEntity::Initialise(CEntity* ped) {
    *this = CAEPedSpeechAudioEntity{ped->AsPed()};
}

// 0x4E69E0
bool CAEPedSpeechAudioEntity::CanPedHoldConversation() const {
    return CanPedSayGlobalContext(CTX_GLOBAL_PCONV_QUESTION);
}

// 0x4E3F70
void CAEPedSpeechAudioEntity::EnablePedSpeech() {
    if (m_IsInitialized) {
        m_IsSpeechDisabled = false;
    }
}

// 0x4E3F90
void CAEPedSpeechAudioEntity::EnablePedSpeechForScriptSpeech() {
    if (m_IsInitialized) {
        m_IsSpeechForScriptsDisabled = false;
    }
}

// 0x4E3FB0
void CAEPedSpeechAudioEntity::StopCurrentSpeech() {
    if (!m_IsInitialized || !m_IsPlayingSpeech) {
        return;
    }

    if (auto* const tFacial = notsa::cast<CTaskComplexFacial>(m_pEntity->AsPed()->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_FACIAL_COMPLEX))) {
        tFacial->StopAll();
    }

    auto* const speech = GetCurrentSpeech();

    if (speech->Status == CAEPedSpeechSlot::eStatus::PLAYING) {
        if (auto* const s = std::exchange(m_Sound, nullptr)) {
            s->StopSoundAndForget();
        }
    }

    *speech = CAEPedSpeechSlot{
        .Status = speech->IsReservedForPedConversation || speech->IsReservedForPlayerConversation
            ? CAEPedSpeechSlot::eStatus::RESERVED
            : CAEPedSpeechSlot::eStatus::FREE,
        .IsReservedForPedConversation    = speech->IsReservedForPedConversation,
        .IsReservedForPlayerConversation = speech->IsReservedForPlayerConversation,
    };

    m_IsPlayingSpeech = false;
    if (m_PedAudioType == PED_TYPE_PLAYER) {
        s_bAPlayerSpeaking = false;
    }
    m_PedSpeechSlotID = -1;
}

// 0x4E4400
bool CAEPedSpeechAudioEntity::GetSoundAndBankIDsForScriptedSpeech(eAudioEvents scriptID) {
    int32         soundID;
    eSoundBankS32 bankID;
    CAEAudioUtility::GetBankAndSoundFromScriptSlotAudioEvent(scriptID, bankID, soundID, -1);
    if (soundID < 0 || bankID < 0) {
        m_BankID  = SND_BANK_UNK;
        m_SoundID = -1;
        return false;
    } else {
        m_BankID  = bankID;
        m_SoundID = soundID;
        return true;
    }
}

// 0x4E4200
bool CAEPedSpeechAudioEntity::GetSexFromModel(eModelID model) {
    const auto* const mi = CModelInfo::GetModelInfo(model)->AsPedModelInfoPtr();
    assert(mi->m_nPedAudioType < PED_TYPE_NUM);
    if (mi->m_nPedAudioType == PED_TYPE_SPC) {
        return true;
    }
    m_IsFemale = notsa::contains({PED_TYPE_CIVFEMALE, PED_TYPE_PROSTITUTE}, mi->GetPedType());
    return true;
}

// 0x4E3F50
bool CAEPedSpeechAudioEntity::GetPedTalking() const {
    return m_IsInitialized && m_IsPlayingSpeech;
}

// 0x4E4170
bool CAEPedSpeechAudioEntity::GetVoiceAndTypeForSpecialPed(uint32 modelNameHash) {
    const auto idx = notsa::indexof(gSpecialPedVoiceNameLookup, modelNameHash);
    if (idx == -1) {
        return false;
    }
    std::tie(m_VoiceID, m_PedAudioType, m_IsFemale) = gSpecialPedVoiceLookup[idx];
    return true;
}

// notsa
ePainSpeechVoices CAEPedSpeechAudioEntity::GetPainVoice() const {
    if (m_PedAudioType == PED_TYPE_PLAYER) {
        return VOICE_PAIN_CARL;
    }
    return m_IsFemale
        ? VOICE_PAIN_FEMALE
        : VOICE_PAIN_MALE;
}

// 0x4E4F70
void CAEPedSpeechAudioEntity::AddScriptSayEvent(eAudioEvents audioEvent, eAudioEvents scriptID, bool overrideSilence, bool isForceAudible, bool isFrontEnd) {
    if (audioEvent != AE_SCRIPT_SPEECH_PED) {
        return;
    }
    if (!m_IsInitialized || s_bAllSpeechDisabled) {
        return;
    }
    if (TheCamera.m_bWideScreenOn) {
        return;
    }
    if (m_IsSpeechDisabled && !overrideSilence) {
        return;
    }
    if (m_IsPlayingSpeech) {
        return;
    }
    if (CStreaming::IsVeryBusy()) {
        return;
    }
    const auto speechSlotID = m_PedAudioType == PED_TYPE_PLAYER
        ? !CAEPedSpeechAudioEntity::s_bAPlayerSpeaking || m_IsPlayingSpeech
            ? PLAYER_SPEECH_SLOT
            : -1
        : CanWePlayScriptedSpeech();
    if (speechSlotID == -1) {
        return;
    }
    if (!GetSoundAndBankIDsForScriptedSpeech(scriptID)) {
        return;
    }

    m_LastGCtx        = CTX_GLOBAL_UNK;
    m_PedSpeechSlotID = speechSlotID;
    m_IsFrontend      = isFrontEnd;

    m_IsForcedAudible = isForceAudible;
    if (const auto veh = m_pEntity->AsPed()->GetVehicleIfInOne()) {
        if (veh == FindPlayerVehicle()) {
            m_IsForcedAudible = true;
        }
    }

    m_EventVolume = GetDefaultVolume(AE_SPEECH_PED);
    if (m_PedAudioType == PED_TYPE_PLAYER) {
        m_EventVolume += 3.f;
    }

    m_IsPlayingSpeech = true;
    if (m_PedAudioType == PED_TYPE_PLAYER) {
        s_bAPlayerSpeaking = true;
    }

    LoadAndPlaySpeech();
}

// 0x4E5670
void CAEPedSpeechAudioEntity::Terminate() {
    StopCurrentSpeech();
    *this = CAEPedSpeechAudioEntity{};
}

// 0x4E5CD0
void CAEPedSpeechAudioEntity::PlayLoadedSound() {
    return I_PlayLoadedSound<false>(m_pEntity);
}

// 0x4E4120
int16 CAEPedSpeechAudioEntity::GetAllocatedVoice() {
    return m_VoiceID;
}

// 0x4E5800
bool CAEPedSpeechAudioEntity::WillPedChatAboutTopic(int16 topic) {
    switch (topic) {
    case 0:  return CanPedSayGlobalContext(CTX_GLOBAL_CONV_DISL_CAR);
    case 1:  return CanPedSayGlobalContext(CTX_GLOBAL_CONV_DISL_CLOTHES);
    case 2:  return CanPedSayGlobalContext(CTX_GLOBAL_CONV_DISL_HAIR);
    case 3:  return CanPedSayGlobalContext(CTX_GLOBAL_CONV_DISL_PHYS);
    case 4:  return CanPedSayGlobalContext(CTX_GLOBAL_CONV_DISL_SHOES);
    case 5:  return CanPedSayGlobalContext(CTX_GLOBAL_CONV_DISL_SMELL);
    case 6:  return CanPedSayGlobalContext(CTX_GLOBAL_CONV_DISL_TATTOO);
    case 7:  return CanPedSayGlobalContext(CTX_GLOBAL_CONV_DISL_WEATHER);
    case 8:
    case 9:  return true;
    default: return false;
    }
}

// 0x4E4130
eAudioPedType CAEPedSpeechAudioEntity::GetPedType() {
    return m_IsInitialized 
        ? m_PedAudioType
        : PED_TYPE_UNK;
}

// 0x4E4150
bool CAEPedSpeechAudioEntity::IsPedFemaleForAudio() {
    return m_IsInitialized
        ? m_IsFemale
        : false;
}

// notsa
tPedSpeechSlotID CAEPedSpeechAudioEntity::GetFreeSpeechSlot() {
    const auto size = s_PedSpeechSlots.size() - 1; // Last one always reserved for the player
    for (size_t n = 0; n < size; n++) {
        const auto i = (s_NextSpeechSlot + n) % size;
        if (s_PedSpeechSlots[i].Status == CAEPedSpeechSlot::eStatus::FREE) {
            s_NextSpeechSlot = (tPedSpeechSlotID)((i + 1u) % size);
            return (tPedSpeechSlotID)i;
        }
    }
    return -1;
}

// 0x4E4470
eSpecificSpeechContext CAEPedSpeechAudioEntity::GetSpecificSpeechContext(eGlobalSpeechContext gCtx, eAudioPedType pedAudioType) {
    // Omitted useless `if`s
    // NOTE: Original code allowed `PED_TYPE_SPC` as a valid ped type too... but that (technically) caused an out-of-bounds read...
    //       We won't assert on that, because there are bounds check for the array (as we use std::array)
    if (const auto* const ctxi = GetGlobalSpeechContextInfo(gCtx)) {
        return ctxi->SpecificSpeechContext[pedAudioType];
    }
    return -1;
}

// notsa
const tGlobalSpeechContextInfo* CAEPedSpeechAudioEntity::GetGlobalSpeechContextInfo(eGlobalSpeechContext gCtx) {
    assert(gCtx > 0 && gCtx < CTX_GLOBAL_NUM);

    // Must use a loop because there are a few skipped values (TODO: Though I guess we could fix this?)
    for (const auto& e : gSpeechContextLookup) {
        if (e.GCtx == gCtx) {
            return &e;
        }
    }
    return nullptr;
}

// notsa
const tSpecificSpeechContextInfo* CAEPedSpeechAudioEntity::GetSpecificSpeechContextInfo(eSpecificSpeechContext sCtx, eGlobalSpeechContext gCtx, eAudioPedType pt, ePedSpeechVoiceS16 voice) {
    if (IsGlobalContextPain(gCtx)) {
        return &gPainSpeechLookup[sCtx][voice];
    }
    switch (pt) {
    case PED_TYPE_GEN:    return &gGenSpeechLookup[sCtx][voice];
    case PED_TYPE_EMG:    return &gEmgSpeechLookup[sCtx][voice];
    case PED_TYPE_PLAYER: return &gPlySpeechLookup[sCtx][voice];
    case PED_TYPE_GANG:   return &gGngSpeechLookup[sCtx][voice];
    case PED_TYPE_GFD:    return &gGfdSpeechLookup[sCtx][voice];
    default:              return nullptr;
    }
}

eSoundBank CAEPedSpeechAudioEntity::GetVoiceSoundBank(eGlobalSpeechContext gCtx, eAudioPedType pt, ePedSpeechVoiceS16 voice) {
    if (IsGlobalContextPain(gCtx)) {
        return gPainVoiceToBankLookup[voice];
    }
    switch (pt) {
    case PED_TYPE_GEN:    return gGenSpeechVoiceToBankLookup[voice];
    case PED_TYPE_EMG:    return gEmgSpeechVoiceToBankLookup[voice];
    case PED_TYPE_PLAYER: return gPlySpeechVoiceToBankLookup[voice];
    case PED_TYPE_GANG:   return gGngSpeechVoiceToBankLookup[voice];
    case PED_TYPE_GFD:    return gGfdSpeechVoiceToBankLookup[voice];
    default:              NOTSA_UNREACHABLE();
    }
}

// The templated functions below must be here because
// due to a cycle dependency with `CPed` they can't be defined in the header

// 0x4E6550 / 0x4E60D0
template<bool IsPedless>
int16 CAEPedSpeechAudioEntity::I_AddSayEvent(CVector pos, eAudioEvents audioEvent, eGlobalSpeechContext gCtx, uint32 startTimeDelayMs, float probability, bool overrideSilence, bool isForceAudible, bool isFrontEnd) {
    if (!m_IsInitialized || s_bAllSpeechDisabled || m_IsSpeechForScriptsDisabled) {
        return -1;
    }

    if (TheCamera.m_bWideScreenOn && !IsGlobalContextImportantForWidescreen(gCtx)) {
        return -1;
    }

    if constexpr (!IsPedless) {
        if (gCtx == CTX_GLOBAL_CHAT) { // 0x4E65BA
            if (CTimer::GetFrameCounter() % 64 || s_bPedConversationHappening || s_bPlayerConversationHappening) {
                return -1;
            }
        } else if (gCtx == CTX_GLOBAL_PAIN_CJ_DROWNING && CWeather::UnderWaterness < 0.5f)  {
            return -1;
        }
    }

    if (CGameLogic::GameState == eGameState::GAME_STATE_PLAYING_LOGO) {
        return -1;
    }

    if (!CAEAudioUtility::ResolveProbability(probability) || audioEvent != AE_SPEECH_PED) {
        return -1;
    }

    if (m_IsSpeechDisabled && !overrideSilence) {
        return -1;
    }

    if constexpr (IsPedless) {
        if (m_IsPlayingSpeech) { // 0x4E616C
            return -1;
        }
        if (gCtx < CTX_GLOBAL_POLICE_BOAT || gCtx > CTX_GLOBAL_POLICE_OVERBOARD) { // 0x4E6176
            return -1;
        }
    } else { // 0x4E6638
        const auto IsContextImportant = [](eGlobalSpeechContext c) {
            return IsGlobalContextPain(c) || IsGlobalContextImportantForInterupting(c);
            };
        if (IsContextImportant(gCtx)) {
            if (const auto* const speech = GetCurrentSpeech()) {
                if (speech->GCtx != CTX_GLOBAL_PAIN_CJ_HIGH_FALL && IsContextImportant(speech->GCtx)) {
                    return -1;
                }
            }
            StopCurrentSpeech();
        } else if (m_IsPlayingSpeech) { // 0x4E664D
            return -1;
        }

        if (!m_pEntity->AsPed()->IsAlive() && !IsGlobalContextPain(gCtx)) { // 0x4E66A7
            return -1;
        }
    }

    if (IsGlobalContextImportantForStreaming(gCtx) || isForceAudible || isFrontEnd) {
        if (CStreaming::ms_numModelsRequested > 15 && CAEVehicleAudioEntity::s_pVehicleAudioSettingsForRadio) {
            return -1;
        }
    } else if (CStreaming::IsVeryBusy()) {
        return -1;
    }

    if constexpr (!IsPedless) {
        if (m_PedAudioType == PED_TYPE_PLAYER) {
            if (FindPlayerPed() != m_pEntity || s_bAPlayerSpeaking && !m_IsPlayingSpeech) {
                return -1;
            }
        }
    }

    if (CVector::DistSqr(TheCamera.GetPosition(), pos) >= sq(IsPedless ? 60.f : 40.f)) {
        return -1;
    }

    eSpecificSpeechContext sCtx;
    const auto soundID = CAEPedSpeechAudioEntity::GetSoundAndBankIDs(gCtx, sCtx);
    if (soundID == -1) {
        return -1;
    }

    const auto speechSlotID = !IsPedless && m_PedAudioType == PED_TYPE_PLAYER
        ? CTimer::GetTimeInMS() >= GetNextPlayTime(gCtx)
        ? PLAYER_SPEECH_SLOT
        : -1
        : CanWePlayGlobalSpeechContext(gCtx);
    if (speechSlotID == -1) {
        return -1;
    }

    SetNextPlayTime(gCtx);
    m_IsForcedAudible = isForceAudible;

    m_IsFrontend = isFrontEnd;
    if constexpr (!IsPedless) {
        if (const auto veh = m_pEntity->AsPed()->GetVehicleIfInOne()) {
            if (veh == FindPlayerVehicle() && gCtx != CTX_GLOBAL_STOMACH_RUMBLE) {
                m_IsFrontend = true;
            }
        }
    }

    m_EventVolume = GetDefaultVolume(AE_SPEECH_PED);
    if constexpr (!IsPedless) {
        if (m_PedAudioType == PED_TYPE_PLAYER) {
            m_EventVolume += 3.f;
        }
        m_EventVolume += GetSpeechContextVolumeOffset(gCtx);
    }

    m_IsPlayingSpeech = true;
    if constexpr (!IsPedless) {
        if (m_PedAudioType == PED_TYPE_PLAYER) {
            s_bAPlayerSpeaking = true;
        }
    }

    m_LastGCtx        = gCtx;
    m_PedSpeechSlotID = speechSlotID;
    LoadAndPlaySpeech(startTimeDelayMs);

    return soundID;
}

template
int16 CAEPedSpeechAudioEntity::I_AddSayEvent<true>(CVector pos, eAudioEvents audioEvent, eGlobalSpeechContext gCtx, uint32 startTimeDelayMs, float probability, bool overrideSilence, bool isForceAudible, bool isFrontEnd);

// 0x4E3520 / 0x4E4D10
template<bool IsPedless>
void CAEPedSpeechAudioEntity::I_UpdateParameters(CAESound* sound, int16 playTime) {
    auto* const ped = IsPedless ? nullptr : m_pEntity->AsPed();
    auto* const speech = GetCurrentSpeech();
    if (playTime == -1) { // Sound has finished?
        *speech = {
            .Status = CAEPedSpeechSlot::eStatus::FREE
        };

        m_Sound           = nullptr;
        m_SoundID         = -1;
        m_BankID          = SND_BANK_UNK;
        m_IsForcedAudible = false;
        m_IsPlayingSpeech = false;
        m_PedSpeechSlotID = -1;

        if constexpr (!IsPedless) {
            if (m_PedAudioType == PED_TYPE_PLAYER) {
                s_bAPlayerSpeaking = false;
            }

            if (s_bPedConversationHappening) {
                if (   this == &s_pConversationPed1->GetSpeechAE() && m_PedSpeechSlotID == s_pConversationPedSlot1
                    || this == &s_pConversationPed2->GetSpeechAE() && m_PedSpeechSlotID == s_pConversationPedSlot2
                ) {
                    speech->Status = CAEPedSpeechSlot::eStatus::RESERVED;
                }
            } else if (s_bPlayerConversationHappening) {
                if (this == &s_pPlayerConversationPed->GetSpeechAE() && m_PedSpeechSlotID == s_pConversationPedSlot1) {
                    speech->Status = CAEPedSpeechSlot::eStatus::RESERVED;
                }
            }

            if (auto* const tFacial = notsa::cast<CTaskComplexFacial>(m_pEntity->AsPed()->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_FACIAL_COMPLEX))) {
                tFacial->StopAll();
            }
        }
    } else { // Update sound
        speech->Status = CAEPedSpeechSlot::eStatus::PLAYING;
        if constexpr (IsPedless) {
            if (!m_IsFrontend || sound->m_nEvent == AE_SPEECH_PED) {
                sound->SetPosition(m_pEntity->GetPosition());
            }
        } else {
            if (ped->bIsTalking && ped->m_nBodypartToRemove == PED_NODE_HEAD) {
                sound->StopSound();
            } else if (!m_IsFrontend) {
                sound->SetPosition(m_pEntity->GetPosition());
            }
        }
    }
}

template
void CAEPedSpeechAudioEntity::I_UpdateParameters<true>(CAESound* sound, int16 playTime);

// 0x4E5CD0 / 0x4E6380
template<bool IsPedless>
void CAEPedSpeechAudioEntity::I_PlayLoadedSound(CEntity* attachTo) {
    if (!attachTo) {
        // Done by the caller (No other solution)
        // if constexpr (IsPedless) {
        //     StopCurrentSpeech()
        // }
        return;
    }

    const auto RequestSound = [&](CVector pos, uint32 flags, float volume, float maxDist) {
        if constexpr (IsPedless) {
            flags |= SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY;
        }
        CAESound s;
        s.Initialise(
            SND_BANK_SLOT_SPEECH1 + m_PedSpeechSlotID,
            m_SoundID,
            this,
            pos,
            m_IsForcedAudible ? 3.f : volume,
            maxDist,
            1.f,
            1.f,
            0,
            (eSoundEnvironment)flags
        );
        if constexpr (IsPedless) {
            s.RegisterWithPhysicalEntity(attachTo);
            // this->m_AttachedTo = nulllptr; Done by the caller
        }
        m_Sound = AESoundManager.RequestNewSound(&s);
    };

    if (m_IsFrontend) {
        RequestSound(
            {0.f, 1.f, 0.f},
            SOUND_UNCOMPRESSABLE | SOUND_UNDUCKABLE | SOUND_REQUEST_UPDATES | SOUND_UNCANCELLABLE | SOUND_FRONT_END,
            3.f,
            1.f
        );
    } else {
        RequestSound(
            attachTo->GetPosition(),
            SOUND_REQUEST_UPDATES | SOUND_UNCANCELLABLE,
            IsPedless ? 12.f : m_EventVolume,
            IsPedless ? 3.5f : CAEVehicleAudioEntity::s_pVehicleAudioSettingsForRadio ? 3.f : 2.f
        );
    }

    auto* const speech = GetCurrentSpeech();
    if (m_Sound) {
        speech->Sound = m_Sound;
        speech->Status = CAEPedSpeechSlot::eStatus::PLAYING;
        if constexpr (!IsPedless) {
            switch (m_LastGCtx) {
            case CTX_GLOBAL_BREATHING:
            case CTX_GLOBAL_STOMACH_RUMBLE:
                break;
            default: {
                notsa::cast<CTaskComplexFacial>(m_pEntity->AsPed()->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_FACIAL_COMPLEX))->SetRequest(
                    eFacialExpression::TALKING,
                    2800
                );
            }
            }
        }
    } else {
        *speech = CAEPedSpeechSlot{};
        if constexpr (!IsPedless) {
            if (m_PedAudioType == PED_TYPE_PLAYER) {
                s_bAPlayerSpeaking = false;
            }
        }
        m_IsPlayingSpeech = false;
        m_PedSpeechSlotID = -1;
    }
}


template
void CAEPedSpeechAudioEntity::I_PlayLoadedSound<true>(CEntity* attachTo);
