#pragma once

#include "TwoWayHook.h"

namespace ReversibleHooks {
namespace ReversibleHook {
/*!
 * @brief A hook that does nothing, it's used for documentation purposes (Dumping hooks in standalone mode)
 */
class NullHook final : public TwoWayHook {
public:
    NullHook(std::string name, void* addressOur, void* addressGTA) : 
        TwoWayHook(std::move(name)), 
        m_AddressOur(addressOur),
        m_AddressGTA(addressGTA)
    {}
    ~NullHook() override = default;

    HookType Type()              const noexcept override { return HookType::Null; }
    void     Serialize(json& j)  const override          { to_json(j, *this); }
    void     Check()             override                { /* nothing to check */ }
    void*    GetHookAddressOur() const noexcept override { return m_AddressOur; }
    void*    GetHookAddressGTA() const noexcept override { return m_AddressGTA; }
    void     ApplyNewState(TwoWayHookState state, TwoWayHookState oldState) override { /* nothing to apply */ }

public:
    friend void to_json(json& j, const NullHook& hook) {
        to_json(j, static_cast<const TwoWayHook&>(hook));
    }

private:
    void* m_AddressGTA{};
    void* m_AddressOur{};
};
}; // namespace ReversibleHook
}; // namespace ReversibleHooks
