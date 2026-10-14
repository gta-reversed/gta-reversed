---
name: reverse-function
description: >
  Step-by-step workflow to reverse ONE GTA:SA function in gta-reversed - from address/name to faithful C++,
  correct RH hook registration, validation against the binary and a test plan. Use when asked to
  reverse/implement/finish a function or method, replace a `plugin::Call*` stub, or flip `.Reversed = false`.
  Argument: function name (e.g. `CCam::LookBehind`) or GTA address (e.g. `0x520690`).
argument-hint: "<Class::Function | 0xADDRESS>"
---

# Reverse a single function

Target: `$ARGUMENTS`

Work through the phases in order. Never skip Phase 2 or Phase 5. Rules: `docs/ReversingRules.md` (cite R-numbers);
background and the x87 cheat-sheet: `docs/ReversingGuide.md`.

## Phase 1 - Locate it in the repo

1. Grep `source/` for the name, and for the address case-insensitively (`0x520690`).
2. Note:
   - the **declaration** (`.h`): signature, `static`/`virtual`/`const`, access;
   - the **definition** (`.cpp`): `plugin::Call*` stub, `NOTSA_UNREACHABLE()`, partial code, or missing;
   - the **install** in `ClassName::InjectHooks()`: macro, address, options (`.Reversed = false`, `.Locked`,
     `.StackArgumentsToPreserve`...).
3. Not in the repo: add the declaration, `// 0xADDR`, the definition next to related functions, and the install.
   Whole class missing: use **reverse-class** instead.

Done when: you know the exact file:line of declaration, definition and install (or have added them).

## Phase 2 - Understand the original

With IDA (`mcp__plugin_ida-pro-mcp_idalib__*`):
1. `idb_list` → session id for `database`; none adopted → `idb_open` the user's IDB (ask for the path if unknown).
2. `lookup_funcs` with the address: it must be a function start and the name must match. Addresses are GTA:SA 1.0 US.
3. `decompile` (with addresses). `disasm` the range whenever you see:
   - `__usercall`/`__userpurge` (register args) → needs `StackArgumentsToPreserve`/`PreserveRegisters` or a wrapper;
     ask before improvising;
   - x87 compares (`fnstsw ax; test ah, 41h`) → ida-to-cpp §3;
   - `qmemcpy`/`memset` of structs, `LOBYTE`/`HIBYTE`, jump tables → ida-to-cpp §4-§8.
4. `callees`: grep each callee's address in the repo; list those that are missing.
5. `xrefs_to`: read 1-2 callers to learn what arguments and return value mean.
6. `type_inspect`, `search_structs`, `stack_frame`, `get_global_value`, `get_string` as needed. Cross-check offsets
   with the repo's `VALIDATE_OFFSET`. The repo header wins for names; if offsets disagree, stop and report.

Without IDA:
```bash
python .claude/skills/validate-against-binary/scripts/asm_validate.py 0xADDR --dump --no-check
```
Enough for small/medium functions. For big ones without IDA, say so and ask before continuing.

Also grep: `docs/ai/Globals.md` (globals it touches), `docs/ai/Enums.md`, `docs/ai/Vtables.md` (virtuals: slot and
address), `docs/ai/InlineCatalogue.md` (code MSVC inlined), and read the subsystem guide in `docs/subsystems/`.

Done when: you can name every callee, global, constant and branch condition of the original.

## Phase 3 - Write the C++

Translate with the **ida-to-cpp** skill. Checklist:
- `// 0xADDR` directly above the definition; `// 0xADDR` at important blocks of long functions.
- Same side-effecting calls in the same order, same early-outs, same RNG calls (R1, R2).
- Same constants bit for bit (R3); comparisons taken from the disassembly (R4); vehicle sub-type where read (R5).
- Inlined engine code → existing helper (`GetPosition()`, `GetIsStatic()`, `FindPlayerPed()`, `RWRSTATE(...)`...).
- Magic numbers → existing enums; else `// TODO: enum`.
- Globals → existing `StaticRef`; new ones `static inline auto& ms_X = StaticRef<T>(0xADDR);` (class) or
  `static auto& s_X = StaticRef<T>(0xADDR);` (used in one function only).
- Original bug: keep it with `// BUG:`; a fix goes behind `notsa::bugfixes::<Class_Function_BugName>` (new entry in
  `source/reversiblebugfixes/Bugs.hpp`) or a generic one (`GenericCrashing`, `GenericOOB`, `GenericUB`,
  `GenericFrameRate`) (R6).
- Callees not reversed yet: call their C++ declaration (its stub forwards to GTA). Use `plugin::Call*` inline only if
  no declaration exists. No dead `plugin::Call*` lines left in the body.

Style: `docs/CodingGuidelines.md`.

## Phase 4 - Register the hook

In `ClassName::InjectHooks()` (macros in `source/reversiblehooks/ReversibleHooks.h`):

| Situation | Install |
|---|---|
| member / static member | `RH_ScopedInstall(Fn, 0xADDR);` |
| overloaded | `RH_ScopedOverloadedInstall(Fn, "Suffix", 0xADDR, Ret(Class::*)(Args...) [const]);` |
| free function | `RH_ScopedGlobalInstall(Fn, 0xADDR);` |
| virtual | `RH_ScopedVMTInstall(Fn, 0xADDR);` inside `RH_ScopedVirtualClass(Cls, vtblAddr, nVirtFns)` |
| virtual the class doesn't override | add `{ .Overrides = false }` |
| ctor / dtor | `RH_ScopedInstall(Constructor, 0xADDR);` / `RH_ScopedInstall(Destructor, 0xADDR);` wrappers, or `RH_ScopedVMTDestructorInstall(0xADDR);` |
| register-passed args | `{ .StackArgumentsToPreserve = N, .PreserveRegisters = true }` |
| our signature differs from the ABI | hook a `Fn_ToHook` wrapper with the original signature (R12) |

- Fully reversed → **remove** `{ .Reversed = false }`.
- Still calling `plugin::Call*<own address>` → **keep** `{ .Reversed = false }` (R9: otherwise infinite recursion).
- New class → **reverse-class** steps 4-5.

## Phase 5 - Validate against the binary

Follow **validate-against-binary**. Done when all of these hold:
1. `python .claude/skills/review-reversal/scripts/check_hooks.py <your .cpp/.h>` → no `[E]`.
2. `python .claude/skills/validate-against-binary/scripts/asm_validate.py Class::Function` → no `[MISSING]` left
   unexplained, no `PRECISION MISMATCH`; every `[CHECK]` explained or fixed.
3. You re-read the C++ next to `--dump` line by line: conditions, constants, loop bounds, argument order, integer
   widths, return value on every path.
4. Testable in isolation (pure math, copyable objects, snapshot-able globals) → a passing `NOTSA_DIFFTEST` in
   `source/difftests/tests/<Subsystem>DiffTests.cpp` with boundary inputs + fuzzing. Otherwise say why not.
5. Build passes: `cmake --build build --config Debug --target gta_reversed` (re-run the configure step if you added
   files). Report failures honestly.
6. Non-trivial change → run the **gta-reversal-reviewer** agent.

## Phase 6 - Report

- `Class::Function` (0xADDR): reversed / partially reversed;
- files changed;
- hooks still `.Reversed = false`, and why;
- assumptions, `TODO`s, original bugs found (gated or not);
- validation: asm_validate result, diff test (name, checks, seed) or why none, build result;
- **in-game test**: where the code runs (mission, area, vehicle, menu) and that the hook can be toggled in the F7
  hooks GUI to A/B compare with the original.
