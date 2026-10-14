# Peds

How peds (pedestrians, the player, cops, medics...) are put together, what runs every frame, and what is left to
reverse. The AI side is covered in [Tasks.md](Tasks.md) and [Events.md](Events.md); this page only describes where it
plugs into `CPed`. General rules are in [../ReversingRules.md](../ReversingRules.md) and
[../ReversingGuide.md](../ReversingGuide.md).

Paths are relative to `source/game_sa/`. Addresses come from the repo (`RH_*Install`, `// 0x...`, `@addr`,
`StaticRef`).

---

## 1. Overview

```
CEntity ─> CPhysical ─> CPed ─┬─> CPlayerPed      (one per player, CWorld::Players[i].m_pPed)
                              ├─> CCivilianPed    (random and mission civilians, gang members, criminals...)
                              ├─> CCopPed         (adds partner and criminals-to-kill list)
                              └─> CEmergencyPed   (medics, firemen)

CPed ──owns──> CPedIntelligence* (pooled)      ──> CTaskManager, CEventHandler, CEventGroup, scanners
     ──points─> CPlayerPedData* (players only)  ──> lives inside CPlayerInfo (CWorld::Players[])
     ──embeds─> CPedIK, CWeapon[NUM_WEAPON_SLOTS], CAcquaintance, 3 audio entities
     ──points─> CPedStat* (from CPedStats), model is a CPedModelInfo
```

- A ped is a skinned `RpClump`. Animation goes through the RpAnimBlend plugin and `CAnimManager` (see
  [Animation.md](Animation.md)). `m_apBones` caches one `AnimBlendFrameData*` per `ePedNode`.
- Behaviour comes almost entirely from tasks run by `CPedIntelligence`. `CPed` itself does physics glue, weapons,
  rendering tweaks (IK, look-at, gun flash) and bookkeeping.
- Peds are allocated from the ped pool (`CPed::operator new`, `0x5E4720` / `0x5E4730`). `CPopulation` creates and
  removes random peds. Scripts create mission peds (`ePedCreatedBy::PED_MISSION`).

---

## 2. Key classes and files

| Class | File | Size | Notes |
|---|---|---|---|
| `CPed` | `Entity/Ped/Ped.h` | `0x79C` | Vtable `0x86C358`, 26 entries |
| `CPlayerPed` | `Entity/Ped/PlayerPed.h` | `0x7A4` | Vtable `0x86D168`. Adds `m_p3rdPersonMouseTarget`, `field_7A0` |
| `CCivilianPed` | `Entity/Ped/CivilianPed.h` | `0x79C` | Vtable `0x86C0A8`. Only overrides `ProcessControl` (`0x5DDBF0`) |
| `CCopPed` | `Entity/Ped/CopPed.h` | `0x7C4` | Vtable `0x86C120`. `m_pCopPartner`, `m_apCriminalsToKill[5]`, `ProcessControl` `0x5DE160` |
| `CEmergencyPed` | `Entity/Ped/EmergencyPed.h` | `0x79C` | **No `InjectHooks` at all** (see Gotchas) |
| `CPedIntelligence` | `PedIntelligence.h` | `0x294` | Task manager, event handler/group, vehicle/ped/event/collision scanners, stuck checker |
| `CPlayerPedData` | `PlayerPedData.h` | `0xAC` | Wanted, clothes, sprint/breath, targeting, player group id |
| `CPlayerInfo` | `PlayerInfo.h` | `0x190` | `CWorld::Players` (`0xB7CD98`), `CWorld::PlayerInFocus` (`0xB7CD74`) |
| `CPedGroup` | `PedGroup.h` | `0x2D4` | Leader (`m_pPed`), `CPedGroupMembership`, `CPedGroupIntelligence` |
| `CPedGroups` | `PedGroups.h` | static | `ms_groups` = 8 groups at `0xC09920`, `ms_activeGroups` `0xC098E0` |
| `CPopulation` | `Population.h` | static | Spawning/removal, ped/car groups, counters (`ms_nTotalPeds` `0xC0EC28`, ...) |
| `CPedModelInfo` | `Models/PedModelInfo.h` | `0x44` | Anim group, ped type, stat type, hit col model, radio, voices |
| `CPedStat` / `CPedStats` | `PedStats.h` | `0x34` / static | `ms_apPedStats` `0xC0BBEC`, loaded from `DATA\PEDSTATS.DAT` |
| `CWeapon` | `Weapon.h` | `0x1C` | Type, state, clip ammo, total ammo, next-shot time. See [Weapons.md](Weapons.md) |
| `CWeaponInfo` | `WeaponInfo.h` | `0x70` | `aWeaponInfo` `0xC8AAB8`, `GetWeaponInfo(type, skill)` |
| `CPedIK` | `PedIK.h` | `0x20` | Torso/arm aiming, slope pitch. `ms_torsoInfo` `0x8D22E4` |
| `CPedSaveStructure` | `Entity/Ped/PedSaveStructure.h` | `0x18C` | Save/load blob |

