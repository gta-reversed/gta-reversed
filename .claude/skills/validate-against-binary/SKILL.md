---
name: validate-against-binary
description: >
  Validate reversed GTA:SA code against the ORIGINAL machine code of gta_sa.exe, in two layers:
  (1) static - asm_validate.py disassembles the original function from the exe and checks callees, globals,
  float constants (bit-exact) and strings against our C++, and prints annotated disassembly with decoded x87 compares;
  (2) dynamic - NOTSA_DIFFTEST differential tests that call the original and our implementation with the same inputs
  inside the game and compare results. Use after reversing/changing a function, when reviewing, when a reversal
  "behaves differently", or to read a function's asm without IDA.
argument-hint: "<Class::Function | 0xADDRESS | --file path.cpp>"
---

# Validate a reversal against the binary

Target: `$ARGUMENTS`

Do every level that applies, in order; each catches different bugs. Background: `docs/ReversingGuide.md` §5 and
rules R1-R4, R16 of `docs/ReversingRules.md`.

| Level | Tool | Catches | Cost |
|---|---|---|---|
| 1. Static fingerprint | `asm_validate.py` | missing/extra calls, wrong or imprecise constants, unmapped globals, wrong strings | seconds, no game |
| 2. Differential test | `NOTSA_DIFFTEST` | any behavioural difference for the tested inputs: branches, math, precision, out-params, side effects | write a test + run the game |
| 3. In-game A/B | F7 hooks GUI | integration, visuals, timing | manual |

## Level 1 - static fingerprint (`asm_validate.py`)

Setup once: the **compact** exe (5,189,632 bytes) path in `.claude/gtasa_exe_path.txt` (one line), or env var
`GTASA_EXE`, or `--exe`. Needs `capstone` (`pip install capstone`). Read-only.

```bash
A=.claude/skills/validate-against-binary/scripts/asm_validate.py
python $A CGeneral::LimitAngle                                # one function, by name
python $A 0x53CB00                                            # ... or by address
python $A --file source/game_sa/General.cpp                   # every reversed hook of a file, summary
python $A 0x53CB50 --dump --no-check                          # annotated disassembly only (reversing without IDA)
python $A 0x46F800 --cpp-file source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h --cpp-func ControlSubTask
                                                              # our implementation isn't found automatically
```
Other flags: `--size N` (function extent heuristic failed), `--helper-depth N` (default 2), `--strict` (`[CHECK]`
also fails), `--quiet` (only MISSING/CHECK lines).

How it works: it disassembles the original from the PE file and maps every call target and data address to repo
symbols (`RH_*Install`, `// 0xADDR` comments, `StaticRef`s, `plugin::Call` wrappers). It searches our body **and the
helpers it calls (2 levels)**, because the original often inlined what we call through helpers.

| Tag | Meaning | What to do |
|---|---|---|
| `[MISSING] 0x... X::Fn - not called by our code` | the original calls it; ours (and its helpers) don't | real omission, or equivalent inline code: check the call site in `--dump` and `docs/ai/InlineCatalogue.md` |
| `[MISSING] 0x... - not in the repo` | callee unknown to the repo | declare it (stub + `{ .Reversed = false }` hook) and call it, or reverse it |
| `[MISSING] ... PRECISION MISMATCH (N ULP off)` | our nearest constant is 3+ ULP from the binary's | use the exact value the binary loads: **real behaviour difference** (R3) |
| `[MISSING] ... global without StaticRef in the repo` | global with no `StaticRef` | add `StaticRef<T>(0xADDR)` (name/type from IDA or `docs/ai/Globals.md`) or map it to the right variable |
| `[MISSING] could not find our C++ implementation` | name lookup failed | pass `--cpp-file` / `--cpp-func` |
| `[CHECK] ... (1-2 ULP off)` | constant computed differently (`x / 1.1f` vs `x * 0.90909f`) | usually fine; may differ in the last bit |
| `[CHECK] ... not found as a literal/named constant` | constant not visible in our code | computed elsewhere? wrong value? |
| `[CHECK] small leaf helper` | tiny callee not in the repo (CVector ops, POD ctors) | usually an operator in ours; `--dump 0xADDR` it if unsure |
| `[CHECK] _ftol` / CRT/library function | float→int cast / C runtime call | ours casts / calls the `std` equivalent |
| `[CHECK] constructor/destructor of X` | implicit ctor/dtor of a local, temporary or base | normally implicit in C++; verify object lifetime |
| `[CHECK] member/element of g` | access into a global object/array | same field/index accessed? |
| `[INFO] virtual call ... slot N` | vtable call | ours calls the virtual at that slot |

