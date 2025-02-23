#include "StdInc.h"
#include "AEMP3BankLoader.h"
#include "AEAudioUtility.h"

// Naming scheme:
// * BankLookup == BankId
//

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
    RH_ScopedInstall(Service, 0x4DFE30,{.reversed=false}); // TODO: broken
}

// 0x4DFB10
CAEMP3BankLoader::CAEMP3BankLoader() {
    m_bInitialised = false;
    m_iStreamingChannel = 4;
}

// 0x4E08F0
bool CAEMP3BankLoader::Initialise() {
    rng::for_each(m_aRequests, &CAESoundRequest::Reset);
    field_664 = 0;
    m_iRequestCount = 0;
    m_iNextRequest = 0;

    if (!LoadBankSlotFile() || !LoadBankLookupFile() || !LoadSFXPakLookupFile())
        return false;

    m_bInitialised = true;
    return true;
}

// 0x4DFDE0, inlined
uint8* CAEMP3BankLoader::GetBankSlotBuffer(uint16 bankSlot, uint32& outLength) {
    if (!m_bInitialised || bankSlot >= m_nBankSlotCount)
        return nullptr;

    outLength = m_paBankSlots[bankSlot].NumBytes;
    return &m_pBuffer[m_paBankSlots[bankSlot].Offset];
}

// 0x4E01E0
float CAEMP3BankLoader::GetSoundHeadroom(uint16 soundId, int16 bankSlot) {
    return m_bInitialised
        ? (float)m_paBankSlots[bankSlot].Sounds[soundId].Headroom / 100.0f
        : 0.f;
}

// 0x4E0220
bool CAEMP3BankLoader::IsSoundBankLoaded(uint16 bankId, int16 bankSlot) {
    assert(bankSlot < m_nBankSlotCount);
    return m_bInitialised && m_paBankSlots[bankSlot].Bank == bankId;
}

// 0x4E0250
bool CAEMP3BankLoader::GetSoundBankLoadingStatus(uint16 bankId, int16 bankSlot) {
    return IsSoundBankLoaded(bankId, bankSlot);
}

// 0x4E0280
uint8* CAEMP3BankLoader::GetSoundBuffer(uint16 soundId, int16 bankSlot, uint32& outSize, uint16& outSampleRate) {
    if (!m_bInitialised || m_paBankSlots[bankSlot].Bank == (uint16)-1)
        return nullptr;

    auto& bank = m_paBankSlots[bankSlot];

    if (!bank.IsSingleSound() && soundId >= bank.NumSounds)
        return nullptr;

    if (bank.IsSingleSound() && m_aBankSlotSound[bankSlot] != soundId)
        return nullptr;

    uint32 bankBufferSize{};
    auto* bankInBuffer = GetBankSlotBuffer(bankSlot, bankBufferSize);

    if (soundId < NUM_BANK_SLOT_ITEMS && bank.IsSingleSound() || soundId < bank.NumSounds - 1) {
        assert(soundId < NUM_BANK_SLOT_ITEMS);
        outSize = bank.Sounds[(soundId + 1) % NUM_BANK_SLOT_ITEMS].BankOffset - bank.Sounds[soundId].BankOffset;
    } else {
        assert(soundId < NUM_BANK_SLOT_ITEMS);
        outSize = m_paBankLookups[bank.Bank].NumBytes - bank.Sounds[soundId].BankOffset;
    }
    assert(soundId < NUM_BANK_SLOT_ITEMS);
    outSampleRate = bank.Sounds[soundId].Frequency;

    return &bankInBuffer[bank.Sounds[soundId].BankOffset];
}

// 0x4E0380
int32 CAEMP3BankLoader::GetLoopOffset(uint16 soundId, int16 bankSlot) {
    if (!m_bInitialised)
        return -1;

    assert(bankSlot < m_nBankSlotCount);
    auto& bank = m_paBankSlots[bankSlot];

    assert(bank.IsSingleSound() && m_aBankSlotSound[bankSlot] == soundId || soundId < bank.NumSounds);
    return bank.Sounds[soundId].LoopStartOffset;
}

// 0x4E03B0
bool CAEMP3BankLoader::IsSoundLoaded(uint16 bankId, uint16 soundId, int16 bankSlot) {
    return m_bInitialised
        && m_paBankSlots[bankSlot].Bank == bankId
        && m_aBankSlotSound[bankSlot] == soundId;
}

// 0x4E0400
bool CAEMP3BankLoader::GetSoundLoadingStatus(uint16 bankId, uint16 soundId, int16 bankSlot) {
    return IsSoundLoaded(bankId, soundId, bankSlot);
}

