# Camera

How the game camera works: `CCamera` (the global `TheCamera`), its three `CCam`s, how a camera mode is chosen and
processed every frame, how scripts take control, and how the result reaches RenderWare. The second half is a work
plan for the biggest unreversed block in the subsystem: the `CCam::Process_*` functions. General rules are in
[../ReversingRules.md](../ReversingRules.md) and [../ReversingGuide.md](../ReversingGuide.md).

Paths are relative to `source/game_sa/`. Addresses come from the repo (`RH_ScopedInstall`, `// 0x...`,
`StaticRef`). Call graphs marked "IDA" were read from the IDA database through
`.claude/skills/find-reversing-targets/scripts/ida_rpc.py` (`callees`, `xrefs_to`, `decompile`).

---

## 1. Overview

```
CGame::Process
  └─ TheCamera.Process()                  CCamera::Process 0x52B730  (stub)
       ├─ UpdateTargetEntity              who are we looking at (player ped or its vehicle)
       ├─ CamControl                      0x527FA0 (stub, ~14 KB): pick the wanted eCamMode, start transitions,
       │                                  cinematic ("Obbe") cams via TryToStartNewCamMode
       ├─ ProcessScriptedCommands         move/track/FOV lerps requested by scripts
       ├─ ProcessFade / ProcessMusicFade / ProcessWideScreenOn
       ├─ m_aCams[m_nActiveCam].Process() CCam::Process 0x526FC0 (stub): look-behind/left/right, then the
       │                                  per-mode Process_* function writes m_vecSource/m_vecFront/m_vecUp/m_fFOV
       ├─ transition blending             old cam values -> new cam values over m_nTransitionDuration
       ├─ ProcessShake, build m_mCameraMatrix
       └─ CDraw::SetFOV, CalculateDerivedValues, CopyCameraMatrixToRWCam, UpdateSoundDistances, far clip
```

- `CCamera` derives from `CPlaceable`, so the final camera matrix is also `TheCamera.GetMatrix()` /
  `TheCamera.GetPosition()`. `m_mCameraMatrix` holds the same data while it is being built.
- The camera runs **after** `CWorld::Process` in `CGame::Process` (see [WorldStreamingPools.md](WorldStreamingPools.md) §7).
- Only the **active** `CCam` is processed each frame. Interpolation between two modes happens in `CCamera::Process`
  from values saved when the transition started, not by running two cams.
- `m_aCams[2]` is the debug camera (comment in `Camera.h`).

---

## 2. Key classes and files

| Class / global | File | Size / address | Notes |
|---|---|---|---|
| `CCamera` | `Camera.h` | `0xD78` | Derives `CPlaceable`. Owns `m_aCams[3]`, transition state, fades, shakes, script lerps, frustum, mirror matrices |
| `CCam` | `Cam.h` | `0x238` | One camera "slot": mode, source/front/up, FOV, buffered angles, target history. Not polymorphic |
| `CQueuedMode` | `QueuedMode.h` | `0xC` | `m_PlayerMode`, `m_PlayerWeaponMode`: mode + duration + zoom limits |
| `CCamPathSplines` | `CamPathSplines.h` | `4` | A single `float* m_pArrPathData`. `CCamera::m_aPathArray[4]` |
| `CamTweak` | `Camera.h` | `0x10` | Per-vehicle-model distance/altitude/angle (`m_aCamTweak[5]`, `InitCameraVehicleTweaks`) |
| `CIdleCam` | `IdleCam.h` | `gIdleCam` `0xB6FDA0` | Idle cinematic. Mostly reversed; `ProcessFOVZoom` (`0x517BF0`) is a stub |
| `TheCamera` | `Camera.cpp` | `0xB6F028` | `StaticRef<CCamera>` |
| `gCameraMode`, `gCameraDirection` | `Camera.cpp` | `0x8CC388`, `0x8CC384` | `CCam::Process` writes both |
| `gCamColVars[28][6]`, `gpCamColVars`, `gCurCamColVars` | `Camera.cpp` | `0x8CC8E0`, `0xB6FE88`, `0x8CCB80` | Collision tuning sets chosen by `SetColVarsPed/Vehicle/AimWeapon` |
| `gpMadeInvisibleEntities[10]` | `Camera.h` | | Entities hidden because they block the camera. Reset first thing in `CCamera::Process` |
| `gbFirstPersonRunThisFrame` | `Cam.cpp` | `0xB6EC20` | Set by `Process_1stPerson` |
| DW cine-cam statics | `Cam.cpp` | `0x8CCB9C`, ... | `gLastFrameProcessedDWCineyCam`, `DWCineyCamLast*` |

