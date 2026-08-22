#include "StdInc.h"

#include "StaticTwoWayHook.h"
#include <reversiblehooks/HooksUtility.hpp>

using namespace ReversibleHooks;

namespace ReversibleHooks {
namespace ReversibleHook {
StaticTwoWayHook::StaticTwoWayHook(
    std::string_view name,
    void*            fnAddressOur,
    void*            fnAddressGTA,
    size_t           numStackArgumentsToPreserve,
    bool             preserveRegisters
) :
    TwoWayHook{ std::string{ name } },
    m_OneWayFromGTA{ std::format("{}_FromGTA", name), fnAddressGTA, fnAddressOur, numStackArgumentsToPreserve, preserveRegisters },
    m_OneWayToGTA{ std::format("{}_ToGTA", name), fnAddressOur, fnAddressGTA, numStackArgumentsToPreserve, preserveRegisters }
{
}

void StaticTwoWayHook::ApplyNewState(TwoWayHookState state, TwoWayHookState oldState) {
    switch (state) {
    case TwoWayHookState::Unhooked: {
        m_OneWayFromGTA.State(false);
        m_OneWayToGTA.State(false);
        break;
    }
    case TwoWayHookState::RedirectToOurs: {
        m_OneWayToGTA.State(false); // Always first unhook old
        m_OneWayFromGTA.State(true);
        break;
    }
    case TwoWayHookState::RedirectToGTA: {
        m_OneWayFromGTA.State(false); // Always first unhook old
        m_OneWayToGTA.State(true);
        break;
    }
    default:
        NOTSA_UNREACHABLE_CASE(state);
    }
}

void StaticTwoWayHook::Check() {
    switch (m_State) {
    case TwoWayHookState::RedirectToGTA:  m_OneWayToGTA.Check(); break;
    case TwoWayHookState::RedirectToOurs: m_OneWayFromGTA.Check(); break;
    }
}

};
};
