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
            bool bf1 : 1;                                //!< doesn't seem to be used
            bool KeyFramesIgnoreNodeOrientation : 1; //!< Whenever orientation 
            bool KeyFramesIgnoreNodeTranslation : 1; //!< Whenever translation 
            bool HasVelocity : 1;                        //!< If true the translation is used to move the ped
            bool HasZVelocity : 1;                       //!< If true 3D velocity extraction is used, otherwise 2D
            bool NeedsKeyFrameUpdate : 1;                //!< If `RpAnimBlendNodeUpdateKeyFrames` needs to be called on update
            bool IsCompressed : 1;                       //!< Is the anim data for this frame compressed
            bool IsUpdatingFrame : 1;                    //!< Doesn't seem to be used
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
