#pragma once

#include <fstream>
#include <memory>
#include <source_location>

#include <reversiblehooks/ReversibleHook/TwoWayHook.h>
#include <Base.h>

namespace ReversibleHooks {
class HookCategory;

class HookCategoryItem {
    using HookPtr   = std::shared_ptr<ReversibleHook::TwoWayHook>;

public:
    using HookState = ReversibleHook::TwoWayHookState;

public:
    HookCategoryItem(
        HookPtr                     hook,
        bool                        isLocked,
        bool                        isReversed,
        HookState                   state,
        std::source_location        installSrcLoc
    ) :
        m_Hook{ std::move(hook) },
        m_IsStateLocked{ isLocked },
        m_IsReversed{ isReversed },
        m_InstallSrcLoc{ std::move(installSrcLoc) }
    {
        SetState(state, true);
    }

    /*!
     * @return The 
     */
    auto GetHook() const noexcept { return m_Hook; }

    /*!
     * @return Hook's current state
     */
    auto GetState() const { return m_Hook->State(); }

    /*!
     * @brief Set state of hook
     * @param state new state
     * @param ignoreLock if true, will ignore the lock and set the state regardless of it
     * @return If the state has changed
     */
    bool SetState(HookState state, bool ignoreLock = false);

    /*!
     * @brief Restore to previous state
     */
    bool SetToPreviousState() { return SetState(m_PrevState); }

    /*!
     * @returns The previous state of the hook (before the last change)
     */
    auto GetPreviousState() const noexcept { return m_PrevState; }

    /*!
     * @return If state is locked (and can't be changed)
     */
    bool GetIsStateLocked() const noexcept { return  m_IsStateLocked; }

    /*!
     * @return Hook's name
     */
    const auto& GetName() const noexcept { return m_Hook->Name(); }

    /*!
     * @return Hook's type
     */
    auto GetType() const noexcept { return m_Hook->Type(); }

    /*!
     * @return Hook's address in GTA's code (nullptr if not applicable)
     */
    void* GetHookAddressGTA() const noexcept { return m_Hook->GetHookAddressGTA(); }

    /*!
     * @return Hook's address in our code (nullptr if not applicable)
     */
    void* GetHookAddressOur() const noexcept { return m_Hook->GetHookAddressOur(); }

    /*!
     * @brief Set category of this item
     */
    void SetCategory(std::shared_ptr<HookCategory> category) { m_Category = std::move(category); }

public:
    friend void to_json(json& j, const HookCategoryItem& item);

protected:
    std::weak_ptr<HookCategory> m_Category{};
    HookState                   m_PrevState{};
    std::source_location        m_InstallSrcLoc;        //!< Source location where the hook was installed
    HookPtr                     m_Hook;                 //!< The hook
    bool                        m_IsReversed : 1 {};    //!< Is re'd?
    bool                        m_IsStateLocked : 1 {}; //!< Is state locked (can't be changed by user)
};
}; // namespace ReversibleHooks
