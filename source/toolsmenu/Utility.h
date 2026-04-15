#pragma once

#include <string_view>
#include <ranges>

#include "TristateCheckbox.h"

struct ImGuiTextFilter;

// Always returns `true` if either strings are empty (This matches the behaviour of `haystack.find(needle) != npos`)
static bool StringContainsString(std::string_view haystack, std::string_view needle, bool caseSensitive = true) {
    if (caseSensitive) {
        return haystack.find(needle) != std::string_view::npos;
    }
    if (haystack.empty() || needle.empty()) {
        return true;
    }
    const auto ToUpper = [](auto&& c) { return (char)std::toupper((unsigned char)c); };
    return !std::ranges::search(haystack, needle, {}, ToUpper, ToUpper).empty();
}

// https://stackoverflow.com/a/48403210
// Maybe get rid of this in C++23 :D
static auto SplitStringView(std::string_view str, std::string_view delim) {
    return str
        | std::ranges::views::split(delim)
        | std::ranges::views::transform([](auto&& rng) {
#if 0 // C++23 - String view from range
            return std::string_view(rng.begin(), rng.end());
#else
            // Workaround for MS-STL debug assert when trying to add to string's end iterator
            const auto size = std::ranges::distance(rng);
            return size ? std::string_view(&*rng.begin(), size) : "";
#endif
        });
}

namespace notsa {
namespace ui {
struct ScopedWindow {
    ScopedWindow(const char* name, ImVec2 defaultSize, bool& open, ImGuiWindowFlags flags = 0) :
        m_needsEnd{open}
    {
        if (open) {
            ImGui::SetNextWindowSize(defaultSize, ImGuiCond_FirstUseEver);
            ImGui::Begin(name, &open, flags);
        }
    }

    ~ScopedWindow() {
        if (m_needsEnd) {
            ImGui::End();
        }
    }

private:
    bool m_needsEnd{};
};

struct ScopedChild {
    template<typename... Ts>
    ScopedChild(Ts&&... args) {
        ImGui::BeginChild(std::forward<Ts>(args)...);
    }

    ~ScopedChild() { ImGui::EndChild(); }
};

template<typename T>
struct ScopedID {
    ScopedID(T id) { ImGui::PushID(id); }
    ~ScopedID() { ImGui::PopID(); }
};

struct ScopedDisable {
    ScopedDisable(bool disable) { ImGui::BeginDisabled(disable); }
    ~ScopedDisable()            { ImGui::EndDisabled(); }
};

//! Render a nested menu (A series of `BeginMenu` calls). If all `BeginMenu` calls return `true` the provided `OnAllVisibleFn` is called.
template<rng::input_range R, typename T>
inline void DoNestedMenu(R&& menuPath, T OnAllVisibleFn) {
    assert(menuPath.size() > 0); // Empty makes no sense

    int32 nopen{};
    for (auto name : menuPath) {
        if (!ImGui::BeginMenu(name)) {
            break;
        }
        nopen++;
    }
    if (nopen == rng::size(menuPath)) {
        std::invoke(OnAllVisibleFn);
    }
    while (nopen--) {
        ImGui::EndMenu();
    }
}

//! Initializer list version of `DoNestedMenu` (So no ugly `std::to_array` has to be used)
template<typename T>
inline void DoNestedMenuIL(std::initializer_list<const char*> menuPath, T OnAllVisibleFn) {
    DoNestedMenu(menuPath, OnAllVisibleFn);
}

/*!
* A tree-node with a tri-state checkbox on the left.
* Make sure to always call `TreePop();` after it!
* @param label      Label of the tree node
* @param cbTriState CheckBox's TriState
* @param cbDisabled Whenever the checkbox of the tree-node is cbDisabled [To make treenode disabled too, use `ScopedDisable`]
* @returns Tuple: [tree node open, checkbox state changed, checkbox state]
*/
inline auto TreeNodeWithTriStateCheckBox(const char* label, ImGui::ImTristate cbTriState, bool cbDisabled, ImGuiTreeNodeFlags treeNodeFlags = 0) {
    // TODO/NOTE: The Tree's label is a workaround for when the label is shorter than the visual checkbox (otherwise the checkbox can't be clic
    const auto open = ImGui::TreeNodeEx("##         ", treeNodeFlags | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth);
        
    ImGui::SameLine();

    // Tree is never cbDisabled, otherwise it couldn't be opened
    ScopedDisable disabler{ cbDisabled };

    // Checkbox + it's label will be the tree name
    bool cbState{};
    const auto stateChanged = ImGui::CheckboxTristate(label, cbTriState, cbState);

    return std::make_tuple(open, stateChanged, cbState);
};
}; // namespace ui
}; // namespace notsa

/*!
* @brief ImGuiTextFilter serialization
*/
inline void to_json(json& j, const ImGuiTextFilter& filter) {
    j = json{
        { "InputBuf", filter.InputBuf }
    };
}
inline void from_json(const json& j, ImGuiTextFilter& filter) {
    const auto buf = j.at("InputBuf").get<std::string_view>();
    strncpy(filter.InputBuf, buf.data(), std::min(sizeof(filter.InputBuf) - 1, buf.size()));
    filter.Build();
}

