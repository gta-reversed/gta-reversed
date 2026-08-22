#pragma once

#include <Base.h>
#include <bitset>
#include <filesystem>

#include <imfilebrowser.h>
#include <reversiblehooks/ReversibleHooks.h>

#include "RListDefs.h"

namespace RHDebugModule {
class HooksExport {
    using HookState = ReversibleHooks::ReversibleHook::TwoWayHookState;
    using HookType  = ReversibleHooks::ReversibleHook::HookType;

private:
    static constexpr const char* POPUP_NAME = "Export Hooks";

public:
    void Render();
    void Open(const RListCategory& toExport, bool exportOwnItemsOnly = false);
    void Close() { m_IsOpen = false; }
    void Reset();

private:
    size_t ExportToFile(const fs::path& path);

private:
    ImGui::FileBrowser m_FileBrowser{
        ImGuiFileBrowserFlags_EnterNewFilename
        | ImGuiFileBrowserFlags_CreateNewDir
        | ImGuiFileBrowserFlags_CloseOnEsc
        | ImGuiFileBrowserFlags_ConfirmOnEnter
        | ImGuiFileBrowserFlags_EditPathString
    };
    bool                           m_IsOpen{};         //!< Whether the export dialog is open
    bool                           m_WasError{};       //!< Whether the last export resulted in an error
    std::string                    m_PopupMessage{};
    const RListCategory*           m_ToExport{};

    bool                           m_OnlyFiltered{};   //!< Export only items that match the current filter (Or all, if no filter)
    std::bitset<+HookState::Count> m_SelectedStates{}; //!< States to export
    std::bitset<+HookType::Count>  m_SelectedTypes{};  //!< Types to export
    bool                           m_OwnItemsOnly{};   //!< Whether to only export items that are directly in the specified category, or also include items in sub-categories
};
}; // namespace RHDebugModule
