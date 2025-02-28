#include "StdInc.h"
#include "AEMP3BankLoader.h"
#include "AEAudioUtility.h"
#include <cstdlib>

// Naming scheme:
// * BankLookup == BankId
//

void CAEMP3BankLoader::InjectHooks() {
    RH_ScopedClass(CAEMP3BankLoader);
    RH_ScopedCategory("Audio/Loaders");

    RH_ScopedInstall(Constructor, 0x4DFB10);
    RH_ScopedInstall(Initialise, 0x4E08F0);
    RH_ScopedInstall(GetBankSlot, 0x4DFDE0);
    RH_ScopedInstall(GetSoundHeadroom, 0x4E01E0);
    RH_ScopedInstall(IsSoundBankLoaded, 0x4E0220);
    RH_ScopedInstall(GetSoundBankLoadingStatus, 0x4E0250);
    RH_ScopedInstall(GetSoundBuffer, 0x4E0280);
    RH_ScopedInstall(IsSoundLoaded, 0x4E03B0);
    RH_ScopedInstall(GetSoundLoadingStatus, 0x4E0400);
    RH_ScopedInstall(UpdateVirtualChannels, 0x4E0450);
    RH_ScopedInstall(LoadSoundBank, 0x4E0670);
    RH_ScopedInstall(LoadSound, 0x4E07A0);
    RH_ScopedInstall(Service, 0x4DFE30); // TODO: broken
}

// 0x4DFB10
CAEMP3BankLoader::CAEMP3BankLoader() {
    m_IsInitialised      = false;
    m_StreamingChannel = 4;
}

// 0x4E08F0
bool CAEMP3BankLoader::Initialise() {
    rng::for_each(m_Requests, &CAESoundRequest::Reset);
    m_NextOneSoundReqIdx       = 0;
    m_RequestCnt = 0;
    m_NextRequestIdx  = 0;

    if (!LoadBankSlotFile() || !LoadBankLookupFile() || !LoadSFXPakLookupFile()) {
        return false;
    }

    m_IsInitialised = true;
    return true;
}

// 0x4DFDE0, inlined
uint8* CAEMP3BankLoader::GetBankSlot(uint16 bankSlot, uint32& outLength) {
    if (!m_IsInitialised || bankSlot >= m_BankSlotCnt) {
        return nullptr;
    }

    outLength = m_BankSlots[bankSlot].NumBytes;
    return &m_Buffer[m_BankSlots[bankSlot].Offset];
}

// 0x4E01E0
float CAEMP3BankLoader::GetSoundHeadroom(uint16 soundId, int16 bankSlot) {
    return m_IsInitialised
        ? (float)m_BankSlots[bankSlot].Sounds[soundId].Headroom / 100.0f
        : 0.f;
}

// 0x4E0220
bool CAEMP3BankLoader::IsSoundBankLoaded(uint16 bankId, int16 bankSlot) {
    assert(bankSlot < m_BankSlotCnt);
    return m_IsInitialised && m_BankSlots[bankSlot].Bank == bankId;
}

// 0x4E0250
bool CAEMP3BankLoader::GetSoundBankLoadingStatus(uint16 bankId, int16 bankSlot) {
    return IsSoundBankLoaded(bankId, bankSlot);
}

// 0x4E0280
uint8* CAEMP3BankLoader::GetSoundBuffer(uint16 soundId, int16 bankSlot, uint32& outSize, uint16& outSampleRate) {
    if (!m_IsInitialised || m_BankSlots[bankSlot].Bank == (uint16)-1) {
        return nullptr;
    }

    auto& bank = m_BankSlots[bankSlot];

    if (!bank.IsSingleSound() && soundId >= bank.NumSounds) {
        return nullptr;
    }

    if (bank.IsSingleSound() && m_BankSlotSound[bankSlot] != soundId) {
        return nullptr;
    }

    uint32 bankBufferSize{};
    auto*  bankInBuffer = GetBankSlot(bankSlot, bankBufferSize);

    if (soundId < AE_BANK_MAX_NUM_SOUNDS && bank.IsSingleSound() || soundId < bank.NumSounds - 1) {
        assert(soundId < AE_BANK_MAX_NUM_SOUNDS);
        outSize = bank.Sounds[(soundId + 1) % AE_BANK_MAX_NUM_SOUNDS].BankOffset - bank.Sounds[soundId].BankOffset;
    } else {
        assert(soundId < AE_BANK_MAX_NUM_SOUNDS);
        outSize = m_BankLkups[bank.Bank].NumBytes - bank.Sounds[soundId].BankOffset;
    }
    assert(soundId < AE_BANK_MAX_NUM_SOUNDS);
    outSampleRate = bank.Sounds[soundId].SampleFrequency;

    return &bankInBuffer[bank.Sounds[soundId].BankOffset];
}

// 0x4E0380
int32 CAEMP3BankLoader::GetLoopOffset(uint16 soundId, int16 bankSlot) {
    if (!m_IsInitialised) {
        return -1;
    }

    assert(bankSlot < m_BankSlotCnt);
    auto& bank = m_BankSlots[bankSlot];

    assert(bank.IsSingleSound() && m_BankSlotSound[bankSlot] == soundId || soundId < bank.NumSounds);
    return bank.Sounds[soundId].LoopStartOffset;
}

