#pragma once

#include "Base.h" 

#include <vector>
#include <string>
#include "Base.h"
#include <reversiblehooks/ReversibleHook/Simple.h>

namespace ReversibleHooks{
namespace ReversibleHook{
struct Virtual : public Base {
    Virtual(std::string fnName, void** vtblGTA, void** vtblOur, size_t fnIdx);
    ~Virtual() override = default;

    void        Switch() override;
    void        Check() override { m_simpleHook.Check(); }
    const char* Symbol() const override { return "V"; }

    auto GetHookGTAAddress() const { return m_pfns[GTA]; }
    auto GetHookOurAddress() const { return m_pfns[OUR]; }

private:
    // Use these values for indexing below arrays
    constexpr static auto GTA = 0u;
    constexpr static auto OUR = 1u;

    //! Original function pointers
    void* m_pfns[2]{};

    //! vtables
    void** m_vtbls[2]{};

    //! Function index in vtable
    size_t m_fnIdx{};

    //! This makes sure direct calls (so not thru the vtbl) are also hooked properly
    Simple m_simpleHook;
};

};
};
