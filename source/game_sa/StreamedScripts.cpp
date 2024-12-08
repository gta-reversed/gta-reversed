#include "StdInc.h"

#include "StreamedScripts.h"
#include "TheScripts.h"

void CStreamedScripts::InjectHooks() {
    RH_ScopedClass(CStreamedScripts);
    RH_ScopedCategory("Scripts");

    RH_ScopedInstall(Initialise, 0x470660);
    RH_ScopedInstall(StartNewStreamedScript, 0x470890);
}

// 0x470660
void CStreamedScripts::Initialise() {
    rng::fill(m_aScripts, CStreamedScriptInfo{});
}

void CStreamedScripts::LoadStreamedScript(RwStream* stream, int32 index) {
    plugin::CallMethod<0x470840, CStreamedScripts*, RwStream*, int32>(this, stream, index);
}

void CStreamedScripts::ReInitialise() {
    plugin::CallMethod<0x4706A0, CStreamedScripts*>(this);
}

int32 CStreamedScripts::FindStreamedScript(const char* scriptName) {
    return int32();
}

int32 CStreamedScripts::FindStreamedScriptQuiet(const char* scriptName) {
    return int32();
}

int16 CStreamedScripts::GetProperIndexFromIndexUsedByScript(int16 scmIndex) {
    return int16();
}

const char* CStreamedScripts::GetStreamedScriptFilename(uint16 index) {
    return nullptr;
}

void CStreamedScripts::RemoveStreamedScriptFromMemory(int32 index) {
    plugin::CallMethod<0x4708E0, CStreamedScripts*, int32>(this, index);
}

// 0x470890
CRunningScript* CStreamedScripts::StartNewStreamedScript(int32 index)
{
    auto* scriptInfo = &m_aScripts[index];
    uint8* ip = scriptInfo->data;

    if (ip)
    {
        CRunningScript * script = CTheScripts::StartNewScript(ip);
        script->SetBaseIp(ip);
        script->SetExternal(true);

        scriptInfo->m_nStatus++;
        CStreaming::SetMissionDoesntRequireModel(SCMToModelId(index));
        return script;
    }

    return nullptr;
}

// 0x470750
void CStreamedScripts::ReadStreamedScriptData()
{
    plugin::CallMethod<0x470750, CStreamedScripts*>(this);
}

int32 CStreamedScripts::RegisterScript(const char* scriptName) {
    return plugin::CallMethodAndReturn<int32, 0x4706C0, CStreamedScripts*, const char*>(this, scriptName);
}

// 0x470910
uint32 CStreamedScripts::GetStreamedScriptWithThisStartAddress(uint8* dataPtr)
{
    uint16 result;
    for (result = 0; result < NUM_STREAMED_SCRIPTS && m_aScripts[result].data != dataPtr; ++result)
        ;
    return result;
}
