#pragma once 

#include "Base.h"

namespace ReversibleHooks{
namespace ReversibleHook {

#pragma pack(push, 1)
struct SHookContent {
    uint8  jumpOpCode{(uint8)JUMP_OPCODE};
    uint32 jumpLocation{};
    uint8  possibleNops[52 - sizeof(jumpOpCode) - sizeof(jumpLocation)]{};
};
#pragma pack(pop)
VALIDATE_SIZE(SHookContent, 0x34);

struct Simple : Base {
    // TODO: Refactor this copy-paste and document it

    SHookContent m_HookContent{};
    uint8        m_OriginalFunctionContent[sizeof(m_HookContent)]{};
    uint32       m_iHookedBytes{};
    uint32       m_iRealHookedAddress{}; // Address of GTA function

    SHookContent m_LibHookContent{};
    uint8        m_LibOriginalFunctionContent[sizeof(m_LibHookContent)]{};
    uint32       m_iLibHookedBytes{};
    uint32       m_iLibFunctionAddress{}; // Address of our function

    Simple(std::string fnName, uint32 installAddress, void* addressToJumpTo, int iJmpCodeSize = 5, int stackArguments = -1);

    virtual ~Simple() override = default;

    bool CheckLibFnForChangesAndStore(void* expected);
    void ApplyJumpToGTACode();
    void GenerateECXPreservationThunk(int stackArguments);
    auto GetHookGTAAddress() const { return (void*)m_iRealHookedAddress; }
    auto GetHookOurAddress() const { return (void*)m_iLibFunctionAddress; }

    void        Switch() override;
    void        Check() override;
    const char* Symbol() const override { return "S"; }
};

};
};
