#pragma once

enum class eSoundRequestStatus : uint32 {
    UNK_0,
    JUST_LOADED,
    ALREADY_LOADED,
    UNK_3
};

struct CAEBankLookupItem {
    uint8 m_nPakFileNumber;
    uint32 m_nOffset;
    uint32 m_nSize;
};
VALIDATE_SIZE(CAEBankLookupItem, 0xC);

struct CAEBankSlotItem {
    uint32 m_nOffset;
    uint32 m_nLoopOffset;
    uint16 m_usSampleRate;
    int16  m_usSoundHeadroom;
};
VALIDATE_SIZE(CAEBankSlotItem, 0xC);

constexpr auto NUM_BANK_SLOT_ITEMS = 400u;

struct CAEBankSlot {
    uint32 m_nOffset;
    uint32 m_nSize;
    uint32 field_8;
    uint32 field_C;
    int16 m_nBankId;
    int16 m_nSoundCount; // -1: Single sound.
    CAEBankSlotItem m_aSlotItems[NUM_BANK_SLOT_ITEMS];

    bool IsSingleSound() const { return m_nSoundCount == -1; }

    // Calculate size of the slot item by <next item's offset> - <item's offset>
    size_t CalculateSizeOfSlotItem(size_t index) const {
        assert(m_nSoundCount == -1 || index < (size_t)m_nSoundCount);
        const auto nextOffset = [&] {
            if (index == m_nSoundCount - 1) {
                return m_nSize;
            } else {
                return m_aSlotItems[index + 1].m_nOffset;
            }
        }();

        return nextOffset - m_aSlotItems[index].m_nOffset;
    }
};
VALIDATE_SIZE(CAEBankSlot, 0x12D4);

struct tPakLookup {
    char m_szName[12];
    uint32 field_0C[10];
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
        , m_nPakFileNumber(lookup->m_nPakFileNumber), m_nBankOffset(lookup->m_nOffset), m_nBankSize(lookup->m_nSize)
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
