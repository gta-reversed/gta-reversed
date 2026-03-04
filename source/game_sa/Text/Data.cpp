#include "StdInc.h"

#include "Data.h"
#include "GxtChar.h"

CData::CData() {
    Buffer = nullptr;
    Count = 0;
}

CData::~CData() {
    Unload();
}

// 0x69F640
void CData::Unload() {
    delete[] Buffer;
    Buffer = nullptr;
    Count = 0;
}

// loadFromMemoryCard always 0
// 0x69F5D0
bool CData::Load(const uint32 length, FILESTREAM file, uint32* offset, bool loadFromMemoryCard) {
#if 0
    uint32 temp = 0;

    if (!length)
        return true;

    for (uint32 i = 0; i < Count; ++i) {
        if (!loadFromMemoryCard)
            if (sizeof(GxtChar) != CFileMgr::Read(file, &temp, sizeof(GxtChar)))
                return false;

        Buffer[i] = (GxtChar)temp;
        ++*offset;
    }
#endif

    Count = length / sizeof(char);
    Buffer = new char[Count];

    CFileMgr::Read(file, Buffer, length);
    *offset += length;

    return true;
}
