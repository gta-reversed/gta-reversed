#pragma once

#include <format>

#include "Enums/HookMode.h"
#include "Enums/TwoWayHookState.h"

#include "StatefulHook.h"

namespace ReversibleHooks {
namespace ReversibleHook {
struct TwoWayHook : public StatefulHook<TwoWayHookState> {
    using StatefulHook::StatefulHook;

    HookMode Mode() const noexcept final { return HookMode::TwoWay; }
    void     Serialize(json& j) const override { to_json(j, *this); }

    virtual void* GetHookAddressGTA() const noexcept = 0;
    virtual void* GetHookAddressOur() const noexcept = 0;

public:
    friend void to_json(json& j, const TwoWayHook& hook) {
        to_json(j, static_cast<const StatefulHook<TwoWayHookState>&>(hook));
        j["AddressGTA"] = std::format("{:p}", hook.GetHookAddressGTA());
        j["AddressOur"] = std::format("{:p}", hook.GetHookAddressOur());
    }

};
}; // namespace ReversibleHook
}; // namespace ReversibleHooks
