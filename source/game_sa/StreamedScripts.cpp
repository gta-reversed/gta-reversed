#include "StdInc.h"

#include "StreamedScripts.h"
#include "TheScripts.h"
#include "SCMChunks.hpp"

void CStreamedScripts::InjectHooks() {
    RH_ScopedClass(CStreamedScripts);
    RH_ScopedCategory("Scripts");

    RH_ScopedInstall(Initialise, 0x470660);
    RH_ScopedInstall(LoadStreamedScript, 0x470840);
    RH_ScopedInstall(ReInitialise, 0x4706A0);
    RH_ScopedInstall(FindStreamedScript, 0x470740);
    RH_ScopedInstall(FindStreamedScriptQuiet, 0x4706F0);
    RH_ScopedInstall(GetProperIndexFromIndexUsedByScript, 0x470810);
    RH_ScopedInstall(GetStreamedScriptFilename, 0x470900);
    RH_ScopedInstall(RemoveStreamedScriptFromMemory, 0x4708E0);
    RH_ScopedInstall(StartNewStreamedScript, 0x470890);
    RH_ScopedInstall(ReadStreamedScriptData, 0x470750);
    RH_ScopedInstall(GetStreamedScriptWithThisStartAddress, 0x470910);
}

// 0x470660
void CStreamedScripts::Initialise() {
    rng::fill(m_aScripts, CStreamedScriptInfo{});
}

// 0x470840
void CStreamedScripts::LoadStreamedScript(RwStream* stream, int32 index) {
    if (CTheScripts::CheckStreamedScriptVersion(stream, "UnKown")) {
        auto& scr = m_aScripts[index];

        scr.m_StreamedScriptMemory = new uint8[scr.m_SizeInBytes];
        RwStreamRead(stream, scr.m_StreamedScriptMemory, scr.m_SizeInBytes);
    }
}

// 0x4706A0
void CStreamedScripts::ReInitialise() {
    for (auto& scr : m_aScripts) {
        // Remove all references so CStreaming::RemoveLeastUsedModel
        // will garbage collect (i.e. remove) it later.
        scr.m_NumberOfUsers = 0;
    }
}

// 0x470740 -- inlined
int32 CStreamedScripts::FindStreamedScript(const char* scriptName) {
    return FindStreamedScriptQuiet(scriptName);
}

// 0x4706F0 -- inlined
int32 CStreamedScripts::FindStreamedScriptQuiet(const char* scriptName) {
    for (auto&& [i, scr] : notsa::enumerate(GetActiveScripts())) {
        if (!_stricmp(scr.m_Filename, scriptName)) {
            return i;
        }
    }
    return -1;
}

// 0x470810
int16 CStreamedScripts::GetProperIndexFromIndexUsedByScript(int16 scmIndex) {
    for (auto&& [i, scr] : notsa::enumerate(GetActiveScripts())) {
        if (scr.m_IndexUsedByScriptFile == scmIndex) {
            return i;
        }
    }
    return -1;
}

// 0x470900
const char* CStreamedScripts::GetStreamedScriptFilename(uint16 index) {
    return m_aScripts[index].m_Filename;
}

// 0x4708E0
void CStreamedScripts::RemoveStreamedScriptFromMemory(int32 index) {
    delete[] std::exchange(m_aScripts[index].m_StreamedScriptMemory, nullptr);
}

// 0x470890
CRunningScript* CStreamedScripts::StartNewStreamedScript(int32 index)
{
    auto& scr = m_aScripts[index];
    if (auto* bip = scr.m_StreamedScriptMemory)
    {
        CRunningScript* rscr = CTheScripts::StartNewScript(bip);
        rscr->SetBaseIp(bip);
        rscr->SetExternal(true);

        scr.m_NumberOfUsers++;
        CStreaming::SetMissionDoesntRequireModel(SCMToModelId(index));
        return rscr;
    }

    return nullptr;
}

// 0x470750
void CStreamedScripts::ReadStreamedScriptData()
{
    auto* streamedScrChunk = CTheScripts::GetSCMChunk<tSCMStreamedScriptFileInfoChunk>();
    m_nLargestExternalSize = streamedScrChunk->m_LargestStreamScriptSize;

    int16 streamIdx = 0;
    for (const auto& streamed : streamedScrChunk->GetStreamedScripts()) {
        if (streamIdx >= m_nCountOfScripts) {
            break;
        }

        // NOTSA: `streamed.m_FileName` was copied into a local string before calling find.
        // NOTSA: Find returning -1 ends with OOB access, we don't let it do that.
        if (const auto i = FindStreamedScript(streamed.m_FileName); i != -1) {
            auto& scr = m_aScripts[i];
            scr.m_SizeInBytes = streamed.m_Size;
            scr.m_IndexUsedByScriptFile = streamIdx++;
        }
    }
}

int32 CStreamedScripts::RegisterScript(const char* scriptName) {
    return plugin::CallMethodAndReturn<int32, 0x4706C0, CStreamedScripts*, const char*>(this, scriptName);
}

// 0x470910
uint32 CStreamedScripts::GetStreamedScriptWithThisStartAddress(uint8* dataPtr)
{
    uint16 result;
    for (result = 0; result < NUM_STREAMED_SCRIPTS && m_aScripts[result].m_StreamedScriptMemory != dataPtr; ++result)
        ;
    return result;
}
