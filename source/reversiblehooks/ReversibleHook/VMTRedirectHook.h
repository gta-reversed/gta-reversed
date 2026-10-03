#pragma once

#include "Base.h"

#include <vector>
#include <string>

#include <reversiblehooks/ReversibleHook/TwoWayHook.h>
#include <reversiblehooks/ReversibleHook/StaticTwoWayHook.h>

namespace ReversibleHooks {
namespace ReversibleHook {
/*!
 * @brief Redirects calls to a function in the VMT to our own function, and vice versa.
 * @note Only modifies the VMT entries, doesn't handle direct calls to the function (eg.: `CEntity::Add`).
 */
struct VMTRedirectHook final : public TwoWayHook {
public:
    VMTRedirectHook(
        std::string name,
        void**      fnVMTEntryOur,
        void**      fnVMTEntryGTA
    );

    ~VMTRedirectHook() override {
        State(TwoWayHookState::Unhooked);
    }
    
    HookType Type() const noexcept override { return HookType::VMTRedirect; }
    void     Check() override { /* nop */ }

    void* GetHookAddressGTA() const noexcept override { return m_FnVMTEntryGTA.OriginalFnPtr; }
    void* GetHookAddressOur() const noexcept override { return m_FnVMTEntryOur.OriginalFnPtr; }
    
protected:
    void ApplyNewState(TwoWayHookState state, TwoWayHookState oldState) override;

private:
    struct VMTEntryInfo {
        void** EntryPtr;                   //!< Pointer to the function's VMT entry (eg.: &vmt[index])
        void*  OriginalFnPtr{ *EntryPtr }; //!< Original function pointer
    } m_FnVMTEntryOur{}, m_FnVMTEntryGTA{};
};
};
};