Sizes are the `VALIDATE_SIZE` values in the headers. There is **no `CPedWeapons` class**: weapons are
`CPed::m_aWeapons` (`std::array<CWeapon, NUM_WEAPON_SLOTS>`) plus `m_nActiveWeaponSlot`, `m_nSavedWeapon` and
`m_nDelayedWeapon`.

### `CPed` layout highlights (`Ped.h`)

| Field | Meaning |
|---|---|
| `m_pedAudio`, `m_pedSpeech`, `m_weaponAudio` | Protected. Use `GetAE()`, `GetSpeechAE()`, `GetWeaponAE()` |
| Flag struct (4 x 32 bits) | See "Ped flags" below |
| `m_pIntelligence`, `m_pPlayerData`, `m_nCreatedBy` | Protected. Use `GetIntelligence()`, `GetPlayerData()`, `IsCreatedBy*()` |
| `m_apBones[TOTAL_PED_NODES]` | Frame data per `ePedNode` (a TODO in the source says it should be `m_apNodes`) |
| `m_nAnimGroup` | `AssocGroupId` copied from the model info in `SetModelIndex` |
| `m_pedIK` | `CPedIK`, constructed with `this` |
| `m_nPedState`, `m_nMoveState` | `ePedState` (legacy state machine, still used everywhere), `eMoveState` |
| `m_fHealth`, `m_fMaxHealth`, `m_fArmour` | |
| `m_fCurrentRotation`, `m_fAimingRotation` | Heading in radians. `CalculateNewOrientation` writes the matrix from it |
| `m_pVehicle` | May be set while not in a vehicle. Use `IsInVehicle()` / `GetVehicleIfInOne()` |
| `m_pStats` | `CPedStat*` |
| `m_pTargetedObject` | Lock-on target |

### Virtual interface (declaration order)

Overrides of `CEntity`/`CPhysical`, then the three virtuals `CPed` adds. Hook status from `CPed::InjectHooks`:

| Virtual | Address | Hooked? |
|---|---|---|
| `SetModelIndex` | `0x5E4880` | VMT, reversed |
| `DeleteRwObject` | `0x5DEBF0` | VMT, reversed |
| `ProcessControl` | `0x5E8CD0` | **No.** Install commented out, body is `plugin::CallMethod` |
| `Teleport` | `0x5E4110` | VMT, reversed |
| `SpecialEntityPreCollisionStuff` | `0x5E3C30` | **No** (commented out, `plugin::CallMethod`) |
| `SpecialEntityCalcCollisionSteps` | `0x5E3E90` | **No** (commented out, `plugin::CallMethod`) |
| `PreRender` | `0x5E8A20` | VMT, reversed (calls `PreRenderAfterTest` unless driving) |
| `Render` | `0x5E7680` | VMT, reversed |
| `SetupLighting` / `RemoveLighting` | `0x553F00` / `0x5533B0` | VMT, reversed |
| `FlagToDestroyWhenNextProcessed` | `0x5E7B70` | VMT, reversed |
| `ProcessEntityCollision` | `0x5E2530` | **No** (commented out, `plugin::CallMethod`) |
| `SetMoveAnim` (new) | `0x5E4A00` | VMT, reversed |
| `Save` / `Load` (new) | `0x5D5730` / `0x5D4640` | VMT, reversed |

Subclass overrides: `CPlayerPed::ProcessControl` (`0x60EA90`), `SetMoveAnim` (`0x609490`, empty), `Load` / `Save`
(`0x5D46E0` / `0x5D57E0`). `CCopPed`, `CCivilianPed` and `CEmergencyPed` override `ProcessControl` only.

### Ped flags (bitfields)

