---
name: find-reversing-targets
description: >
  Find and prioritise what to reverse next in gta-reversed. Lists hooks still marked `.Reversed = false` (per file,
  with addresses), quick wins (files 1-3 functions from completion), real coverage of the game's methods against the
  IDA database, plugin::Call stubs without hooks, NOTSA_UNREACHABLE placeholders and unimplemented script commands.
  Use when the user asks "what should I reverse next", "what is left in <class/subsystem>", or wants progress numbers.
argument-hint: "[subsystem/class filter, e.g. Ped, Audio, Cam]"
---

# Find reversing targets

Filter: `$ARGUMENTS` (may be empty). Run everything from the repo root. `SC=.claude/skills/find-reversing-targets/scripts`.

## 1. Collect candidates

### a. Registered hooks (no IDA needed)
```bash
python $SC/list_unreversed.py --top 30                          # per-file summary, most missing first
python $SC/list_unreversed.py --filter "$ARGUMENTS" --details   # every function + address for a file/category
python $SC/list_unreversed.py --almost-done --top 25            # files with 1-3 left: quick wins
```
This only sees functions that already have an `RH_*Install`.

### b. The whole binary (needs IDA with the ida-pro-mcp plugin running)
```bash
python $SC/ida_rpc.py export                          # once per IDB change -> .claude/cache/idb_export.json
python $SC/idb_coverage.py                            # summary
python $SC/idb_coverage.py --classes 40               # classes with most bytes left
python $SC/idb_coverage.py --class CGarage            # every method of one class + status
python $SC/idb_coverage.py --untouched 30             # classes absent from the repo
python $SC/idb_coverage.py --missing 50 --filter Task # biggest functions not in the repo
```
Default scope (`--scope methods`) is methods of game classes in the game region; RenderWare, D3DX, libpng, CRT, FLIRT
library functions, free functions and unnamed `sub_`s are excluded. `--scope game` adds free and unnamed functions.

Statuses: `REVERSED`; `DORMANT` (reversed, but the class's `InjectHooks()` is never called); `STUB` (hook with
`.Reversed = false`); `NOHOOK` (`plugin::Call` stub with no hook; fix with
`review-reversal/scripts/add_missing_hooks.py`); `KNOWN` (address only in a comment/declaration); `MISSING`.

Without an export, `docs/BinaryMap.md` §2 has the last snapshot of these numbers.

### c. Other sources (Grep)
- `REGISTER_COMMAND_UNIMPLEMENTED(` in `source/game_sa/Scripts/Commands/*.cpp`, or better
  `script_cmd.py list --status unregistered --original real` (see **reverse-script-command**). Never target
  `CRunningScript::ProcessCommands*To*` as a whole.
- `NOTSA_UNREACHABLE()` bodies installed as `.Reversed = false`: not written at all. Check xrefs in IDA first; some
  are unused in the original.
- Commented-out installs (`// RH_Scoped...Install`): usually an unknown/incorrect address or an ABI problem (R12).
- `TODO` comments in `source/game_sa/<Subsystem>`.
- `docs/ai/FrameCallGraph.md` starts with the biggest unreversed functions that run every frame.
- The subsystem guides (`docs/subsystems/README.md`) end with a "what is left" list.
- `docs/ReversedClasses.md` / `docs/hooks.json` are CI-generated and may lag behind the source.

### d. Per-class and per-struct views
- `python $SC/class_dossier.py <Class> [--ida] [--out f.md]`: methods with status and size, vtable slots, layout,
  dependencies with a leaf-first order, callers (`--ida`). Run it before planning a class.
- `python $SC/struct_check.py --export` (IDA → `.claude/cache/structs_export.json`), then
  `struct_check.py [--type T] [--markdown]`: repo `VALIDATE_SIZE`/`VALIDATE_OFFSET` vs IDB types.
- `python $SC/repo_to_idb.py [--apply]`: push repo names into unnamed IDB items (dry run by default). Never save the
  IDB without the user's consent.
- `python $SC/catalogues.py [--only vtables,globals,enums,callgraph]` regenerates `docs/ai/Vtables.md`, `Globals.md`,
  `Enums.md`, `FrameCallGraph.md` (needs `ida_rpc.py export` and `ida_rpc.py export-xrefs` first). Only when asked.

## 2. Prioritise

Rank candidates and state the reasoning:
1. **Unblocks others**: leaves called by many unreversed functions (`xrefs_to` / `callees`, or the dossier's order).
2. **Quick wins**: the last 1-3 functions of a file (`--almost-done`).
3. **Size**: `func_profile` / `lookup_funcs` in IDA, or the sizes printed by `idb_coverage.py` / `class_dossier.py`. Prefer < ~150 instructions for a
   first contribution; flag 1000+ instruction monsters (`CCam::Process*`, `CAutomobile::ProcessControl`).
4. **Testability**: runs every frame or somewhere easy to reach (free roam, a vehicle, a menu); pure functions can
   get a `NOTSA_DIFFTEST`.
5. **Risk**: avoid `__usercall`, mid-function hooks and save/load layout code unless asked.
6. **No overlap**: `git log --oneline -20 -- <file>`; `gh pr list --search "<Class>"` if `gh` is available.

## 3. Report

```
### <filter or "Whole project"> - <N> unreversed hooks in <M> files

| # | Target | Address | File | Size | Why |
|---|--------|---------|------|------|-----|
| 1 | CFoo::Bar | 0x5EF420 | source/game_sa/Foo.cpp:12 | ~80 insns | leaf, used by 4 unreversed fns |

Recommended next step: reverse #1 with the reverse-function skill (or the gta-reverser agent).
```

Done when the user has a ranked table with addresses, sizes and reasons. Offer to start on the top pick; don't start
reversing if they only asked for a list.
