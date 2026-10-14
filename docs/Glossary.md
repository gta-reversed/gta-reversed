# Glossary

Terms used in the code, in the docs and in reviews. Each entry links to the page with more detail when there is one.

## Project and tooling

| Term | Meaning |
|---|---|
| **ASI / ASI loader** | An `.asi` file is a DLL that an ASI loader (a proxy `vorbisFile.dll` from `contrib/plugins.zip`) loads into `gta_sa.exe`. `gta_reversed.asi` is our build output. The loader sets the working directory to `scripts/` while loading it, so logs, `gta-reversed.ini` and `difftests.log` live there ([Architecture.md](Architecture.md) §11) |
| **Compact exe** | The `gta_sa.exe` 1.0 US variant (5,189,632 bytes) that every address in the repo refers to |
| **Hook** | A patch at a GTA function address. Depending on its state it jumps to our C++ or keeps running the original. Registered with `RH_Scoped*Install` in `InjectHooks()` ([HookRegistration.md](HookRegistration.md)) |
| **Hook path** | `<Category>/<Scope>/<Fn>`, e.g. `Global/CGeneral/LimitAngle`: where a hook appears in F7 → Settings → Hooks. Set by `RH_ScopedCategory` and `RH_ScopedClass`/`RH_ScopedNamespace` |
| **Hook state** | `RedirectToOurs` (our code runs), `RedirectToGTA` (the original runs), `Unhooked` (nothing patched). `.Reversed = false` starts the hook as `RedirectToGTA` and locks it; `.Locked = true` stops it from being toggled in the F7 menu |
| **`InjectHooks` / `InjectHooksMain`** | Each class's hook registration function, and the function in `source/InjectHooksMain.cpp` that calls all of them from `DllMain` |
| **VMT install** | `RH_ScopedVMTInstall`: hooks a virtual function through its vtable slot. Needs `RH_ScopedVirtualClass(cls, vtable, slots)` and an address that is in that vtable ([ai/Vtables.md](ai/Vtables.md)) |
| **Stub** | A function whose body still calls the original (`plugin::Call*<0xADDR>`); its hook must be `{ .Reversed = false }` |
| **`StaticRef`** | `StaticRef<T>(0xADDR)` returns a reference to a global variable in the game's memory (`source/Base.h`) |
| **`VALIDATE_SIZE` / `VALIDATE_OFFSET`** | Compile-time checks that a struct's size or field offset matches the original layout |
| **`notsa::bugfixes`** | Switchable fixes for bugs of the original game (`source/reversiblebugfixes/Bugs.hpp`) |
| **NOTSA** | "Not SA": code or behaviour that doesn't exist in the original game (debug tools, fixes, helpers) |
| **Difftest** | A differential test (`NOTSA_DIFFTEST`) that runs our function and the original with the same inputs and compares the results ([ReversingGuide.md](ReversingGuide.md)) |
| **Coverage status** | REVERSED (hooked and ours), STUB (hooked, `.Reversed = false`), NOHOOK (stub with no hook), KNOWN (address in the repo, no hook), MISSING (not in the repo), DORMANT (has a hook line, but its class's `InjectHooks` is never called, so no hook is installed; only our own C++ callers reach it) ([BinaryMap.md](BinaryMap.md) §2) |
| **IDB** | The IDA Pro database of `gta_sa.exe`, reached through the `ida-pro-mcp` plugin |
| **ULP** | "Unit in the last place": distance between adjacent float values. Used as the tolerance for float comparisons |

## Machine code and compiler

| Term | Meaning |
|---|---|
| **x87** | The FPU stack the original code uses for floats. Intermediate results keep 80-bit precision until they are stored, which `float` code in our build doesn't reproduce |
| **`_ftol`** | The CRT helper the original calls for float → int conversion (truncates toward zero) |
| **`__thiscall` / `__cdecl` / `__stdcall` / `__fastcall`** | Calling conventions: member functions (`this` in ECX), free functions (caller cleans the stack), callbacks and Win32 API (callee cleans), register arguments |
| **`__usercall`** | IDA's name for a non-standard calling convention, with arguments in arbitrary registers. They can't be hooked directly; the repo leaves their install commented out with a note (eg. `CPlantMgr::LoadModels`) |
| **Inlined function** | A function MSVC copied into its callers instead of calling it. Its body shows up in the pseudocode; our code calls the function instead ([ai/InlineCatalogue.md](ai/InlineCatalogue.md)) |
| **Scalar deleting destructor** | The vtable's slot 0: destroys the object and optionally frees it. The repo hooks the plain destructor instead, through a `Destructor()` wrapper ([HookRegistration.md](HookRegistration.md) §3) |
| **Vtable** | The table of virtual function pointers at the start of every polymorphic object |

## Game engine

| Term | Meaning |
|---|---|
| **RenderWare (RW)** | The 3D engine the game is built on (`Rw*`, `Rp*`, `Rt*` functions). It is called, not reversed |
| **Clump / Atomic / Frame** | RW objects: a clump is a model instance made of atomics (meshes) attached to frames (transforms) |
| **Model info** | `CBaseModelInfo` and subclasses: per-model data loaded from IDE files. Instances refer to it by model index ([subsystems/WorldStreamingPools.md](subsystems/WorldStreamingPools.md)) |
| **IDE / IPL** | Text or binary data files: IDE defines model types, IPL places instances in the world |
| **IMG / streaming** | `CStreaming` loads models, textures, collisions, animations and scripts on demand from the IMG archives |
| **LOD** | Level of detail: low-poly models drawn far away instead of the full model |
| **Pool** | A fixed-size allocator for one kind of object (`GetPedPool()`, `GetVehiclePool()`, ...). Script handles encode the pool index |
| **Sector / repeat sector** | The world grid `CWorld` uses to find entities near a position |
| **Entity / Physical / Ped / Vehicle / Object** | The class tree: `CEntity` → `CPhysical` → `CPed`, `CVehicle`, `CObject` |
| **Vehicle type vs sub-type** | `m_nVehicleType` (AUTOMOBILE, BIKE, BOAT, TRAIN) vs `m_nVehicleSubType` (also HELI, PLANE, QUAD, TRAILER, BMX, ...). Use `IsSub*()` for the concrete class ([subsystems/Vehicles.md](subsystems/Vehicles.md)) |
| **Task** | A unit of ped behaviour (`CTaskSimple*`, `CTaskComplex*`), managed by `CTaskManager` ([subsystems/Tasks.md](subsystems/Tasks.md)) |
| **Event** | Something a ped notices (`CEvent*`); the event handler turns it into a response task ([subsystems/Events.md](subsystems/Events.md)) |
| **Decision maker** | Data tables that pick a ped's response to an event |
| **Col model** | `CColModel`: the collision shape of a model (spheres, boxes, lines, triangles) ([subsystems/CollisionPhysics.md](subsystems/CollisionPhysics.md)) |
| **Path nodes** | The road and footpath graph (`CPathFind`), streamed per area ([subsystems/PathFind.md](subsystems/PathFind.md)) |
| **Anim association** | `CAnimBlendAssociation`: one animation playing on a clump, with blend weight and speed ([subsystems/Animation.md](subsystems/Animation.md)) |
| **AE (audio engine)** | The `CAE*` audio classes ([subsystems/Audio.md](subsystems/Audio.md)) |
| **GXT** | The game's text files; strings are looked up by key with `TheText.Get` ([subsystems/HudFrontend.md](subsystems/HudFrontend.md)) |

## Scripts

| Term | Meaning |
|---|---|
| **SCM** | The compiled mission script `main.scm` and the virtual machine that runs it ([subsystems/Scripts.md](subsystems/Scripts.md)) |
| **Opcode / command** | One SCM instruction (eg. `0x0001 WAIT`). The repo registers handlers per command; unregistered ones run the original `ProcessCommandsXToY` switch ([ScriptCommandReversing.md](ScriptCommandReversing.md)) |
| **Running script** | `CRunningScript`: one script thread with its own instruction pointer, locals and timers |
| **Script brain** | A streamed script attached to a ped or object model (`CScriptsForBrains`) |
| **CLEO** | A third-party script extension library; some of its opcodes are implemented in `Scripts/Commands/CLEO/` |
