# Vehicles

How vehicles are structured, what runs every frame, and what is still left to reverse. General rules are in
[../ReversingRules.md](../ReversingRules.md) (R5 covers type vs sub-type) and [../ReversingGuide.md](../ReversingGuide.md).
Path following is in [PathFind.md](PathFind.md), collision response in [CollisionPhysics.md](CollisionPhysics.md).

Paths are relative to `source/game_sa/`. Addresses come from `RH_Scoped*Install`, `// 0xADDR` comments or
`StaticRef` in the repo.

---

## 1. Overview

```
CEntity (0x38)  vtable 22 slots                       Entity/Entity.h
└─ CPhysical (0x138)  vtable 23 slots                 Entity/Physical.h
   └─ CVehicle (0x5A0)  vtable 0x871E80, 66 slots     Entity/Vehicle/Vehicle.h
      ├─ CAutomobile (0x988)  0x871120, 71 slots      Automobile.h
      │  ├─ CHeli         (0xA18)  0x871680, 71       Heli.h
      │  ├─ CPlane        (0xA04)  0x871948, 71       Plane.h
      │  ├─ CQuadBike     (0x9BC)  0x871AE8, 71       QuadBike.h
      │  ├─ CMonsterTruck (0x99C)  0x8717D8, 71       MonsterTruck.h
      │  └─ CTrailer      (0x9F4)  0x871C28, 71       Trailer.h
      ├─ CBike (0x814)  0x871360, 67 slots            Bike.h
      │  └─ CBmx (0x838)  0x871528, 67                Bmx.h
      ├─ CBoat  (0x7E8)  0x8721A0, 66 slots           Boat.h
      └─ CTrain (0x6AC)  0x872370, 66 slots           Train.h
```

Sizes come from `VALIDATE_SIZE` in each header; vtable address and slot count from `RH_ScopedVirtualClass` in each
`InjectHooks()`. Every class is `NOTSA_EXPORT_VTABLE`.

### 1.1 Type versus sub-type (`IsXxx` against `IsSubXxx`)

`CVehicle` has two `eVehicleType` fields (`m_nVehicleType`, `m_nVehicleSubType`; enum in `Enums/eVehicleType.h`):

- `m_nVehicleType` is set by the **base** constructor that runs: `CAutomobile` sets `VEHICLE_TYPE_AUTOMOBILE`, `CBike`
  sets `BIKE`, `CBoat` sets `BOAT`, `CTrain` sets `TRAIN`.
- `m_nVehicleSubType` is set again by each leaf constructor: `CHeli` sets `HELI`, `CPlane` `PLANE`, `CQuadBike`
  `QUAD`, `CMonsterTruck` `MTRUCK`, `CTrailer` `TRAILER`, `CBmx` `BMX`.

So **a heli, plane, quad, monster truck or trailer has `IsAutomobile() == true`**, and a BMX has `IsBike() == true`.
`IsHeli()`, `IsPlane()`, `IsQuad()`, `IsMonsterTruck()`, `IsTrailer()` and `IsBMX()` on a live `CVehicle` are never
true. To identify the concrete class, use `IsSubHeli()`, `IsSubPlane()` and the other `IsSub*` helpers. Only
`CVehicleModelInfo::m_nVehicleType` holds the "real" type read from `vehicles.ide`, and
`CCarCtrl::GetNewVehicleDependingOnCarModel` switches on it.

The handling-based checks are something else again: `IsRealHeli()`, `IsRealPlane()`, `IsRealBike()` and
`IsRealBoat()` test the `m_bIsHeli`, `m_bIsPlane` and similar model flags in `tHandlingData`. For example, the Vortex
hovercraft is an automobile whose handling acts like a boat.

Other helpers: `GetVehicleAppearance()` (`eVehicleAppearance`), `IsSubRoadVehicle()`, `IsSubFakeAircraft()`
(FHELI/FPLANE are the RC aircraft), `IsMissionVehicle()`, `IsLawEnforcementVehicle()`. Casts are unchecked
`reinterpret_cast`s: `AsAutomobile()`, `AsBike()`, `AsBoat()` and `AsTrain()` in `Entity.h`, `AsPlane()` and
`AsHeli()` in `Vehicle.h`.

### 1.2 Creation

The factory is `CCarCtrl::GetNewVehicleDependingOnCarModel` (`0x421440`, **still a stub**). Its intended body is in
the comment right below the stub: a switch on `CVehicleModelInfo::m_nVehicleType` that runs
`new CMonsterTruck/CQuadBike/CHeli/...`. `CCarGenerator::DoInternalProcessing` constructs vehicles directly.
Vehicles live in the vehicle pool (`CVehicle::operator new`).

Handling pointers are set in the constructors: `CAutomobile::CAutomobile` sets `m_pHandlingData` and
`m_pFlyingHandlingData`, and copies the handling flags into `m_nHandlingFlagsIntValue`. `CBike`, `CBoat` and `CTrain`
fetch the same pointers through `gHandlingDataMgr.GetVehiclePointer(mi->m_nHandlingId)`.

---

## 2. Key classes and files

