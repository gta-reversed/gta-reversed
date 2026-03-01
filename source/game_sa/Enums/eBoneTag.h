/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <extensions/WEnum.hpp>

enum eBoneTag : int16 {
    BONE_UNKNOWN     = -1,

    BONE_ROOT      = 0, // Normal or Root, both are same
    BONE_PELVIS      = 1,
    BONE_SPINE       = 2,
    BONE_SPINE1      = 3,
    BONE_NECK        = 4,
    BONE_HEAD        = 5,
    BONE_L_BROW      = 6,
    BONE_R_BROW      = 7,
    BONE_JAW         = 8,

    BONE_R_CLAVICLE  = 21,
    BONE_R_UPPER_ARM = 22,
    BONE_R_FORE_ARM  = 23,
    BONE_R_HAND      = 24,
    BONE_R_FINGER    = 25,
    BONE_R_FINGER_01 = 26,

    BONE_L_CLAVICLE  = 31,
    BONE_L_UPPER_ARM = 32,
    BONE_L_FORE_ARM  = 33,
    BONE_L_HAND      = 34,
    BONE_L_FINGER    = 35,
    BONE_L_FINGER_01 = 36,

    BONE_L_THIGH     = 41,
    BONE_L_CALF      = 42,
    BONE_L_FOOT      = 43,
    BONE_L_TOE_0     = 44,

    BONE_R_THIGH     = 51,
    BONE_R_CALF      = 52,
    BONE_R_FOOT      = 53,
    BONE_R_TOE_0     = 54,

    BONE_BELLY       = 201,

    BONE_L_BREAST    = 302,
    BONE_R_BREAST    = 301,

    BONE_MAX_ID = 303,
    MAX_BONE_NUM = 32
};

using eBoneTag16 = notsa::WEnumS16<eBoneTag>;
using eBoneTag32 = notsa::WEnumS32<eBoneTag>;
