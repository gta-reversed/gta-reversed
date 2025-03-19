#include "StdInc.h"
#include "AEMP3BankLoader.h"
#include "AEAudioUtility.h"
#include <cstdlib>

void CAEMP3BankLoader::InjectHooks() {
    RH_ScopedClass(CAEMP3BankLoader);
    RH_ScopedCategory("Audio/Loaders");

    RH_ScopedInstall(Constructor, 0x4DFB10);
    RH_ScopedInstall(Initialise, 0x4E08F0);
    RH_ScopedInstall(GetBankSlotBuffer, 0x4DFDE0);
    RH_ScopedInstall(GetSoundHeadroom, 0x4E01E0);
    RH_ScopedInstall(IsSoundBankLoaded, 0x4E0220);
    RH_ScopedInstall(GetSoundBankLoadingStatus, 0x4E0250);
    RH_ScopedInstall(GetSoundBuffer, 0x4E0280);
    RH_ScopedInstall(IsSoundLoaded, 0x4E03B0);
    RH_ScopedInstall(GetSoundLoadingStatus, 0x4E0400);
    RH_ScopedInstall(UpdateVirtualChannels, 0x4E0450);
    RH_ScopedInstall(LoadSoundBank, 0x4E0670);
    RH_ScopedInstall(LoadSound, 0x4E07A0);
    RH_ScopedInstall(Service, 0x4DFE30);
}

// 0x4E08F0
bool CAEMP3BankLoader::Initialise() {
    rng::fill(m_Requests, CAESoundRequest{});
    m_NextOneSoundReqIdx = 0;
    m_RequestCnt         = 0;
    m_NextRequestIdx     = 0;

    if (!LoadBankSlotFile() || !LoadBankLookupFile() || !LoadSFXPakLookupFile()) {
        return false;
    }

    m_IsInitialised = true;
    return true;
}

// 0x4DFDE0, inlined
uint8* CAEMP3BankLoader::GetBankSlotBuffer(eSoundBankSlot bankSlot, uint32& outLength) {
    if (!m_IsInitialised || bankSlot >= m_BankSlotCnt) {
        return nullptr;
    }
    const auto& slot = GetBankSlot(bankSlot);
    outLength = slot.NumBytes;
    return &m_Buffer[slot.OffsetBytes];
}

// 0x4E01E0
float CAEMP3BankLoader::GetSoundHeadroom(eSoundID soundId, eSoundBankSlot bankSlot) {
    return m_IsInitialised
        ? (float)(GetBankSlot(bankSlot).Sounds[soundId].Headroom) / 100.0f
        : 0.f;
}

// 0x4E0220
bool CAEMP3BankLoader::IsSoundBankLoaded(eSoundBank bankId, eSoundBankSlot bankSlot) {
    return m_IsInitialised && GetBankSlot(bankSlot).Bank == bankId;
}

// 0x4E0250
bool CAEMP3BankLoader::GetSoundBankLoadingStatus(eSoundBank bankId, eSoundBankSlot bankSlot) {
    return IsSoundBankLoaded(bankId, bankSlot);
}

