---
name: review-reversal
description: >
  Review checklist + automated checker for reversed GTA:SA code in gta-reversed - hook registration
  (InjectHooks/InjectHooksMain/.Reversed=false/self-recursive plugin::Call/vtable addresses), fidelity against the
  original binary, memory layout asserts, bug-fix gating and CodingGuidelines. Use before committing, before a PR,
  or when asked to review/validate a reversal. The gta-reversal-reviewer agent runs this skill.
argument-hint: "[files... | --base <git ref>]"
---

# Review a reversal

Arguments: `$ARGUMENTS` (files to check; empty = everything changed vs `master` plus uncommitted changes).

Cite the rule (R1-R20 of `docs/ReversingRules.md`) for every finding.

## 1. Automated checks

```bash
python .claude/skills/review-reversal/scripts/check_hooks.py $ARGUMENTS
```
Regex heuristics: confirm each finding before acting on it. Exit code 1 if any `[E]`.

| Finding | Meaning | Usual fix |
|---|---|---|
| `[E] X::InjectHooks() is never called` | the class's hooks are never installed | `#include` + `X::InjectHooks();` in `InjectHooksMain()` |
| `[E] address 0x... is already installed` | duplicate install in one file | fix the wrong address |
| `[E] ... still forwards to GTA via plugin::Call but is installed as reversed` | **infinite recursion** (R9) | add `{ .Reversed = false }` or finish the function |
| `[E] 0x... is not in X's GTA vtable` | `RH_ScopedVMTInstall` address outside the vtable; `VMTInfo` throws at startup (R10-R11) | fix the address / vtable / slot count, or use `RH_ScopedInstall` |
| `[E] InjectHooks is private but there is no friend` | won't compile | `friend void InjectHooksMain();` |
| `[W] ... marked .Reversed = false but its body has no plugin::Call` | reversed but the flag was forgotten? | remove the flag if the body is complete |
| `[W] annotated with 0x... but no RH_*Install uses that address` | function never hooked (R8) | add an install, unless inlined/unused in the original |
| `[W] RH_ScopedVMTInstall requires RH_ScopedVirtualClass` | wrong scope macro | `RH_ScopedVirtualClass(Cls, vtbl, nVirtFns)` |
| `[W] X::InjectHooks() is commented out in InjectHooksMain` | hooks disabled on purpose? | confirm with the author |

Other modes: `--base origin/master` (changed files vs another ref); `--all` (whole code base, many pre-existing
warnings; for clean-up tasks, not PR review). The same checker runs after every Edit/Write
(`.claude/hooks/post_edit_check_hooks.py`) and on PRs (`.github/workflows/hook-registration.yml`).

Then validate every touched function against the binary (**validate-against-binary**):
```bash
python .claude/skills/validate-against-binary/scripts/asm_validate.py --file <changed .cpp>   # summary per hook
python .claude/skills/validate-against-binary/scripts/asm_validate.py Class::Function         # details
```
Every `[MISSING]` blocks until explained; a `PRECISION MISMATCH` always blocks.

Related: `python .claude/skills/review-reversal/scripts/add_missing_hooks.py [--file F] [--apply]` registers
`plugin::Call` stubs that have no hook as `{ .Reversed = false }` (dry run by default; skips virtuals, ctors/dtors and
classes whose `InjectHooks()` lives elsewhere).

## 2. Manual checklist

### Fidelity (R1-R7, most important)
- [ ] Original decompiled at each installed address (IDA `decompile`/`disasm`, or `asm_validate.py --dump`) and
      compared line by line.
- [ ] `asm_validate.py`: no unexplained `[MISSING]`, no `PRECISION MISMATCH`.
- [ ] Testable functions have a passing `NOTSA_DIFFTEST` (boundary inputs + fuzzing; any tolerance justified).
- [ ] Side-effecting calls: all present, same order, same arguments (including `this`).
- [ ] Conditions: `<`/`<=`, signedness, x87 compare direction (ida-to-cpp §3), `&&`/`||` order.
- [ ] Constants bit for bit; no rounded literal where the binary loads the exact value, and vice versa.
- [ ] Integer widths and truncation (`int8`/`uint16` members, `_ftol`, sign extension); `double` where the original
      accumulates in x87 registers.
- [ ] RNG calls: same number, same order.
- [ ] Vehicle type vs sub-type (R5).
- [ ] Every return path returns what the original returns.
- [ ] Deviations gated (`notsa::bugfixes::*` / `notsa::IsFixBugs()`) and commented; new entries in
      `source/reversiblebugfixes/Bugs.hpp`.

### Hooks (R8-R12, `docs/HookRegistration.md`)
- [ ] `static void InjectHooks();` exists and is called in `source/InjectHooksMain.cpp`.
- [ ] `friend void InjectHooksMain();` only when needed.
- [ ] Every function with an address has an `RH_*Install`; addresses verified (IDA or `docs/ai/Vtables.md`).
- [ ] Stubs keep `{ .Reversed = false }`; reversed functions don't.
- [ ] Virtuals: `RH_ScopedVirtualClass` with the right vtable and count; `.Overrides = false` when not overridden.
- [ ] Register-argument functions: `StackArgumentsToPreserve` / `PreserveRegisters`.
- [ ] Changed signatures hook a `_ToHook` wrapper.

### Layout (R13-R15)
- [ ] `VALIDATE_SIZE`/`VALIDATE_OFFSET` present and valid; no reordering; padding kept.
- [ ] `StaticRef` addresses match IDA / `docs/ai/Globals.md`; `constexpr` tables only if the game never writes them.

### Guidelines (`docs/CodingGuidelines.md`) and scope (R17)
- [ ] Naming, `auto` rules, `f` suffix, fixed-width ints, ranges, `std::array`, SA types, `RWRSTATE`, `override`.
- [ ] `// 0xADDR` above every function; address comments in long functions.
- [ ] No unrelated changes or reformatting.

### Build & test (R16)
- [ ] `cmake --build build --config Debug --target gta_reversed` passes (or the failure is reported).
- [ ] In-game test plan: where the code runs, and the hook toggled in the F7 hooks GUI for an A/B comparison.

## 3. Output

Use the output format of the **gta-reversal-reviewer** agent (`.claude/agents/gta-reversal-reviewer.md`): verdict,
validation status lines, Blocking, Non-blocking, Suggested in-game test. Give file:line, rule and the exact fix for
every blocking issue.
