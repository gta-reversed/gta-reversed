/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"

class CRunningScript;

class CStreamedScripts {
public:
    static constexpr uint16 NUM_STREAMED_SCRIPTS = 82;

    struct {
        uint8* data;
        char   m_nStatus;
        char   field_5;
        int16  m_nScmIndex;
        char   m_Name[20];
        int32  m_nSize;
    } m_aScripts[NUM_STREAMED_SCRIPTS];

    int32 m_nLargestExternalSize;
    int16 m_nCountOfScripts;
    int16 field_A46;

public:
    static void     InjectHooks();

    void            Initialise();
    void            ReInitialise();

    int32           FindStreamedScript(const char* scriptName);
    int32           FindStreamedScriptQuiet(const char* scriptName);
    int16           GetProperIndexFromIndexUsedByScript(int16 scmIndex);
    const char*     GetStreamedScriptFilename(uint16 index);
    uint32          GetStreamedScriptWithThisStartAddress(uint8* dataPtr);
    void            LoadStreamedScript(RwStream* stream, int32 index);
    void            ReadStreamedScriptData();
    int32           RegisterScript(const char* scriptName);
    void            RemoveStreamedScriptFromMemory(int32 index);
    CRunningScript* StartNewStreamedScript(int32 index);
};
VALIDATE_SIZE(CStreamedScripts, 0xA48);
