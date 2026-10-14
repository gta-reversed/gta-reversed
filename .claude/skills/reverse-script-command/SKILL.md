---
name: reverse-script-command
description: >
  Implement (reverse) a GTA:SA SCM script command/opcode in gta-reversed's command system
  (source/game_sa/Scripts/Commands/*.cpp) - locate the original `case` in CRunningScript::ProcessCommandsXXXToYYY
  with script_cmd.py, write a typed C++ handler using the ReadArg/StoreArg conventions, document it with the opcode
  doc-comment block and register it with REGISTER_COMMAND_HANDLER. Use for any request mentioning an opcode
  (e.g. 0x02AB) or a COMMAND_* name, or when asked which script commands are left.
argument-hint: "<COMMAND_NAME | opcode>"
---

# Reverse a script command

Target: `$ARGUMENTS`

Read `docs/ScriptCommandReversing.md` first (ReadArg / StoreArg type tables). The fidelity rules of
`docs/ReversingRules.md` (R1-R7) apply to handlers like to any function.

## 0. Locate with `script_cmd.py`

```bash
S=.claude/skills/reverse-script-command/scripts/script_cmd.py
python $S export                      # once, with IDA open: opcode -> case address map (.claude/cache/script_cases.json)
python $S locate SET_CHAR_PROOFS      # also COMMAND_X, 0x02AB or 683: repo status (+ file:line), original case address + size
python $S locate 0x02AB --decompile   # Hex-Rays lines of JUST that case (needs IDA)
python $S locate 0x02AB --dump        # annotated disassembly of the case from the exe (no IDA)
python $S stats                       # repo status x what the original does
python $S list --status unregistered --original real --sort size --top 40   # real work, smallest first
```

Repo statuses: `HANDLER` (`REGISTER_COMMAND_HANDLER`), `NOP` (`REGISTER_COMMAND_NOP`), `UNIMPLEMENTED`
(`REGISTER_COMMAND_UNIMPLEMENTED`), `UNREGISTERED` (no registration: GTA's original dispatcher still runs it).
What the original does (`--original`): `real` (unique case code), `real-code-shared` (2-7 opcodes share it, e.g.
VAR/LVAR variants), `default` (8+ share it), `no-op` (returns `OR_CONTINUE` without reading params),
`not-implemented` (returns `OR_INTERRUPT`, the default path), `no-case`.

- **Work left = UNREGISTERED x real** (about 925 commands, see `stats`).
- `NOP x real` and `UNIMPLEMENTED x real` may be mis-registered: check them against `--decompile` before trusting them.
- `REGISTER_COMMAND_UNIMPLEMENTED` is for commands the game itself doesn't implement: it logs, debug-breaks and returns
  `OR_INTERRUPT`. An original `no-op` returns `OR_CONTINUE` instead; that matters only if a script uses it.

Done when: you have the opcode, the repo status, the commands file, and the original case's code.

## 1. Find where it goes in the repo

1. The enum value is in `source/game_sa/Enums/eScriptCommands.h`.
2. The handler goes in the `source/game_sa/Scripts/Commands/<Category>Commands.cpp` that groups similar commands
   (`*_CHAR_*` in `CharacterCommands.cpp`, vehicles in `VehicleCommands.cpp`, ...). Follow the file's existing grouping.
3. Grep `COMMAND_<NAME>\b` in `source/game_sa/Scripts` for an existing `REGISTER_COMMAND_UNIMPLEMENTED` / `NOP` line.

## 2. Read the original case

Opcodes are dispatched in blocks of 100 by `CRunningScript::ProcessCommands<N>To<N+99>` (`0x02AB` = 683 →
`ProcessCommands600To699`). `script_cmd.py locate --decompile` prints just the case. Typical shape:
```
CollectParameters(N);                                  // N input params
ped = GetPedPool()->GetAtRef(ScriptParams[0].iParam);
... logic ...
ScriptParams[0].fParam = result; StoreParameters(1);   // outputs
UpdateCompareFlag(cond);                               // boolean result
```
Translation:
- `CollectParameters(N)` → N handler arguments; `StoreParameters(K)` → K return values (`MultiRet<...>` if K > 1);
  `UpdateCompareFlag(x)` → return `bool`.
- Pool handles → `CPed&` / `CVehicle&` / `CObject&`; `T*` only if the original accepts an invalid/-1 handle.
- `CTheScripts::UsedObjectArray[-x]` → `script::Model`.
- `ReadTextLabelFromScript` → `const char*` / `std::string_view`.
- Mission cleanup (`if (m_bUseMissionCleanup) MissionCleanUp.AddEntityToList(...)`) → the file's helper
  (`HandleEntityMissionCleanup(S, entity)` in `CharacterCommands.cpp`).
- `CRunningScript& S` as the first parameter when the handler needs the script itself.

## 3. Write the handler

Inside the file's anonymous namespace, in the matching section:

```cpp
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
 * @returns {float} heading      (only if it returns something)
 */
void SetCharProofs(CPed& ped, bool bullet, bool fire, bool explosion, bool collision, bool melee) {
    ...
}
```

- Doc block fields follow the Sanny Builder library naming; stay consistent with neighbouring commands.
- Same side effects in the same order; original bugs kept (fixes behind `notsa::bugfixes`).

## 4. Register

In the file's `REGISTER_COMMAND_HANDLER_BEGIN("<Category>")` block:
```cpp
REGISTER_COMMAND_HANDLER(COMMAND_SET_CHAR_PROOFS, SetCharProofs);
```
- If a `REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_X);` or `REGISTER_COMMAND_NOP(COMMAND_X ...);` line exists, replace it.
- A command that is a no-op in the original: `REGISTER_COMMAND_NOP(COMMAND_X, <param types>...)`; list the parameter
  types so they are still read (e.g. `REGISTER_COMMAND_NOP(COMMAND_SHUT_PLAYER_UP, uint32, uint32);`).

## 5. Verify and report

Done when:
- `script_cmd.py locate <name>` shows status `HANDLER`;
- the build passes (`cmake --build build --config Debug --target gta_reversed`);
- you described how to trigger it: a mission or script that uses the command (search a decompiled `main.scm` if the
  user has one, or ask); a CLEO script can exercise a single opcode.

Report: opcode, name, handler signature, file, how it was tested (or why not).
