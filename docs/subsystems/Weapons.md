# Weapons

How a shot happens: weapon state and ammo (`CWeapon`), weapon data (`CWeaponInfo`, `weapon.dat`), the ped side
(slots, skills, aiming tasks), the four ways a weapon fires (instant hit, projectile, area effect, sniper bullet),
the effects that follow (bullet traces, explosions, fires, crosshairs), and how damage reaches a ped through
`CEventDamage` and `CPedDamageResponseCalculator`. Ped basics are in [Peds.md](Peds.md), the event queue in
[Events.md](Events.md), the task tree in [Tasks.md](Tasks.md), and vehicle damage in [Vehicles.md](Vehicles.md) ("Damage").

All paths are relative to the repo root. Addresses are the ones written in the repo (`RH_*Install` or `// 0xADDR`)
and were checked against the IDB.

---

## 1. Overview

```
CPed::m_aWeapons[13]  (CWeapon, 0x1C each)       source/game_sa/Entity/Ped/Ped.h
  index = eWeaponSlot = CWeaponInfo::m_nSlot      source/game_sa/Weapon.h
  m_nActiveWeaponSlot -> GetActiveWeapon()
        │
        │  task asks to fire: CTaskSimpleUseGun::FireGun, CTaskSimpleGangDriveBy::FireGun,
        │  CTaskSimpleThrowProjectile::ProcessPed, CTaskSimplePlayerOnFoot::ProcessPlayerWeapon
        ▼
CWeapon::Fire  0x742300  ── switch m_Type ──┬─ FireInstantHit   0x73FB10  pistols, SMGs, rifles, shotguns, minigun
                                            ├─ FireM16_1stPerson 0x741C00 player in M16/heli-cannon first person
                                            ├─ FireSniper       0x73AAC0  player in sniper mode -> CBulletInfo
                                            ├─ FireProjectile   0x741360  rockets, grenades, molotovs, satchels -> CProjectileInfo
                                            ├─ FireAreaEffect   0x73E800  flamethrower, spraycan, extinguisher -> CShotInfo
                                            ├─ CWorld::UseDetonator, TakePhotograph (camera)
                                            ▼
   hit: CWeapon::DoBulletImpact 0x73B550 ─┬─ CBulletTraces::AddTrace, Fx (impact, blood, tyre burst), CGlass
                                          ├─ vehicle: CVehicle::InflictDamage / BurstTyre
                                          ├─ object:  CObject::ObjectDamage / ApplyForce
                                          └─ ped:     CWeapon::GenerateDamageEvent 0x73A530
                                                       ├─ CPedDamageResponseCalculator::ComputeDamageResponse (health/armour)
                                                       └─ victim->GetEventGroup().Add(CEventDamage) -> CEventHandler
```

Melee does **not** go through `CWeapon::Fire`. Fists and melee weapons are handled by `CTaskSimpleFight`
(`FightStrike`, `FightHitPed`, ..., none reversed), which also ends in `GenerateDamageEvent`. Vehicle guns use
`CWeapon::FireFromCar` (`0x73FA20`), `FireInstantHitFromCar(2)` and `CVehicle::FirePlaneGuns`/`FireUnguidedMissile`.

---

## 2. Key classes

