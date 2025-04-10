#include "StdInc.h"

#include "Simple.h"

namespace ReversibleHooks{
namespace ReversibleHook{
Simple::Simple(std::string fnName, uint32 installAddress, void* addressToJumpTo, int iJmpCodeSize, int stackArguments) :
    Base{ std::move(fnName), HookType::Simple },
    m_iLibFunctionAddress((uint32)addressToJumpTo),
    m_iRealHookedAddress(installAddress),
    m_iHookedBytes(iJmpCodeSize)
{
    if (stackArguments != -1)
        GenerateECXPreservationThunk(stackArguments);

    m_HookContent.jumpLocation = ReversibleHooks::GetJMPLocation(installAddress, m_iLibFunctionAddress);
    memset(m_HookContent.possibleNops, NOP_OPCODE, iJmpCodeSize - ReversibleHooks::x86JMPSize);

    // Copy lib function content
    m_LibHookContent.jumpLocation = ReversibleHooks::GetJMPLocation(m_iLibFunctionAddress, installAddress);
    m_iLibHookedBytes = ReversibleHooks::x86JMPSize;
    memcpy(&m_LibOriginalFunctionContent, (void*)m_iLibFunctionAddress, m_iLibHookedBytes);

    auto InstallHook = [&](bool bVirtProtect) {
        DWORD dwProtectHoodlum[2] = { 0 };
        if (bVirtProtect)
            VirtualProtect((void*)m_iRealHookedAddress, m_iHookedBytes, PAGE_EXECUTE_READWRITE, &dwProtectHoodlum[0]);

        memcpy(&m_OriginalFunctionContent, (void*)m_iRealHookedAddress, m_iHookedBytes);
        memcpy((void*)m_iRealHookedAddress, &m_HookContent, m_iHookedBytes);

        if (bVirtProtect)
            VirtualProtect((void*)m_iRealHookedAddress, m_iHookedBytes, dwProtectHoodlum[0], &dwProtectHoodlum[1]);

        m_bIsHooked = true;
        m_isVisible = true;
    };

    const int maxBytesToProtect = std::max(iJmpCodeSize, 8);
    DWORD dwProtect[2] = { 0 };
    VirtualProtect((void*)installAddress, maxBytesToProtect, PAGE_EXECUTE_READWRITE, &dwProtect[0]);

    // workaround for hoodlum crashes due to securom protection.
    if (*(uint8*)installAddress == NOP_OPCODE) {
        bool bJumpFound = false;
        uint32 i = 0;
        for (; i < 4; i++) {
            if (*(uint8*)(installAddress + i) == JUMP_OPCODE) {
                bJumpFound = true;
                break;
            }
        }
        if (bJumpFound) {
            DWORD newInstallAddress = installAddress + i;
            DWORD hoodlumHookAddress = *(DWORD*)(newInstallAddress + 1) + newInstallAddress + ReversibleHooks::x86JMPSize;
            m_iRealHookedAddress = hoodlumHookAddress;
            m_HookContent.jumpLocation = ReversibleHooks::GetJMPLocation(hoodlumHookAddress, m_iLibFunctionAddress);
            m_iHookedBytes = ReversibleHooks::x86JMPSize;

            InstallHook(true);
        }
        else {
            printf("HookInstall: Couldn't find the jump for address = %#.8x\n", installAddress);
            InstallHook(false);
        }
    }
    else {
        InstallHook(false);
    }
    VirtualProtect((void*)installAddress, maxBytesToProtect, dwProtect[0], &dwProtect[1]);
}

// VS has this magic called `Edit and Continue`
// The way it works is as follows:
// A function address basically just points to a `jmp` instruction, which
// then jumps to the actual function.
// Whenever a function is edited the compiler changes the `jmp` address
// thus, what we've stored as `m_LibOriginalFunctionContent` has changed.
// So, here we check if thats the case, and update `m_LibOriginalFunctionContent` if necessary
bool Simple::CheckLibFnForChangesAndStore(void* expected) {
    if (memcmp((void*)m_iLibFunctionAddress, expected, m_iLibHookedBytes) != 0) {
        memcpy(&m_LibOriginalFunctionContent, (void*)m_iLibFunctionAddress, m_iLibHookedBytes);
        return true;
    }
    return false;
}

void Simple::ApplyJumpToGTACode() {
    using namespace ReversibleHooks::detail;
    VirtualCopy((void*)m_iLibFunctionAddress, (void*)&m_LibHookContent, m_iLibHookedBytes);
}

/*
 * There are optimised chunks of code in the original game where a function X calls another
 * function Y and expects that ECX is preserved across the call, even though __thiscall does
 * not guarantee that. This will lead to crashes (in the debug build, at least) because our
 * reversed functions might clobber ECX.
 *
 * For such troublesome functions we'll generate a thunk that pushes ECX to stack, calls our
 * hook, then restores ECX.
 *
 * The generated thunk assumes that the callee is responsible for cleaning the stack (stdcall/thiscall)
 * Also, each thunk allocates a new page. This is not memory-friendly, but hopefully there won't be
 * many functions that will need this workaround.
 */
void Simple::GenerateECXPreservationThunk(int stackArguments)
{
    size_t maxThunkSize = 7 * stackArguments + 16; // space for push instrs and prologue/epilogue
    int32 stackOffset = 4 * (stackArguments + 1);
    void* pThunk = VirtualAlloc(nullptr, maxThunkSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    assert(pThunk);
    uint8* pCode = (uint8*)pThunk;

    // push ECX (save ECX)
    *pCode++ = 0x51;

    // push [ESP + stackOffset] (re-push all arguments)
    for (int i = 0; i < stackArguments; ++i) {
        *pCode++ = 0xFF;
        *pCode++ = (stackOffset >= 128) ? 0xB4 : 0x74;
        *pCode++ = 0x24;
        *(int32*)pCode = stackOffset;
        pCode += (stackOffset >= 128) ? 4 : 1;
    }

    // call m_iLibFunctionAddress (call our hooked function)
    *pCode++ = 0xE8;
    *(int32*)pCode = ReversibleHooks::GetJMPLocation((uint32)(pCode - 1), m_iLibFunctionAddress);
    pCode += 4;

    // pop ECX (restore)
    *pCode++ = 0x59;

    // ret (and pop arguments, if any)
    *pCode++ = (stackArguments > 0) ? 0xC2 : 0xC3;
    if (stackArguments > 0) {
        *(uint16*)pCode = (uint16)(stackArguments * 4);
        pCode += 2;
    }

    DWORD dwProtect;
    VirtualProtect(pThunk, maxThunkSize, PAGE_EXECUTE_READ, &dwProtect);
    m_iLibFunctionAddress = (uint32)pThunk; // redirect to our newly-generated thunk instead
}

void Simple::Switch()
{
    using namespace ReversibleHooks::detail;
    if (m_bIsLocked)
        return;

    if (m_bIsHooked) {
        // Unhook (make our code jump to the GTA function)

        VirtualCopy((void*)m_iRealHookedAddress, (void*)&m_OriginalFunctionContent, m_iHookedBytes);
        ApplyJumpToGTACode();
    } else {
        // Hook (make the GTA function jump to ours)

        VirtualCopy((void*)m_iRealHookedAddress, (void*)&m_HookContent, m_iHookedBytes);
        VirtualCopy((void*)m_iLibFunctionAddress, (void*)&m_LibOriginalFunctionContent, m_iLibHookedBytes);
    }
    m_bIsHooked = !m_bIsHooked;
}

void Simple::Check() {
    if (m_bIsHooked) {
        CheckLibFnForChangesAndStore((void*)m_LibOriginalFunctionContent);
    } else {
        if (CheckLibFnForChangesAndStore((void*)&m_LibHookContent)) {
            ApplyJumpToGTACode(); // Compiler overwrote it, so we must re-apply the jump
        }
    }
}
};
};
