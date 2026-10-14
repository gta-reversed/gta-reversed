# Collision/ — col shapes, col models and the CCollision tests

Guide: [../../../docs/subsystems/CollisionPhysics.md](../../../docs/subsystems/CollisionPhysics.md) (col models,
Test vs Process, `CPhysical` response, the `CWorld::Process` collision loop, open bugs). The physics response lives
in `../Entity/Physical.cpp`, not here.

## Layout
- `Collision.h/.cpp` — `CCollision`, all static: `Test*` (bool only) and `Process*` (fill a `CColPoint` and shrink
  `maxTouchDistance` / `mindistsq`) for sphere/box/line/triangle/disk pairs, `ProcessColModels`, line of sight,
  camera collision (`CheckCameraCollision*`, `SphereCastVs*`). ~3,800 lines: read only the function you need.
- Shapes: `ColSphere`, `ColBox` (+ `Box`), `ColLine`, `ColDisk`, `ColTriangle`, `ColTrianglePlane`, `ColPoint`
  (contact result, `m_nSurfaceTypeA/B`), `ColSurface` (material, piece, lighting, light; 4 bytes).
- `ColModel.h` — `CColModel` (0x30): bounding box/sphere, `m_nColSlot`, flags, `CCollisionData* m_pColData`.
- `CollisionData.h` — `CCollisionData` (0x30): counts + pointers; span helpers `GetSpheres()`, `GetBoxes()`, `GetTris()`,
  `GetTriPlanes()`, `GetLines()`, `GetShdwTris()`. Vertices are `CompressedVector` (`../CompressedVector.h`, fixed
  point /128).
- `ColStore.*` (col file streaming, `ColDef` pool), `ColAccel.*` (col cache for faster loading), `TempColModels.*`
  (built-in models: ped, door, bbox, ...), `CollisionEventScanner.*` (collision -> AI events).

## Conventions
- `RH_ScopedClass` + category `"Collision"`; `CCollision` (and `ColourSet`) use `RH_ScopedCategoryGlobal()`.
- `CCollision::s_DebugSettings` is NOTSA: per shape-pair enable switches and `AllowLineOriginInsideSphere` (default
  `false` = original: a line starting and ending inside a sphere does not hit it). Keep the `IsEnabled(...)`
  early-outs when touching those functions, and keep defaults equal to the original.
- Use `CompressedVector`'s own conversions, not the deprecated `CompressVector` / `UncompressVector`.
- Check against the original with a difftest (`source/difftests/`). `TEST_COLLISION_FUNCS` only enables randomized
  self-tests (`CCollision::Tests`) that run before hooks are installed.

## Pitfalls
- Face groups: when `bHasFaceGroups` is set, a `uint32` count and a `TFaceGroup[]` sit *before* `m_pTriangles` with no
  pointer to them (long comment in `CollisionData.h`). Only valid if the col model is a single allocation.
- `m_pTrianglePlanes` is allocated lazily (`CalculateTrianglePlanes` / `RemoveTrianglePlanes`) and is the only
  pointer outside the single data buffer.
- `m_pLines` and `m_pDisks` share a union, selected by `bUsesDisks`.
- Many functions inline `DotProduct` / `GetMatrix`; see `docs/ai/InlineCatalogue.md` when matching the asm.
