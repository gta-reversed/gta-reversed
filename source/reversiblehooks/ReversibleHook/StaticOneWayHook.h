#pragma once

#include <reversiblehooks/HookConstants.hpp>
#include <reversiblehooks/HooksUtility.hpp>
#include "OneWayHook.h"

namespace ReversibleHooks {
namespace ReversibleHook {
/*!
 * @brief Hook that does hooking in 1 way only
 */
class StaticOneWayHook final : public OneWayHook {
    using HookData = std::byte[Constants::MAX_HOOK_DATA_SIZE];

public:
    /*!
     * @param name Name of the hook (eg.: `Add` for `CEntity::Add`)
     * @param from Address we redirect from, this is where the hook is installed
     * @param to The address the hook jumps to
     * @param preservation What the hook code should preserve across the call of `to` (stack arguments, registers) - If nothing, a simple jump is used
     */
    StaticOneWayHook(
        std::string                      name,
        void*                            from,
        void*                            to,
        const Utility::HookPreservation& preservation = {}
    );
    ~StaticOneWayHook();

    void Check();

    void* GetTo() const noexcept { return m_To; }
    void* GetFrom() const noexcept { return m_From; }

protected:
    void ApplyNewState(bool state, bool oldState) override;
    HookType Type() const noexcept override { return HookType::StaticOneWay; }

protected:
    /*!
     * @brief Generates the code preserving the registers and stack arguments into `m_PreservationCodeData`.
     * @return Address of the generated code.
     */
    void* GeneratePreservationCode();

    /*!
     * @brief Generates the hook code that will be written to the hook address.
     * @return The size of the generated hook code.
     */
    size_t GenerateHookCode();

    /*!
     * @brief Restores the original code at the hook address.
     */
    void RestoreHook();

    /*!
     * @brief Applies the hook by copying the generated hook code to the hook address.
     */
    void ApplyHook(bool skipJumpToReacalculation = false);

    /*!
     * @brief Re-calculate jump-to address
     * @returns If the address has changed
     */
    bool RecalculateAddressJumpTo();

protected:
    void*               m_From{};                        //!< Address we redirect from, this is where the hook is installed
    void*               m_To{};                          //!< The addres we want to jump to (If this itself is just a jump instruction, we will optimize it, see @ref Utility::GetJumpToAddress)
    void*               m_CalculatedJumpTo{};            //!< The address the hook actually jumps to (See @ref Utility::GetJumpToAddress)
    size_t              m_HookSize{};                    //!< Size of the data overwritten at the hook address
    HookData            m_HookData{};                    //!< Stores the code that is written to the hook address.
    HookData            m_RestoreHookData{};             //!< Data overwritten at the hook address (before we installed the hook)
    Utility::VirtualPtr m_PreservationCodeData{};        //!< Generated hook code if any (If present the code is first redirected here, then to `m_AddressUsedTo`)
    size_t              m_PreservationCodeDataBufSize{}; //!< Size of the generated hook code's buffer
    Utility::HookPreservation m_Preservation{};          //!< What the hook code preserves across the call (If anything, the hook jumps to `m_PreservationCodeData` that calls `m_CalculatedJumpTo`)
};
}; // namespace ReversibleHook
}; // namespace ReversibleHooks
