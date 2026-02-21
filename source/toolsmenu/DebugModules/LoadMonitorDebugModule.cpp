#include <StdInc.h>
#include "LoadMonitorDebugModule.hpp"
#include "LoadMonitor.h"

namespace notsa {
namespace debugmodules {

void LoadMonitorDebugModule::RenderWindow() {
    const ::notsa::ui::ScopedWindow window{ "Load Monitor", {}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }

    auto& lm = g_LoadMonitor;

    if (ImGui::CollapsingHeader("General State", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("In Frame: %u", lm.GetInFrame());
        ImGui::Text("Use Load Monitor: %u", lm.GetUseLoadMonitor());
        ImGui::Text("Force Proc Level: %d", static_cast<int>(lm.GetForceProcLevel()));
        ImGui::Text("Display Type: %d", static_cast<int>(lm.GetDisplayType()));
        ImGui::Text("Var Console Display Type: %d", static_cast<int>(lm.GetVarConsoleDisplayType()));
        ImGui::Text("Proc Level: %d", static_cast<int>(lm.GetProcLevel()));
        ImGui::Text("Proc Level To Force: %d", static_cast<int>(lm.GetProcLevelToForce()));

        bool ambientCrime = lm.IsAmbientCrimeEnabled();
        if (ImGui::Checkbox("Ambient Crime Enabled", &ambientCrime)) {
            if (ambientCrime) {
                lm.EnableAmbientCrime();
            } else {
                lm.DisableAmbientCrime();
            }
        }
    }

    if (ImGui::CollapsingHeader("Timing", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Last Time: %u ms", lm.GetLastTime());
        ImGui::Text("Num Frames This Sec: %u", lm.GetNumFramesThisSec());
        ImGui::Text("FPS: %u", lm.GetFPS());
    }

    if (ImGui::CollapsingHeader("Load Types Data")) {
        if (ImGui::BeginTable("LoadTypesTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
            ImGui::TableSetupColumn("Load Type");
            ImGui::TableSetupColumn("Start Time");
            ImGui::TableSetupColumn("Cycles This Frame");
            ImGui::TableSetupColumn("Max Cycles");
            ImGui::TableSetupColumn("Peak Level");
            ImGui::TableHeadersRow();

            for (size_t i = 0; i < +eLoadType::NUM_LOAD_TYPES; ++i) {
                const auto t = static_cast<eLoadType>(i);
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%zu", i);
                ImGui::TableNextColumn();
                ImGui::Text("%u", lm.GetStartTime(t));
                ImGui::TableNextColumn();
                ImGui::Text("%u", lm.GetCyclesThisFrame(t));
                ImGui::TableNextColumn();
                ImGui::Text("%u", lm.GetMaxCycles(t));
                ImGui::TableNextColumn();
                ImGui::Text("%.4f", lm.GetPeakLevel(t));
            }

            ImGui::EndTable();
        }
    }

    if (ImGui::CollapsingHeader("Smoothed & Averaged Values")) {
        if (ImGui::BeginTable("SmoothedTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Load Type");
            ImGui::TableSetupColumn("Smoothed Value");
            ImGui::TableSetupColumn("Normalized Peak Range");
            ImGui::TableSetupColumn("Averaged Cycles/Sec");
            ImGui::TableHeadersRow();

            for (size_t i = 0; i < +eLoadType::NUM_LOAD_TYPES; ++i) {
                const auto t = static_cast<eLoadType>(i);
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%zu", i);
                ImGui::TableNextColumn();
                ImGui::Text("%.4f", lm.GetSmoothedValue(t));
                ImGui::TableNextColumn();
                ImGui::Text("%.4f", lm.GetNormalizedPeakRangeValue(t));
                ImGui::TableNextColumn();
                ImGui::Text("%.4f", lm.GetAveragedCyclesThisSecond(t));
            }

            ImGui::EndTable();
        }
    }

    if (ImGui::CollapsingHeader("Cycles History")) {
        for (size_t i = 0; i < +eLoadType::NUM_LOAD_TYPES; ++i) {
            const auto t = static_cast<eLoadType>(i);
            if (ImGui::TreeNode(reinterpret_cast<void*>(i), "Load Type %zu", i)) {
                for (size_t j = 0; j < 8; ++j) {
                    ImGui::Text("[%zu] = %u", j, lm.GetCyclesHistory(t, j));
                }
                ImGui::TreePop();
            }
        }
    }

    if (ImGui::CollapsingHeader("Graph Data")) {
        ImGui::Text("Current Graph Index: %d", lm.GetCurrentGraphIndex());
    }
}

void LoadMonitorDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra" }, [&] {
        ImGui::MenuItem("Load Monitor", nullptr, &m_IsOpen);
    });
}

}; // namespace debugmodules
}; // namespace notsa
