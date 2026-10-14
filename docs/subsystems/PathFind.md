# Path finding (`CPathFind`)

How the game stores its road and footpath network, streams it in per map area, and answers the queries that cars
and peds make ("closest node", "next node when wandering", "route from A to B"). It also covers the consumers:
`CCarCtrl`/`CAutoPilot` for traffic and mission cars, and `CTaskComplexFollowNodeRoute`/`CTaskComplexWander` for peds.
For the general workflow see [../ReversingGuide.md](../ReversingGuide.md). Driving AI is described in more depth in
[Vehicles.md](Vehicles.md) ("AI driving"), the task tree in [Tasks.md](Tasks.md), and DAT streaming in
[WorldStreamingPools.md](WorldStreamingPools.md).

All paths are relative to the repo root. Addresses are the ones written in the repo (`RH_*Install` or `// 0xADDR`)
and were checked against the IDB. Where the IDB disagrees with the repo, this page says so.

---

## 1. Overview

```
ThePaths (CPathFind, 0x96F050, size 0x3C80)          source/game_sa/PathFind.h
 ├─ 64 map areas  (8 x 8 grid of 750 x 750 units, world -3000..+3000)   area = y * 8 + x
 │   └─ one streamed file per area: nodes<N>.dat = streaming model RESOURCE_ID_DAT + N (25511 + N)
 ├─ 8 interior areas (ids 64..71), built at run time by InteriorGroup_c::SetupPaths
 │
 │  per area (all arrays are indexed by area id, 72 slots unless noted):
 ├─ m_pPathNodes[a]        CPathNode[m_anNumNodes]: first m_anNumVehicleNodes car nodes, then m_anNumPedNodes ped nodes
 ├─ m_pNodeLinks[a]        CNodeAddress[m_anNumAddresses + 192]   neighbours; node.m_wBaseLinkId .. +m_nNumLinks
 ├─ m_pLinkLengths[a]      uint8 per link (distance used by DoPathSearch)
 ├─ m_pPathIntersections[a] CPathIntersectionInfo per link (road crossing / ped traffic light bits)
 ├─ m_pNaviLinks[a]        CCarPathLinkAddress per link -> which navi node describes that road segment (64 slots)
 └─ m_pNaviNodes[a]        CCarPathLink[m_anNumCarPathLinks]  "navi nodes": lane counts, direction, traffic lights
```

There is one node graph for both kinds of traffic. A node is a car node or a ped node depending on its index inside
its area (`GetPathNodesInArea`). Searches take an `ePathType` (`PATH_TYPE_VEH = 0`,
`PATH_TYPE_PED = 1`) and only look at the matching half of each area. Boats use the car nodes
with `m_bWaterNode` set.

A node is addressed by `CNodeAddress { uint16 area; uint16 node; }` (`source/game_sa/NodeAddress.h`). An
address with area `0xFFFF` is invalid (`IsValid`, `operator bool`). Before you dereference an address, check that its
area is loaded (`IsAreaNodesAvailable`, `0x420AA0`), because only the areas near the player (plus a few requested
ones) are in memory at any time (§3.1).

---

## 2. Data layout

### 2.1 Structures (`source/game_sa/PathFind.h`)

| Struct | Size | Notes |
|---|---|---|
| `CNodeAddress` | `0x4` | `NodeAddress.h`. Area id + node id, `-1/-1` = invalid |
| `CCarPathLinkAddress` | `0x2` | 10-bit navi node id + 6-bit area id. Only map areas (0..63) fit in 6 bits |
| `CPathIntersectionInfo` | `0x1` | `m_bRoadCross`, `m_bPedTrafficLight`. Read by `TestCrossesRoad` (`0x44D790`) and `TestForPedTrafficLight` (`0x44D480`) |
| `CCarPathLink` ("navi node") | `0xE` | `+0x0` position as `int16 * 8`, `+0x4` `m_attachedTo` (the node this segment points at), `+0x8` direction as `int8 / 100`, `+0xA` width `/16`, `+0xB` lanes: bits 0-2 `m_numOppositeDirLanes`, bits 3-5 `m_numSameDirLanes`, bit 6 traffic light direction, `+0xC` traffic light state (2 bits) and `m_bridgeLights` |
| `CPathNode` | `0x1C` | see below |
| `CNodesSwitchedOnOrOff` | `0x1C` | a box plus `isOff`/`isCars`; 64 of them are kept in `m_aNodeSwitches` |
| `CPathFind` | `0x3C80` | the whole network plus the search hash table |
| `CNodeRoute` | `0x24` | `source/game_sa/NodeRoute.h`: `notsa::Route<CNodeAddress, 8>` (`source/game_sa/Route.hpp`), pooled (`ms_pNodeRoutePool`, `0xB744B8`). `new`/`delete` are `0x41B860`/`0x41B870` |

