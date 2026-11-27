#include "StdInc.h"

#include "AEPoliceScannerAudioEntity.h"

#include "AEAudioHardware.h"
#include "AEAudioUtility.h"
#include "TheZones.h"

namespace {
constexpr uint32 NEW_DIALOGUE_DELAY_MS      = 10'000; // 0x8C8140
constexpr uint32 PLAYBACK_START_DELAY_MS    = 2'000;  // 0x8C8144
constexpr uint32 ABORT_PLAYBACK_DELAY_MS    = 5'000;  // 0x8C8148
constexpr int16  NEXT_SOUND_OVERLAP_MS      = 40;     // 0x8C814C
constexpr float  STOPPING_VOLUME_STEP       = 6.f;    // 0x8C8150
constexpr uint32 NOISE_TO_DIALOGUE_DELAY_MS = 300;    // 0x8C8154
constexpr float  NOISE_VOLUME_OFFSET        = -6.f;   // 0x8C8158
constexpr float  NO_RADIO_VOLUME_OFFSET     = -8.f;   // 0x8C815C

// SND_BANK_SCRIPT_SCANNER_INSTRUCTIONS
constexpr eSoundID SND_INSTRUCTION_IN_A     = 1;
constexpr eSoundID SND_INSTRUCTION_SWIMMING = 2;
constexpr eSoundID SND_INSTRUCTION_ON_A     = 3;
constexpr eSoundID SND_INSTRUCTION_ON_FOOT  = 4;
constexpr eSoundID SND_INSTRUCTION_SUSPECT  = 7;

// SND_BANK_SCRIPT_SCANNER_DIRECTIONS
constexpr eSoundID SND_DIRECTION_CENTRAL = 0;
constexpr eSoundID SND_DIRECTION_EAST    = 1;
constexpr eSoundID SND_DIRECTION_NORTH   = 2;
constexpr eSoundID SND_DIRECTION_SOUTH   = 3;
constexpr eSoundID SND_DIRECTION_WEST    = 4;

// SND_BANK_SCRIPT_SCANNER_COLOURS
constexpr eSoundID SND_COLOUR_REMAPPED = 4;

#include "AEPoliceScannerAudioEntityLUTs.inc.h"
} // namespace

// 0x4E6E00
CAEPoliceScannerAudioEntity::~CAEPoliceScannerAudioEntity() {
    if (ms_PSControlling == this && ms_ScannerPlaybackState != STATE_IDLE) {
        ms_IsStoppingScanner = true;
        if (ms_Sound) {
            ms_Sound->StopSoundAndForget();
            ms_Sound = nullptr;
        }
        FinishedPlayingScannerDialogue();
    }
}

// 0x5B9C30
void CAEPoliceScannerAudioEntity::StaticInitialise() {
    ms_NextNewScannerDialogueTime = 0;
    ms_IsScannerDisabled          = false;
    ms_ScannerPlaybackState       = STATE_IDLE;
    ms_PSControlling              = nullptr;
    ms_CurrentSlots               = nullptr;
    ms_VolumeOffset               = 0.0f;
}

// 0x4E6E90
void CAEPoliceScannerAudioEntity::Reset() {
    StopScanner(true);
    FinishedPlayingScannerDialogue();
}

