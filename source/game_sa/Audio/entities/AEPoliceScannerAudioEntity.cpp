#include "StdInc.h"

#include "AEPoliceScannerAudioEntity.h"

#include "AEAudioHardware.h"

#define NUM_POLICE_SCANNER_SLOTS 5

float& CAEPoliceScannerAudioEntity::s_fVolumeOffset = *(float*)0xB61CF8;
CAESound*& CAEPoliceScannerAudioEntity::s_pSound = *(CAESound**)0xB61D00;
uint32& CAEPoliceScannerAudioEntity::s_nAbortPlaybackTime = *(uint32*)0xB61D08;
uint32& CAEPoliceScannerAudioEntity::s_nPlaybackStartTime = *(uint32*)0xB61D0C;
int16& CAEPoliceScannerAudioEntity::s_nSectionPlaying = *(int16*)0xB61D04;

int16 (&CAEPoliceScannerAudioEntity::s_SlotState)[NUM_POLICE_SCANNER_SLOTS] = *(int16(*)[NUM_POLICE_SCANNER_SLOTS])0xB61D14;
tScannerSlot* (&CAEPoliceScannerAudioEntity::s_pCurrentSlots) = *(tScannerSlot*(*))0xB61D10;
tScannerSlot (&CAEPoliceScannerAudioEntity::s_ScannerSlotFirst)[NUM_POLICE_SCANNER_SLOTS] = *(tScannerSlot (*)[NUM_POLICE_SCANNER_SLOTS])0xB61D34;
tScannerSlot (&CAEPoliceScannerAudioEntity::s_ScannerSlotSecond)[NUM_POLICE_SCANNER_SLOTS] = *(tScannerSlot (*)[NUM_POLICE_SCANNER_SLOTS])0xB61D20;
bool& CAEPoliceScannerAudioEntity::s_bStoppingScanner = *(bool*)0xB61CFC;
bool& CAEPoliceScannerAudioEntity::s_bScannerDisabled = *(bool*)0xB61D4E;
CAEPoliceScannerAudioEntity::State& CAEPoliceScannerAudioEntity::s_nScannerPlaybackState = *(CAEPoliceScannerAudioEntity::State*)0xB61D4C;
uint32& CAEPoliceScannerAudioEntity::s_NextNewScannerDialogueTime = *(uint32*)0xB61D50;

CAEPoliceScannerAudioEntity* CAEPoliceScannerAudioEntity::s_pPSControlling = *(CAEPoliceScannerAudioEntity**)0xB61D48;

// 0x4E6E00
CAEPoliceScannerAudioEntity::~CAEPoliceScannerAudioEntity() {
    if (s_pPSControlling == this && s_nScannerPlaybackState != STATE_INITIAL) {
        s_bStoppingScanner = true;
        if (s_pSound) {
            s_pSound->StopSoundAndForget();
            s_pSound = nullptr;
        }
        FinishedPlayingScannerDialogue();
    }
}

// 0x5B9C30
void CAEPoliceScannerAudioEntity::StaticInitialise() {
    s_NextNewScannerDialogueTime = 0;
    s_bScannerDisabled           = false;
    s_nScannerPlaybackState      = STATE_INITIAL;
    s_pPSControlling             = nullptr;
    s_pCurrentSlots              = nullptr;
    s_fVolumeOffset              = 0.0f;
}

// 0x4E6E90
void CAEPoliceScannerAudioEntity::Reset() {
    StopScanner(true);
    FinishedPlayingScannerDialogue();
}

// 0x4E71E0
void CAEPoliceScannerAudioEntity::AddAudioEvent(eAudioEvents event, eCrimeType crimeType, const CVector& point) {
    plugin::CallMethod<0x4E71E0, CAEPoliceScannerAudioEntity*, eAudioEvents, eCrimeType, const CVector&>(this, event, crimeType, point);
}

// 0x4E6BC0
void CAEPoliceScannerAudioEntity::PrepSlots() {
    for (auto i = 0; i < NUM_POLICE_SCANNER_SLOTS; i++) {
        s_SlotState[i] = s_pCurrentSlots[i].IsNotInitialized();
    }
}