`CPathNode` layout (offsets computed from the header, matches `VALIDATE_SIZE`):

| Offset | Field | Meaning |
|---|---|---|
| `0x00` / `0x04` | `m_next`, `m_prev` | bucket list links, only used during `DoPathSearch` |
| `0x08` | `m_vPos` | `CompressedLargeVector` (3 x `int16`, divide by 8) |
| `0x0E` | `m_totalDistFromOrigin` | search distance, `0x7FFE` (`SHRT_MAX - 1`) when not visited |
| `0x10` | `m_wBaseLinkId` | first link in `m_pNodeLinks[area]` |
| `0x12` / `0x14` | `m_wAreaId`, `m_wNodeId` | the node's own address |
| `0x16` | `m_nPathWidth` | `/16` |
| `0x17` | `m_nFloodFill` | connected-component id. Two nodes with different values can never be joined (`DoPathSearch` gives up at once). The IDB calls it `m_nNodeType` |
| `0x18` | byte 0 | `m_nNumLinks:4`, `m_onDeadEnd`, `m_isSwitchedOff`, `m_bRoadBlocks`, `m_bWaterNode` |
| `0x19` | byte 1 | `m_isSwitchedOffOriginal`, `unk1` (temporary "already recorded" mark, see §3.3), `m_bDontWander`, `m_bNotHighway`, `m_bHighway`, ... |
| `0x1A` | byte 2 | `m_nSpawnProbability:4`, `m_nBehaviourType:4` (1 = roadblock, 2 = parking) |

### 2.2 `CPathFind` fields (offsets from the header comments and `VALIDATE_SIZE`)

| Offset | Field | Notes |
|---|---|---|
| `0x0004` | `m_pathFindHashTable[512]` | buckets by `distance % 512`, used only by `DoPathSearch` |
| `0x0804` | `m_pPathNodes[72]` | `nullptr` = area not loaded (`IsAreaLoaded`) |
| `0x0924` | `m_pNaviNodes[72]` | |
| `0x0A44` | `m_pNodeLinks[72]` | |
| `0x0B64` | `m_pLinkLengths[72]` | |
| `0x0C84` | `m_pPathIntersections[72]` | |
| `0x0DA4` | `m_pNaviLinks[64]` | only 64 entries, see Gotchas (`Init` writes 72) |
| `0x0FA4..` | `m_anNumNodes`, `m_anNumVehicleNodes`, `m_anNumPedNodes`, `m_anNumCarPathLinks`, `m_anNumAddresses` | 72 each |
| | `m_aDynamicLinksBaseIds[64][16]`, `m_aDynamicLinksIds[64][16]` | bookkeeping for links added at run time (interiors) |
| | `m_interiorIDs[8]` | `-1` = free interior slot |
| | `m_nNumNodeSwitches`, `m_aNodeSwitches[64]` | script road switches, saved in the save game |
| | `m_loadAreaRequestPending` + rect | `MakeRequestForNodesToBeLoaded` (`0x450D70`) |

Region helpers (all reversed): `FindXRegionForCoors` `0x44D890`, `FindYRegionForCoors` `0x44D8C0`,
`FindRegionForCoors` `0x44D830`, `FindXCoorsForRegion` `0x44D8F0`, `FindYCoorsForRegion` `0x44D910`,
`FindStartPointOfRegion` `0x44D930`. The NOTSA helpers `GetPathNodesInArea`, `GetNodeLinkedNodes`,
`IterAreasTouchingRect` and `FindIntersection` replace
loops that the compiler inlined everywhere.

---

## 3. Per-frame flow and streaming

### 3.1 Streaming the areas