| Type | Size | File |
|---|---|---|
| `CAutoPilot` | 0x98 | `AutoPilot.h` (member `CVehicle::m_autoPilot`) |
| `CDamageManager` | 0x18 | `DamageManager.h` (member `CAutomobile::m_damageManager`) |
| `tHandlingData` | 0xE0 | `tHandlingData.h` |
| `tBikeHandlingData` / `tFlyingHandlingData` / `tBoatHandlingData` | 0x40 / 0x58 / 0x3C | `t*HandlingData.h` |
| `cHandlingDataMgr` | 0xC624 | `cHandlingDataMgr.h`, global `gHandlingDataMgr` at `0xC2B9C8` |
| `CVehicleModelInfo` | 0x308 (`CVehicleStructure` 0x314) | `Models/VehicleModelInfo.h` |
| `CAEVehicleAudioEntity` | | `Audio/Entities/AEVehicleAudioEntity.h` (member `CVehicle::m_vehicleAudio`, first field of `CVehicle`) |
| `CCarCtrl`, `CCarAI`, `CCarEnterExit` | static classes | `CarCtrl.*`, `CarAI.*`, `CarEnterExit.*` |

### 2.1 Virtual interface of `CVehicle`

Virtuals inherited from `CEntity`/`CPhysical` that vehicles override: `SetModelIndex`, `DeleteRwObject`,
`ProcessControl`, `Teleport`, `SpecialEntityPreCollisionStuff`, `SpecialEntityCalcCollisionSteps`, `PreRender`,
`Render`, `SetupLighting`, `RemoveLighting`, `FlagToDestroyWhenNextProcessed`, and `CPhysical::ProcessEntityCollision`.
`CVehicle` itself does **not** override `ProcessControl`; each leaf class does.

New virtuals, in **declaration order, which is vtable order**. The slot index is 0-based and follows from
`RH_ScopedVirtualClass(CPhysical, ..., 23)`. Check the address in [../ai/Vtables.md](../ai/Vtables.md) before relying
on a slot number.

| Slot | Function | Slot | Function |
|---|---|---|---|
| 23 | `ProcessControlCollisionCheck(bool)` | 45 | `IsRoomForPedToLeaveCar(uint32, CVector*)` |
| 24 | `ProcessControlInputs(uint8 playerNum)` | 46 | `ProcessDrivingAnims(CPed*, bool)` |
| 25 | `GetComponentWorldPosition(int32, CVector&)` | 47 | `GetRideAnimData()` |
| 26 | `IsComponentPresent(int32)` | 48 | `SetupSuspensionLines()` |
| 27 | `OpenDoor(CPed*, int32 comp, eDoors, float, bool)` | 49 | `AddMovingCollisionSpeed(CVector&)` |
| 28 | `ProcessOpenDoor(CPed*, uint32, uint32, uint32, float)` | 50 | `Fix()` |
| 29 | `GetDooorAngleOpenRatioU32(uint32)` | 51 | `SetupDamageAfterLoad()` |
| 30 | `GetDooorAngleOpenRatio(eDoors)` | 52 | `DoBurstAndSoftGroundRatios()` |
| 31 | `IsDoorReadyU32(uint32)` | 53 | `GetHeightAboveRoad()` |
| 32 | `IsDoorReady(eDoors)` | 54 | `PlayCarHorn()` |
| 33 | `IsDoorFullyOpenU32(uint32)` | 55 | `GetNumContactWheels()` (default 4) |
| 34 | `IsDoorFullyOpen(eDoors)` | 56 | `VehicleDamage(float, eVehicleCollisionComponent, CEntity*, CVector*, CVector*, eWeaponType)` |
| 35 | `IsDoorClosedU32(uint32)` | 57 | `CanPedStepOutCar(bool)` |
| 36 | `IsDoorClosed(eDoors)` | 58 | `CanPedJumpOutCar(CPed*)` |
| 37 | `IsDoorMissingU32(uint32)` | 59 | `GetTowHitchPos(CVector&, bool, CVehicle*)` |
| 38 | `IsDoorMissing(eDoors)` | 60 | `GetTowBarPos(CVector&, bool, CVehicle*)` |
| 39 | `IsOpenTopCar()` | 61 | `SetTowLink(CVehicle*, bool)` |
| 40 | `RemoveRefsToVehicle(CEntity*)` | 62 | `BreakTowLink()` |
| 41 | `BlowUpCar(CEntity*, bool)` | 63 | `FindWheelWidth(bool rear)` |
| 42 | `BlowUpCarCutSceneNoExtras(bool, bool, bool, bool)` | 64 | `Save()` |
| 43 | `SetUpWheelColModel(CColModel*)` | 65 | `Load()` |
| 44 | `BurstTyre(uint8, bool)` | | |

`CAutomobile` adds five virtuals: slot 66 `ProcessAI(uint32& extraHandlingFlags)`, 67 `ResetSuspension`, 68
`ProcessFlyingCarStuff`, 69 `DoHoverSuspensionRatios` and 70 `ProcessSuspension` (71 slots). `CBike` adds its **own**
`ProcessAI` at slot 66 (67 slots), unrelated to `CAutomobile`'s.

> The door functions come in pairs that take `uint32` and `eDoors` (the `...U32` names are NOTSA names for the
> overloads). The header warns about mixing them up. Component ids (`tComponent`, `eCarNodes`) are not the same as
> `eDoors`; see `CVehicle::ProcessOpenDoor` for the mapping.

### 2.2 Overrides per class (as declared in the headers)

