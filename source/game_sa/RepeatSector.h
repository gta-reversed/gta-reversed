/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PtrListDoubleLink.h"

class CVehicle;
class CPed;
class CObject;

enum eRepeatSectorList : int32 {
    REPEATSECTOR_VEHICLES = 0,
    REPEATSECTOR_PEDS     = 1,
    REPEATSECTOR_OBJECTS  = 2
};

class CRepeatSector {
public:
    CPtrListDoubleLink<CVehicle*> Vehicles;
    CPtrListDoubleLink<CPed*>     Peds;
    CPtrListDoubleLink<CObject*>  Objects;
};
VALIDATE_SIZE(CRepeatSector, 0xC);
