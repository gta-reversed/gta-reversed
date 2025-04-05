#include "StdInc.h"

#include "PoolsDebugModule.h"

#include "EntryExitManager.h"
#include "StuntJumpManager.h"
#include "CustomCarEnvMapPipeline.h"

void PoolsDebugModule::RenderWindow() {
    const notsa::ui::ScopedWindow window{ "Pools Stats", {446.f, 512.f}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }

    if (!ImGui::BeginTable("PoolsDebugModule", 5, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_ContextMenuInBody)) {
        return;
    }

    ImGui::TableSetupColumn("Name");
    ImGui::TableSetupColumn("Size");
    ImGui::TableSetupColumn("Active objects");
    ImGui::TableSetupColumn("Usage (%)");
    ImGui::TableSetupColumn("DealWithNoMemory");
    ImGui::TableHeadersRow();

    const auto Draw = [](auto* pool, const char* name) {

        if (!pool) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s", name);

            ImGui::TableNextColumn();
            ImGui::Text("NOT INITIALIZED");
        } else {
            ImGui::TableNextRow();
            ImGui::PushID(&pool);

            ImGui::TableNextColumn();
            ImGui::Text("%s", name);

            ImGui::TableNextColumn();
            ImGui::Text("%d", pool->GetSize());

            const auto used       = pool->GetNoOfUsedSpaces();
            const auto percentage = (float)(used) / (float)(pool->GetSize());
            ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f * percentage, 1.0f - percentage, 0.0f, 1.0f });

            ImGui::TableNextColumn();
            ImGui::Text("%d", used);

            ImGui::TableNextColumn();
            ImGui::Text("%.1f %%", (double)(percentage) * 100.0); // Avoid MSVC warning

            ImGui::PopStyleColor();

            ImGui::TableNextColumn();
            ImGui::Text(pool->CanDealWithNoMemory() ? "T" : "F");

            ImGui::PopID();
        }
    };

    // CPools
    Draw(CPools::GetPedPool(), "Ped");
    Draw(CPools::GetVehiclePool(), "Vehicle");
    Draw(CPools::GetBuildingPool(), "Building");
    Draw(CPools::GetObjectPool(), "Object");
    Draw(CPools::GetDummyPool(), "Dummy");
    Draw(CPools::GetColModelPool(), "Col Model");
    Draw(CPools::GetTaskPool(), "Task");
    Draw(CPools::GetPedIntelligencePool(), "Ped Intelligence");
    Draw(CPools::GetPtrNodeSingleLinkPool(), "Ptr Node Single Link");
    Draw(CPools::GetPtrNodeDoubleLinkPool(), "Ptr Node Double Link");
    Draw(CPools::GetEntryInfoNodePool(), "Entry Info Node");
    Draw(CPools::GetPointRoutePool(), "Point Route");
    Draw(CPools::GetPatrolRoutePool(), "Patrol Route");
    Draw(CPools::GetEventPool(), "Event");
    Draw(CPools::GetNodeRoutePool(), "Node Route");
    Draw(CPools::GetTaskAllocatorPool(), "Task Allocator");
    Draw(CPools::GetPedAttractorPool(), "Ped Attractor");

    // Other pools
    Draw(CTxdStore::ms_pTxdPool, "TXD");
    Draw(CIplStore::ms_pPool, "IPL");
    Draw(CEntryExitManager::mp_poolEntryExits, "Entry Exits");
    Draw(CStuntJumpManager::mp_poolStuntJumps, "Stunt Jumps");
    Draw(CColStore::ms_pColPool, "Collision");
    Draw(CQuadTreeNode::ms_pQuadTreeNodePool, "Quad Tree Node");
    Draw(CVehicleModelInfo::CVehicleStructure::m_pInfoPool, "Vehicle Structure");

    Draw(CCustomCarEnvMapPipeline::m_gEnvMapPipeMatDataPool, "Env Map Pipe: Material Data");
    Draw(CCustomCarEnvMapPipeline::m_gEnvMapPipeAtmDataPool, "Env Map Pipe: Atomic Data");
    Draw(CCustomCarEnvMapPipeline::m_gSpecMapPipeMatDataPool, "Spec Map Pipe: Material Data");

    ImGui::EndTable();
}

void PoolsDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Stats" }, [&] {
        ImGui::MenuItem("Pools", nullptr, &m_IsOpen);
    });
}
