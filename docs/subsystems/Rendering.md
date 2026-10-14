# Rendering

How a frame is drawn: which game code runs in which order, where RenderWare (RW) takes over, and what is still left to
reverse. General workflow and hook rules: [../ReversingGuide.md](../ReversingGuide.md),
[../ReversingRules.md](../ReversingRules.md). Where RW and D3DX live in the exe: [../BinaryMap.md](../BinaryMap.md).
RW render-state idioms seen in IDA: [../ai/TipsAndTricks.md](../ai/TipsAndTricks.md) (§1, "RenderWare").

---

## 1. Overview

GTA SA renders with **RenderWare Graphics 3.x on Direct3D 9**. There are two layers:

| Layer | What it is | Status in the repo |
|---|---|---|
| **RenderWare** (`RwCamera*`, `RpClumpRender`, `RwIm2D*`, `RwIm3D*`, `RwRaster*`, `RwD3D9*`, `RwRenderStateSet`...) | Criterion's middleware, statically linked into `gta_sa.exe` | **Not reversed.** Declared in `source/game_sa/RenderWare/rw/*.h` (original SDK headers) and implemented in `rw/*.cpp` as one-line thunks that call the original address (e.g. `RwIm3DTransform` -> `0x7EF450` in `rw/rwcore.cpp`). The coverage script excludes RW and D3DX entirely. |
| **Game rendering code** (`CRenderer`, `CVisibilityPlugins`, `CShadows`, `CCoronas`, `CClouds`, `CPostEffects`, `CSprite*`, `CFont`, `CHud`, `CRadar`, `Fx`, custom pipelines) | Rockstar code that *uses* RW | Mostly reversed, see section 7 |

A few RW functions are hooked anyway, only to add Tracy zones: `RwCameraBeginUpdate`, `RwCameraEndUpdate`,
`RwCameraClear`, `RwCameraShowRaster` (`InjectHooks` at the top of `rw/rwcore.cpp`). Treat everything else under
`RenderWare/rw` as a black box: call it, do not reverse it.

`RenderWare/RenderWare.h` also exposes engine globals: `RwEngineInstance` (0xC97B24), `RsGlobal` (0xC17040),
`GetD3D9Device()` (reads 0xC97C28), the D3D view/projection matrices and the `RWRSTATE(x)` cast macro.
`RenderWare/D3D*.cpp` contain small reversed D3D resource helpers.

Plugins that hang game data on RW objects are attached in `PluginAttach()` (`app/app.cpp`, 0x53D870): visibility,
anim blend, TXD store, MatFX, UV anim, custom building renderer, car FX renderer, breakables, collision, 2D effects,
pipeline plugin (`source/game_sa/Plugins/`).

---

## 2. Key classes and files