Important `CCam` fields: `m_nMode` (`eCamMode`), `m_vecSource` (position), `m_vecFront`, `m_vecUp`, `m_fFOV`,
`m_pCamTargetEntity`, `m_fHorizontalAngle` ("beta"), `m_fVerticalAngle` ("alpha", radians), `m_vecTargetCoorsForFudgeInter`
(the point the cam looks at; used by the transition code), `m_bResetStatics` (first frame of a mode),
`m_nDirectionWasLooking` (`3` = forward), `m_fTrueAlpha` / `m_fTrueBeta`.

Important `CCamera` fields: `m_nActiveCam`, `m_pTargetEntity`, `m_nModeToGoTo`, `m_nTypeOfSwitch` (`eSwitchType`),
`m_bLookingAtPlayer`, `m_bLookingAtVector`, `m_bStartInterScript`, `m_nWhoIsInControlOfTheCamera`,
`m_bTransitionState`, `m_nTimeTransitionStart`, `m_nTransitionDuration`, `m_vec*WhenInterPol` / `*DuringInter`,
`m_nCarZoom` / `m_nPedZoom` (zoom level), `m_pRwCamera`, `m_mCameraMatrix`, `m_mViewMatrix`, frustum normals.

There are no virtuals besides `CCamera`'s destructor (from `CPlaceable`).

### Camera modes (`Enums/eCamMode.h`) and their `Process_*`

`CCam::Process` ends in a `switch (m_nMode)`. Mapping read from IDA (decompile of `0x526FC0`):

| Mode(s) | Function | Address | Status |
|---|---|---|---|
| `BEHINDCAR`, `CAM_ON_A_STRING`, `BEHINDBOAT` | `Process_FollowCar_SA(..., false)` | `0x5245B0` | stub, 6299 B |
| `FOLLOWPED` (mouse 3rd person off) | `Process_FollowPed_SA(..., false)` | `0x522D40` | stub, 6249 B |
| `FOLLOWPED` (`CCamera::m_bUseMouse3rdPerson`) | `Process_FollowPedWithMouse` | `0x50F970` | stub, 3152 B |
| `SNIPER`, `M16_1STPERSON`, `HELICANNON_1STPERSON`, `CAMERA` | `Process_M16_1stPerson` | `0x5105C0` | stub, 5492 B |
| `ROCKETLAUNCHER` / `ROCKETLAUNCHER_HS` | `Process_Rocket(..., false/true)` | `0x511B50` | reversed |
| `WHEELCAM` | `Process_WheelCam` | `0x512110` | stub, 2558 B |
| `FIXED` | `Process_Fixed` | `0x51D470` | reversed |
| `1STPERSON` | `Process_1stPerson` | `0x517EA0` | reversed |
| `FLYBY` | `Process_FlyBy` | `0x5B25F0` | stub, 1188 B |
| `PED_DEAD_BABY` | `ProcessPedsDeadBaby` (then clears garage flags) | `0x519250` | stub, 1459 B |
| `ARRESTCAM_ONE` | `ProcessArrestCamOne` | `0x518500` | stub, 3367 B |
| `ARRESTCAM_TWO` | nothing | | |
| `SPECIAL_FIXED_FOR_SYPHON` | `Process_SpecialFixedForSyphon` | `0x517500` | stub, 553 B |
| `*_RUNABOUT` (sniper, rocket, 1st person, M16, fight, rocket HS) | `Process_1rstPersonPedOnPC` | `0x50EB70` | stub, 2156 B (partial C++ exists) |
| `EDITOR` | `Process_Editor` | `0x50F3F0` | reversed |
| `ATTACHCAM` | `Process_AttachedCam` | `0x512B10` | stub, 977 B |
| `TWOPLAYER` | `Process_Cam_TwoPlayer` | `0x525E50` | stub, 2406 B |
| `TWOPLAYER_IN_CAR_AND_SHOOTING` | `Process_Cam_TwoPlayer_InCarAndShooting` | `0x519810` | stub, 3126 B |
| `TWOPLAYER_SEPARATE_CARS` | `Process_Cam_TwoPlayer_Separate_Cars` | `0x513510` | stub, 1744 B |
| `TWOPLAYER_SEPARATE_CARS_TOPDOWN` | `Process_Cam_TwoPlayer_Separate_Cars_TopDown` | `0x513BE0` | reversed |
| `AIMWEAPON`, `AIMWEAPON_FROMCAR`, `AIMWEAPON_ATTACHED` | `Process_AimWeapon` | `0x521500` | stub, 6004 B |
| `DW_HELI_CHASE`, `DW_CAM_MAN`, `DW_BIRDY`, `DW_PLANE_SPOTTER` | `Process_DW_HeliChaseCam` / `CamManCam` / `BirdyCam` / `PlaneSpotterCam` (`false`) | `0x51A740` / `0x51B120` / `0x51B850` / `0x51C250` | stubs |
| `DW_PLANECAM1..3` | `Process_DW_PlaneCam1..3(false)` | `0x51C760` / `0x51CC30` / `0x51D100` | stubs |
| `DW_DOG_FIGHT`, `DW_FISH` | only `TheCamera.m_bUseNearClipScript = false` | | |
| anything else | source = 0, front = (0,1,0), up = (0,0,1) | | |

