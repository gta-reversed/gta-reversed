# Reversing guide: from original code to validated C++

This guide covers how to reverse a function, end to end. It starts with finding a target and finishes with a
validated, reviewable change. It complements:
- [ReversingRules.md](ReversingRules.md): the rules every reversal must follow (R1-R20)
- [CodingGuidelines.md](CodingGuidelines.md): style rules (mandatory)
- [HookRegistration.md](HookRegistration.md): hook registration rules (mandatory)
- [Architecture.md](Architecture.md): how the project is built and injected, and how hooks work internally
- [BinaryMap.md](BinaryMap.md): layout of gta_sa.exe and real coverage of the game's methods
- [ai/TipsAndTricks.md](ai/TipsAndTricks.md): engine quirks, IDA idioms, known wrong addresses, recovered types
- [ai/CaseStudies.md](ai/CaseStudies.md): real reversal mistakes and their lessons
- [ai/InlineCatalogue.md](ai/InlineCatalogue.md): generated list of functions MSVC inlines, small helpers, CRT calls,
  globals without `StaticRef`, precision diffs
- [ScriptCommandReversing.md](ScriptCommandReversing.md): script command argument conventions
- [subsystems/README.md](subsystems/README.md): one guide per subsystem
- [Glossary.md](Glossary.md): terms used in the code and the docs

> AI agents: the same workflow is packaged as skills in `.claude/skills/`, mainly `reverse-function`, `ida-to-cpp`
> and `validate-against-binary`. The `gta-reverser` and `gta-reversal-reviewer` agents in `.claude/agents/` use them.

---

## 0. How the project works (why validation matters)

Every original function you reverse is installed as a **two-way reversible hook**
(`source/reversiblehooks/`). A hook has 3 states:

| State | GTA's address | Our function |
|---|---|---|
| `RedirectToOurs` (default when reversed) | patched → jumps to ours | intact |
| `RedirectToGTA` (default when `.Reversed = false`) | intact | patched → jumps to GTA |
| `Unhooked` | intact | intact |

A reversed function goes live for every player as soon as it's hooked. A subtle difference, such as `<` instead of
`<=` or `3.14159f` instead of `3.1415927f`, silently changes game behaviour. Mismatches are rarely visible right away.
That's why every reversal goes through the validation levels in §5.

Addresses are those of **GTA:SA 1.0 US**. The *compact* exe the tools use has the same layout; it must be exactly
5,189,632 bytes.

---

## 1. Pick a target

```bash
python .claude/skills/find-reversing-targets/scripts/list_unreversed.py --top 30          # most missing per file
python .claude/skills/find-reversing-targets/scripts/list_unreversed.py --almost-done      # 1-3 left: quick wins
python .claude/skills/find-reversing-targets/scripts/list_unreversed.py --filter Ped --details
```
`list_unreversed.py` only sees hooks that are already registered. To start from the binary instead (needs the IDB
export, see [BinaryMap.md](BinaryMap.md)):
```bash
python .claude/skills/find-reversing-targets/scripts/idb_coverage.py --classes 40         # classes with most work left
python .claude/skills/find-reversing-targets/scripts/idb_coverage.py --untouched 30       # classes not in the repo at all
python .claude/skills/find-reversing-targets/scripts/class_dossier.py CGarage             # everything about one class
```

Good first targets are **leaf functions**: they call few or no unreversed functions, are under about 150 instructions
and run somewhere easy to reach in-game. Avoid `__usercall` functions, mid-function hooks and save/load code until you
are comfortable.

---

## 2. Read the original

### With IDA (recommended)
Open the database and decompile the function. Also read its **callees**: which ones already exist in the repo?
`Grep` for their addresses. Read 1-2 **callers** too: they tell you what the arguments and the return value mean.

### Without IDA: annotated disassembly
```bash
python .claude/skills/validate-against-binary/scripts/asm_validate.py 0x53CB50 --dump --no-check
```
The output is the original disassembly from `gta_sa.exe`, annotated with:
- callee names resolved from the repo (`; CEntity::RegisterReference`), CRT helpers (`_ftol` = float→int cast) and
  implicit constructors/destructors (identified by the vtable they write)
- global variable names (`StaticRef`s), including `name+offset` for fields of global objects and arrays
- float/double constants read from `.rdata` (`; = 180.0f`)
- strings, vtables and switch jump tables
- **decoded x87 comparisons**: `jne 0x509c76  ; jump if st(0) <= 3.1415927410125732f`

Hex-Rays often gets x87 comparisons backwards, so take them from the disassembly. The cheat-sheet for the
`fcom` / `fnstsw` / `test ah` pattern is in [ai/TipsAndTricks.md](ai/TipsAndTricks.md) §3.

---

## 3. Write the C++

Follow the translation patterns in `.claude/skills/ida-to-cpp/SKILL.md` and the style in CodingGuidelines.md. The
fidelity rules are R1-R7 in [ReversingRules.md](ReversingRules.md). In short:
- the same calls (RNG calls included), writes and early returns, in the same order (R1, R2)
- the same constants, bit for bit (R3)
- the same comparisons, integer widths and float precision (R4)
- original bugs stay, unless a `notsa::bugfixes` entry gates the fix (R6)
- `// 0xADDR` above the function (R13), and address comments inside long functions