| Class / file | Role |
|---|---|
| `app/app_game.cpp` | `Idle` (0x53E920), `FrontendIdle` (0x53E770), `RenderScene` (0x53DF40), `RenderEffects` (0x53E170), `Render2dStuff` (0x53E230), `RenderMenus` (0x53E530) |
| `app/app.cpp` | `AppEventHandler` (0x53EC10), `DoRWStuffStartOfFrame_Horizon` (0x53D7A0), `DoRWStuffEndOfFrame`, `DefinedState` (0x734650), `DefinedState2d` (0x734750) |
| `CRenderer` (`Renderer.h/.cpp`) | World scan, visibility, render lists (visible / LOD / super-LOD / invisible), `PreRender`, `RenderRoads`, `RenderEverythingBarRoads`, fading entities |
| `CVisibilityPlugins` (`VisibilityPlugins.*`) | RW plugin data on atomics/clumps/frames (model index, flags, alpha), sorted alpha lists, vehicle/ped LOD render callbacks |
| `CEntity::Render` (`Entity/Entity.cpp`) | Calls `RpAtomicRender`/`RpClumpRender`; subclasses add `SetupRender`/`ResetAfterRender` |
| `Pipelines/` | Custom D3D9 RW pipelines: `CCustomBuildingPipeline`, `CCustomBuildingDNPipeline` (day/night vertex colours, `m_fDNBalanceParam` 0x8D12C0), `CCustomBuildingRenderer`, `CCustomCarEnvMapPipeline` (env map, spec, env-atm). `PipelinesCommon.hpp` has the inline D3D9 instance render helpers |
| `CCarFXRenderer` (`Fx/CarFXRenderer.*`) | Car dirt texture, car pipe setup, `PreRenderUpdate` |
| `CShadows` (`Shadows.*`), `CStencilShadows`, `CRealTimeShadowManager` (`g_realTimeShadowMan`), `CShadowCamera` | Projected/stored/static/permanent shadows, stencil shadows, real-time ped/vehicle shadows |
| `CCoronas` (`Coronas.*`), `CRegisteredCorona` | Light coronas, sun/moon, reflections on wet roads |
| `CClouds` (`Clouds.*`) | Sky gradient polys, clouds, moon, rainbows, moving fog, volumetric clouds |
| `CWeather`, `CTimeCycle` (`m_CurrentColours` 0xB7C4A0) | Inputs: sky/fog colours, far clip, fog start, ambient, post-FX colours, rain, heat haze, underwater-ness, lightning |
| `CPostEffects` (`PostEffects.*`) | Full-screen effects using a copy of the frame buffer: colour filter, radiosity, night/infrared vision, grain, heat haze, speed FX, underwater ripple, CCTV, fog |
| `CMirrors` | Mirror/reflection render-to-texture (`BeforeMainRender`, `RenderMirrorBuffer`) |
| `CSprite` (`Sprite.*`) | 3D-to-screen billboards (`CalcScreenCoors`, `RenderOneXLUSprite*`, buffered variants, `FlushSpriteBuffer`) |
| `CSprite2d` (`Sprite2d.*`) | Textured/coloured screen quads via Im2D; `maVertices` (0xC80468), `NearScreenZ`, `RecipNearClip` |
| `CFont` (`Font.*`) | Text: `PrintString` buffers glyphs, `DrawFonts` (0x71A210) -> `RenderFontBuffer` flushes them |
| `CHud` (`Hud.*`), `CRadar` (`Radar.*`) | HUD and radar/map drawing, both built on `CSprite2d` + `CFont`; see [HudFrontend.md](HudFrontend.md) |
| `Fx_c` (`g_fx`, 0xA9AE00), `FxManager_c` (`g_fxMan`, 0xA9AE80), `FxSystemBP_c`, `FxSystem_c`, `FxEmitterBP_c`, `FxPrimBP_c` | Particle system (`Fx/`): blueprints from `effects.fxp`, update in `CGame::Process`, render in `RenderEffects` |
| `RenderBuffer` (`RenderBuffer.hpp/.cpp`) | Shared Im3D vertex/index scratch buffer (`TempBufferVertices` 0xC4D958, `aTempBufferIndices` 0xC4B958, declared in `common.h`) |

Other renderers called from the frame: `CBirds`, `CSkidmarks`, `CRopes`, `CGlass`, `CMovingThings`, `CWaterCannons`,
`CWaterLevel`, `CHeli` search lights, `CWeaponEffects`, `CSpecialFX`, `CPointLights`, `CPlantMgr`/`CGrassRenderer`,
`BreakManager_c` (`g_breakMan`), `CMBlur`/`CCamera::RenderMotionBlur`.

---

## 3. Per-frame flow

`AppEventHandler` dispatches `rsIDLE` to `Idle(param)` and `rsFRONTENDIDLE` to `FrontendIdle()`. `Idle`,
`FrontendIdle` and `Render2dStuff` are hooked with `{ .Locked = true }` (`InjectHooks` in `app_game.cpp`): never
unhook them, the game locks (or ImGui stops working).

### Update phase (no drawing)