The flags are one anonymous struct of `bool : 1` fields in `Ped.h`, grouped in four 32-bit words. The source
comments name the words `m_nPedFlags`, `m_nSecondPedFlags`, `m_nThirdPedFlags`, `m_nFourthPedFlags` (IDA/plugin-sdk
names; there are no such members in the C++). The fourth word only uses 22 bits.

- Word 1 (bits 0-31): `bIsStanding` (bit 0), `bWasStanding`, `bIsLooking`, ..., `bInVehicle` (bit 8),
  `bIsInTheAir`, `bIsLanding`, ..., `bIsDucking` (bit 26), `bAllowMedicsToReviveMe` (bit 31).
- Word 2: `bResetWalkAnims`, `bOnBoat`, ..., `bFadeOut` (bit 3), ..., `bKnockedOffBike` (bit 31).
- Word 3: `bDonePositionOutOfCollision`, `bDontRender`, ... `CantBeKnockedOffBike` is a **2-bit** `uint8` field
  (values in `eCantBeKnockedOffBike`), so every flag after it in that word is shifted by one bit compared to a naive
  count.
- Word 4: `bHasBulletProofVest`, `bUsingMobilePhone`, ..., `bUsedForReplay`.

When IDA shows `this->m_nThirdPedFlags & 0x1000` or `(ped->m_nPedFlags >> 8) & 1`, count bits from the start of the
word (comments give the bit index for word 1). Defaults are default member initialisers, so the constructor does not
repeat them. A few names are known to be wrong (`bRemoveHead`, `bKnockedOffBike`, `bDoBloodyFootprints` vs its
"bIsLeader" comment); keep the existing name and add a comment instead of renaming in a reversal PR.

---

## 3. Frame flow

Driven from `CGame::Process` and the render loop in `Idle()` (`source/app/app_game.cpp`). The full frame order is in
[WorldStreamingPools.md](WorldStreamingPools.md) §7.

```
CGame::Process
 ├─ CPopulation::Update(generatePeds)           spawn/remove random peds
 ├─ CWorld::Process
 │   ├─ CEventGlobalGroup::AddEventsToPed / AddEventsToGroup, inform-friends/group queues, Flush
 │   ├─ entity->UpdateAnim()       for each moving entity
 │   ├─ entity->ProcessControl()   for each moving entity; m_bRemoveFromWorld peds go to CPopulation::RemovePed
 │   │                             then again for "postponed" entities with CWorld::bForceProcessControl = true
 │   ├─ ProcessCollision / ProcessShift passes
 │   ├─ g_ikChainMan.Update, ProcessAttachedEntities
 │   ├─ for peds: CPedIntelligence::ProcessAfterProcCol, attached peds -> PositionAttachedPed
 │   ├─ CPlayerInfo::Process(i) for each player
 │   └─ CPedScriptedTaskRecord::Process, CPedGroups::Process, RemoveFallenPeds every 8th frame
 └─ TheCamera.Process ...
Idle(): CRenderer::PreRender()                -> CPed::PreRender -> PreRenderAfterTest (IK, shadows, skin anim)
        CWorld::ProcessPedsAfterPreRender()   -> CPedIntelligence::ProcessAfterPreRender
```

### `CPed::ProcessControl` (`0x5E8CD0`, not reversed)

Order of the main calls, read from the IDA decompilation. Use it as a map, not as truth; verify every branch when you
reverse it.

1. `CAEPedAudioEntity::Service` on `m_pedAudio`.
2. Fire/molotov FX on the ped (`FxManager_c::CreateFxSystem("molotov_flame", ...)` / `DestroyFxSystem`).
3. Every 32 frames (offset by the entity random seed): `CEntity::PruneReferences`. Fade-out alpha handling through
   `CVisibilityPlugins::GetClumpAlpha` / `SetClumpAlpha` (`bFadeOut`).
4. Mission-ped/player-group flag fix-ups, `ProcessBuoyancy` (`0x5E1FA0`, reversed), player wetness counter.
5. **Postpone check**: if standing on an entity that has not been processed yet and `CWorld::bForceProcessControl`
   (`0xB7CD6E`) is false, it sets the entity's "was postponed" flag and skips the rest. `CWorld::Process` calls it
   again later in the frame.
6. `CPedIntelligence::ProcessFirst` (`0x6073A0`), vertical speed clamp, then either `CPhysical::SkipPhysics` (ped
   standing still, see the long condition) or `CPhysical::ProcessControl`.
