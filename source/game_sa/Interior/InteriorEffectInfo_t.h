#pragma once

#include "Base.h"

#include "Vector.h"

struct C2dEffectInterior;
class  CEntity;

struct InteriorEffectInfo_t {
    CEntity*                          Entity;
    size_t                            NumFx;
    std::array<C2dEffectInterior*, 8> Effects;
    std::array<uint32, 8>             FxIds;
    float                             DistSq; ///< Min dist. sq. to camera from any of the effects (in the `Effects` array)
    bool                              IsCulled;
};
VALIDATE_SIZE(InteriorEffectInfo_t, 0x50);