| Class / file | Size | Role |
|---|---|---|
| `CWeapon` (`source/game_sa/Weapon.h/.cpp`) | `0x1C` | One weapon in a ped slot: `m_Type`, `m_State` (`eWeaponState`: READY, FIRING, RELOADING, OUT_OF_AMMO, MELEE_MADECONTACT), `m_AmmoInClip`, `m_TotalAmmo`, `m_TimeForNextShotMs`, `m_IsFirstPersonWeaponModeSelected`, `m_DontPlaceInHand` (goggles), `m_FxSystem` (flame/spray/extinguisher particles) |
| `CWeaponInfo` (`WeaponInfo.h/.cpp`) | `0x70` | Static data per weapon and skill level. `aWeaponInfo[80]` at `0xC8AAB8`. Aim offsets `ms_WeaponAimOffsets` at `0xC8A8A8` |
| `CWeaponModelInfo` (`Models/WeaponModelInfo.h`) | | Model info of a weapon model; `SetWeaponInfo(type)` is called while `weapon.dat` loads |
| `CProjectileInfo` (`ProjectileInfo.h/.cpp`) | `0x24` | 32 live projectiles: `gaProjectileInfo` `0xC891A8` (info) + `ms_apProjectile` `0xC89110` (the `CProjectile` objects). Almost all stubs |
| `CProjectile` (`Entity/Object/Projectile.h`) | `0x17C` | `CObject` subclass used for the projectile entity. Only the constructor has a C++ body, and it is not hooked (IDB `0x5A4030`) |
| `CBulletInfo` (`BulletInfo.h/.cpp`) | `0x2C` | 8 moving bullets (`aBulletInfos`, `0xC88740`). Only `FireSniper` creates them |
| `CBulletTraces` / `CBulletTrace` (`BulletTraces.h`, `BulletTrace.h`) | `0x2C` per trace | 16 tracer lines (`aTraces`, `0xC7C748`), updated and drawn by `CSpecialFX` |
| `CShotInfo` (`ShotInfo.h/.cpp`) | `0x2C` | 100 slow "shots" for area-effect weapons (`aShotInfos`, `0xC89690`) |
| `CExplosion` (`Explosion.h/.cpp`) | `0x7C` | 16 explosions (`aExplosions`, `0xC88950`), `eExplosionType` 0..12 |
| `CFire` / `CFireManager` (`Fire.h`, `FireManager.h`) | `0x28` / `0x964` | 60 fires in `gFireManager` (`0xB71F80`) |
| `CCreepingFire` (`CreepingFire.h`) | | 32 x 32 grid of fire state (`m_aFireStatus`, `0xB71B68`) for spreading ground fire |
| `CWeaponEffects` (`WeaponEffects.h/.cpp`) | `0x2C` | 2 crosshairs / lock-on markers (`gCrossHair`, `0xC8A838`) |
| `CEventDamage` (`Events/EventDamage.h/.cpp`) | | The damage event. Vtable `0x85AD78`, 17 slots |
| `CPedDamageResponseCalculator` (`PedDamageResponseCalculator.h/.cpp`) | `0x14` | Turns a hit into health/armour loss (`CPedDamageResponse`, `0xC`) |

Enums: `eWeaponType` (`source/game_sa/Enums/eWeaponType.h`: real weapons 0..46, `WEAPON_LAST_WEAPON = 47`, then
damage-only types `WEAPON_ARMOUR` 48 .. `WEAPON_FLARE` 58), `eWeaponFire` (MELEE, INSTANT_HIT, PROJECTILE,
AREA_EFFECT, CAMERA, USE), `eWeaponSkill` (POOR, STD, PRO, COP), `eWeaponSlot` (`Weapon.h`, 13 slots). The
weapon flag bits (`CWeaponInfo::m_nFlags`) are in `source/game_sa/Enums/eWeaponFlags.h`; bits `0x40` and `0x80` are
unknown.

---

## 3. Weapon data (`CWeaponInfo`, `weapon.dat`)

`CWeaponInfo::Initialise` (`0x5BF750`) clears all 80 entries and the aim offsets, then `LoadWeaponData`
(`0x5BE670`) parses `DATA\WEAPON.DAT` until `ENDWEAPONDATA`. The first character of a line
selects the record:

| Prefix | Record | Fields |
|---|---|---|
| `$` | gun | name, fire type, target range, weapon range, model 1/2, slot, anim group, clip size, damage, fire offset, **skill level**, required stat, accuracy, move speed, two anim loops (start/end/fire, in frames of 1/30 s), breakout time, flags (hex), and optional speed, radius, lifespan, spread |
| `%` | aim offsets | anim group, aim X/Z, duck X/Z, reload sound times A/B and crouched A/B |
| `£` (byte 163) | melee | see the `£` branch of `LoadWeaponData` |

Skill levels: the 11 weapons `WEAPON_PISTOL` .. `WEAPON_TEC9` (`TypeHasSkillStats`) have POOR, STD, PRO and COP
entries; every other weapon only has STD. `GetWeaponInfoIndex` gives the layout:

```
index  0..46   STD entry of every weapon (index = weapon type)
index 47..57   POOR entries of the 11 skill weapons
index 58..68   PRO entries
index 69..79   COP entries
```

`CWeaponInfo::GetWeaponInfo(type, skill = STD)` (`0x743C60`) is the original accessor. The NOTSA overloads
`CWeapon::GetWeaponInfo(CPed*)` and `CWeaponInfo::GetWeaponInfo(CPed*, skill)` pick the skill with
`CPed::GetWeaponSkill(type)` (`0x5E3B60`). For the player it compares the weapon's skill stat with the POOR and PRO
`m_nReqStatLevel`; cops with a pistol get COP; other peds use `m_nWeaponSkill`. Weapons without skill stats are STD.

Other helpers: `GetWeaponReloadTime` (`0x743D70`: 1000 ms, or 2000 for twin pistols, when the `bReload` flag is set;
1000 for `bLongReload`; otherwise the first reload sound time + 100 that exceeds 400, else 400),
`GetTargetHeadRange` (`0x743D50`), `GetSkillStatIndex` (`0x743CD0`), `FindWeaponType` (`0x743D10`),
`GetCrouchReloadAnimationID` (`0x685700`).

