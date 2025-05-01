#pragma once
#include "AEBankLoader.h"

struct tVirtualChannelSettings {
    eSoundBankSlot BankSlotIDs[300]{};
    eSoundID       SoundIDs[300]{};

    tVirtualChannelSettings() {
        rng::fill(BankSlotIDs, SND_BANK_SLOT_UNK);
        rng::fill(SoundIDs, -1);
    }
};
VALIDATE_SIZE(tVirtualChannelSettings, 0x4B0);

class CAEMP3BankLoader : public CAEBankLoader {
public:
    static void InjectHooks();

    CAEMP3BankLoader() = default;

    bool   Initialise();
    uint8* GetBankSlotBuffer(eSoundBankSlot bankSlot, uint32& outLength);
    float  GetSoundHeadroom(eSoundID soundId, eSoundBankSlot bankSlot);
    bool   IsSoundBankLoaded(eSoundBank bankId, eSoundBankSlot bankSlot);
    bool   GetSoundBankLoadingStatus(eSoundBank bankId, eSoundBankSlot bankSlot);
    uint8* GetSoundBuffer(eSoundID soundId, eSoundBankSlot bankSlot, uint32& outIndex, uint16& outSampleRate);
    int32  GetLoopOffset(eSoundID soundId, eSoundBankSlot bankSlot);
    bool   IsSoundLoaded(eSoundBank bankId, eSoundID soundId, eSoundBankSlot bankSlot) const;
    bool   GetSoundLoadingStatus(eSoundBank bankId, eSoundID soundId, eSoundBankSlot bankSlot);
    void   UpdateVirtualChannels(tVirtualChannelSettings* settings, int16* lengths, int16* loopStartTimes);
    void   LoadSoundBank(eSoundBank bankId, eSoundBankSlot bankSlot);
    void   LoadSound(eSoundBank bankId, eSoundID soundId, eSoundBankSlot bankSlot);
    void   Service();

private:
    void AddRequest(eSoundBank bank, eSoundBankSlot slot, std::optional<eSoundID> sound);

private:
    // NOTSA
    CAEMP3BankLoader* Constructor() {
        this->CAEMP3BankLoader::CAEMP3BankLoader();
        return this;
    }
};
VALIDATE_SIZE(CAEMP3BankLoader, 0x6E4);