// 0x4E71E0
void CAEPoliceScannerAudioEntity::AddAudioEvent(eAudioEvents event, eCrimeType crimeType, const CVector& point) {
    std::array<tScannerSlot, NUM_POLICE_SCANNER_SLOTS> first{}, second{};

    if (event != AE_CRIME_COMMITTED) {
        return;
    }
    if (crimeType <= CRIME_FIRE_WEAPON || crimeType > CRIME_AIM_GUN) {
        return;
    }

    first[0]               = { SND_BANK_SCRIPT_SCANNER_INSTRUCTIONS, INTRO_SOUNDS[CAEAudioUtility::GetRandomNumberInRange(0, 3)] };
    first[1]               = { SND_BANK_SCRIPT_SCANNER_NUMBERS, CRIME_SOUNDS[crimeType] };

    const auto* const zone = CTheZones::FindSmallestZoneForPosition(point, true);
    if (!zone) {
        return;
    }
    const auto scannerZone = rng::find_if(SCANNER_ZONES, [zone](const tScannerZone& sz) {
        return rng::equal(sz.Name, zone->m_TextLabel) && sz.SoundID >= 0;
    });
    if (scannerZone == SCANNER_ZONES.end()) {
        return;
    }

    // Which part of the zone: the middle half of the zone's rect (on each axis) is "central"
    if (scannerZone->HasDirection) {
        const float width         = (float)(zone->m_fX2 - zone->m_fX1);
        const float height        = (float)(zone->m_fY2 - zone->m_fY1);
        const float centerX       = width * 0.5f + (float)zone->m_fX1;
        const float centerY       = 0.5f * height + (float)zone->m_fY1;
        const float quarterW      = width * 0.25f;
        const float quarterH      = height * 0.25f;
        bool        hasNorthSouth = false;
        if (point.y > centerY + quarterH) { // 0x4E7386
            first[2]      = { SND_BANK_SCRIPT_SCANNER_DIRECTIONS, SND_DIRECTION_NORTH };
            hasNorthSouth = true;
        } else if (point.y < centerY - quarterH) { // 0x4E73A3
            first[2]      = { SND_BANK_SCRIPT_SCANNER_DIRECTIONS, SND_DIRECTION_SOUTH };
            hasNorthSouth = true;
        }
        if (point.x > centerX + quarterW) { // 0x4E73C2
            first[3] = { SND_BANK_SCRIPT_SCANNER_DIRECTIONS, SND_DIRECTION_EAST };
        } else if (point.x < centerX - quarterW) { // 0x4E73DC
            first[3] = { SND_BANK_SCRIPT_SCANNER_DIRECTIONS, SND_DIRECTION_WEST };
        } else if (!hasNorthSouth) { // 0x4E73E9
            first[3] = { SND_BANK_SCRIPT_SCANNER_DIRECTIONS, SND_DIRECTION_CENTRAL };
        }
    }
    first[4] = { SND_BANK_SCRIPT_SCANNER_AREAS, scannerZone->SoundID };

    // Describe the player this scanner (`CWanted::m_PoliceScannerAudioEntity`) belongs to
    CPlayerPed* player{};
    for (auto i = 0; i < MAX_PLAYERS; i++) {
        if (auto* const wanted = FindPlayerWanted(i); wanted && &wanted->m_PoliceScannerAudioEntity == this) {
            player = FindPlayerPed(i);
        }
    }
    if (player) {
        // NOTE: Not `IsInVehicle()`: `bInVehicle` without a vehicle says nothing about the player, it doesn't take the on foot path
        if (player->bInVehicle) {
            auto* const veh     = player->GetVehicleIfInOne();
            const auto  vehType = veh ? veh->m_vehicleAudio.m_AuSettings.VehicleAudioTypeForName : AE_VAT_NONE;
            if (vehType >= 0 && vehType < AE_VAT_END && SCANNER_VEHICLES[vehType].SoundID >= 0) { // 0x4E7489
                const auto& scannerVeh = SCANNER_VEHICLES[vehType];
                const auto  isOnA      = notsa::contains({ AE_VAT_MOPED, AE_VAT_BIKE, AE_VAT_QUADBIKE, AE_VAT_MOWER, AE_VAT_BICYCLE, AE_VAT_TRACTOR }, vehType);
                second[0]              = { SND_BANK_SCRIPT_SCANNER_INSTRUCTIONS, SND_INSTRUCTION_SUSPECT };
                second[1]              = { SND_BANK_SCRIPT_SCANNER_INSTRUCTIONS, isOnA ? SND_INSTRUCTION_ON_A : SND_INSTRUCTION_IN_A };
                if (scannerVeh.HasColour) {
                    if (veh->GetRemapIndex() != -1) { // 0x4E74E0
                        second[3] = { SND_BANK_SCRIPT_SCANNER_COLOURS, SND_COLOUR_REMAPPED };
                    } else if (const auto colour = veh->m_nPrimaryColor; colour > 0 && colour < SCANNER_COLOURS.size() && SCANNER_COLOURS[colour] >= 0) { // 0x4E74E9
                        second[3] = { SND_BANK_SCRIPT_SCANNER_COLOURS, SCANNER_COLOURS[colour] };
                    }
                }
                second[4] = { SND_BANK_SCRIPT_SCANNER_VEHICLES, scannerVeh.SoundID };
            }
        } else if (player->GetIntelligence()->GetTaskSwim()) { // 0x4E7538
            second[0] = { SND_BANK_SCRIPT_SCANNER_INSTRUCTIONS, SND_INSTRUCTION_SUSPECT };
            second[4] = { SND_BANK_SCRIPT_SCANNER_INSTRUCTIONS, SND_INSTRUCTION_SWIMMING };
        } else if (!player->GetIntelligence()->GetTaskJetPack()) { // 0x4E7550
            second[0] = { SND_BANK_SCRIPT_SCANNER_INSTRUCTIONS, SND_INSTRUCTION_SUSPECT };
            second[4] = { SND_BANK_SCRIPT_SCANNER_INSTRUCTIONS, SND_INSTRUCTION_ON_FOOT };
        }
    }

    PlayPoliceScannerDialogue(first.data(), second.data());
}

