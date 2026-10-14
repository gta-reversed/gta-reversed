# World, Streaming & Pools

The core data structures behind every entity in the game: where entities come from (pools), where they live
(`CWorld` sectors), how their models get into memory (`CStreaming`, `CModelInfo`, `CFileLoader`) and the order in
which the frame updates them (`CGame::Process`). Read this before reversing anything that allocates, finds, iterates,
streams or ticks entities. Build and hook mechanics are in [../Architecture.md](../Architecture.md), general rules in
[../ReversingRules.md](../ReversingRules.md). Collision itself is in [CollisionPhysics.md](CollisionPhysics.md).

Paths are relative to `source/game_sa/` unless stated otherwise. Addresses are only quoted where the repo states them
(`StaticRef`, `RH_ScopedInstall`, `// 0xADDR` comments).

---

## 1. Overview

```
 data files (DEFAULT.DAT, GTA.DAT -> IDE/IPL/IMG/COL)          gta3.img / gta_int.img / extra IMGs
          │ CFileLoader::LoadLevel                                    │ CdStream (async reads, 2 channels)
          ▼                                                           ▼
 CModelInfo::ms_modelInfoPtrs[20000]  <── model ids ──>  CStreaming::ms_aInfoForModel[26316]
 (static model definitions)                              (load state + flags + IMG position per resource)
          │ CEntity::m_nModelIndex                                    │ ConvertBufferToObject -> RwObject on the model info
          ▼                                                           ▼
 Entities: allocated from CPools (operator new -> CPool::New)  ── CWorld::Add ──>  sectors / repeat sectors / LOD lists
          │                                                                         + moving list (non-static physicals)
          ▼
 Per frame: Idle() -> CGame::Process() -> CStreaming::Update ... CWorld::Process ... -> render
```

Key facts:
- Every `CPed` / `CVehicle` / `CObject` / `CBuilding` / `CDummy` lives in a fixed-size `CPool`; the SCM "handle" of an
  entity is `(slotIndex << 8) | slotState`.
- The world is a 120x120 grid of 50-unit sectors (`CSector`: buildings + dummies) plus a 16x16 wrapped grid of
  `CRepeatSector`s (vehicles, peds, objects). Big LOD buildings go into 30x30 LOD lists.
- Model ids `0..19999` are DFFs; the same id space is extended to TXD/COL/IPL/DAT/IFP/RRR/SCM for streaming.

---

## 2. Key classes: the entity hierarchy

```
CPlaceable (0x18)                       Entity/Placeable.h
└─ CEntity (0x38)                       Entity/Entity.h
   ├─ CBuilding (0x38)                  Entity/Building.h
   │  └─ CAnimatedBuilding (0x38)
   ├─ CDummy (0x38)                     Entity/Dummy/
   │  ├─ CDummyObject (0x38)
   │  └─ CDummyPed (0x38)
   └─ CPhysical (0x138)                 Entity/Physical.h
      ├─ CPed (0x79C)                   Entity/Ped/          -> Peds.md
      │  ├─ CPlayerPed (0x7A4)  ├─ CCivilianPed (0x79C)  ├─ CCopPed (0x7C4)  └─ CEmergencyPed (0x79C)
      ├─ CVehicle (0x5A0)               Entity/Vehicle/      -> Vehicles.md
      │  ├─ CAutomobile (0x988)
      │  │  ├─ CHeli (0xA18)  ├─ CPlane (0xA04)  ├─ CMonsterTruck (0x99C)  ├─ CQuadBike (0x9BC)  └─ CTrailer (0x9F4)
      │  ├─ CBike (0x814)  └─ CBmx (0x838)
      │  ├─ CBoat (0x7E8)
      │  └─ CTrain (0x6AC)
      └─ CObject (0x17C)                Entity/Object/
         ├─ CCutsceneObject (0x19C)  ├─ CHandObject (0x18C)  └─ CProjectile (0x17C)
```

Sizes are the `VALIDATE_SIZE` values in each header. `CCamera` also derives from `CPlaceable`.

### CPlaceable (`Entity/Placeable.h`)
- `m_placement` (`CSimpleTransform`: position + heading) and `m_matrix` (`CMatrixLink*`, may be null).
- `GetPosition()` returns the matrix position if a matrix exists, otherwise `m_placement.m_vPosn`. **Always use it**
  instead of touching either member. Also `GetRight/GetForward/GetUp`, `SetPosn`, `SetHeading`, `GetHeading`,
  `IsPointInRange`, `IsEntityInRange` (NOTSA).
- Matrices come from a fixed array of `NUM_MATRICES_TO_CREATE = 900` (`InitMatrixArray`, `AllocateMatrix`,
  `AllocateStaticMatrix`).

