# Models/ — model info classes

Guide: [../../../docs/subsystems/WorldStreamingPools.md](../../../docs/subsystems/WorldStreamingPools.md) §6 (stores,
`ModelInfoType`, hierarchy) and §5 (streaming of models). Vehicle model info (components, dummies):
[Vehicles.md](../../../docs/subsystems/Vehicles.md) §8.

## Layout
Vtable and virtual count as in each `RH_ScopedVirtualClass`:
```
CBaseModelInfo        0x85BB9C 15   BaseModelInfo.h   (0x20; GetModelType() is pure virtual)
├─ CAtomicModelInfo   0x85BBF0 16   ── CDamageAtomicModelInfo 0x85BC30, CTimeModelInfo 0x85BCB0,
│                                      CLodAtomicModelInfo 0x85BC70 ── CLodTimeModelInfo 0x85BCF0 (all 16)
└─ CClumpModelInfo    0x85BD30 17   ── CPedModelInfo 0x85BDC0, CVehicleModelInfo 0x85C5C8, CWeaponModelInfo 0x85BD78 (17)
```
- `ModelInfo.h` — `CModelInfo` (static): `ms_modelInfoPtrs[20000]` (`0xA9B0C8`), the fixed per-class `CStore`s,
  `AddAtomicModel(id)` / `AddPedModel(id)` / ..., `IsCarModel` / `IsBoatModel` / ... helpers.
- `TimeInfo.*` — `CTimeInfo` (on/off hours, `RH_ScopedClass`), embedded in the time model infos.

## Conventions
- Category `"Models"` for all classes.
- Access via `CModelInfo::GetModelInfo(id)`, `GetPedModelInfo(id)`, `GetVehicleModelInfo(id)` or
  `entity->GetModelInfo()`; don't index `ms_modelInfoPtrs` by hand in new code.
- `AsAtomicModelInfoPtr()` / `AsDamageAtomicModelInfoPtr()` / `AsLodAtomicModelInfoPtr()` are virtual (null when not
  that kind). `AsVehicleModelInfoPtr()` / `AsPedModelInfoPtr()` / `AsWeaponModelInfoPtr()` are unchecked
  `reinterpret_cast`s: check `GetModelType()` first where the original does.
- `m_nKey` is a name hash (`CKeyGen`); lookup by name is `GetModelInfo(const char*, int32* outIndex)`.
- Use `{ .Overrides = false }` where GTA has a per-class copy of a virtual that our class only inherits.

## Pitfalls
- `CLodTimeModelInfo` is deliberately NOT registered in `source/InjectHooksMain.cpp`: its `InjectHooks` installs
  `GetModelType` / `GetTimeInfo` at `0x4C5660` / `0x4C5670` (the `CTimeModelInfo` functions) into vtable `0x85BCF0`,
  which does not contain them, so `VMTInfo` threw at startup. Find the real slots in IDA before registering it.
- `RH_ScopedVMTInstall` throws if the address is not in the class vtable. The LOD/time classes share many base
  functions: check each vtable's actual slots instead of copying a sibling's addresses.
- `CVehicleModelInfo::InjectHooks` also registers a nested `RH_ScopedClass(CLinkedUpgradeList)`.
- Store sizes are fixed (e.g. vehicle 212, ped 278); exceeding them is a hard failure in the original too.