| Class | Overrides |
|---|---|
| `CAutomobile` | Almost everything: ProcessControl, ProcessControlCollisionCheck, ProcessControlInputs, all door functions, BlowUpCar(+CutScene), SetUpWheelColModel, BurstTyre, IsRoomForPedToLeaveCar, SetupSuspensionLines, AddMovingCollisionSpeed, Fix, SetupDamageAfterLoad, DoBurstAndSoftGroundRatios, GetHeightAboveRoad, PlayCarHorn, GetNumContactWheels, VehicleDamage, tow functions, FindWheelWidth, Save/Load, ProcessEntityCollision, PreRender, Render, Teleport |
| `CHeli` | ProcessControl, ProcessControlInputs, ProcessFlyingCarStuff, PreRender, Render, BlowUpCar, Fix, BurstTyre, SetUpWheelColModel, SetupDamageAfterLoad |
| `CPlane` | Same as CHeli, plus OpenDoor and VehicleDamage |
| `CQuadBike` | ProcessControl, ProcessControlInputs, ProcessAI, ProcessSuspension, ResetSuspension, SetupSuspensionLines, ProcessDrivingAnims, GetRideAnimData, PreRender, Fix, SetupDamageAfterLoad |
| `CMonsterTruck` | ProcessControl, ProcessControlCollisionCheck, ProcessSuspension, ResetSuspension, SetupSuspensionLines, ProcessEntityCollision, PreRender, BurstTyre, SetUpWheelColModel |
| `CTrailer` | ProcessControl, ProcessAI, ProcessSuspension, ResetSuspension, SetupSuspensionLines, ProcessEntityCollision, PreRender, tow functions |
| `CBike` | ProcessControl, ProcessAI, ProcessControlInputs, ProcessControlCollisionCheck, ProcessEntityCollision, ProcessDrivingAnims, ProcessOpenDoor, PreRender, Render, Teleport, SetModelIndex, VehicleDamage, BlowUpCar, BurstTyre, Fix, SetupSuspensionLines, SetUpWheelColModel, DoBurstAndSoftGroundRatios, PlayCarHorn, SetupDamageAfterLoad, RemoveRefsToVehicle, GetComponentWorldPosition, IsComponentPresent, door functions, IsRoomForPedToLeaveCar, GetRideAnimData, GetHeightAboveRoad, GetNumContactWheels, FindWheelWidth |
| `CBmx` | ProcessControl, ProcessAI, ProcessDrivingAnims, PreRender, BlowUpCar, SetUpWheelColModel, BurstTyre, FindWheelWidth |
| `CBoat` | ProcessControl, ProcessControlInputs, PreRender, Render, Teleport, SetModelIndex, GetComponentWorldPosition, IsComponentPresent, BlowUpCar, ProcessOpenDoor |
| `CTrain` | **Only ProcessControl is declared.** The binary also has CTrain overrides of SetModelIndex `0x6F5530`, PreRender `0x6F5570`, GetDooorAngleOpenRatio `0x6F5930`/`0x6F59C0`, IsRoomForPedToLeaveCar `0x6F5EE0`, OpenDoor `0x6F6370` and Teleport `0x6F7190` (addresses from the IDB coverage tool, which reports them as MISSING). Declare them as `override` when you reverse them |

---

## 3. Frame flow

### 3.1 Game loop entry points (`CGame::Process`)

```
CTrain::UpdateTrains()            (empty in the original: "NOP")
CHeli::UpdateHelis()              (police/news heli spawning, address known but not hooked)
CTheCarGenerators::Process()      (parked cars)
CWorld::Process()                 ──> per-entity ProcessControl / ProcessCollision / ProcessShift
CCarCtrl::PruneVehiclesOfInterest
CCarCtrl::GenerateRandomCars()    ──> GenerateOneRandomCar() (0x430050, 7.9 KB, stub)
CCarCtrl::RemoveDistantCars()
CCarCtrl::RemoveCarsIfThePoolGetsFull()
```

`CWorld::Process` walks the moving list: first `entity->ProcessControl()` on every entity, then `ProcessCollision()`
on every entity, then `ProcessShift()` (see [WorldStreamingPools.md](WorldStreamingPools.md) §3.5). Rendering is
separate: `CRenderer::PreRender` calls `entity->PreRender()` on the visible lists and then
`CHeli::SpecialHeliPreRender()`. `CRenderer::RenderOneNonRoad` calls `vehicle->SetupRender()`, then `Render()`, then
`ResetAfterRender()`.

### 3.2 `CAutomobile::ProcessControl` (`0x6B1880`, reversed)

The reference implementation. `CHeli`, `CPlane`, `CQuadBike` and `CTrailer` all call it as their base. The sequence:

1. Audio: `m_vehicleAudio.Service()`.
2. Crashing aircraft (`MISSION_PLANE/HELI_CRASH_AND_BURN`), nitro cheats, bus doors (`ProcessAutoBusDoors`), then
   `ProcessCarAlarm`, `UpdateClumpAlpha` and `UpdatePassengerList`.
3. **`ProcessAI(extraHandlingFlags)`** (`0x6B4800`). It switches on `GetStatus()`:
   - `STATUS_SIMPLE`: `CCarAI::UpdateCarAI` and `CCarCtrl::UpdateCarOnRails`. The car runs "on rails" without
     physics and returns `true`, which means skip physics.
   - `STATUS_PHYSICS` / `STATUS_GHOST`: `CCarAI::UpdateCarAI`, `CCarCtrl::SteerAICarWithPhysics` and
     `CCarCtrl::ReconsiderRoute`.
   - `STATUS_ABANDONED`, `STATUS_WRECKED`, `STATUS_REMOTE_CONTROLLED`, `STATUS_FORCED_STOP`, `STATUS_IS_TOWED`: each has
     a special case.
   - `STATUS_PLAYER`: `PruneReferences`, then **`ProcessControlInputs(padNum)`**, which reads the pad and sets the
     gas, brake and steer values.