7. `RequestDelayedWeapon` (`0x5E8910`), `PlayFootSteps` (`0x5E57F0`, stub), **`CPedIntelligence::Process`**
   (`0x608260`), `CalculateNewVelocity` (`0x5E4C50`, stub), `UpdatePosition` (`0x5E1B10`, stub).
8. Bleeding (`CShadows::AddPermanentShadow`, `CLocalisation::Blood`), `CPopulation::UpdatePedCount`, death speech
   (`Say`), chainsaw idle audio, `CAEPedWeaponAudioEntity::Service`.

`CPedIntelligence::Process` is reversed: vehicle scanner, ped scanner, event scanner, `m_eventHandler.HandleEvents()`,
`m_TaskMgr.ManageTasks()`, relationship recorder, `LookAtInterestingEntities` (details in [Tasks.md](Tasks.md) §3).

### Subclass `ProcessControl`

- `CPlayerPed::ProcessControl` (reversed): drunk blur and breath, then **`CPed::ProcessControl()`** (so GTA's code
  still runs), wanted level update, minigun spin, target marker (`CWeaponEffects::MarkTarget`), sprint energy,
  `GetActiveWeapon().Update(this)`, weapon switch (`ProcessWeaponSwitch`, stub), `ProcessAnimGroups` (stub), look
  direction from the active camera (`TheCamera.GetActiveCamera().m_nMode == MODE_FOLLOWPED`), `ProcessGroupBehaviour`
  (stub), water cover, "keep up" group speech.
- `CCivilianPed::ProcessControl`: skips everything if `GetCreatedBy() == (PED_MISSION | PED_GAME)`, otherwise
  `CPed::ProcessControl` then `GetActiveWeapon().Update(this)`.
- `CCopPed::ProcessControl`: wanted-level/decision-maker tweaks, then `CPed::ProcessControl`, then (unless postponed
  or dead) `GetActiveWeapon().Update(this)` and target handling.
- `CEmergencyPed::ProcessControl` (not hooked): base call, then weapon update.

### Rendering side

- `CPed::PreRenderAfterTest` (`0x5E65A0`, reversed): swim/jetpack roll and pitch, `m_pedIK.PitchForSlope()`,
  `UpdateRpHAnim()`, minigun barrel rotation, shadows (`CShadows::StoreShadowForPedObject`, real-time shadows),
  `ShoulderBoneRotation`, wind/rain cloth scaling and earthquake sway on bone matrices, blood/splash FX,
  `CPedModelInfo::AnimatePedColModelSkinnedWorld`.
- `CPed::Render` (`0x5E7680`, reversed), with the NOTSA cheats `RenderBigHead` / `RenderThinBody`.

---

## 4. Subsystems that hang off peds

### Animation hookup

- `CPed::SetModelIndex`: `RpAnimBlendClumpInit`, `RpAnimBlendClumpFillFrameArray(clump, m_apBones)`, stats from the
  model info, `m_nAnimGroup = mi->m_nAnimType`, `CAnimManager::AddAnimation(clump, group, ANIM_ID_IDLE)`, sets
  `RpAnimBlendClumpGetData(clump)->m_PedPosition` to `&m_vecAnimMovingShiftLocal` (root motion goes there), creates
  the skinned hit col model, `UpdateRpHAnim()`.
- `CPed::SetMoveAnim` picks walk/run/sprint associations with `RpAnimBlendClumpGetAssociation` and blends idle with
  `CAnimManager::BlendAnimation`. `SetMoveAnimSpeed` scales them unless `bMoveAnimSpeedHasBeenSetByTask`.
- Most animation is started by tasks (`Tasks/TaskTypes/*`), not by `CPed`.
- Use the accessors `GetAnimHierarchy()` / `GetAnimBlendData()`.

### IK

- `CPedIK` (`PedIK.cpp`, fully hooked): `RotateTorso`, `RotateTorsoForArm`, `PointGunInDirection`,
  `PointGunAtPosition`, `PitchForSlope`, `MoveLimb`. Callers are mostly tasks (`CTaskSimpleUseGun`,
  `CTaskSimpleArrestPed`, `CTaskSimpleSwim`) plus `PreRenderAfterTest`.
- Look-at and other chains go through `IKChainManager_c` (`Ragdoll/IKChainManager.h`, global `g_ikChainMan`), updated
  once per frame in `CWorld::Process`.

