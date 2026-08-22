#include "StdInc.h"

#include <Base.h>

#include <cassert>
#include <filesystem>
#include <format>

#include "HookCategory.h"
#include "HookCategoryItem.h"
#include "ReversibleHook/Enums/TwoWayHookState.h"
#include "ReversibleHook/TwoWayHook.h"

namespace ReversibleHooks {
bool HookCategoryItem::SetState(HookState state, bool ignoreLock) {
    if (!ignoreLock && m_IsStateLocked) {
        return false;
    }
    assert((m_IsReversed || state == HookState::RedirectToGTA) && "Hooks for functions not `reversed` should redirect to gta");
    const auto prev = m_Hook->State();
    if (!m_Hook->State(state)) {
        return false;
    }
    m_PrevState = prev;
    return true;
}
void to_json(json& j, const HookCategoryItem& item) {
    to_json(j, *item.GetHook());
    if (const auto category = item.m_Category.lock()) {
        j["Category"] = category->Name();
    } else {
        j["Category"] = nullptr;
    }
    j["IsLocked"]              = item.GetIsStateLocked();
    j["IsReversed"]            = item.m_IsReversed;
    j["InstallSourceLocation"] = std::format("{}:{}", fs::relative(item.m_InstallSrcLoc.file_name(), notsa::GetSourceCodeBasePath()).generic_string(), item.m_InstallSrcLoc.line());
}
}; // namespace ReversibleHooks
