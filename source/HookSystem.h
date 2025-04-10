#pragma once

#include <cstdint>

const std::uint32_t JUMP_OPCODE = 0xE9;
const std::uint32_t NOP_OPCODE = 0x90;

#pragma pack(push, 1)
struct SHook
{
    unsigned char jumpOpCode;
    unsigned int jumpLocation;
    unsigned char possibleNops[47] = {0}; // maximum size for nops

    SHook ()
    {
        jumpOpCode = JUMP_OPCODE;
        jumpLocation = 0;
    }
};
#pragma pack(pop)

VALIDATE_SIZE(SHook, 0x34);

template<typename T>
void *FunctionPointerToVoidP(T func)
{
    union
    {
        T a;
        void *b;
    } c = {func};
    return c.b;
}

template<typename T>
void HookInstallHoodlum(DWORD installAddress, T addressToJumpTo)
{
    DWORD dwAddressToJumpTo = (DWORD)FunctionPointerToVoidP(addressToJumpTo);
    const DWORD x86FixedJumpSize = 5;
    SHook theHook;
    theHook.jumpLocation = (DWORD)dwAddressToJumpTo - (DWORD)installAddress - (DWORD)x86FixedJumpSize;
    DWORD dwProtect[2];
    VirtualProtect((void*)installAddress, 5, PAGE_EXECUTE_READWRITE, &dwProtect[0]);
    memcpy((void*)installAddress, &theHook, x86FixedJumpSize);
    VirtualProtect((void*)installAddress, 5, dwProtect[0], &dwProtect[1]);
}

template<typename T>
void HookInstall(DWORD installAddress, T addressToJumpTo, int iJmpCodeSize = 5)
{
    DWORD dwAddressToJumpTo = (DWORD) FunctionPointerToVoidP(addressToJumpTo);

    const DWORD x86FixedJumpSize = 5;
    SHook theHook;

    theHook.jumpLocation =  (DWORD)dwAddressToJumpTo - (DWORD)installAddress - (DWORD)x86FixedJumpSize;
    memset(theHook.possibleNops, NOP_OPCODE, iJmpCodeSize - x86FixedJumpSize);

    const int maxBytesToProtect = std::max(iJmpCodeSize, 8);
    DWORD dwProtect[2];
    VirtualProtect((void*)installAddress, maxBytesToProtect, PAGE_EXECUTE_READWRITE, &dwProtect[0]);
    
    // workaround for hoodlum crashes due to securom protection.
    if (*(std::uint8_t*)installAddress == NOP_OPCODE) {
        bool bJumpFound = false;
        std::uint32_t i = 0;
        for (; i < 4; i++) {
            if (*(std::uint8_t*)(installAddress + i) == JUMP_OPCODE) {
                bJumpFound = true;
                break;
            }
        }
        if (bJumpFound) {
            DWORD newInstallAddress = installAddress + i;
            DWORD jumpDestination = *(DWORD*)(newInstallAddress + 1) + newInstallAddress + x86FixedJumpSize;
            HookInstallHoodlum(jumpDestination, addressToJumpTo);
        }
        else {
            printf("HookInstall: Couldn't find the jump for address = %#.8x\n", installAddress);
            memcpy((void*)installAddress, &theHook, iJmpCodeSize);
        }
    }
    else {
        memcpy((void*)installAddress, &theHook, iJmpCodeSize);
    }
    VirtualProtect((void*)installAddress, maxBytesToProtect, dwProtect[0], &dwProtect[1]);
}
