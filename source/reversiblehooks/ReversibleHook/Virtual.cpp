#include "StdInc.h"

#include "Virtual.h"

namespace ReversibleHooks {
namespace ReversibleHook {
Virtual::Virtual(
    std::string name,
    void**      fnVMTEntryOur,
    void**      fnVMTEntryGTA,
    bool        reversed 
) :
    Base{ name, HookType::Virtual, reversed },
    m_VirtualDispatchHook{ name, fnVMTEntryOur, fnVMTEntryGTA, reversed },
    m_DirectCallHook{ name, (uint32)(*fnVMTEntryGTA), *fnVMTEntryOur, reversed }
{
    Switch(); // Switch to initial state (hooked)
}

void Virtual::Switch() {
    m_IsHooked = !m_IsHooked;
    m_DirectCallHook.Switch();
    m_VirtualDispatchHook.Switch();
}
};
};
