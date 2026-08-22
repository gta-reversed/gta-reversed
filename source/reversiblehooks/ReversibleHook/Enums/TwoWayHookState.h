#pragma once

namespace ReversibleHooks {
namespace ReversibleHook {
enum class TwoWayHookState {
    Unhooked,
    RedirectToGTA,
    RedirectToOurs,

    Count
};
NLOHMANN_JSON_SERIALIZE_ENUM(TwoWayHookState, {
    { TwoWayHookState::Unhooked,       "UNHOOKED"         },
    { TwoWayHookState::RedirectToGTA,  "REDIRECT_TO_GTA"  },
    { TwoWayHookState::RedirectToOurs, "REDIRECT_TO_OURS" },
});
}; // namespace ReversibleHook
}; // namespace ReversibleHooks

inline std::optional<const char*> EnumToString(ReversibleHooks::ReversibleHook::TwoWayHookState state) {
    using State = ReversibleHooks::ReversibleHook::TwoWayHookState;
    switch (state) {
    case State::Unhooked:       return "Unhooked";
    case State::RedirectToGTA:  return "RedirectToGTA";
    case State::RedirectToOurs: return "RedirectToOurs";
    default:                    return nullptr;
    }
}
