#pragma once

//struct AnimFlags {
//    bool bLooped : 1;
//    bool bFreezeLastFrame : 1;
//    bool bUnlockLastFrame : 1;
//    bool bPartial : 1;
//    bool bMovement : 1;
//    bool bTranslateY : 1;
//    bool bTranslateX : 1;
//    bool bWalk : 1;
//    bool b200 : 1;
//    bool bAddToBlend : 1;
//    bool b800 : 1;
//    bool bSecondaryTaskAnim : 1;
//    bool bFreezeTranslation : 1;
//    bool bBlockReferenced : 1;
//    bool bIndestructible : 1;
//};

struct AnimDescriptor {
    AnimationId AnimId{};
    int32       Flags{};
};

struct AnimAssocDefinition {
    constexpr static size_t ANIM_NAME_BUF_SZ = 24;
    
    char   GroupName[16]{};
    char   BlockName[16]{};
    int32  ModelIndex{};
    int32  NumAnims{};

    const char**    AnimNames{}; //< Pointers to heap allocated char arrays (size 24 each) - The array of pointers itself is heap allocated as well - Size == animsCount
    AnimDescriptor* AnimDescr{};
};
VALIDATE_SIZE(AnimAssocDefinition, 0x30);
