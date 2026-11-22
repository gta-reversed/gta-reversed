## CAnimBlendSequence
Represents frames for a single bone.

## CAnimBlendAssociation
Represents a running animtion for a clump (Usually peds)
 
The sequence/frames data is copied from `CAnimBlendHierarchy` to `CAnimBlendAssociation` when a clump requests an animation.
The instance of `CAnimBlendAssociation` gets destroyed when the ped/clump stops playing the animation.
But for `CAnimBlendHierarchy`, it is never destroyed and stays in memory unless CStreaming forces the IFP to unload to create space in memory.

A clump can have one, or more, instances of this class. Usually there's only 1 primary animation,
but there are also partial animations, which can be played alongside primary animations, like hand gestures or smoking.
So if an animation moves up to 15 bones in one animation, there'll be 15 instances of `CAnimBlendSequence`,
and there'll be always one instance of `CAnimBlendHierarchy` - the animation itself

## CAnimBlendHierarchy
The animation object. It contains `CAnimBlendSequence`'s each of which are the animation for one bone (node).
The sequence/frames data is copied from it to `CAnimBlendAssociation` when a clump requests an animation.
It is never destroyed and stays in memory unless `CStreaming` forces the IFP to unload to create space in memory.

## CAnimBlendStaticAssociation
Stores static animation data loaded from an animation clump.

## CAnimBlendAssocGroup
Represents a group (block) (Such as `ped`, `int_house`, etc).
Is also used for mapping `AnimationId`'s and animation names to animation data.
