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
    REPEATSECTOR_OBJECTS  = 2,

    REPEATSECTOR_COUNT
};

class CRepeatSector {
protected:
    CPtrListDoubleLink<void*> m_ptrListArray[REPEATSECTOR_COUNT];

public:
    CPtrListDoubleLink<void*>* GetPtrListArray() { return m_ptrListArray; } // unused

    CPtrListDoubleLink<CVehicle*>& GetOverlapVehiclePtrList() {
        return reinterpret_cast<CPtrListDoubleLink<CVehicle*>&>(m_ptrListArray[REPEATSECTOR_VEHICLES]);
    }

    CPtrListDoubleLink<CPed*>& GetOverlapPedPtrList() {
        return reinterpret_cast<CPtrListDoubleLink<CPed*>&>(m_ptrListArray[REPEATSECTOR_PEDS]);
    }

    CPtrListDoubleLink<CObject*>& GetOverlapObjectPtrList() {
        return reinterpret_cast<CPtrListDoubleLink<CObject*>&>(m_ptrListArray[REPEATSECTOR_OBJECTS]);
    }
};

VALIDATE_SIZE(CRepeatSector, 0xC);