All "normal" `Process_*` share the signature `(const CVector& target, float orientation, float speedVar,
float speedVarWanted)`. `CCam::Process` computes those from the target entity first.

---

## 3. Frame flow

### 3.1 `CCamera::Process` (`0x52B730`, stub)

Order of the main calls (IDA):

1. `ResetMadeInvisibleObjects()`, clear `m_bJust_Switched`.
2. If `m_bLookingAtPlayer`, `m_bTargetJustBeenOnTrain` or `m_nWhoIsInControlOfTheCamera == 2`:
   `UpdateTargetEntity()` (`0x50C360`, reversed). Then re-target the active cam and the other cam
   (`(m_nActiveCam + 1) % 2`) to `m_pTargetEntity` with `RegisterReference`.
3. **`CamControl()`** (`0x527FA0`).
4. `ProcessVectorMoveLinear()`, `ProcessVectorTrackLinear()`, `ProcessFOVLerp()` (this is the inlined
   `ProcessScriptedCommands`, comment "inlined - 0x52B845" in `Camera.cpp`). `ProcessFade`, `ProcessMusicFade`,
   `ProcessWideScreenOn` when the matching flags are set.
5. `RwCameraSetNearClipPlane(Scene.m_pRwCamera, 0.3f)`, then **`m_aCams[m_nActiveCam].Process()`**.
6. End the transition when `CTimer` time passes `m_nTimeTransitionStart + m_nTransitionDuration`. Apply
   `m_fNearClipScript` if `m_bUseNearClipScript`. Set `m_bJust_Switched` when a wrapped angle difference exceeds 0.3 rad.
7. `ProcessShake(...)` (`0x516560`, stub) while the shake timer runs.
8. **Transition blend** when `m_bTransitionState`: the fraction `t = elapsed / m_nTransitionDuration` is shaped by
   `m_fFractionInterToStopMoving` / `m_fFractionInterToStopCatchUp` (and the `*Target` variants), and source,
   target, up and FOV are interpolated between the values stored at the start (`m_vecSourceWhenInterPol`, ...,
   with the start speeds `m_vec*SpeedAtStartInter`) and the new cam's output. `StoreValuesDuringInterPol`
   (`0x50C290`, reversed) saves the blended values; `CCam::KeepTrackOfTheSpeed` (`0x509DF0`, reversed) records speeds.
9. Build `m_mCameraMatrix` from source/front/up, apply shake offset, water and 1st-person special cases.
10. `CDraw::SetFOV(fov)`, **`CalculateDerivedValues(false, true)`** (`0x5150E0`, stub), **`CopyCameraMatrixToRWCam(false)`**
    (`0x50AFA0`, stub), copy `m_vecGameCamPos`, `UpdateSoundDistances()` (`0x515BD0`, stub), far clip from the time
    cycle, previous-position bookkeeping and `m_fCameraAverageSpeed`.

### 3.2 `CCamera::CamControl` (`0x527FA0`, stub, ~14 KB)

Decides which mode the active cam should be in. IDA callees show what it touches: `CPad` look/target buttons,
`Restore` / `RestoreWithJumpCut`, garages (`CGarages::IsPointInAGarageCameraZone`, `CameraShouldBeOutside`,
`SetCamPositionForFixedMode`), cull zones with stairs, `GetArrPosForVehicleType`, `ClearPlayerWeaponMode`,
`CCam::GetWeaponFirstPersonOn`, jetpack/hold tasks, `SetCameraDirectlyBehindForFollowPed_CamOnAString`,
`StartTransition` (`0x515200`, reversed) and five unnamed helpers that call **`TryToStartNewCamMode`** (`0x51E560`,
stub). Those helpers are most likely the `ProcessObbeCinemaCamera{Car,Heli,Boat,Plane,Train}` functions that
`Camera.h` declares with `assert(0)` bodies and no address. Find them with `xrefs_to 0x51E560` and confirm before
naming them.

