#pragma once

enum class eSoundRequestStatus : uint32 {
    UNK_0,
    JUST_LOADED,
    ALREADY_LOADED,
    UNK_3
};

struct CAEBankLookupItem {
    uint8 PakFile;
    uint32 FileOffset;
    uint32 NumBytes;
};
VALIDATE_SIZE(CAEBankLookupItem, 0xC);

struct CAEBankSlotItem {
    uint32 BankOffset;
    uint32 LoopStartOffset;
    uint16 Frequency; // Or sample rate?
    int16  Headroom;
};
VALIDATE_SIZE(CAEBankSlotItem, 0xC);

constexpr auto NUM_BANK_SLOT_ITEMS = 400u;

struct CAEBankSlot {
    uint32          Offset{};
    uint32          NumBytes{};
    uint32          Unk1{};
    uint32          Unk2{};
    int16           Bank{};
    int16           NumSounds{}; // -1: Single sound.
    CAEBankSlotItem Sounds[NUM_BANK_SLOT_ITEMS];

    bool IsSingleSound() const { return NumSounds == -1; }

    // Calculate size of the slot item by <next item's offset> - <item's offset>
    size_t CalculateSizeOfSlotItem(size_t index) const {
        assert(NumSounds == -1 || index < (size_t)NumSounds);
        const auto next = IsSingleSound()
            ? NumBytes
            : Sounds[index + 1].BankOffset;
        return next - Sounds[index].BankOffset;
    }
};
VALIDATE_SIZE(CAEBankSlot, 0x12D4);

struct tPakLookup {
    char   BaseFilename[12]{};
    uint32 FileCopyLSNs[10]{};
};
VALIDATE_SIZE(tPakLookup, 0x34);

// NOTSA
#pragma warning(push)
#pragma warning(disable : 4200) // nonstandard extension used: zero-sized array in struct/union
struct CdAudioStream {
    int16 m_nSoundCount;
    int16 __pad;
    CAEBankSlotItem m_aSlotItems[400];
    uint8 m_aBankData[]; // uint16 samples?
};
#pragma warning(pop)
VALIDATE_SIZE(CdAudioStream, 0x12C4 /* + samples*/);

class CAESoundRequest {
public:
    CAEBankSlot* m_pBankSlotInfo{};
    uint32 m_nBankOffset{};
    uint32 m_nBankSize{};
    CdAudioStream* m_pStreamOffset{};
    CdAudioStream* m_pStreamBuffer{};
    eSoundRequestStatus m_nStatus{eSoundRequestStatus::UNK_0};
    int16 m_nBankId{-1};
    int16 m_nBankSlotId{-1};
    int16 m_nNumSounds{-1};
    uint8 m_nPakFileNumber{};

public:
    CAESoundRequest() = default;

    CAESoundRequest(int16 bankId, int16 bankSlot, int16 numSounds, CAEBankSlot& slot,
                    CAEBankLookupItem* lookup, eSoundRequestStatus status)
        : m_nBankId(bankId), m_nBankSlotId(bankSlot), m_nNumSounds(numSounds), m_pBankSlotInfo(&slot)
        , m_nPakFileNumber(lookup->PakFile), m_nBankOffset(lookup->FileOffset), m_nBankSize(lookup->NumBytes)
        , m_nStatus(status)
    {}

    void Reset() {
        m_nBankId = m_nBankSlotId = m_nNumSounds = -1;
        m_pBankSlotInfo = nullptr;
        m_nStatus = eSoundRequestStatus::UNK_0;
    }

    bool IsSingleSound() const {
        return m_nNumSounds == -1;
    }
};
VALIDATE_SIZE(CAESoundRequest, 0x20);

class CAEBankLoader {
public:
    CAEBankSlot* m_paBankSlots;
    CAEBankLookupItem* m_paBankLookups;
    tPakLookup* m_paPakLookups;
    uint16 m_nBankSlotCount;
    uint16 m_nBankLookupCount;
    int16 m_nPakLookupCount;
    uint16 __pad;
    bool m_bInitialised;
    uint32 m_nBufferSize;
    uint8* m_pBuffer;
    int32* m_paStreamHandles;
    CAESoundRequest m_aRequests[50];
    uint16 field_664;
    uint16 m_iRequestCount;
    uint16 m_iNextRequest;
    uint16 m_iStreamingChannel;
    uint16 m_aBankSlotSound[60];

public:
    static void InjectHooks();

    CAEBankLoader() = default;
    ~CAEBankLoader();

    CAEBankLookupItem* GetBankLookup(uint16 bankId);
    bool LoadBankLookupFile();
    bool LoadBankSlotFile();
    bool LoadSFXPakLookupFile();

    void CalculateBankSlotsInfosOffsets();

private:
    // NOTSA
    void Deconstructor() { this->~CAEBankLoader(); }
};
VALIDATE_SIZE(CAEBankLoader, 0x6E4);
