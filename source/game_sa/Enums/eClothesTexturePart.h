/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

enum eClothesTexturePart : int32 {
    CLOTHES_TEXTURE_TORSO           = 0,
    CLOTHES_TEXTURE_HEAD            = 1,
    CLOTHES_TEXTURE_LEGS            = 2,
    CLOTHES_TEXTURE_SHOES           = 3,
    CLOTHES_TEXTURE_LOWER_LEFT_ARM  = 4,
    CLOTHES_TEXTURE_UPPER_LEFT_ARM  = 5,
    CLOTHES_TEXTURE_UPPER_RIGHT_ARM = 6,
    CLOTHES_TEXTURE_LOWER_RIGHT_ARM = 7,
    CLOTHES_TEXTURE_BACK_TOP        = 8,
    CLOTHES_TEXTURE_LEFT_CHEST      = 9,
    CLOTHES_TEXTURE_RIGHT_CHEST     = 10,
    CLOTHES_TEXTURE_STOMACH         = 11,
    CLOTHES_TEXTURE_LOWER_BACK      = 11, // todo: add 12?
    CLOTHES_TEXTURE_UPPER_BACK      = 12, //!< Not sure
    CLOTHES_TEXTURE_NECKLACE        = 13,
    CLOTHES_TEXTURE_BRACELET        = 14,
    CLOTHES_TEXTURE_GLASSES         = 15,
    CLOTHES_TEXTURE_HATS            = 16,
    CLOTHES_TEXTURE_SPECIAL         = 17,
    CLOTHES_TEXTURE_UNAVAILABLE     = 18,
    CLOTHES_TEXTURE_TOTAL           = CLOTHES_TEXTURE_UNAVAILABLE
};