// 0x4E03B0
bool CAEMP3BankLoader::IsSoundLoaded(uint16 bankId, uint16 soundId, int16 bankSlot) {
    return m_IsInitialised
        && m_BankSlots[bankSlot].Bank == bankId
        && m_BankSlotSound[bankSlot] == soundId;
}

// 0x4E0400
bool CAEMP3BankLoader::GetSoundLoadingStatus(uint16 bankId, uint16 soundId, int16 bankSlot) {
    return IsSoundLoaded(bankId, soundId, bankSlot);
}

// 0x4E0450
void CAEMP3BankLoader::UpdateVirtualChannels(tVirtualChannelSettings* settings, int16* lengths, int16* loopStartTimes) {
    for (auto i = 0u; i < std::size(settings->BankSlotIDs); i++) {
        const auto  slot     = settings->BankSlotIDs[i];
        const auto& slotInfo = m_BankSlots[slot];
        const auto  sfx      = settings->SoundIDs[i];

        if (slot < 0 || sfx < 0 || slotInfo.Bank == -1 || (sfx >= slotInfo.NumSounds && slotInfo.NumSounds >= 0)) {
            lengths[i]        = -1;
            loopStartTimes[i] = -1;
        } else {
            lengths[i] = CAEAudioUtility::ConvertFromBytesToMS(
                slotInfo.CalculateSizeOfSlotItem(sfx),
                slotInfo.Sounds[sfx].SampleFrequency,
                1
            );

            const auto loopOffset = slotInfo.Sounds[sfx].LoopStartOffset;
            loopStartTimes[i] = loopOffset != -1
                ? CAEAudioUtility::ConvertFromBytesToMS(2 * loopOffset, slotInfo.Sounds[sfx].SampleFrequency, 1u) // Why `*2`?
                : -1;
        }
    }
}

// Implementation of `LoadSoundBank` and `LoadSound`
void CAEMP3BankLoader::AddRequest(eSoundBank bank, eSoundBankSlot slot, std::optional<eSoundID> sound) {
    if (!m_IsInitialised) {
        return;
    }

    // Invalid bank?
    if (bank < 0 || bank > m_BankLkupCnt) {
        NOTSA_LOG_WARN("Trying to load invalid bank ({}) into slot ({})", (int32)(bank), (int32)(slot));
        return;
    }

    // Invalid slot?
    if (slot < 0 || slot > m_BankSlotCnt) {
        NOTSA_LOG_WARN("Trying to load bank ({}) into invalid slot ({})", (int32)(bank), (int32)(slot));
        return;
    }

    // Already loaded?
    if (IsSoundBankLoaded(bank, slot)) {
        return;
    }

    // Already bank being loaded?
    for (auto& req : m_Requests) {
        if (req.Bank == bank && req.Slot == slot && req.SoundID == sound.value_or(-1)) {
            return;
        }
    }

    const auto* const lkup = GetBankLookup(bank);
    m_Requests[m_NextRequestIdx] = CAESoundRequest{
        .SlotInfo     = &m_BankSlots[slot],
        .BankOffset   = lkup->FileOffset,
        .BankNumBytes = lkup->NumBytes,
        .Status       = eSoundRequestStatus::REQUESTED,
        .Bank         = (eSoundBank)(bank),
        .Slot         = (eSoundBankSlot)(slot),
        .SoundID      = sound.value_or(-1),
        .PakFileNo    = lkup->PakFileNo
    };
    m_RequestCnt++;
    m_NextRequestIdx = (m_NextRequestIdx + 1) % std::size(m_Requests);
}


// 0x4E0670
void CAEMP3BankLoader::LoadSoundBank(uint16 bankId, int16 bankSlot) {
    AddRequest((eSoundBank)(bankId), (eSoundBankSlot)(bankSlot), std::nullopt);
}

// 0x4E07A0
void CAEMP3BankLoader::LoadSound(uint16 bankId, uint16 soundId, int16 bankSlot) {
    AddRequest((eSoundBank)(bankId), (eSoundBankSlot)(bankSlot), (eSoundID)(soundId));
}

