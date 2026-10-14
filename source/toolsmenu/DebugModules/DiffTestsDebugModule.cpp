#include <StdInc.h>
#include "DiffTestsDebugModule.hpp"

namespace notsa {
namespace debugmodules {
using namespace notsa::difftest;

void DiffTestsDebugModule::Run(size_t idx) {
    m_Results[idx] = RunTest(GetRegisteredTests()[idx], static_cast<uint32>(m_Seed));
}

void DiffTestsDebugModule::RenderWindow() {
    const ::notsa::ui::ScopedWindow window{ "Diff tests", { 700.f, 450.f }, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }

    const auto& tests = GetRegisteredTests();
    m_Results.resize(tests.size());

    ImGui::TextWrapped(
        "Compares reversed functions with the original game code. The functions under test are un-hooked while their test runs. "
        "Some tests need a loaded game (world, peds, vehicles) - run them in-game."
    );

    if (ImGui::Button("Run all")) {
        for (size_t i = 0; i < tests.size(); i++) {
            Run(i);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Run failed")) {
        for (size_t i = 0; i < tests.size(); i++) {
            if (m_Results[i].Status == TestStatus::Failed) {
                Run(i);
            }
        }
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(120.f);
    ImGui::InputInt("Seed", &m_Seed, 1, 100, ImGuiInputTextFlags_CharsHexadecimal);
    ImGui::SameLine();
    m_NameFilter.Draw("Filter", 180.f);

    size_t passed{}, failed{}, skipped{};
    for (const auto& r : m_Results) {
        switch (r.Status) {
        case TestStatus::Passed:  passed++;  break;
        case TestStatus::Failed:  failed++;  break;
        case TestStatus::Skipped: skipped++; break;
        default:                  break;
        }
    }
    ImGui::Text("%zu tests | passed: %zu | failed: %zu | skipped: %zu", tests.size(), passed, failed, skipped);
    ImGui::Separator();

    if (!ImGui::BeginTable("DiffTests", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg)) {
        return;
    }
    ImGui::TableSetupColumn("Run", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Checks", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Test", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();

    for (auto&& [i, test] : rngv::enumerate(tests)) {
        if (!m_NameFilter.PassFilter(test.Name.data(), test.Name.data() + test.Name.size())) {
            continue;
        }
        const auto& result = m_Results[i];

        ImGui::PushID(static_cast<int>(i));
        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        if (ImGui::SmallButton("Run")) {
            Run(i);
        }

        ImGui::TableNextColumn();
        const auto color = [&] {
            switch (result.Status) {
            case TestStatus::Passed:  return ImVec4{ 0.3f, 0.9f, 0.3f, 1.f };
            case TestStatus::Failed:  return ImVec4{ 1.0f, 0.3f, 0.3f, 1.f };
            case TestStatus::Skipped: return ImVec4{ 1.0f, 0.8f, 0.2f, 1.f };
            default:                  return ImVec4{ 0.7f, 0.7f, 0.7f, 1.f };
            }
        }();
        ImGui::TextColored(color, "%s", StatusToString(result.Status));

        ImGui::TableNextColumn();
        if (result.Status != TestStatus::NotRun) {
            ImGui::Text("%zu/%zu", result.NumChecks - result.NumFailures, result.NumChecks);
        }

        ImGui::TableNextColumn();
        const auto treeOpen = ImGui::TreeNodeEx(test.Name.data(), result.Status == TestStatus::Failed ? ImGuiTreeNodeFlags_DefaultOpen : 0, "%.*s", (int)test.Name.size(), test.Name.data());
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s:%u", test.Location.file_name(), test.Location.line());
        }
        if (treeOpen) {
            if (test.RequiresLoadedGame) {
                ImGui::BulletText("Requires a loaded game");
            }
            for (const auto hook : test.Hooks) {
                ImGui::BulletText("Hook: %.*s", (int)hook.size(), hook.data());
            }
            if (result.Status != TestStatus::NotRun) {
                ImGui::BulletText("Seed: 0x%X | %.2f ms", result.Seed, result.DurationMs);
            }
            for (const auto& note : result.Notes) {
                ImGui::BulletText("%s", note.c_str());
            }
            for (const auto& msg : result.Messages) {
                ImGui::TextColored({ 1.f, 0.5f, 0.5f, 1.f }, "%s", msg.c_str());
            }
            if (result.NumFailures > result.Messages.size()) {
                ImGui::Text("... and %zu more failures (see log)", result.NumFailures - result.Messages.size());
            }
            ImGui::TreePop();
        }

        ImGui::PopID();
    }
    ImGui::EndTable();
}

void DiffTestsDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Tools" }, [&] {
        ImGui::MenuItem("Diff tests", nullptr, &m_IsOpen);
    });
}
}; // namespace debugmodules
}; // namespace notsa