```
CGame::Process
 └─ ThePaths.UpdateStreaming(false)              0x450A60 (reversed)
     ├─ runs when a 512 ms boundary is crossed, when s_bLoadPathsNeeded is set, or when forced
     ├─ MarkRegionsForCoors(player, 350)          0x44DB60
     ├─ MarkRegionsForCoors(SetPathsNeededAtPosition pos, 300)   0x44DCD0 (CEntryExit::TransitionFinished)
     ├─ MarkRegionsForCoors(each mission car that is not heli/plane/boat/train, 300)
     ├─ script rect from MakeRequestForNodesToBeLoaded -> STREAMING_MISSION_REQUIRED
     └─ for each of the 64 areas: RequestModel(DAT, KEEP_IN_MEMORY or MISSION_REQUIRED) / RemoveModel(DAT)

CStreaming::ConvertBufferToObject (DAT case)
 └─ ThePaths.LoadPathFindData(stream, area)      0x4529F0
CStreaming::RemoveModel (DAT case)
 └─ ThePaths.UnLoadPathFindData(area)            0x44D0F0
```

`LoadPathFindData(stream, area)` reads the five counts, then the nodes, navi nodes, links (`numAddresses + 192`
entries: 16 dynamic link blocks of 12 are reserved at the end), navi links, link lengths and intersections. After
that it copies `m_isSwitchedOff` to `m_isSwitchedOffOriginal`, applies every stored script road switch to the new
area (`SwitchRoadsOffInAreaForOneRegion`), and resets that area's dynamic-link table. The other overload
`LoadPathFindData(area)` (`0x452F40`) reads `data\paths\nodes%d.dat` from disk.

`LoadSceneForPathNodes` (`0x44DE00`) requests the areas within 350 units of a point; `AreNodesLoadedForArea`
(`0x44DD10`) and `HaveRequestedNodesBeenLoaded` (`0x450DB0`) are what scripts poll.

### 3.2 Who calls what during a frame

`CWorld::Process` runs every ped and vehicle. Their AI is what queries the paths:

| Caller | Path function | Status of the callee |
|---|---|---|
| `CTaskComplexWander::UpdatePathNodes` (`0x669ED0`) | `FindNextNodeWandering` `0x451B70` | STUB |
| `CTaskComplexWander::ComputeTargetPos` (`0x669F60`) | `TakeWidthIntoAccountForWandering` `0x4509A0` | REVERSED |
| `CTaskComplexFollowNodeRoute::ComputePathNodes` (`0x66EFA0`) | `ComputeRoute` `0x452760` -> `DoPathSearch` `0x4515D0` | both STUB |
| `CCarCtrl::JoinCarWithRoadSystemGotoCoors` (`0x42F870`) | `DoPathSearch`, then `RemoveBadStartNode` | STUB caller |
| `CCarCtrl::PickNextNodeToFollowPath` / `ToChaseCar` / `ReconsiderRoute` / `SteerAICarWithPhysicsFollowPath_Racing` | `DoPathSearch` | STUB callers |
| `CCarCtrl::PickNextNodeRandomly` (`0x42DE80`) | node links, navi links, `ThisNodeWillLeadIntoADeadEnd` | STUB caller |
| `CCarCtrl::GenerateCarCreationCoors2`, `CCarGenerator`, `CGangWars`, `CWorld::RemoveFallen*`, `CPed::PositionPedOutOfCollision`, script commands | `FindNodeClosestToCoors` `0x44F460` | STUB |
| `CPopulation`, `CStuckCarCheck`, `CGangWars` | `FindNthNodeClosestToCoors` `0x44F8C0` | STUB |
| `CPopulation` | `GeneratePedCreationCoors_Interior` `0x44ECA0` | STUB |

### 3.3 Closest-node queries

- `FindNodeClosestToCoors` (`0x44F460`, STUB): searches the region of `pos` with
  `FindNodeClosestInRegion` (`0x44F2C0`, MISSING), then rings of neighbouring regions (750 units further each time,
  at most 4 extra rings) while the best distance is larger than the distance to the region edge, and finally the 8
  interior areas unless the last argument is set. The repo's parameter names (`unk2`, `unk3`, `unk4`, `bBoatsOnly`,
  `unk6`) are still guesses: `unk2` and `unk4` are passed through to `FindNodeClosestInRegion`, `unk3` is not used,
  `unk4 = 1` skips nodes that carry the temporary "recorded" mark (byte 1, bit 1), and `unk6` skips the interior
  areas.
- `RecordNodesClosestToCoors` (declared in `PathFind.h`, no address in the repo) is `0x44FA30`. It clears the
  mark on every node of the requested type, then calls `FindNodeClosestToCoors(..., unk4 = 1, ...)` `count` times,
  marking each node it finds. **The IDB names `0x44FA30` `CPathFind::FindNodeClosestToCoors`.** That name is wrong.