4. Settle/idle logic for abandoned and wrecked cars. This logic can decide to skip physics.
5. `VehicleDamage(0, DEFAULT, ...)` applies the collision damage accumulated during the last collision pass.
6. Model specials: fire truck water cannon, tank turret, packer/dozer/forklift/towtruck "misc component" arms, and
   `NitrousControl`.
7. If physics is skipped, `SkipPhysics()` runs. Otherwise:
   - `CPhysical::ProcessControl()`, plus Skimmer boat handling and water entry.
   - **`ProcessSuspension()`** (`0x6AFB10`). It computes spring and damping forces per wheel from
     `m_fWheelsSuspensionCompression[]` and `m_wheelColPoint[]`.
   - Gear and engine logic, perfect-handling cheat, then **`ProcessCarWheelPair`** for the front and rear pairs. The
     order depends on `handlingFlags.bProcRearwheelFirst`. `ProcessCarWheelPair` calls `CVehicle::ProcessWheel` for
     each wheel to apply traction, brake and skid.
8. `ProcessCarOnFireAndExplode` and the siren (`CCarAI::MakeWayForCarWithSiren`).
9. Per-wheel bookkeeping. The current compression is copied to `...Prev` and **reset to 1.0**, so the next collision
   pass can fill it again. Wheel spin goes to the audio.
10. `CCarCtrl::ScanForPedDanger`, hydraulics, tow links (`m_pTowingVehicle` / `m_pVehicleBeingTowed`), earthquake,
    harvester and Rhino.

### 3.3 Wheels, suspension and colpoints

- Suspension rays are **collision lines** in the vehicle's col model. `CAutomobile::SetupSuspensionLines` builds them
  from `tHandlingData::m_fSuspensionUpperLimit/LowerLimit` and the wheel size (`m_aSuspensionSpringLength`,
  `m_aSuspensionLineLength`).
- `CAutomobile::ProcessEntityCollision` runs during `CWorld`'s collision pass, **after** ProcessControl.
  `CCollision::ProcessColModels` returns a touch distance for each line, in the range [0,1]. When a line hits closer
  than the stored value, the code writes `m_fWheelsSuspensionCompression[i]` and `m_wheelColPoint[i]`. In that array,
  1.0 means fully extended and no contact.
- The next frame's `ProcessSuspension` and `ProcessCarWheelPair` read those values. Helpers in `Automobile.h` decode
  them (`GetAllWheelsOffGround`, sand, rail-track and shallow-water checks on `m_wheelColPoint[i].m_nSurfaceTypeB`).
- Bikes use `CVehicle::ProcessBikeWheel` (`0x6D73B0`, stub). Monster trucks, trailers and quads override
  ProcessSuspension and SetupSuspensionLines.

### 3.4 Other classes

| Class | ProcessControl | Notes |
|---|---|---|
| `CBike` | `0x6B9250`, **stub, 14 KB** | Rider anims, lean (`CalculateLeanMatrix` is reversed), and its own ProcessAI (`0x6BC930`, stub) |
| `CBmx` | Reversed. Calls `CBike::ProcessControl` | PreRender and ProcessAI are stubs |
| `CBoat` | Reversed | Calls `CVehicle::ProcessBoatControl` and `m_vehicleAudio.Service()` |
| `CTrain` | Reversed (`0x6F86A0`) | Track following: `FindPositionOnTrackFromCoors` and the track-file loaders are stubs |
| `CHeli` | `0x6C7050`, **NOHOOK** | ProcessControlInputs, ProcessFlyingCarStuff, PreRender and BlowUpCar are also NOHOOK |
| `CPlane` | `0x6C9260`, **stub** | Has untested reversed code after the `return plugin::CallMethod` |
| `CQuadBike` / `CTrailer` | Quad is a stub (`0x6CDCC0`). Trailer is reversed | Both call `CAutomobile::ProcessControl` |
| `CMonsterTruck` | Stub (`0x6C8250`) | ProcessSuspension (`0x6C83A0`) is also a stub |

Flight physics live in `CVehicle::FlyingControl(eFlightModel, ...)`, shared by planes, helis and the RC aircraft.
Rotor blade collision is in `CVehicle::BladeColSectorList`.

### 3.5 PreRender and Render

- `CVehicle::PreRender` handles collision lighting, `PreRenderDriverAndPassengers`, 2dfx lights and the env-map
  coefficient. `CVehicle::Render` sets the dirt textures and then calls `CEntity::Render`.
- `CAutomobile::PreRender` (`0x6AAB50`, **8.5 KB, NOHOOK**) and `CAutomobile::Render` (`0x6A2B10`, NOHOOK) are still
  `plugin::CallMethod` stubs. They handle wheel matrices (`UpdateWheelMatrix`, KNOWN), lights through
  `CVehicle::DoVehicleLights`, exhaust and nitro effects, and dust. Every automobile-derived class depends on them.
- Lights and headlight beams: `CVehicle::DoHeadLightBeam` and `DoVehicleLights`.

### 3.6 Audio hookup

`CVehicle::m_vehicleAudio` is a `CAEVehicleAudioEntity`. The constructors call `m_vehicleAudio.Initialise(this)` and
the destructors call `Terminate()`. `Initialise` reads per-model settings with `GetVehicleAudioSettings`. Each frame,
`Service()` runs from the vehicle's ProcessControl and calls `ProcessVehicle`. One-off sounds use
`m_vehicleAudio.AddAudioEvent(AE_..., volume)` (tyre bursts, doors, hydraulics). Only a handful of audio functions are
still stubs, for example `ProcessGenericJet` and `ProcessPlayerBicycle`. See [Audio.md](Audio.md).

