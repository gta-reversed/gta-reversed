# Entity/Ped/ — CPed and its subclasses

Guide: [../../../../docs/subsystems/Peds.md](../../../../docs/subsystems/Peds.md) (layout, flags, per-frame flow,
helpers, gotchas). Ped behaviour lives mostly in tasks/events: [Tasks.md](../../../../docs/subsystems/Tasks.md),
[Events.md](../../../../docs/subsystems/Events.md).

## Layout
- Classes (vtable, all `RH_ScopedVirtualClass(..., 26)`): `CPed` `0x86C358` (`Ped.h/.cpp`, 0x79C), `CPlayerPed`
  `0x86D168` (0x7A4), `CCopPed` `0x86C120` (0x7C4), `CCivilianPed` `0x86C0A8` (0x79C). `CEmergencyPed` (0x79C) has no
  `InjectHooks` at all.
- Subclasses mostly override `ProcessControl` only; `CPlayerPed` also `SetMoveAnim`, `Save`, `Load`.
- `PedSaveStructure.*` = save blob; `PlayerSkin.*` = `CPlayerSkin`, `RH_ScopedClass` (no vtable).
- Related code elsewhere in `source/game_sa/`: `PedIntelligence.*`, `PedGroup*.*`, `Population.*`, `PlayerPedData.*`,
  `PlayerInfo.*`, `PedIK.*`, `Weapon.*`, `Models/PedModelInfo.*`.

## Conventions
- Use accessors instead of raw members: `GetIntelligence()`, `GetTaskManager()`, `GetActiveWeapon()`,
  `IsInVehicle()` / `GetVehicleIfInOne()`, `IsStateDead()` / `IsAlive()`, `IsCop()` / `IsPlayer()` / `AsPlayer()`,
  `IsCreatedByMission()`, `GetPedModelInfo()`, `GetAE()` / `GetSpeechAE()` / `GetWeaponAE()`.
- No `CPedWeapons` class: weapons are `CPed::m_aWeapons` + `m_nActiveWeaponSlot`.
- Ped flags are one anonymous bitfield struct of four 32-bit words (IDA: `m_nPedFlags`, `m_nSecondPedFlags`,
  `m_nThirdPedFlags`, `m_nFourthPedFlags`). For `flags & 0x1000` in IDA, count bits from the start of that word.
  Word 3 contains the 2-bit `CantBeKnockedOffBike`, so every later flag in that word is shifted by one.
- Don't rename wrong flag names in a reversal PR; add a comment.
- Bones: `eBoneTag` (not `AnimationId`, as some older helpers do).

## Pitfalls
- `CPed::ProcessControl` (`0x5E8CD0`) is not hooked (install commented out in `Ped.cpp`), and every subclass calls
  it, so it runs the original. Same for `ProcessEntityCollision` and the `SpecialEntity*` virtuals.
- `m_pVehicle` is not cleared when the ped leaves the car: test `bInVehicle` too (or use `IsInVehicle()`).
- Some stubs forward through raw `((void(__thiscall*)(...))0x5E....)` casts instead of `plugin::CallMethod`: grep for
  the address too.
- `PlayFootSteps` returns `plugin::CallMethod<0x5E57F0>` first; the C++ below it is an untested draft.
- Pointers from scripts or older frames: validate with `IsPedPointerValid` / `CPed::IsPointerValid()`.