// 0x4E6CD0
void CAEPoliceScannerAudioEntity::LoadSlots() {
    if (!s_pCurrentSlots) {
        return;
    }

    int16 bankSlotId = 33;
    bool canPlay = true;

    for (auto i = 0; i < NUM_POLICE_SCANNER_SLOTS; i++) {
        auto& currentSlot = s_pCurrentSlots[i];

        if (s_SlotState[i])
        {
            if (s_SlotState[i] == 2) {
                bool loaded = AEAudioHardware.IsSoundLoaded(currentSlot.bankId, currentSlot.sfxId, bankSlotId);
                if (loaded)
                    s_SlotState[i] = 3;
                else
                    canPlay = false;
            }
        }
        else if (currentSlot.IsNotInitialized())
        {
            s_SlotState[i] = 1;
        }
        else
        {
            if (!CStreaming::IsVeryBusy()) {
                AEAudioHardware.LoadSound(currentSlot.bankId, currentSlot.sfxId, bankSlotId);
                s_SlotState[i] = 2;
            }
            canPlay = false;
        }

        ++bankSlotId;
    }

    if (canPlay) {
        s_nScannerPlaybackState = FOUR;
    }
}

// 0x4E6DB0
void CAEPoliceScannerAudioEntity::EnableScanner() {
    s_bScannerDisabled = false;
}

// 0x4E71B0
void CAEPoliceScannerAudioEntity::DisableScanner(bool a1, bool bStopSound) {
    s_bScannerDisabled = true;
    if (a1 && s_nScannerPlaybackState != STATE_INITIAL) {
        if (s_pPSControlling) {
            StopScanner(bStopSound);
        }
    }
}

// 0x4E6DC0
void CAEPoliceScannerAudioEntity::StopScanner(bool bStopSound) {
    if (s_nScannerPlaybackState == STATE_INITIAL)
        return;

    s_bStoppingScanner = true;
    if (bStopSound) {
        if (s_pSound) {
            s_pSound->StopSoundAndForget();
            s_pSound = nullptr;
        }
        FinishedPlayingScannerDialogue();
    }
}

// 0x4E6C30
void CAEPoliceScannerAudioEntity::FinishedPlayingScannerDialogue() {
    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SCANNER_NOISE_STOP);
    s_nScannerPlaybackState      = STATE_INITIAL;
    s_pPSControlling             = nullptr;
    s_pCurrentSlots              = nullptr;
    s_bStoppingScanner           = false;
    s_NextNewScannerDialogueTime = s_NextNewScannerDialogueTime + CTimer::GetTimeInMS();
    s_fVolumeOffset              = 0.0f;

    std::ranges::fill(s_SlotState, -1);
    s_SlotState[4] = 1;

    for (auto i = 0; i < NUM_POLICE_SCANNER_SLOTS; i++) {
        s_ScannerSlotFirst[i].Clear();
        s_ScannerSlotSecond[i].Clear();
    }
}