---

## 4. Handling

- `HANDLING.CFG` is parsed by `cHandlingDataMgr::LoadHandlingData` into fixed arrays: `tHandlingData[210]`,
  `tBikeHandlingData[13]`, `tFlyingHandlingData[24]` and `tBoatHandlingData[12]`. The index is
  `CVehicleModelInfo::m_nHandlingId`. Use the accessors `GetVehiclePointer`, `GetBikeHandlingPointer`,
  `GetFlyingPointer` and `GetBoatPointer`. `ConvertDataToGameUnits` converts the values after loading.
- `tHandlingData` holds mass, drag, centre of mass, buoyancy, traction, the `cTransmission` (gears, drive type
  `'F'/'R'/'4'`, engine), brakes, steering lock, the suspension block, damage multiplier and two flag words:
  - `m_nModelFlags` (`eVehicleHandlingModelFlags`): IS_VAN, IS_BUS, NO_DOORS, the axle types, and
    `m_bIsBike/IsHeli/IsPlane/IsBoat`, which `IsReal*()` reads.
  - `m_nHandlingFlags` (`eVehicleHandlingFlags`): 1G/2G boost, NPC anti-roll, wheel widths, HYDRAULIC_INST, NOS_INST,
    offroad, PROC_REARWHEEL_FIRST and similar.
- **The handling data is shared by every vehicle of the model.** The vehicle stores a pointer, `m_pHandlingData`.
  Per-vehicle state lives in `CVehicle::m_nHandlingFlagsIntValue` / `handlingFlags` (a union), a copy that the
  constructor takes and that upgrades toggle (for example `bNosInst` and `bHydraulicInst`).
- `ProcessAI` returns per-frame extra flags (`eExtraHandlingFlags`, `Automobile.h`): PERFECT, NITROS,
  WHEELS_TOUCHING_SAND and TAXI_BOOST.

---

## 5. Damage

- **Health**: `CVehicle::m_fHealth` starts at 1000. At or below 0 the vehicle burns and explodes
  (`CAutomobile::ProcessCarOnFireAndExplode`, then `BlowUpCar` → `BlowUpCar_Impl`).
- **Collision damage**: `CPhysical`'s collision code stores `m_fDamageIntensity` and `m_pDamageEntity`. The next
  ProcessControl calls `VehicleDamage(0, ...)`. `CAutomobile::VehicleDamage` (`0x6A7650`) checks
  `CVehicle::CanVehicleBeDamaged`, emits `CEventVehicleDamageCollision`, subtracts health, and calls
  `m_damageManager.ApplyDamage(...)`.
- **`CDamageManager`** (fully reversed) packs the state of the wheels (`eCarWheelStatus`), doors (`eDoorStatus`),
  lights (2 bits each), panels (4 bits each) and engine (`m_nEngineStatus`, 0-250).
  `ApplyDamage(vehicle, tComponent, intensity, colDmgMult)` maps the component to a group, scales the intensity, and
  only acts above 150. It then progresses the panel, wheel or door damage and calls back into
  `CAutomobile::SetBumperDamage/SetPanelDamage/SetDoorDamage` to swap in the damaged atomics or spawn flying
  components (`SpawnFlyingComponent`).
- **Weapons and explosions**: `CVehicle::InflictDamage` (`0x6D7C90`) has C++ but its hook is commented out (KNOWN).
  Bikes have `CBike::VehicleDamage` (stub) and `DamageKnockOffRider` (reversed). Planes have `CPlane::VehicleDamage`
  (stub) and aeroplane component damage (`ProgressAeroplaneDamage`).

---

## 6. AI driving

### 6.1 The pieces

| Piece | Role |
|---|---|
| `CAutoPilot` (`AutoPilot.h`) | Per-vehicle AI state: current/next path nodes and links, lanes, `m_nCarMission` (`eCarMission`), `m_nCarDrivingStyle` (`eCarDrivingStyle`), `m_nTempAction` (`eAutoPilotTempAction`, for reversing, handbrake turns and swerving), cruise speed, target entity, destination, and an 8-entry pathfind node cache. The constructor (`0x6D5E20`) has C++ but **no hook**. `ModifySpeed` (`0x41B980`) and `RemoveOnePathNode` (`0x41B950`) are NOHOOK stubs |
| `CCarAI` (`CarAI.cpp`) | **Fully reversed.** The high-level mission state machine. `UpdateCarAI` switches on status and mission, and moves between the `_FARAWAY` and `_CLOSE` variants (for example `MISSION_RAMPLAYER_FARAWAY` becomes `_CLOSE` once the car is near). Also police mission selection by wanted level, `GetCarToGoToCoors*`, `TellCarTo*` and `MakeWayForCarWithSiren` |
| `CCarCtrl` (`CarCtrl.cpp`) | The low-level layer: traffic spawning and removal, path-node picking, steering, collision avoidance, and boat, heli and plane steering. **Mostly unreversed** (see §10) |

### 6.2 Missions

`eCarMission` (`Enums/eCarMission.h`) has about 69 values. The groups are:

