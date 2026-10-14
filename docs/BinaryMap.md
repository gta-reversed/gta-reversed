# gta_sa.exe binary map & real coverage of the game code

This page describes how the original executable is laid out and how much of the **game's own code** has been
reversed so far. The figures come from the community IDA database (`gta_sa.i64`, compact exe, 21,682 functions),
cross-referenced against this repository.

> Snapshot: **2026-10-12**. To regenerate it, open the IDB in IDA with the `ida-pro-mcp` plugin and run:
> ```bash
> python .claude/skills/find-reversing-targets/scripts/ida_rpc.py export          # IDB -> .claude/cache/idb_export.json
> python .claude/skills/find-reversing-targets/scripts/idb_coverage.py --markdown --classes 50
> ```

---

## 1. Executable layout (1.0 US / compact, image base `0x400000`)

| Segment | Range | Perm | Contents |
|---|---|---|---|
| `HEADER` | `0x400000-0x401000` | | PE header |
| `.text` | `0x401000-0x857000` | r-x | all code (see the region map below) |
| `_rwcseg` | `0x857000-0x858000` | r-x | RenderWare code segment |
| `.idata` | `0x858000-0x85837C` | r-- | import address table (Win32, D3D9, DirectInput, DirectSound...) |
| `.rdata` | `0x85837C-0x8A4000` | r-- | constants: float/double literals, strings, **vtables** (about 545 named), jump tables, GUIDs |
| `.data` | `0x8A4000-0xC9E000` | rw- | globals and static arrays (initialised and BSS) |
| `_rwdseg` | `0xCAF000-0xCB0000` | rw- | RenderWare data |
| `.rsrc` | `0xCB0000-0xCB1000` | r-- | resources |

### Code region map
The boundaries are approximate: each library is mostly contiguous, but small helpers are scattered.

| Range | What lives there | Reversed function-by-function? |
|---|---|---|
| `0x401000-0x748000` | **Game code**: all GTA classes. It also contains the embedded D3DX JPEG library around `0x5C0000-0x5CFFFF` (Fx/Furniture loaders sit next to it), RW plugin glue, and the Win32 platform skeleton (`WinMain`, `psInitialise`, DirectInput pads) around `0x745000-0x748000` | **yes**, this is the project's scope |
| `0x748000-0x760000` | RenderWare RpWorld, D3D9 pipelines | no (RW is called through wrappers in `source/game_sa/RenderWare/`) |
| `0x760000-0x7C0000` | D3DX9: math, shader compiler | no |
| `0x7C0000-0x7E0000` | RW toolkits (RtAnim, HAnim, UVAnim, RtDict), libpng | no |
| `0x7E0000-0x820000` | RenderWare core: RwCamera, Im2D/Im3D, D3D9 driver, plugin registry, MatFX | no |
| `0x820000-0x840000` | Statically linked MSVC CRT: `_rand` `0x821B1E`, `__ftol2` `0x821B40`, `_floor` `0x8219F0`, `operator delete[]` `0x8213AE`, string/printf/malloc... | no (use `std` equivalents) |
| `0x840000-0x857000` | Static initialisers (`GlobalInit_*`, global constructors) and exception-handling unwind funclets (about 1,700 tiny functions) | no (C++ static init does this) |

`asm_validate.py` reports calls at or above `0x820000` as CRT calls and labels `__ftol2` as a float→int cast.
`idb_coverage.py` uses the same regions to decide what counts as game code.

---

## 2. Real coverage: game class methods only

**Scope:** member functions of the game's own classes, in the game code region. Excluded:
- RenderWare, D3DX, libpng, the CRT, static initialisers
- functions IDA identifies as library code (FLIRT)
- free functions and unnamed `sub_` helpers
- FLIRT noise "classes" from unrelated libraries (`std::`, `ATL::`, `CryptoPP`, Qt...)

