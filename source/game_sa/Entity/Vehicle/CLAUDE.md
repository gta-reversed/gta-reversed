# Entity/Vehicle/ — CVehicle and its subclasses

Guide: [../../../../docs/subsystems/Vehicles.md](../../../../docs/subsystems/Vehicles.md) (vtable layout, per-frame
flow, handling, damage, AI driving, gotchas). AI driving code is outside this folder: `CarCtrl.*`, `CarAI.*`,
`CarEnterExit.*`, `AutoPilot.h` in `source/game_sa/`.

## Layout
Vtable address and virtual count as in each `RH_ScopedVirtualClass`; size from `VALIDATE_SIZE`:
```
CVehicle       0x871E80 66  0x5A0  Vehicle.h/.cpp (huge; read only the part you need)
├─ CAutomobile 0x871120 71  0x988  ── CHeli 0x871680, CPlane 0x871948, CQuadBike 0x871AE8,
│                                     CMonsterTruck 0x8717D8, CTrailer 0x871C28 (all 71)
├─ CBike       0x871360 67  0x814  ── CBmx 0x871528 (67)
├─ CBoat       0x8721A0 66  0x7E8
└─ CTrain      0x872370 66  0x6AC
```
- All classes are `NOTSA_EXPORT_VTABLE`. Vehicle audio is `CVehicle::m_vehicleAudio` (`CAEVehicleAudioEntity`,
  `../../Audio/Entities/`).

## Conventions (type vs sub-type, the most common bug here)
- `m_nVehicleType` is set by the base constructor (AUTOMOBILE / BIKE / BOAT / TRAIN); `m_nVehicleSubType` by the leaf.
- So a heli/plane/quad/monster truck/trailer has `IsAutomobile() == true`, a BMX has `IsBike() == true`, and
  `IsPlane()`, `IsHeli()`, `IsQuad()`, `IsTrailer()`, `IsBMX()` ... are never true on a live vehicle.
- Use `IsSubPlane()`, `IsSubHeli()`, `IsSubTrailer()`, ... for the concrete class. Check which field the original
  reads (offset of `m_nVehicleType` vs `m_nVehicleSubType`) and pick the matching helper.
- `IsRealHeli/Plane/Bike/Boat()` test `tHandlingData` flags: a third, different notion (e.g. the Vortex).
- `AsAutomobile()` / `AsBike()` / `AsPlane()` / `AsHeli()` are unchecked casts.

## Pitfalls
- `m_pHandlingData` is shared per model: never write through it for a single vehicle.
- `IsSubFlyingVehicle()` is `IsSubHeli() && IsSubPlane()`, so it is always false (used in `GangWars.cpp`). Check the
  original before relying on it.
- Some bodies only forward (raw `((void(__thiscall*)(...))0xADDR)` casts in `Vehicle.cpp`) or have commented-out
  `// RH_Scoped...` lines. `CPlane::ProcessControl` returns `plugin::CallMethod` first (hook `.Reversed = false`),
  with an untested draft below. Don't treat these as finished.
- `CTrain`'s constructor is still a stub calling the original, so trains carry GTA's vtable: overrides missing from
  `Train.h` still run as original code.
- Wheel colpoints read in `ProcessControl` come from the previous collision pass.
- Template-hooked functions (e.g. `BladeColSectorList<...>`) and deleting-destructor thunks show as MISSING in the
  coverage tool; ignore those.
