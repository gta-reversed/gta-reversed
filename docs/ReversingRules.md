# Reversing rules

These are the rules every reversed function has to follow. Each rule says why it exists and how to check it.
The detailed workflow is in [ReversingGuide.md](ReversingGuide.md), style rules are in
[CodingGuidelines.md](CodingGuidelines.md), and hook registration is in [HookRegistration.md](HookRegistration.md). If
this page and one of those disagree, the more specific page wins; please fix the discrepancy.

The rules are numbered so a review can refer to them ("R3").

---

## Fidelity

**R1. Same observable behaviour as the original.** Our function must produce the same results and the same side
effects (writes to members and globals, calls, events, sounds, random numbers) as the function at its GTA address.
"Equivalent but cleaner" is fine; "slightly different" is not.
*Why:* other original code still calls into ours and expects the original contract, and toggling a hook in the F7
menu must not change the game.
*Check:* `asm_validate.py <Class::Fn>` and, for pure functions, a `NOTSA_DIFFTEST`.

**R2. Same calls, in the same order, with the same arguments.** This includes `CGeneral::GetRandomNumber*` / `rand()`:
an extra or missing random call shifts every later random value.
*Check:* the `[MISSING]` / `[CHECK]` lines of `asm_validate.py`; confirm each one against `--dump`, since the
original may have inlined the callee (see [ai/InlineCatalogue.md](ai/InlineCatalogue.md)).

**R3. Same constants, bit for bit.** Use the value the binary loads. The named constants in `common.h` (`PI`,
`TWO_PI`, `FRAC_*`, `DEG_TO_RAD`, ...) are the exact `std::numbers` floats (`PI` = `3.1415927f`). Where the original
loads a rounded value instead, write that literal with a `// NOTE:` comment (examples in
[ai/TipsAndTricks.md](ai/TipsAndTricks.md) §2).
*Check:* `PRECISION MISMATCH` in `asm_validate.py` is a real behaviour difference unless it is 1-2 ULP.

**R4. Same comparisons, integer widths and float precision.**
- Take the comparison operators from the disassembly (`--dump`), not from the pseudocode: `<` vs `<=`, signed vs
  unsigned, and x87 `fcomp`/`fnstsw` flag tests are easy to get wrong.
- Keep the truncation of `int8`/`uint16` members and the `_ftol` (truncate toward zero) conversions.
- The original keeps intermediate float results in x87 registers (80-bit). When a value is accumulated across a loop
  without being stored, compute it in `double` (`CGeneral::LimitRadianAngle`, [ai/CaseStudies.md](ai/CaseStudies.md) §3).
- Keep the operation order. `(1 / a) * 0.5` and `1 / (2a)` round differently (`CGeneral::SolveQuadratic`,
  [ai/CaseStudies.md](ai/CaseStudies.md) §2).

**R5. Use the vehicle sub-type when the original reads it.** `m_nVehicleType` only says AUTOMOBILE, BIKE, BOAT or
TRAIN. `IsPlane()` / `IsHeli()` / `IsTrailer()` / `IsBMX()` are never true on a live vehicle. Check which offset the
original reads and use `IsSub*()` when it reads `m_nVehicleSubType`.

**R6. Reversal mistakes and original bugs are fixed in different ways.**
- If *our* code differs from the original, it is a reversal mistake: fix it to match the original and leave a
  `// NOTE:` comment when the fix is not obvious.
- If the *original game* has a bug, keep the original behaviour by default. Gate a fix behind a `notsa::bugfixes`
  entry (`source/reversiblebugfixes/Bugs.hpp`, named `Class_Function_BugName`), or `notsa::IsFixBugs()` for trivial
  cases (CodingGuidelines: *Fixing bugs*). If you don't fix it, at least leave a `// BUG:` comment.
*Why:* bug fixes have to stay switchable, so original behaviour can always be reproduced and compared.

**R7. Don't silently drop the original's error handling.** Where the original quietly bails out on an "impossible"
state, prefer an `assert` (CodingGuidelines: *Using assert*). Where the game really can recover, keep the early
return and log if useful.

## Hooks

**R8. Every function that has a GTA address gets a hook.** That is
`RH_ScopedInstall(Fn, 0xADDR)`, or `RH_ScopedVMTInstall` for virtuals, inside the class's `InjectHooks()`.
`InjectHooksMain()` must call that `InjectHooks()`.
*Why:* a function without a hook can't be toggled, isn't counted as progress, and the original code never reaches it.
*Check:* `check_hooks.py`. It runs automatically after every edit, and in CI on pull requests.