`TryToStartNewCamMode(camSequence)` is a big `switch` on the cinematic sequence number. Each case checks
conditions (line of sight, ground/roof Z, water, vehicle type), then calls `TakeControl(entity, mode, JUMPCUT, 2)` or
probes a DW cam with `Process_DW_*(true)` and returns whether the shot is usable.

Mode changes go through `StartTransition(newMode)` or `StartTransitionWhenNotFinishedInter` (`0x515BC0`).
`StartTransition` copies the fixed-mode vectors and target entity into the active cam, rotates the player for weapon
modes, sets `m_bTransitionState`, durations and the "when interpolation started" values. `m_nTypeOfSwitch == JUMPCUT`
skips the blend.

### 3.3 `CCam::Process` (`0x526FC0`, stub; our body is `NOTSA_UNREACHABLE()`)

1. Clear crosshair if needed, advance the collision-check frame counter (`m_bCollisionChecksOn`).
2. Work out `target`, `orientation`, `speedVar`, `speedVarWanted` from `m_pCamTargetEntity` (vehicle or ped; climb
   task target via `CTaskSimpleClimb::GetCameraTargetPos`; fight-cam special case in `MODE_AIMWEAPON`).
3. Look direction: read `CPad::GetLookBehindForCar/ForPed`, `GetLookLeft/Right`, set `gCameraDirection`
   (0 = behind, 1 = left, 2 = right, 3 = forward, matching `LookRight(false/true)`), break transitions when the
   player starts looking, set `TheCamera.m_bJust_Switched` / `m_bResetOldMatrix`.
4. `switch (m_nMode)` (§2, camera modes).
5. Write `gCameraMode`, compute `m_fTrueBeta` / `m_fTrueAlpha` from `m_vecSource - m_vecTargetCoorsForFudgeInter`,
   `KeepTrackOfTheSpeed` (not during transitions), save `m_vecSourceBeforeLookBehind`.
6. On the active cam only: `LookBehind()` (`0x520690`, stub) or `LookRight(bool)` (`0x520E40`, stub) depending on
   `gCameraDirection`, store `m_nDirectionWasLooking`.
7. Apply the script lerp results: `m_bFOVLerpProcessed` -> `m_fFOV = m_fFOVNew`, `m_bVecMoveLinearProcessed` ->
   `m_vecSource`, `m_bVecTrackLinearProcessed` -> `m_vecFront` + `GetVectorsReadyForRW()`.

### 3.4 Render matrix

- `CCamera::SetRwCamera` (`0x50C100`) stores the `RwCamera` (`Scene.m_pRwCamera`, set from `CGame::InitialiseRenderWare`).
- `CopyCameraMatrixToRWCam` (`0x50AFA0`, stub; IDA callees: `CMatrix::UpdateMatrix`, `RwMatrixUpdate`,
  `RwFrameUpdateObjects`, `RwFrameOrthoNormalize`) writes `m_mCameraMatrix` into the RW frame. `GetRwMatrix()` reads
  it back.
- `CalculateDerivedValues` (`0x5150E0`, stub; IDA callees: `InvertMatrix`, `CMatrix`, `CalculateFrustumPlanes`)
  inverts the camera matrix and calls `CalculateFrustumPlanes` (`0x514D60`, stub; calls `TransformVectors`). The
  frustum members (`m_avecFrustumNormals`, `m_avecFrustumWorldNormals`, `m_fFrustumPlaneOffsets`) feed
  `IsSphereVisible` (`0x420C40`, stub).
- Mirrors: `DealWithMirrorBeforeConstructRenderList`, `CalculateMirroredMatrix`, `SetCameraUpForMirror`,
  `RestoreCameraAfterMirror` are reversed and call the two stubs above.
- `CReplay` also calls `CalculateDerivedValues(false, true)`.

---

## 4. Interpolation, cinematic cams and splines

- **Mode transitions**: §3.1 step 8 and `StartTransition`. Script interpolation tuning:
  `SetParametersForScriptInterpolation(stopMoving, catchUp, time)` (`0x50C030`).
