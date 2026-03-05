#include "StdInc.h"

#include "Data.h"
#include "GxtChar.h"

CData::CData() {
    m_data = nullptr;
    m_size = 0;
}

CData::~CData() {
    Unload();
}

// 0x69F640
void CData::Unload() {
    delete[] m_data;
    m_data = nullptr;
    m_size = 0;
}

// loadFromMemoryCard always 0
// 0x69F5D0
bool CData::Load(const uint32 length, FILESTREAM file, uint32* offset, bool loadFromMemoryCard) {
#if 0
    uint32 temp = 0;

    if (!length)
        return true;

    for (uint32 i = 0; i < m_size; ++i) {
        if (!loadFromMemoryCard)
            if (sizeof(GxtChar) != CFileMgr::Read(file, &temp, sizeof(GxtChar)))
                return false;

        m_data[i] = (GxtChar)temp;
        ++*offset;
    }
#endif

    m_size = length / sizeof(char);
    m_data = new char[m_size];

    CFileMgr::Read(file, m_data, length);
    *offset += length;

    return true;
}
