#include "StdInc.h"

#include <extensions/debug.hpp>

#include <reversiblehooks/HooksUtility.hpp>
#include <reversiblehooks/HookConstants.hpp>

#include "StaticOneWayHook.h"

using notsa::debug::GetFunctionInfoAtAddress;

namespace ReversibleHooks {
namespace ReversibleHook {
StaticOneWayHook::StaticOneWayHook(
    std::string                      name,
    void*                            from,
    void*                            to,
    const Utility::HookPreservation& preservation
) :
    OneWayHook{ std::move(name) },
    m_From{ from },
    m_To{ to },
    m_Preservation{ preservation }
{
}

StaticOneWayHook::~StaticOneWayHook() {
    State(false);
}

void StaticOneWayHook::ApplyNewState(bool state, bool oldState) {
    if (state) {
        ApplyHook();
    } else {
        RestoreHook();
    }
}

void StaticOneWayHook::Check() {
    if (!m_State) {
        return; // Not hooked, nothing to check
    }
    // Edit & Continue in MSVC works by pointing the address of a function to a `jmp` instruction that jumps to the new function code.
    // We don't know if `from` or `to` is on "our" side, so we check both.
    // - If it's `from` then the hook might've been overwritten
    // - if it's `to`, then the `jmp` we followed (See @ref RecalculateAddressJumpTo) might point to a different address than before, so we need to re-generate the hook code.
    if (memcmp(m_From, m_HookData, m_HookSize) != 0) {
        ApplyHook();
    } else if (RecalculateAddressJumpTo()) {
        GenerateHookCode();
        ApplyHook(true);
    }
}

void* StaticOneWayHook::GeneratePreservationCode() {
    assert(m_Preservation.IsNeeded() && "GeneratePreservationCode called without any preservation requested");

    // The code is executed from the buffer, so relative addresses must be calculated relative to it
    const auto Generate = [this](std::byte* buf, size_t bufSize) {
        return Utility::GenerateHookCode(buf, bufSize, buf, m_CalculatedJumpTo, m_Preservation);
    };

    const auto requiredSize = Generate(nullptr, 0);
    if (!m_PreservationCodeData || m_PreservationCodeDataBufSize < requiredSize) {
        std::tie(m_PreservationCodeData, m_PreservationCodeDataBufSize) = Utility::VirtualAllocForCode(requiredSize);
    }

    Utility::ScopedVirtualProtectModify g{ m_PreservationCodeData.get(), m_PreservationCodeDataBufSize, PAGE_EXECUTE_READWRITE };
    Generate(m_PreservationCodeData.get(), m_PreservationCodeDataBufSize);
    FlushInstructionCache(GetCurrentProcess(), m_PreservationCodeData.get(), requiredSize);

    return m_PreservationCodeData.get();
}

size_t StaticOneWayHook::GenerateHookCode() {
    assert(m_CalculatedJumpTo && "Must calculate jump address before generating hook code");

    // If something has to be preserved the hook jumps to the generated code that does that (and calls `m_CalculatedJumpTo`),
    // otherwise directly to the target function.
    // (The preservation code can't be written to the hook address directly, as only `MAX_HOOK_DATA_SIZE` bytes may be overwritten there)
    const auto jumpTo = m_Preservation.IsNeeded()
        ? GeneratePreservationCode()
        : m_CalculatedJumpTo;

    m_HookSize = Utility::GenerateHookCode(
        m_HookData,
        sizeof(m_HookData),
        m_From,
        jumpTo
    );
    return m_HookSize;
}

void StaticOneWayHook::RestoreHook() {
    assert(m_HookSize > 0 && "Nothing to restore, was the hook applied at all?");
    Utility::ScopedVirtualProtectModify g{ m_From, m_HookSize, PAGE_EXECUTE_READWRITE };
    if (memcmp(m_From, m_HookData, m_HookSize) == 0) {
        memcpy(m_From, m_RestoreHookData, m_HookSize);
    } else if (memcmp(m_From, m_RestoreHookData, m_HookSize) == 0) {
        throw std::runtime_error(std::format(
            "Hook (`{}`) at function {} (-> {}) was already restored, but the hook code is still present."
            "Could happen if there's duplicate of this exact hook.",
            Name(),
            GetFunctionInfoAtAddress(std::bit_cast<uintptr_t>(m_From)),
            GetFunctionInfoAtAddress(std::bit_cast<uintptr_t>(m_To))
        ));
    } else { // This might be due to Edit&Continue, but also if there's another hook for this address
        HookData data;
        memcpy(data, m_From, m_HookSize);
        NOTSA_LOG_WARN(
            "Hook (`{}`) at function {} (-> {}) has been overwritten with: {:n:02x}\n"
            "This can happen if there's another hook installed on the same address or edit & continue has overwritten it."
            "Can also happen if a derived class hooks a method but doesn't override it (in which case .Overrides = false should be specified as a hook option).",
            Name(),
            GetFunctionInfoAtAddress(std::bit_cast<uintptr_t>(m_From)),
            GetFunctionInfoAtAddress(std::bit_cast<uintptr_t>(m_To)),
            std::span{ reinterpret_cast<uint8*>(data), m_HookSize }
        );
    }
}

void StaticOneWayHook::ApplyHook(bool skipJumpToReacalculation) {
    if (!skipJumpToReacalculation && RecalculateAddressJumpTo()) {
        GenerateHookCode();
    }
    assert(m_HookSize > 0 && "Hook code not generated");
    Utility::ScopedVirtualProtectModify g{ m_From, m_HookSize, PAGE_EXECUTE_READWRITE };
    memcpy(m_RestoreHookData, m_From, m_HookSize);
    memcpy(m_From, m_HookData, m_HookSize);
}

bool StaticOneWayHook::RecalculateAddressJumpTo() {
    const auto jumpTo = Utility::GetJumpToAddress(m_To);
    if (jumpTo) {
        if (jumpTo == m_From) {
            throw std::runtime_error(std::format(
                "Hook (`{}`) at function {} (-> {}) performs a jump to itself, most likely it was hooked already in the opposite direction",
                Name(),
                GetFunctionInfoAtAddress(std::bit_cast<uintptr_t>(m_From)),
                GetFunctionInfoAtAddress(std::bit_cast<uintptr_t>(m_To))
            ));
        }
        if (jumpTo == m_To) {
            throw std::runtime_error(std::format(
                "Hook (`{}`) at function {} (-> {}) is already hooked to jump to the target function",
                Name(),
                GetFunctionInfoAtAddress(std::bit_cast<uintptr_t>(m_From)),
                GetFunctionInfoAtAddress(std::bit_cast<uintptr_t>(m_To))
            ));
        }
    }
    const auto to = jumpTo ? jumpTo : m_To;
    return std::exchange(m_CalculatedJumpTo, to) != to;
}
}; // namespace ReversibleHook
}; // namespace ReversibleHooks