- **Script lerps** (reversed): `VectorMoveLinear` (`0x50D160`), `VectorTrackLinear` (`0x50D1D0`), `LerpFOV`
  (`0x50D280`) set start/end values; `ProcessVectorMoveLinear` / `ProcessVectorTrackLinear` / `ProcessFOVLerp` compute
  them with an optional sine ease; `CCam::Process` applies them (§3.3 step 7). `m_bCameraPersistPosition` / `Track` /
  `m_bBlockZoom` keep the last value after the end time.
- **Shakes**: `CamShake` (`0x50A9F0`), `CamShakeNoPos` (`0x50A970`), `AddShakeSimple` (`0x50D240`) are reversed; the
  per-frame `ProcessShake(float)` (`0x516560`) is a stub.
- **DW ("Dave White") cinematic cams**: `MODE_DW_*`. Each `Process_DW_*` uses the reversed helpers
  `GetCoreDataForDWCineyCamMode` (`0x517130`), `IsTimeToExitThisDWCineyCamMode` (`0x517400`),
  `Finalise_DW_CineyCams` (`0x50DD70`), `CacheLastSettingsDWCineyCam` (`0x50D7A0`), plus `CHandShaker`, and
  `CHeliCamSettings` (heli chase; 3 functions not in the repo). `GetLookFromLampPostPos` (`0x5161A0`) and `IsLampPost`
  serve the cam-man and birdy cams.
- **Idle cam**: `CIdleCam::Process` (`0x522C80`) is called from `Process_FollowPed_SA` (IDA callee).
- **Cutscene splines**: `LoadPathSplines` (`0x5B24D0`, stub) fills `m_aPathArray[4]`; `DeleteCutSceneCamDataMemory`
  (`0x5B24A0`) frees them; `TakeControlWithSpline` (`0x50CAE0`) switches to `MODE_FLYBY`; `SetPercentAlongCutScene`
  / `GetPositionAlongSpline` use `m_fPositionAlongSpline`. `Process_FlyBy` (`0x5B25F0`) only calls two unnamed
  helpers placed next to it (IDA), which evaluate the splines. `CCamPathSplines` has 2 functions missing in the repo.

### Script camera control

Script commands live in `Scripts/Commands/CameraCommands.cpp`.

| API | Address | What it does |
|---|---|---|
| `TakeControl(entity, mode, switchType, who)` | `0x50C7C0` | `MODE_NONE` picks `FOLLOWPED` for peds, `CAM_ON_A_STRING` for vehicles; no entity means the player. Sets `m_nModeToGoTo`, `m_bStartInterScript` |
| `TakeControlNoEntity(pos, switchType, who)` | `0x50C8B0` | `MODE_FIXED` looking at `pos` (`POINT_CAMERA_AT_POINT`) |
| `TakeControlAttachToEntity(...)` | `0x50C910` | `ATTACH_CAMERA_TO_*` commands. Still a stub (see Gotchas) |
| `TakeControlWithSpline(switchType)` | `0x50CAE0` | Cutscene fly-by |
| `SetCamPositionForFixedMode(src, upOffset)` | `0x50BEC0` | Position for `MODE_FIXED` |
| `Restore()` / `RestoreWithJumpCut()` | `0x50B930` / `0x50BAB0` | Give the camera back to the game |
| `SetZoomValueFollowPedScript` / `SetZoomValueCamStringScript` | `0x50C160` / `0x50C1B0` | Script zoom |
| `Fade(duration, dir)`, `SetFadeColour` | `0x50AC20`, `0x50BF00` | Fades |
| `SetWideScreenOn/Off`, `SetNearClipScript` | `0x50C140`/`0x50C150`, `0x50BF90` | |

`who` (`m_nWhoIsInControlOfTheCamera`): scripts pass `1`, the cinematic code passes `2`. In `TakeControl` a request
with `2` is ignored while a script (`1`) holds the camera, unless `m_bCinemaCamera`. Gameplay code asks for weapon
modes with `SetNewPlayerWeaponMode` / `ClearPlayerWeaponMode` (`CPlayerPed::UpdateCameraWeaponModes`).

---

## 5. Reversing the `CCam::Process_*` functions one by one

### Shared building blocks (all reversed unless noted)