### Weapons

- `CPed::GiveWeapon` (`0x5E6080`), `SetCurrentWeapon` (`0x5E61F0` / `0x5E6280`), `AddWeaponModel` /
  `RemoveWeaponModel`, `GetWeaponSkill`, `DoGunFlash`, `SetGunFlashAlpha` and `GiveDelayedWeapon` are reversed.
- `CWeapon::Update(owner)` is called from the subclass `ProcessControl`s. The firing pipeline is in
  [Weapons.md](Weapons.md).
- Data: `CWeaponInfo::GetWeaponInfo(type, skill)` or the NOTSA overload `GetWeaponInfo(CPed*)`.

### Player

- `CPlayerInfo` holds `m_pPed`, `m_PlayerData` (the `CPlayerPedData` that `CPed::m_pPlayerData` points to), money,
  `m_pRemoteVehicle`, state. `CPlayerInfo::Process` (`0x56F8D0`) is still a `.Reversed = false` stub.
- `CPlayerPed::GetPlayerGroup()` returns `CPedGroups::GetGroup(GetPlayerData()->m_nPlayerGroup)`.
- `CPlayerPed::UpdateCameraWeaponModes` (`0x609830`) is where the ped asks the camera for weapon modes.

### Groups

- `CPedGroups::Process` (`0x5FC800`) and `RemoveGroup` are reversed. The other 9 statics (`AddGroup`, `Init`,
  `IsGroupLeader`, `GetPedsGroup`, `GetGroupId`, `IsInPlayersGroup`, `AreInSameGroup`, ...) are hooked
  `plugin::Call` stubs (`.Reversed = false`): small, easy targets.
- `CPedGroup` stubs: `PlayerGaveCommand_Gather` (`0x5FAB60`), `FindDistanceToFurthestMember` (`0x5FB010`).

### Population

- `CPopulation::Update`: `ManagePopulation`, `RemovePedsIfThePoolGetsFull`, start-of-game generation countdown,
  `UpdatePedCounts`, then `AddToPopulation` (`0x614720`, **stub**) and `GeneratePedsAtAttractors`. Spawn radii scale
  with `TheCamera.m_fGenerationDistMultiplier`.
- `ManagePed` deletes peds that are far enough (`(pos - player).Magnitude2D() * m_fRemovalDistMultiplier`) and
  `CanBeDeleted()`. `CPopulation::RemovePed` is the only correct way to delete a random ped.
- `CPopulation::AddPed` creates `CCopPed` / `CEmergencyPed` / `CCivilianPed` by ped type.

### Models and stats

- `CPedModelInfo`: `m_nAnimType`, `m_nPedType`, `m_nStatType`, `m_nCarsCanDriveMask`, hit col model
  (`m_pHitColModel`, created lazily by `CreateHitColModelSkinned`), radio stations, voices. Use
  `ped->GetPedModelInfo()`.
- `CPedStats::GetPedStatInfo(name)`, `GetPedStatByArrayIndex(i)`. `CPed::SetPedStats(ePedStats)` (`0x5DEBC0`).

---

## 5. Helpers to use

| Instead of | Use |
|---|---|
| `ped->m_pIntelligence->m_TaskMgr` | `ped->GetTaskManager()`, `GetIntelligence()->FindTaskByType(...)`, `GetTaskManager().HasAnyOf<TASK_...>()` |
| `ped->m_aWeapons[ped->m_nActiveWeaponSlot]` | `GetActiveWeapon()`, `GetWeaponInSlot(slot)`, `GetWeapon(type)` |
| `bInVehicle && m_pVehicle` | `IsInVehicle()`, `IsInVehicle(veh)`, `GetVehicleIfInOne()`, `GetRealPosition()` |
| raw state compares | `IsStateDriving()`, `IsStateDead()`, `IsStateDying()`, `IsStateDeadForScript()`, `IsAlive()` |
| ped type compares | `IsCop()`, `IsGangster()`, `IsCivilian()`, `IsPlayer()`, `AsCop()`, `AsPlayer()`, ... |
| `m_nCreatedBy == 2` | `IsCreatedByMission()`, `IsCreatedBy(PED_MISSION)` |
| `(CPedModelInfo*)GetModelInfo()` | `GetPedModelInfo()` |
| audio members | `GetAE()`, `GetSpeechAE()`, `GetWeaponAE()` |
| raw pointers to peds that may die | `CPed::Ref` (`notsa::EntityRef<CPed>`), or `CEntity::SetEntityReference` / `ClearReference` |
| player wanted | `GetPlayerWanted()`, `CPlayerPed::GetWanted()` |
| bone world position | `GetBonePosition(eBoneTag)`, `GetTransformedBonePosition(RwV3d&, bone)` |
| `rand()` style choices | `CGeneral::GetRandomNumberInRange` (as `SetModelIndex` does) |

