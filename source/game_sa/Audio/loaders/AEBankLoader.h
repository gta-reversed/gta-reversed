#pragma once

enum class eSoundRequestStatus : uint32 {
    INACTIVE,
    REQUESTED,
    PENDING_READ,
    PENDING_LOAD_ONE_SOUND
};

struct CAEBankLookupItem {
    uint8  PakFileNo;
    uint32 FileOffset; //!< Offset (#Sectors)
    uint32 NumBytes;
};

VALIDATE_SIZE(CAEBankLookupItem, 0xC);

struct CAEBankSlotItem {
    uint32 BankOffset; // Bytes?
    uint32 LoopStartOffset; // Bytes?
    uint16 SampleFrequency;
    int16  Headroom;
};
VALIDATE_SIZE(CAEBankSlotItem, 0xC);

constexpr auto AE_BANK_MAX_NUM_SOUNDS = 400;
using AEBankSlotItems = std::array<CAEBankSlotItem, AE_BANK_MAX_NUM_SOUNDS>;

struct CAEBankSlot {
    uint32          Offset;
    uint32          NumBytes;
    uint32          Unused1;
    uint32          Unused2;
    eSoundBank      Bank;
    int16           NumSounds; //!< Number of sounds in this bank [`-1` if only 1 sound]
    AEBankSlotItems Sounds;

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
    char   BaseFilename[12];
    uint32 FileCopyLSNs[10];
};
VALIDATE_SIZE(tPakLookup, 0x34);

// NOTSA
#pragma warning(push)
#pragma warning(disable : 4200) // nonstandard extension used: zero-sized array in struct/union
struct AEAudioStream {
    int16           NumSounds; // maybe i32?
    int16           __pad;
    AEBankSlotItems Sounds;
    uint8           BankData[]; // uint16 samples?
};
#pragma warning(pop)
VALIDATE_SIZE(AEAudioStream, 0x12C4 /* + samples*/);

class CAESoundRequest {
public:
    CAEBankSlot*        SlotInfo{};                              //!< Slot's info (Same as `&m_BankSlots[Slot]`)
    uint32              BankOffset{};                            //!< Offset (#Sectors) (From lookup)
    uint32              BankNumBytes{};                          //!< Size of bank [bytes] (From lookup)
    AEAudioStream*      StreamDataPtr{};                         //!< Sector aligned pointer into the buffer
    void*               StreamBufPtr{};                          //!< Buffer (Allocated using `CMemoryMgr::Malloc`)
    eSoundRequestStatus Status{ eSoundRequestStatus::INACTIVE }; //!< Current load status
    eSoundBank          Bank{ -1 };                              //!< Bank to load
    eSoundBankSlot      Slot{ -1 };                              //!< Slot to load the bank into
    int16               SoundID{ -1 };                           //!< Requested sound in the bank. If `-1` the whole bank should be loaded
    uint8               PakFileNo{};                             //!< PakFileNo (From lookup)

public:
    void Reset() {
        Bank = SND_BANK_UNK;
        Slot = SND_BANK_SLOT_UNK;
        SoundID = -1;
        SlotInfo = nullptr;
        Status = eSoundRequestStatus::INACTIVE;
    }
};
VALIDATE_SIZE(CAESoundRequest, 0x20);

class CAEBankLoader {
public:
    CAEBankSlot*       m_BankSlots;
    CAEBankLookupItem* m_BankLkups;
    tPakLookup*        m_PakLkups;
    uint16             m_BankSlotCnt;
    uint16             m_BankLkupCnt;
    int16              m_PakLkupCount;
    uint16             __pad1;
    bool               m_IsInitialised;
    uint32             m_BufferSize;
    uint8*             m_Buffer;
    int32*             m_StreamHandles;
    CAESoundRequest    m_Requests[50];
    uint16             m_NextPendingRequestIdx;
    uint16             m_RequestCnt;
    uint16             m_NextRequestIdx;
    uint16             m_StreamingChannel;
    uint16             m_BankSlotSound[60];

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