// 0x4E0280
uint8* CAEMP3BankLoader::GetSoundBuffer(eSoundID soundId, eSoundBankSlot bankSlot, uint32& outSize, uint16& outSampleRate) {
    if (!m_IsInitialised) {
        return nullptr;
    }

    auto& slot = GetBankSlot(bankSlot);
    if (slot.Bank == SND_BANK_UNK) {
        NOTSA_LOG_WARN("Trying to get sound ({}) buffer from a slot ({}) with no bank!", (int32)(soundId), (int32)(bankSlot));
        return nullptr;
    }

    if (slot.IsSingleSound() ? m_BankSlotSound[bankSlot] != soundId : soundId >= slot.NumSounds) {
        return nullptr;
    }

    uint32 bankSlotBufferSize{};
    auto*  bankSlotBuffer = GetBankSlotBuffer(bankSlot, bankSlotBufferSize);

    outSize = soundId < (eSoundID)(slot.Sounds.size()) && slot.IsSingleSound() || soundId < slot.NumSounds - 1
        ? slot.Sounds[(soundId + 1) % slot.Sounds.size()].BankOffsetBytes - slot.Sounds[soundId].BankOffsetBytes
        : GetBankLookup(slot.Bank).NumBytes - slot.Sounds[soundId].BankOffsetBytes;
    outSampleRate = slot.Sounds[soundId].SampleFrequency;

    const auto offset = slot.Sounds[soundId].BankOffsetBytes;
    assert(offset < bankSlotBufferSize && "Out-of-bounds offset");
    assert(offset + outSize < bankSlotBufferSize && "Out-of-bounds size");
    return &bankSlotBuffer[offset];
}

// 0x4E0380
int32 CAEMP3BankLoader::GetLoopOffset(eSoundID soundId, eSoundBankSlot bankSlot) {
    if (!m_IsInitialised) {
        return -1;
    }

    auto& slot = GetBankSlot(bankSlot);

    assert(slot.IsSingleSound() ? m_BankSlotSound[bankSlot] == soundId : soundId < slot.NumSounds);
    return slot.Sounds[soundId].LoopStartOffset;
}

// 0x4E03B0
bool CAEMP3BankLoader::IsSoundLoaded(eSoundBank bankId, eSoundID soundId, eSoundBankSlot bankSlot) const {
    return m_IsInitialised
        && GetBankSlot(bankSlot).Bank == bankId
        && m_BankSlotSound[bankSlot] == soundId;
}

// 0x4E0400
bool CAEMP3BankLoader::GetSoundLoadingStatus(eSoundBank bankId, eSoundID soundId, eSoundBankSlot bankSlot) {
    return IsSoundLoaded(bankId, soundId, bankSlot);
}