---

## 4. Per-frame flow

### 4.1 Global update

```
CGame::Process
 ├─ CCreepingFire::Update()                     0x539CE0
 ├─ gFireManager.Update()                       0x53AF00   CFire::ProcessFire for each active fire
 ├─ CWeapon::UpdateWeapons()                    0x73A360
 │   ├─ CShotInfo::Update            0x739E60   move area-effect shots, set peds/cars on fire, spray paint, extinguish
 │   ├─ CExplosion::Update           0x737620   damage, fuel jets, fire spread, sounds
 │   ├─ CProjectileInfo::Update      0x738B20   STUB: move projectiles, fuses, heat seeking, remove/explode
 │   └─ CBulletInfo::Update          0x7360D0   move sniper bullets, line-of-sight hit test
 └─ CWorld::Process                             peds and vehicles (see 4.2)
       ...
CSpecialFX::Update -> CBulletTraces::Update
RenderEffects -> CWeaponEffects::Render          0x742CF0, STUB (source/app/app_game.cpp)
CSpecialFX::Render -> CBulletTraces::Render
```

### 4.2 Per ped

- `CWeapon::Update(owner)` (`0x73DB40`) runs once per frame for the active weapon, from
  `CPlayerPed::ProcessControl`, `CCivilianPed`, `CCopPed` and `CEmergencyPed`
  `ProcessControl`, and also from `CTaskSimpleGangDriveBy::ProcessPed`, `CVehicle::DoDriveByShootings` and the
  two-player camera. It is the state machine:
  - FIRING: play shotgun pump sounds; when `m_TimeForNextShotMs` passes go to READY (or OUT_OF_AMMO when the
    total is 0 for a non-melee weapon).
  - RELOADING: play reload sounds in step with the reload anim (`ANIM_ID_RELOAD` or the crouched one). Shorten the
    wait if the anim has finished. When the time passes, `Reload(owner)` refills the clip and the state goes to READY.
  - MELEE_MADECONTACT goes to READY. Every state except FIRING calls `StopWeaponEffect`.
- `Reload` (`0x73AEB0`): `m_AmmoInClip = min(clip size for the owner's skill, m_TotalAmmo)`.

### 4.3 Who asks for a shot

| Caller | Address | Status |
|---|---|---|
| `CTaskSimpleUseGun::FireGun` | `0x61EB10` | reversed. Passes `m_TargetEntity` and `m_TargetPos` (null if X or Y is 0) |
| `CTaskSimpleUseGun::ProcessPed` | `0x62A380` | STUB (3 KB): aiming, burst, reload commands |
| `CTaskSimpleGangDriveBy::FireGun` | `0x627CC0` | not reversed |
| `CTaskSimpleThrowProjectile::ProcessPed` | `0x62AF50` | reversed |
| `CTaskSimplePlayerOnFoot::ProcessPlayerWeapon` | `0x6859A0` | detonator and camera fire directly |

Aiming for the player: `CPlayerPed::FindWeaponLockOnTarget` (`0x60DC50`), `FindNextWeaponLockOnTarget`
(`0x60E530`) and `Compute3rdPersonMouseTarget` (`0x60B650`) are STUBs. The lock-on target is
`CPed::m_pTargetedObject`, and the marker is drawn by `CWeaponEffects::MarkTarget` (`0x742BF0`, called from
`CCam` and `CPlayerPed`). `CPed::SetAimFlag`/`ClearAimFlag` are reversed. Auto-aim helpers in `CWeapon`
(`DoDoomAiming` reversed; `DoTankDoomAiming`, `DoDriveByAutoAiming`, `FindNearestTargetEntityWithScreenCoors` STUB)
change the end point of a shot.

---

## 5. Firing in detail

### 5.1 `CWeapon::Fire(firedBy, startPosn, barrelPosn, targetEnt, targetPosn, altPosn)` (`0x742300`)

1. Weapon info for the shooter's skill. If `startPosn` is null, use `(0, 0, 0.6)` transformed by the shooter's matrix
   for both start and barrel. In first-person weapon mode, move the effect position 0.15 m sideways.
2. Return false unless the state is READY or FIRING. If the clip is empty, refill it from the total, or return false
   when there is no ammo at all.
3. Dispatch by type:

| Type | Function | Next-shot delay (original) |
|---|---|---|
| grenade, tear gas, molotov, satchel | `FireProjectile`, force = from the distance to `targetPosn` (0.2..1), or the player's `m_fAttackButtonCounter * 0.0375`, or 0.3 | anim loop |
| pistols, SMGs, AK47, M4, country rifle, minigun | `FireM16_1stPerson` for player 1 in M16/heli-cannon first person, otherwise `FireInstantHit(..., altPosn, false, true)` | none when the ped is on foot or doing `TASK_SIMPLE_GANG_DRIVEBY`, else anim loop |
| shotguns | `FireInstantHit` | anim loop |
| sniper rifle | `FireSniper` for player 1 in `MODE_SNIPER`, else `FireInstantHit` | anim loop |
| rocket launcher (+HS) | `FireProjectile`. A non-player ped does not fire when the target is 8 m or closer | anim loop |
| flamethrower, spraycan, extinguisher | `FireAreaEffect` | none |
| detonator | `CWorld::UseDetonator`, ammo set to 1 | anim loop |
| camera | `TakePhotograph` (`0x73C1F0`) | 1100 ms |
| anything else | nothing fires | |

4. On success: set `bFiringWeapon`, play `AE_WEAPON_FIRE`, the player says `CTX_GLOBAL_SHOOT` at a ped target (not
   with the silenced pistol). Satchels give the detonator. Update the player stats (explosives or bullets fired).
   Unless the infinite-ammo cheat is on: decrement the clip, and decrement the total (for the player only while
   game progress is below 100% or for the detonator; for other peds only below 25000, which means infinite).
5. Clip now empty and ammo left: state RELOADING, next shot at `now + GetWeaponReloadTime()` (a quarter of that with
   the fast-reload perk). Otherwise the next shot is at `now + (animLoopEnd - animLoopStart) * 900`, at `now` without
   a delay, or 1100 ms for the camera.
6. `WEAPON_UNARMED` and `WEAPON_BASEBALLBAT` always return true.

### 5.2 Instant hit

`FireInstantHit` (`0x73FB10`, STUB, 6 KB) is still original code. It builds the shot line from the weapon range
and accuracy, calls `CWeapon::ProcessLineOfSight` (`0x73B300`: birds and oil fires react, then
`CWorld::ProcessLineOfSight`), `CheckForShootingVehicleOccupant` (STUB), and for shotguns `SetUpPelletCol`
(`0x73C710`, reversed) with `ms_PelletTestCol` (`0xC8A7DC`). Every hit goes to the reversed **`DoBulletImpact`**
(`0x73B550`):

- player shot: report `CRIME_FIRE_WEAPON`, update the hit and skill stats (`CStats::UpdateStatsWhenWeaponHit`) when
  the target is a valid enemy, a damageable car or a breakable object;
- add a tracer (`CBulletTraces::AddTrace`, starting 40% of the way along for incremental hits);
- building: impact Fx, remember `m_nModelIndexOfLastBuildingShot`;
- vehicle: tyre pieces burst the tyre and send `CEventVehicleDamageWeapon` to the occupants; other pieces call
  `CVehicle::InflictDamage` (damage doubled in the two-player car camera) and push the car;
- object: break-by-gun objects take 151 (or `151 * smash multiplier`) or 50 damage; otherwise uproot static
  objects and push them;
- ped (not the shooter, not the same ped type unless civilian or the player is shooting):
  `GenerateDamageEvent` with the weapon damage, 150 for a player point-blank shot (not shotguns), and `-incremental
  * damage` for negative `incrementalHit`; then blood Fx;
- player: pad shake.

`FireOneInstantHitRound` (`0x73AF00`) is a simpler version used by `CVehicle::FireFixedMachineGuns`, `CHeli::ProcessControl` and a script command: point light,
line of sight, tracer, and damage to the ped or vehicle that was hit.

### 5.3 Projectiles

`FireProjectile` (`0x741360`, reversed):

- Rocket launchers: the player must be in a scoped camera mode. The projectile starts 1 m along the camera front
  (player) or the entity forward. With `RLAUNCHER_HS`, the player gets a homing target only when
  `PickTargetForHeatSeekingMissile` (`0x73F910`) returns the same target as last time and the button has been held
  for more than 1.5 s. Then the crosshair turns red; otherwise the projectile is a plain rocket. AI peds check
  line of sight 8 m towards the target first.
- Thrown weapons: the start is pushed forward so it is at least 0.3 m in front of the thrower.
- If the line of sight from the hand to the start point is clear, call `CProjectileInfo::AddProjectile`
  (`0x737C80`, STUB). For a rocket that has a target entity or target position, it passes a direction towards it.
  If the way is blocked, grenades and satchels are dropped 1 m behind the ped (0.4 m lower) with force 0 when that
  sphere is free; otherwise `RemoveNotAdd` (`0x737C00`, STUB) explodes the projectile at once (grenade/satchel ->
  `EXPLOSION_GRENADE`, molotov -> `EXPLOSION_MOLOTOV`, rockets -> `EXPLOSION_ROCKET`).
- Report `CRIME_EXPLOSION`, add an interesting event, and a global `CEventGunShot`.