| Helper | Address | Use |
|---|---|---|
| `WellBufferMe(target, value, speed, topSpeed, step, isAngle)` | `0x509AE0` (file-static in `Cam.cpp`) | Spring-damped approach of a value to a target |
| `WrapAngle(float&)` | `0x509BE0` (file-static) | Wrap to [-pi, pi) |
| `CCam::ClipBeta()` | `0x509C50` (not hooked) | Wrap `m_fHorizontalAngle`. **Buggy, see Gotchas** |
| `CCam::ClipAlpha()` | inlined | Clamp `m_fVerticalAngle` to [-85.5, 60] degrees |
| `CCam::GetVectorsReadyForRW()` | `0x509CE0` | Normalise front, rebuild up. Almost every mode ends with it |
| `CCam::KeepTrackOfTheSpeed` | `0x509DF0` | Speed bookkeeping (called by `Process`, not by modes) |
| `CCam::RotCamIfInFrontCar`, `Using3rdPersonMouseCam`, `DoCamBump`, `GetWeaponFirstPersonOn` | | `GetWeaponFirstPersonOn` (`0x509DC0`) has C++ but no hook |
| `CCamera::CameraColDetAndReact(src, target)` | `0x520190` | **stub**. Pull the camera in front of obstacles |
| `CCamera::ImproveNearClip(veh, ped, src, target)` | `0x516B20` | **stub** |
| `CCamera::AvoidTheGeometry(...)` | `0x514030` | **missing**: declared in `Camera.h`, no definition |
| `CCamera::ConeCastCollisionResolve` | `0x51A5D0` | Sphere-cast collision (used by `Process_Fixed`) |
| `CCamera::SetColVarsPed/Vehicle/AimWeapon`, `SetCamCollisionVarDataSet` | `0x50CC50`/`0x50CCA0`/`0x50CBF0`/`0x50CB60` | Pick a `gCamColVars` row |
| `CCamera::Camera*ModeSpecialCases` | `0x50CD30`..`0x50CDE0` | Entities to ignore for collision |
| `CCamera::HandleCameraMotionForDucking` / `...DuringAim` | `0x50CFA0` / `0x50D090` | **stubs** |
| `CCamera::ResetDuckingSystem`, `ApplyVehicleCameraTweaks`, `GetArrPosForVehicleType`, `Find3rdPersonQuickAimPitch` | | reversed |
| `ApplyUnderwaterMotionBlur()` | inlined in `Cam.cpp` | |

### Dependencies of each stub (IDA callees, math/pad noise removed)

| Stub | Calls into (stubs in **bold**) |
|---|---|
| `Process_FollowPed_SA` | **`CameraColDetAndReact`**, **`ImproveNearClip`**, **`HandleCameraMotionForDucking`**, `SetColVarsPed`, `CameraGenericModeSpecialCases`, `ResetDuckingSystem`, `CIdleCam::Process`, `IKChainManager_c::LookAt`, `ClipBeta`, `FTrunc`/`VecTrunc`, `GetVectorsReadyForRW`, ped task getters (swim, jetpack, parachute, climb, use gun) |
| `Process_FollowCar_SA` | **`CameraColDetAndReact`**, **`ImproveNearClip`**, `ApplyVehicleCameraTweaks`, `GetArrPosForVehicleType`, `SetColVarsVehicle`, `CameraVehicleModeSpecialCases`, `ClipBeta`, `FTrunc`, `GetVectorsReadyForRW` |
| `Process_AimWeapon` | **`CameraColDetAndReact`**, **`ImproveNearClip`**, **`HandleCameraMotionForDuckingDuringAim`**, `SetColVarsAimWeapon`, `CameraGenericModeSpecialCases`, `CameraPedAimModeSpecialCases`, `Find3rdPersonQuickAimPitch`, `ResetDuckingSystem`, `CPed::GetBonePosition`, `ClipBeta` |
| `LookBehind` | **`CameraColDetAndReact`**, **`ImproveNearClip`**, **`HandleCameraMotionForDucking`**, `CameraVehicleModeSpecialCases` |
| `LookRight` | **`CameraColDetAndReact`**, `CameraVehicleModeSpecialCases`, `CPed::SetPedPositionInCar`, `GetBonePosition` |
| `Process_M16_1stPerson` | `WellBufferMe`, `ClipBeta`, `CPad::SniperZoomIn/Out`, `CPed::PositionAttachedPed` (**CPed stub**), `CAutomobile::SetHeliOrientation`, `GetBonePosition` |
| `Process_1rstPersonPedOnPC` | `ConvertPedNode2BoneTag`, RpHAnim matrix access, `GetBonePosition`, `ClipBeta`, sniper zoom pads |
| `Process_FollowPedWithMouse` | `CWorld::ProcessLineOfSight`, `CWorld::TestSphereAgainstWorld`, `ClipBeta`, `CMatrix::SetRotateZOnly` |
| `Process_WheelCam` | `CWorld::ProcessLineOfSight`, `CWaterLevel::GetWaterLevel`, `GetBonePosition`, vehicle col model |
| `Process_AttachedCam` | `CWaterLevel::GetWaterLevel`, matrix helpers only |
| `Process_SpecialFixedForSyphon` | **`AvoidTheGeometry`**, weapon info, `SetHeading` |
| `ProcessArrestCamOne` | **`AvoidTheGeometry`**, `GetLookFromLampPostPos`, `CMessages::AddBigMessage`, three unnamed helpers |
| `ProcessPedsDeadBaby` | **`AvoidTheGeometry`**, `CWorld::TestSphereAgainstWorld`, `CWaterLevel::GetWaterLevelNoWaves` |
| `Process_Cam_TwoPlayer` | **`Process_FollowCar_SA`**, **`Process_FollowPed_SA`**, `CHud::SetHelpMessage`, two unnamed helpers |
| `Process_Cam_TwoPlayer_InCarAndShooting` | **`AvoidTheGeometry`**, `RotCamIfInFrontCar`, `CTaskSimpleGangDriveBy::FireGun`, `CWeapon::FindNearestTargetEntityWithScreenCoors` (**stub**), `CamShakeNoPos` |
| `Process_Cam_TwoPlayer_Separate_Cars` | no calls (pure math) |
| `Process_DW_*` | `GetCoreDataForDWCineyCamMode`, `IsTimeToExitThisDWCineyCamMode`, `Finalise_DW_CineyCams`, `CHandShaker::Reset`, `CWorld::ProcessLineOfSight` / `FindObjectsInRange`, `IsLampPost` |
| `Process_FlyBy` | two unnamed spline helpers |

