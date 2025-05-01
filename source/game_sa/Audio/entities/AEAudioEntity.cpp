/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"
#include "AEAudioEntity.h"

int8*& CAEAudioEntity::m_pAudioEventVolumes = *reinterpret_cast<int8**>(0xBD00F8);

CAEAudioEntity::CAEAudioEntity() : m_pEntity(nullptr), m_tempSound() {}

// NOTSA | INLINED | REFACTORED
bool CAEAudioEntity::StaticInitialise() {
    m_pAudioEventVolumes = new int8[45401];
    auto file = CFileMgr::OpenFile("AUDIO\\CONFIG\\EVENTVOL.DAT", "r");
    if (!file) {
        NOTSA_LOG_WARN("[AudioEngine] Failed to open EVENTVOL.DAT");
        CFileMgr::CloseFile(file);
        return false;
    }
    if (CFileMgr::Read(file, m_pAudioEventVolumes, 45401) != 45401) {
        NOTSA_LOG_WARN("[AudioEngine] Failed to read EVENTVOL.DAT");
        CFileMgr::CloseFile(file);
        return false;
    }
    CFileMgr::CloseFile(file);
    return true;
}

// NOTSA | INLINED | REFACTORED
void CAEAudioEntity::Shutdown() {
    delete[] m_pAudioEventVolumes;
    m_pAudioEventVolumes = nullptr;
}