`AddProjectile` creates a `CProjectile` (`CObject`) with the weapon model and a collision sphere, gives it velocity,
registers a radar blip for some types, and starts Fx. `Update` (STUB) moves the projectiles, runs fuses, steers
heat-seeking rockets (`EvaluateTargetForHeatSeekingMissile`, `0x73E560`), makes tear gas choke peds
(`CWorld::SetPedsChokingEffect`), and calls `RemoveProjectile` (`0x7388F0`, STUB), which explodes it
(`CExplosion::AddExplosion`) and removes the object. `RemoveDetonatorProjectiles` (`0x738860`) is called by
`CWorld::UseDetonator`. `IsProjectileInRange` (`0x739860`) is the only reversed function.

### 5.4 Sniper bullets (`CBulletInfo`)

`FireSniper` (`0x73AAC0`, reversed) does not trace instantly. It adds a `CBulletInfo` at the camera
source with velocity `front * 16` (`AddBullet`, `0x736010`, lifetime 1000 ms), shakes the pad and camera, reports the
crime, and adds `CEventGunShot` and `CEventGunShotWhizzedBy` to the global event group. It also cycles
`CCoronas::MoonSize` when the shot points at the moon.

`CBulletInfo::Update` (`0x7360D0`) moves each bullet by `velocity * timeStep / 2` and tests the
segment with `CWorld::ProcessLineOfSight` (including dead peds, tyres and bikers). Peds get `GenerateDamageEvent`
plus blood, vehicles get `InflictDamage` or a tyre burst, objects get damage or a push, buildings record the model.
For the sniper rifle it also reports the impact sound and passes the segment to `CHeli::TestSniperCollision`
(`0x6C6890`), so that sniper bullets can hit helicopters.

### 5.5 Area effect (`CShotInfo`)