`Idle`: `CTimer::Update`, `CSprite2d::InitPerFrame`, `CFont::InitPerFrame`, `CPointLights::NumLights = 0`,
`CGame::Process()`, `AudioEngine.Service()`, `SetLightsWithTimeOfDayColour`.
Inside `CGame::Process` the render-related updates are: `CWeather::Update`, `CSkidmarks::Update`, `CGlass::Update`,
`CClouds::Update`, `CMovingThings::Update`, `CBirds/CSpecialFX/CRopes::Update`, `CPostEffects::Update`,
`CTimeCycle::Update`, `CCoronas::DoSunAndMoon` + `Update`, `CShadows::UpdatePermanentShadows`, `CPlantMgr::Update`,
`CCustomBuildingRenderer::Update`, `CStencilShadows::RenderBuffer/Process`, `g_fx.Update`, `g_breakMan.Update`, and
finally `CWaterLevel::PreRenderWater`.

If `param` is null, `Idle` returns here (update only, nothing drawn).

### Visibility (still in `Idle`)

1. **`CRenderer::ConstructRenderList()`**: tunnel attributes from `CCullZones`, "in the sky" check,
   `ms_lowLodDistScale` (scaled by `CTimeCycle::m_CurrentColours.m_fLodDistMult`), `CMirrors::BeforeConstructRenderList`,
   `COcclusion::ProcessBeforeRendering`, reset counters, `ScanWorld()`, `ProcessLodRenderLists()`,
   `CStreaming::StartRenderEntities()`.
2. **`CRenderer::ScanWorld()`** builds a 13-point frustum, transforms it with the camera matrix and rasterises it into
   world sectors (`CWorldScan::ScanWorld(points, n, ScanSectorList)`), then LOD sectors. For each entity
   `SetupEntityVisibility` returns `eRendererVisibility`: `RENDERER_VISIBLE` -> `AddEntityToRenderList`,
   `RENDERER_INVISIBLE` -> invisible list (still pre-rendered), `RENDERER_CULLED`, `RENDERER_STREAMME` (request the
   model).
3. `AddEntityToRenderList` puts distance-fading / draw-last entities into `CVisibilityPlugins`' sorted alpha list,
   LOD parents (`GetNumLodChildren()`) into `ms_aVisibleLodPtrs`, the rest into `ms_aVisibleEntityPtrs`
   (capacities `MAX_VISIBLE_ENTITY_PTRS` 1000, `MAX_VISIBLE_LOD_PTRS` 1000, `MAX_VISIBLE_SUPERLOD_PTRS` 50,
   `MAX_INVISIBLE_ENTITY_PTRS` 150 in `Renderer.h`).
4. **`CRenderer::PreRender()`**: `entity->PreRender()` for every list, plus entities in the alpha and underwater alpha
   lists, then `CHeli::SpecialHeliPreRender` and `CShadows::RenderExtraPlayerShadows`. Entity `PreRender` is where
   lights, coronas, shadows and particles get *registered* for this frame.
5. `CWorld::ProcessPedsAfterPreRender`, `g_realTimeShadowMan.Update()`, `CMirrors::BeforeMainRender()`.

### Drawing (rest of `Idle`)

```
DoRWStuffStartOfFrame_Horizon(sky top/bottom colours)   // camera size, RwCameraClear(Z|stencil), RsCameraBeginUpdate,
                                                        // DefinedState, view matrix, CClouds::RenderSkyPolys (sky gradient)
DefinedState(); far clip = CTimeCycle::GetFarClip(); fogPlane = GetFogStart()
CMirrors::RenderMirrorBuffer()
RenderScene()                                           // 3D world, see below
CVisibilityPlugins::RenderWeaponPedsForPC() / ResetWeaponPedsForPC()
RenderEffects()                                         // transparent/3D effects, see below
TheCamera.RenderMotionBlur()
Render2dStuff()                                         // HUD, text
RenderMenus(); DoFade(); CHud::DrawAfterFade(); CMessages::Display(false); CFont::DrawFonts(); CCredits::Render()
notsa::ui::UIRenderer::GetInstance().DrawLoop()         // NOTSA ImGui
RwCameraEndUpdate; RsCameraShowRaster                   // present
```

**`RenderScene()`**:

