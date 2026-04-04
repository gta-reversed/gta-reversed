#include "StdInc.h"
#include <extensions/CommandLine.h>

#include <reversiblehooks/ReversibleHook/Virtual.h>

namespace ReversibleHooks {
namespace ReversibleHook {
Virtual::Virtual(
    std::string name,
    void**      vtblGTA,
    void**      vtblOur,
    size_t      fnIdx,
    bool        reversed
) :
    Base{ name, HookType::Virtual, reversed },
    m_vtbls{ vtblGTA, vtblOur }, // Should be in the same order as the indexers: GTA, OUR
    m_fnIdx{ fnIdx },
#ifdef NOTSA_STANDALONE
    m_simpleHook{ name, NULL, nullptr }
#else
    m_simpleHook{ name, (uint32)(vtblGTA[fnIdx]), vtblOur[fnIdx] }
#endif
{
#ifndef NOTSA_STANDALONE
    for (const auto [i, vtbl] : rngv::enumerate(m_vtbls)) {
        m_pfns[i] = vtbl[fnIdx];
    }
#endif

    Switch();
}

void Virtual::Switch() {
    m_IsHooked = !m_IsHooked;

#ifndef NOTSA_STANDALONE
    // Redirect VTBL entries
    const auto pfn = m_pfns[m_IsHooked ? OUR : GTA];
    for (const auto vtbl : m_vtbls) {
        detail::ScopedVirtualProtectModify m{ &vtbl[m_fnIdx], sizeof(pfn), PAGE_EXECUTE_READWRITE }; // Make sure we have permissions writing here...
        vtbl[m_fnIdx] = pfn;
    }

    m_simpleHook.State(m_IsHooked);
#endif
}
};
};