- `FindNthNodeClosestToCoors` (`0x44F8C0`, STUB) and `FindNodeClosestToCoorsFavourDirection` (`0x44FCE0`,
  reversed) are variants. `FindNodePairClosestToCoors` (`0x44FEE0`, MISSING) is used by car
  creation.

### 3.4 `DoPathSearch` (`0x4515D0`, STUB)

The route search. It is a Dijkstra search **backwards from the target**, using integer link lengths and the
512-bucket hash table as a cheap priority queue:

1. Resolve the target address (hint, or `FindNodeClosestToCoors(targetPos)` when the hint is invalid or its area is
   not loaded), then the origin address the same way. Fail (`*outDistance = 100000`) if either is missing or if their
   `m_nFloodFill` values differ. If they are the same node, return 0 nodes and distance 0.
2. Clear the 512 buckets and insert the target with distance 0. Every node that gets a distance is remembered in
   `aNodesToBeCleared` (`0x972CD0`, 5000 entries).
3. Process bucket `depth % 512`: for each node there, relax every link whose area is loaded. A link is skipped when it
   is the forbidden node, when it switches between water and land nodes (unless allowed), or, with
   `oneSideOnly`, when the navi node of that segment has no lane in the travel direction. Then remove the node from
   its bucket.
4. Stop when the buckets are empty, when `depth > maxSearchDepth`, or when 4950 nodes have been touched. Once the
   origin has been reached, write the origin, then walk "downhill": from each node follow the link whose
   `distance - linkLength` equals the neighbour's distance, until the target or `maxNodesToFind`.
5. Reset `m_totalDistFromOrigin = 0x7FFE` on every remembered node.

`ComputeRoute` (`0x452760`, STUB) is the ped wrapper: `DoPathSearch(type, start, startNode, end, tmp[8], &n, 8,
nullptr, 999999.88, nullptr, 999999.88, false, EmptyNodeAddress, false, false)` and copies the result into a
`CNodeRoute`. Cars call `DoPathSearch` directly with `m_autoPilot.m_aPathFindNodesInfo` (8 entries),
`oneSideOnly = bCantGoAgainstTraffic` (car-control flag bit 6), water transitions allowed only for model 539
(Vortex hovercraft), and `forBoats = IsSubBoat()`.

The hook is `.Reversed = false` with the comment *"Sometimes breaks `CTaskComplexFollowNodeRoute::ComputePathNodes`
- To repro just walk around in groove st."* (`CPathFind::InjectHooks`). The reason is now known; see Gotchas, three bugs
in the backtracking loop and one in the lane test.

### 3.5 Switching roads on and off

Script commands (`SWITCH_ROADS_*` / `SWITCH_PED_ROADS_*` in `source/game_sa/Scripts/Commands/PathCommands.cpp`; these
handlers are not registered yet, see [Scripts.md](Scripts.md) Gotchas, so the original handlers run) call
`SwitchRoadsOffInArea` (`0x452C80`) / `SwitchPedRoadsOffInArea` (`0x452F00`). These:

1. call `SwitchRoadsOffInAreaForOneRegion` (`0x452820`) for each of the 64 areas. For every car or ped node inside
   the box that `ThisNodeHasToBeSwitchedOff` (`0x44D3E0`: every behaviour type except roadblock and parking), it
   calls `SwitchOffNodeAndNeighbours` (`0x452160`) and then follows the up to two chains it returns, so whole road
   segments up to the next junction change state;
2. drop stored switch boxes that lie completely inside the new box;
3. store the new box (unless "back to original"), so that areas streamed in later get the same switch applied in
   `LoadPathFindData`.

`TidyUpNodeSwitchesAfterMission` (`0x44D3B0`) trims the list back to 54 entries. `Save` (`0x5D34C0`) and
`Load` (`0x5D3500`) write the count followed by the boxes; they are called for `eBlocks::PATHS`
in `CGenericGameStorage`. The NOTSA bug fix `notsa::bugfixes::CPathFind_SwitchRoadsOffInArea_StrayAreas` fixes the original's wrong index when compacting the list.

`SetLinksBridgeLights` (`0x44D960`) and `MarkRoadNodeAsDontWander` (`0x450560`) are the other run-time edits.

