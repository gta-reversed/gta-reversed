#pragma once

#include <Base.h>

#include "StaticOneWayHook.h"
#include "TwoWayHook.h"

namespace ReversibleHooks {
namespace ReversibleHook {
/*!
 * @brief 2-way hook used for static (that is non-virtual) functions
 */
class StaticTwoWayHook final : public TwoWayHook {
public:
    /*!
     * @brief Constructor for hooking static functions
     * @param name Name of the function (eg.: `Add` for `CEntity::Add`)
     * @param fnAddressOur Address of our function (eg.: `&Class::Add`)
     * @param fnAddressGTA Address of GTA's function (eg.: `&Class::Add`)
     * @param numStackArgumentsToPreserve Number of stack arguments to preserve (if any)
     * @param preserveRegisters Whether to preserve registers (if true, must specify `numStackArgumentsToPreserve`)
     */
    StaticTwoWayHook(
        std::string_view name,
        void*            fnAddressOur,
        void*            fnAddressGTA,
        size_t           numStackArgumentsToPreserve = 0,
        bool             preserveRegisters           = false
    );

    ~StaticTwoWayHook() override {
        State(TwoWayHookState::Unhooked);
    }

    HookType Type() const noexcept override { return HookType::StaticTwoWay; }
    void     Check() override;

    void* GetHookAddressGTA() const noexcept override { return m_OneWayFromGTA.GetFrom(); }
    void* GetHookAddressOur() const noexcept override { return m_OneWayToGTA.GetFrom(); }

protected:
    void ApplyNewState(TwoWayHookState state, TwoWayHookState oldState) override;

protected:
    StaticOneWayHook m_OneWayFromGTA; //!< Redirect calls from gta to our code
    StaticOneWayHook m_OneWayToGTA;   //!< Redirect calls from our code to gta
};
};
};