### CEntity (`Entity/Entity.h`)
| Member | Notes |
|---|---|
| `m_pRwObject` (private) | `RpClump`/`RpAtomic`; use `GetRwObject()/GetRpClump()/GetRpAtomic()` |
| `m_nFlags` bitfield | `m_bUsesCollision`, `m_bIsStatic`, `m_bIsVisible`, `m_bIsBIGBuilding`, `m_bStreamingDontDelete`, `m_bRemoveFromWorld`, `m_bImBeingRendered`, `m_bOffscreen`, `m_bIsStaticWaitingForCollision`, `m_bDontStream`, `m_bIsTempBuilding`, `m_bIsProcObject`, `m_bUnimportantStream`, `m_bTunnel`... (32 bits, list mirrors MTA's `CEntitySA.h`) |
| `m_nRandomSeed` | `uint16` |
| `m_nModelIndex` | `uint16`; index into `CModelInfo::ms_modelInfoPtrs`; use `GetModelIndex()/GetModelId()` |
| `m_pReferences` | `CReference*` chain of registered pointers (see `RegisterReference`, `SafeRegisterRef`) |
| `m_pStreamingLink` | `CLink<CEntity*>*` into `CStreaming::ms_rwObjectInstances` |
| `m_ScanCode` | compared to `CWorld::ms_nCurrentScanCode` (see 3.3) |
| `m_IplIndex`, `m_AreaCode` | IPL the entity came from, interior area (`IsInCurrentArea()` also accepts `AREA_CODE_13`) |
| `m_pLod` / `m_nLodIndex` | LOD entity (or `-1` index while loading), `m_NumLodChildren`, `m_NumLodChildrenRendered` |
| `m_info` | `CEntityInfo { eEntityType m_nType : 3; eEntityStatus m_nStatus : 5; }` |

`eEntityType` (`Enums/eEntityType.h`): `NOTHING=0, BUILDING, VEHICLE, PED, OBJECT, DUMMY, NOTINPOOLS`.
`eEntityStatus` (same folder): `PLAYER=0, PLAYER_PLAYBACK_FROM_BUFFER, SIMPLE, PHYSICS, ABANDONED, WRECKED,
TRAIN_MOVING, TRAIN_NOT_MOVING, REMOTE_CONTROLLED, FORCED_STOP, IS_TOWED, IS_SIMPLE_TOWED, GHOST`.

Type/status helpers: `GetType()/SetType()`, `GetIsTypePed/Vehicle/Object/Building/Dummy()`, `GetIsTypePhysical()`
(type strictly between BUILDING and DUMMY, `0x4DA030`), `GetStatus()/SetStatus()`, `TreatAsPlayerForCollisions()`.
Casts (NOTSA, unchecked `reinterpret_cast`): `AsPhysical/AsVehicle/AsAutomobile/AsBike/AsBmx/AsBoat/AsTrain/AsPed/
AsObject/AsBuilding/AsDummy`. Key virtuals: `Add()`, `Add(const CRect&)`, `Remove()`, `SetModelIndex`,
`CreateRwObject`, `DeleteRwObject`, `GetBoundRect`, `ProcessControl`, `ProcessCollision`, `ProcessShift`,
`TestCollision`, `Teleport`, `PreRender`, `Render`, `SetupLighting`, `FlagToDestroyWhenNextProcessed`.

### CPhysical (`Entity/Physical.h`)
- `m_nPhysicalFlags` / `physicalFlags` bitfield (mirrored by the `ePhysicalFlags` enum at the top of the header):
  `bApplyGravity`, `bCollidable`, `bDisableMoveForce`, `bInfiniteMass`, `bSubmergedInWater`, `bOnSolidSurface`,
  `bBulletProof`, `bFireProof`, `bCollisionProof`, `bMeleeProof`, `bInvulnerable`, `bExplosionProof`,
  `bAttachedToEntity`, `bTouchingWater`, `bCanBeCollidedWith`, `bRenderScorched`...
- Dynamics: `m_vecMoveSpeed`, `m_vecTurnSpeed`, friction speeds, `m_vecForce`, `m_vecTorque`, `m_fMass`,
  `m_fTurnMass`, `m_fAirResistance`, `m_fElasticity`, `m_fBuoyancyConstant`, `m_vecCentreOfMass`.
- World links: `m_pCollisionList` (`CEntryInfoList`: one `CEntryInfoNode` per repeat-sector list the entity is in)
  and `m_pMovingList` (node in `CWorld::ms_listMovingEntityPtrs`). `AddToMovingList()/RemoveFromMovingList()`.
- Collision bookkeeping: `m_apCollidedEntities[6]`, `m_nNumEntitiesCollided`, `m_pDamageEntity`, `m_fDamageIntensity`.

### Leaves
- `CVehicle`: `m_nVehicleType` / `m_nVehicleSubType`. Only `IsAutomobile()`, `IsBike()`, `IsBoat()`, `IsTrain()` are
  meaningful on the type; use `IsSubHeli()`, `IsSubPlane()`... for the leaf class ([Vehicles.md](Vehicles.md) §1.1).
- `CPed`: `m_nPedType`, `m_pVehicle` + `bInVehicle` (use `GetVehicleIfInOne()`), `IsPlayer()`, `IsCop()`.
- `CBuilding` / `CDummy` add no data (0x38 like `CEntity`). Dummies are placeholders for objects (`CDummyObject`).

---

## 3. CWorld (`World.h`, `World.cpp`)

### 3.1 Grids and lists
| Constant / array | Value / address | Contents |
|---|---|---|
| `MAX_WORLD_UNITS` | 6000 (map is -3000..3000, `WORLD_BOUNDS`) | |
| `ms_aSectors` | `0xB7D0B8`, `mdarray<CSector,120,120>` | `CSector` (`Sector.h`, 0x8): `Buildings` (single-link list), `Dummies` (double-link) |
| `ms_aRepeatSectors` | `0xB992B8`, 16x16 | `CRepeatSector` (`RepeatSector.h`, 0xC): `Vehicles`, `Peds`, `Objects` (double-link) |
| `ms_aLodPtrLists` | `0xB99EB8`, 30x30 (200-unit cells) | entities with `m_bIsBIGBuilding` |
| `ms_listMovingEntityPtrs` | `0xB9ACC8` | every non-static `CPhysical` that is processed each frame |
| `ms_listObjectsWithControlCode` | `0xB9ACCC` | objects whose `ProcessControlLogic` runs each frame |
| `ms_nCurrentScanCode` | `0xB7CD78` | see 3.3 |
| `Players[MAX_PLAYERS=2]`, `PlayerInFocus` | `0xB7CD98`, `0xB7CD74` | `CPlayerInfo` |
| `pIgnoreEntity`, `bIncludeDeadPeds`, `bIncludeCarTyres`, `bIncludeBikers` | `0xB7CD68`... | line-test options (`ResetLineTestOptions`) |

Coordinate helpers in `World.h`: `GetSectorX/Y(float)` (0..119), `GetSectorfX/Y`, `GetSectorPosX/Y`,
`GetLodSectorX/Y`, `IsInWorldBounds`. Accessors: `GetSector(x,y)` clamps to the grid (`0x407260`),
`GetRepeatSector(x,y)` wraps with `abs(coord) % 16` (`0x4072A0`; the original used `&`), `GetLodPtrList(x,y)` asserts
(`0x4072C0`).

### 3.2 Add / Remove
- `CWorld::Add` (`0x563220`): `UpdateRwMatrix`, `UpdateRwFrame`, virtual `entity->Add()`, then `AddToMovingList()` if
  it is neither building nor dummy and not static.
- `CWorld::Remove` (`0x563280`): virtual `entity->Remove()` + `RemoveFromMovingList()` for physicals. Does **not**
  delete the entity.
- `CEntity::Add(const CRect&)`: clamps the bound rect to the map, then BIG buildings go to the LOD lists, everything
  else into the sector lists by type (buildings/dummies -> `CSector`, vehicles/peds/objects -> `CRepeatSector`).
- `CPhysical::Add` (`0x544A30`) has its body under `#if 0` and is hooked with `.Reversed = false`, so the original
  code runs. `CPhysical::Remove` (`0x5424C0`) is reversed and walks `m_pCollisionList`. `CPhysical::RemoveAndAdd`
  (`0x542560`) is also `.Reversed = false`.
- Moving an entity: after changing its position call `RemoveAndAdd()` (physicals) or `CWorld::Remove` + `CWorld::Add`.

### 3.3 Scan codes
Every sector query first calls `AdvanceCurrentScanCode()` (`0x4072E0`; wraps to 1 after `ClearScanCodes()` at
65535). Large entities sit in several sectors, so each list walk skips `entity->IsScanCodeCurrent()` and calls
`SetCurrentScanCode()` on the first visit. If you write a new sector walk, do the same or you will process entities
twice.

### 3.4 Queries
All take `bool buildings, vehicles, peds, objects, dummies` filters and walk the needed sectors; each has a templated
`...SectorList(PtrListType&, ...)` worker.
- `ProcessLineOfSight(origin, target, outColPoint, outEntity, ...)` (`0x56BA00`): walks sectors along the line
  (Bresenham-like) and keeps the closest hit. `GetIsLineOfSightClear`, `ProcessVerticalLine`,
  `ProcessVerticalLine_FillGlobeColPoints`, `TestSphereAgainstWorld`.
- `FindObjectsInRange`, `FindObjectsOfTypeInRange`, `FindObjectsKindaColliding`, `FindObjectsIntersectingCube`,
  `FindObjectsIntersectingAngledCollisionBox`, `FindMissionEntitiesIntersectingCube`, `FindNearestObjectOfType`,
  `FindLodOfTypeInRange` (out-params: `int16* outCount, int16 maxCount, CEntity** outEntities`).
- Ground: `FindGroundZForCoord`, `FindGroundZFor3DCoord`, `FindRoofZFor3DCoord`, `FindLowestZForCoord`,
  `PutToGroundIfTooLow`, `AddGroundZToCoord`.
- Area ops: `ClearCarsFromArea`, `ClearPedsFromArea`, `ClearExcitingStuffFromArea`, `TriggerExplosion`,
  `SetPedsOnFire`, `ExtinguishAllCarFiresInArea`, `CallOffChaseForArea`, `RemoveReferencesToDeletedObject`.
- Iteration helpers (NOTSA, `World.h`): `IterateSectors(minX, minY, maxX, maxY, fn)`,
  `IterateSectorsOverlappedByRect(CRect, fn)`, `IterateLodSectorsOverlappedByRect(CRect, fn)`; `fn(x, y)` returns
  `false` to stop. `CRect{point, radius}` builds the rect for a radius query.

### 3.5 `CWorld::Process` (`0x5684A0`)
1. Add global events to every moving ped and every `CPedGroup`, process inform queues, flush the event group
   ([Events.md](Events.md) §3.1).
2. Every 64 frames `CReferences::PruneAllReferencesInWorld()`.
3. Cutscene-only mode (`bProcessCutsceneOnly`): update cutscene objects and return.
4. `UpdateAnim()` on moving entities.
5. `ProcessControl()` on each moving entity; entities flagged `m_bRemoveFromWorld` are removed (peds through
   `CPopulation::RemovePed`, others `Remove` + `delete`); static ones leave the moving list. Then a second pass with
   `bForceProcessControl = true` for `GetWasPostponed()` entities.
6. `ProcessControlLogic()` on `ms_listObjectsWithControlCode`.
7. Collision: 1 + 4 passes of `ProcessCollision()` on entities not in a safe position, then mark stuck ones; then two
   `ProcessShift()` passes (`bSecondShift`). In replay playback only the RW matrices are updated.
8. `CVehicleRecording::SaveOrRetrieveDataForThisFrame`, `g_ikChainMan.Update`, `ProcessAttachedEntities`, then ped
   post-collision processing (`CPedIntelligence::ProcessAfterProcCol`, `CPlayerInfo::Process`, ...; see
   [Peds.md](Peds.md) §3).

---

## 4. Pools & handles

### 4.1 `CPool<T, S = T>` (`Core/Pool.h`, size 0x14)
- `T` is the common base returned by the API; `S` the widest type stored (slot size = `sizeof(S)`).
- Members: `m_Storage`, `m_SlotState` (1 byte per slot: `Ref : 7`, `IsEmpty : 1`), `m_Capacity`, `m_LastFreeSlot`,
  `m_OwnsAllocations`, `m_DealWithNoMemory`. Terminology table at the top of the file (R* `JustIndex` = our index,
  R* `Index` = our id, `Ref` = handle, `Size` = capacity).
- API: `New()`, `NewAt(ref)`, `Delete(obj)`, `GetAt(idx)` (null if free), `GetIndex(obj)`, `GetRef(obj)` =
  `(idx << 8) | slotStateByte` (`0x424160`), `GetAtRef(ref)` (validates the state byte, returns null on stale handles),
  `IsObjectValid(obj)` (`0x5A1CD0`), `IsFreeSlotAtIndex`, `GetNoOfUsedSpaces` (slow), `GetSize`.
- Iteration (NOTSA): `GetAllValid()` (range of `T&`) and `GetAllValidWithIndex()` (range of `tuple<int32, T&>`).
- Debug fills: `0xFD` never used, `0xCD` freshly `New`ed, `0xDD` deleted. Recognising these in a crash dump tells you
  "uninitialised" vs "use after free".

### 4.2 CPools (`Pools.cpp`, `Pools/Pools.h`)
Pool pointers are `StaticRef`s at the top of `Pools.cpp` (e.g. `ms_pPedPool` `0xB74490`, `ms_pVehiclePool`
`0xB74494`, `ms_pBuildingPool` `0xB74498`, `ms_pObjectPool` `0xB7449C`, `ms_pDummyPool` `0xB744A0`).
`CPools::Initialise` (`0x550F10`) creates them:

| Accessor | Class (`Pools/*.h`) | `T`, `S` | Capacity |
|---|---|---|---|
| `GetPtrNodeSingleLinkPool()` | `CPtrNodeSingleLinkPool` | `CPtrNodeSingleLink<void*>` | 70000 |
| `GetPtrNodeDoubleLinkPool()` | `CPtrNodeDoubleLinkPool` | `CPtrNodeDoubleLink<void*>` | 3200 |
| `GetEntryInfoNodePool()` | `CEntryInfoNodePool` | `CEntryInfoNode` | 500 |
| `GetPedPool()` | `CPedPool` | `CPed`, `CCopPed` | 140 |
| `GetVehiclePool()` | `CVehiclePool` | `CVehicle`, `CHeli` | 110 |
| `GetBuildingPool()` | `CBuildingPool` | `CBuilding` | 13000 |
| `GetObjectPool()` | `CObjectPool` | `CObject`, `CCutsceneObject` | 350 |
| `GetDummyPool()` | `CDummyPool` | `CDummy` | 2500 |
| `GetColModelPool()` | `CColModelPool` | `CColModel` | 10150 |
| `GetTaskPool()` | `CTaskPool` | `CTask`, `CTaskSimpleSlideToCoord` | 500 |
| `GetEventPool()` | `CEventPool` | `CEvent`, `CEventDamage` | 200 |
| `GetPointRoutePool()` / `GetPatrolRoutePool()` / `GetNodeRoutePool()` | | | 64 / 32 / 64 |
| `GetTaskAllocatorPool()` | `CTaskAllocatorPool` | `CTaskAllocator`, `char[32]` | 16 |
| `GetPedIntelligencePool()` | `CPedIntelligencePool` | `CPedIntelligence` | 140 |
| `GetPedAttractorPool()` | `CPedAttractorPool` | `CPedAttractor` | 64 |

Other pools outside `CPools`: `CTxdStore::ms_pTxdPool`, `CQuadTreeNode<void*>::ms_pQuadTreeNodePool`
(`Core/QuadTreeNode.h`), `CVehicleModelInfo::CVehicleStructure::m_pInfoPool`, `CIplDefPool`, the col/IPL store
pools, `CStuntJumpManager::mp_poolStuntJumps`, the custom pipeline pools.

### 4.3 Allocation and handles
- Entities allocate through class `operator new`: `CPed::operator new` -> `GetPedPool()->New()`, and the
  `operator new(size, int32 poolRef)` overload -> `NewAt(ref)` used when loading saves / script-created entities.
  Same for `CVehicle` and `CObject`. So `new CAutomobile(...)` is pool allocation; never `malloc` an entity.
- Script handles: `CPools::GetPedRef/GetPed` (`0x54FF60` / `0x54FF90`), `GetVehicleRef/GetVehicle`,
  `GetObjectRef/GetObject`.
- Validity: `IsPedPointerValid`, `IsVehiclePointerValid`, `IsObjectPointerValid`, `IsEntityPointerValid`, or
  generically `CPools::IsValidPointer(ptr)`.
- `notsa::PoolOf<T>()` (`Pools/Pools.h`) maps a type to its pool, including derived types (`PoolOf<CAutomobile>()` ->
  vehicle pool); the `notsa::PooledType` concept constrains it.

---

## 5. Streaming (`Streaming.h/.cpp`, `StreamingInfo.h`, `CdStreamInfo.h`)

### 5.1 Resource id space (`Streaming.h`, `constants.h`)
| Range start | Value | Count | Store |
|---|---|---|---|
| `RESOURCE_ID_DFF` | 0 | 20000 | `CModelInfo` |
| `RESOURCE_ID_TXD` | 20000 | 5000 | `CTxdStore` |
| `RESOURCE_ID_COL` | 25000 | 255 | `CColStore` |
| `RESOURCE_ID_IPL` | 25255 | 256 | `CIplStore` |
| `RESOURCE_ID_DAT` | 25511 | 64 | path nodes |
| `RESOURCE_ID_IFP` | 25575 | 180 | `CAnimManager` blocks |
| `RESOURCE_ID_RRR` | 25755 | 475 | vehicle recordings |
| `RESOURCE_ID_SCM` | 26230 | 82 | streamed scripts |
| list sentinels | 26312..26315 | 4 | head/tail of the loaded and requested lists |

`RESOURCE_ID_TOTAL = 26316` = size of `ms_aInfoForModel`. Convert with `TXDToModelId(slot)`, `ModelIdToTXD(id)`, etc.
and classify with `IsModelDFF/TXD/...` / `GetModelType(id)`.

### 5.2 CStreamingInfo (0x14)
`m_NextIndex` / `m_PrevIndex` (int16 indices into the array, forming the loaded/requested doubly linked lists),
`m_NextIndexOnCd`, `m_Flags`, `m_ImgID` (index into `CStreaming::ms_files`), `m_CDOffset` and `m_CDSize` (in 2048-byte
sectors, `STREAMING_SECTOR_SIZE`), `m_LoadState`.
- `eStreamingLoadState`: `NOT_LOADED=0, LOADED=1, REQUESTED=2, READING=3, FINISHING=4` (second half of a big model).
- `eStreamingFlags`: `GAME_REQUIRED=0x2`, `MISSION_REQUIRED=0x4`, `KEEP_IN_MEMORY=0x8`, `PRIORITY_REQUEST=0x10`,
  `LOADING_SCENE=0x20`, `DONTREMOVE_IN_LOADSCENE` = all of them. `IsRequiredToBeKept()` = game/mission/keep.

### 5.3 Important CStreaming state
`ms_aInfoForModel` (`0x8E4CC0`), `ms_files[TOTAL_IMG_ARCHIVES=8]` (`0x8E48D8`, `tStreamingFileDesc`: name, CdStream
handle), `ms_channel[2]` (`0x8E4A60`, `tStreamingChannel`: up to 16 model ids, `LoadStatus` IDLE/READING/STARTED/ERR),
`ms_pStreamingBuffer[2]`, `ms_memoryAvailable` / `ms_memoryUsedBytes`, `ms_numModelsRequested`,
`ms_numPriorityRequests`, `ms_bLoadingBigModel`, `ms_disableStreaming`, `ms_rwObjectInstances` (entities with an RW
object, for LRU deletion), `ms_pedsLoaded` / `ms_vehiclesLoaded` (population streaming).

### 5.4 Lifecycle of a model
1. `RequestModel(id, flags)` (`0x4087E0`): NOT_LOADED -> add to the request list and recursively request
   dependencies (DFF -> its TXD and IFP; TXD -> parent TXD). Already requested -> only adds `PRIORITY_REQUEST`. Already
   loaded -> moves it to the front of the loaded list (LRU).
2. `LoadRequestedModels()` (`0x40E3A0`) alternates the two channels: `ProcessLoadingChannel` on a busy channel,
   `RequestModelStream` on an idle one, which picks files from the request list in CD order (`GetNextFileOnCd`) and
   issues `CdStreamRead`.
3. `ProcessLoadingChannel` -> `ConvertBufferToObject(buffer, id)` builds a memory `RwStream` and dispatches on
   `GetModelType`: DFF -> clump/atomic on the model info, TXD -> `CTxdStore`, COL -> `CColStore`, IPL -> `CIplStore`,
   DAT -> paths, IFP -> anims, RRR -> recordings, SCM -> streamed scripts. Big files go through `FINISHING` +
   `FinishLoadingLargeFile`.
4. `LoadAllRequestedModels(bOnlyPriorityRequests)` loads synchronously (flushes channels, loops until empty). This is
   the "I need it now" call used by scripts and init (27 uses of `LoadAllRequestedModels(false)` in the repo).
5. Release: `SetModelIsDeletable(id)` / `SetModelTxdIsDeletable` / `SetMissionDoesntRequireModel` clear the keep
   flags; `RemoveModel(id)` unloads now; `MakeSpaceFor`, `RemoveLeastUsedModel`, `DeleteLeastUsedEntityRwObject`,
   `DeleteRwObjectsBehindCamera` free memory under pressure.

Typical code: `CStreaming::RequestModel(id, STREAMING_KEEP_IN_MEMORY); CStreaming::LoadAllRequestedModels(false);
if (CStreaming::IsModelLoaded(id)) { ... }`.

### 5.5 `CStreaming::Update` (`0x40E670`)
Returns if paused. Unless streaming is disabled: camera >= 50 above ground -> `AddLodsToRequestList`, otherwise
`AddModelsToRequestList` (sector based, around the camera). Every 128 frames refresh `m_bBoatsNeeded`. If outside
and not in a cutscene/replay: `StreamVehiclesAndPeds_Always`, and if not `IsVeryBusy()` `StreamVehiclesAndPeds` +
`StreamZoneModels`. Then `LoadRequestedModels()`, `CColStore` / `CIplStore` `LoadCollision` / `LoadIpls` +
`Ensure...InMemory` around the player (or the remote-controlled vehicle), and `PurgeRequestList()` if enabled.

### 5.6 IMG archives and CdStream
- `InitImageList()` adds `MODELS\GTA3.IMG` and `MODELS\GTA_INT.IMG`; other `IMG` lines from the level files are added
  by `CFileLoader::LoadLevel` through `AddImageToList`.
- `Init2()` (`0x5B8AD0`, called from `CFileLoader::LoadLevel` before the first IPL) inits all infos and lists, calls
  `LoadCdDirectory()` (reads each archive's directory and fills `m_CDOffset/m_CDSize/m_ImgID`) and allocates the two
  streaming buffers (size = largest file).
- `CdStreamInfo.h/.cpp`: `CdStreamOpen` (`0x4067B0`), `CdStreamRead` (`0x406A20`), `CdStreamGetStatus`, `CdStreamSync`,
  `CdStreamThread` (`0x406560`). `CdStreamPos` packs `Offset : 24` (sectors) and `FileID : 8`. `eCdStreamStatus`:
  `READING_SUCCESS=0, WAITING_TO_READ=250, READING_FAILURE=254, READING=255`.

---

## 6. Model info and file loading

### 6.1 Model info (`Models/`)

- `CModelInfo::ms_modelInfoPtrs` (`StaticRef<CBaseModelInfo*[20000]>(0xA9B0C8)`). Access with
  `CModelInfo::GetModelInfo(id)`, `GetPedModelInfo(id)`, `GetVehicleModelInfo(id)`, or `entity->GetModelInfo()`; by
  name with `GetModelInfo(const char*, int32* outIndex)`.
- Storage: the concrete infos live in fixed `CStore`s (`ms_atomicModelInfoStore` 14000, damage atomic 70, LOD atomic
  1, time 169, LOD time 1, weapon 51, clump 92, vehicle 212, ped 278, 2dfx 100). `AddAtomicModel(id)` etc. take a
  store slot and register the pointer.
- Classes and `ModelInfoType` (`Models/BaseModelInfo.h`): `ATOMIC=1, TIME=3, WEAPON=4, CLUMP=5, VEHICLE=6, PED=7,
  LOD=8`.

```
CBaseModelInfo (0x20)        m_nKey (name hash), m_nRefCount, m_nTxdIndex, m_nAlpha, 2dfx, m_nObjectInfoIndex,
│                            m_nFlags (bIsLod, bIsRoad, bDrawLast, bDontCastShadowsOn, ...), m_pColModel,
│                            m_fDrawDistance, m_pRwObject
├─ CAtomicModelInfo (0x20)
│  ├─ CDamageAtomicModelInfo (0x24)
│  ├─ CTimeModelInfo (0x24)          (CTimeInfo: on/off hours)
│  └─ CLodAtomicModelInfo (0x24)
│     └─ CLodTimeModelInfo (0x28)
└─ CClumpModelInfo (0x24)
   ├─ CPedModelInfo (0x44)
   ├─ CVehicleModelInfo (0x308)
   └─ CWeaponModelInfo (0x28)
```

`GetModelType()` is pure virtual; `AsAtomicModelInfoPtr()` etc. are virtual; `AsVehicleModelInfoPtr()`,
`AsPedModelInfoPtr()`, `AsWeaponModelInfoPtr()` are unchecked casts. Vehicle-class tests by model id:
`CModelInfo::IsCarModel/IsBoatModel/IsHeliModel/IsBikeModel/...`.

### 6.2 File loading (`FileLoader.h/.cpp`)

`CGame::Initialise` (`0x53BC80`): `Init1` -> `CFileLoader::LoadLevel("DATA\\DEFAULT.DAT")` -> `LoadLevel(datFile)`
(GTA.DAT) -> `Init2`. `CFileLoader::LoadLevel` (`0x5B9030`) reads lines:
- `TEXDICTION` -> load a TXD into the current dictionary.
- `IPL` -> before the first IPL: `MatchAllModelStrings`, `CObjectData::Initialise("DATA\\OBJECT.DAT")`,
  `CVehicleModelInfo::SetupCommonData`, `CStreaming::Init2`, `CColStore::LoadAllBoundingBoxes`,
  `ConvertAnimFileIndex` on all model infos; then `LoadScene(path)`.
- `IMG` -> `CStreaming::AddImageToList` (skips `GTA_INT.IMG`, already added), `COLFILE` -> `LoadCollisionFile`,
  `MODELFILE` -> `LoadAtomicFile`, `HIERFILE` -> `LoadClumpFile`, `IDE` -> `LoadObjectTypes`, `EXIT` stops.

`LoadObjectTypes` (IDE) sections: `objs, tobj, weap, hier, anim, cars, peds, path, 2dfx, txdp` -> `LoadObject`,
`LoadTimeObject`, `LoadWeaponObject`, `LoadClumpObject`, `LoadAnimatedClumpObject`, `LoadVehicleObject`,
`LoadPedObject`, path nodes, `Load2dEffect`, `LoadTXDParent`. `LoadScene` (text IPL) sections:
`path, inst, mult, zone, cull, occl, grge, enex, pick, cars, jump, tcyc, auzo` -> `LoadObjectInstance` (creates
`CBuilding` / `CDummy` / `CObject` and `CWorld::Add`s it), `LoadZone`, `LoadCullZone`, `LoadOcclusionVolume`,
`LoadGarage`, `LoadEntryExit`, `LoadPickup`, `LoadCarGenerator`, `LoadStuntJump`, `LoadTimeCyclesModifier`,
`LoadAudioZone`. Binary IPLs streamed from the IMG go through `CIplStore`. `ms_line` (`0xB71848`) is a static
512-byte line buffer; see the comment in `LoadLevel` on why paths are copied.

Init order relevant here (`CGame::Init1`): `CPools::Initialise` -> `CPlaceable::InitMatrixArray` -> ...
`CQuadTreeNode<void*>::InitPool` -> ... `CWorld::Initialise` -> `CModelInfo::Initialise` -> `CStreaming::InitImageList`
-> `CStreaming::ReadIniFile`. `CGame::Init2` then loads initial vehicles/peds/weapons with `LoadAllRequestedModels`.

---

## 7. Frame flow (main loop order)

`Idle()` (`source/app/app_game.cpp`, `0x53E920`, locked hook): `CTimer::Update`, `CSprite2d::InitPerFrame`,
`CFont::InitPerFrame`, `CPointLights::NumLights = 0`, **`CGame::Process()`**, `AudioEngine.Service()`,
`SetLightsWithTimeOfDayColour`, then (if not in the menu/fade) `CRenderer::ConstructRenderList`,
`CRenderer::PreRender`, `CWorld::ProcessPedsAfterPreRender`, shadows, mirrors, `RenderScene`, `RenderEffects`,
motion blur, `Render2dStuff`, then `RenderMenus`, fade, HUD, fonts, the ImGui draw loop and the RW present. Rendering
details are in [Rendering.md](Rendering.md).

`CGame::Process()` (`0x53BEE0`):
1. `CPad::UpdatePads()`, `g_LoadMonitor.BeginFrame()`
2. `CStreaming::Update()` (timed; the time decides population work below)
3. `CCutsceneMgr::Update()`
4. `FrontEndMenuManager.Process()` (unless a cutscene is processing or code is paused)
5. `CTheZones::Update()`, `CCover::Update()`, `CAudioZones::Update(false, camPos)`, `CWindModifiers::Number = 0`
6. **If not paused** (`!CTimer::GetIsPaused()`):
   1. `CSprite2d::SetRecipNearClip`, `CSprite2d::InitPerFrame`, `CFont::InitPerFrame`
   2. `CCheat::DoCheats`, `CClock::Update`, `CWeather::Update`
   3. `CTheScripts::Process` (SCM runs before the world)
   4. `CCollision::Update`, `ThePaths.UpdateStreaming(false)`
   5. `CTrain::UpdateTrains`, `CHeli::UpdateHelis`, `CDarkel::Update`, `CSkidmarks::Update`, `CGlass::Update`,
      `CWanted::UpdateEachFrame`, `CCreepingFire::Update`, `CSetPieces::Update`, `gFireManager.Update`
   6. `CPopulation::Update(bool)` (full update only if streaming took < 4 ms)
   7. `CWeapon::UpdateWeapons`
   8. `CTheCarGenerators::Process` (not during cutscenes), `CCranes::UpdateCranes` (not in replay playback)
   9. `CClouds::Update`, `CMovingThings::Update`, `CWaterCannons::Update`, `CUserDisplay::Process`, `CReplay::Update`
   10. **`CWorld::Process()`** (entity control, collision, shift; see 3.5), `g_LoadMonitor.EndFrame()`
   11. Unless `CTimer::IsSkipProcessThisFrame()`: `CPickups::Update`, `CCarCtrl::PruneVehiclesOfInterest`,
       `CGarages::Update`, `CEntryExitManager::Update`, `CStuntJumpManager::Update`, `CBirds::Update`,
       `CSpecialFX::Update`, `CRopes::Update`
   12. `CPostEffects::Update`, `CTimeCycle::Update`, `CPopCycle::Update`
   13. `g_InterestingEvents.ScanForNearbyEntities()` (if enabled)
   14. `TheCamera.Process()` (or `ProcessFade()` in replay): the camera runs **after** the world
   15. `CCullZones::Update`; `CGameLogic::Update`, `CGangWars::Update` (not in playback)
   16. `CConversations::Update`, `CPedToPlayerConversations::Update`, `CBridge::Update`
   17. `CCoronas::DoSunAndMoon`, `CCoronas::Update`, `CShadows::UpdatePermanentShadows`
   18. `CPlantMgr::Update(camPos)`, `CCustomBuildingRenderer::Update`
   19. `CStencilShadows::RenderBuffer(...)`, `CStencilShadows::Process(camPos)`
   20. Not in playback: `CCarCtrl::GenerateRandomCars` (only if < 4 ms used), `CRoadBlocks::GenerateRoadBlocks`,
       `CCarCtrl::RemoveDistantCars`, `CCarCtrl::RemoveCarsIfThePoolGetsFull`
   21. `g_fx.Update`, `g_breakMan.Update`, `g_interiorMan.Update`, `g_procObjMan.Update`, `g_waterCreatureMan.Update`
7. `CWaterLevel::PreRenderWater()` (always)

---

## 8. Common helpers (use these instead of raw access)

| Need | Use | Instead of |
|---|---|---|
| Player ped / vehicle / position | `FindPlayerPed()`, `FindPlayerVehicle()`, `FindPlayerCoors()`, `FindPlayerInfo()`, `FindPlayerEntity()`, `FindPlayerWanted()` (bottom of `World.h`, `World_Helper.cpp`) | `CWorld::Players[CWorld::PlayerInFocus].m_pPed` |
| Entity position | `GetPosition()`, `GetPosition2D()`, `GetForward()`, `GetHeading()` | `m_matrix->GetPosition()` (null for matrix-less entities) |
| Static test | `GetIsStatic()` (`m_bIsStatic \|\| m_bIsStaticWaitingForCollision`, `0x4633E0`) | reading `m_bIsStatic` |
| Type test / cast | `GetIsTypePed()`, `GetIsTypePhysical()`, `AsPed()`, `AsVehicle()`, `veh->IsBike()` | `m_info.m_nType == 3`, C casts |
| Model info | `entity->GetModelInfo()`, `CModelInfo::GetVehicleModelInfo(id)` | indexing `ms_modelInfoPtrs` |
| Collision model | `GetColModel()`, `GetBoundRadius()`, `GetBoundCentre()`, `GetBoundRect()` | `GetModelInfo()->m_pColModel` |
| Iterate all peds/vehicles | `for (auto& ped : GetPedPool()->GetAllValid())`, `GetAllValidWithIndex()` | index loops with `GetAt(i)` + null checks |
| Pointer still alive? | `IsPedPointerValid`, `IsVehiclePointerValid`, `CPools::IsValidPointer` | comparing with null |
| Keep a pointer to an entity | `CEntity::SetEntityReference`, `SafeRegisterRef`/`SafeCleanUpRef`, `ClearReference` | raw pointers (they dangle when the entity is deleted) |
| Area queries | `CWorld::IterateSectorsOverlappedByRect(CRect{pos, r}, fn)`, `FindObjectsInRange` | manual sector index maths |
| Model ready? | `CStreaming::IsModelLoaded(id)`, `GetInfo(id).IsLoaded()` | `ms_aInfoForModel[id].m_LoadState == 1` |
| Ground height | `CWorld::FindGroundZForCoord`, `FindGroundZFor3DCoord` | own line tests |

More engine idioms (inlined helpers as IDA shows them) are in [../ai/TipsAndTricks.md](../ai/TipsAndTricks.md) §1.

---

## 9. Gotchas

- **Status bits are shifted in the original.** `m_nStatus` is bits 3..7 of the byte; in disassembly the compared value
  is usually `status << 3` (comment on `CEntityInfo` in `Entity.h`).
- **Unchecked casts.** `AsPed()` / `AsVehicle()` / `AsVehicleModelInfoPtr()` are `reinterpret_cast`; check the type
  first.
- **Pool slot size is the widest subclass**, not `sizeof(T)` (`CPedPool` uses `CCopPed`, `CVehiclePool` uses `CHeli`,
  `CObjectPool` uses `CCutsceneObject`). Adding fields to a leaf class beyond the `S` type corrupts the pool.
- **Handles go stale.** The 7-bit `Ref` counter changes on reuse; `GetAtRef` returns null for an old handle while a
  cached pointer silently points at a different entity.
- **`CWorld::Remove` does not delete**, and peds must be removed through `CPopulation::RemovePed` (see
  `CWorld::Process`). Deleting an entity that is still in the world leaves dangling sector nodes.
- **Scan codes:** forgetting `IsScanCodeCurrent()` / `SetCurrentScanCode()` in a sector walk processes multi-sector
  entities several times. Nested queries share one scan code, so do not call another world query from inside a walk.
- **Repeat sectors wrap.** Entities 800 units apart can share a repeat-sector list; always re-check the actual position.
- **`GetSector` clamps** out-of-map coordinates to the border sector rather than failing.
- **Streaming ids are not model ids** for TXD/COL/IPL/...: use `TXDToModelId(slot)` etc. when calling `RequestModel`.
- **`RequestModel` alone does not load anything this frame.** Use `LoadAllRequestedModels(false)` when the model is
  needed immediately, and clear keep flags afterwards (`SetModelIsDeletable`) or the model leaks streaming memory.
- **Peds/vehicles in the loaded list.** `RequestModel` on a loaded ped/vehicle DFF removes it from the LRU list and does
  not re-add it; they are managed by the population code.
- **`CPhysical::Add` is still original code** (`#if 0` body, `.Reversed = false`). When debugging sector membership of
  physicals you are looking at GTA's implementation; `CPhysical::Remove` is ours.
- **(Fixed: both hooks now use the IDB addresses.)** **Hook address mismatch:** `CWorld::InjectHooks` installed
  `FindPlayerSlotWithVehiclePointer` at `0x563FD0`, which is `FindPlayerSlotWithRemoteVehiclePointer`;
  `FindPlayerSlotWithVehiclePointer` is `0x564000`.
- **Templated `...SectorList` functions** are hooked through one instantiation (`CPtrListDoubleLink<CPhysical*>` in
  `World.cpp`, `CPtrListSingleLink<CEntity*>` in `Streaming.cpp`); other instantiations are inlined copies.
- **`LoadLine` reuses a static buffer** (`CFileLoader::ms_line`): copy any string you keep across another `LoadLine`.

---

## 10. What is left

Snapshot from `idb_coverage.py --class <Name>` (status words in [README.md](README.md)):

| Class | Total | Reversed | Stub | NOHOOK | Known | Missing | Bytes left |
|---|---|---|---|---|---|---|---|
| `CWorld` | 84 | 64 | 0 | 0 | 4 | 16 | 8,812 |
| `CWorldScan` | 2 | 0 | 0 | 2 | 0 | 0 | 3,001 |
| `CFileLoader` | 51 | 46 | 1 | 0 | 4 | 0 | 2,852 |
| `CPhysical` | 62 | 54 | 2 | 0 | 4 | 2 | 2,275 |
| `CStreaming` | 112 | 90 | 0 | 0 | 14 | 8 | 1,404 |
| `CPool` (template instances) | 24 | 6 | 0 | 0 | 1 | 17 | 1,328 |
| `CPools` | 19 | 16 | 3 | 0 | 0 | 0 | 812 |
| `CEntity` | 81 | 66 | 0 | 0 | 1 | 14 | 178 |
| `CStreamingInfo` | 10 | 7 | 0 | 0 | 0 | 3 | 61 |
| `CModelInfo` | 29 | 28 | 0 | 0 | 1 | 0 | 6 |
| `CIplStore` / `CColStore` | 31 / 19 | all | | | | | 0 |

What is really left:
- **Most MISSING `CWorld` / `CStreaming` entries are false positives.** The 16 `CWorld` misses are the templated
  `...SectorList` workers (`ProcessLineOfSightSectorList` `0x566EE0`, `TriggerExplosionSectorList` `0x567750`,
  `TestSphereAgainstSectorList` `0x566140`, ...), and `CStreaming`'s `DeleteRwObjects...InSectorList` /
  `ProcessEntitiesInSectorList` likewise; they *are* installed with template-argument names that the script does not
  match. `CWorld::CastShadow` (`0x564600`, unused debug) has code.
- **Real work:** `CWorldScan::ScanWorld` / `SetExtraRectangleToScan` (~3 KB, `plugin::Call` stubs without hooks,
  defined in `Renderer.cpp`); `CPools::SaveVehiclePool` / `SaveObjectPool` / `SavePedPool` (stubs);
  `CPhysical::Add` (`0x544A30`) and `CPhysical::RemoveAndAdd` (`0x542560`) stubs; `CFileLoader::Load2dEffect`
  (`0x5B7670`, 2451 bytes, stub).
- **Small/trivial:** `CStreaming` KNOWN entries are tiny accessors (`IsInitialised`, `IsObjectInCdImage`,
  `SetLoadVehiclesInLoadScene`, `SetModelTxdIsDeletable`, `UpdateForAnimViewer`...) that exist but are not hooked;
  `GetDiscInDrive` (`0x40E9B0`) and `GetNumModelsRequested` (`0x4E3510`) are missing. `CPool` misses are per-type
  template instances (`New`/`Delete`/ctor/dtor for patrol-route, quad-tree, oct-tree, vehicle-structure and pipeline
  pools), mostly covered by the header-only `CPool` template. `CEntity` / `CPlaceable` / `CBuilding` / `CDummy` misses
  are constructors, destructors, `operator new/delete` and tiny inlined getters (`GetEntityLod`, `SetCurrentArea`...).
