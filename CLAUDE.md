# gta-reversed — notes for Claude

Hook-based re-implementation of GTA: San Andreas (PC 1.0 US / compact exe, 32-bit). The DLL replaces original
functions at runtime; every reversed function must behave exactly like the original unless a documented,
switchable bug fix (`notsa::bugfixes`) says otherwise.

## Read first
- `docs/ReversingRules.md` — **the rules every reversal must follow (R1-R20)**, each with why and how to check; cite them in reviews
- `docs/Architecture.md` — build, startup flow, hook system internals, source tree, debug UI, script engine, CI
- `docs/ReversingGuide.md` — end-to-end workflow: read the original, write C++, register, **validate against the binary**
- `docs/BinaryMap.md` — gta_sa.exe layout (game code vs RenderWare/D3DX/CRT), key globals, real coverage of game methods
- `docs/CodingGuidelines.md` — style and C++ rules (mandatory)
- `docs/HookRegistration.md` — InjectHooks / InjectHooksMain, every `RH_Scoped*` macro, install options (`.Reversed`, `.Locked`, `.Overrides`...), review checklist
- `docs/ai/TipsAndTricks.md` — engine quirks, IDA idioms, known wrong addresses, recovered enums
- `docs/ScriptCommandReversing.md` — script command handlers: registration, argument/return types
- `docs/subsystems/README.md` — per-subsystem guides (world/streaming/pools, peds, tasks, events, vehicles, camera, rendering, audio, scripts, collision/physics, animation, pathfinding, weapons, HUD/frontend, save/load); most `source/game_sa/` subfolders also have a short `CLAUDE.md` that loads automatically
- `docs/ai/CaseStudies.md` — 10 real reversal mistakes (branching, operation order, x87, type vs sub-type, VMT, recursion...) and their lessons
- `docs/Glossary.md` — project, compiler, engine and script terms
- `docs/ai/InlineCatalogue.md` — functions the compiler inlined, small helpers, CRT calls, globals without StaticRef, known precision diffs
- Generated lookup catalogues in `docs/ai/` (large: **grep them, don't read them whole**): `Vtables.md` (vtable
  address, slots and per-slot status of every virtual class), `Globals.md` (address/type/users of every game global,
  split into StaticRef'd and not yet), `Enums.md` (IDB enums missing from the repo, with members),
  `FrameCallGraph.md` (per-frame call tree with unreversed bytes below each node)

## Agents (`.claude/agents/`)
- **gta-reverser** — reverses functions/classes/script commands end to end (IDA MCP → C++ → hooks → verify)
- **gta-reversal-reviewer** — read-only review of a reversal against IDA, hooks and guidelines

## Skills (`.claude/skills/`)
| Skill | Use for |
|---|---|
| `/find-reversing-targets [filter]` | what is left / what to reverse next |
| `/reverse-function <Cls::Fn \| 0xADDR>` | one function, step by step |
| `/reverse-class <Cls>` | new class, layout, vtable, InjectHooks registration |
| `/ida-to-cpp` | pseudocode → idiomatic C++ translation reference |
| `/reverse-script-command <COMMAND>` | SCM opcodes |
| `/validate-against-binary <Cls::Fn \| 0xADDR>` | static check vs gta_sa.exe (`asm_validate.py`) + `NOTSA_DIFFTEST` differential tests |
| `/review-reversal [files]` | checklist + `check_hooks.py` before commit/PR |

## Scripts (run with `python <path>`; all under `.claude/skills/*/scripts/`)
| Script | What it gives you |
|---|---|
| `find-reversing-targets/idb_coverage.py [--classes N \| --class C \| --missing N \| --untouched N]` | real coverage of game methods by status (REVERSED/DORMANT/STUB/NOHOOK/KNOWN/MISSING) |
| `find-reversing-targets/class_dossier.py <Class> [--ida]` | everything about one class: methods + status + size, vtable, fields, callers, files |
| `find-reversing-targets/struct_check.py [--type T] [--export]` | repo `VALIDATE_SIZE/OFFSET` vs IDB structs |
| `find-reversing-targets/repo_to_idb.py [--apply]` | push repo names/addresses into the IDB (dry run by default; never save the IDB without asking) |
| `reverse-script-command/script_cmd.py list\|locate\|stats` | script-command status vs original handler, pseudocode of a case |
| `review-reversal/check_hooks.py [files \| --base origin/master \| --all]` | hook registration errors (also runs automatically after every Edit/Write via the PostToolUse hook) |
| `review-reversal/add_missing_hooks.py [--apply]` | registers `plugin::Call` stubs that have no hook as `.Reversed = false` |
| `validate-against-binary/asm_validate.py <Cls::Fn\|0xADDR> [--dump]` | static compare of our C++ against the original asm |
| `find-reversing-targets/catalogues.py [--only ...]` | regenerates `docs/ai/{Vtables,Globals,Enums,FrameCallGraph}.md` (needs `ida_rpc.py export` + `export-xrefs`) |
| `validate-against-binary/validate_all.py` + `inline_catalogue.py` | whole-repo static validation → `.claude/cache/validate_all.jsonl` → `docs/ai/InlineCatalogue.md` |

Batch reversal: the named workflow `.claude/workflows/reverse-class.js` (args `{cls, max, maxBytes}`) plans a class,
runs one `gta-reverser` per function in its own worktree/branch, then `gta-reversal-reviewer`. Start it only on request
(it spawns many agents).

## Key facts
- Hooks: `RH_ScopedClass`/`RH_ScopedVirtualClass` + `RH_ScopedCategory` + `RH_Scoped*Install(fn, 0xADDR[, { .Reversed = false }])`
  (`source/reversiblehooks/ReversibleHooks.h`). A function still calling `plugin::Call*<its own address>` **must**
  be `.Reversed = false`, otherwise it recurses forever.
- Globals: `static inline auto& ms_X = StaticRef<T>(0xADDR);` — never guess addresses.
- Layout: keep `VALIDATE_SIZE` / `VALIDATE_OFFSET` valid.
- Build: `cmake --build build --config Debug --target gta_reversed` (configure with `python setup.py`; re-run it after
  adding files, sources are globbed).
- Validation against the original code: `asm_validate.py` (exe path in `.claude/gtasa_exe_path.txt`; `--dump` gives
  annotated disassembly without IDA) and differential tests in `source/difftests/` (F7 → Tools → Diff tests, or
  `--run-difftests[=filter] --difftests-exit`). The ASI loader sets the working dir to `scripts/`, so the report
  (`difftests.log`), `logs/log.log` and `gta-reversed.ini` live in `<game>/scripts/`.
- Floats: `PI`/`TWO_PI`/`FRAC_*` in `common.h` are the exact `std::numbers` floats (`3.1415927f`/`6.2831855f`). Where
  the original loads a rounded value (e.g. `6.28318f` in `FxEmitter_c::CreateParticle`), write that literal with a
  `// NOTE:`. The original keeps x87 extended precision: accumulate loops in `double`, compare float results with ULP
  tolerance (`CompareFloats`).
- IDA Pro is reachable through the `ida-pro-mcp` plugin (`mcp__plugin_ida-pro-mcp_idalib__*`); open the user's
  IDB (`gta_sa.i64` of the 1.0 US compact exe) with `idb_open` if `idb_list` is empty. For bulk
  queries use `.claude/skills/find-reversing-targets/scripts/ida_rpc.py` (`export` -> `.claude/cache/idb_export.json`).
- Only the game's own code is reversed function-by-function (region `0x401000-0x748000`); RenderWare, D3DX, libpng
  and the CRT are called, not reversed.
- `InjectHooksMain` runs under the loader lock: nothing on that path may throw or wait (an escaping exception freezes
  the game at the console with no message). `RH_ScopedVMTInstall` throws if the address is not in the class
  vtable; `check_hooks.py` checks that.
- Don't change unrelated code; one subject per PR.
