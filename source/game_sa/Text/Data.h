#pragma once

// TDAT block
class CData {
public:
    char*  Buffer; // TODO: GxtChar*
    uint32 Count;

public:
    CData();
    ~CData();
    bool Load(const uint32 bytes, FILESTREAM file, uint32* numCharsRead, bool loadFromMemoryCard);
    void Unload();
};
