# Fx/ — particle effects system (effects.fxp)

Guide: [../../../docs/subsystems/Rendering.md](../../../docs/subsystems/Rendering.md) (where Fx update/render sit in
the frame, Fx coverage and what is left).

## Layout
- Naming: plugin-sdk style `Xxx_c` classes (not `CXxx`); keep it.
- Runtime singletons: `Fx_c& g_fx` (`Fx.h`: effect spawners `AddBlood` / `AddSparks` / `Trigger*`) and
  `FxManager_c& g_fxMan` (`FxManager.h`: loads `models\effects.fxp`, owns blueprints, live systems and the memory
  pool). Particle quad batching is free functions in `Fx.h` (`RenderBegin`, `RenderAddTri_`, `RenderEnd`).
- Blueprints (data from the .fxp): `FxSystemBP_c` -> `FxEmitterBP_c` / `FxPrimBP_c` -> `FxInfoManager_c` ->
  `Blueprints/FxInfo*_c` (one class per info type, `eFxInfoType.h`; keyframe interpolation in `Blueprints/Interp/`).
- Instances: `FxSystem_c` (one running effect, also holds `m_FireAE` audio), `FxEmitter_c` (a `FxPrim_c`),
  `FxEmitterPrt_c` (one particle, a `Particle_c` from `Particle.h`). `FxFrustumInfo`, `FxBox`, `FxSphere`, `FxPlane_c` are culling helpers.
- `CarFXRenderer.*` (`CCarFXRenderer`) is car dirt / pipeline setup, only loosely related.

## Conventions
- Most classes are `RH_ScopedClass` + category `"Fx"`; `FxEmitterBP_c` (`0x85A788`, 7) and `FxEmitter_c` (`0x85A7A4`,
  6) are `RH_ScopedVirtualClass`. `FxInfo_c` is an abstract `NOTSA_EXPORT_VTABLE` base (`Load(FILESTREAM, version)`,
  `GetValue(...)`).
- Blueprint / particle memory comes from `g_fxMan.GetMemPool().GetMem(size)` (`FxMemoryPool_c`), not `new`. The pool is
  reset as a whole, so objects in it are never freed one by one.
- `.fxp` is a line-based text format read in a fixed order (no seeking). Use `ReadField<T>(file, "NAME:")` from
  `FxTools.h`: it reads one line and checks the field name (NOTSA), so a wrong read order fails loudly.

## Pitfalls
- `Fx.cpp` `InjectHooks`: lines `// + RH_ScopedInstall(...)` have C++ bodies that are not installed yet. Plain
  `// RH_ScopedInstall(...)` ones (`AddBlood`, `AddSparks`, `AddWheel*`, `TriggerGunshot`, ...) still forward through a
  raw `((void(__thiscall*)(...))0xADDR)` cast. Grep for the address, not only `plugin::Call`.
- `FxManager_c::DestroyAllFxSystems` stays `.Reversed = false` on purpose ("broken for some reason?").
- `FxEmitterBP_c`: `CreateInstance` is `.Reversed = false` (marked "bad"); `Render` (`0x4A2C40`) and `RenderHeatHaze`
  are not reversed yet.
- `FxEmitter_c::CreateParticle` uses the literal `6.28318f` (the original's rounded 2π), not `TWO_PI`; keep it.
- Particle math runs every frame for hundreds of particles: keep float vs double exactly as the original (x87).