### 3.6 Interior paths

`InteriorGroup_c::SetupPaths` (`0x595590`, STUB) builds the paths of a procedural interior:
`StartNewInterior` (`0x44DE80`) -> `AddNodeToNewInterior` (`0x450E90`) for up to 64 nodes -> `AddInteriorLink`
(`0x44DED0`) / `AddInteriorLinkToExternalNode` (`0x44DF30`) -> `CompleteNewInterior` (`0x452270`, MISSING). The last
one allocates area `64 + slot`, links it to the outside world with `AddDynamicLinkBetween2Nodes(_For1Node)`
(`0x4512D0` MISSING / `0x44E000` reversed) and fills `CNodeAddress` results for the caller.
`InteriorGroup_c::Exit` (`0x595290`, STUB) calls `RemoveInterior` (`0x44E1A0`, reversed: see Gotchas).

---

## 4. Cars

`CAutoPilot` (`source/game_sa/AutoPilot.h`, size `0x98`) holds the car's position in the graph:

- `m_currentAddress`, `m_startingRouteNode`, `m_endingRouteNode`: the node behind the car, the node it is driving
  to, and the one after. (The names are misleading: `JoinCarWithRoadSystemGotoCoors` sets `m_currentAddress =
  route[0]`, drops it, then sets `m_startingRouteNode = route[0]` again, and `PickNextNodeAccordingStrategy` reads
  `m_startingRouteNode` as "the node we are heading for".)
- `m_nCurrentPathNodeInfo`, `m_nNextPathNodeInfo`, `m_nPreviousPathNodeInfo`: the `CCarPathLinkAddress`es of the
  road segments (navi nodes) between those nodes, filled by `CCarCtrl::FindLinksToGoWithTheseNodes` (`0x42B470`).
  Steering follows navi nodes, not path nodes.
- `m_nCurrentLane`, `m_nNextLane`: lane index inside the navi node (`CCarPathLink::OneWayLaneOffset`, `0x44DB00`,
  inline in the header).
- `m_aPathFindNodesInfo[8]` + `m_nPathFindNodesCount`: the planned route for "go to" missions.
  `RemoveOnePathNode` (`0x41B950`, NOHOOK) shifts it by one.

Call chain (see "AI driving" in [Vehicles.md](Vehicles.md) for the steering side):

```
mission set / car created / script        CCarCtrl::JoinCarWithRoadAccordingToMission  0x432CB0 (reversed)
 ├─ cruising, parking, ...                 JoinCarWithRoadSystem          0x42F5A0 STUB: nearest node pair, pick direction
 └─ chase / go to coordinates              JoinCarWithRoadSystemGotoCoors 0x42F870 STUB:
                                             DoPathSearch(car pos -> target) into m_aPathFindNodesInfo
                                             RemoveBadStartNode (sub_44E4F0 in the IDB)
                                             < 2 nodes -> JoinCarWithRoadSystem
                                             else current/starting node from the route, FindLinksToGoWithTheseNodes
each time the car reaches its target node  PickNextNodeAccordingStrategy  0x432B10 STUB
                                           (from SteerAICarWithPhysicsFollowPath 0x434900 and UpdateCarOnRails 0x436540)
 ├─ sets m_autoPilot.field_41 from the node's highway bits (-1 for behaviour types 8 and 9)
 ├─ *_PLAYER_FARAWAY                       PickNextNodeToChaseCar(player coords)   0x426EF0
 ├─ GOTOCOORDINATES(_ACCURATE)             PickNextNodeToFollowPath                0x427740 (re-runs DoPathSearch)
 ├─ *CAR_FARAWAY, KILLPED_FARAWAY, ESCORT_*_FARAWAY  PickNextNodeToChaseCar(target entity coords)
 └─ anything else                          PickNextNodeRandomly                    0x42DE80 (3.5 KB)
```

`sub_44E4F0` is called only from `JoinCarWithRoadSystemGotoCoors` with `(car pos, route, &count)`. That matches the
declaration `RemoveBadStartNode(CVector, CNodeAddress*, int16*)` in `PathFind.h`, which has no address in the
repo yet.

Traffic creation uses `CCarCtrl::GenerateCarCreationCoors2` (`0x424210`) -> `FindNodeClosestToCoors` and the
node's `m_nSpawnProbability`; `CPathFind::CalcRoadDensity` (`0x44EFC0`, MISSING) is on the
`GenerateRandomCars` path (`docs/ai/FrameCallGraph.md`).