1. Z off: `CMovingThings::Render_BeforeClouds`, `CClouds::Render` (skipped while rendering a mirror).
2. Z on: `CCarFXRenderer::PreRenderUpdate`, **`CRenderer::RenderRoads`** (fog on, backface cull, only road buildings),
   `CCoronas::RenderReflections`, **`CRenderer::RenderEverythingBarRoads`** (non-road entities; vehicles and
   semi-transparent peds go to the sorted/underwater alpha lists; LODs drawn last with `zShift -= 100`),
   `g_breakMan.Render(false)`.
3. `RenderFadingInUnderwaterEntities`, water (when the camera is underwater), `RenderFadingInEntities` (alpha list).
4. Shadows with a pushed near plane: `CShadows::UpdateStaticShadows`, `RenderStaticShadows`, `RenderStoredShadows`
   (camera end/begin update around a near-clip change).
5. `g_breakMan.Render(true)`, `CPlantMgr::Render`, then (no mirror) `CClouds::RenderBottomFromHeight`,
   `CWeather::RenderRainStreaks`, `CCoronas::RenderSunReflection`, water (camera above water), and
   `CStencilShadows::RenderStencilShadows`.

**`RenderEffects()`**: `CBirds`, `CSkidmarks`, `CRopes`, `CGlass`, `CMovingThings`,
`CVisibilityPlugins::RenderReallyDrawLastObjects`, **`CCoronas::Render`**, **`g_fx.Render(camera, false)`**,
`CWaterCannons`, `CWaterLevel::RenderWaterFog`, `CClouds::MovingFogRender` + `VolumetricCloudsRender`, heli/script
search lights, `CWeaponEffects`, mouse-recruit triangle, `CSpecialFX::Render`, `CVehicleRecording::Render`,
`CPointLights::RenderFogEffect`, `CRenderer::RenderFirstPersonVehicle`, **`CPostEffects::Render`**, NOTSA `Render3D`.

**`Render2dStuff()`**: 2D render states (no Z, vertex alpha, src-alpha blend, no cull), replay display, pickup text,
widescreen borders, sniper/rocket zoom box, `AudioEngine.DisplayRadioStationName`, **`CHud::Draw`**,
`CSpecialFX::Render2DFXs`, on-screen timers, `CMessages::Display(true)`, `CDarkel`, `CGarages`,
**`CFont::DrawFonts`**. The HUD side is described in [HudFrontend.md](HudFrontend.md).

**`CPostEffects::Render`** first copies the camera raster into `pRasterFrontBuffer` (`RwRasterPushContext` +
`RwRasterRenderFast`), then applies, in order: fog, colour filter (time-cycle post-FX colours), darkness filter +
radiosity, speed FX, night vision / infrared + grain, radiosity, rain grain, grain, heat haze (`HeatHazeFX`, run when
`CWeather::HeatHaze > 0` or `g_fxMan.m_bHeatHazeEnabled`), underwater ripple, CCTV, photo capture.
`FxManager_c::Render(camera, bHeatHaze)` has a separate heat-haze particle pass; the normal pass is
`g_fx.Render(camera, false)` in `RenderEffects`.

`FrontendIdle` is the menu-only path: clear, `DrawFrontEnd`, fade, fonts, credits, ImGui, present.

---

## 4. RenderWare usage and helpers

- **Render states.** `RwRenderStateSet(rwRENDERSTATEXXX, RWRSTATE(value))` is a macro that calls the device's
  `fpRenderStateSet` through `RwEngineInstance` (`rw/rwplcore.h`). It is never a reversing target. Save/restore with
  `RwRenderStateGet(state, RWRSTATE(&saved))` (example: `CEntity::Render`).
- **Baseline state.** `DefinedState()` / `DefinedState2d()` reset everything (linear filter, src-alpha/inv-src-alpha,
  alpha test `> 2`, fog colour = sky bottom colour). Functions assume this baseline and leave states changed; SA
  rarely restores. Reproduce state changes exactly, including redundant ones.
- **Im2D (screen space).** Fill `RwIm2DVertex` (via `CSprite2d::SetVertices` or the `RwIm2DVertexSet*` macros) and call
  `RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, verts, 4)` (see `CSprite2d::Draw`). Screen Z comes from
  `CSprite2d::NearScreenZ` / `RecipNearClip`, refreshed by `CSprite2d::SetRecipNearClip`.