`FireAreaEffect` (`0x73E800`) works out the shot direction (towards the target, along the
3rd-person mouse camera for the player, or along the heading tilted by the player's look pitch), calls
`CShotInfo::AddShot` (`0x739C30`), then `DoWeaponEffect` (`0x73E690`), which creates or moves the
"flamethrower" / "extinguisher" / "spraycan" particle system in `m_FxSystem`. The flamethrower also starts creeping
fires about one shot in three, and every call reports `CRIME_FIRE_WEAPON`. A shot is a point that travels
`m_Speed` per frame (randomised by `RandTable` when `bRandSpeed`, spread by `m_fSpread`) and lives `m_fLifespan`.
`CShotInfo::Update` (`0x739E60`) checks peds and cars near each shot: the flamethrower sets them on fire
(`gFireManager.StartFire`) and damages them, the spraycan calls `CWorld::SprayPaintWorld` (tags), and the extinguisher
calls `gFireManager.ExtinguishPointWithWater`. `GetFlameThrowerShotPosn` (`0x739DE0`) is used by the audio.

### 5.6 Explosions and fire

`CExplosion::AddExplosion(victim, creator, type, pos, lifetime, usesSound, cameraShake, invisible)` (`0x736A50`)
takes a free slot, sets radius, propagation rate, expiry and damage percentage per type, plays
Fx and sound, shakes the camera and pads, and calls `CWorld::TriggerExplosion` (`0x56B790`), which runs
`TriggerExplosionSectorList` (`0x567750`) over the nearby sectors: peds get damage events and are knocked over,
vehicles and objects are damaged and pushed, bomb timers are processed. `CExplosion::Update` (`0x737620`)
runs `TriggerExplosion` for delayed explosions (non-zero `lifetime`) when their timer ends, grows the radius by
`m_fPropagationRate`, adds point lights, sets peds, the world and cars on fire for molotovs
(`CWorld::SetPedsOnFire`/`SetWorldOnFire`/`SetCarsOnFire`), adds random secondary rocket explosions to aircraft
explosions, and lets car and boat explosions start creeping fires (`CCreepingFire::TryToStartFireAtCoors`,
`0x53A450`). The fuel arrays (`m_vecFuelDirection`, ...) are only filled in `AddExplosion`.

`CFireManager` (all 24 functions reversed): `StartFire(pos, ...)` (`0x539F00`), `StartFire(entity, creator, ...)`
(`0x53A050`) for burning peds and cars, `StartScriptFire` (`0x53A270`), `ExtinguishPoint(WithWater)`,
`FindNearestFire`, `GetNextFreeFire` (may extinguish an old fire). `Update` (`0x53AF00`)
calls `CFire::ProcessFire` (`0x53A570`). A fire follows the entity it burns, damages a burning
vehicle (`InflictDamage` with `WEAPON_FLAMETHROWER`), sets the player and nearby vehicles on fire, sometimes spreads
through `CCreepingFire` while it has generations left (`m_NumGenerationsAllowed`), merges with small fires next to
it, and goes out when `m_TimeToBurn` passes.

### 5.7 Damage to peds

```
CWeapon::GenerateDamageEvent(victim, creator, type, damage, piece, direction)   0x73A530
 ├─ CPedDamageResponseCalculator calc{creator, damage, type, piece, false}      0x4AD3F0
 ├─ CEventDamage event{creator, now, type, piece, direction, false, victim->bInVehicle}   0x4AD830
 ├─ dead victim already in TASK_SIMPLE_DEAD: play the floor-hit anim, done
 ├─ !event.AffectsPed(victim) (0x4B35A0): return false
 ├─ creator is the player: CRIME_DAMAGED_PED
 ├─ calc.ComputeDamageResponse(victim, event.m_damageResponse, true)            0x4B5AC0 STUB
 │    ├─ AccountForPedDamageStats 0x4AD430 STUB, AccountForPedArmour 0x4AD550 STUB
 │    └─ ComputeWillKillPed 0x4B3210 (reversed) -> ComputeWillForceDeath 0x4AD610 STUB
 │         health -= damage, or 0 when it would drop below 1 or death is forced; pain speech
 ├─ melee weapon (or a falling object) and not in a car: event.ComputeAnim, play the hit anim now
 ├─ stealth mode for the silenced pistol or a stealth kill
 └─ victim->GetEventGroup().Add(event)   (not while doing a gang drive-by)
         └─ later, in the victim's event handling: CEventHandler::ComputeDamageResponse (0x4C0170)
                        -> picks the response task: dying / falling when the ped was killed or knocked down,
                           otherwise a hit reaction or a reaction to the attacker
```

`CEventDamage::ProcessDamage` (`0x4B3A20`), `ComputeDeathAnim` (`0x4B3A60`), `ComputeDamageAnim` (`0x4B3FC0`) and
`ComputeBodyPartToRemove` (`0x4ADC10`, headshots) are reversed.

---

## 6. Gotchas

- **Damage-only weapon types alias skill entries.** `GetWeaponInfo(type, STD)` returns `aWeaponInfo[type]`, so the
  damage-only types 47..58 (`WEAPON_FALL` = 54, `WEAPON_FLARE` = 58, ...) land on the POOR (47..57) and PRO (58)
  entries of the skill weapons. Code that reads `m_nWeaponFire` for any `eWeaponType` reads unrelated data; the
  NOTSA guard in `GenerateDamageEvent` checks `TypeIsWeapon` first when bug fixes are enabled.
- **`CWeaponInfo::m_nAmmoClip` is `uint16`**, and the original reads it sign-extended (`(__int16)` in `Fire`).
- **Melee is not `Fire`.** No original caller passes a melee weapon to `CWeapon::Fire` (the 7 call sites are use-gun,
  gang drive-by, throw-projectile and player-on-foot). The repo's `default: NOTSA_UNREACHABLE()` in `Fire`
  differs from the original, which treats an unknown type as "fired if `targetPosn` is not null". This is latent.
- **`LaserScopeDot` (`0x73A8D0`) is hooked as reversed but its body is `NOTSA_UNREACHABLE()`**.
  The original has no callers, so this is harmless unless some code starts calling it.
- **`FireInstantHit` is still original**, but `DoBulletImpact`, `GenerateDamageEvent`, `ProcessLineOfSight` and
  `SetUpPelletCol` that it calls are reversed. A bug in those shows up on every gunshot.
- **`CEventDamage` is built on the stack** and added by copy (`CEventGroup::Add` clones it). The response
  (`m_damageResponse`) must be computed before the add.
- **Weapon audio sizes**: `CAEWeaponAudioEntity` is `0xA0` in the repo and `0xA8` in the IDB (see
  [../BinaryMap.md](../BinaryMap.md) struct table); `CAEPedWeaponAudioEntity` is `0xA8` in the repo.
  `CPed::m_weaponAudio` is a `CAEPedWeaponAudioEntity`, so check both before you trust either size. The reversed
  `CAEWeaponAudioEntity` functions are dormant: its `InjectHooks()` is commented out in `InjectHooksMain` (see
  [Audio.md](Audio.md)).
- **`CPedDamageResponseCalculator::IsBleedingWeapon` is registered at `0x4B5C2A`** (`.Reversed = false`). That address is inside `ComputeDamageResponse` (`0x4B5AC0`-`0x4B5CEF`); the function was
  inlined and has no address of its own. Turning the hook on in the F7 menu would patch the middle of
  `ComputeDamageResponse`. Remove the registration (keep the body as an inline helper).
- **(Bug, live) rocket launcher never fires at a target** (the `CanFire` lambda in `Fire`). `CanFire` returns
  `dist <= 8 && !firedBy->GetIsTypePed()`. It is only evaluated when `firedByPed` is set, so it is always false, and
  any call with a `targetEnt` or `targetPosn` returns "not fired". The original (`0x7425B3`) refuses only when the
  shooter is a ped, is **not the player**, and the target (entity first, else position) is within 8 m.
  `CTaskSimpleUseGun::FireGun` passes a target, so AI rocket peds are affected.
- **(Bug, live) wrong next-shot delays in `Fire`**: pistol-class weapons in a vehicle return
  `delay = (task == TASK_SIMPLE_GANG_DRIVEBY)`, the inverse of the original (no delay during a gang
  drive-by). The flamethrower, spraycan and extinguisher return `delay = true`; the original
  sets no delay (`0x742768`).
- **(Bug, live) clip refill in `Fire`** uses the skill-specific clip size; the original
  (`0x742446`) uses the STD entry (`GetWeaponInfo(type, STD)->m_nAmmoClip`). `Reload` does use the skill size.
- **(Bug, live) `FireProjectile`**:
  - a rocket gets its aim direction only when **both** `targetEntity` and `targetPos` are set.
    The original uses the entity if there is one, else the position;
  - the "throw blocked" fallback for grenades and satchels (`0x74193B`) adds the projectile when
    `TestSphereAgainstWorld` finds something. The original adds it when the sphere is **free**, and with force 0
    instead of `force`;
  - the heat-seeking target is kept whenever `PickTargetForHeatSeekingMissile` returns one. The original keeps it
    only when it is the same as `m_LastHSMissileTarget` and the button has been held for over 1.5 s (`0x74178B`),
    otherwise it fires a dumb `WEAPON_ROCKET`.
- **(Bug, live) `CBulletInfo::Update`**:
  - the original destroys the bullet after any hit (`m_bExists = false`, `0x7360D0`); the repo keeps it flying, so a
    sniper bullet can pass through walls and hit several targets;
  - `IsSphereVisible(colPoint.m_vecNormal, 1)` (twice) should test `colPoint.m_vecPoint`;
  - the original clears `m_pCreator` when it is a ped that is no longer valid (`CPed::IsPointerValid`); the repo does
    not, and then calls `info.m_pCreator->AsPed()`.
  - Quirk to keep in mind when fixing: in the original the "hit something" variable is not reset between bullets,
    so after one bullet hits, the later bullets in the same `Update` are destroyed too.
- **(Bug, live) `CPedDamageResponseCalculator::ComputeWillKillPed`** checks
  `CHEAT_NO_ONE_CAN_STOP_US` (index 77). The original (`0x4B3226`, byte `0x96916D` = `m_aCheatsActive + 0x3D`) checks
  index 61, `CHEAT_INFINITE_HEALTH`. If this was meant as a fix (the infinite-health entry has no PC cheat string), it
  should be a `notsa::bugfixes` switch.
- **IDB/tool traps.** `CWorld::TriggerExplosionSectorList` is hooked as a template
  (`CWorld::InjectHooks`) and shows as MISSING in `idb_coverage.py`. `CVehicle::InflictDamage` (`0x6D7C90`) has a body in
  `Vehicle.cpp` but its hook is commented out in `CVehicle::InjectHooks`, so vehicles still take bullet damage through
  the original code.

---

## 7. What is left

Numbers from `python .claude/skills/find-reversing-targets/scripts/idb_coverage.py --class <Class>` (checked
2026-10-14). Bytes are the sizes of functions not yet REVERSED.

| Class | Functions | Reversed | Stub | No-hook | Known | Missing | Bytes left |
|---|---|---|---|---|---|---|---|
| `CWeapon` | 40 | 34 | 6 | 0 | 0 | 0 | 12,498 of 31,604 |
| `CWeaponInfo` | 14 | 10 | 0 | 0 | 0 | 4 | 84 |
| `CProjectileInfo` | 12 | 1 | 11 | 0 | 0 | 0 | 7,592 of 7,918 |
| `CProjectile` | 3 | 0 | 0 | 0 | 0 | 3 | 208 |
| `CBulletInfo` | 4 | 4 | 0 | 0 | 0 | 0 | 0 |
| `CBulletTraces` / `CBulletTrace` | 5 / 1 | 5 / 0 | 0 | 0 | 0 | 0 / 1 | 0 / 20 (inline `Update`) |
| `CShotInfo` | 5 | 5 | 0 | 0 | 0 | 0 | 0 |
| `CExplosion` | 11 | 11 | 0 | 0 | 0 | 0 | 0 |
| `CFire` / `CFireManager` / `CCreepingFire` | 6 / 24 / 3 | 5 / 24 / 3 | 0 | 0 | 0 | 1 / 0 / 0 | 1 / 0 / 0 |
| `CWeaponEffects` | 11 | 8 | 1 | 0 | 0 | 2 | 3,891 (`Render` 3,887) |
| `CEventDamage` | 25 | 20 | 0 | 0 | 1 | 4 | 271 |
| `CEventGunShot` | 14 | 5 | 0 | 0 | 0 | 9 | 203 (inline virtuals) |
| `CPedDamageResponseCalculator` | 7 | 2 | 4 | 0 | 0 | 1 | 1,437 |
| `CTaskSimpleUseGun` | 21 | 17 | 1 | 0 | 2 | 1 | 3,335 (`ProcessPed` 3,011) |
| `CTaskSimpleThrowProjectile` | 10 | 9 | 0 | 0 | 0 | 1 | 30 |
| `CTaskSimpleFight` | 29 | 0 | 5 | 4 | 0 | 20 | 13,871 |
| `CPlayerPed` | 66 | 48 | 15 | 0 | 1 | 2 | 16,020 |

`CWeapon` stubs: `FireInstantHit` 6,177 B, `FireInstantHitFromCar` 1,975, `DoTankDoomAiming` 1,341,
`CheckForShootingVehicleOccupant` 1,156, `DoDriveByAutoAiming` 1,049, `FindNearestTargetEntityWithScreenCoors` 800.
`CProjectileInfo` stubs: `Update` 3,379, `AddProjectile` 2,900, `RemoveProjectile` 551, then seven small ones
(`RemoveIfThisIsAProjectile`, `RemoveAllProjectiles`, `RemoveDetonatorProjectiles`, `RemoveNotAdd`, `Initialise`,
`RemoveFXSystem`, `Shutdown`, 42-185 B each). `CPedDamageResponseCalculator` stubs: `ComputeDamageResponse` 559,
`ComputeWillForceDeath` 418, `AccountForPedDamageStats` 279, `AccountForPedArmour` 180 (a draft body sits after an
early `return` in `PedDamageResponseCalculator.cpp`). `CPlayerPed` weapon-related stubs: `FindWeaponLockOnTarget`
2,270, `FindNextWeaponLockOnTarget` 1,362, `Compute3rdPersonMouseTarget` 1,062, `ProcessWeaponSwitch` 1,012.
Outside these classes: `CVehicle::InflictDamage` (KNOWN, 1,581 B, body exists), `CVehicle::ProcessWeapons`,
`FireHeatSeakingMissile` and `GetPlaneWeaponFiringStatus` (KNOWN), `CVehicle::FireFixedMachineGuns` (STUB, 822 B).

### Suggested order

1. **Fix the live bugs in `Fire`, `FireProjectile`, `CBulletInfo::Update` and `ComputeWillKillPed`** (section 6). They are
   small, and each one is visible in game: an AI ped with a rocket launcher, throwing a grenade against a wall,
   a heat-seeking lock that is shorter than 1.5 s, sniping through a wall, gang drive-by fire rate, flamethrower
   fire rate. Also drop the bogus `IsBleedingWeapon` registration.
2. **`CPedDamageResponseCalculator`**: `AccountForPedArmour` (draft exists), `AccountForPedDamageStats`,
   `ComputeWillForceDeath`, then `ComputeDamageResponse` (inline `IsBleedingWeapon` there). These are pure
   functions of ped state and suit a difftest (`source/difftests/`): copy a ped's health/armour/flags, run both
   versions, compare the `CPedDamageResponse`.
3. **`CProjectileInfo`**: the small helpers, then `RemoveProjectile`, `AddProjectile`, and `Update` last. Also
   `CProjectile`'s constructor and destructors. Test grenades, molotovs, satchels and the detonator, tear gas, rockets
   and heat-seeking rockets.
4. **`FireInstantHit`** (6 KB) together with `CheckForShootingVehicleOccupant`, then `FireInstantHitFromCar` and the
   aiming helpers (`DoDriveByAutoAiming`, `DoTankDoomAiming`, `FindNearestTargetEntityWithScreenCoors`). Validate
   statically with `asm_validate.py`; test with every gun class, shooting peds, car occupants, tyres, glass and
   objects.
5. **Hook `CVehicle::InflictDamage`** after checking the existing body against `0x6D7C90`.
6. **`CWeaponEffects::Render`** (3.9 KB, mostly drawing; an in-game A/B is enough).
7. **`CTaskSimpleUseGun::ProcessPed`** and the `CPlayerPed` targeting functions, then **`CTaskSimpleFight`** (the
   whole melee system, 14 KB, `LoadMeleeData` first).

For every function follow [../ReversingGuide.md](../ReversingGuide.md): read it in IDA, write it, register it, then
run `asm_validate.py` and a difftest or an in-game A/B with the hook toggled in the F7 menu.
