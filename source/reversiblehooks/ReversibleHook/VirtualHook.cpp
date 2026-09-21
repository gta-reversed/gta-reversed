#include "StdInc.h"

#include "VirtualHook.h"

namespace ReversibleHooks {
namespace ReversibleHook {
VirtualHook::VirtualHook(
    std::string_view name,
    void**           fnVMTEntryOur,
    void**           fnVMTEntryGTA
) :
    VirtualHook{
        std::move(name),
        fnVMTEntryOur,
        *fnVMTEntryOur,
        fnVMTEntryGTA,
        *fnVMTEntryGTA
    }
{
}

VirtualHook::VirtualHook(
    std::string_view name,
    void**           fnVMTEntryOur,
    void*            fnAddressOur,
    void**           fnVMTEntryGTA,
    void*            fnAddressGTA
) :
    TwoWayHook{ std::string{ name } },
    m_VirtualDispatchHook{ std::string{ name }, fnVMTEntryOur, fnVMTEntryGTA },
    m_DirectCallHook{ std::string{ name }, fnAddressOur, fnAddressGTA }
{
}

void VirtualHook::Check() {
    m_DirectCallHook.Check();
    m_VirtualDispatchHook.Check();
}

void VirtualHook::ApplyNewState(TwoWayHookState state, TwoWayHookState oldState) {
    m_DirectCallHook.State(state);
    m_VirtualDispatchHook.State(state);
}
}; // namespace ReversibleHook
}; // namespace ReversibleHooks
