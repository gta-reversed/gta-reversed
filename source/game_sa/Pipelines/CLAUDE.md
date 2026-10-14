# Pipelines/ — custom RenderWare D3D9 object pipelines

Guide: [../../../docs/subsystems/Rendering.md](../../../docs/subsystems/Rendering.md) (§2 key classes, §4 RW usage,
§7 what is left). RW itself (`Rw*` / `Rp*` / `Rx*`, `_rwD3D9*`) is called, never reversed.

## Layout
- `CustomBuilding/` — `CCustomBuildingPipeline` (plain buildings), `CCustomBuildingDNPipeline` (day/night prelit
  vertex colours blended by `m_fDNBalanceParam` `0x8D12C0`, extra vertex colour plugin), `CCustomBuildingRenderer`
  (front end used by model infos, `CEntity` / `CObject`, `CGame`: `AtomicSetup`, `IsCBPCPipelineAttached`,
  `UpdateDayNightBalanceParam`).
- `CustomCarEnvMap/` — `CCustomCarEnvMapPipeline`: vehicle env map, specular and "env atm" material/atomic plugins,
  split over `_EnvAtm`, `_EnvMat`, `_SpecMat` .cpp files (plugin callbacks of each data kind).
- `PipelinesCommon.hpp` — inline copies of RW's D3D9 instance-render helpers (`0x5DA640` render, `0x5DA6A0` render
  with lighting, `SetTextureStagesForLighting`). Use them instead of re-deriving the D3D9 call sequence.

## Conventions
- All-static classes with `RH_ScopedClass`. Category: `"Pipelines"` for `CCustomBuildingDNPipeline` and
  `CCustomCarEnvMapPipeline`; `RH_ScopedCategoryGlobal()` for `CCustomBuildingPipeline` and `CCustomBuildingRenderer`.
- RW callbacks (`CustomPipeInstanceCB`, `CustomPipeRenderCB`, plugin constructor/destructor/copy CBs) are hooked like
  normal functions.
- `CreatePipe` / `CreateCustomObjPipe` build an `RxPipeline` stored in a `StaticRef<RxPipeline*> ObjPipeline`;
  `RegisterPlugin` fills the `ms_*PluginOffset` globals (initially -1).
- Plugin data only through the accessors (`EnvMapPlGetData`, `EnvMapAtmPlGetData`, `SpecMapPlGetData`, ...) built on
  `RWPLUGINOFFSET`; per-material data comes from fixed pools (`m_gEnvMapPipeMatDataPool`, ...).

## Pitfalls
- RW stores the callback pointers in pipeline nodes when the pipe is created at startup: toggling a callback's hook in
  F7 later may not change which version RW calls. Restart to test a callback reliably.
- IDA shows `RwD3D9SetRenderState` / `RwRenderStateSet` and many RW macros as indirect calls through
  `RwEngineInstance` (`[0xC97B24] + offset`); match the pattern, not an xref.
- D3D state set here leaks into the next draw: restore texture stages / render states exactly where the original does.
- Our build uses SSE and the original x87: tiny lighting/colour float differences are expected; compare with ULP
  tolerance in difftests.