**R9. A function that still calls GTA is `{ .Reversed = false }`.** This applies to any function whose body is
`plugin::Call*<its own address>`. Installed as reversed, the hook redirects GTA to our function, which calls GTA,
which redirects to ours, forever. Hooks that are already
`{ .State = HS::RedirectToGTA, .Locked = true }` are fine; don't unlock them.
*Check:* `check_hooks.py` reports a self-call without `.Reversed = false` as an error.

**R10. `RH_ScopedVMTInstall` only for functions that are in that class's vtable.** `VMTInfo` throws when the address
isn't in the vtable named by `RH_ScopedVirtualClass(cls, vtable, slots)`. Static callbacks, non-virtual helpers and
inherited functions use `RH_ScopedInstall`. Look the vtable up in [ai/Vtables.md](ai/Vtables.md).

**R11. Nothing on the `InjectHooksMain` path may throw or wait.** It runs inside `DllMain` with the loader lock held. An
exception there freezes the game at the console with no message (see [ai/CaseStudies.md](ai/CaseStudies.md) §8). Test
every new registration by starting the game once.

**R12. Don't hook a function whose signature we changed.** If our C++ signature differs from the original ABI (for
example, it returns a struct where the original takes an out-parameter), hook a wrapper with the original signature.
The convention is a `Fn_ToHook` or `Fn_Hooked` member next to the function, as in
`CTaskSimpleCarJumpOut::ComputeAnimID_ToHook`. Leave a comment saying so. A hook commented out with an "ABI" reason
stays commented out until such a wrapper exists.

## Addresses, globals and layout

**R13. Never guess an address.** Take function and global addresses from IDA, or from
[ai/Globals.md](ai/Globals.md) and [ai/Vtables.md](ai/Vtables.md). Put the address in a `// 0xADDR` comment above each
definition.

**R14. Globals are references into the game's memory.** Declare them as
`static inline auto& ms_X = StaticRef<T>(0xADDR);`. Never use a copy, because both
the game and our code must see the same variable.

**R15. Keep layouts exact.** Every reversed struct or class has `VALIDATE_SIZE`, and `VALIDATE_OFFSET` for fields
you rely on. Don't reorder, resize or remove fields; keep unknown bytes as padding arrays (`char _pad[N];`, numbered `_pad2`, `_pad10`... when there are several) instead.
*Check:* `struct_check.py` compares the repo with the IDB types; known mismatches are in
[BinaryMap.md](BinaryMap.md) §3.

## Validation before a pull request

**R16. Validate what you changed.** Before opening a PR:
1. Build with no warnings: `cmake --build build --config Debug --target gta_reversed`.
2. `python .claude/skills/review-reversal/scripts/check_hooks.py --base origin/master` must report 0 errors.
3. Run `python .claude/skills/validate-against-binary/scripts/asm_validate.py --file <changed .cpp>` and explain or
   fix every finding.
4. For pure functions (math, geometry, lookups), add or extend a `NOTSA_DIFFTEST` and run
   `gta_sa.exe --run-difftests=<filter> --difftests-exit`.
5. Test the affected feature in game with the hook on and off (F7 → Hooks). Name the places or missions you used.

If something could not be tested, say so in the PR description (CodingGuidelines: *Contributing*).

## Scope and hygiene

**R17. One subject per pull request.** Don't touch unrelated code, don't reformat files, don't rename things in passing.

**R18. Generated files are regenerated, not edited.**
- `docs/ai/InlineCatalogue.md`, `Vtables.md`, `Globals.md`, `Enums.md` and `FrameCallGraph.md` come from the scripts
  named at the top of each file.
- `docs/ReversedClasses.md` and `docs/hooks.json` come from CI.

**R19. Local data stays local.**
- `.claude/cache/` (exports of a local IDB) and `.claude/gtasa_exe_path.txt` are gitignored; don't commit them.
- Scripts that talk to IDA are read-only, except `repo_to_idb.py --apply`. Never save someone's IDB without asking.

**R20. Keep the docs true.** When you fix something listed in a *Gotchas* section of
[subsystems/](subsystems/README.md), mark it **(Fixed: ...)** in that section. When a class gets much further,
refresh the numbers with `idb_coverage.py`.
