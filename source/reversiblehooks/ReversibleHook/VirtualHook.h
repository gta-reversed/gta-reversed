#pragma once

#include <Base.h>

#include "TwoWayHook.h"
#include "StaticTwoWayHook.h"
#include "VMTRedirectHook.h"

namespace ReversibleHooks {
namespace ReversibleHook {
/*!
 * @brief Handles hooking of virtual functions, including both direct calls and calls that use the VMT.
 */
struct VirtualHook : public TwoWayHook {
    /*!
     * @brief Constructor for hooking virtual functions where the direct call and virtual functions are the same (So basically all virtual functions other than destructors)
     * @param name Name of the function (eg.: `Add` for `CEntity::Add`)
     * @param fnVMTEntryOur Pointer to our VMT's entry of the function (eg.: `&vmt[index]`)
     * @param fnVMTEntryGTA Pointer to GTA's VMT entry of the function (eg.: `&vmt[index]`)
     */
    VirtualHook(
        std::string_view name,
        void**           fnVMTEntryOur,
        void**           fnVMTEntryGTA
    );

    /*!
     * @brief Advanced constructor for hooking virtual functions where the direct call and virtual functions aren't the same (For example destructors vs deleting virtual destructor)
     * @param name Name of the function (eg.: `Add` for `CEntity::Add`)
     * @param fnVMTEntryOur Pointer to our VMT's entry of the function (eg.: `&vmt[index]`)
     * @param fnAddressOur Address of our function (eg.: &Class::VirtualFunction)
     * @param fnVMTEntryGTA Pointer to GTA's VMT entry of the function (eg.: `&vmt[index]`)
     * @param fnAddressGTA Address of GTA function (eg.: &Class::VirtualFunction)
     */
    VirtualHook(
        std::string_view name,
        void**           fnVMTEntryOur,
        void*            fnAddressOur,
        void**           fnVMTEntryGTA,
        void*            fnAddressGTA
    );

    ~VirtualHook() override {
        State(TwoWayHookState::Unhooked);
    }

    HookType Type() const noexcept override { return HookType::Virtual; }
    void Check() final override;

    void* GetHookAddressGTA() const noexcept override { return m_DirectCallHook.GetHookAddressGTA(); }
    void* GetHookAddressOur() const noexcept override { return m_DirectCallHook.GetHookAddressOur(); }

protected:
    void ApplyNewState(TwoWayHookState state, TwoWayHookState oldState) final override;

private:
    StaticTwoWayHook m_DirectCallHook;      //!< For direct calls (Eg.: Explicit calls like `Class::VirtualFunction()`)
    VMTRedirectHook  m_VirtualDispatchHook; //!< For calls that use the VMT (Eg.: `object->VirtualFunction()`)
};
}; // namespace ReversibleHook
}; // namespace ReversibleHooks
