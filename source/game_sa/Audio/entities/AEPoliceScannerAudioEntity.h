#pragma once

#include "AEAudioEntity.h"

struct tScannerSlot {
    eSoundBank Bank{SND_BANK_UNK};
    eSoundID   SoundID{-1};

    tScannerSlot() {
        Clear();
    }

    void Clear() {
        *this = tScannerSlot{};
    }

    // bad name?
    [[nodiscard]] bool IsInUse() const {
        return Bank < 0 || SoundID < 0;
    }
};

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

    static float&     s_fVolumeOffset;
    static bool&      s_bStoppingScanner;
    static CAESound*& s_pSound;
    static uint32&    s_nAbortPlaybackTime;
    static uint32&    s_nPlaybackStartTime;
    static int16&     s_nSectionPlaying;

    static int16         (&s_SlotState)[5];
    static tScannerSlot* (&s_pCurrentSlots);
    static tScannerSlot  (&s_ScannerSlotFirst)[5];
    static tScannerSlot  (&s_ScannerSlotSecond)[5];

    static CAEPoliceScannerAudioEntity* s_pPSControlling;
    static State&                       s_nScannerPlaybackState;
    static bool&                        s_bScannerDisabled;
    static uint32&                      s_NextNewScannerDialogueTime;

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