- **Im3D (world space).** `RwIm3DTransform(verts, n, ltm, flags)` -> `RwIm3DRenderIndexedPrimitive(...)` ->
  `RwIm3DEnd()`; examples in `CCoronas::Render` and `CClouds`.
- **`RenderBuffer` namespace.** Preferred wrapper over the shared temp buffers: `StartStoring`/`StopStoring` (original
  API, 0x708290/0x7077A0), `RenderStuffInBuffer` (0x707800), `ClearRenderBuffer`, `RenderIfDoesntFit`, and NOTSA
  `PushVertex`/`PushIndex`/`PushIndices`/`CanFitVertices`/`Render`. Used by `CShadows` and `CCoronas`. The buffer is
  shared by everybody: flush it before another system uses it.
- **Textures** come from TXDs (`CTxdStore`); 2D sprites wrap them in `CSprite2d` (`SetTexture`, `Delete`).
  `RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(tex)))` binds one.
- **Pipelines.** Custom pipelines replace RW's D3D9 instance render callbacks (`CustomPipeRenderCB`) and talk to D3D9
  directly via `RwD3D9SetTexture`, `RwD3D9SetTextureStageState`, `RwD3D9SetVertexShader`, `RwD3D9DrawIndexedPrimitive`
  (`Pipelines/PipelinesCommon.hpp`). Material/atomic extension data is accessed through plugin offsets.
- **Visibility plugin data.** Read/write it only through `CVisibilityPlugins` accessors (`GetAtomicId`,
  `SetClumpAlpha`, `GetModelInfo(atomic)`, `GetFrameHierarchyId`...), never by raw offsets.
- **Profiling.** `ZoneScoped` (Tracy) is in most top-level render functions; `PUSH_RENDERGROUP`/`POP_RENDERGROUP`
  are no-op macros (`app/app_debug.h`).
- **Debug modules** (F7, `source/toolsmenu/DebugModules/`): `PostEffectsDebugModule`, `TimeCycleDebugModule`,
  `CloudsDebugModule`, `COcclusionDebugModule`, `CullZonesDebugModule`, `ParticleDebugModule`,
  `TwoDEffectsDebugModule`, `WaterLevelDebugModule`.

---

## 5. Gotchas

- **`underWater` in `RenderScene` is inverted.** `const auto underWater = CWeather::UnderWaterness <= 0.0f;` is
  *true when not underwater*. The water order follows the original; do not "fix" the name without checking both
  branches.
- **Function-pointer identity with hooks.** Alpha lists store render callbacks. `CRenderer::PreRender` compares
  `m_pCallback` against both our `CVisibilityPlugins::RenderEntity` and the original `0x732B40`. Any code that compares
  callback pointers must accept both, otherwise toggling the hook changes behaviour.
- **LOD render lists live in `PC_Scratch`.** `CRenderer::GetLodRenderListBase()` returns `&PC_Scratch[24]`, and the
  sector scan lists (`SetupScanLists`, `ScanSectorList`) sit at the start of `PC_Scratch`. Other code that uses `PC_Scratch` during the
  frame can corrupt them.
- **Camera begin/end pairs.** Changing near clip or `zShift` requires `RwCameraEndUpdate` -> change ->
  `RwCameraBeginUpdate` (see `RenderEverythingBarRoads` and the shadow block in `RenderScene`). Missing a pair breaks
  D3D state silently.
- **Font text is deferred.** `CFont::PrintString` only buffers; nothing appears until `CFont::DrawFonts` ->
  `RenderFontBuffer` (still `.Reversed = false`). The order of `PrintString` calls matters for overlap.
- **Sprite buffers are deferred too.** `CSprite::RenderBufferedOneXLUSprite*` needs `FlushSpriteBuffer()`; mixing
  buffered and immediate sprites changes draw order.
- **`RwIm2D*`/`RwRenderStateSet` are macros**, so they do not show up as calls to a fixed address in IDA; you see
  indirect calls through `RwEngineInstance` (`[0xC97B24] + offset`). Recognise the pattern instead of searching xrefs.
