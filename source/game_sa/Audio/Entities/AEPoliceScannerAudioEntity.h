#pragma once

#include "AEAudioEntity.h"

struct tScannerSlot {
    eSoundBank Bank{ SND_BANK_UNK };
    eSoundID   SoundID{ -1 };

    //! Nothing to play in this slot
    [[nodiscard]] bool IsEmpty() const {
        return Bank < 0 || SoundID < 0;
    }
};
VALIDATE_SIZE(tScannerSlot, 0x4);

#define NUM_POLICE_SCANNER_SLOTS 5

class NOTSA_EXPORT_VTABLE CAEPoliceScannerAudioEntity : public CAEAudioEntity {
public:
    enum State : int16 {
        STATE_IDLE     = 0, //!< No dialogue
        STATE_LOADING  = 2, //!< Loading the sounds of the current section
        STATE_LOADED   = 4, //!< Sounds loaded, waiting for the start time
        STATE_STARTING = 5, //!< Click + noise started, waiting for the start time
        STATE_PLAYING  = 7, //!< Playing the sounds of the current section
    };

    enum SlotState : int16 {
        SLOT_STATE_TO_LOAD = 0,
        SLOT_STATE_EMPTY   = 1, //!< Nothing to play
        SLOT_STATE_LOADING = 2,
        SLOT_STATE_LOADED  = 3,
        SLOT_STATE_PLAYED  = 5, //!< The sound was requested
    };

    static inline auto& ms_VolumeOffset               = StaticRef<float>(0xB61CF8);
    static inline auto& ms_IsStoppingScanner          = StaticRef<bool>(0xB61CFC);
    static inline auto& ms_Sound                      = StaticRef<CAESound*>(0xB61D00);
    static inline auto& ms_SectionPlaying             = StaticRef<int16>(0xB61D04); //!< 0 - first, 1 - second dialogue list
    static inline auto& ms_AbortPlaybackTime          = StaticRef<uint32>(0xB61D08);
    static inline auto& ms_PlaybackStartTime          = StaticRef<uint32>(0xB61D0C);
    static inline auto& ms_CurrentSlots               = StaticRef<tScannerSlot*>(0xB61D10);
    static inline auto& ms_SlotState                  = StaticRef<std::array<SlotState, NUM_POLICE_SCANNER_SLOTS>>(0xB61D14);
    static inline auto& ms_ScannerSlotSecond          = StaticRef<std::array<tScannerSlot, NUM_POLICE_SCANNER_SLOTS>>(0xB61D20);
    static inline auto& ms_ScannerSlotFirst           = StaticRef<std::array<tScannerSlot, NUM_POLICE_SCANNER_SLOTS>>(0xB61D34);
    static inline auto& ms_PSControlling              = StaticRef<CAEPoliceScannerAudioEntity*>(0xB61D48);
    static inline auto& ms_ScannerPlaybackState       = StaticRef<State>(0xB61D4C);
    static inline auto& ms_IsScannerDisabled          = StaticRef<bool>(0xB61D4E);
    static inline auto& ms_NextNewScannerDialogueTime = StaticRef<uint32>(0xB61D50);
    static inline auto& ms_ClickVolumeOffset          = StaticRef<float>(0xB61D54); //!< Never written, always 0

public:
    CAEPoliceScannerAudioEntity() = default; // 0x56DA00
    ~CAEPoliceScannerAudioEntity();

    static void StaticInitialise();
    static void Reset();

    void AddAudioEvent(eAudioEvents event, eCrimeType crimeType, const CVector& point);

    static void PrepSlots();
    static void LoadSlots();

    static void EnableScanner();
    static void DisableScanner(bool stopScanner, bool stopSound);
    static void StopScanner(bool stopSound);

    static void FinishedPlayingScannerDialogue();
    void PlayLoadedDialogue();

    static void PopulateScannerDialogueLists(const tScannerSlot* first, const tScannerSlot* second);
    static bool CanWePlayNewScannerDialogue();
    void PlayPoliceScannerDialogue(tScannerSlot* first, tScannerSlot* second);

    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;

    static void Service();

    static void InjectHooks();

private:
    CAEPoliceScannerAudioEntity* Constructor();
    CAEPoliceScannerAudioEntity* Destructor();
};
VALIDATE_SIZE(CAEPoliceScannerAudioEntity, 0x7C);
