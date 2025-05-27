#include <StdInc.h>
#include "SoundManagerDebugModule.hpp"
#include <Audio/managers/AESoundManager.h>

namespace ig = ImGui;

namespace notsa { 
namespace debugmodules {
void SoundManagerDebugModule::RenderWindow() {
    const ::notsa::ui::ScopedWindow window{ "SoundManagerDebugModule", {}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }
    RenderSoundsTable();
}

void SoundManagerDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra", "Audio" }, [&] {
        ImGui::MenuItem("Sound Manager", nullptr, &m_IsOpen);
    });
}

void SoundManagerDebugModule::RenderSoundsTable() {
    if (!ImGui::BeginTable(
        "SoundsTable",
        5,
        ImGuiTableFlags_Sortable
        | ImGuiTableFlags_Resizable
        | ImGuiTableFlags_Reorderable
        | ImGuiTableFlags_BordersInnerH
        | ImGuiTableFlags_ScrollY
        | ImGuiTableFlags_SizingFixedFit
        //| ImGuiTableFlags_SortMulti
    )) {
        return;
    }

    ig::TableSetupColumn("#", ImGuiTableColumnFlags_NoResize, 40.f);
    ig::TableSetupColumn("Bank Slot ID", ImGuiTableColumnFlags_NoResize, 60.f);
    ig::TableSetupColumn("Sfx ID", ImGuiTableColumnFlags_NoResize, 60.f);
    ig::TableSetupColumn("Distance", ImGuiTableColumnFlags_NoResize, 90.f);
    ig::TableSetupColumn("Progress", ImGuiTableColumnFlags_NoResize, 90.f);
    //ig::TableSetupColumn("Color", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_NoSort, 10.f);
    //ig::TableSetupColumn("Type", ImGuiTableColumnFlags_NoResize, 120.f);
    //ig::TableSetupColumn("Entity", ImGuiTableColumnFlags_NoResize, 80.f);
    ig::TableHeadersRow();

    for (const auto id : AESoundManager.GetPhysicallyPlayingSoundList()) {
        if (id == -1) {
            continue;
        }

        auto* const sound = &AESoundManager.m_VirtuallyPlayingSoundList[id];

        ImGui::PushID(id);
        ig::BeginGroup();

        // #
        ig::TableNextColumn();
        ig::Text("%d", id);

        // Bank ID
        ig::TableNextColumn();
        ig::Text("%d", sound->m_nBankSlotId);

        // Sfx ID
        ig::TableNextColumn();
        ig::Text("%d", sound->m_nSoundIdInSlot);

        // Distance
        ig::TableNextColumn();
        ig::Text("%.2f m", CVector::Dist(TheCamera.GetPosition(), sound->m_vecCurrPosn));

        // Progress
        ig::TableNextColumn();
        {
            int16 min = 0;
            ig::SliderScalarN("Progress", ImGuiDataType_S16, &sound->m_nCurrentPlayPosition, 1, &min, &sound->m_nSoundLength, "%hd");
        }

        if (ig::IsItemHovered()) {
            char buf[4096]{};
            snprintf(
                buf, sizeof(buf),
                "Stack trace: \n"
                "%s",
                std::to_string(m_SoundsInfo[id].StackTrace).c_str()
            );
            ig::SetTooltip(buf);
        }

        ig::EndGroup();
        ImGui::PopID();
    }

    ig::EndTable();
}
}; // namespace debugmodules
}; // namespace notsa