- **Screen-space float differences.** The original computes in x87; tiny differences in screen coordinates are normal
  (see R4 in [../ReversingRules.md](../ReversingRules.md)).
- **Vision FX lighting.** Roads and entities temporarily change the day/night balance or lights when night/infrared
  vision is active (`CPostEffects::FilterFX_StoreAndSetDayNightBalance`, `NightVisionSetLights`, called from
  `CRenderer::RenderOneRoad`, `RenderRoads` and `RenderOneNonRoad`).
- **`CMirrors::bRenderingReflection` / `TypeOfMirror`** skip clouds, rain, sun reflection and stored shadows during the
  mirror pass; keep those checks.

---

## 6. What is RW vs game code (quick test)

| You see in IDA | It is |
|---|---|
| Address in the RW ranges of [../BinaryMap.md](../BinaryMap.md), names `Rw*`, `Rp*`, `Rt*`, `_rw*`, `_rx*` | RW: declare/call through `RenderWare/rw` wrappers |
| `RwD3D9*`, `rwD3D9*`, D3DX helpers | RW D3D9 driver / D3DX: do not reverse |
| Calls through the `RwEngineInstance` function table | `RwRenderStateSet`/`RwIm2D*` macros |
| `C*` class methods, `Fx*_c`, `*CB` callbacks registered with RW (`RenderVehicleHiDetailCB`, `CustomPipeRenderCB`) | Game code: reverse |

If a needed RW function has no wrapper, add one in the matching `rw/*.cpp` in the same thunk style
(`((Ret(__cdecl*)(Args...))0xADDR)(args...)`), with the address taken from IDA.

---

## 7. What is left

From `idb_coverage.py --class <C>` (IDB export, method scope). Rev = reversed, Dormant = reversed but the class'
`InjectHooks()` is not called, Stub = hooked with `.Reversed = false`, Known = address in repo but not hooked. Refresh
with `idb_coverage.py` before relying on the numbers.