// 0x4E6BC0
void CAEPoliceScannerAudioEntity::PrepSlots() {
    if (!ms_CurrentSlots) {
        return;
    }
    for (auto&& [i, state] : rngv::enumerate(ms_SlotState)) {
        state = ms_CurrentSlots[i].IsEmpty() ? SLOT_STATE_EMPTY : SLOT_STATE_TO_LOAD;
    }
}

// 0x4E6CD0
void CAEPoliceScannerAudioEntity::LoadSlots() {
    if (!ms_CurrentSlots) {
        return;
    }

    bool allLoaded = true;
    for (auto&& [i, state] : rngv::enumerate(ms_SlotState)) {
        const auto  bankSlot = (eSoundBankSlot)(SND_BANK_SLOT_SCANNER_FIRST + i);
        const auto& slot     = ms_CurrentSlots[i];
        switch (state) {
        case SLOT_STATE_TO_LOAD: { // 0x4E6D3A
            if (slot.IsEmpty()) {
                state = SLOT_STATE_EMPTY;
                break;
            }
            if (!CStreaming::IsVeryBusy()) {
                AEAudioHardware.LoadSound(slot.Bank, slot.SoundID, bankSlot);
                state = SLOT_STATE_LOADING;
            }
            allLoaded = false;
            break;
        }
        case SLOT_STATE_LOADING: { // 0x4E6D0D
            if (AEAudioHardware.IsSoundLoaded(slot.Bank, slot.SoundID, bankSlot)) {
                state = SLOT_STATE_LOADED;
            } else {
                allLoaded = false;
            }
            break;
        }
        default:
            break;
        }
    }
    if (allLoaded) {
        ms_ScannerPlaybackState = STATE_LOADED;
    }
}

// 0x4E6DB0
void CAEPoliceScannerAudioEntity::EnableScanner() {
    ms_IsScannerDisabled = false;
}

// 0x4E71B0
void CAEPoliceScannerAudioEntity::DisableScanner(bool stopScanner, bool stopSound) {
    ms_IsScannerDisabled = true;
    if (stopScanner && ms_ScannerPlaybackState != STATE_IDLE && ms_PSControlling) {
        StopScanner(stopSound);
    }
}

// 0x4E6DC0
void CAEPoliceScannerAudioEntity::StopScanner(bool stopSound) {
    if (ms_ScannerPlaybackState == STATE_IDLE) {
        return;
    }

    ms_IsStoppingScanner = true;
    if (stopSound) {
        if (ms_Sound) {
            ms_Sound->StopSoundAndForget();
            ms_Sound = nullptr;
        }
        FinishedPlayingScannerDialogue();
    }
}

// 0x4E6C30
void CAEPoliceScannerAudioEntity::FinishedPlayingScannerDialogue() {
    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SCANNER_NOISE_STOP);
    ms_ScannerPlaybackState       = STATE_IDLE;
    ms_PSControlling              = nullptr;
    ms_CurrentSlots               = nullptr;
    ms_IsStoppingScanner          = false;
    ms_NextNewScannerDialogueTime = CTimer::GetTimeInMS() + NEW_DIALOGUE_DELAY_MS;
    ms_VolumeOffset               = 0.0f;

    ms_SlotState.fill(SLOT_STATE_EMPTY);

    ms_ScannerSlotFirst.fill({});
    ms_ScannerSlotSecond.fill({});
}