---

## 5. Peds

### 5.1 Wandering (`CTaskComplexWander`, `source/game_sa/Tasks/TaskTypes/TaskComplexWander.cpp`)

The task keeps `m_LastNode`, `m_NextNode` and a direction `m_nDir` (0-7, eight compass sectors).
`UpdatePathNodes` moves `m_NextNode` into `m_LastNode` and asks `FindNextNodeWandering(PATH_TYPE_PED, pos, &last,
&next, dir, &outDir)` (STUB) for the neighbour that best matches the direction. `UpdateDir` (`0x669DA0`)
sometimes turns at nodes with 3 or more links. The walk target is the node position offset sideways inside the path
width by `TakeWidthIntoAccountForWandering(node, ped->m_nRandomSeed)`, plus 1 m in Z. `ScanForBlockedNodes`
(`0x674560`) re-picks when the next node is blocked. `CreateNextSubTask` (`0x674140`) is still a STUB, and the
standard civilian subclass `CTaskComplexWanderStandard` has no reversed function.

### 5.2 Following a route (`CTaskComplexFollowNodeRoute`, fully reversed apart from one destructor)

```
SetTarget / CreateFirstSubTask
 └─ ComputePathNodes(ped)                 0x66EFA0
     ├─ snap m_TargetPt to the ground
     ├─ if !CanGoStraightThere: ThePaths.ComputeRoute(PATH_TYPE_PED, pos, target, m_StartNode, m_NodeRoute)
     ├─ drop the route if its first node is much further away than the target (3 m, or 50 m in some cases)
     ├─ inside an interior group: recompute through the interior's door node
     │  (These2NodesAreAdjacent, ComputeRoute, CNodeRoute::Reverse 0x667900)
     ├─ otherwise: cut the route at the first node from which the target is directly reachable
     └─ m_CurrPtIdx / m_CurrNode
 └─ ComputeRoute()                        0x6699E0   CNodeRoute -> CPointRoute (node positions + target)
ControlSubTask / CreateNextSubTask walk the CPointRoute with CTaskSimpleGoToPoint(Fine) / GoToPointAndStandStill
```

`CPointRoute` is `notsa::Route<CVector>` (`source/game_sa/PedGeometryAnalyser.h`, size `0x64`). It is also
filled by `CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox` (`0x5F3DD0`) for walking round cars and
objects. There is **no `CPedPath` class in San Andreas** (it existed in Vice City); the IDB has no function with that
name. Short-range ped avoidance is done with `CPointRoute` and `CPedGeometryAnalyser` instead.

### 5.3 Spawning

`CPopulation` asks for ped nodes near the camera with `FindNthNodeClosestToCoors` and, for interiors,
`GeneratePedCreationCoors_Interior` (STUB). `GeneratePedCreationCoors` is declared in `PathFind.h` but has no
address or body in the repo.

---

## 6. Gotchas

- **Two node kinds, one array.** Car nodes come first in each area, ped nodes after them. Code that loops over
  `m_anNumNodes` without looking at the type touches both. Use `GetPathNodesInArea(area, type)`.
- **Always check the area.** `GetPathNode` (`0x420AC0`) only asserts. Linked nodes can be in another area that is not
  loaded; `GetNodeLinkedNodes(node)` filters them out, `GetNodeLinkedNodes(node, false)` does not.
- **`CCarPathLinkAddress` has 6 bits of area,** so navi nodes only exist for the 64 map areas, and `m_pNaviLinks`
  has 64 slots, not 72. `CPathFind::Init` (`0x44D080`) clears 72 entries of it and of
  `m_aTempNodes`; the overflow lands in the next arrays. This is copied from the original on purpose (commented).
- **Link arrays have 192 spare entries** (16 blocks of 12) for dynamic links. `AddDynamicLinkBetween2Nodes_For1Node`
  moves a node's links into a free block and records it in `m_aDynamicLinks*`. Search for `MAGIC_NUM_12`.
- **The bucket list uses the bucket slot as a fake node.** `AddNodeToList` stores `&m_pathFindHashTable[i]` in
  `m_prev`, so `RemoveNodeFromList` writes through `m_prev->m_next` (offset 0) into the bucket. Keep `m_next` at
  offset 0. The IDB uses the opposite names for the two pointers.
