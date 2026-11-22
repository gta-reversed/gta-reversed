#pragma once

#include "Text.h"

struct tMissionTextOffset {
    char   szMissionName[8];
    uint32 offset;
};

// TABL block
class CMissionTextOffsets {
public:
    CMissionTextOffsets();

    bool Load(uint32 length, FILE* file, uint32* offset, int32 unknown);

    tMissionTextOffset GetTextOffset(uint32 id) const { return m_offsets[id]; }
    uint32 GetSize() const { return m_size; }

private:
    static const uint16 MAX_MISSION_TEXTS = 200;

    tMissionTextOffset m_offsets[MAX_MISSION_TEXTS];
    uint32             m_size;
};