// 0x4E6F60
void CAEPoliceScannerAudioEntity::PlayLoadedDialogue() {
    return plugin::CallMethod<0x4E6F60, CAEPoliceScannerAudioEntity*>(this);

    int16 i;
    for (i = 0; i < NUM_POLICE_SCANNER_SLOTS; ++i) {
        if (s_SlotState[i] == FIVE) {
            break;
        }
    }

    if (i == NUM_POLICE_SCANNER_SLOTS) {
        i = 0;
        goto LABEL_7;
    }

    if (i >= NUM_POLICE_SCANNER_SLOTS) {
    LABEL_9:
        auto volumeChange = s_fVolumeOffset /* + flt_B61D54 */;
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SCANNER_CLICK, volumeChange);
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SCANNER_NOISE_STOP);
        if (s_nSectionPlaying) {
            FinishedPlayingScannerDialogue();
        } else {
            auto v2 = (int16*)s_ScannerSlotSecond;
            s_pCurrentSlots = s_ScannerSlotSecond;
            s_nSectionPlaying = 1;
            auto v3 = s_SlotState;
            auto v4 = 5;
            do {
                *v3 = *v2 < 0 || v2[1] < 0;
                v2 += 2;
                ++v3;
                --v4;
            } while (v4);
            s_nPlaybackStartTime = 0;
            // s_nAbortPlaybackTime = *(_DWORD*)&gSpeechContextLookup[366][0] + CTimer::GetTimeInMS();
            s_nScannerPlaybackState = TWO;
        }
    } else {
    LABEL_7:
        while (s_SlotState[i] != 3) {
            if (++i >= 5)
                goto LABEL_9;
        }

        auto volume = GetDefaultVolume(AE_CRIME_COMMITTED) + s_fVolumeOffset;
        CAESound sound;
        sound.Initialise(i + 33, s_pCurrentSlots[i].sfxId, this, { 0.0, 1.0f, 0.0f }, volume, 1.0f, 1.0f, 1.0f, 0, SOUND_DEFAULT, 0.0f, 0);
        sound.m_fMaxVolume = (float)i;
        sound.m_nEnvironmentFlags = SOUND_FRONT_END | SOUND_UNCANCELLABLE | SOUND_REQUEST_UPDATES | SOUND_UNDUCKABLE;
        sound.m_nEvent = AE_CRIME_COMMITTED;

        s_pSound = AESoundManager.RequestNewSound(&sound);
        if (s_pSound) {
            s_SlotState[i] = 5;
            s_nScannerPlaybackState = SEVEN;
        }
    }
}

// 0x4E6B60
void CAEPoliceScannerAudioEntity::PopulateScannerDialogueLists(const tScannerSlot* first, const tScannerSlot* second) {
    assert(first && second);
    if (s_nScannerPlaybackState == STATE_INITIAL) {
        for (auto slotIndex = 0; slotIndex < NUM_POLICE_SCANNER_SLOTS; slotIndex++) {
            s_ScannerSlotFirst[slotIndex] = first[slotIndex];
            s_ScannerSlotSecond[slotIndex] = second[slotIndex];
        }
    }
}

// inlined
// 0x4E6C00
bool CAEPoliceScannerAudioEntity::CanWePlayNewScannerDialogue() {
    if (s_nScannerPlaybackState == STATE_INITIAL)
        return false;

    if (CTimer::GetTimeInMS() < s_NextNewScannerDialogueTime)
        return false;

    if (TheCamera.m_bWideScreenOn)
        return false;

    if (s_bScannerDisabled)
        return false;

    return true;
}

// 0x4E6ED0
void CAEPoliceScannerAudioEntity::PlayPoliceScannerDialogue(tScannerSlot* first, tScannerSlot* second) {
    assert(first && second);
    if (CanWePlayNewScannerDialogue()) { // todo: maybe little bit wrong
        PopulateScannerDialogueLists(first, second);
        s_pPSControlling = this;
        s_nSectionPlaying = 0;
        s_pCurrentSlots = s_ScannerSlotFirst;
        PrepSlots();
        s_nScannerPlaybackState = TWO;
        s_nPlaybackStartTime = CTimer::GetTimeInMS() + 2000; // todo: gSpeechContextLookup[365][6]
        s_nAbortPlaybackTime = CTimer::GetTimeInMS() + 5000; // todo: gSpeechContextLookup[366][0]
    }
}

// 0x4E7590
void CAEPoliceScannerAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (curPlayPos == -1) {
        s_pSound = nullptr;
        if (s_bStoppingScanner) {
            if (s_nScannerPlaybackState != STATE_INITIAL) {
                s_bStoppingScanner = true; // V1048 [CWE-1164] The 's_bStoppingScanner' variable was assigned the same value
                FinishedPlayingScannerDialogue();
            }
            return;
        }
        PlayLoadedDialogue();
        return;
    }

    if (s_bStoppingScanner) {
        sound->m_fVolume = sound->m_fVolume - 6.0f; // todo: *(float*)&gSpeechContextLookup[366][4]
        return;
    }

    if (sound->m_nSoundLength > 0 && curPlayPos > sound->m_nSoundLength - 40 && sound->m_nBankSlotId != 37) { // todo: -40 should be replaced with by gSpeechContextLookup[366][2]
        sound->SetIndividualEnvironment(eSoundEnvironment::SOUND_REQUEST_UPDATES, false);
        s_pSound = nullptr;
        PlayLoadedDialogue();
        return;
    }
}