- **`m_totalDistFromOrigin` is shared state.** It must be `0x7FFE` for every node outside a search.
  `DoPathSearch` resets only the nodes it recorded (5000 at most), so a search that touches more nodes leaves stale
  distances behind.
- **IDB naming traps.** `0x44F460` (the real `FindNodeClosestToCoors`) is named as a free function
  `FindNodeClosestToCoors::...` in the IDB, so `idb_coverage.py --class CPathFind` does not list it. `0x44FA30` is
  `RecordNodesClosestToCoors` but is named `CPathFind::FindNodeClosestToCoors`. `sub_44E4F0` is `RemoveBadStartNode`.
  `CNodeRoute::operator new` (`0x41B860`) shows as MISSING although `CNodeRoute::InjectHooks` hooks it (the script does not
  match `operator` names).
- **`CountNeighboursToBeSwitchedOff` (`0x4504F0`) has an inline `__asm mov eax, this`**.
  The comment says the original `SwitchOffNodeAndNeighbours` crashed when this function was unhooked, because it
  expects `eax` to be left unchanged. The `return` that follows overwrites `eax` anyway, so the workaround probably
  does nothing. Check `SwitchOffNodeAndNeighbours` (`0x452160`) in IDA before you change either function.
- **`TakeWidthIntoAccountForWandering` uses `% 16` and `/ 16`** on an `int16` seed where the original uses `& 0xF`
  and `>> 4` (`0x4509A0`). The results differ only for negative seeds. Ped seeds come from
  `rand()` (0..0x7FFF), so this is latent.
- **(Bug, not fixed) `DoPathSearch` backtracking** (the final `for` loop): the original loop is
  `while (count < maxNodesToFind && node != target)`. The repo has `node == target || count < maxNodesToFind`, so it
  never stops at the target. It increments `outNodesCount` both in the body and in the `for` increment. It also reads
  `m_pNodeLinks[area][linkNum]` without adding `m_wBaseLinkId`, so it follows the wrong links. These three
  explain the `ComputePathNodes` breakage noted in the hook comment.
- **(Bug, not fixed) `DoPathSearch` lane test** (the `oneSideOnly` branch): when the navi node is attached to the linked
  node, the original requires `m_numSameDirLanes != 0` (byte `+0xB >> 3`), otherwise `m_numOppositeDirLanes`. The
  repo tests the two fields the other way round. It only matters for `oneSideOnly` (car) searches.
- **(Bug, live) `FindNodeCoorsForScript(addr, bool*)`** (`0x4505E0`) reads
  `m_pNodeLinks[node->m_wBaseLinkId]` (an area pointer chosen by link id) instead of
  `m_pNodeLinks[node->m_wAreaId][node->m_wBaseLinkId]`. It also calls `GetPathNode` on the linked node without the
  original's "area loaded" check (the original then returns the node's own position). The function is hooked and
  reversed. It is used by script commands and `CTaskSimpleCreateCarAndGetIn`.
- **(Bug, live) `LoadPathFindData(stream, area)`** (last loop) resets the dynamic link tables of areas
  0..15 every time any area loads. The original (`0x4529F0`) resets only the 16 entries of the loaded area.
- **(Bug, live) `RemoveInterior`** (`0x44E1A0`), reached from `InteriorGroup_c::Exit`:
  - it never sets `m_interiorIDs[slot] = -1`, so interior slots are never freed (`StartNewInterior` then runs past 8);
  - it frees `m_pLinkLengths` twice and never frees `m_pNodeLinks` (the `FreeAndNull` calls at the end), which leaks;
  - it looks up the dynamic link in `m_aDynamicLinksIds[intSlot]` where the original uses the table of the
    area being scanned;
  - `rng::remove_if` over `GetNodeLinkedNodes(node, false)` moves whole `CPathNode` objects. The original
    shifts the `CNodeAddress` links;
  - it scans all 72 areas; the original scans only the 64 map areas.

---

## 7. What is left

Numbers from `python .claude/skills/find-reversing-targets/scripts/idb_coverage.py --class <Class>` (checked
2026-10-14). Bytes are the sizes of functions not yet REVERSED.

