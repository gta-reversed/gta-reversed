# Script command reversing

How to implement an SCM script command (opcode) in `source/game_sa/Scripts/Commands/`. How the script VM dispatches
commands: [Architecture.md](Architecture.md) §9 and [subsystems/Scripts.md](subsystems/Scripts.md). Step-by-step
workflow (find the original `case`, write, register): the `/reverse-script-command` skill.

## 1. How it works

- A command handler is a plain C++ function. `CommandParser<Cmd, Fn>` (`Scripts/CommandParser/Parser.hpp`) reads
  one script argument per function parameter, in order, with `notsa::script::Read<T>` (`ReadArg.hpp`). It then stores
  the return value with `StoreArg` (`StoreArg.hpp`).
- Commands with no registered handler still run GTA's original `ProcessCommands<N>To<N+99>` switch.
- Find the original code and the status of a command with:

  ```bash
  python .claude/skills/reverse-script-command/scripts/script_cmd.py locate SET_CHAR_PROOFS   # or COMMAND_X, 0x02AB
  python .claude/skills/reverse-script-command/scripts/script_cmd.py list --status unimplemented --sort size
  ```

## 2. Writing and registering a handler

Handlers live in an anonymous namespace of the matching `Commands/<Group>Commands.cpp`. Each is preceded by the
opcode doc-comment block used in `CharacterCommands.cpp`:

```cpp
namespace {
/*
 * @opcode 02AB
 * @command SET_CHAR_PROOFS
 * @class Char
 * @method SetProofs
 *
 * @brief Sets the character's immunities
 *
 * @param {Char} self
 * @param {bool} bulletProof
 * ...
 */
void SetCharProofs(CPed& ped, bool bullet, bool fire, bool explosion, bool collision, bool melee) { ... }
};

void notsa::script::commands::character::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER_BEGIN("Char");
    REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_PROOFS, SetCharProofs);
}
```

Each group's `RegisterHandlers()` must be called from `CRunningScript::InjectCustomCommandHooks()`
(`Scripts/RunningScript.cpp`). `DrawCommands.cpp` and `PathCommands.cpp` define one that is currently **not** called,
so their handlers are inactive.

Registration macros (`Parser.hpp`):

| Macro | Use for |
|---|---|
| `REGISTER_COMMAND_HANDLER_BEGIN("Group")` | Once at the top of `RegisterHandlers()`. Sets the hook path `Scripts/Commands/<Group>` |
| `REGISTER_COMMAND_HANDLER(COMMAND_X, Fn)` | An implemented command. Registering the same command twice is a fatal error |
| `REGISTER_COMMAND_NOP(COMMAND_X[, ArgTypes...])` | A command that does nothing in the original; the types are read and discarded |
| `REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_X)` | A command the original game doesn't implement either. If it runs, it logs, breaks into the debugger and returns `OR_INTERRUPT` (vanilla behaviour). It is **not** a fallback to GTA's code |
| `REGISTER_UNSUPPORTED_COMMAND_HANDLER(COMMAND_X, Fn)` | Our implementation of a command the original lacks. Only active with `IMPLEMENT_UNSUPPORTED_OPCODES` (`config.h`), otherwise it acts like `UNIMPLEMENTED` |

Handler signature rules:
- Parameters are read in declaration order, one script argument each (a `CVector` reads three).
- Two parameter types read nothing: `CRunningScript&` gives the running script, and `eScriptCommands` gives the
  current command.
- Return `void` to store nothing, `OpcodeResult` to control the script yourself, or any type from §4 to store it.
  Return several values with `MultiRet<Ts...>` (or `return_multiple(...)`).

## 3. Reading arguments (`ReadArg.hpp`)

- By value: a copy. Use a pointer or reference when you need to modify the script variable.
- Pointer = nullable (`nullptr` for an invalid handle). Reference = must exist; a null value asserts. A null
  reference usually means the parameter type doesn't match the original (e.g. `CVehicle&` instead of `CPed&`).
- Don't take complex types (`CPed`, `CVehicle`, ...) by value. It either fails to compile or makes a needless copy.

| Type | Nullable? | Behaviour |
|---|---|---|
| `CRunningScript` | No | The script running this command (reads nothing) |
| Any arithmetic type | No | By value: a constant, variable or array element (integer constants are range-checked in debug builds; a float constant read into an integer parameter breaks into the debugger). `T&`/`T*`: the variable or array element itself |
| Any enum type | No | Reads the underlying integer type, then casts |
| `script::Model`\* | No | `eModelID`: the value if ≥ 0, otherwise `CTheScripts::UsedObjectArray[-value].nModelIndex` |
| `script::Hash` | No | An `int32` argument reinterpreted as `uint32`, for values that don't fit a signed type (e.g. the clothes hashes in `GivePlayerClothes`) |
| `CPlayerPed` | No | `FindPlayerPed(<int32 arg>)` |
| `CPlayerInfo` | No | `FindPlayerInfo(<int32 arg>)` |
| Any pooled type (`CPed`, `CVehicle`, `CObject`, ...) | Yes (invalid handle) | `PoolOf<T>().GetAtRef(<int32 handle>)` |
| `ScriptEntity<T>` | Yes | Same as a pooled type, but keeps the handle too (`.e` entity, `.h` handle) |
| Script things (`C2dEffect` and other script-owned objects) | Yes (invalid) | Packed index + ID; `nullptr` if the handle is `-1`, the slot is inactive, or it was reused |
| `scm::StringRef` | No | Reads a string constant, variable or array element; a **mutable** reference whatever the string type |
| `std::string_view`, `notsa::ci_string_view` | No | Same, as an **immutable** view |
| `const char*` | No | Same as `std::string_view`, copied into a buffer so it is null-terminated |
| `CVector` | No | Three floats |
| `CVector2D` | No | Two floats |
| `CRect` | No | Two `CVector2D`: (minX, minY), (maxX, maxY), or top-left then bottom-right |
| `CRGBA` | No | Four `uint8` |

\*: Use `script::Model` only where the original reads `UsedObjectArray`; use `eModelID` otherwise.

## 4. Storing results (`StoreArg.hpp`)

| Return type | Behaviour |
|---|---|
| `bool` | Not stored: updates the script's compare flag (the code marks this as not fully verified) |
| `CompareFlagUpdate` | Updates the compare flag explicitly |
| Any arithmetic type | Zeroes the target variable/array element, then copies the value (which may be smaller than 4 bytes) |
| Any enum type | Stored as its underlying integer type |
| Any pooled type (pointer or reference) | Stores the entity's handle; `-1` for `nullptr` |
| `CVector` | Three floats |
| `CVector2D` | Two floats |
| `MultiRet<T...>` | Each value separately, in order |
