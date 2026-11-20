#include "StdInc.h"

#include "CustomBuildingRenderer.h"

#include "CustomBuildingDNPipeline.h"
#include "CustomBuildingPipeline.h"

// Written by 0x5D6830 (inlined into `Update` here) and read by `CCustomBuildingDNPipeline::PreRenderUpdate`
static auto& s_DNUpdateSlot    = StaticRef<uint32>(0xC02C14);
static auto& s_DNUpdateAllFlag = StaticRef<uint32>(0xC02C18);

// NOTSA: What 0x5D6E90 does: it returns the first dword of the geometry's extra vertex colour plugin data (`ExtraVertColour::NightColors`), not its address.
// The original tests it together with the geometry's prelit colours to tell whether the model has night vertex colours.
static bool GeometryHasNightColours(RpGeometry* geometry) {
    return CCustomBuildingDNPipeline::GetExtraVertColourPtr(geometry)->NightColors && geometry->preLitLum;
}

void CCustomBuildingRenderer::InjectHooks() {
    RH_ScopedClass(CCustomBuildingRenderer);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5D7EC0);
    RH_ScopedInstall(Shutdown, 0x5D7EE0);
    RH_ScopedInstall(PluginAttach, 0x5D7EF0);
    RH_ScopedInstall(AtomicSetup, 0x5D7F00);
    RH_ScopedNamedInstall(IsCBPCPipelineAttached_Hooked, "IsCBPCPipelineAttached", 0x5D7F40);
    RH_ScopedInstall(UpdateDayNightBalanceParam, 0x5D7F80);
    RH_ScopedInstall(Update, 0x5D8050);
}

// 0x5D7EC0
bool CCustomBuildingRenderer::Initialise() {
    auto pipe = CCustomBuildingPipeline::CreatePipe();
    if (pipe)
        return CCustomBuildingDNPipeline::CreatePipe();

    return pipe;
}

// 0x5D7EE0
void CCustomBuildingRenderer::Shutdown() {
    CCustomBuildingPipeline::DestroyPipe();
    CCustomBuildingDNPipeline::DestroyPipe();
}

// 0x5D7EF0
bool CCustomBuildingRenderer::PluginAttach() {
    return CCustomBuildingDNPipeline::ExtraVertColourPluginAttach();
}

// 0x5D7F00
void CCustomBuildingRenderer::AtomicSetup(RpAtomic* atomic) {
    if (GeometryHasNightColours(atomic->geometry)) {
        CCustomBuildingDNPipeline::CustomPipeAtomicSetup(atomic);
    } else {
        CCustomBuildingPipeline::CustomPipeAtomicSetup(atomic);
    }
}

// 0x5D7F40
bool CCustomBuildingRenderer::IsCBPCPipelineAttached(RpAtomic* atomic) {
    // Pipeline ids the original sets in `CCustomBuildingPipeline::CustomPipeAtomicSetup` (0x5D7E50) and `CCustomBuildingDNPipeline::CustomPipeAtomicSetup` (0x5D71C0)
    constexpr uint32 BUILDING_PIPELINE_ID = 0x53F2009C, BUILDING_DN_PIPELINE_ID = 0x53F20098;

    const auto id       = GetPipelineID(atomic);
    const auto geometry = atomic->geometry; // Read after the call, without a check, like the original
    if (id == BUILDING_PIPELINE_ID || id == BUILDING_DN_PIPELINE_ID) {
        return true;
    }
    return GeometryHasNightColours(geometry);
}

// 0x5D7F80
void CCustomBuildingRenderer::UpdateDayNightBalanceParam() {
    // The original multiplies by 1/60 (.rdata 0x859044 = 0x3C888889) everywhere instead of dividing by 60.
    // NOTE: All operands are small integers or results of single float ops here, so plain float math rounds exactly like the x87 (24-bit precision) code.
    constexpr auto ONE_OVER_60 = 1.0f / 60.0f;

    // Inlined minutes-of-day, NOT CClock::GetMinutesToday (0x55F470): the original sums the integer `hours * 60 + minutes` first (FILD),
    // then adds `seconds * (1/60)`
    const auto minutes = (float)(CClock::ms_nGameClockHours * 60 + CClock::ms_nGameClockMinutes) + (float)CClock::ms_nGameClockSeconds * ONE_OVER_60;

    auto& balance      = CCustomBuildingDNPipeline::m_fDNBalanceParam;
    if (minutes < 360.0f) {
        balance = 1.0f;
    } else if (minutes < 420.0f) {
        balance = (420.0f - minutes) * ONE_OVER_60;
    } else if (minutes < 1200.0f) {
        balance = 0.0f;
    } else if (minutes < 1260.0f) {
        balance = 1.0f - (1260.0f - minutes) * ONE_OVER_60;
    } else {
        balance = 1.0f;
    }
}

// 0x5D8050
void CCustomBuildingRenderer::Update() {
    ZoneScoped;

    UpdateDayNightBalanceParam();

    // 0x5D6830 (argument 0), inlined
    s_DNUpdateSlot    = (s_DNUpdateSlot + 1) & 15;
    s_DNUpdateAllFlag = 0;
}