// 0x4E0450
void CAEMP3BankLoader::UpdateVirtualChannels(tVirtualChannelSettings* settings, int16* lengths, int16* loopStartTimes) {
    for (auto i = 0u; i < std::size(settings->BankSlotIDs); i++) {
        const auto slotId = settings->BankSlotIDs[i];
        const auto soundId = settings->SoundIDs[i];
        const auto& bankSlot = m_paBankSlots[slotId];

        if (slotId < 0 || soundId < 0 || bankSlot.Bank == -1 || (soundId >= bankSlot.NumSounds && bankSlot.NumSounds >= 0)) {
            lengths[i] = -1;
            loopStartTimes[i] = -1;
        } else {
            lengths[i] = CAEAudioUtility::ConvertFromBytesToMS(
                bankSlot.CalculateSizeOfSlotItem(soundId),
                bankSlot.Sounds[soundId].Frequency,
                1u
            );

            loopStartTimes[i] = [&]() -> int32 {
                if (const auto loopOffset = bankSlot.Sounds[soundId].LoopStartOffset; loopOffset != -1) {
                    return CAEAudioUtility::ConvertFromBytesToMS(
                        2 * loopOffset, // ??
                        bankSlot.Sounds[soundId].Frequency,
                        1u
                    );
                } else {
                    return -1;
                }
            }();
        }
    }
}

// 0x4E0670
void CAEMP3BankLoader::LoadSoundBank(uint16 bankId, int16 bankSlot) {
    if (!m_bInitialised)
        return;

    if (bankId > m_nBankLookupCount || bankSlot < 0 || bankSlot > m_nBankSlotCount)
        return;

    if (IsSoundBankLoaded(bankId, bankSlot))
        return;

    for (auto& req : m_aRequests) {
        if (req.m_nBankId == bankId && req.m_nBankSlotId == bankSlot)
            return;
    }

    m_aRequests[m_iNextRequest] = CAESoundRequest(
        bankId,
        bankSlot,
        -1,
        m_paBankSlots[bankSlot],
        GetBankLookup(bankId),
        eSoundRequestStatus::JUST_LOADED
    );

    m_iRequestCount++;
    m_iNextRequest = (m_iNextRequest + 1) % std::size(m_aRequests);

    NOTSA_LOG_TRACE("Loading bank ({}) into slot ({})", bankId, bankSlot);
}

// 0x4E07A0
bool CAEMP3BankLoader::LoadSound(uint16 bankId, uint16 soundId, int16 bankSlot) {
    if (!m_bInitialised)
        return false;

    if (soundId >= NUM_BANK_SLOT_ITEMS || bankId > m_nBankLookupCount || bankSlot < 0 || bankSlot > m_nBankSlotCount)
        return false;

    if (IsSoundLoaded(bankId, soundId, bankSlot))
        return true;

    for (auto& req : m_aRequests) {
        if (req.m_nBankId == bankId && req.m_nBankSlotId == bankSlot && req.m_nNumSounds == soundId)
            return true;
    }

    const auto bankLookup = GetBankLookup(bankId);
    auto& nextRequest = m_aRequests[m_iNextRequest];

    nextRequest.m_nBankId = bankId;
    nextRequest.m_nBankSlotId = bankSlot;
    nextRequest.m_nNumSounds = soundId;
    nextRequest.m_pBankSlotInfo = &m_paBankSlots[bankSlot];
    nextRequest.m_nPakFileNumber = bankLookup->PakFile;
    nextRequest.m_nBankOffset = bankLookup->FileOffset;
    nextRequest.m_nBankSize = bankLookup->NumBytes;
    nextRequest.m_nStatus = eSoundRequestStatus::JUST_LOADED;

    m_iRequestCount++;
    m_iNextRequest = (m_iNextRequest + 1) % std::size(m_aRequests);

    return true;
}

