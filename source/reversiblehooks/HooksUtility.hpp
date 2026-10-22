#pragma once

#include <Base.h>
#include <Windows.h>
#include <extensions/utility.hpp>

namespace ReversibleHooks {
namespace Utility {
struct ScopedVirtualProtectModify {
    ScopedVirtualProtectModify(LPVOID address, SIZE_T sz, DWORD newProtect = PAGE_EXECUTE_READWRITE) :
        m_Address{ address },
        m_Size{ sz }
    {
        if (VirtualProtect(address, sz, newProtect, &m_InitialProtect) == 0) {
            throw std::logic_error{ std::format("VirtualProtect failed, error code: {}", GetLastError()) };
        }
    }

    ~ScopedVirtualProtectModify() {
        DWORD prev{};
        if (VirtualProtect(m_Address, m_Size, m_InitialProtect, &prev) == 0) {
            NOTSA_LOG_ERR("VirtualProtect undo failed, error code: {}", GetLastError());
        }
    }

private:
    DWORD  m_InitialProtect{};
    LPVOID m_Address{};
    SIZE_T m_Size{};
};

template<typename T>
constexpr void* FunctionToVoidPtr(T fn) {
    static_assert(sizeof(fn) == sizeof(void*), "Function pointer must fit into a `void*`"); // Some member functions might be larger than this in some cases
    return std::bit_cast<void*>(fn);
}

template<typename T>
constexpr T VoidToFunctionPtr(void* p) {
    return std::bit_cast<T>(p);
}

/*! 
 * @brief Copies memory from `src` to `dst`, temporarily changing the protection of the destination memory to allow writing.
 * @param dst The destination address to copy to.
 * @param src The source address to copy from.
 * @param nbytes The number of bytes to copy.
 */
inline void VirtualCopy(void* dst, void* src, size_t nbytes) {
    ScopedVirtualProtectModify g{ dst, nbytes };
    memcpy(dst, src, nbytes);
}

namespace detail {
template<typename T, typename... ConstructorArgs>
    requires std::is_class_v<T>
struct ConstructorWrapper : protected T {
    using T::T;
    T* Construct(ConstructorArgs... args) {
        this->ConstructorWrapper::ConstructorWrapper(std::forward<ConstructorArgs>(args)...);
        return static_cast<T*>(this);
    }
};
};

/*!
 * @tparam T The class to get the constructor's address of
 * @returns Address to a wrapper that calls the class's constructor for the given arguments
 */
template<typename T, typename... Args>
constexpr void* GetConstructorAddress() {
    return FunctionToVoidPtr(&detail::ConstructorWrapper<T, Args...>::Construct);
}

namespace detail {
template<typename T>
struct DestructorWrapper : protected T {
    T* Destruct() {
        this->T::~T();
        return static_cast<T*>(this);
    }
};
};

/*!
 * @note In case of virtual destructors, there's another one called virtual deliting destructor which is only available through the VMT
 * @tparam T The class to get the destructor's address of
 * @returns Address to a wrapper that calls the the class's (base, eg.: `&Class::~Class`) destructor
 */
template<typename T>
constexpr void* GetScalarDestructorAddress() {
    return FunctionToVoidPtr(&detail::DestructorWrapper<T>::Destruct);
}

/*!
 * @brief Reads a specified number of bytes from a given memory address and returns them as an integral type.
 * @tparam T 
 * @param address 
 * @param bytes 
 * @return 
 */
template<std::integral T>
inline T ReadAtAddress(void* address, size_t bytes) {
    assert(bytes <= sizeof(T));
    T ret = 0;
    memcpy(&ret, address, bytes);
    return ret;
}

/*!
 * @param at The address to check for a `jmp` instruction.
 * @return If there's a `jmp` instruction at `at` the function returns it's *absolute* target address, otherwise it null
 */
inline void* GetJumpToAddress(void* at) {
    const auto p = static_cast<std::byte*>(at);

    ScopedVirtualProtectModify g{ at, 16, PAGE_EXECUTE_READ }; // 16 is quite arbitrary, but should be enough for any of these instructions

    // If the instruction at the given address matches the provided opcode sequence.
    const auto InstructionIs = [p](std::initializer_list<uint8_t> opcode) {
        return memcmp(p, opcode.begin(), opcode.size()) == 0;
    };

    // Read jmp (relative) operand and convert it to an absolute address
    const auto GetJumpAddress = [p](int32 jmpOpSize, int32 addressSize) {
        intptr_t rel = 0;
        assert(sizeof(rel) >= addressSize);
        memcpy(&rel, p + jmpOpSize, addressSize);
        return p + jmpOpSize + addressSize + rel;
    };

    if (InstructionIs({ 0xEB })) { // jmp <rel8> - Jump to address relative to current address
        return GetJumpAddress(1, 1);
    }
    if (InstructionIs({ 0xE9 })) { // jmp <rel32> - Jump to address relative to current address
        return GetJumpAddress(1, 4);
    }

    return nullptr; // No jump found, return the address itself
}

namespace detail {
struct VirtualFreeDeleter {
void operator()(void* ptr) const noexcept {
    if (ptr) {
        VirtualFree(ptr, 0, MEM_RELEASE); 
    }
}
};
};
using VirtualPtr = std::unique_ptr<std::byte[], detail::VirtualFreeDeleter>;


/*!
 * @brief Allocate virtual memory for code execution, with specified protection flags.
 * @param minSize The minimum size of the memory to allocate, adjusted to the system's allocation granularity.
 * @param allocationType The type of memory allocation (default: MEM_RESERVE | MEM_COMMIT).
 * @param protect The memory protection flags (default: PAGE_EXECUTE_READWRITE).
 * @return The allocated memory pointer and the size of the allocated memory.
 * @throws std::runtime_error if the allocation fails.
 */
inline std::pair<VirtualPtr, size_t> VirtualAllocForCode(size_t minSize, DWORD allocationType = MEM_RESERVE | MEM_COMMIT, DWORD protect = PAGE_EXECUTE_READWRITE) {
    SYSTEM_INFO si{};
    GetSystemInfo(&si);
    const auto size = notsa::round_up_to_multiple<size_t>(minSize, si.dwAllocationGranularity);
    void* const ptr = VirtualAlloc(nullptr, size, allocationType, protect);
    if (!ptr) {
        throw std::runtime_error{ std::format("Failed to allocate memory, error code: {}", GetLastError()) };
    }
    return { VirtualPtr{ static_cast<std::byte*>(ptr) }, size };
}

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

/*!
 * @brief Writes assembly code to preserve all registers, call a specified function, and then restore the registers.
 * @param buf The buffer to write the assembly code to. If nullptr, the function will return the required size of the buffer.
 * @param bufSize The size of the buffer. If 0, the function will return the required size of the buffer.
 * @param base The base address where the generated code will be executed (used for calculating relative jumps).
 * @param functionToCall The function to call after preserving the registers.
 * @param preservation What to preserve. `NumStackArguments` must be correct, otherwise the stack will be corrupted - Their value is preserved by re-pushing them before calling `functionToCall`.
 * @return The number of bytes written to the buffer, or the required size if bufSize is 0 and buf is nullptr.
 * @throws std::invalid_argument if the buffer is too small to hold the generated code.
 */
inline size_t GenerateHookCode(
    std::byte*              buf,
    size_t                  bufSize,
    void*                   base,
    void*                   functionToCall,
    const HookPreservation& preservation = {}
) {
    const auto [numStackArguments, preserveRegisters, callerCleansStack, returnsInEAX, returnsInEDX] = preservation;

    assert(buf != nullptr || bufSize == 0 && "`bufSize` must be 0 if `buf` is nullptr");

    auto* out = buf;
    const auto Write = [&](auto value, size_t size) {
        if (buf) {
            if ((out - buf) + size > bufSize) {
                throw std::invalid_argument{ "Out of memory" };
            }
            memcpy(out, &value, size);
        }
        out += size;
    };

    // Write: [op] <address:rel32>
    const auto WriteJumpOp = [&, base](uint8 op, void* address) {
        const auto ip = out - buf; // Offset of this instruction
        Write(op, 1);                                                               // [op]
        Write((std::byte*)(address) - ((std::byte*)(base) + ip) - (1 + 4), 4);      // <address:rel32> (Calculated relative to `base`)
    };

    // Write: <op> [esp + <offset>] (`op` is the 2 bytes of the opcode + modrm, the SIB byte is always 0x24 (esp))
    const auto WriteESPRelativeOp = [&](uint8 op, uint8 modrm, int32 offset) {
        Write(op, 1);
        if (offset < 128) {
            Write((uint8)(0x40 | modrm), 1); // [esp + disp8]
            Write((uint8)(0x24), 1);
            Write((uint8)(offset), 1);
        } else {
            Write((uint8)(0x80 | modrm), 1); // [esp + disp32]
            Write((uint8)(0x24), 1);
            Write(offset, 4);
        }
    };

    // If no stack arguments and no registers to preserve,
    // we can just `jmp` directly to the function otherwise we must `call`
    if (!numStackArguments && !preserveRegisters) {
        WriteJumpOp(0xE9, functionToCall); // jmp <address:rel32>
    } else {
        constexpr int32 PUSHAD_SIZE = 8 * 4; // 8 registers are pushed

        // pushad (save all registers)
        if (preserveRegisters) {
            Write((uint8)(0x60), 1);
        }

        // push [ESP + <offset>] (re-push all arguments)
        // The offset doesn't change, as each `push` moves `esp` down by 4 bytes, the next (previous) argument will be at the same offset
        const auto stackOffset = (int32)(4 * numStackArguments) + (preserveRegisters ? PUSHAD_SIZE : 0);
        for (size_t i = 0; i < numStackArguments; ++i) {
            WriteESPRelativeOp(0xFF, 0x34, stackOffset); // push dword ptr [esp + <offset>] (FF /6)
        }

        // call <address:rel32>
        WriteJumpOp(0xE8, functionToCall);

        // add esp, <size> (pop the re-pushed arguments if the function didn't)
        if (callerCleansStack && numStackArguments) {
            Write((uint8)(0x81), 1);
            Write((uint8)(0xC4), 1);
            Write((int32)(4 * numStackArguments), 4);
        }

        if (preserveRegisters) {
            // Overwrite the saved registers used for the return value, so `popad` restores them with the return value
            // The layout pushed by `pushad` is (from lower to higher addresses): edi, esi, ebp, esp, ebx, edx, ecx, eax
            if (returnsInEAX) {
                WriteESPRelativeOp(0x89, 0x04, 7 * 4); // mov [esp + <offset of eax>], eax
            }
            if (returnsInEDX) {
                WriteESPRelativeOp(0x89, 0x14, 5 * 4); // mov [esp + <offset of edx>], edx
            }

            // popad (restore all registers)
            Write((uint8)(0x61), 1);
        }

        // ret (and pop arguments, if the callee is responsible for that)
        if (numStackArguments > 0 && !callerCleansStack) {
            Write((uint8)(0xC2), 1); // ret <number of bytes to pop>
            Write((uint16)(numStackArguments * 4), 2); // <number of bytes to pop>
        } else {
            Write((uint8)(0xC3), 1); // ret
        }
    }

    return out - buf; // Return the number of bytes written (or required if `bufSize` was 0)
}
}; // namespace Utility
}; // namespace ReversibleHooks
