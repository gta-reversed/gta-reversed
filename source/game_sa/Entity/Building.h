/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Entity.h"

class NOTSA_EXPORT_VTABLE CBuilding : public CEntity {
public:
    CBuilding();
    static void* operator new(unsigned size);
    static void operator delete(void* data);

public:
    static void InjectHooks();

    void ReplaceWithNewModel(int32 newModelIndex);
};

VALIDATE_SIZE(CBuilding, 0x38);

bool IsBuildingPointerValid(CBuilding* building);

// orig in entity.cpp
static int32 gBuildings; // 0xB71804