- Cruising and stopping: `MISSION_NONE`, `CRUISE`, `WAITFORDELETION`, `STOP_FOREVER`, `EMERGENCYVEHICLE_STOP`.
- Going to coordinates: `GOTOCOORDINATES`, plus the `_STRAIGHTLINE`, `_ACCURATE`, `_ASTHECROWSWIMS` and `_RACING`
  variants, and `FOLLOW_RECORDED_PATH`.
- Chasing the player or a car: `RAM/BLOCK/APPROACH PLAYER_FARAWAY/_CLOSE`, `RAMCAR/BLOCKCAR/FOLLOWCAR_*`,
  `KILLPED_*`, `DO_DRIVEBY_*`.
- Formation: `ESCORT_*`, `PROTECTION_*`.
- Parking: `PARK_PERPENDICULAR/PARALLEL(_2)`.
- Air: `HELI_*` (fly to coordinates or in a direction, follow, police, news, land, keep entity in view, crash and
  burn), `PLANE_*` (fly to coordinates, attack the player, dog fight, crash and burn).
- Boats: `BOAT_ATTACKPLAYER`, `BOAT_CIRCLEPLAYER`.

### 6.3 Call chain for an AI car with physics

```
CAutomobile::ProcessAI (STATUS_PHYSICS)
 ├─ CCarAI::UpdateCarAI                        mission transitions (reversed)
 ├─ CCarCtrl::SteerAICarWithPhysics            0x437C20 stub
 │   └─ SteerAICarWithPhysics_OnlyMission      0x436A90 stub, 4.2 KB: big switch on m_nCarMission
 │       ├─ SteerAICarWithPhysicsFollowPath(_Racing), ...HeadingForTarget, ...TryingToBlockTarget(_Stop)
 │       ├─ SteerAICarParkParallel/Perpendicular, SteerAICarTowardsPointInEscort, ...FollowPreRecordedPath
 │       ├─ SteerAIBoatWithPhysics*  /  SteerAIHeli* / FlyAIHeli*  /  SteerAIPlane* / FlyAIPlaneInCertainDirection
 │       └─ FindMaximumSpeedForThisCarInTraffic → SlowCarDownFor{Cars,Peds,Objects}SectorList, WeaveThrough*
 └─ CCarCtrl::ReconsiderRoute                  0x42FC40 stub
STATUS_SIMPLE: CCarCtrl::UpdateCarOnRails      0x436540 stub
                └─ SlowCarOnRailsDownForTrafficAndLights (reversed)
Route choice: PickNextNodeAccordingStrategy (0x432B10) → PickNextNodeRandomly (3.5 KB) / ToFollowPath / ToChaseCar
Physics LOD: SwitchVehicleToRealPhysics, SwitchBetweenPhysicsAndGhost, JoinCarWithRoadSystem(GotoCoors)
```

The script-facing helpers `CCarCtrl::JoinCarWithRoadAccordingToMission` and `CreateCarForScript` are reversed.

---

## 7. Enter and exit

- Peds use the task system: `CTaskComplexEnterCar` (with the AsDriver/AsPassenger and Timed variants),
  `CTaskSimpleCarOpenDoorFromOutside`, `CTaskSimpleCarGetIn`, `CTaskSimpleCarAlign`, `CTaskSimpleCarCloseDoor*`,
  `CTaskComplexLeaveCar*` and `CTaskSimpleCarDrive`, all in `Tasks/TaskTypes/` (see [Tasks.md](Tasks.md)).
- `CCarEnterExit` holds the shared geometry and rules: `GetNearestCarDoor`, `ComputeTargetDoorToExit`,
  `IsCarDoorReady` / `InUse`, `IsCarQuickJackPossible`, `IsCarSlowJackRequired`, `SetPedInCarDirect`,
  `IsVehicleStealable`. **Still stubs**: `GetNearestCarPassengerDoor` (`0x650BB0`), `GetPositionToOpenCarDoor`
  (`0x64E740`), `IsPlayerToQuitCarEnter` (`0x64F240`), `IsRoomForPedToLeaveCar` (`0x6504C0`) and
  `MakeUndraggedPassengerPedsLeaveCar` (`0x64F540`).
- On the vehicle side: `CVehicle::SetDriver`, `AddPassenger` (two overloads), `RemoveDriver` and `RemovePassenger`, and
  the virtual door functions `OpenDoor`, `ProcessOpenDoor`, `IsDoor*` and `CanPedStepOutCar/JumpOutCar`. Door ids:
  `eDoors` (`DamageManager.h`). Bikes have no real doors, so `CBike::IsDoor*` are inline header overrides that return
  constants.

---

## 8. Model info, components and dummies

- `CVehicleModelInfo` holds `m_nVehicleType`, `m_nHandlingId`, `m_nVehicleClass`, the wheel model and sizes,
  colours, the upgrade list (`m_anUpgrades`) and `m_pVehicleStruct` (`CVehicleStructure`). That struct holds
  `m_avDummyPos[eVehicleDummy]` (lights, seats, exhaust, engine, gas cap, trailer attach, vehicle gun and others) and
  `m_aUpgrades[eVehicleUpgradePosn]`. Use `GetModelDummyPosition(dummy)`.
- `SetClump` and `PreprocessHierarchy` find the frames by name through `ms_vehicleDescs[eVehicleType]` (`0x8A7740`).
  Extras are chosen by `ChooseComponent` and `ms_compsToUse`. The loaders are `SetupCommonData`,
  `LoadVehicleColours` and `LoadVehicleUpgrades`.
