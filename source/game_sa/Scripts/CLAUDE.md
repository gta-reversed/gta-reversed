# Scripts/ — SCM virtual machine and script command handlers

Guides: [../../../docs/subsystems/Scripts.md](../../../docs/subsystems/Scripts.md) (overview, command statuses) and
[../../../docs/ScriptCommandReversing.md](../../../docs/ScriptCommandReversing.md) (ReadArg/StoreArg type tables).
To implement an opcode use `/reverse-script-command <COMMAND | opcode>` (helper
`.claude/skills/reverse-script-command/scripts/script_cmd.py locate|list|stats`).

## Layout
- `RunningScript.*` — `CRunningScript` (one script thread). `ProcessOneCommand` (`0x469EB0`) runs our handler from
  `CustomCommandHandlerOf(cmd)` if one is registered, otherwise the original block dispatcher
  `s_OriginalCommandHandlerTable[cmd / 100]` (`0x8A6168`). `InjectCustomCommandHooks()` calls each
  `commands::<x>::RegisterHandlers()` (CLEO ones only with `NOTSA_WITH_CLEO_SCRIPT_COMMANDS`).
- `TheScripts.*` — `CTheScripts` (script space, used-object array, mission cleanup, script-owned arrays).
- `Commands/<Category>Commands.cpp` — handlers grouped by category; `Commands/CLEO/` — CLEO opcodes (NOTSA);
  `Commands/Commands.hpp` declares the `RegisterHandlers`.
- `CommandParser/` — `ReadArg.hpp` (argument decoding), `StoreArg.hpp` (return values), `Parser.hpp` (macros).
- Opcode enum: `source/game_sa/Enums/eScriptCommands.h`. Results: `OpcodeResult.h` (`OR_CONTINUE`, `OR_WAIT`, `OR_INTERRUPT`).

## Conventions
- A handler is a plain function in the file's anonymous namespace with typed args: arithmetic/enum by value,
  `CPed&` / `CVehicle&` for handles that must be valid, `T*` only when the original accepts -1, `CRunningScript& S`
  first if needed, `script::Model` only where the original reads `UsedObjectArray`.
- Return `bool` = compare flag, a value = `StoreParameters`, `MultiRet<A, B>` for several outputs.
- Add the `@opcode/@command/@class/@method/@brief/@param/@returns` doc block like neighbouring handlers.
- Register inside the file's `REGISTER_COMMAND_HANDLER_BEGIN("<Category>")` block with
  `REGISTER_COMMAND_HANDLER(COMMAND_X, Fn)`. No address or `RH_ScopedInstall`: the macro registers the hook
  (category `"Scripts/Commands"`).
- Most commands left to do are simply not registered (the original case runs). If a `REGISTER_COMMAND_UNIMPLEMENTED` or
  `REGISTER_COMMAND_NOP` line exists for the opcode, replace it.
- Mission cleanup: reuse `HandleEntityMissionCleanup(S, entity)` (`CharacterCommands.cpp`) where the original calls
  `MissionCleanUp.AddEntityToList`.

## Pitfalls
- `draw::RegisterHandlers` and `path::RegisterHandlers` are declared but never called from
  `InjectCustomCommandHooks`: handlers in `DrawCommands.cpp` / `PathCommands.cpp` never run.
- `REGISTER_COMMAND_UNIMPLEMENTED` is for opcodes the original never implemented: it debug-breaks and returns
  `OR_INTERRUPT`. Some vanilla no-ops return `OR_CONTINUE` instead; use `REGISTER_COMMAND_NOP` for those.
- In the original, several opcodes share one `case` (VAR/LVAR variants); check every opcode that falls into it.
- Argument types must match the original operand kinds one to one, or the IP drifts.
