#pragma once

#include <Base.h>

namespace ReversibleHooks {
namespace ReversibleHook {
/*!
 * @brief Modes of hooks available (TwoWay or OneWay)
 */
enum class HookMode {
    TwoWay, //!< Can go either way (GTA->Ours or Ours->GTA)
    OneWay, //!< Can only go one way (GTA->Ours or Ours->GTA)
};

NLOHMANN_JSON_SERIALIZE_ENUM(HookMode, {
    { HookMode::TwoWay, "TWO_WAY" },
    { HookMode::OneWay, "ONE_WAY" },
});
}; // namespace ReversibleHook
}; // namespace ReversibleHooks
