#pragma once

#include "AEAudioEntity.h"

struct tScannerSlot {
    eSoundBank Bank{SND_BANK_UNK};
    eSoundID   SoundID{-1};

    // bad name?
    [[nodiscard]] bool IsActive() const {
        return Bank < 0 || SoundID < 0;
    }
};

#define NUM_POLICE_SCANNER_SLOTS 5

class NOTSA_EXPORT_VTABLE CAEPoliceScannerAudioEntity : public CAEAudioEntity {
public:
    enum State : int32 {
        STATE_INITIAL = 0,
        ONE           = 1,
        TWO           = 2,
        THREE         = 3,
        FOUR          = 4,
        FIVE          = 5,
        SIX           = 6,
        SEVEN         = 7,
    };

    static inline auto& s_fVolumeOffset = StaticRef<float>(0xB61CF8);
    static inline auto& s_bStoppingScanner = StaticRef<bool>(0xB61CFC);
    static inline auto& s_pSound = StaticRef<CAESound*>(0xB61D00);
    static inline auto& s_nAbortPlaybackTime = StaticRef<uint32>(0xB61D08);
    static inline auto& s_nPlaybackStartTime = StaticRef<uint32>(0xB61D0C);
    static inline auto& s_nSectionPlaying = StaticRef<int16>(0xB61D04);

    static inline auto (&s_SlotState)[NUM_POLICE_SCANNER_SLOTS] = StaticRef<int16[NUM_POLICE_SCANNER_SLOTS]>(0xB61D14);
    static inline auto& s_pCurrentSlots = StaticRef<tScannerSlot*>(0xB61D10);
    static inline auto (&s_ScannerSlotFirst)[NUM_POLICE_SCANNER_SLOTS] = StaticRef<tScannerSlot[NUM_POLICE_SCANNER_SLOTS]>(0xB61D34);
    static inline auto (&s_ScannerSlotSecond)[NUM_POLICE_SCANNER_SLOTS] = StaticRef<tScannerSlot[NUM_POLICE_SCANNER_SLOTS]>(0xB61D20);

    static inline auto& s_pPSControlling = StaticRef<CAEPoliceScannerAudioEntity*>(0xB61D48);
    static inline auto& s_nScannerPlaybackState = StaticRef<CAEPoliceScannerAudioEntity::State>(0xB61D4C);
    static inline auto& s_bScannerDisabled = StaticRef<bool>(0xB61D4E);
    static inline auto& s_NextNewScannerDialogueTime = StaticRef<uint32>(0xB61D50);

public:
    CAEPoliceScannerAudioEntity() = default; // 0x56DA00
    ~CAEPoliceScannerAudioEntity();

    static void StaticInitialise();
    static void Reset();

    void AddAudioEvent(eAudioEvents event, eCrimeType crimeType, const CVector& point);

    static void PrepSlots();
    static void LoadSlots();

    static void EnableScanner();
    static void DisableScanner(bool a1, bool bStopSound);
    static void StopScanner(bool bStopSound);

    static void FinishedPlayingScannerDialogue();
    void PlayLoadedDialogue();

    static void PopulateScannerDialogueLists(const tScannerSlot* first, const tScannerSlot* second);
    static bool CanWePlayNewScannerDialogue();
    void PlayPoliceScannerDialogue(tScannerSlot* first, tScannerSlot* second);

    void UpdateParameters(CAESound* sound, int16 curPlayPos) override;

    static void Service();

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CAEPoliceScannerAudioEntity* Constructor();
    CAEPoliceScannerAudioEntity* Destructor();

};

VALIDATE_SIZE(CAEPoliceScannerAudioEntity, 0x7C);
