#pragma once

// TDAT block
class CData {
public:
    char*  m_data; // TODO: GxtChar*
    uint32 m_size;

public:
    CData();
    ~CData();
    bool Load(const uint32 bytes, FILESTREAM file, uint32* numCharsRead, bool loadFromMemoryCard);
    void Unload();
};
