#include "StdInc.h"

#include <string>
#include <chrono>
#include <optional>
#include <format>

#include <imgui.h>
#include <libs/imgui/misc/cpp/imgui_stdlib.h>
#include <imfilebrowser.h>

#include <TristateCheckbox.h>

#include <extensions/CustomFormatters.hpp>
#include <reversiblehooks/ReversibleHooks.h>
#include <reversiblehooks/HookCategory.h>

#include "Utility.h"
#include "HooksDebugModule.h"

#include "RListFilterer.h"
#include "RListSorter.hpp"

using namespace RHDebugModule;
using namespace ImGui;

constexpr ImVec2 STATE_BUTTON_SIZE{ 80.f, 0.f };
constexpr auto   FILTER_INPUT_DEBOUNCE_TIME = std::chrono::milliseconds{ 250 };

namespace RHDebugModule {
HooksDebugModule::HooksDebugModule() :
    m_FilterProcessor{ .Thread{ [this] { FilteringThread(); } } }
{}

HooksDebugModule::~HooksDebugModule() {
    m_FilterProcessor.Exiting = true;
    m_FilterProcessor.CV.notify_one();
    m_FilterProcessor.Thread.join();
}

void HooksDebugModule::FilteringThread() {
    while (!m_FilterProcessor.Exiting) {
        std::unique_lock lock{ m_FilterProcessor.Mtx };
        m_FilterProcessor.CV.wait(lock);
        if (m_FilterProcessor.Exiting) {
            break;
        }
        if (!m_RenderList.RootCategory) {
            continue; // Nothing to filter
        }
        /* Hold lock until we finish */
        NOTSA_LOG_DEBUG("Running filter");
        const auto now = FilterClock::now();
        RListFilterer{ std::move(m_FilterProcessor.HookFilter) }.Process(*m_RenderList.RootCategory);
        RListSorter{}.Process(*m_RenderList.RootCategory);
        m_FilterProcessor.FinishedAt        = FilterClock::now();
        m_FilterProcessor.NeedToAckFinished = true;
    }
}

bool HooksDebugModule::RunFilter() {
    if (!m_RenderList.RootCategory) {
        return false; // Nothing to filter
    }
    {
        std::unique_lock lock{ m_FilterProcessor.Mtx, std::try_to_lock };
        if (!lock.owns_lock()) {
            return false; // Filter still running
        }
        m_FilterProcessor.HookFilter = { m_Filter.Input, m_Filter.CaseSensitive, m_Filter.Cutoffs };
        m_FilterProcessor.StartedAt  = FilterClock::now();
    }
    m_FilterProcessor.CV.notify_one();
    return true;
}

void HooksDebugModule::CheckNeedsToRunFilter() {
    if (std::exchange(m_Filter.Changed, false)) {
        m_Filter.RunAt = FilterClock::now() + FILTER_INPUT_DEBOUNCE_TIME;
    } else if (m_Filter.RunAt.has_value() && *m_Filter.RunAt < FilterClock::now()) {
        if (RunFilter()) {
            m_Filter.RunAt = std::nullopt;
        }
    }
}

void HooksDebugModule::RenderFilter() {
    SetNextItemWidth(-1.f);
    m_Filter.Changed |= InputText("##FilterInput", &m_Filter.Input);
    SetItemTooltip(
        "`cpy`                - Filter by function\n"
        "`cped::`             - Filter by category name\n"
        "`ped/player`         - Filter by category path, showing all items\n"
        "`::function`         - Filter by function name, any category\n"
        "`ped/player::busted` - Filter by both category path and function name\n"
        "`/entity`            - Filter relative to root, must match whole path\n"
        "Use `*` as wildrcard\n"
        "For more tips see gta-reversed-modern/discussions/190\n"
    );
}

template<std::predicate<HookState> SetStateFn>
bool HooksDebugModule::HandleSlideSetterForItem(std::optional<HookState> state, HookState next, SetStateFn&& SetState) {
    switch (m_SlideSetter.Mode) {
    case SlideSetter::Mode::NONE: {
        return false;
    }
    case SlideSetter::Mode::PICK_THEN_SET: {
        if (state.has_value()) {
            m_SlideSetter.StateToSet = *state;
            m_SlideSetter.Mode       = SlideSetter::Mode::SET;
        }
        return false;
    }
    case SlideSetter::Mode::SET: {
        return SetState(m_SlideSetter.StateToSet);
    }
    case SlideSetter::Mode::TOGGLE: {
        return SetState(next);
    }
    default: NOTSA_UNREACHABLE_CASE(m_SlideSetter.Mode);
    }
}

void HooksDebugModule::UpdateSlideSetterMode() {
    const auto next =
        IsMouseDown(ImGuiMouseButton_Middle)  ? SlideSetter::Mode::TOGGLE
        : IsMouseDown(ImGuiMouseButton_Right) ? SlideSetter::Mode::PICK_THEN_SET
                                              : SlideSetter::Mode::NONE;

    if (m_SlideSetter.Mode == next) {
        return;
    }

    if (next != SlideSetter::Mode::NONE) {
        if (m_SlideSetter.Mode == SlideSetter::Mode::SET && next == SlideSetter::Mode::PICK_THEN_SET) {
            return; // Already setting
        }
    }

    m_SlideSetter = { .Mode = next };
}

const char* StateToString(HookState state) {
    switch (state) {
    case HookState::Unhooked:       return "unhooked";
    case HookState::RedirectToGTA:  return "gta";
    case HookState::RedirectToOurs: return "our";
    default:                        return "unk";
    }
}

const char* GetTypeSymbolUI(const ReversibleHooks::HookCategoryItem& i) noexcept {
    using enum ReversibleHooks::ReversibleHook::HookType;
    switch (i.GetType()) {
    case StaticTwoWay:      return "S";
    case Virtual:           return "V";
    case VirtualDestructor: return "VD";
    case VMTRedirect:       return "VR";
    default:                return "U";
    }
}

template<
    std::predicate<HookState> SetStateFn,
    std::predicate<>          RestoreStateFn,
    std::invocable            RenderExtraCtxMenuItemsFn>
bool HooksDebugModule::StateChanger(
    const char*                 title,
    bool                        disabled,
    ImTristate                  onOffCheckboxState,
    std::optional<HookState>    current,
    HookState                   next,
    SetStateFn&&                SetState,
    RestoreStateFn&&            RestoreState,
    RenderExtraCtxMenuItemsFn&& RenderExtraCtxMenuItems
) {
    notsa::ui::ScopedID      idg{ "StateChanger" };
    notsa::ui::ScopedDisable sdg{ disabled };

    bool changed = false;

    SameLine();
    BeginGroup();
    {
        Selectable("##selectable", false, ImGuiSelectableFlags_AllowItemOverlap);

        if (BeginPopupContextItem("state_context")) {
            if (MenuItem("Set to Our")) {
                changed |= SetState(HookState::RedirectToOurs);
            }
            if (MenuItem("Set to GTA")) {
                changed |= SetState(HookState::RedirectToGTA);
            }
            if (MenuItem("Set to Unhooked")) {
                changed |= SetState(HookState::Unhooked);
            }
            if (MenuItem("Restore previous state")) {
                changed |= RestoreState();
            }
            RenderExtraCtxMenuItems();
            EndPopup();
        }

        SameLine(); 
        if (bool checked; CheckboxTristate("##on-off", onOffCheckboxState, checked)) {
            if (checked) {
                changed |= RestoreState();
            } else {
                changed |= SetState(HookState::Unhooked);
            }
        }

        const auto a = (int32)(GetStyle().Colors[ImGuiCol_Button].w * 255.f);
        PushStyleColor(
            ImGuiCol_Button,
            current.transform([a] (HookState state) {
                switch (state) {
                case HookState::Unhooked:       return IM_COL32(127, 0, 0, a);  // Red
                case HookState::RedirectToGTA:  return IM_COL32(69, 69, 69, a); // Dark gray
                case HookState::RedirectToOurs: return IM_COL32(0, 127, 0, a);  // Green
                default:                        NOTSA_UNREACHABLE_CASE(state);
                }
            }).value_or(IM_COL32(127, 127, 0, a)) // yellow for mixed
        );

        SameLine();
        if (Button(current.has_value() ? StateToString(*current) : "mixed", STATE_BUTTON_SIZE) && !disabled) {
            changed |= SetState(next);
        }
        if (IsItemHovered()) {
            const auto id = ImGui::GetID("setter");
            if (m_SlideSetter.LastUsedOnID != id) {
                const auto used = HandleSlideSetterForItem(current, next, SetState);
                if (used) {
                    m_SlideSetter.LastUsedOnID = id;
                }
                changed |= used;
            }
            SetTooltip(
                "Left click: Redirect to Our/GTA code\n"
                "Middle click: Toggle (Slide setter)\n"
                "Right click + hold: Slide setter (Enable/disable all hovered items)\n"
            );
        }
        PopStyleColor();

        SameLine();
        TextUnformatted(title);
    }
    EndGroup();

    return changed;
}

void HooksDebugModule::RenderMenuBar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Tools")) {
            if (ImGui::MenuItem("Export Hooks", "CTRL + S")) {
                if (m_RenderList.RootCategory) { // Should never be null here, but just in case
                    m_HooksExport.Open(*m_RenderList.RootCategory);
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Options")) {
            if (ImGui::BeginMenu("Filter")) {
                m_Filter.Changed |= Checkbox("Case Sensitive", &m_Filter.CaseSensitive);
                if (BeginMenu("Cutoffs")) {
                    const auto CutoffSlider = [&](float* value, const char* name) {
                        m_Filter.Changed |= SliderFloat(name, value, 0.f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                    };
                    CutoffSlider(&m_Filter.Cutoffs.CategoryInPath, "CategoryInPath");
                    CutoffSlider(&m_Filter.Cutoffs.CategoryGlobal, "CategoryGlobal");
                    CutoffSlider(&m_Filter.Cutoffs.ItemGlobal, "ItemGlobal");
                    CutoffSlider(&m_Filter.Cutoffs.ItemLocal, "ItemLocal");
                    CutoffSlider(&m_Filter.Cutoffs.ItemAddress, "ItemAddress");
                    if (Button("Reset Cutoffs")) {
                        m_Filter.Cutoffs = {};
                        m_Filter.Changed = true;
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Tree")) {
                bool changed = false;
                changed |= ImGui::Checkbox("Display item address", &m_RenderList.BuilderOpts.DisplayTitleWithItemAddress);
                changed |= ImGui::Checkbox("Display filter scores", &m_RenderList.BuilderOpts.DisplayTitleWithFilterScores);
                if (changed) {
                    if (m_RenderList.RootCategory) { // Shouldn't be null here, but just in case
                        m_RenderList.Builder.UpdateList(*m_RenderList.RootCategory, m_RenderList.BuilderOpts);
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

bool HooksDebugModule::RenderCategoryItems(RListCategory& cat) {
    if (!IsMatchingScoreOrNone(cat.MaxFilterScoreOwnItems)) {
        return false;
    }
    bool changed = false;
    for (auto& item : cat.Items) {
        if (!IsMatchingScoreOrNone(item.FilterScore)) {
            continue;
        }
        notsa::ui::ScopedID idg{ item.Ptr->GetName() };

        // Draw hook symbol
        {
            PushStyleVar(ImGuiStyleVar_Alpha, GetStyle().Alpha * 0.5f);
            AlignTextToFramePadding();
            Text(GetTypeSymbolUI(*item.Ptr));
            PopStyleVar();
        }

        // State checkbox
        changed |= StateChanger(
            item.DisplayTitle.c_str(),
            item.Ptr->GetIsStateLocked(),
            item.Ptr->GetState() == HookState::Unhooked
                ? ImTristate::NONE
                : ImTristate::ALL,
            item.Ptr->GetState(),
            item.Ptr->GetState() == HookState::RedirectToOurs
                ? HookState::RedirectToGTA
                : HookState::RedirectToOurs,
            [&] (HookState s) { return item.Ptr->SetState(s); },
            [&] { return item.Ptr->SetToPreviousState(); },
            [&] {
                if (MenuItem("Copy name")) {
                    SetClipboardText(item.Ptr->GetName().c_str());
                }
                const auto Addr2Clipboard = [](void* addr) {
                    SetClipboardText(std::format("{}", addr).c_str());
                };
                if (MenuItem("Copy our address")) {
                    Addr2Clipboard(item.Ptr->GetHookAddressOur());
                }
                if (MenuItem("Copy GTA address")) {
                    Addr2Clipboard(item.Ptr->GetHookAddressGTA());
                }
            }
        );

        if (IsItemHovered()) {
            const auto gta = item.Ptr->GetHookAddressGTA(),
                       our = item.Ptr->GetHookAddressOur();
            const auto AddrToClipboard = [](void* addr) {
                SetClipboardText(std::format("{}", addr).c_str());
            };

            std::string tooltipText = std::format("SA: {} / Our: {}", gta, our);
            if (item.Ptr->GetIsStateLocked()) {
                tooltipText += "\n(locked)";
            }
            SetTooltip(tooltipText.c_str());

            if (IsItemClicked(ImGuiMouseButton_Right)) {
                AddrToClipboard(gta);
            } else if (IsItemClicked(ImGuiMouseButton_Middle)) {
                AddrToClipboard(our);
            }
        }
    }
    return changed;
}


auto HooksDebugModule::RenderCategory(RListCategory& cat) -> RenderCategoryResult {
    if (!cat.Items.IsEmpty() && !cat.Categories.IsEmpty()) {
        return RenderCategoryResult::SKIPPED_NOTHING_TO_SHOW;
    }

    if (!IsMatchingScoreOrNone(cat.MaxFilterScore)) {
        return RenderCategoryResult::SKIPPED_FILTERED;
    }

    const auto hasOwnItemsToShow = !cat.Items.IsEmpty() && IsMatchingScoreOrNone(cat.MaxFilterScoreOwnItems),
               hasSubItemsToShow = !cat.Categories.IsEmpty() && IsMatchingScoreOrNone(cat.MaxFilterScoreSubItems);

    if (!hasOwnItemsToShow && !hasSubItemsToShow) {
        return RenderCategoryResult::SKIPPED_FILTERED;
    }

    notsa::ui::ScopedID idg{ cat.Category->Name() };

    bool changed = false;

    const auto TreeNodeWithCheckbox = [&changed, this](
        const char*                                     label,
        bool                                            disabled,
        bool                                            hasAnyUnhooked,
        ReversibleHooks::HookCategory::CommonItemsState commonState,
        HookState                                       next,
        std::invocable<HookState> auto&&                SetState,
        std::invocable<> auto&&                         RestoreState,
        std::invocable<> auto&&                         OpenExportHooks
    ) {
        // TODO/NOTE: The Tree's label is a workaround for when the label is shorter than the visual checkbox (otherwise the checkbox can't be clicked)
        //const auto open = TreeNodeEx("##         ", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth);

        AlignTextToFramePadding(); 
        const auto open = TreeNodeEx("##node", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_AllowOverlap);

        changed |= StateChanger(
            label,
            disabled,
            commonState == HookState::Unhooked
                ? ImTristate::NONE
                : hasAnyUnhooked
                    ? ImTristate::MIXED
                    : ImTristate::ALL,
            commonState,
            next,
            SetState,
            RestoreState,
            [&] {
                if (MenuItem("Export")) {
                    OpenExportHooks();
                }
            }
        );

        return open;
    };
    
    const auto SetCategoryOwnItemsState = [&](RListCategory& c, HookState state) {
        return rng::fold_left(c.Items, false, [state](bool changed, RListCategoryItem& item) {
            return item.Ptr->SetState(state) | changed;
        });
    };

    const auto RestoreCategoryOwnItemsState = [&](RListCategory& c) {
        return rng::fold_left(c.Items, false, [](bool changed, RListCategoryItem& item) {
            return item.Ptr->SetToPreviousState() | changed;
        });
    };

    //
    // Category tree node
    //
    if (m_FilterProcessor.NeedToAckFinished) {
        SetNextItemOpen(cat.MaxScoreAllItems > 0.f || cat.MaxFilterScoreSubCats > 0.f, ImGuiCond_Always);
    }

    if (TreeNodeWithCheckbox(
        cat.DisplayTitle.c_str(),
        !cat.AnyUnlockedItems,
        cat.AnyUnhookedItems,
        cat.CommonStateAllItems,
        cat.CommonStateAllItems.value_or(cat.LastSetAllItemsState.value_or(HookState::RedirectToOurs)) == HookState::RedirectToOurs
            ? HookState::RedirectToGTA
            : HookState::RedirectToOurs,
        [&] (HookState state) -> bool { // Set state of all items and sub-categories
            cat.LastSetAllItemsState = state;
            return [&, state](this auto&& Self, RListCategory& c) -> bool {
                bool changed = SetCategoryOwnItemsState(c, state);
                for (auto& sc : c.Categories) {
                    changed |= Self(sc) && m_RenderList.Builder.UpdateCategory(sc, m_RenderList.BuilderOpts); // Also update category if it has changed
                }
                return changed;
            }(cat);
        },
        [&] () -> bool { // Restore state of all items and sub-categories
            return [&](this auto&& Self, RListCategory& c) -> bool {
                bool changed = RestoreCategoryOwnItemsState(c);
                for (auto& sc : c.Categories) {
                    changed |= Self(sc) && m_RenderList.Builder.UpdateCategory(sc, m_RenderList.BuilderOpts); // Also update category if it has changed
                }
                return changed;
            }(cat);
        },
        [&] () { // Open export hooks for this category
            m_HooksExport.Open(cat, false);
        }
    )) {
        const auto hasSubCategoriesToShow = !cat.Categories.IsEmpty() && (IsMatchingScoreOrNone(cat.MaxFilterScoreSubCats) || IsMatchingScoreOrNone(cat.MaxFilterScoreSubItems));

        // Draw items (hooks) (if any)
        bool itemsStateChanged = false;
        if (hasOwnItemsToShow) {
            if (hasSubItemsToShow) { // Render a separate tree node that's like a category for the items
                if (m_FilterProcessor.NeedToAckFinished) {
                    if (cat.MaxFilterScoreOwnItems > 0.f) {
                        SetNextItemOpen(cat.MaxFilterScoreOwnItems > 0.f, ImGuiCond_Always);
                    }
                }
                if (TreeNodeWithCheckbox(
                    "Hooks",
                    !cat.AnyUnlockedOwnItems,
                    cat.AnyUnhookedOwnItems,
                    cat.CommonStateOwnItems,
                    cat.CommonStateOwnItems.value_or(cat.LastSetAllItemsState.value_or(HookState::RedirectToOurs)) == HookState::RedirectToOurs
                        ? HookState::RedirectToGTA
                        : HookState::RedirectToOurs,
                    [&] (HookState s) -> bool {
                        cat.LastSetAllItemsState = s;
                        return SetCategoryOwnItemsState(cat, s);
                    },
                    [&] () -> bool {
                        return RestoreCategoryOwnItemsState(cat);
                    },
                    [&] () { // Open export hooks for the items of the category
                        m_HooksExport.Open(cat, true);
                    }
                )) {
                    itemsStateChanged |= RenderCategoryItems(cat);
                    TreePop();
                }
            } else { // If there are no subcategories we can draw all items directly under this node
                itemsStateChanged |= RenderCategoryItems(cat);
            }
        }

        // Draw subcategories
        if (hasSubItemsToShow) {
            for (auto& v : cat.Categories) {
                changed |= RenderCategory(v) == RenderCategoryResult::RENDERED_CATEGORY_STATE_CHANGED;
            }
        }

        // Pop the category's tree node
        TreePop();
    }

    // Now check if we've changed, and if so, check if that change affects the state of the parent
    if (changed) {
        changed &= m_RenderList.Builder.UpdateCategory(cat, m_RenderList.BuilderOpts);
    }        

    return changed
        ? RenderCategoryResult::RENDERED_CATEGORY_STATE_CHANGED
        : RenderCategoryResult::RENDERED;
}

void HooksDebugModule::RenderHooksSection(bool isFiltering) {
    notsa::ui::ScopedChild c{ "HooksScrollableSection", ImVec2(0.0f, -GetFrameHeightWithSpacing()), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar };

    if (isFiltering) {
        notsa::ui::WindowCenteredTextUnformatted("Interaction disabled, filtering in progress...");
    } else {
        switch (RenderCategory(*m_RenderList.RootCategory)) {
        case RenderCategoryResult::RENDERED_CATEGORY_STATE_CHANGED: {
            m_RenderList.Builder.UpdateCategory(*m_RenderList.RootCategory, m_RenderList.BuilderOpts);
            break;
        }
        case RenderCategoryResult::SKIPPED_FILTERED: {
            notsa::ui::WindowCenteredTextUnformatted("No filter results");
            break;
        }
        }
    }

}

void HooksDebugModule::RenderFooter(bool isFiltering) {
    if (isFiltering) {
        TextUnformatted("Status: Filtering...");
    } else {
        Text("Filtering took %lld ms", static_cast<long long>(std::chrono::duration_cast<std::chrono::milliseconds>(m_FilterProcessor.FinishedAt - m_FilterProcessor.StartedAt).count()));
    }
}

void HooksDebugModule::RenderHooksExport() {
    if (Shortcut(ImGuiMod_Ctrl | ImGuiKey_S)) {
        if (m_RenderList.RootCategory) { // Should never be null here, but just in case
            m_HooksExport.Open(*m_RenderList.RootCategory);
        }
    }
    m_HooksExport.Render();
}

void HooksDebugModule::RenderWindow() {
    const notsa::ui::ScopedWindow window{ "ReversibleHooks (TM) (R)", {500.f, 700.f}, m_IsOpen, ImGuiWindowFlags_MenuBar};
    if (!m_IsOpen) {
        return;
    }

    if (!m_RenderList.RootCategory) {
        VERIFY(m_RenderList.RootCategory = m_RenderList.Builder.ConstructList(
            ReversibleHooks::RHManager::GetInstance().GetRootCategory(),
            m_RenderList.BuilderOpts
        ));
    }

    UpdateSlideSetterMode();

    {
        const std::unique_lock lock{ m_FilterProcessor.Mtx, std::try_to_lock };
        const auto isFilteringInProgress = !lock.owns_lock();
        const notsa::ui::ScopedDisable sdg{ isFilteringInProgress };

        if (m_FilterProcessor.NeedToAckFinished) {
            if (m_RenderList.BuilderOpts.DisplayTitleWithFilterScores) {
                m_RenderList.Builder.UpdateList(*m_RenderList.RootCategory, m_RenderList.BuilderOpts); // Need to update because filter scores have changed
            }
        }

        RenderMenuBar();
        RenderFilter();
        Separator();
        RenderHooksSection(isFilteringInProgress);
        Separator();
        RenderFooter(isFilteringInProgress);

        RenderHooksExport();
    }

    CheckNeedsToRunFilter();

    m_FilterProcessor.NeedToAckFinished = false;
}

void HooksDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Settings" }, [&] {
        ImGui::MenuItem("Hooks", nullptr, &m_IsOpen);
    });
}

void HooksDebugModule::OnDeserialized() {
    if (m_RenderList.BuilderOpts != RListBuilder::Options{}) {
        m_RenderList.Builder.UpdateList(*m_RenderList.RootCategory, m_RenderList.BuilderOpts);
    }
    m_Filter.Changed = true;
}

};