Also: `FindPlayerPed()`, `FindPlayerVehicle()`, `CWorld::Players[i]`, `CPedGroups::GetGroup(id)`,
`CPopulation::GetModelsInPedGroup(group)`.

---

## 6. Gotchas

- **`CPed::ProcessControl` is not hooked.** `CPlayerPed`, `CCopPed`, `CCivilianPed` and `CEmergencyPed` call
  `CPed::ProcessControl()`, which forwards to GTA. When you reverse it, register it with `RH_ScopedVMTInstall` (the
  commented line is in `CPed::InjectHooks`) and difftest it hard: it is the hottest ped function.
- **The postpone mechanism.** `ProcessControl` can return early and mark the entity as postponed (§3 step 5). Keep
  the exact condition and the `CWorld::bForceProcessControl` check, or peds standing on vehicles break.
- **`CEmergencyPed` is completely unhooked.** It has no `InjectHooks` and is not listed in
  `source/InjectHooksMain.cpp`. Its constructor (`0x5DE340`), `ProcessControl` (`0x5DE400`) and `InRange`
  (`0x5DE470`) are only comments (KNOWN). `ProcessControl` also has a "bad OG expression" comment.
- **`CPlayerPed::CPlayerPed` (`0x60D5B0`) and `CPed::CanWeRunAndFireWithWeapon` have C++ bodies but no hook.**
  Register and verify them before relying on them.
- **`CPedIntelligence::GetActivePrimaryTask` always reads the PHYSICAL_RESPONSE slot** (see [Tasks.md](Tasks.md) §7).
  Check against `0x4B85B0` before trusting it.
- **Bone IDs typed as `AnimationId`.** In `PreRenderAfterTest` the local helper `GetHierMatrix(AnimationId id)` passes
  the value to `RpHAnimIDGetIndex`, which expects a bone tag. The `ANIM_ID_*` names there are meaningless; use
  `eBoneTag` in new code.
- **Flag word 3 shift** caused by the 2-bit `CantBeKnockedOffBike` (§2, Ped flags).
- `m_pVehicle` is not cleared when the ped leaves the car. Always test `bInVehicle` too.
- `CPed::Teleport` ignores `resetRotation` and flushes tasks only for players or peds leaving a car.
- `CCivilianPed::ProcessControl` compares `GetCreatedBy()` with `PED_MISSION | PED_GAME` (= `PED_GAME_MISSION`,
  replay peds). Keep the expression as is.
- Several stubs use raw function-pointer casts instead of `plugin::CallMethod` (`UpdatePosition`,
  `CalculateNewVelocity`, `DoFootLanded`, `SortPeds`, `CPopulation::AddToPopulation`). They behave the same, but grep
  for the address, not only for `plugin::`.
- `PlayFootSteps` keeps half-working reversed code after an early `return plugin::CallMethod`. Don't assume it is
  correct.
- `IsPedPointerValid(ped)` / `CPed::IsPointerValid()` check the pool slot. Use them on pointers that come from
  scripts or old frames.

---

## 7. What is left

Snapshot from `idb_coverage.py --class <C>` (status words in [README.md](README.md)):

