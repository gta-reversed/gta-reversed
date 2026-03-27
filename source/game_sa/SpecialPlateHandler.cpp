#include "StdInc.h"

#include "SpecialPlateHandler.h"

void CSpecialPlateHandler::InjectHooks() {
    RH_ScopedClass(CSpecialPlateHandler);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x6F2D10);
    RH_ScopedInstall(Find, 0x6F2D30);
    RH_ScopedInstall(Add, 0x6F2D90);
    RH_ScopedInstall(Remove, 0x6F2DD0);
}

// unused
// 0x6F2D10
void CSpecialPlateHandler::Init() {
    m_nCount = 0;

    for (auto &entry : m_plateTextEntries) {
        entry.m_nCarGenId = -1;
        entry.m_szPlateText[0] = 0;
    }
}

// Writes plate text to outText if exists, "\0" otherwise.
// 0x6F2D30
void CSpecialPlateHandler::Find(int32 carGenId, char* outText) {
    outText[0] = '\0';
    if (m_nCount == 0)
        return;

    for (auto &entry : m_plateTextEntries) {
        if (entry.m_nCarGenId == carGenId) {
            strcpy_s(outText, std::size(entry.m_szPlateText), entry.m_szPlateText);
            return;
        }
    }
}

// 0x6F2D90
void CSpecialPlateHandler::Add(int32 carGenId, const char* text) {
    if (m_nCount != PLATES_COUNT) {
        auto& plateEntry = m_plateTextEntries[m_nCount];
        plateEntry.m_nCarGenId = carGenId;
        strcpy_s(plateEntry.m_szPlateText, text);
        m_nCount++;
    }
}

// unused
// 0x6F2DD0
void CSpecialPlateHandler::Remove(int32 plateTextId) {
    if (plateTextId >= 0 && m_nCount) {
        tCarGenPlateText* plateText = &m_plateTextEntries[plateTextId];
        plateText->m_nCarGenId = -1;
        plateText->m_szPlateText[0] = 0;
        if ((unsigned)plateTextId < m_nCount - 1) {
            *plateText = m_plateTextEntries[m_nCount - 1];
        }
        --m_nCount;
    }
}