### How to work on one mode

Read the original with IDA, keep every `static` local as a `StaticRef` with its address (see `Process_Rocket` and
`Process_1stPerson`), name the `m_f*` fields as you understand them, and compare in game through F7 -> Hooks with
category `Camera/CCam`. Camera code is float-heavy x87: use the comparison cheat-sheet in
[../ReversingGuide.md](../ReversingGuide.md) §2 and expect tiny differences in difftests.

---

## 6. Gotchas

- **Stubs are `NOTSA_UNREACHABLE()`.** `CCam::Process`, `LookBehind`, `LookRight` and most `Process_*` stubs have
  bodies that abort. They only work because the hook is `.Reversed = false` and GTA's code runs. Never call them from
  our code until they are reversed; never flip such a hook to "ours".
- **(Fixed: now `{ .Reversed = false }`.)** **`CCamera::TakeControlAttachToEntity` recursed forever.** It was
  installed as reversed while its body is `plugin::CallMethod<0x50C910>` on its own address
  ([R9](../ReversingRules.md)). It is still a stub.
- **`CCam::ClipBeta` is wrong.** The original (`0x509C50`) does `if (beta > pi) beta -= 2pi; else if (beta < -pi)
  beta += 2pi;`. The repo version subtracts 2pi whenever `beta >= -pi`. It is used by the reversed `Process_Rocket`
  and the partial `Process_1rstPersonPedOnPC`.
- **DW cams return a bool.** The originals return 0/1 and take a "just testing" flag: `TryToStartNewCamMode` calls them
  with `true` and checks the result, `CCam::Process` calls them with `false`. `Cam.h` declares them `void`.
- `Process_WheelCam` is declared `bool` (correct). Check the return type of every `Process_*` in IDA.
- `m_fHorizontalAngle` is "beta", `m_fVerticalAngle` is "alpha" (plugin-sdk names). `m_fTrueBeta` / `m_fTrueAlpha` are
  recomputed after every mode from source and target.
- `gCameraDirection` uses 0 = behind, 1 = left, 2 = right, 3 = forward, while `eLookingDirection` in `Camera.h` is
  only half named. Don't trust the enum names.
- Only the active cam runs; `CCamera::Process` only re-targets the other cam (`(m_nActiveCam + 1) % 2`).
- `CCamera::GetActiveCamera()` (static) and `TheCamera.GetActiveCam()` are the same thing; a TODO in `Camera.h`
  prefers the latter.
- `ProcessObbeCinemaCamera{Plane,Train,Boat,Car,Heli}` are declared with `assert(0)` bodies and no address;
  `ProcessObbeCinemaCameraPed` (`0x50B880`) really is empty.
- `UpdateSoundDistances` (`0x515BD0`) is a stub next to `StartTransitionWhenNotFinishedInter` (`0x515BC0`). Don't
  confuse the two addresses.
