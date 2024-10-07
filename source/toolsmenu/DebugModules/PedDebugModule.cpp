#include "StdInc.h"

#include <list>
#include <vector>
#include <optional>
#include <format>

#include "extensions/enumerate.hpp"
#include "PedDebugModule.h"
#include "Pools.h"
#include "TaskManager.h"
#include "Hud.h"

// Define extra conversion function from our vector type to imgui's vec2d
#include <imgui.h>

using namespace ImGui;

struct PedInfo {
    CPed*   ped{};
    CVector posWorld{};
    CVector posScreen{};
};

//! General tab
namespace GeneralTab {
void ProcessPed(CPed& ped) {
    if (BeginTabItem("General")) {
        // Don't allow CJ to be removed kekw
        if (FindPlayerPed() != &ped) {
            if (Button("Remove")) {
                CPopulation::RemovePed(&ped);
            }
        }
        SameLine();
        if (Button("Catapult")) {
            const auto entity = ped.IsInVehicle() ? (CPhysical*)ped.m_pVehicle : &ped;
            entity->ApplyMoveForce(entity->GetUpVector() * 10000.f);
        }

        Text("Pool ID: %d", GetPedPool()->GetIndex(&ped));
        Text("Skin Model: %d", ped.m_nModelIndex);

        const auto& pos = ped.GetPosition();
        Text("Position: %.3f, %.3f, %.3f", pos.x, pos.y, pos.z);

        EndTabItem();
    }
}
};

//! Tasks tab
namespace TasksTab {

//! Process a single task
void ProcessTask(CTask* task, std::optional<size_t> idx) {
    const auto taskType = task->GetTaskType();

    const auto DoTreeNode = [&] {
        return idx // If set, this is a root task
            ? TreeNodeEx(task, ImGuiTreeNodeFlags_DefaultOpen, "%i: %s", (int)*idx, GetTaskTypeName(taskType)) // For root tasks show index, eventually as a string from `ePrimaryTasks` and `eSecondaryTask`
            : TreeNodeEx(GetTaskTypeName(taskType), ImGuiTreeNodeFlags_DefaultOpen);
    };

    PushID((int)taskType);
    if (DoTreeNode()) {
        if (const auto sub = task->GetSubTask()) {
            ProcessTask(sub, std::nullopt);
        }
        TreePop();
    }
    PopID();
}

//! Process category, eg.: secondary or primary
void ProcessTaskCategory(const char* label, const auto& tasks) {
    if (TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen)) {
        for (const auto& [idx, task] : notsa::enumerate(tasks)) {
            if (task) {
                ProcessTask(task, idx);
            }
        }
        TreePop();
    }
}

//! Process a single ped. This call is done within an imgui tab 
void ProcessPed(CPed& ped) {
    if (BeginTabItem("Tasks")) {
        auto& taskMgr = ped.GetTaskManager();
        ProcessTaskCategory("Primary", taskMgr.GetPrimaryTasks());
        ProcessTaskCategory("Secondary", taskMgr.GetSecondaryTasks());        
        EndTabItem();
    }
}
};

void PedDebugModule::ProcessPed(PedInfo& pi) {
    if (!pi.ped->GetIsOnScreen()) {
        return;
    }

    const auto remap = [](auto value, auto low1, auto high1, auto low2, auto high2) {
        return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
    };
    
    const auto depth = pi.posScreen.z;

    if (depth > 100.f) {
        return; // Too distant
    }

    SetNextWindowPos({ pi.posScreen.x, pi.posScreen.y });
    const auto size = CVector2D{ 600, 400 } / remap(depth, 0.f, 100.f, 1.f, 4.f);
    SetNextWindowSize({size.x, size.y});
    //SetNextWindowSize({ 600, 400 }, ImGuiCond_FirstUseEver);

    // Format a title with a custom ID that should hopefully match only this ped
    char title[1024];
    *std::format_to(title, "Ped Debug###{}{}", (ptrdiff_t)(pi.ped), pi.ped->m_nRandomSeed) = 0; // Null terminate :D

    if (m_AutoCollapseEnabled) {
        SetNextWindowCollapsed((pi.ped->GetPosition() - TheCamera.GetPosition()).SquaredMagnitude() >= sq(m_CollapseToggleDist));
    } else {
        SetNextWindowCollapsed(true, ImGuiCond_Once);
    }

    if (Begin(title, nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing)) {
        if (BeginTabBar("##PedBar")) {
            TasksTab::ProcessPed(*pi.ped);
            GeneralTab::ProcessPed(*pi.ped);
            EndTabBar();
        }
    }
    End();
}

// Gotta use this function because `Update` is outside a frame context
void PedDebugModule::RenderWindow() {
    if (!m_IsVisible) {
        return;
    }

    // https://stackoverflow.com/a/60971856
    const auto to_vector = []<rng::range R>(R&& r) {
        using elem_t = std::decay_t<rng::range_value_t<R>>;
        return std::vector<elem_t>{r.begin(), r.end()};
    };

    auto peds = to_vector(
            GetPedPool()->GetAllValid()
        | rng::views::transform([this](CPed& ped) -> std::optional<PedInfo> {
            if (!ped.GetIsOnScreen()) {
                return std::nullopt;
            }

            PedInfo pi{
                .ped = &ped,
                .posWorld = ped.GetPosition()
            };

            if (!CalcScreenCoors(ped.GetBonePosition(BONE_HEAD) + ped.GetRightVector() * 0.5f, pi.posScreen)) {
                DEV_LOG("Failed to calculate on-screen coords of ped");
                return std::nullopt;
            }
               
            if (pi.posScreen.z >= m_DrawDist) { // posScreen.z == depth == distance from camera
                return std::nullopt;
            }

            return pi;
        })
        | rng::views::filter([](auto&& optPI) { return optPI.has_value(); })
        | rng::views::transform([](auto&& optPI) { return *optPI; })
    );
    // Sort by depth (furthest away first, this way when windows are rendered the closest ped's will be drawn last => on top of everythign else)
    rng::sort(peds, std::greater<>{}, [](const PedInfo& pi) -> float { return pi.posScreen.z; });

    for (PedInfo& pi : peds) {
        ProcessPed(pi);
    }
}

void PedDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Visualization", "Peds" }, [&] {
        Checkbox("Enable", &m_IsVisible);

        const notsa::ui::ScopedDisable disable1{ !m_IsVisible };
        SliderFloat("Draw distance", &m_DrawDist, 4.f, 300.f); // Realistically GTA won't generate peds even at 200 units

        Checkbox("Auto-Collapse", &m_AutoCollapseEnabled);
        SameLine();
        const notsa::ui::ScopedDisable disable2{ !m_AutoCollapseEnabled };
        SliderFloat("Distance", &m_CollapseToggleDist, 4.f, 300.f);
    });
}