- At runtime each class keeps an `RwFrame*` table indexed by its node enum, filled by `SetupModelNodes()`: CAutomobile
  uses `m_aCarNodes[eCarNodes]` (`Enums/eCarNodes.h`), CBike uses `m_aBikeNodes[eBikeNodes]`, and CBoat and CTrain
  have their own tables. `GetComponentWorldPosition` and `IsComponentPresent` index these tables.

---

## 9. Gotchas

1. **`CCarCtrl` is mostly hooked stubs.** 83 `.Reversed = false` stubs, 20 reversed, and 6 NOHOOK
   (`SlowCarDownFor{Peds,Cars,Objects}SectorList`, `WeaveThrough{Cars,Peds,Objects}SectorList`, template
   sector-list workers). A NOHOOK function is never reached by the original code ([R8](../ReversingRules.md)). CHeli
   (ProcessControl, PreRender, ProcessControlInputs, ProcessFlyingCarStuff, BlowUpCar) and
   `CAutomobile::PreRender/Render` are NOHOOK too.
2. **Type versus sub-type** (§1.1, [R5](../ReversingRules.md)). Two places look suspicious:
   `CTaskSimpleCarJumpOut::ProcessPed` (`m_veh->IsPlane()`; the original checks the sub-type at `0x64DE90`) and
   `CPhysical::ProcessCollision` (`vehicle->IsTrailer()`, unverified). Check them against the original before you
   copy the pattern.
3. **`IsSubFlyingVehicle()` is always false.** It is written as `IsSubHeli() && IsSubPlane()` instead of `||`
   (`CGangWars::Update` checks heli OR plane at `0x446DB3`). Used in `CGangWars::Update`.
4. **Some reversed code is not hooked**, so the coverage tool reports it as KNOWN. Examples are the `CHeli` and
   `CAutoPilot` constructors and CHeli's `GenerateHeli`, `SendDownSwat` and `UpdateHelis`. `Vehicle.cpp` also has 16
   raw `((void(__thiscall*)(...))0xADDR)` calls and about 20 commented-out `// RH_Scoped...` lines in
   `CVehicle::InjectHooks` (`InflictDamage`, `ProcessWeapons`, `ProcessSirenAndHorn`, `UpdateTrailerLink`,
   `DoSunGlare`, `AddVehicleUpgrade`...). Some of these bodies only forward. Do not assume a body is a real
   reimplementation.
5. **Stubs with dead code after them.** `CPlane::ProcessControl` returns through `plugin::CallMethod` and has
   "untested" reversed code below the return. Treat that code as a draft.
6. **Handling data is shared.** Writing through `m_pHandlingData` changes every car of that model. Copy the value
   first, or change the per-vehicle `handlingFlags`.
7. **Missing CTrain overrides** (§2.2). CTrain objects carry the original vtable (the constructor `0x6F6030` is a
   stub), so at runtime the original overrides still run. Our headers simply do not declare them.
8. **Order of ProcessControl and ProcessCollision.** Wheel colpoints read in ProcessControl come from the *previous*
   collision pass. ProcessControl resets the compression to 1.0 at the end (§3.2 step 9).
9. **Coverage tool caveats.** Template-hooked functions (for example
   `RH_ScopedInstall(BladeColSectorList<CPtrListSingleLink<CEntity*>>, 0x6DAF00)`), inline header overrides and the
   30-byte "deleting destructor" thunks (`~CBike` at `0x6B8950` and similar) show as MISSING. Ignore those.

---

## 10. What is left

Snapshot from `idb_coverage.py --class <C>` (status words in [README.md](README.md)):

| Class | Funcs | Reversed | Stub | NOHOOK | Known | Missing | Bytes left |
|---|---|---|---|---|---|---|---|
| `CCarCtrl` | 110 | 20 | 83 | 6 | 0 | 1 | 84,878 |
| `CBike` | 56 | 22 | 18 | 0 | 1 | 15 | 34,010 |
| `CVehicle` | 203 | 137 | 3 | 0 | 17 | 46 | 19,976 |
| `CPlane` | 25 | 14 | 9 | 0 | 1 | 1 | 17,765 |
| `CAutomobile` | 117 | 99 | 0 | 2 | 11 | 5 | 16,020 |
| `CHeli` | 27 | 11 | 0 | 5 | 10 | 1 | 13,686 |
| `CTrain` | 45 | 17 | 19 | 0 | 0 | 9 | 10,852 |
| `CBmx` | 14 | 7 | 3 | 0 | 3 | 1 | 8,440 |
| `CCarEnterExit` | 32 | 27 | 5 | 0 | 0 | 0 | 4,789 |
| `CMonsterTruck` | 13 | 5 | 6 | 0 | 0 | 2 | 3,041 |
| `CAutoPilot` | 3 | 0 | 0 | 2 | 1 | 0 | 1,024 |
| `CQuadBike` | 12 | 9 | 1 | 0 | 0 | 2 | 964 |
| `CBoat` | 23 | 22 | 0 | 0 | 0 | 1 (dtor thunk) | 30 |
| `CCarAI`, `CDamageManager` | 24, 27 | all | 0 | 0 | 0 | 0 | 0 |
| `CTrailer`, `cHandlingDataMgr`, `CVehicleModelInfo` | 16, 11, 76 | 13, 9, 67 | 0 | 0 | 0, 1, 0 | 3, 1, 9 | small |

### What is left, by class

