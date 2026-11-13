#pragma once

#include "RenderWare.h"

class CCustomBuildingRenderer {
public:
    static void InjectHooks();

    CCustomBuildingRenderer();
    ~CCustomBuildingRenderer();

    static bool Initialise();
    static void Shutdown();
    static bool PluginAttach();
    static void AtomicSetup(RpAtomic* atomic);
    static bool IsCBPCPipelineAttached(RpAtomic* atomic);

    // NOTSA: Hook wrapper. The original returns 0 or 1 in the whole EAX, and its callers test all of it (`TEST EAX, EAX` at 0x4C43D5, 0x4C48E1, 0x4C4F3E, 0x533EC0, 0x5A1EB6).
    static uint32 IsCBPCPipelineAttached_Hooked(RpAtomic* atomic) { return IsCBPCPipelineAttached(atomic) ? 1u : 0u; }
    static void UpdateDayNightBalanceParam();
    static void Update();
};
