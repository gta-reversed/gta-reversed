#pragma once

namespace ReversibleHooks {
namespace Constants {
constexpr auto  NOP_OPCODE                = 0x90U;        //!< Opcode for `nop` instruction for x64 and x86
constexpr uint8 JUMP_OPCODE               = 0xE9U;        //!< Opcode for `jmp [rel32]` instruction for x64 and x86
constexpr auto  JUMP_OP_SIZE              = 5U;           //!< Size of `jmp [rel32]` (0xE9) instruction for x64 and x86
constexpr auto  MAX_GTA_JUMP_SEARCH       = 4U;           //!< Maximum number of bytes to search for a jump instruction in GTA code
constexpr auto  FUNCTION_PADDING_MAX_SIZE = 52u;          //!< Maximum size of padding for redirection
constexpr auto  MAX_HOOK_DATA_SIZE        = JUMP_OP_SIZE; //!< Maximum size of a hook (The maximum size of data we write to the hooked function) - Can't increase this a lot because of the way Edit&Continue (I mean, it's possible to detect with, but I'd rather not deal with that)
};
};
