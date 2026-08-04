#pragma once

#include <Base.h>
#include <Windows.h>

namespace ReversibleHooks {
namespace Utility {
struct ScopedVirtualProtectModify {
    ScopedVirtualProtectModify(LPVOID address, SIZE_T sz, DWORD newProtect = PAGE_EXECUTE_READWRITE) :
        m_Address{ address },
        m_Size{ sz },
        m_NewProtect{ newProtect }
    {
        if (VirtualProtect(address, sz, newProtect, &m_InitialProtect) == 0) {
            throw std::logic_error{ std::format("VirtualProtect failed, error code: {}", GetLastError()) };
        }
    }

    ~ScopedVirtualProtectModify() {
        DWORD prev{};
        if (VirtualProtect(m_Address, m_Size, m_InitialProtect, &prev) == 0) {
            throw std::logic_error{ std::format("VirtualProtect undo failed, error code: {}", GetLastError()) };
        }
        if (prev != m_NewProtect) {
            throw std::logic_error{ std::format("Expected previous protection to be 0x{:X}, but got 0x{:X}", m_NewProtect, prev) };
        }
    }

private:
    DWORD  m_InitialProtect{};
    DWORD  m_NewProtect{};
    LPVOID m_Address{};
    SIZE_T m_Size{};
};

/*!
 * @brief Copies memory from `src` to `dst`, temporarily changing the protection of the destination memory to allow writing.
 * @param dst The destination address to copy to.
 * @param src The source address to copy from.
 * @param nbytes The number of bytes to copy.
 */
void VirtualCopy(void* dst, void* src, size_t nbytes) {
    ScopedVirtualProtectModify g{ dst, nbytes, PAGE_WRITECOPY };
    memcpy(dst, src, nbytes);
}

}; // namespace Utility
}; // namespace ReversibleHooks
