#include "StdInc.h"

#include <reversiblehooks/ReversibleHook/Virtual.h>

namespace ReversibleHooks{
namespace ReversibleHook{
Virtual::Virtual(std::string fnName, void** vtblGTA, void** vtblOur, size_t fnIdx) :
    Base{ fnName, HookType::Virtual },
    m_vtbls{vtblGTA, vtblOur}, // Should be in the same order as the indexers: GTA, OUR
    m_fnIdx{ fnIdx },
    m_simpleHook{ std::move(fnName), (uint32)vtblGTA[fnIdx], vtblOur[fnIdx]} // Making sure this has a name too, for debugging purposes..
{
    for (const auto [i, vtbl] : rngv::enumerate(m_vtbls)) {
        m_pfns[i] = vtbl[fnIdx];
    }

    Switch();
};

void Virtual::Switch()
{
    m_bIsHooked = !m_bIsHooked;

    // Redirect VTBL entries
    const auto pfn = m_pfns[m_bIsHooked ? OUR : GTA];
    for (const auto vtbl : m_vtbls) {
        detail::ScopedVirtualProtectModify m{ &vtbl[m_fnIdx], sizeof(pfn), PAGE_EXECUTE_READWRITE }; // Make sure we have permissions writing here...
        vtbl[m_fnIdx] = pfn;
    }

    m_simpleHook.State(m_bIsHooked);
}
};
};