- **CCarCtrl (85 KB).** Nearly the whole driving AI. The biggest functions are `GenerateOneRandomCar` (7.9 KB),
  `SteerAICarWithPhysics_OnlyMission` (4.2 KB), `SteerAICarWithPhysicsFollowPath` (3.7 KB), `PickNextNodeRandomly`
  (3.6 KB), `SteerAICarWithPhysicsFollowPath_Racing` (3.3 KB), `FlyAIHeliInCertainDirection` (3.0 KB),
  `GenerateCarCreationCoors2` (2.8 KB) and `PickNextNodeToFollowPath` / `ToChaseCar` (2.1-2.2 KB). The factory
  `GetNewVehicleDependingOnCarModel` is a stub with its body already drafted in a comment.
- **CBike (34 KB).** `ProcessControl` (14 KB), `ProcessRiderAnims` (3.3 KB), `PreRender` (3.5 KB), `FixHandsToBars`
  (2.5 KB), `ProcessControlInputs` (1.8 KB), `ProcessAI` (1.6 KB), `SetupSuspensionLines`, `DoBurstAndSoftGroundRatios`,
  `DoSoftGroundResistance`, `VehicleDamage`, `BurstTyre`, `PlaceOnRoadProperly`, `GetCorrectedWorldDoorPosition`. It
  also needs `CVehicle::ProcessBikeWheel` (2 KB, stub).
- **CVehicle (20 KB).** Stubs `ProcessBikeWheel`, `FireFixedMachineGuns`, `DoDriveByShootings`; KNOWN
  `AddSingleWheelParticles` (2.9 KB), `DoSunGlare` (2.2 KB), `InflictDamage` (1.6 KB), `UpdateTrailerLink` /
  `UpdateTractorLink`, the weapon helpers (`ProcessWeapons`, `FireHeatSeakingMissile`, `GetPlaneWeaponFiringStatus`),
  the upgrade helpers and `ProcessSirenAndHorn`.
- **CPlane (18 KB).** `PreRender` (5.7 KB), `ProcessFlyingCarStuff` (3.3 KB), `ProcessControlInputs` (2.5 KB),
  `DoPlaneGenerationAndRemoval` (2.3 KB), `VehicleDamage`, `BlowUpCar`, `FindPlaneCreationCoors`, `ProcessControl`.
- **CAutomobile (16 KB).** `PreRender` (8.5 KB) and `Render` (1 KB), both NOHOOK, plus KNOWN functions:
  `UpdateWheelMatrix` (2.2 KB), `DoHeliDustEffect`, `SetRandomDamage`, `DoNitroEffect`,
  `HasCarStoppedBecauseOfLight`, `SetUpWheelColModel` and `BurstTyre`.
- **CHeli (14 KB).** `ProcessControl` (2.4 KB), `ProcessControlInputs`, `ProcessFlyingCarStuff`, `PreRender` and
  `BlowUpCar` (all NOHOOK). `SearchLightCone` (3.1 KB), `GenerateHeli`, `SendDownSwat` and `UpdateHelis` are KNOWN
  (check whether the existing body is complete, then hook it).
- **CTrain (11 KB).** Track and station logic (`FindPositionOnTrackFromCoors`, `ReadAndInterpretTrackFile`,
  `DoTrainGenerationAndRemoval` at 2.2 KB, `CreateMissionTrain`, `AddNearbyPedAsRandomPassenger`, `InitTrains`, the
  `Find*` helpers), the constructor, and the undeclared virtuals listed in §2.2.

### Suggested order

1. **Hook what already exists.** Add the RH installs for KNOWN bodies (CHeli constructor, `GenerateHeli`,
   `SendDownSwat`, `UpdateHelis`, `CAutoPilot` constructor, the commented-out CVehicle hooks) after you check each one
   against IDA. This is cheap, and it makes the existing code testable in the F7 menu. Register the CHeli,
   CAutomobile and CAutoPilot NOHOOKs as `.Reversed = false` (`add_missing_hooks.py`).
2. **The CCarCtrl leaves.** Pick small, self-contained stubs first: `FindSpeedMultiplier`,
   `FindPercDependingOnDistToLink`, `ThisVehicleShouldTryNotToTurn`, `StopCarIfNodesAreInvalid`,
   `SwitchVehicleToRealPhysics`, `UpdateCarCount`, `ClearInterestingVehicleList`, `RegisterVehicleOfInterest`,
   `CAutoPilot::ModifySpeed` / `RemoveOnePathNode`, `GetNewVehicleDependingOnCarModel`. Then the
   `SlowCarDownFor*` / `WeaveThrough*` sector lists, the `PickNextNode*` functions, the `SteerAICarWithPhysics*`
   family, and finally `SteerAICarWithPhysics`, `_OnlyMission` and `GenerateOneRandomCar`.
3. **CBike, bottom-up.** `SetupSuspensionLines`, `DoBurstAndSoftGroundRatios`, `DoSoftGroundResistance`,
   `CVehicle::ProcessBikeWheel`, `ProcessControlInputs`, `ProcessAI`, then `ProcessControl`, and last
   `ProcessRiderAnims`, `FixHandsToBars` and `PreRender`. After CBike, the CBmx stubs follow.
4. **Air vehicles.** `CVehicle::FlyingControl` is already reversed, so the CHeli and CPlane
   `ProcessControlInputs` / `ProcessFlyingCarStuff` / `ProcessControl` functions are tractable. Do PreRender later.
   The CCarCtrl heli and plane steering functions pair naturally with this step.
5. **CTrain track logic,** then **`CAutomobile::PreRender`** (large, mostly visual, so A/B it in game).

Steering and physics code is float-heavy x87; validate each function as described in
[../ReversingGuide.md](../ReversingGuide.md) §5.
