#include "StdInc.h"

#include <imgui.h>
#include "HooksExport.hpp"

namespace IG = ImGui;

namespace {
template<typename T, size_t N>
void MultiSelectEnumListbox(const char* label, std::bitset<N>& selected) {
    if (!IG::BeginListBox(label)) {
        return;
    }
    const auto ApplyRequests = [&] (ImGuiMultiSelectIO* msIO) {
        for (auto& req : msIO->Requests) {
            const auto SetRange = [&](auto first, auto last) {
                for (size_t i = (size_t)(first); i < (size_t)(last); ++i) {
                    selected.set(i, req.Selected);
                }
            };
            switch (req.Type) {
            case ImGuiSelectionRequestType_SetAll:   SetRange(0, N); break;
            case ImGuiSelectionRequestType_SetRange: SetRange(req.RangeFirstItem, req.RangeLastItem + 1); break;
            }
        }
    };
    ApplyRequests(IG::BeginMultiSelect(ImGuiMultiSelectFlags_NoAutoSelect, (int32)(selected.count()), (int32)(N)));
    for (size_t i = 0; i < N; ++i) {
        IG::SetNextItemSelectionUserData(i);
        IG::Selectable(*EnumToString(static_cast<T>(i)), selected[i]);
    }
    ApplyRequests(IG::EndMultiSelect());
    IG::EndListBox();
}
};

namespace RHDebugModule {
void HooksExport::Render() {
    if (m_IsOpen) {
        IG::OpenPopup(POPUP_NAME);
    }
    if (!ImGui::BeginPopupModal(POPUP_NAME, &m_IsOpen, ImGuiWindowFlags_AlwaysAutoResize)) {
        return;
    }

    // Draw the actual popup content
    {
        MultiSelectEnumListbox<HookType>("Hook Types", m_SelectedTypes);
        MultiSelectEnumListbox<HookState>("Hook States", m_SelectedStates);
        IG::Checkbox("Export only items matching current filter", &m_OnlyFiltered);
        IG::Checkbox("Export only own items (not sub-category items)", &m_OwnItemsOnly);
        if (IG::Button("Select file & export", { -1.f, 0.f })) {
            m_FileBrowser.SetTypeFilters({ ".json" });
            m_FileBrowser.Open();
        }
    }

    // Handle file browser
    m_FileBrowser.Display();
    if (m_FileBrowser.HasSelected()) {
        const auto& path = m_FileBrowser.GetSelected();
        try {
            const auto count = ExportToFile(path);
            m_PopupMessage = std::format(
                "Successfully exported {} hooks to file {}",
                count, fs::canonical(path).string()
            );
            m_WasError = false;
        } catch (const std::exception& e) {
            m_PopupMessage = std::format("[Error]\n{}", e.what());
            m_WasError     = true;
        }
        IG::OpenPopup("Export Result");
        m_FileBrowser.ClearSelected();
    }

    // Popup for export result
    if (IG::BeginPopupModal("Export Result", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        IG::TextUnformatted(m_PopupMessage.c_str());
        if (IG::Button(m_WasError ? "Try again" : "OK")) {
            IG::CloseCurrentPopup();
            if (!m_WasError) {
                Close();
            }
        }
        IG::EndPopup();
    }

    // End main popup
    IG::EndPopup();
}

void HooksExport::Open(const RListCategory& toExport, bool exportOwnItemsOnly) {
    Reset();
    m_IsOpen       = true;
    m_ToExport     = &toExport;
    m_OwnItemsOnly = exportOwnItemsOnly;
}

void HooksExport::Reset() {
    *this = {};
    m_SelectedStates.set();
    m_SelectedTypes.set();
}

size_t HooksExport::ExportToFile(const fs::path& path) {
    ZoneScoped;

    std::ofstream ofs{ path };
    ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);

    json::array_t arr;

    [&](this auto&& Self, const RListCategory& cat) -> void {
        if (!IsMatchingScoreOrNone(cat.MaxFilterScoreOwnItems)) {
            return;
        }
        for (const auto& item : cat.Items) {
            if (!IsMatchingScoreOrNone(item.FilterScore)) {
                continue;
            }
            if (!m_SelectedStates[+item.Ptr->GetState()]) {
                continue;
            }
            if (!m_SelectedTypes[+item.Ptr->GetType()]) {
                continue;
            }
            to_json(arr.emplace_back(), *item.Ptr);
        }
        if (m_OwnItemsOnly) {
            return;
        }
        for (auto& sc : cat.Categories) {
            Self(sc); // Recurse into subcategories
        }
    }(*m_ToExport);

    ofs << arr;

    return arr.size();
}
}; // namespace RHDebugModule
