#pragma once

#include <type_traits>

#include <Base.h>

namespace ReversibleHooks {
/*!
 * @brief Describes what the hook code must preserve across the call of the hooked function.
 * Some of the original code relies on registers not touched by the callee (eg.: `ecx`/`edx`) being preserved
 * across the call, even though the calling convention doesn't guarantee that.
 */
struct HookPreservation {
    size_t NumStackArguments{}; //!< Number of stack arguments (in dwords) to re-push before calling the function
    bool   PreserveRegisters{}; //!< Preserve all registers (except the ones used for the return value) using pushad/popad
    bool   CallerCleansStack{}; //!< The caller pops the arguments (`__cdecl`), otherwise the callee does (`__stdcall`/`__thiscall`)
    bool   ReturnsInEAX{};      //!< The return value is in `eax` - It must be kept when restoring the registers
    bool   ReturnsInEDX{};      //!< The return value's high part is in `edx` (64-bit values) - It must be kept when restoring the registers

    //! Whether any code has to be generated for the preservation
    bool IsNeeded() const noexcept { return NumStackArguments || PreserveRegisters; }
};

namespace detail {
template<typename R>
constexpr void SetReturnRegisters(HookPreservation& p) {
    if constexpr (!std::is_void_v<R> && !std::is_floating_point_v<R>) { // Floats are returned in `st(0)`, not affected by `popad`
        p.ReturnsInEAX = true;
        p.ReturnsInEDX = sizeof(R) > sizeof(uint32);
    }
}

template<typename T>
struct CallingConvention {
    static constexpr HookPreservation Get() { return {}; }
};

template<typename R, typename... Args>
struct CallingConvention<R(__cdecl*)(Args...)> {
    static constexpr HookPreservation Get() { HookPreservation p{ .CallerCleansStack = true }; SetReturnRegisters<R>(p); return p; }
};

template<typename R, typename... Args>
struct CallingConvention<R(__cdecl*)(Args..., ...)> {
    static constexpr HookPreservation Get() { HookPreservation p{ .CallerCleansStack = true }; SetReturnRegisters<R>(p); return p; }
};

template<typename R, typename... Args>
struct CallingConvention<R(__stdcall*)(Args...)> {
    static constexpr HookPreservation Get() { HookPreservation p{}; SetReturnRegisters<R>(p); return p; }
};

template<typename R, typename C, typename... Args>
struct CallingConvention<R(C::*)(Args...)> {
    static constexpr HookPreservation Get() { HookPreservation p{}; SetReturnRegisters<R>(p); return p; }
};

template<typename R, typename C, typename... Args>
struct CallingConvention<R(C::*)(Args...) const> {
    static constexpr HookPreservation Get() { HookPreservation p{}; SetReturnRegisters<R>(p); return p; }
};
}; // namespace detail

/*!
 * @brief Get the calling convention related preservation info of a function (pointer) type
 * @tparam T Type of the function pointer
 */
template<typename T>
constexpr HookPreservation GetCallingConventionOf() {
    return detail::CallingConvention<T>::Get();
}
}; // namespace ReversibleHooks
