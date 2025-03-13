#pragma once

enum class eSoundRequestStatus : uint32 {
    INACTIVE,
    REQUESTED,
    PENDING_READ,
    PENDING_LOAD_ONE_SOUND
};

struct AEBankLookup {
    uint8  PakFileNo;
    uint32 FileOffset; //!< Offset (#Sectors)
    uint32 NumBytes;
};

VALIDATE_SIZE(AEBankLookup, 0xC);

struct CAEBankSlotItem {
    uint32 BankOffsetBytes;
    uint32 LoopStartOffset;
    uint16 SampleFrequency;
    int16  Headroom;
};
VALIDATE_SIZE(CAEBankSlotItem, 0xC);

constexpr auto AE_BANK_MAX_NUM_SOUNDS = 400;
using AEBankSlotItems = std::array<CAEBankSlotItem, AE_BANK_MAX_NUM_SOUNDS>;

struct CAEBankSlot {
    uint32          OffsetBytes{}; //!< Slot offset in the buffer
    uint32          NumBytes{};    //!< Slot size in the buffer
    uint32          Unused1{};     //!< Unused on PC
    uint32          Unused2{};     //!< Unused on PC
    eSoundBank      Bank{};        //!< Bank that's loaded into this slot
    int16           NumSounds{};   //!< Number of sounds in this bank [`-1` if only 1 sound]
    AEBankSlotItems Sounds{};      //!< Sounds of this bank (Also see `NumSounds`)

    bool IsSingleSound() const { return NumSounds == -1; }

    // Calculate size of the sound at the given index
    size_t GetSoundSize(eSoundID id) const {
        assert(IsSingleSound() || id >= 0 && id < (eSoundID)(NumSounds));
        const auto next = IsSingleSound()
            ? NumBytes
            : Sounds[id + 1].BankOffsetBytes;
        return next - Sounds[id].BankOffsetBytes;
    }
};
VALIDATE_SIZE(CAEBankSlot, 0x12D4);

using AEPakID = int16;

struct AEPakLookup {
    char   BaseFilename[12];
    uint32 FileCopyLSNs[10];
};
VALIDATE_SIZE(AEPakLookup, 0x34);

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
    uint32              BankOffsetBytes{};                       //!< Offset (#Sectors) (From lookup)
    uint32              BankNumBytes{};                          //!< Size of bank [bytes] (From lookup)
    AEAudioStream*      StreamDataPtr{};                         //!< Sector (STREAMING_SECTOR_SIZE) aligned pointer into the buffer
    void*               StreamBufPtr{};                          //!< Buffer (Allocated using `CMemoryMgr::Malloc`)
    eSoundRequestStatus Status{ eSoundRequestStatus::INACTIVE }; //!< Current load status
    eSoundBank          Bank{ -1 };                              //!< Bank to load
    eSoundBankSlot      Slot{ -1 };                              //!< Slot to load the bank into
    int16               SoundID{ -1 };                           //!< Requested sound in the bank. If `-1` the whole bank should be loaded
    uint8               PakFileNo{};                             //!< PakFileNo (From lookup)
};
VALIDATE_SIZE(CAESoundRequest, 0x20);

class CAEBankLoader {
public:
    static void InjectHooks();

    CAEBankLoader() = default;
    ~CAEBankLoader();

    bool LoadBankLookupFile();
    bool LoadBankSlotFile();
    bool LoadSFXPakLookupFile();

    void CalculateBankSlotsInfosOffsets();

    auto GetBankLookups() const noexcept { assert(m_IsInitialised); return std::span{ m_BankLkups, (size_t)(m_BankLkupCnt) }; }
    AEBankLookup& GetBankLookup(uint16 bankId) const;

    auto GetPakLookups() const noexcept { assert(m_IsInitialised); return std::span{ m_PakLkups, (size_t)(m_PakLkupCount) }; }
    AEPakLookup& GetPakLookup(AEPakID pak) const;

    auto GetBankSlots() const noexcept { assert(m_IsInitialised); return std::span{ m_BankSlots, (size_t)(m_BankSlotCnt) }; }
    CAEBankSlot& GetBankSlot(eSoundBankSlot slot) const;

private:
    // NOTSA
    void Deconstructor() { this->~CAEBankLoader(); }

protected:
    CAEBankSlot*                    m_BankSlots{};
    AEBankLookup*                   m_BankLkups{};
    AEPakLookup*                    m_PakLkups{};
    uint16                          m_BankSlotCnt{};
    uint16                          m_BankLkupCnt{};
    uint16                          m_PakLkupCount{};
    uint16                          __pad1{};
    bool                            m_IsInitialised{};
    uint32                          m_BufferSize{};
    uint8*                          m_Buffer{};
    int32*                          m_StreamHandles{};
    std::array<CAESoundRequest, 50> m_Requests{};
    uint16                          m_NextOneSoundReqIdx{};
    uint16                          m_RequestCnt{};
    uint16                          m_NextRequestIdx{};
    uint16                          m_StreamingChannel{ 4 };
    std::array<eSoundID, 60>        m_BankSlotSound; //!< Sound ID if a bank is used for a single sound only, otherwise (when a whole bank is loaded) `-1`
};
VALIDATE_SIZE(CAEBankLoader, 0x6E4);
