/*
    Plugin-SDK (Grand Theft Auto) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskComplex.h"
#include "Ped.h"

class CTaskComplexKillPedFromBoat : public CTaskComplex {
public:
    CPed* m_pPed;

public:
    CTaskComplexKillPedFromBoat(CPed* ped);
};

VALIDATE_SIZE(CTaskComplexKillPedFromBoat, 0x10);