| Class | Total | Rev | Stub | Known | Missing | Notes |
|---|---:|---:|---:|---:|---:|---|
| `CRenderer` | 37 | 32 | 0 | 0 | 5 | Only trivial `Init`/`Shutdown`/loading-priority accessors and one `ScanPtrList_RequestModels` instantiation |
| `CVisibilityPlugins` | 82 | 76 | 0 | 6 | 0 | |
| `CShadows` | 30 | 20 | 7 | 2 | 1 | `CastShadowEntityXY` (5.7 KB), `CastShadowEntityXYZ`, `StoreShadowForVehicle`, `AddPermanentShadow`, `UpdatePermanentShadows`, `StoreStaticShadow`, `GeneratePolysForStaticShadow`; sector-list casters |
| `CHud` | 38 | 0 (+30 dormant) | 8 | 0 | 0 | `CHud::InjectHooks()` is deliberately **not** called from `InjectHooksMain` (untested in game), so the 30 reversed functions are never installed. Stubs: `DrawPlayerInfo` (4 KB), `DrawVitalStats`, `DrawCrossHairs`, `DrawHelpText`, `DrawWanted`, `DrawMissionTimers`, `DrawSubtitles`, `DrawSuccessFailedMessage` |
| `CPostEffects` | 43 | 33 | 3 | 6 | 1 | `Radiosity` (3.7 KB), `HeatHazeFX`, `SpeedFX`; small `DrawQuadSet*UVs` known-only |
| `CFont` | 37 | 30 | 3 | 0 | 4 | `RenderFontBuffer`, `PrintChar`, `ProcessCurrentString` (0x71A220) stubs; `PrintString(float,float,uint,...)` 0x719B40 missing |
| `CSprite` | 23 | 15 | 0 | 5 | 3 | `Render_4_Rot`/`Render_5_Rot` missing, some 2D rotate variants known-only |
| `CSprite2d` | 37 | 33 | 0 | 3 | 1 | |
| `CRadar` | 64 | 59 | 2 | 1 | 2 | Stubs `ClipRadarPoly`, `DrawEntityBlip` |
| `CCoronas`, `CTimeCycle`, `CMirrors`, `CPointLights`, `CBirds`, `CSkidmarks`, `CSpecialFX`, `CMovingThings`, `CBrightLights`, `CShadowCamera`, `CGrassRenderer`, `CStencilShadows`, `CWaterCannons`, `RenderBuffer` | | all | 0 | 0 | 0 | Done |
| `CClouds` | 21 | 20 | 1 | 0 | 0 | `RenderBottomFromHeight` |
| `CWeather` | 14 | 13 | 0 | 1 | 0 | `UpdateWeatherRegion` hook commented out as "bad" |
| `CWaterLevel` | 36 | 24 | 8 | 1 | 3 | Stubs include `RenderWater`, `RenderWaterFog`, `RenderHighDetailWaterRectangle/Triangle`; `RenderHighDetailWater*_OneLayer` missing |
| `CCustomBuildingRenderer` | 7 | 3 | 4 | 0 | 0 | `AtomicSetup`, `IsCBPCPipelineAttached`, `UpdateDayNightBalanceParam`, `Update` |
| `CCustomBuildingDNPipeline` | 18 | 13 | 2 | 2 | 1 | Stubs `CustomPipeMaterialSetup`, `CustomPipeRenderCB` |
| `CCustomCarEnvMapPipeline` | 40 | 36 | 0 | 3 | 1 | |
| `CCarFXRenderer` | 9 | 6 | 1 | 0 | 2 | `InitialiseDirtTexture` |
| `CRealTimeShadowManager` / `CRealTimeShadow` | 5 / 3 | 4 / 1 | 1 / 0 | 0 | 0 / 2 | |
| `CWeaponEffects` | 11 | 8 | 1 | 0 | 2 | `Render` |
| `CMBlur` | 6 | 0 | 0 | 6 | 0 | Addresses known, nothing hooked |
| `FxManager_c` | 25 | 24 | 1 | 0 | 0 | `DestroyAllFxSystems` stub ("broken for some reason?") |
| `FxSystem_c` | 36 | 31 | 4 | 1 | 0 | `Update`, `UpdateBoundingBoxCB`, `AddParticle` overloads |
| `Fx_c` | 36 | 3 | 0 | 31 | 2 | 10.9 KB left. Most `Add*`/`Trigger*`/init hooks are commented out in `Fx_c::InjectHooks`; some bodies still call the original address directly |
| `FxEmitterBP_c` | 12 | 2 | 7 | 1 | 2 | 8.2 KB left: `Render` (0x4A2C40), `RenderHeatHaze`, `UpdateParticle`, `Update`, `CreateInstance` ("bad"), `Load` |
| `FxEmitter_c`, `FxInfoManager_c`, `FxPrimBP_c` | 9 / 6 / 4 | 0 / 4 / 1 | 1 / 1 / 1 | | | 1-3 KB each left |

Remaining work, by impact:

1. **Particles** (`Fx/`): `Fx_c` effect spawners and `FxEmitterBP_c::Render` are the largest rendering gap
   (~19 KB together). Particle rendering itself (`RenderBegin`/`RenderAddTri_`/`RenderEnd` in `Fx.cpp`) is reversed.
2. **HUD**: register `CHud` (validate the 30 dormant functions in game first), then the eight `CHud::Draw*` stubs
   (~17 KB) plus `CFont::RenderFontBuffer`/`PrintChar`/`ProcessCurrentString`.
3. **Shadows**: the `CShadows::Cast*` family and static/permanent shadow storage (~13 KB).
4. **Water**: `CWaterLevel::RenderWater` and the high-detail water render stubs (~13 KB).
5. **Post effects**: `Radiosity`, `HeatHazeFX`, `SpeedFX` (~7 KB).
6. Smaller: `CCustomBuildingRenderer` (4 stubs), `CCustomBuildingDNPipeline` (2 stubs), `CSprite::Render_4_Rot/Render_5_Rot`,
   `CMBlur`, `CClouds::RenderBottomFromHeight`, `CCarFXRenderer::InitialiseDirtTexture`.

Verify render work visually with the F7 hook toggles (A/B against the original) and, where it applies, the matching
debug module; screen-space code is easiest to check at several resolutions and in widescreen mode.
