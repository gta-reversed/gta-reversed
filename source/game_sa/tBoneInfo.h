#pragma once

#include "Base.h"
#include "eBoneTag.h"

struct BoneInfo_t {
    eBoneTag BoneTag;       // This bone
    eBoneTag ParentBoneTag; // Previous connected bone
    CVector  PoseRots;      //!< No clue
    float    MinX, MaxX;    //!< Limits on the X axis
    float    MinY, MaxY;    //!< Limits on the Y axis
    float    MinZ, MaxZ;    //!< Limits on the Z axis
};
VALIDATE_SIZE(BoneInfo_t, 0x28);
