# Animation/ — anim blend system (CAnimManager and friends)

Guide: [../../../docs/subsystems/Animation.md](../../../docs/subsystems/Animation.md). Object model in short:
`README.md` here. How peds use it: [Peds.md](../../../docs/subsystems/Peds.md) §4 "Animation hookup".

## Layout (sizes from `VALIDATE_SIZE`)
- `AnimManager.*` — `CAnimManager` (static): `ms_aAnimations` (2500 `CAnimBlendHierarchy`, `0xB4EA40`),
  `ms_aAnimBlocks` (`0xB5D4A0`), `ms_aAnimAssocDefinitions` (`0x8AA5A8`).
- `CAnimBlendHierarchy` (0x18) — one animation; owns one `CAnimBlendSequence` (0xC) per bone, key frames in
  `AnimSequenceFrames.h` (compressed or not).
- `CAnimBlock` (0x20) — one IFP block, ref-counted (`AddAnimBlockRef` / `RemoveAnimBlockRef`).
- `CAnimBlendAssocGroup` (0x14) — maps `AnimationId`s / names to `CAnimBlendStaticAssociation`s (0x14, vtable `0x85C6CC`).
- `CAnimBlendAssociation` (0x3C, virtual destructor) — a running anim on a clump, one `CAnimBlendNode` (0x18) per bone.
  `CAnimBlendClumpData` (0x14) is the per-clump list; `AnimBlendFrameData` (0x18) per frame.
- Clump-level API: `../Plugins/RpAnimBlendPlugin/RpAnimBlend.h` (`RpAnimBlendClumpGetAssociation`, `RpAnimBlendClumpGetData`).
- Enums: `AnimationId`, `AssocGroupId` in `../Enums/AnimationEnums.h`; `eAnimationFlags` in `AnimBlendAssociation.h`.

## Conventions
- Hooks: `RH_ScopedClass` (or `RH_ScopedVirtualClass` for the static association) + category `"Animation"`.
  Overloaded constructors / `Init` use `RH_ScopedOverloadedInstall` (see `AnimBlendAssociation.cpp`).
- Start anims with `CAnimManager::AddAnimation`, `BlendAnimation`, `AddAnimationAndSync`; find them with
  `RpAnimBlendClumpGetAssociation(clump, animId)`.
- Use the flag helpers (`IsPlaying()`, `IsLooped()`, `IsPartial()`, ...) and `eAnimationFlags` names, not raw masks.
  Some flag names are known to be doubtful (comments in the enum); don't rename them in a reversal PR.

## Pitfalls
- `AnimAssocDefinitions.cpp` defines `ms_aAnimAssocDefinitionsX`, a NOTSA copy of the table that nothing uses;
  game code reads the `StaticRef` `ms_aAnimAssocDefinitions`.
- `SetFinishCallback` / `SetDeleteCallback(fn, data)` store a raw `void*` (usually a task). The owner must reset it
  (`SetDefaultFinishCallback()`, i.e. `CDefaultAnimCallback::DefaultAnimCB`) before it dies.
- `ANIMATION_IS_BLEND_AUTO_REMOVE` deletes the association once faded out: don't keep a pointer to it across frames
  without a delete callback.
- `ANIMATION_FACIAL` associations are never destroyed (treated as indestructible by `BlendAnimation`).
- Hierarchies are valid only while their block is loaded; streaming code checks `GetNumRefsToAnimBlock`.
- Bones: use `eBoneTag` / `ePedNode`, not `AnimationId` (some older ped code mixes them).
