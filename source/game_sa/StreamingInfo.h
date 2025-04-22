/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <Base.h>
#include <CdStreamInfo.h>

//! Index into CStreaming::ms_files
using StreamingImgID = uint8;

enum eStreamingFlags {
    STREAMING_DEFAULT = 0x0,
    STREAMING_UNKNOWN_1 = 0x1,
    STREAMING_GAME_REQUIRED = 0x2,
    STREAMING_MISSION_REQUIRED = 0x4,
    STREAMING_KEEP_IN_MEMORY = 0x8,
    STREAMING_PRIORITY_REQUEST = 0x10,
    STREAMING_LOADING_SCENE = 0x20,
    STREAMING_DONTREMOVE_IN_LOADSCENE = STREAMING_LOADING_SCENE | STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY | STREAMING_MISSION_REQUIRED | STREAMING_GAME_REQUIRED,
};

enum eStreamingLoadState : uint8 {
    // Model isn't loaded
    LOADSTATE_NOT_LOADED = 0,

    // Model is loaded
    LOADSTATE_LOADED = 1,

    // Model in request list, but not yet in loading channel (TODO: Verify this)
    LOADSTATE_REQUESTED = 2,

    // Model is being read
    LOADSTATE_READING = 3,

    // If the model is a `big` one this state is used to indicate
    // that the model's first half has been loaded and is yet to be
    // finished by loading the second half.
    // When it has been loaded the state is set to `LOADED`
    LOADSTATE_FINISHING = 4
};

constexpr auto STREAMING_SECTOR_SIZE = 2048u; // OR `<< 11`

class CStreamingInfo {
public:
    static CStreamingInfo*& ms_pArrayBase;

public:
    static void InjectHooks();

    void Init();
    [[nodiscard]] CdStreamPos GetCdPosn() const;
    void SetCdPosnAndSize(uint32 offset, size_t CdSize);
    bool GetCdPosnAndSize(CdStreamPos& CdPosn, size_t& CdSize);
    [[nodiscard]] bool HasCdPosnAndSize() const noexcept { return m_CDSize != 0; }
    [[nodiscard]] auto GetCdSize() const { return m_CDSize; }
    CStreamingInfo* GetNext() { return m_NextIndex == -1 ? nullptr : &ms_pArrayBase[m_NextIndex]; }
    CStreamingInfo* GetPrev() { return m_PrevIndex == -1 ? nullptr : &ms_pArrayBase[m_PrevIndex]; }
    void SetImg(StreamingImgID imgID) { m_ImgID = imgID; }

    /*!
    * @addr 0x407480
    * @brief Insert `*this` after the item `*after`
    */
    void AddToList(CStreamingInfo* after);

    /*!
    * @addr 0x4074E0
    * @brief Remove `*this` from it's current list
    */
    void RemoveFromList();

    /*!
    * @notsa
    * @brief Check if `*this` is in any list
    */
    bool InList() const;

    void SetFlags(uint32 flags) { m_Flags |= flags; }
    void ClearFlags(uint32 flags) { m_Flags &= ~flags; }
    [[nodiscard]] auto GetFlags() const noexcept { return m_Flags; }
    void ClearAllFlags() noexcept { m_Flags = 0; } // Clears all flags
    [[nodiscard]] bool AreAnyFlagsSetOutOf(uint32 flags) const noexcept { return GetFlags() & flags; }

    [[nodiscard]] bool IsLoadedOrBeingRead() const noexcept {
        switch (m_LoadState) {
        case eStreamingLoadState::LOADSTATE_LOADED:
        case eStreamingLoadState::LOADSTATE_READING:
            return true;
        default:
            return false;
        }
    }
    [[nodiscard]] bool IsLoaded() const           { return m_LoadState == eStreamingLoadState::LOADSTATE_LOADED; }
    [[nodiscard]] bool IsRequested() const        { return m_LoadState == eStreamingLoadState::LOADSTATE_REQUESTED; }
    [[nodiscard]] bool IsBeingRead() const        { return m_LoadState == eStreamingLoadState::LOADSTATE_READING; }
    [[nodiscard]] bool IsLoadingFinishing() const { return m_LoadState == eStreamingLoadState::LOADSTATE_FINISHING; }

    [[nodiscard]] bool DontRemoveInLoadScene() const noexcept   { return m_Flags & eStreamingFlags::STREAMING_DONTREMOVE_IN_LOADSCENE; }
    [[nodiscard]] bool IsGameRequired() const noexcept          { return m_Flags & eStreamingFlags::STREAMING_GAME_REQUIRED; }
    [[nodiscard]] bool IsMissionRequired() const noexcept       { return m_Flags & eStreamingFlags::STREAMING_MISSION_REQUIRED; }
    [[nodiscard]] bool DoKeepInMemory() const noexcept          { return m_Flags & eStreamingFlags::STREAMING_KEEP_IN_MEMORY; }
    [[nodiscard]] bool IsPriorityRequest() const noexcept       { return m_Flags & eStreamingFlags::STREAMING_PRIORITY_REQUEST; }
    [[nodiscard]] bool IsLoadingScene() const noexcept          { return m_Flags & eStreamingFlags::STREAMING_LOADING_SCENE; }
    [[nodiscard]] bool IsMissionOrGameRequired() const noexcept { return IsMissionRequired() || IsGameRequired(); }
    [[nodiscard]] bool IsRequiredToBeKept() const noexcept      { return IsMissionOrGameRequired() || DoKeepInMemory(); } // GameRequired || MissionRequired || m_KeepInMemory

public:
    int16 m_NextIndex{-1};     // ms_pArrayBase array index
    int16 m_PrevIndex{-1};     // ms_pArrayBase array index
    int16 m_NextIndexOnCd{-1}; // ModelId after this file in the containing image file
    union {
        uint8 m_Flags; // see eStreamingFlags
        struct {
            uint8 bUnkn0x1 : 1;
            uint8 m_IsGameRequired : 1;
            uint8 m_IsMissionRequired : 1;
            uint8 m_KeepInMemory : 1;
            uint8 m_IsPriorityRequest : 1;
            uint8 m_IsLoadingScene : 1;
        };
    };
    StreamingImgID      m_ImgID;    //!< Index into CStreaming::ms_files
    uint32              m_CDOffset;    //!< Offset in directory (in sectors)
    size_t              m_CDSize;   //!< Size of resource (in sectors); (m_CDSize * STREAMING_BLOCK_SIZE = actual size in bytes)
    eStreamingLoadState m_LoadState{LOADSTATE_NOT_LOADED};
};
VALIDATE_SIZE(CStreamingInfo, 0x14);
