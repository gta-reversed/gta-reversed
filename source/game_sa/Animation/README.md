# Animation object model

How the anim blend classes relate. Sizes, hooks and pitfalls: `CLAUDE.md` in this folder; full guide:
[docs/subsystems/Animation.md](../../../docs/subsystems/Animation.md).

## CAnimBlendHierarchy
One animation (one entry of an IFP file). It holds one `CAnimBlendSequence` per bone it moves (`m_pSequences`,
`m_nSeqCount`) and its total time. All hierarchies live in `CAnimManager::ms_aAnimations`. They are never freed
individually: they stay in memory until `CStreaming` unloads their IFP block.

## CAnimBlendSequence
The key frames of one bone (node) for one animation. An animation that moves 15 bones has 15 sequences and one
hierarchy.

## CAnimBlock
One IFP file (`ped`, `int_house`, ...) streamed as a whole. Ref-counted: a block stays loaded while something holds a
reference (`CAnimManager::AddAnimBlockRef` / `RemoveAnimBlockRef`).

## CAnimBlendAssocGroup and CAnimBlendStaticAssociation
A group (e.g. `default`, `bikes`, `gangs`) maps `AnimationId`s and names to animations of one block. Each entry is a
`CAnimBlendStaticAssociation`: a template with the hierarchy, the default flags and the bone-to-sequence mapping for a
given model.

## CAnimBlendAssociation
A running animation on a clump (usually a ped). It is created from a static association when the clump starts the
animation, and destroyed when the clump stops playing it. It has one `CAnimBlendNode` per bone; each node points to
the hierarchy's `CAnimBlendSequence` (the key frames are shared, not copied) and keeps the per-instance playback state.

A clump can play several associations at once: usually one primary animation, plus partial ones on top (hand gestures,
smoking, facial anims). `CAnimBlendClumpData` holds the list of associations of a clump and one `AnimBlendFrameData`
per frame.