Exit code: 0 clean, 1 findings, 2 environment error.

A clean result is **necessary, not sufficient**: control flow is not checked. Always read `--dump` next to our C++
for branch conditions (the `; jump if st(0) <= X` annotations decode x87 compares).

Whole repository: `validate_all.py [--filter X]` runs Level 1 on every reversed hook (~0.3 s each) into
`.claude/cache/validate_all.jsonl`; `inline_catalogue.py --out docs/ai/InlineCatalogue.md` regenerates the catalogue.
Only when asked.

Done when: no `[MISSING]` is left unexplained, no `PRECISION MISMATCH`, every `[CHECK]` has a reason, and you
compared the control flow with `--dump`.

## Level 2 - differential tests (`NOTSA_DIFFTEST`)

API and conventions: `source/difftests/CLAUDE.md` and `source/difftests/DiffTest.hpp`. Examples:
`source/difftests/tests/GeneralDiffTests.cpp` (fuzzing scalar functions), `RectDiffTests.cpp` (mutating member
functions), `VectorMatrixDiffTests.cpp` (`CompareFloats`).

How it works: the hooks named in the test are **Unhooked** while it runs, so `GTAFn<Sig>(0xADDR)` runs the original
and a normal C++ call runs ours. Callees keep their hook state.

Procedure:
1. Pick the file `source/difftests/tests/<Subsystem>DiffTests.cpp` (new files are globbed; re-run the CMake configure).
2. Write the hook path exactly as in the hooks GUI: `<Category>/<Class>/<Fn>`, overloads with their suffix
   (`Global/CGeneral/LimitAngle`, `Core/CRect/Restrict`, `Global/CGeneral/GetRadianAngleBetweenPoints-`).
   A wrong path makes the test *Skipped*.
3. Choose the comparison:
   - pure function → `Fuzz(ctx, label, n, gen, GTAFn<...>(0xADDR), &Ours)` plus `CompareCall` for hand-picked inputs;
   - mutating member → run each on a byte-identical copy, then `CompareObjects`;
   - out-params → `ctx.Check` + `ApproxEqual`;
   - globals → `CompareSideEffects` (snapshot, run original, restore, run ours, compare);
   - needs the world (pools, peds, player) → `NOTSA_DIFFTEST_INGAME` (F7 menu only).
4. Inputs: every constant from the `--dump` compares, exactly at it and one step either side (`std::nextafter`);
   `0`, `-0`, negatives, huge values; `ctx.InterestingFloat()` and seeded `ctx.Random*`. Respect preconditions
   (valid enums, non-null pointers). Keep fuzz counts ≤ 10k.
5. Tolerance: bit-exact by default. `Tolerance{ .Ulps = N }` only for transcendental chains (x87 vs SSE), with a
   comment. Never raise it to hide a Level 1 constant mismatch.
6. Run: F7 → Tools → Diff tests → Run all, or unattended
   `gta_sa.exe --run-difftests[=filter] --difftests-exit [--difftests-out=<file>]`. Unattended runs happen inside
   `DllMain` before the game initialises (`_INGAME` tests skipped); exit code 1 if a test failed. The report goes to
   `<game>/scripts/difftests.log` (the ASI loader sets the working directory to `scripts/`).

Don't diff-test: functions consuming `rand()` (GTA's CRT and ours are different generators); irreversible side effects
(spawning, freeing, streaming, audio) unless snapshotted and restored; `__usercall` functions.

Done when: the test passes, or you stated why the function can't be diff-tested.

## Level 3 - in-game A/B

F7 → hooks GUI: toggle the function between ours and GTA's while reproducing the scenario (mission, place, vehicle,
menu where it runs). Watch for behaviour/visual differences, asserts and the log.

## Report

```
### Validation: <Class::Fn> @ 0xADDR
- Static (asm_validate): clean | N findings (each with a verdict)
- Diff test: <test name> - passed (N checks, seed 0x...) | failed (first mismatch: inputs, original, ours) | not written (why)
- In-game: tested <scenario> | not tested
```