```
IDB: C:\Program Files\Rockstar Games\Grand Theft Auto San Andreas\gta_sa_compact.exe - 21682 functions in total

GAME CLASS METHODS: 11123 functions in 985 classes (2902 KiB of code)
  reversed    7025   63.2% of functions   58.8% of bytes   (hooked + reversed)
  dormant      109    1.0% of functions    0.9% of bytes   (reversed, but its class' InjectHooks() is never called)
  stub         793    7.1% of functions   23.2% of bytes   (hooked, `.Reversed = false`)
  nohook       117    1.1% of functions    2.7% of bytes   (plugin::Call stub WITHOUT hook)
  known        728    6.5% of functions    3.5% of bytes   (address in repo, not hooked)
  missing     2351   21.1% of functions   10.8% of bytes   (not in the repo)

Excluded from the count above:
  free (non-member) functions in game code      827  (305 reversed)
  unnamed/trivial sub_/nullsub_ in game code   3079  (mostly tiny compiler-generated/inlined helpers)
  FLIRT noise classes (std::, ATL::, ...)        30
  init+eh                                      1814
  renderware                                   1698
  d3dx                                         1355
  crt                                           658
  library/RW helpers inside game region         552
  rw-toolkits+libpng                            546

(For comparison, docs/hooks.json only counts registered hooks: 7463/8027 = 93.0%)
```

How to read this:
- **The 93% in `ReversedClasses.md` is relative to registered hooks only.** Measured against the binary, about
  **63% of the game's methods (59% of the bytes)** are reversed.
- **`stub` covers 23% of the bytes, so the largest functions are still stubs.** Examples: `CRunningScript`, `CCam`,
  `Interior_c` and `CPedGeometryAnalyser`.
