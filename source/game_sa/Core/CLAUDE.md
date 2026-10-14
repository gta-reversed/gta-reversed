# Core/ — math types and engine containers

Guide: [../../../docs/subsystems/WorldStreamingPools.md](../../../docs/subsystems/WorldStreamingPools.md) §4 (pools
and handles) and §3 (world lists built from these containers). Everything depends on this folder: a wrong result here
shows up everywhere, so difftest changes (`source/difftests/tests/VectorMatrixDiffTests.cpp`, `RectDiffTests.cpp`).

## Layout
- Math: `Vector.h` (`CVector`), `Vector2D.h`, `Matrix.h` (`CMatrix`, 0x48: right/forward/up/pos + `m_pAttachMatrix`
  to an `RwMatrix`, `UpdateRW()` writes back), `Quaternion.h`, `Rect.h` (`CRect`).
- Containers: `Pool.h` (`CPool<T, S>`: handle = `(index << 8) | state byte`, `GetAtRef` rejects stale handles, NOTSA
  `GetAllValid()`), `Store.h` (`CStore<T, N>`, fixed arrays used by model infos), `LinkList.h`/`Link.h`,
  `PtrList*`/`PtrNode*` (single/double linked entity lists of the world sectors), `EntryInfoList/Node` (which sector
  lists a physical is in), `MatrixLink*`, `QuadTreeNode.h`, `OctTree*`, `SArray.h`, `List_c`/`ListItem_c`.
- `KeyGen.h` — `CKeyGen::GetKey` / `AppendStringToKey` (name hashes for model infos, anims); `constexpr`.

## Conventions
- Hooks: `RH_ScopedClass` + category `"Core"`.
- Prefer the NOTSA helpers over the original free functions: `TransformPoint` / `TransformVector` /
  `InverseTransformPoint` / `InverseTransformVector` (the free `MultiplyMatrixWithVector`, `Multiply3x3_MV/_VM` are deprecated),
  `a.Cross(b)` / `a.Dot(b)`. The original in-place forms (`Cross_OG`, ...) stay hooked for GTA callers.
- Small members (`Magnitude`, `operator+`, `GetPosition`, ...) are inlined everywhere in the original: check
  `docs/ai/InlineCatalogue.md` before concluding that a call is missing.

## Pitfalls
- `PI`, `TWO_PI`, `HALF_PI`, `SQRT_2`, `LOG2_E`, `FRAC_*`, ... in `source/game_sa/common.h` are the exact floats
  (`std::numbers`, `PI` = `3.1415927f`). Where the original loads a rounded value instead, write that literal with a
  `// NOTE:` (e.g. `6.28318f` in `FxEmitter_c::CreateParticle`).
- The original runs on x87 with extended precision: sums of products and sin/cos/sqrt chains are rounded to float
  only when stored. Where it accumulates in registers across a loop, accumulate in `double` and round at the same
  point. Keep the original operation order for dot/cross products.
- `Normalise` / `NormaliseAndMag` special-case a non-positive squared length (set `x = 1`); keep it.
- `CRect` is default-constructed flipped (empty), and the 4-float constructor takes `(left, bottom, right, top)` =
  minX, minY, maxX, maxY (legacy naming quirk).
- Difftests compare with small ULP tolerances (`CompareFloats`, `Tolerance{ .Ulps = 4 }`); anything larger is a real
  difference (a wrong index in `CMatrix::ConvertToEulerAngles` was found this way).