| Class | Functions | Reversed | Stub | No-hook | Known | Missing | Bytes left |
|---|---|---|---|---|---|---|---|
| `CPathFind` | 75 | 54 | 7 | 0 | 0 | 14 | 9,406 of 18,654 |
| `CPathNode` | 2 | 1 | 0 | 0 | 0 | 1 | 99 |
| `CCarPathLink` | 1 | 0 | 0 | 0 | 0 | 1 | 96 (`OneWayLaneOffset`, already inline) |
| `CNodeRoute` | 2 | 0* | 0 | 0 | 0 | 2 | 54 (*`operator new` is hooked, see Gotchas) |
| `CAutoPilot` | 3 | 0 | 0 | 2 | 1 | 0 | 1,024 |
| `CCarCtrl` | 110 | 20 | 83 | 6 | 0 | 1 | 84,878 of 88,917 |
| `CTaskComplexFollowNodeRoute` | 20 | 19 | 0 | 0 | 0 | 1 | 30 |
| `CTaskComplexWander` | 20 | 13 | 1 | 0 | 1 | 5 | 1,406 |
| `CTaskComplexWanderStandard` | 10 | 0 | 0 | 5 | 2 | 3 | 2,252 |
| `InteriorGroup_c` | 25 | 2 | 22 | 0 | 0 | 1 | 7,902 |

Not counted above because the IDB does not attribute them to `CPathFind`: `FindNodeClosestToCoors` (`0x44F460`,
STUB, 1,118 B) and `RemoveBadStartNode` (`sub_44E4F0`, 285 B, not in the repo).

Biggest `CPathFind` functions left: `DoPathSearch` 1,427 B (STUB), `CompleteNewInterior` 1,251 (MISSING),
`FindNextNodeWandering` 1,161 (STUB), `FindNodePairClosestToCoors` 1,080 (MISSING),
`GeneratePedCreationCoors_Interior` 794 (STUB), `FindParkingNodeInArea` 476 (STUB), `CalcRoadDensity` 457
(MISSING), `FindNodeOrientationForCarPlacement` 449 (STUB), `FindNodeClosestInRegion` 402 (MISSING),
`RecordNodesInCircle` 381 (MISSING), `FindNthNodeClosestToCoors` 363 (STUB), `RecordNodesClosestToCoors` 304
(MISSING, IDB name wrong), `CalcDistToAnyConnectingLinks` 298 (MISSING), `ComputeRoute` 190 (STUB).

### Suggested order

1. **Fix the live bugs first** (one PR each): `RemoveInterior`, `FindNodeCoorsForScript(addr)`, the dynamic-link
   reset in `LoadPathFindData`. Test by entering and leaving a few houses/shops (interior groups) and by running
   script commands that place cars on nodes.
2. **Finish `DoPathSearch`.** Fix the backtracking loop and the lane test listed in Gotchas, compare against
   `asm_validate.py CPathFind::DoPathSearch`, then enable the hook and walk around Grove Street (the repro in the
   hook comment). A difftest is practical: the function is deterministic for a given loaded area set; call both
   versions with the same arguments and compare node lists and distance. Then hook `ComputeRoute` (190 B).
3. **The closest-node family**, bottom-up: `FindNodeClosestInRegion` -> `FindNodeClosestToCoors` (rename the
   parameters while you are there) -> `RecordNodesClosestToCoors` (fix the IDB name) -> `FindNthNodeClosestToCoors`
   -> `FindNodePairClosestToCoors`, `CalcDistToAnyConnectingLinks`, `RecordNodesInCircle`. These are pure queries
   and suit difftests.
4. **`FindNextNodeWandering`**, then `CTaskComplexWander::CreateNextSubTask` and `CTaskComplexWanderStandard`.
5. **Interiors:** `CompleteNewInterior`, `AddDynamicLinkBetween2Nodes`, then `InteriorGroup_c::SetupPaths`.
6. **Car side:** `CAutoPilot::RemoveOnePathNode`/`ModifySpeed` (add the missing hooks), `RemoveBadStartNode`,
   `FindNodeOrientationForCarPlacement`, `CCarCtrl::FindLinksToGoWithTheseNodes`, `JoinCarWithRoadSystem(GotoCoors)`,
   `FindNodesThisCarIsNearestTo`, `IsThisAnAppropriateNode`, `FindPathDirection`, then the `PickNextNode*`
   functions. The steering functions are covered in the status section of [Vehicles.md](Vehicles.md).

For every function follow [../ReversingGuide.md](../ReversingGuide.md): read it in IDA, write it, register it,
then run `asm_validate.py` and a difftest or an in-game A/B with the hook toggled in the F7 menu.
