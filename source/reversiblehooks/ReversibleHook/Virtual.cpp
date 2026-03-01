#include "StdInc.h"

#include <reversiblehooks/ReversibleHook/Virtual.h>

namespace ReversibleHooks{
namespace ReversibleHook{
Virtual::Virtual(
    std::string name,
    void** vtblGTA,
    void** vtblOur,
    size_t fnIdx,
    bool reversed
) :
    Base{ name, HookType::Virtual, reversed },
    m_vtbls{vtblGTA, vtblOur}, // Should be in the same order as the indexers: GTA, OUR
    m_fnIdx{ fnIdx },
    m_simpleHook{ name, (uint32)vtblGTA[fnIdx], vtblOur[fnIdx]} // Making sure this has a name too, for debugging purposes..
{
    for (const auto [i, vtbl] : rngv::enumerate(m_vtbls)) {
        m_pfns[i] = vtbl[fnIdx];
    }

    Switch();
};

void Virtual::Switch()
{
    m_IsHooked = !m_IsHooked;

    // Redirect VTBL entries
    const auto pfn = m_pfns[m_IsHooked ? OUR : GTA];
    for (const auto vtbl : m_vtbls) {
        detail::ScopedVirtualProtectModify m{ &vtbl[m_fnIdx], sizeof(pfn), PAGE_EXECUTE_READWRITE }; // Make sure we have permissions writing here...
        vtbl[m_fnIdx] = pfn;
    }

    m_simpleHook.State(m_IsHooked);
}
};
};
