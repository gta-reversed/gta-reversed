#pragma once

// TDAT block
class CData {
public:
    char*  m_data;
    uint32 m_size;

public:
    CData();
    ~CData();

    bool Load(uint32 length, FILESTREAM file, uint32* offset, uint8 unknown);
    void Unload();
};
