#pragma once

#include "Base.h"
#include "Vector.h"
#include <extensions/WEnum.hpp>

class CEntity;

enum class eInteriorInfoType {
    UNK_1 = 1,
    UNK_2 = 2,
    UNK_3 = 3,
    UNK_4 = 4,
    UNK_5 = 5,
    UNK_6 = 6,
    UNK_7 = 7,
    UNK_8 = 8,
    UNK_9 = 9,
    UNK_10 = 10
};

struct InteriorInfo_t {
    notsa::WEnumS8<eInteriorInfoType> Type;
    bool                              IsInUse;
    CVector                           Pos;
    CVector                           Dir;
    CEntity*                          EntityIgnoredCollision;
};
VALIDATE_SIZE(InteriorInfo_t, 0x20);
