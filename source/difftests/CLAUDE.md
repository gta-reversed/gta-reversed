# difftests/ — differential tests (ours vs the original machine code, in-process)

Guide: [../../docs/ReversingGuide.md](../../docs/ReversingGuide.md) "Level 2: differential tests"; skill
`/validate-against-binary`.

## Layout
- `DiffTest.hpp` — API with doc comments (macros, `Context`, `Tolerance`, compare helpers, `GTAFn<Sig>(0xADDR)`).
- `DiffTest.cpp` — runner and report (`RunAll`). Command-line wiring: `source/extensions/CommandLine.cpp`.
- `tests/GeneralDiffTests.cpp` (fuzzing scalar functions), `tests/VectorMatrixDiffTests.cpp` (`CVector`/`CMatrix`,
  `MakeMatrixPair`, `CompareFloats`), `tests/RectDiffTests.cpp`. New `tests/*.cpp` files come from the CMake glob:
  re-run the configure step after adding one.

## Conventions
- `NOTSA_DIFFTEST("Class::Fn", "<Category>/<Class>/<Fn>", ...)`: the extra args are hook paths as shown in F7 → Hooks
  (overloads carry their suffix, e.g. `"Core/CMatrix/SetScale-fff"`). Those hooks are unhooked during the test, so
  `GTAFn<Sig>(0xADDR)` runs the original and a normal call runs ours. Callees keep their current state.
- `NOTSA_DIFFTEST_INGAME` when the function needs the world, pools or player.
- Helpers: `CompareCall`, `Fuzz(ctx, label, n, gen, original, ours, tol)`, `CompareObjects`/`CompareBytes` (with
  `ignore` ranges), `CompareFloats` (ULP report), `MemorySnapshot`/`CompareSideEffects` (globals), `ctx.Check`, `ctx.Note`.
- Inputs: `ctx.RandomFloat/RandomInt/RandomVector` (seeded, reproducible) and `ctx.InterestingFloat()` (0, -0, ±1,
  ±PI, ±2PI, huge, tiny...). Add boundary values from the asm (`std::nextafter` on either side).
- Mutating member functions: start both objects from byte-identical copies (`memcpy`, see `MakeMatrixPair`) and give
  out-params the same initial value.
- Comparisons are bit-exact by default. `Tolerance{ .Ulps = N }` (plus `.Abs` near 0) only where x87 (original) vs
  SSE (ours) legitimately differs (sin/cos/atan/sqrt chains, cancellation); comment why.
- Run in game: F7 → Tools → Diff tests. Unattended: `gta_sa.exe --run-difftests[=filter] --difftests-exit`
  (optional `--difftests-out=<file>`); exit code 1 if any test failed.

## Pitfalls
- Unattended runs happen inside `DllMain` right after hook installation (loader lock, game not initialised):
  `_INGAME` tests are skipped there. Use the F7 runner for them.
- The report goes to `<game>/scripts/difftests.log`: the ASI loader sets the working directory to `scripts/`.
- Unity build: keep helpers in `namespace {}` with names unique across test files.
- Only generate inputs the original is defined for: valid enum/flag combinations (e.g. the 6 Euler orders in the
  `ConvertToEulerAngles` test), non-null pointers, domains its callers use. Include special cases the original
  handles explicitly (e.g. the zero vector in `Normalise`).
- A mismatch beyond a few ULP is a bug (wrong constant, float vs double), not noise.
- Don't diff-test functions that consume `rand()` (different CRT generators), `__usercall` functions, or functions
  with side effects you don't snapshot and restore.