// 0x4E6F60
void CAEPoliceScannerAudioEntity::PlayLoadedDialogue() {
    // Continue after the slot played last (or from the first one)
    const auto played = rng::find(ms_SlotState, SLOT_STATE_PLAYED);
    for (int32 i = played == ms_SlotState.end() ? 0 : (int32)(played - ms_SlotState.begin()); i < NUM_POLICE_SCANNER_SLOTS; i++) {
        if (ms_SlotState[i] != SLOT_STATE_LOADED) {
            continue;
        }

        CAESound sound;
        sound.Initialise(
            (eSoundBankSlot)(SND_BANK_SLOT_SCANNER_FIRST + i),
            ms_CurrentSlots[i].SoundID,
            this,
            { 0.0f, 1.0f, 0.0f },
            GetDefaultVolume(AE_CRIME_COMMITTED) + ms_VolumeOffset
        );
        sound.m_ClientVariable = (float)i;
        sound.m_Flags          = SOUND_FRONT_END | SOUND_IS_CANCELLABLE | SOUND_REQUEST_UPDATES | SOUND_IS_DUCKABLE;
        sound.m_Event          = AE_CRIME_COMMITTED;
        ms_Sound               = AESoundManager.RequestNewSound(&sound);
        if (ms_Sound) {
            ms_SlotState[i]         = SLOT_STATE_PLAYED;
            ms_ScannerPlaybackState = STATE_PLAYING;
        }
        return;
    }

    // Section finished
    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SCANNER_CLICK, ms_ClickVolumeOffset + ms_VolumeOffset);
    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SCANNER_NOISE_STOP);
    if (ms_SectionPlaying != 0) {
        FinishedPlayingScannerDialogue();
        return;
    }

    // Load the second section
    ms_CurrentSlots   = ms_ScannerSlotSecond.data();
    ms_SectionPlaying = 1;
    PrepSlots();
    ms_AbortPlaybackTime    = CTimer::GetTimeInMS() + ABORT_PLAYBACK_DELAY_MS;
    ms_PlaybackStartTime    = 0;
    ms_ScannerPlaybackState = STATE_LOADING;
}

// 0x4E6B60
void CAEPoliceScannerAudioEntity::PopulateScannerDialogueLists(const tScannerSlot* first, const tScannerSlot* second) {
    assert(first && second);
    if (ms_ScannerPlaybackState == STATE_IDLE) {
        rng::copy_n(first, NUM_POLICE_SCANNER_SLOTS, ms_ScannerSlotFirst.begin());
        rng::copy_n(second, NUM_POLICE_SCANNER_SLOTS, ms_ScannerSlotSecond.begin());
    }
}

// 0x4E6C00
bool CAEPoliceScannerAudioEntity::CanWePlayNewScannerDialogue() {
    if (ms_ScannerPlaybackState != STATE_IDLE) {
        return false;
    }
    if (CTimer::GetTimeInMS() < ms_NextNewScannerDialogueTime) {
        return false;
    }
    if (TheCamera.m_bWideScreenOn) {
        return false;
    }
    return !ms_IsScannerDisabled;
}

// 0x4E6ED0
void CAEPoliceScannerAudioEntity::PlayPoliceScannerDialogue(tScannerSlot* first, tScannerSlot* second) {
    assert(first && second);
    if (!CanWePlayNewScannerDialogue()) {
        return;
    }
    PopulateScannerDialogueLists(first, second);
    ms_PSControlling  = this;
    ms_SectionPlaying = 0;
    ms_CurrentSlots   = ms_ScannerSlotFirst.data();
    PrepSlots();
    ms_ScannerPlaybackState = STATE_LOADING;
    ms_PlaybackStartTime    = CTimer::GetTimeInMS() + PLAYBACK_START_DELAY_MS;
    ms_AbortPlaybackTime    = CTimer::GetTimeInMS() + ABORT_PLAYBACK_DELAY_MS;
}

// 0x4E7590
void CAEPoliceScannerAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (curPlayPos == -1) {
        ms_Sound = nullptr;
        if (ms_IsStoppingScanner) {
            if (ms_ScannerPlaybackState != STATE_IDLE) {
                ms_IsStoppingScanner = true;
                FinishedPlayingScannerDialogue();
            }
            return;
        }
        PlayLoadedDialogue();
        return;
    }

    if (ms_IsStoppingScanner) {
        sound->SetVolume(sound->GetVolume() - STOPPING_VOLUME_STEP);
        return;
    }

    // Start the next sound a bit before this one ends (but not after the area: the last sound of a section)
    if (sound->GetSoundLength() > 0 && curPlayPos > sound->GetSoundLength() - NEXT_SOUND_OVERLAP_MS && sound->m_BankSlot != SND_BANK_SLOT_SCANNER_AREA) {
        sound->SetFlags(SOUND_REQUEST_UPDATES, false);
        ms_Sound = nullptr;
        PlayLoadedDialogue();
    }
}

