#include "StdInc.h"

#include "CustomBuildingRenderer.h"

#include "CustomBuildingDNPipeline.h"
#include "CustomBuildingPipeline.h"

void CCustomBuildingRenderer::InjectHooks() {
    RH_ScopedClass(CCustomBuildingRenderer);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5D7EC0);
    RH_ScopedInstall(Shutdown, 0x5D7EE0);
    RH_ScopedInstall(PluginAttach, 0x5D7EF0);
    RH_ScopedInstall(AtomicSetup, 0x5D7F00);
    RH_ScopedInstall(IsCBPCPipelineAttached, 0x5D7F40);
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
    const auto* const geometry = RpAtomicGetGeometry(atomic);
    if (CCustomBuildingDNPipeline::GetExtraVertColourPtr(geometry) && geometry->preLitLum) {
        CCustomBuildingDNPipeline::CustomPipeAtomicSetup(atomic);
    } else {
        CCustomBuildingPipeline::CustomPipeAtomicSetup(atomic);
    }
}

// 0x5D7F40
bool CCustomBuildingRenderer::IsCBPCPipelineAttached(RpAtomic* atomic) {
    const auto pipelineId = GetPipelineID(atomic);
    if (pipelineId == 0x53F2009C || pipelineId == 0x53F20098)
        return true;
    const auto* const geometry = RpAtomicGetGeometry(atomic);
    return CCustomBuildingDNPipeline::GetExtraVertColourPtr(geometry) && geometry->preLitLum;
}

// 0x5D7F80
void CCustomBuildingRenderer::UpdateDayNightBalanceParam() {
    const auto minutes = CClock::GetMinutesToday();
    if (minutes < 360.0f) {
        CCustomBuildingDNPipeline::m_fDNBalanceParam = 1.0f;
    } else if (minutes < 420.0f) {
        CCustomBuildingDNPipeline::m_fDNBalanceParam = (420.0f - minutes) / 60.0f;
    } else if (minutes < 1200.0f) {
        CCustomBuildingDNPipeline::m_fDNBalanceParam = 0.0f;
    } else if (minutes < 1260.0f) {
        CCustomBuildingDNPipeline::m_fDNBalanceParam = 1.0f - (1260.0f - minutes) / 60.0f;
    } else {
        CCustomBuildingDNPipeline::m_fDNBalanceParam = 1.0f;
    }
}

// 0x5D8050
void CCustomBuildingRenderer::Update() {
    ZoneScoped;

    static auto& magic1 = StaticRef<uint32>(0xC02C14);
    static auto& magic2 = StaticRef<uint32>(0xC02C18);
    UpdateDayNightBalanceParam();
    magic1 = (magic1 + 1) & 15;
    magic2 = 0;
}