// 0x4DFE30, broken
void CAEMP3BankLoader::Service() {
    // TODO: Refactor the shit out of this
    for (auto&& [i, req] : notsa::enumerate(m_aRequests)) {
        switch (req.m_nStatus) {
        case eSoundRequestStatus::JUST_LOADED: {
            if (CdStreamGetStatus(m_iStreamingChannel) != eCdStreamStatus::READING_SUCCESS)
                continue;

            const auto numSectors = req.IsSingleSound() ? (req.m_nBankSize + sizeof(CdAudioStream)) / STREAMING_SECTOR_SIZE + 2 : 4;
            const auto bankOffsetInSectors = req.m_nBankOffset / STREAMING_SECTOR_SIZE;

            req.m_pStreamBuffer = (CdAudioStream*)CMemoryMgr::Malloc(numSectors * STREAMING_SECTOR_SIZE);
            req.m_pStreamOffset = req.m_pStreamBuffer;
            //&req.m_pStreamBuffer[req.m_nBankOffset - bankOffsetInSectors * STREAMING_SECTOR_SIZE]; // == &buffer[0]?
            CdStreamRead(
                m_iStreamingChannel,
                req.m_pStreamBuffer,
                bankOffsetInSectors & 0xFFFFFF | m_paStreamHandles[req.m_nPakFileNumber], // TODO: Encoding helper.
                numSectors
            );

            req.m_nStatus = eSoundRequestStatus::ALREADY_LOADED;
            break;
        }
        case eSoundRequestStatus::ALREADY_LOADED: {
            if (CdStreamGetStatus(m_iStreamingChannel) != eCdStreamStatus::READING_SUCCESS)
                continue;

            if (req.IsSingleSound()) {
                memcpy(
                    &m_pBuffer[req.m_pBankSlotInfo->Offset],
                    &req.m_pStreamOffset->m_aBankData,
                    req.m_nBankSize
                );
                memcpy(
                    &m_paBankSlots[req.m_nBankSlotId].Sounds,
                    &req.m_pStreamOffset->m_aSlotItems,
                    sizeof(m_paBankSlots[req.m_nBankSlotId].Sounds)
                );
                m_paBankSlots[req.m_nBankSlotId].Bank = req.m_nBankId;
                m_paBankSlots[req.m_nBankSlotId].NumSounds = req.m_pStreamOffset->m_nSoundCount;
                m_aBankSlotSound[req.m_nBankSlotId] = -1;

                CMemoryMgr::Free(req.m_pStreamBuffer);
                req.m_pStreamBuffer = nullptr;
                req.m_nBankId = req.m_nBankSlotId = req.m_nNumSounds = -1;
                req.m_nStatus = eSoundRequestStatus::UNK_0;

                m_iRequestCount--;
            } else {
                memcpy(
                    m_paBankSlots[req.m_nBankSlotId].Sounds,
                    &req.m_pStreamOffset->m_aSlotItems,
                    sizeof(m_paBankSlots[req.m_nBankSlotId].Sounds)
                );
                m_paBankSlots[req.m_nBankSlotId].Bank = -1;
                m_paBankSlots[req.m_nBankSlotId].NumSounds = -1; // ?
                m_aBankSlotSound[req.m_nBankSlotId] = -1;

                const auto nextItemOffset = [&] {
                    if (req.m_nNumSounds >= req.m_pStreamOffset->m_nSoundCount - 1) {
                        // No next item?
                        return m_paBankLookups[req.m_nBankId].NumBytes;
                    } else {
                        return req.m_pStreamOffset->m_aSlotItems[req.m_nNumSounds + 1].BankOffset;
                    }
                }();
                req.m_nBankSize = nextItemOffset - req.m_pStreamOffset->m_aSlotItems[req.m_nNumSounds].BankOffset;
                req.m_nBankOffset += req.m_pStreamOffset->m_aSlotItems[req.m_nNumSounds].BankOffset + sizeof(CdAudioStream);

                const auto bankOffsetInSectors = req.m_nBankOffset / STREAMING_SECTOR_SIZE;

                CMemoryMgr::Free(req.m_pStreamBuffer);

                req.m_pStreamBuffer = (CdAudioStream*)CMemoryMgr::Malloc((bankOffsetInSectors + 2) * STREAMING_SECTOR_SIZE);
                req.m_pStreamOffset = req.m_pStreamBuffer; //&req.m_pStreamBuffer[req.m_nBankOffset - bankOffsetInSectors * STREAMING_SECTOR_SIZE]; // == &buffer[0]?

                CdStreamRead(
                    m_iStreamingChannel,
                    req.m_pStreamBuffer,
                    bankOffsetInSectors & 0xFFFFFF | m_paStreamHandles[req.m_nPakFileNumber],
                    bankOffsetInSectors + 2
                );

                req.m_nStatus = eSoundRequestStatus::UNK_3;
            }
            break;
        }
        case eSoundRequestStatus::UNK_3: {
            if (CdStreamGetStatus(m_iStreamingChannel) != eCdStreamStatus::READING_SUCCESS || req.IsSingleSound())
                continue;

            memcpy(
                &m_pBuffer[req.m_pBankSlotInfo[0].Offset],
                req.m_pStreamOffset,
                req.m_nBankSize
            );

            auto& slot = m_paBankSlots[req.m_nBankSlotId];
            slot.Bank = req.m_nBankId;
            slot.Sounds[req.m_nNumSounds].BankOffset = 0;
            slot.Sounds[(req.m_nNumSounds + 1) % std::size(slot.Sounds)].BankOffset = req.m_nBankSize;
            m_aBankSlotSound[req.m_nBankSlotId] = req.m_nNumSounds;

            CMemoryMgr::Free(req.m_pStreamBuffer);
            req.m_pStreamBuffer = nullptr;
            req.m_nBankId = req.m_nBankSlotId = req.m_nNumSounds = -1;
            req.m_nStatus = eSoundRequestStatus::UNK_0;

            m_iRequestCount--;

            if (i == field_664) {
                field_664 = (field_664 + 1) % std::size(m_aRequests);
            }
            break;
        }
        default:
            break;
        }
    }
}

const CAEBankSlot* CAEMP3BankLoader::GetBankSlot(eSoundBankSlot slot) const {
    return &m_paBankSlots[slot];
}
