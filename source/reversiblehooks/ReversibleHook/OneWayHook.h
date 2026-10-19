#pragma once

#include <utility>

#include "Enums/HookMode.h"
#include "Enums/HookType.h"

#include "StatefulHook.h"

namespace ReversibleHooks {
namespace ReversibleHook {
class OneWayHook : public StatefulHook<bool> {
public:
    using StatefulHook::StatefulHook;

    HookMode Mode() const noexcept final { return HookMode::OneWay; }
    void     Serialize(json& j) const override { NOTSA_UNREACHABLE("No support for serialization"); } // We don't need it for these classes, they're internal
};
}; // namespace ReversibleHook
}; // namespace ReversibleHooks