// 0x4E0450
void CAEMP3BankLoader::UpdateVirtualChannels(tVirtualChannelSettings* settings, int16* lengths, int16* loopStartTimes) {
    for (auto i = 0u; i < std::size(settings->BankSlotIDs); i++) {
        const auto sfx    = settings->SoundIDs[i];
        const auto slotID = settings->BankSlotIDs[i];
        const auto slot   = slotID != SND_BANK_SLOT_UNK
            ? &GetBankSlot(slotID)
            : nullptr;

        if (!slot || sfx == -1 || slot->Bank == SND_BANK_UNK || (sfx >= slot->NumSounds && !slot->IsSingleSound())) {
            lengths[i]        = -1;
            loopStartTimes[i] = -1;
        } else {
            lengths[i] = CAEAudioUtility::ConvertFromBytesToMS(
                slot->GetSoundSize(sfx),
                slot->Sounds[sfx].SampleFrequency,
                1
            );

            const auto loopOffset = slot->Sounds[sfx].LoopStartOffset;
            loopStartTimes[i] = loopOffset != -1
                ? CAEAudioUtility::ConvertFromBytesToMS(2 * loopOffset, slot->Sounds[sfx].SampleFrequency, 1u) // Why `*2`?
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

    // Same request already exists?
    for (auto& req : m_Requests) {
        if (req.Bank == bank && req.Slot == slot && req.SoundID == sound.value_or(-1)) {
            return;
        }
    }

    // Add new request
    const auto* const bankLkup = &GetBankLookup(bank);
    auto& req = m_Requests[m_NextRequestIdx] = CAESoundRequest{
        .SlotInfo        = &GetBankSlot(slot),
        .BankOffsetBytes = bankLkup->FileOffset,
        .Status          = eSoundRequestStatus::REQUESTED,
        .Bank            = bank,
        .Slot            = slot,
        .SoundID         = sound.value_or(-1),
        .PakFileNo       = bankLkup->PakFileNo
    };
    if (!sound.has_value()) {
        req.BankNumBytes = bankLkup->NumBytes;
    }
    m_RequestCnt++;
    m_NextRequestIdx = (m_NextRequestIdx + 1) % std::size(m_Requests);
}


// 0x4E0670
void CAEMP3BankLoader::LoadSoundBank(eSoundBank bankId, eSoundBankSlot bankSlot) {
    // Already loaded?
    if (IsSoundBankLoaded(bankId, bankSlot)) {
        return;
    }
    AddRequest((eSoundBank)(bankId), (eSoundBankSlot)(bankSlot), std::nullopt);
}

// 0x4E07A0
void CAEMP3BankLoader::LoadSound(eSoundBank bankId, eSoundID soundId, eSoundBankSlot bankSlot) {
    if (IsSoundLoaded(bankId, soundId, bankSlot)) {
        return;
    }
    if (soundId >= 0 && soundId < AE_BANK_MAX_NUM_SOUNDS) {
        AddRequest((eSoundBank)(bankId), (eSoundBankSlot)(bankSlot), (eSoundID)(soundId));
    } else {
        NOTSA_LOG_WARN("Trying to load invalid sound ({})", (int32)(soundId));
    }
}

// 0x4DFE30
void CAEMP3BankLoader::Service() {
    for (auto&& [i, req] : notsa::enumerate(m_Requests)) {
        const auto AllocateMemoryAndRead = [&](size_t readSizeBytes) {
            // Convert bytes to sectors
            const auto readSizeSectors = (readSizeBytes / STREAMING_SECTOR_SIZE) + 2;

            // Allocate buffers
            assert(!req.StreamBufPtr);
            const auto buf    = (std::byte*)(CMemoryMgr::Malloc(readSizeSectors * STREAMING_SECTOR_SIZE));
            req.StreamBufPtr  = buf;
            req.StreamDataPtr = (AEAudioStream*)(buf + (req.BankOffsetBytes % STREAMING_SECTOR_SIZE)); /* Align buffer pointer */

            // Read
            CdStreamRead( // 0x4E016F
                m_StreamingChannel,
                req.StreamBufPtr,
                { .Offset = req.BankOffsetBytes / STREAMING_SECTOR_SIZE, .FileID = CdStreamHandleToFileID(m_StreamHandles[req.PakFileNo]) },
                readSizeSectors
            );
        };

        switch (req.Status) {
        case eSoundRequestStatus::REQUESTED: { // 0x4E0117
            if (CdStreamGetStatus(m_StreamingChannel) != eCdStreamStatus::READING_SUCCESS) {
                continue;
            }

            // For single sound requests we load the header only and then later the sound, otherwise the whole bank
            AllocateMemoryAndRead((sizeof(AEAudioStream) + (req.SoundID == -1 ? req.BankNumBytes : 0)));

            // Now wait for it to be read
            req.Status = eSoundRequestStatus::PENDING_READ;
            break;
        }
        case eSoundRequestStatus::PENDING_READ: { // 0x4DFF58
            if (CdStreamGetStatus(m_StreamingChannel) != eCdStreamStatus::READING_SUCCESS) {
                continue;
            }

            // Copy over sound info into the bank slot
            req.SlotInfo->Sounds = req.StreamDataPtr->Sounds;

            if (req.SoundID == -1) { // 0x4DFF6E - Load whole bank?
                // Copy over data into internal buffer as the request buffer will be deallocated now
                assert(m_BufferSize > req.SlotInfo->OffsetBytes);
                assert(m_BufferSize >= req.SlotInfo->OffsetBytes + req.BankNumBytes);
                memcpy(
                    &m_Buffer[req.SlotInfo->OffsetBytes],
                    &req.StreamDataPtr->BankData,
                    req.BankNumBytes
                );

                VERIFY(req.SlotInfo == &m_BankSlots[req.Slot]);
                req.SlotInfo->Bank        = req.Bank;
                req.SlotInfo->NumSounds   = req.StreamDataPtr->NumSounds;

                m_BankSlotSound[req.Slot] = -1; // Whole bank loaded, so use `-1`

                CMemoryMgr::Free(std::exchange(req.StreamBufPtr, nullptr));
                m_RequestCnt--;

                req.Status = eSoundRequestStatus::INACTIVE;
            } else { // 0x4E0033 - Load specified sound only
                // At this point only the header (AEAudioStream) has been
                // loaded into memory, with that info we can calculate
                // where the sound's data is and load it in the next step

                VERIFY(req.SlotInfo == &m_BankSlots[req.Slot]);
                req.SlotInfo->Bank      = SND_BANK_UNK;
                req.SlotInfo->NumSounds = -1;
                req.BankOffsetBytes    += sizeof(AEAudioStream) + req.StreamDataPtr->Sounds[req.SoundID].BankOffsetBytes;

                m_BankSlotSound[req.Slot] = -1; // This will be set when the sound has been loaded below

                // 0x4E006F - Calculate bank size
                const auto nextOrEnd = req.SoundID + 1 >= req.StreamDataPtr->NumSounds
                    ? GetBankLookup(req.Bank).NumBytes                            // If no more sounds we use the end of bank
                    : req.StreamDataPtr->Sounds[req.SoundID + 1].BankOffsetBytes; // Otherwise use next sound's offset
                req.BankNumBytes = nextOrEnd - req.StreamDataPtr->Sounds[req.SoundID].BankOffsetBytes;

                // 0x4E00BA - De-allocate old buffer
                CMemoryMgr::Free(std::exchange(req.StreamBufPtr, nullptr));

                // 0x4E00C7 - Read the sound's data 
                AllocateMemoryAndRead(req.BankNumBytes);

                req.Status = eSoundRequestStatus::PENDING_LOAD_ONE_SOUND;
            }
            break;
        }
        case eSoundRequestStatus::PENDING_LOAD_ONE_SOUND: { // 0x4DFE92
            if (CdStreamGetStatus(m_StreamingChannel) != eCdStreamStatus::READING_SUCCESS || req.SoundID == -1) {
                continue;
            }

            NOTSA_LOG_DEBUG("Processing CAEMP3BankLoader request:\n");
            NOTSA_LOG_DEBUG("SoundID:       {}", (int32)(req.SoundID));
            NOTSA_LOG_DEBUG("Bank:          {}", (int32)(req.Bank));
            NOTSA_LOG_DEBUG("Slot:          {}", (int32)(req.Slot));
            NOTSA_LOG_DEBUG("StreamDataPtr: 0x{:x}", (uintptr)(req.StreamDataPtr));
            NOTSA_LOG_DEBUG("StreamBufPtr:  0x{:x}", (uintptr)(req.StreamBufPtr));

            // Copy over data into internal buffer as the request buffer will be deallocated now
            assert(m_BufferSize > req.SlotInfo->OffsetBytes);
            assert(m_BufferSize >= req.SlotInfo->OffsetBytes + req.BankNumBytes);
            memcpy(
                &m_Buffer[req.SlotInfo->OffsetBytes],
                req.StreamDataPtr,
                req.BankNumBytes
            );

            VERIFY(req.SlotInfo == &m_BankSlots[req.Slot]);
            req.SlotInfo->Bank                                                                        = req.Bank;
            req.SlotInfo->Sounds[req.SoundID].BankOffsetBytes                                         = 0;
            req.SlotInfo->Sounds[(req.SoundID + 1) % std::size(req.SlotInfo->Sounds)].BankOffsetBytes = req.BankNumBytes;

            m_BankSlotSound[req.Slot] = req.SoundID;

            CMemoryMgr::Free(std::exchange(req.StreamBufPtr, nullptr));
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