| Class | Functions | Reversed | Stub | NOHOOK | Known | Missing | Bytes left |
|---|---|---|---|---|---|---|---|
| `CPed` | 148 | 130 | 7 | 4 | 2 | 5 | 23,370 |
| `CPlayerPed` | 66 | 48 | 15 | 0 | 1 | 2 | 16,020 |
| `CPedGeometryAnalyser` | 37 | 8 | 29 | 0 | 0 | 0 | 18,694 |
| `CPlayerInfo` | 30 | 25 | 3 | 0 | 0 | 2 | 10,328 |
| `CPopulation` | 65 | 63 | 2 | 0 | 0 | 0 | 5,247 |
| `CPedGroupPlacer` | 4 | 1 | 3 | 0 | 0 | 0 | 3,661 |
| `CPedStuckChecker` | 1 | 0 | 0 | 1 | 0 | 0 | 2,174 |
| `CPedDamageResponseCalculator` | 7 | 2 | 4 | 0 | 0 | 1 | 1,437 |
| `CPedGroups` | 12 | 2 | 9 | 0 | 1 | 0 | 677 |
| `CPedGroupIntelligence` | 29 | 26 | 1 | 0 | 2 | 0 | 559 |
| `CPlayerPedData` | 6 | 5 | 0 | 0 | 0 | 1 | 537 |
| `CPedGroup` | 12 | 10 | 2 | 0 | 0 | 0 | 500 |
| `CPedIntelligence` | 58 | 50 | 0 | 4 | 4 | 0 | 391 |
| `CEmergencyPed` | 5 | 0 | 0 | 0 | 3 | 2 | 353 |

`--class CPed` also matches `CPool::CPed::AtIndex`. Several MISSING rows, such as the pool `operator new/delete`, are
registered under overloaded names the script does not match.

Remaining `CPed` work (size in bytes):
- Stubs: `CalculateNewVelocity` 1679, `UpdatePosition` 1167, `PlayFootSteps` 1755, `DoFootLanded` 1130,
  `PositionPedOutOfCollision` 2966, `KillPedWithCar` 4009, `PositionAttachedPed` 767.
- NOHOOK: `ProcessControl` 2145, `ProcessEntityCollision` 4345, `SpecialEntityPreCollisionStuff` 603,
  `SpecialEntityCalcCollisionSteps` 342.
- Known/unhooked: `SortPeds` (unused), `MakeTyresMuddySectorList` 1891 (template instance, hooked as a stub).

Remaining `CPlayerPed` work, all hooked `.Reversed = false` stubs: `SetRealMoveAnim` 2711, `FindWeaponLockOnTarget`
2270, `DrawTriangleForMouseRecruitPed` 1593, `FindNextWeaponLockOnTarget` 1362, `Compute3rdPersonMouseTarget` 1062,
`MakeThisPedJoinOurGroup` 1029, `ProcessWeaponSwitch` 1012, `ProcessAnimGroups` 909, `KeepAreaAroundPlayerClear` 817,
`SetInitialState` 727, `ProcessGroupBehaviour` 589, `ControlButtonSprint` 526, `EvaluateNeighbouringTarget` 388,
`PlayerWantsToAttack` 200, `GetButtonSprintResults` 113. Plus the unhooked constructor and the missing destructors.

`CPedIntelligence` NOHOOKs are small: `IncrementAngerAtPlayer`, `IsInHearingRange`, `HasInterestingEntites`,
`RemoveAllInterestingEntities`.

### Suggested order

1. **Quick wins (register and verify):** the `CPedGroups` stubs, the `CPedIntelligence` NOHOOKs,
   `CPed::CanWeRunAndFireWithWeapon`, the `CPlayerPed` constructor, `CEmergencyPed` hooks,
   `CPlayerPed::GetButtonSprintResults` / `ControlButtonSprint` / `PlayerWantsToAttack`.
2. **Collision leaves:** `SpecialEntityCalcCollisionSteps`, `SpecialEntityPreCollisionStuff` (VMT installs).
3. **Movement core:** `CalculateNewVelocity`, then `UpdatePosition`. Both run for every ped every frame. Write
   difftests (`source/difftests/`) before swapping.
4. **Footsteps:** `DoFootLanded`, then `PlayFootSteps` (partial code exists).
5. **`CPed::ProcessControl`** once 3-4 are done, so its callees are already ours.
6. **Player input/targeting:** `ProcessAnimGroups`, `SetRealMoveAnim`, `ProcessWeaponSwitch`, then the targeting
   chain `EvaluateNeighbouringTarget` -> `FindNextWeaponLockOnTarget` -> `FindWeaponLockOnTarget` ->
   `Compute3rdPersonMouseTarget`, then group behaviour (`MakeThisPedJoinOurGroup`, `ProcessGroupBehaviour`,
   `DrawTriangleForMouseRecruitPed`), `KeepAreaAroundPlayerClear`, `SetInitialState`.
7. **Big leaves last:** `ProcessEntityCollision`, `PositionPedOutOfCollision`, `KillPedWithCar`,
   `PositionAttachedPed`, `CPopulation::AddToPopulation`, `CPlayerInfo::Process`.
