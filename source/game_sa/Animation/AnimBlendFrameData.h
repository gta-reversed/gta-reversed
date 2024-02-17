/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"
#include "RpHAnimBlendInterpFrame.h"

class AnimBlendFrameData {
public:
    union {
        struct {
            bool bf1 : 1;                                    // doesn't seem to be used
            bool NodesDontAffectKeyFrameOrientation : 1;   // KeyFrame orientation will be affected
            bool NodesDontAffectKeyFrameTranslation : 1;   // KeyFrame translation will be affected
            bool HasVelocity : 1;
            bool HasZVelocity : 1;
            bool NeedsKeyFrameUpdate : 1;
            bool IsCompressed : 1;
            bool IsUpdatingFrame : 1;                        // doesn't seem to be used
        };
        uint8 Flags;
    };

    union {
        CVector BonePos;  // For skinned clumps (?)
        CVector FramePos; // For non-skinned clumps (?)
    };

    union {
        RpHAnimBlendInterpFrame* KeyFrame; // For skinned clumps
        RwFrame*                 Frame;    // For non-skinned clumps
    };

    uint32 BoneTag; // If `BONE_UNKNOWN` (-1) this is a non-skinned clump, otherwise a skinned one
};
VALIDATE_SIZE(AnimBlendFrameData, 0x18);