- **`dormant` (109 functions) is reversed code with no hook**, because its class's `InjectHooks()` is never
  called. Original code that calls such a function still runs the original; only our own C++ callers reach the
  reversed version (eg. `CHud::Draw` runs every frame because our `Render2dStuff` calls it directly), and it can't
  be toggled in the F7 menu. The classes left this way on purpose are `CHud` (not tested in game), `CMaths`
  (`InitMathsTables` is a no-op, so the game's sin table would stay empty), `CPedAttractor`, `CPedShelterAttractor`,
  `CConversationNode` and `CLodTimeModelInfo` (its `InjectHooks` uses the wrong vtable, see
  [ai/CaseStudies.md](ai/CaseStudies.md) §8).
- **`nohook` (117 functions) breaks the rule in `HookRegistration.md`.** These are `plugin::Call` stubs without an
  `RH_*Install`. `add_missing_hooks.py` registered the safe ones on 2026-10-12, which took this number down from 313.
  The rest are virtuals, constructors/destructors, templates, or functions defined outside their class's `.cpp`, so
  they need to be registered by hand.
- **`known` means the address appears in the repo but has no hook.** That's usually an inlined function or a
  `// 0xADDR` comment for a function we implement differently.

### Script commands
The 27 missing `CRunningScript` methods are mostly the original `ProcessCommands<N>To<N+99>` dispatchers, the largest
functions in the game (4-9 KB each). They are **not** reversed as functions. Instead,
`CRunningScript::ProcessOneCommand` calls a custom handler when one is registered, and falls back to the original
dispatcher through `s_OriginalCommandHandlerTable` otherwise. Progress here is the number of implemented commands.

The `switch` tables of the 27 original dispatchers were extracted from the IDB with `script_cmd.py export`. That gives
the address of each opcode's `case`, which `script_cmd.py stats` then compares with what the repo registers:

| Repo registration ↓ / original behaviour → | real code | shared code (2-7 opcodes) | default path? (8+) | no-op (`OR_CONTINUE`) | not implemented (`OR_INTERRUPT`) | no case |
|---|---:|---:|---:|---:|---:|---:|
| `REGISTER_COMMAND_HANDLER` | 587 | 22 | 21 | 15 | 8 | 2 |
| `REGISTER_COMMAND_NOP` | **27** | 10 | 0 | 245 | 0 | 0 |
| `REGISTER_COMMAND_UNIMPLEMENTED` | **3** | 2 | 62 | 50 | 486 | 110 |
| not registered (original dispatcher runs it) | **925** | 43 | 2 | 6 | 13 | 0 |

How to read it:
- **About 925 commands with real code in the original are not registered.** Those are the script work left; list
  them with `script_cmd.py list --status unregistered --original real --sort size`.
- **27 `NOP`s and 3 `UNIMPLEMENTED`s have real code in the original**, so they may be registered wrongly. Check each
  one with `script_cmd.py locate <cmd> --decompile`.
- **Original no-ops that are registered as `UNIMPLEMENTED` behave differently.** In vanilla those return
  `OR_CONTINUE`, but `REGISTER_COMMAND_UNIMPLEMENTED` returns `OR_INTERRUPT` (and debug-breaks). That only matters if
  a script uses them.

### Classes with the most code left (bytes not yet reversed)

| Class | Methods | Reversed | Stub | Unhooked stub | Known | Missing | Bytes left |
|---|---:|---:|---:|---:|---:|---:|---:|
| `CRunningScript` | 66 | 24 | 15 | 0 | 0 | 27 | 162,878 |
| `CCarCtrl` | 110 | 20 | 83 | 6 | 0 | 1 | 84,878 |
| `CCam` | 46 | 18 | 25 | 0 | 2 | 1 | 65,770 |
| `CCamera` | 119 | 96 | 17 | 0 | 4 | 2 | 37,975 |
| `CBike` | 56 | 22 | 18 | 0 | 1 | 15 | 34,010 |
| `CHud` | 38 | 0 | 8 | 0 | 0 | 0 | 29,566 |
| `Interior_c` | 54 | 0 | 52 | 0 | 0 | 2 | 24,233 |
| `CPed` | 148 | 130 | 7 | 4 | 2 | 5 | 23,370 |
| `CVehicle` | 203 | 137 | 3 | 0 | 17 | 46 | 19,976 |
| `CPedGeometryAnalyser` | 37 | 8 | 29 | 0 | 0 | 0 | 18,694 |
| `CPlane` | 25 | 14 | 9 | 0 | 1 | 1 | 17,765 |
| `CPlayerPed` | 66 | 48 | 15 | 0 | 1 | 2 | 16,020 |
| `CAutomobile` | 117 | 99 | 0 | 2 | 11 | 5 | 16,020 |
| `CGarage` | 37 | 4 | 14 | 0 | 0 | 19 | 14,415 |
| `CTaskSimpleFight` | 29 | 0 | 5 | 4 | 0 | 20 | 13,871 |
| `CHeli` | 27 | 11 | 0 | 5 | 10 | 1 | 13,686 |
| `CShadows` | 30 | 20 | 7 | 0 | 2 | 1 | 13,263 |
| `CWaterLevel` | 36 | 24 | 8 | 0 | 1 | 3 | 12,930 |
| `CWeapon` | 40 | 34 | 6 | 0 | 0 | 0 | 12,498 |
| `CStats` | 52 | 32 | 19 | 0 | 0 | 1 | 11,656 |
| `Fx_c` | 36 | 3 | 0 | 0 | 31 | 2 | 10,877 |
| `CTrain` | 45 | 17 | 19 | 0 | 0 | 9 | 10,852 |
| `CPlayerInfo` | 30 | 25 | 3 | 0 | 0 | 2 | 10,328 |
| `CAERadioTrackManager` | 43 | 29 | 14 | 0 | 0 | 0 | 9,634 |
| `CAEScriptAudioEntity` | 20 | 8 | 11 | 0 | 0 | 1 | 9,467 |
| `CPathFind` | 75 | 54 | 7 | 0 | 0 | 14 | 9,406 |
| `CWorld` | 84 | 64 | 0 | 0 | 4 | 16 | 8,812 |
| `CBmx` | 14 | 7 | 3 | 0 | 3 | 1 | 8,440 |
| `FxEmitterBP_c` | 12 | 2 | 7 | 0 | 1 | 2 | 8,191 |
| `InteriorGroup_c` | 25 | 2 | 22 | 0 | 0 | 1 | 7,902 |
| `CEventHandler` | 72 | 69 | 3 | 0 | 0 | 0 | 7,891 |
| `CAEWeaponAudioEntity` | 20 | 0 | 0 | 0 | 0 | 0 | 7,604 |
| `CProjectileInfo` | 12 | 1 | 11 | 0 | 0 | 0 | 7,592 |
| `CRope` | 8 | 5 | 2 | 0 | 1 | 0 | 7,580 |
| `CPostEffects` | 43 | 33 | 3 | 0 | 6 | 1 | 7,364 |
| `CTaskSimpleGangDriveBy` | 13 | 0 | 0 | 3 | 0 | 10 | 7,359 |
| `CClothesBuilder` | 28 | 13 | 8 | 0 | 0 | 7 | 6,927 |
| `CAEFrontendAudioEntity` | 8 | 4 | 2 | 0 | 0 | 2 | 6,666 |
| `CTaskUtilityLineUpPedWithCar` | 5 | 2 | 3 | 0 | 0 | 0 | 6,664 |
| `CTaskComplexKillCriminal` | 12 | 9 | 2 | 0 | 0 | 1 | 6,441 |

### Classes with no presence in the repo at all
129 game classes have no function in the repo. The largest are:
`CTaskComplexGoPickUpEntity`, `CTaskComplexAvoidEntity`, `CAttractorScanner`, `CTaskComplexFollowPatrolRoute`,
`CPhoneInfo`, `CPedAcquaintanceScanner`, `CTaskInteriorSitInChair`, `CTaskComplexUseMobilePhone`,
`CBugstarInterface`, `CTaskComplexReactToAttack`, `CPlayerCrossHair`, `CTaskSimpleBeHitWhileMoving`,
`CTaskComplexWaitForPed`, `CPatrolRoute`, `CTaskComplexKillAllThreats`, many `CEvent*` and several
`C*Scanner` classes. They're good self-contained targets for `/reverse-class`.

---

## 3. Struct layouts: repo vs IDB

`struct_check.py` compares every `VALIDATE_SIZE` / `VALIDATE_OFFSET` in the repo with the IDB's types. The repo's
sizes are checked by the compiler, so a mismatch often means the IDB is outdated (`CVehicleAnimGroup` is 0x4 there,
for example). Other mismatches are real repo bugs, or layouts the repo changed (NOTSA members, OpenAL). Look at both
sides before changing anything.

Summary: repo VALIDATE_SIZE: 745 types (613 also in the IDB); IDB structs/unions: 1689; findings: 23 SIZE, 2 OFFSET, 0 NOSIZE

| Kind | Type | Detail | Where |
|---|---|---|---|
| SIZE | `C2dEffect` | repo 0x40 vs IDB 0x44 | `source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.h` |
| SIZE | `CAEAudioChannel` | repo 0x60 vs IDB 0x64 | `source/game_sa/Audio/Hardware/AEAudioChannel.h` |
| SIZE | `CAEAudioHardware` | repo 0x1014 vs IDB 0x10B8 | `source/game_sa/Audio/Hardware/AEAudioHardware.h` |
| SIZE | `CAEGlobalWeaponAudioEntity` | repo 0xA8 vs IDB 0xB0 | `source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.h` |
| SIZE | `CAEMFDecoder` | repo 0xC4 vs IDB 0xBC | `source/game_sa/Audio/Loaders/AEMFDecoder.h` |
| SIZE | `CAEStaticChannel` | repo 0x90 vs IDB 0x94 | `source/game_sa/Audio/Hardware/AEStaticChannel.h` |
| SIZE | `CAEStreamingChannel` | repo 0x60098 vs IDB 0x600A0 | `source/game_sa/Audio/Hardware/AEStreamingChannel.h` |
| SIZE | `CAEVorbisDecoder` | repo 0x28 vs IDB 0x20 | `source/game_sa/Audio/Loaders/AEVorbisDecoder.h` |
| SIZE | `CAEWaterCannonAudioEntity` | repo 0xA0 vs IDB 0x90 | `source/game_sa/Audio/Entities/AEWaterCannonAudioEntity.h` |
| SIZE | `CAEWaveDecoder` | repo 0x3C vs IDB 0x38 | `source/game_sa/Audio/Loaders/AEWaveDecoder.h` |
| SIZE | `CAEWeaponAudioEntity` | repo 0xA0 vs IDB 0xA8 | `source/game_sa/Audio/Entities/AEWeaponAudioEntity.h` |
| SIZE | `CColDisk` | repo 0x24 vs IDB 0x28 | `source/game_sa/Collision/ColDisk.h` |
| SIZE | `CCompressedMatrixNotAligned` | repo 0x14 vs IDB 0x12 | `source/game_sa/CompressedMatrixNotAligned.h` |
| SIZE | `CFileCarGenerator` | repo 0x30 vs IDB 0x2E | `source/game_sa/CarGenerator.h` |
| SIZE | `CPlantColEntEntry` | repo 0x14 vs IDB 0xC | `source/game_sa/Plant/PlantColEntEntry.h` |
| SIZE | `CPlantLocTri` | repo 0x54 vs IDB 0x4C | `source/game_sa/Plant/PlantLocTri.h` |
| SIZE | `CTaskSimpleGiveCPR` | repo 0x18 vs IDB 0x14 | `source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.h` |
| SIZE | `CVehicleAnimGroup` | repo 0x94 vs IDB 0x4 | `source/game_sa/VehicleAnimGroupData.h` |
| SIZE | `FxEmitterPrt_c` | repo 0x44 vs IDB 0x3C | `source/game_sa/Fx/FxEmitterPrt.h` |
| SIZE | `IKChainManager_c` | repo 0xB18 vs IDB 0xE0C | `source/game_sa/Ragdoll/IKChainManager.h` |
| SIZE | `WaterCreature_c` | repo 0x34 vs IDB 0x2C | `source/game_sa/WaterCreature_c.h` |
| SIZE | `_ProjectionParam` | repo 0x98 vs IDB 0x58 | `source/game_sa/Shadows.h` |
| SIZE | `cHandlingDataMgr` | repo 0xC624 vs IDB 0xC5AC | `source/game_sa/cHandlingDataMgr.h` |
| OFFSET | `CAEAudioChannel` | `m_nBufferFrequency` @ 0x4B: no IDB member starts there (inside IDB member `m_wFrequencyMult` +0x4A size 2) | `source/game_sa/Audio/Hardware/AEAudioChannel.h` |
| OFFSET | `CAEAudioChannel` | `m_wFrequencyMult` @ 0x49: no IDB member starts there (inside IDB member `field_47` +0x48 size 2) | `source/game_sa/Audio/Hardware/AEAudioChannel.h` |

## 4. Bugs found while building this tooling

### 4.1 Fixed (hook registration and startup)

| Where | Problem | How it was found |
|---|---|---|
| `CWorld` hooks | `FindPlayerSlotWithVehiclePointer` was installed at the address of `FindPlayerSlotWithRemoteVehiclePointer` | subsystem doc review + IDB |
| `CTaskInteriorLieInBed::InjectHooks` | Static `FinishAnimCB` was registered with `RH_ScopedVMTInstall`. The throw inside `DllMain` hung the game at startup | in-game run, stack sampling, then the new vtable check in `check_hooks.py` |
| 4 hooks (`CCamera::TakeControlAttachToEntity`, `CConversationForPed::IsPlayerInPositionForConversation`, `CDecision::Set`, `CTheScripts::ScriptDebugCircle2D`) | Installed as reversed while still calling `plugin::Call` on their own address | `check_hooks.py` |
| 209 stubs | Had no hook registration (or a commented-out one); now registered as `{ .Reversed = false }` | `add_missing_hooks.py` |
| 14 classes | Hooks never installed, because `InjectHooks()` was never called (original callers ran the original code) | `check_hooks.py` |

### 4.2 Behaviour differences found, not fixed yet

Our C++ differs from the original in these places. The fixes change game code, so they belong in their own pull
requests. [ai/CaseStudies.md](ai/CaseStudies.md) shows each fix. Two difftests fail until they are fixed:
`LimitRadianAngle` and `ConvertToEulerAngles`.

The rounded `PI`/`TWO_PI` in `common.h` used to be on this list (it also made the `GetATanOfXY`,
`GetRadianAngleBetweenPoints` and `GetAngleBetweenPoints` difftests fail). It is fixed: the constants are now the
exact `std::numbers` values.

| Where | Problem | How it was found |
|---|---|---|
| `CGeneral::SolveQuadratic` | Different operation order (`1/(2a)` instead of `(1/a)*0.5`) | difftest |
| `CGeneral::LimitRadianAngle` | Loop rounded to float on every step, while the original keeps x87 extended precision | difftest |
| `CMatrix::ConvertToEulerAngles` | Wrong index (`m[j][k]` instead of `m[j][i]`) in the `EULER_ANGLES` branch | difftest + decompilation |
| `CCam::ClipBeta` | Subtracts 2π whenever β ≥ −π | subsystem doc review + `--dump` (static check was clean) |
| `CPedIntelligence::GetActivePrimaryTask` | Loop variable unused, so it always reads the same slot | subsystem doc review + `--dump` |
| `CVehicle::IsSubFlyingVehicle` | `&&` instead of `\|\|`, so it is always false (`CGangWars::Update`) | subsystem doc review + `--dump` |
| `CTaskSimpleCarJumpOut` door timing | `IsPlane()` (type) instead of `IsSubPlane()` (sub type) | subsystem doc review + `--dump` |

---

## 5. Key globals

Every one of these is already mapped to a `StaticRef` in the repo. Names are the IDB's.

| Global | Address | Type |
|---|---|---|
| `CPools::ms_pPedPool` / `VehiclePool` / `BuildingPool` / `ObjectPool` / `DummyPool` / `TaskPool` | `0xB74490` / `494` / `498` / `49C` / `4A0` / `4A8` | `CPool*`; file-scope in `Pools.cpp`, use `GetPedPool()`, `GetVehiclePool()`... |
| `CWorld::Players` | `0xB7CD98` | `CPlayerInfo[2]` |
| `CWorld::PlayerInFocus` | `0xB7CD74` | `uint8` |
| `CWorld::ms_aSectors` / `ms_aRepeatSectors` | `0xB7D0B8` / `0xB992B8` | `CSector[14400]` / `CRepeatSector[16][16]` |
| `TheCamera` | `0xB6F028` | `CCamera` |
| `CTimer::m_snTimeInMilliseconds` / `ms_fTimeStep` / `m_FrameCounter` | `0xB7CB84` / `0xB7CB5C` / `0xB7CB4C` | private; use `CTimer::GetTimeInMS()`, `GetTimeStep()`, `GetFrameCounter()` |
| `CStreaming::ms_aInfoForModel` | `0x8E4CC0` | `CStreamingInfo[26316]` |
| `CModelInfo::ms_modelInfoPtrs` | `0xA9B0C8` | `CBaseModelInfo*[20000]`; use `CModelInfo::GetModelInfo()` |
| `CTheScripts::ScriptSpace` / `ScriptsArray` | `0xA49960` / `0xA8B430` | `char[269000]` / `CRunningScript[96]` |
| `CGame::currArea` / `currLevel` | `0xB72914` / `0xB7290C` | `int32` |
| `ThePaths` | `0x96F050` | `CPathFind` |
| `CGarages::aCarsInSafeHouse` | `0x96ABD8` | `CStoredCar[20][4]` (`0x96ABD4` is `ModelToTry`, not this) |
| `AudioEngine` | `0xB6BC90` | `CAudioEngine` |
| `RsGlobal` | `0xC17040` | `RsGlobalType` |
| `gGameState` | `0xC8D4C0` | `int32` holding an `eGameState` |
| `CTxdStore::ms_pTxdPool` / `CColStore::ms_pColPool` | `0xC8800C` / `0x965560` | `CPool*` |

Data names: the IDB names 4,007 items in `.data` and 9,438 in `.rdata`. The repo has 2,340 `StaticRef`s, and they
cover 2,011 of the 3,044 meaningfully named `.data` globals.

---

## 6. Tools that use this information

| Tool | What it gives you |
|---|---|
| `idb_coverage.py` | Coverage tables, per-class function lists (`--class CCam`), biggest missing functions (`--missing`), untouched classes (`--untouched`) |
| `asm_validate.py` | Static validation of a reversal against the exe. With the IDB export cached, unknown callees and globals are labelled with their IDA names (`CMatrix::~CMatrix [IDB]`) |
| `ida_rpc.py` | Scriptable access to the IDA MCP server (`health`, `export`, `export-xrefs`, `call <tool> <json>`) |
| `list_unreversed.py` | Hooks still marked `.Reversed = false` (works without IDA) |
| `class_dossier.py` | Everything needed to reverse one class in one report: methods and status, vtable, layout, dependencies, suggested order |
| `struct_check.py` | Repo `VALIDATE_SIZE`/`VALIDATE_OFFSET` vs IDB types (§3) |
| `script_cmd.py` | Opcode ↔ name, repo status, the original `case` address, its Hex-Rays pseudocode (`--decompile`), stats (§2) |
| `repo_to_idb.py` | Pushes names the repo knows into IDA (placeholder names only; nothing is saved) |
| `add_missing_hooks.py` | Registers `{ .Reversed = false }` hooks for `plugin::Call` stubs without a hook |
| `validate_all.py` + `inline_catalogue.py` | Static validation of every reversed hook, mined into [ai/InlineCatalogue.md](ai/InlineCatalogue.md) |
| `catalogues.py` | Generates [ai/Vtables.md](ai/Vtables.md), [ai/Globals.md](ai/Globals.md), [ai/Enums.md](ai/Enums.md) and [ai/FrameCallGraph.md](ai/FrameCallGraph.md) from the IDB exports and the exe |
