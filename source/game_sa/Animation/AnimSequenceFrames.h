#pragma once

#include "Quaternion.h"

#include <extensions/FixedVector.hpp>
#include <extensions/FixedQuat.hpp>

/*
 * KeyFrame -> CAnimSequenceChildFrameUncompressed
 * KeyFrameTrans -> CAnimSequenceRootFrameUncompressed
 *
 * KeyFrameCompressed -> CAnimSequenceChildFrameCompressed
 * KeyFrameTransCompressed -> CAnimSequenceRootFrameCompressed
 * */

struct KeyFrame {
    CQuaternion Rot;
    float       DeltaTime; //< Relative to previous key frame
};

struct KeyFrameTrans : KeyFrame {
    CVector Trans;
};

//struct DeltaTime : public FixedFloat<int16, 60.f> {
//    using FixedFloat::FixedFloat;
//    DeltaTime(float x) {
//        Set(x, true);
//    }
//    DeltaTime(FixedFloat<int16, 60.f> x) : FixedFloat{x} {}
//};

struct KeyFrameCompressed {
    FixedQuat<int16, 4096.f>      Rot;
    FixedFloat<int16, 60.f, true> DeltaTime;
};

struct KeyFrameTransCompressed : KeyFrameCompressed {
    FixedVector<int16, 1024.f> Trans;
};

VALIDATE_SIZE(KeyFrameCompressed, 0xA);
VALIDATE_SIZE(KeyFrameTransCompressed, 0x10);
VALIDATE_SIZE(KeyFrame, 0x14);
VALIDATE_SIZE(KeyFrameTrans, 0x20);
