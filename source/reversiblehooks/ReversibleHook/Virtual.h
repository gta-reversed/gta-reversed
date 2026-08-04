#pragma once

#include "Base.h" 

#include <vector>
#include <string>

#include "Base.h"
#include "Simple.h"
#include "VMTRedirect.h"

#include <reversiblehooks/VMTInfo.h>

namespace ReversibleHooks {
namespace ReversibleHook {
/*!
 * @brief Handles hooking of virtual functions, including both direct calls and calls that use the VMT.
 */
struct Virtual : public Base {
    Virtual(
        std::string name,
        void**      fnVMTEntryOur,
        void**      fnVMTEntryGTA,
        bool        reversed = true
    );
    ~Virtual() override = default;

    void        Switch() override;
    void        Check() override { m_DirectCallHook.Check(); m_VirtualDispatchHook.Check(); }
    const char* Symbol() const override { return "V"; }

    auto        GetHookGTAAddress() const { return m_DirectCallHook.GetHookGTAAddress(); }
    auto        GetHookOurAddress() const { return m_DirectCallHook.GetHookOurAddress(); }

private:
    Simple      m_DirectCallHook;     //!< For direct calls (Eg.: Explicit calls like `Class::VirtualFunction()`)
    VMTRedirect m_VirtualDispatchHook; //!< For calls that use the VMT (Eg.: `object->VirtualFunction()`)
};
}; // namespace ReversibleHook
}; // namespace ReversibleHooks