// 0x4E7630
void CAEPoliceScannerAudioEntity::Service() {
    if (TheCamera.m_bWideScreenOn && ms_ScannerPlaybackState != STATE_IDLE) {
        ms_IsStoppingScanner = true;
    }

    switch (ms_ScannerPlaybackState) {
    case STATE_LOADING: { // 0x4E7667
        if (CTimer::GetTimeInMS() > ms_AbortPlaybackTime || ms_IsStoppingScanner) {
            FinishedPlayingScannerDialogue();
            return;
        }
        LoadSlots();
        return;
    }
    case STATE_LOADED: { // 0x4E7685
        if (ms_IsStoppingScanner) {
            FinishedPlayingScannerDialogue();
            return;
        }
        if (CTimer::GetTimeInMS() >= ms_PlaybackStartTime) {
            ms_VolumeOffset = CAEVehicleAudioEntity::s_pPlayerAttachedForRadio ? 0.0f : NO_RADIO_VOLUME_OFFSET;
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SCANNER_CLICK, ms_ClickVolumeOffset + ms_VolumeOffset);
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_SCANNER_NOISE_START, NOISE_VOLUME_OFFSET + ms_VolumeOffset);
            ms_PlaybackStartTime    = CTimer::GetTimeInMS() + NOISE_TO_DIALOGUE_DELAY_MS;
            ms_ScannerPlaybackState = STATE_STARTING;
        }
        return;
    }
    case STATE_STARTING: { // 0x4E771B
        if (ms_IsStoppingScanner) {
            FinishedPlayingScannerDialogue();
            return;
        }
        if (CTimer::GetTimeInMS() >= ms_PlaybackStartTime && ms_PSControlling) {
            ms_ScannerPlaybackState = STATE_PLAYING;
            ms_PSControlling->PlayLoadedDialogue();
        }
        return;
    }
    case STATE_IDLE:
    case STATE_PLAYING:
        return;
    default:
        NOTSA_UNREACHABLE();
    }
}

void CAEPoliceScannerAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEPoliceScannerAudioEntity, 0x85F368, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Constructor, 0x56DA00);
    RH_ScopedInstall(Destructor, 0x4E6E00);
    RH_ScopedInstall(StaticInitialise, 0x5B9C30);
    RH_ScopedInstall(Reset, 0x4E6E90);
    RH_ScopedInstall(AddAudioEvent, 0x4E71E0);
    RH_ScopedInstall(PrepSlots, 0x4E6BC0);
    RH_ScopedInstall(LoadSlots, 0x4E6CD0);
    RH_ScopedInstall(EnableScanner, 0x4E6DB0);
    RH_ScopedInstall(DisableScanner, 0x4E71B0);
    RH_ScopedInstall(StopScanner, 0x4E6DC0);
    RH_ScopedInstall(FinishedPlayingScannerDialogue, 0x4E6C30);
    RH_ScopedInstall(PlayLoadedDialogue, 0x4E6F60);
    RH_ScopedInstall(PopulateScannerDialogueLists, 0x4E6B60);
    RH_ScopedInstall(CanWePlayNewScannerDialogue, 0x4E6C00);
    RH_ScopedInstall(PlayPoliceScannerDialogue, 0x4E6ED0);
    RH_ScopedVMTInstall(UpdateParameters, 0x4E7590);
    RH_ScopedInstall(Service, 0x4E7630);
}

CAEPoliceScannerAudioEntity* CAEPoliceScannerAudioEntity::Constructor() {
    this->CAEPoliceScannerAudioEntity::CAEPoliceScannerAudioEntity();
    return this;
}

CAEPoliceScannerAudioEntity* CAEPoliceScannerAudioEntity::Destructor() {
    this->CAEPoliceScannerAudioEntity::~CAEPoliceScannerAudioEntity();
    return this;
}
