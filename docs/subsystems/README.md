# Subsystem guides

One page per game subsystem. Each page explains how the subsystem works at run time, where it lives in the
repository, which original functions drive it, the pitfalls found while reversing it, and what is left. Read the page
for the area you are about to touch before opening IDA.

General rules are not repeated here: see [../ReversingRules.md](../ReversingRules.md) (R1-R20),
[../ReversingGuide.md](../ReversingGuide.md) (workflow and validation), [../HookRegistration.md](../HookRegistration.md)
and [../ai/TipsAndTricks.md](../ai/TipsAndTricks.md) (engine idioms, IDA quirks).

| Page | Covers | Source folders (under `source/game_sa/`) |
|---|---|---|
| [WorldStreamingPools.md](WorldStreamingPools.md) | Entity hierarchy, pools and handles, `CWorld` sectors, `CStreaming`, `CModelInfo`, `CFileLoader`, the `CGame::Process` frame order | root (`World*`, `Streaming*`, `Pools*`, `FileLoader*`, `Game*`), `Entity/`, `Models/`, `Core/`, `Pools/` |
| [Peds.md](Peds.md) | `CPed`, `CPlayerPed`, cops/medics/emergency peds, ped flags, ped groups, population, per-frame ped update | `Entity/Ped/`, `PedIntelligence*`, `PedGroup*`, `Population*`, `PlayerInfo*` |
| [Tasks.md](Tasks.md) | Task tree, `CTaskManager`, per-frame task loop, task vtable layout, templates for `CTask*` classes | `Tasks/` |
| [Events.md](Events.md) | Event queue, priorities, `CEventHandler`, response tasks, scanners, decision makers | `Events/`, `DecisionMakers/` |
| [Vehicles.md](Vehicles.md) | Vehicle class tree, type vs sub-type, `ProcessControl` and suspension, handling, damage, `CCarCtrl` / `CCarAI`, enter/exit | `Entity/Vehicle/`, `CarCtrl*`, `CarAI*`, `CarEnterExit*`, `AutoPilot*`, `DamageManager*` |
| [Camera.md](Camera.md) | `CCamera`, the three `CCam`s, mode selection, transitions, script control, the `CCam::Process_*` work plan | `Camera*`, `Cam*`, `IdleCam*` |
| [Rendering.md](Rendering.md) | Frame draw order, where RenderWare takes over, pipelines, visibility | `Renderer*`, `Pipelines/`, `Fx/` |
| [Audio.md](Audio.md) | The AE engine: audio hardware, entities, banks, per-frame servicing | `Audio/` |
| [Scripts.md](Scripts.md) | SCM script engine: `main.scm` loading, `CRunningScript` / `CTheScripts`, command dispatch and argument parsing, script brains | `Scripts/`, `ScriptsForBrains*` |
| [CollisionPhysics.md](CollisionPhysics.md) | Col models and `CColStore`, `CCollision` Test/Process functions, `CPhysical` forces and collision response, the `CWorld::Process` collision loop, surfaces, buoyancy | `Collision/`, `Entity/Physical*`, `SurfaceInfos_c*`, `Buoyancy*` |
| [Animation.md](Animation.md) | `CAnimManager`, groups/blocks/IFP, blend associations and callbacks, the RpAnimBlend per-frame update, anim block streaming | `Animation/` |
| [PathFind.md](PathFind.md) | Path node areas and streaming, node/link layout, closest-node queries, `DoPathSearch`, road switches, how cars and peds follow paths | `PathFind*`, `CarCtrl*`, `AutoPilot*`, `Tasks/` (route tasks) |
| [Weapons.md](Weapons.md) | `CWeapon` firing pipeline, `weapon.dat` / `CWeaponInfo`, projectiles, bullets, explosions, fire, damage events | `Weapon*`, `Projectile*`, `BulletInfo*`, `Explosion*`, `Fire*` |
| [HudFrontend.md](HudFrontend.md) | `CHud` (and why it is not registered), radar drawing, `CFont`, GXT text, `CMessages`, the front-end menu, widescreen | `Hud*`, `Radar*`, `Font*`, `Text/`, `Messages*`, `MenuManager*` |
| [SaveLoad.md](SaveLoad.md) | `CGenericGameStorage` and `C_PcSave`, the save file format and the 28 blocks, how to verify a Load/Save pair | `GenericGameStorage*`, `C_PcSave*`, each class's `Load` / `Save` |

Most of these folders also have a short `CLAUDE.md` that points back to the matching page.

## Page layout

Pages follow the same order so they are easy to scan:

1. **Overview**: what the subsystem does, with a diagram.
2. **Key classes and files**: sizes, vtables, globals.
3. **Frame flow**: who calls what each frame, in order.
4. Subsystem-specific sections (data formats, interfaces, templates).
5. **Gotchas**: traps found while reversing, specific to this subsystem.
6. **What is left**: a coverage snapshot and a suggested order.

## Conventions

- **References use symbol names, not line numbers.** Write `CWorld::Process`, not `World.cpp:2155`; line numbers go
  stale with every change. Addresses are only quoted when the repo states them (`RH_*Install`, `StaticRef`,
  `// 0xADDR`).
- **Status words** are the ones printed by `idb_coverage.py` (see [../BinaryMap.md](../BinaryMap.md) §2):
  REVERSED (hooked and reversed), DORMANT (reversed, but the class's `InjectHooks()` is never called), STUB (hooked
  with `.Reversed = false`), NOHOOK (`plugin::Call` stub without a hook), KNOWN (address in the repo, not hooked),
  MISSING (not in the repo).
- **Numbers are snapshots** and drift. Regenerate with
  `python .claude/skills/find-reversing-targets/scripts/idb_coverage.py --class <Class>`. For a single class,
  `class_dossier.py <Class>` gives a fresher, more detailed view than any page here.
- Items marked **(Fixed: ...)** in a Gotchas section were real bugs that have since been corrected; they are kept
  because the same mistake tends to reappear elsewhere ([R20](../ReversingRules.md)).

## Related

- [../Architecture.md](../Architecture.md): build, hook system internals, start-up order
- [../BinaryMap.md](../BinaryMap.md): layout of `gta_sa.exe`, coverage, struct and bug tables
- [../ai/CaseStudies.md](../ai/CaseStudies.md): real reversal mistakes and their lessons
- [../Glossary.md](../Glossary.md): project, compiler, engine and script terms