// 0x4DFE30
void CAEMP3BankLoader::Service() {
    // TODO: Refactor the shit out of this
    for (auto&& [i, req] : notsa::enumerate(m_Requests)) {
        const auto AllocateStreamBuffer = [&](uint32 sectors) {
            const auto buf  = (std::byte*)(CMemoryMgr::Malloc(sectors * STREAMING_SECTOR_SIZE));
            req.StreamBufPtr  = (AEAudioStream*)(buf);
            req.StreamDataPtr = (AEAudioStream*)(buf + (req.BankOffset % STREAMING_SECTOR_SIZE));
        };
        switch (req.Status) {
        case eSoundRequestStatus::REQUESTED: {
            if (CdStreamGetStatus(m_StreamingChannel) != eCdStreamStatus::READING_SUCCESS) {
                continue;
            }

            // Read whole bank into the buffer
            const auto sectors = req.SoundID == -1
                ? (sizeof(AEAudioStream) + req.BankNumBytes) / STREAMING_SECTOR_SIZE + 2
                : 2 + 2;
            AllocateStreamBuffer(sectors);
            CdStreamRead( // 0x4E016F
                m_StreamingChannel,
                req.StreamBufPtr,
                { .Offset = req.BankOffset / STREAMING_SECTOR_SIZE, .FileID = CdStreamHandleToFileID(m_StreamHandles[req.PakFileNo]) },
                sectors
            );

            req.Status = eSoundRequestStatus::PENDING_READ;
            break;
        }
        case eSoundRequestStatus::PENDING_READ: {
            if (CdStreamGetStatus(m_StreamingChannel) != eCdStreamStatus::READING_SUCCESS) {
                continue;
            }

            // Copy over sound info into the bank slot
            req.SlotInfo->Sounds = req.StreamDataPtr->Sounds;

            if (req.SoundID == -1) { // 0x4DFF6E - Load whole bank?
                // Copy over data into internal buffer as the request buffer will be deallocated now
                assert(m_BufferSize >= req.BankNumBytes);
                memcpy(
                    &m_Buffer[req.SlotInfo->Offset],
                    &req.StreamDataPtr->BankData,
                    req.BankNumBytes
                );

                VERIFY(req.SlotInfo == &m_BankSlots[req.Slot]);
                req.SlotInfo->Bank        = req.Bank;
                req.SlotInfo->NumSounds   = req.StreamDataPtr->NumSounds;

                m_BankSlotSound[req.Slot] = -1; // Whole bank loaded, so use `-1`

                CMemoryMgr::Free(req.StreamBufPtr);
                m_RequestCnt--;

                req.Status = eSoundRequestStatus::INACTIVE;
            } else { // 0x4E0033 - Load specified sound only
                // Now the whole bank is loaded into the memory,
                // this is done because we need the offset of the sound we need
                // then, we read the same data again (talk about inefficiency)
                // and this time actually copy it into the bank's data buffer

                m_BankSlotSound[req.Slot] = -1;

                VERIFY(req.SlotInfo == &m_BankSlots[req.Slot]);
                req.SlotInfo->Bank      = SND_BANK_UNK;
                req.SlotInfo->NumSounds = -1;
                req.BankOffset         += req.StreamDataPtr->Sounds[req.SoundID].BankOffset + sizeof(AEAudioStream);

                // Calculate bank size
                const auto nextOrEnd = req.SoundID + 1 >= req.StreamDataPtr->NumSounds
                    ? m_BankLkups[req.Bank].NumBytes                       // If no more sounds we use the end of bank
                    : req.StreamDataPtr->Sounds[req.SoundID + 1].BankOffset; // Otherwise use next sound's offset
                req.BankNumBytes = nextOrEnd - req.StreamDataPtr->Sounds[req.SoundID].BankOffset;

                CMemoryMgr::Free(req.StreamBufPtr);

                const auto offset = req.BankOffset / STREAMING_SECTOR_SIZE;
                const auto sectors = offset + 2;
                AllocateStreamBuffer(sectors);

                CdStreamRead( // 0x4E00FB
                    m_StreamingChannel,
                    req.StreamBufPtr,
                    { .Offset = offset, .FileID = CdStreamHandleToFileID(m_StreamHandles[req.PakFileNo]) },
                    sectors
                );

                req.Status = eSoundRequestStatus::PENDING_LOAD_ONE_SOUND;
            }
            break;
        }
        case eSoundRequestStatus::PENDING_LOAD_ONE_SOUND: {
            if (CdStreamGetStatus(m_StreamingChannel) != eCdStreamStatus::READING_SUCCESS || req.SoundID == -1) {
                continue;
            }

            // Copy over data into internal buffer as the request buffer will be deallocated now
            assert(m_BufferSize >= req.BankNumBytes);
            memcpy(
                &m_Buffer[req.SlotInfo->Offset],
                req.StreamDataPtr,
                req.BankNumBytes
            );

            VERIFY(req.SlotInfo == &m_BankSlots[req.Slot]);
            req.SlotInfo->Bank                                                                   = req.Bank;
            req.SlotInfo->Sounds[req.SoundID].BankOffset                                         = 0;
            req.SlotInfo->Sounds[(req.SoundID + 1) % std::size(req.SlotInfo->Sounds)].BankOffset = req.BankNumBytes;

            m_BankSlotSound[req.Slot] = req.SoundID;

            CMemoryMgr::Free(req.StreamBufPtr);
            m_RequestCnt--;

            req.Status = eSoundRequestStatus::INACTIVE;

            if (m_NextOneSoundReqIdx == i) {
                m_NextOneSoundReqIdx = (m_NextOneSoundReqIdx + 1) % std::size(m_Requests);
            }
            break;
        }
        case eSoundRequestStatus::INACTIVE:
            break;
        default:
            NOTSA_UNREACHABLE("Invalid: {}", (int32)(req.Status));
        }
    }
}