// 0x4E7630
void CAEPoliceScannerAudioEntity::Service() {
    static constexpr uint32 startDelay = 300;   // 0x8C8154
    static constexpr float noiseVolume = -6.0f; // 0x8C8158
    static constexpr float clickVolume = +0.0f; // 0xB61D54

    bool finishPlaying;
    if (TheCamera.m_bWideScreenOn && s_nScannerPlaybackState != STATE_INITIAL) {
        finishPlaying = true;
        s_bStoppingScanner = true;
    } else {
        finishPlaying = s_bStoppingScanner;
    }

    switch (s_nScannerPlaybackState) {
    case TWO:
        if (CTimer::GetTimeInMS() > s_nAbortPlaybackTime || finishPlaying) {
            FinishedPlayingScannerDialogue();
            break;
        }

        LoadSlots();
        break;
    case FOUR:
        if (finishPlaying) {
            FinishedPlayingScannerDialogue();
            break;
        }

        if (CTimer::GetTimeInMS() >= s_nPlaybackStartTime) {
            s_fVolumeOffset = CAEVehicleAudioEntity::s_pVehicleAudioSettingsForRadio ? 0.0f : -8.0f; // todo: -8 is gSpeechContextLookup[367][2]
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SCANNER_CLICK,       s_fVolumeOffset + clickVolume, 1.0f);
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SCANNER_NOISE_START, s_fVolumeOffset + noiseVolume, 1.0f); // todo: noiseVolume is gSpeechContextLookup[367][0]
            s_nPlaybackStartTime = startDelay + CTimer::GetTimeInMS(); // todo: startDelay is gSpeechContextLookup[366][6]
            s_nScannerPlaybackState = FIVE;
        }
        break;
    case FIVE:
        if (finishPlaying) {
            FinishedPlayingScannerDialogue();
            break;
        }

        if (CTimer::GetTimeInMS() >= s_nPlaybackStartTime && s_pPSControlling) {
            s_nScannerPlaybackState = SEVEN;
            s_pPSControlling->PlayLoadedDialogue();
        }
        break;
    default:
        return;
    }
}

void CAEPoliceScannerAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEPoliceScannerAudioEntity, 0x85F368, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Constructor, 0x56DA00);
    RH_ScopedInstall(Destructor, 0x4E6E00);
    RH_ScopedInstall(StaticInitialise, 0x5B9C30);
    RH_ScopedInstall(Reset, 0x4E6E90);
    RH_ScopedInstall(AddAudioEvent, 0x4E71E0, { .reversed = false });
    RH_ScopedInstall(PrepSlots, 0x4E6BC0);
    RH_ScopedInstall(LoadSlots, 0x4E6CD0, { .reversed = false });
    RH_ScopedInstall(EnableScanner, 0x4E6DB0);
    RH_ScopedInstall(DisableScanner, 0x4E71B0);
    RH_ScopedInstall(StopScanner, 0x4E6DC0);
    RH_ScopedInstall(FinishedPlayingScannerDialogue, 0x4E6C30);
    RH_ScopedInstall(PlayLoadedDialogue, 0x4E6F60, { .reversed = false });
    RH_ScopedInstall(PopulateScannerDialogueLists, 0x4E6B60);
    RH_ScopedInstall(CanWePlayNewScannerDialogue, 0x4E6C00);
    RH_ScopedInstall(PlayPoliceScannerDialogue, 0x4E6ED0);
    RH_ScopedVMTInstall(UpdateParameters, 0x4E7590);
    RH_ScopedInstall(Service, 0x4E7630, { .reversed = false });
}

CAEPoliceScannerAudioEntity* CAEPoliceScannerAudioEntity::Constructor() {
    this->CAEPoliceScannerAudioEntity::CAEPoliceScannerAudioEntity();
    return this;
}

CAEPoliceScannerAudioEntity* CAEPoliceScannerAudioEntity::Destructor() {
    this->CAEPoliceScannerAudioEntity::~CAEPoliceScannerAudioEntity();
    return this;
}