Before reversing something similar, read the matching case in [ai/CaseStudies.md](ai/CaseStudies.md).

---

## 4. Register the hook

The details are in HookRegistration.md and rules R8-R12. In short:
- Remove `{ .Reversed = false }` from the function you reversed.
- Keep it on everything that still calls `plugin::Call*<own address>` (R9).
- Use `RH_ScopedVirtualClass` + `RH_ScopedVMTInstall` only for functions in that class's vtable (R10).
- A new class needs `InjectHooks()`, plus a call to it from `InjectHooksMain()` (R8). Start the game once to check
  that the registration doesn't throw (R11).

Check it:
```bash
python .claude/skills/review-reversal/scripts/check_hooks.py           # files changed vs master
```

---

## 5. Validate against the binary

Do the levels in order. Each one catches mistakes the others miss.

### Level 1: static fingerprint (seconds, no game needed)

```bash
# one-time: path of the COMPACT exe, and the disassembler
echo "D:/Games/GTA San Andreas/gta_sa_compact.exe" > .claude/gtasa_exe_path.txt
pip install capstone

python .claude/skills/validate-against-binary/scripts/asm_validate.py CGeneral::LimitAngle
python .claude/skills/validate-against-binary/scripts/asm_validate.py --file source/game_sa/Core/Rect.cpp
```

The script disassembles the original function and checks it against our C++ body. It also follows the helpers our
code calls, 2 levels deep, because the original often inlined them. It checks that:
- every function the original calls is also called by ours
- every global it touches is referenced, by name or through an accessor (`ms_fTimeStep` → `GetTimeStep()`)
- every float constant appears in our code **with the same float32 bits**. Near-misses are reported with their ULP
  distance
- every string literal it uses is present

Example of a real finding, from before the constants in `common.h` were made exact:
```
[BAD] CGeneral::GetATanOfXY @ 0x53CC70 -> 4 finding(s)
  [MISSING] 1.5707963705062866 - ours only has 1.57079506 (11 ULP off): PRECISION MISMATCH
```
See `.claude/skills/validate-against-binary/SKILL.md` for every result tag. A clean result is necessary but not
sufficient, because the script does not check control flow ([ai/CaseStudies.md](ai/CaseStudies.md) §1).

### Level 2: differential tests (exact behaviour, runs in the game process)

The framework is `source/difftests/DiffTest.hpp`, with examples in `source/difftests/tests/`. While a test runs, the
functions under test are `Unhooked`. `GTAFn<Sig>(0xADDR)` then runs the **original machine code**, and a normal C++
call runs **ours**. Both get the same inputs, generated by a seeded RNG plus hand-picked edge cases, and the results
are compared:

```cpp
NOTSA_DIFFTEST("CGeneral::LimitAngle", "Global/CGeneral/LimitAngle") {
    Fuzz(ctx, "LimitAngle", 5'000,
        [](Context& c) { return std::tuple{ c.RandomFloat(-2000.f, 2000.f) }; },
        GTAFn<float(__cdecl*)(float)>(0x53CB00),
        &CGeneral::LimitAngle);
}
```

Helpers:
- `CompareCall`: compare return values for one input
- `Fuzz`: compare return values for many generated inputs
- `CompareFloats`: compare float arrays (matrices, vectors) with a tolerance
- `CompareObjects`: compare two object copies byte by byte, for mutating member functions. It reports differing
  offsets, which you map to members with `VALIDATE_OFFSET`
- `CompareSideEffects`: snapshot memory regions, run the original, restore, run ours, then compare the regions
- `NOTSA_DIFFTEST_INGAME`: for tests that need a loaded world

Run the tests:
- **In-game:** F7 → Tools → Diff tests → Run all
- **Automated:** `gta_sa.exe --run-difftests[=filter] --difftests-exit` (optionally `--difftests-out=<file>`). Tests
  run right after the hooks are installed, which happens inside `DllMain` before the game has initialised anything,
  so only self-contained tests are run there (`NOTSA_DIFFTEST_INGAME` tests are skipped). With `--difftests-exit`,
  the process terminates with exit code 1 if any test failed, 0 otherwise
- The report goes to `difftests.log` in the working directory. The ASI loader sets that to `scripts/`, so look in
  `<game>/scripts/difftests.log`

Guidelines:
- Test every comparison boundary you saw in `--dump`, exactly at the boundary and one step either side
  (`std::nextafter`)
- Comparisons are bit-exact by default. Use `Tolerance{ .Ulps = N }` only for transcendental math (x87 vs SSE) or
  x87 extended precision, and comment why
- Don't diff-test functions that consume the RNG: GTA's CRT `rand` and ours are different generators
- Don't diff-test functions with irreversible side effects unless you snapshot and restore the state. Never diff-test
  `__usercall` functions

### Level 3: in-game A/B

Reproduce the scenario: the mission, place, vehicle or menu where the code runs. Then toggle the hook in the F7 hooks
GUI between ours and GTA's. Watch the behaviour, visuals, asserts and log.

---

## 6. Review & PR

Go through the checklist of rule R16 (build, `check_hooks.py`, `asm_validate.py --file`, difftests, in-game test)
and the review checklist in `.claude/skills/review-reversal/SKILL.md`, then open the PR. In the PR description,
state which validation levels you ran and how you tested in-game. If you couldn't test something, say so.
