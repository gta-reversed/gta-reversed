/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

enum eClothesModelPart : int32 {
    CLOTHES_MODEL_TORSO        = 0,
    CLOTHES_MODEL_HEAD         = 1,
    CLOTHES_MODEL_HANDS        = 2,
    CLOTHES_MODEL_LEGS         = 3,
    CLOTHES_MODEL_SHOES        = 4, // FEET
    CLOTHES_MODEL_NECKLACE     = 5,
    CLOTHES_MODEL_BRACELET     = 6, // WATCH
    CLOTHES_MODEL_GLASSES      = 7,
    CLOTHES_MODEL_HATS         = 8,
    CLOTHES_MODEL_SPECIAL      = 9, // extra1
    CLOTHES_MODEL_UNAVAILABLE  = 10,

    //
    // Add above
    //
    CLOTHES_MODEL_TOTAL = CLOTHES_MODEL_UNAVAILABLE
};