- `CCamera` fields carry default member initialisers next to the reversed constructor (`0x51A450`). Check both when
  you change a field.

---

## 7. What is left

Snapshot from `idb_coverage.py --class CCam` / `--class CCamera` (status words in [README.md](README.md)):

| Class | Functions | Reversed | Stub | NOHOOK | Known | Missing | Bytes left |
|---|---|---|---|---|---|---|---|
| `CCam` | 46 | 18 | 25 | 0 | 2 | 1 | 65,770 |
| `CCamera` | 119 | 96 | 17 | 0 | 4 | 2 | 37,975 |
| `CIdleCam` | 15 | 13 | 1 | 0 | 1 | 0 | 682 |
| `CHeliCamSettings` | 3 | 0 | 0 | 0 | 0 | 3 | 424 |
| `CCamPathSplines` | 2 | 0 | 0 | 0 | 0 | 2 | 10 |

`CCam` is third in the whole game by unreversed bytes, after `CRunningScript` and `CCarCtrl`. Its stubs: `Process`,
`ProcessArrestCamOne`, `ProcessPedsDeadBaby`, `Process_1rstPersonPedOnPC`, `Process_AimWeapon`,
`Process_AttachedCam`, `Process_Cam_TwoPlayer`, `_InCarAndShooting`, `_Separate_Cars`, the seven `Process_DW_*`,
`Process_FlyBy`, `Process_FollowCar_SA`, `Process_FollowPedWithMouse`, `Process_FollowPed_SA`,
`Process_M16_1stPerson`, `Process_SpecialFixedForSyphon`, `Process_WheelCam`, `LookBehind`, `LookRight`. KNOWN:
`ClipBeta` and `GetWeaponFirstPersonOn`. Missing: the destructor.

`CCamera` stubs: `CamControl` (14,214 B), `TryToStartNewCamMode` (7,088 B), `Process` (5,716 B), `ImproveNearClip`,
`ProcessShake(float)` (1,394 B), `CameraColDetAndReact`, `CopyCameraMatrixToRWCam`, `CalculateFrustumPlanes`,
`TakeControlAttachToEntity`, `UpdateSoundDistances`, `LoadPathSplines`, `CalculateDerivedValues`, `IsSphereVisible`,
`HandleCameraMotionForDucking`, `HandleCameraMotionForDuckingDuringAim`, `SetNearClipBasedOnPedCollision`,
`ProcessShake()`. KNOWN: the destructor, the second `IsSphereVisible` overload, `AddShake`, `VectorMoveRunning`.
Missing: `AvoidTheGeometry` (2,084 B, declared only) and `VectorTrackRunning` (implemented in `Camera.cpp`, no
address match).

### Suggested order

1. **Leaf camera helpers first**: `CameraColDetAndReact` (1279 B), `ImproveNearClip` (1542 B),
   `HandleCameraMotionForDucking` (230 B), `HandleCameraMotionForDuckingDuringAim` (205 B), `AvoidTheGeometry`
   (2084 B, add the definition), `SetNearClipBasedOnPedCollision` (82 B). Also fix `ClipBeta`.
2. **Self-contained modes** to learn the style: `Process_Cam_TwoPlayer_Separate_Cars`, `Process_AttachedCam`,
   `Process_SpecialFixedForSyphon`, `Process_WheelCam`, `Process_FollowPedWithMouse`.
3. **DW cams**: `PlaneCam3` (729 B), `PlaneCam2`, `PlaneCam1`, `PlaneSpotterCam`, `CamManCam`, `BirdyCam`,
   `HeliChaseCam` (needs `CHeliCamSettings`). Fix their return type first (Gotchas).
4. **1st person**: `Process_1rstPersonPedOnPC` (partial C++ in `Cam.cpp`), `Process_M16_1stPerson`.
5. **Special cams**: `ProcessPedsDeadBaby`, `ProcessArrestCamOne`, `Process_FlyBy` (+ `LoadPathSplines`).
6. **Core 3rd person**: `LookRight`, `LookBehind`, `Process_FollowCar_SA`, `Process_FollowPed_SA`, `Process_AimWeapon`.
7. **Two-player**: `Process_Cam_TwoPlayer_InCarAndShooting`, `Process_Cam_TwoPlayer`.
8. **Dispatchers last**: `CCam::Process`, then `CopyCameraMatrixToRWCam`, `CalculateFrustumPlanes`,
   `CalculateDerivedValues`, `ProcessShake`, `TryToStartNewCamMode` (+ the Obbe helpers), `CamControl`,
   `CCamera::Process`.
