/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
    source\game_sa\Audio\entities\AEAudioEntity.cpp
*/

#include "StdInc.h"
#include "AEAudioEntity.h"
#include "AESoundManager.h"

constexpr auto EVENT_VOLUMES_BUFFER_SIZE = 45401;

auto& m_pAudioEventVolumes = StaticRef<int8*>(0xBD00F8); // Use `GetDefaultVolume` to access!

// NOTSA: Cancel all sounds owned by this entity to prevent use-after-free
CAEAudioEntity::~CAEAudioEntity() {
    // Sadece ses yöneticisi tam olarak başlatıldıysa sesleri iptal et.
    if (AESoundManager.m_NumAllocatedPhysicalChannels > 0) {
        AESoundManager.CancelSoundsOwnedByAudioEntity(this, true);
    }
}

// NOTSA | INLINED | REFACTORED
bool CAEAudioEntity::StaticInitialise() {
    auto file = CFileMgr::OpenFile("AUDIO\\CONFIG\\EVENTVOL.DAT", "r");
    if (!file) {
        NOTSA_LOG_WARN("[AudioEngine] Failed to open EVENTVOL.DAT");
        return false; // REFACTOR: Dosya açılamadıysa null pointer'ı kapatmaya çalışma.
    }

    m_pAudioEventVolumes = new int8[EVENT_VOLUMES_BUFFER_SIZE];
    
    // REFACTOR: Okuma başarısız olursa ayrılan belleği serbest bırak (Memory leak önlendi).
    if (CFileMgr::Read(file, m_pAudioEventVolumes, EVENT_VOLUMES_BUFFER_SIZE) != EVENT_VOLUMES_BUFFER_SIZE) {
        NOTSA_LOG_WARN("[AudioEngine] Failed to read EVENTVOL.DAT");
        delete[] m_pAudioEventVolumes;
        m_pAudioEventVolumes = nullptr;
        CFileMgr::CloseFile(file);
        return false;
    }
    
    CFileMgr::CloseFile(file);
    return true;
}

// NOTSA | INLINED | REFACTORED
void CAEAudioEntity::Shutdown() {
    delete[] std::exchange(m_pAudioEventVolumes, nullptr);
}

float CAEAudioEntity::GetDefaultVolume(eAudioEvents event) {
    return static_cast<float>(m_pAudioEventVolumes[event]);
}