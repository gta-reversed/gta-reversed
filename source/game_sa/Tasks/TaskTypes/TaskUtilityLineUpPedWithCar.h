/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Base.h"
#include "Vector.h"

class CTaskUtilityLineUpPedWithCar {
public:
    CVector m_Offset;
    float   m_fDoorOpenPosZ;
    int32   m_fTime;
    int32   m_nDoorOpenPosType;
    int32   m_nDoorIdx;

public:
    CTaskUtilityLineUpPedWithCar(const CVector& offset, int32 time, int32 doorOpenPosType, int32 doorIdx);
    ~CTaskUtilityLineUpPedWithCar() = default;

    CVector GetLocalPositionToOpenCarDoor(CVehicle* vehicle, float animProgress, CAnimBlendAssociation* assoc);
    CVector GetPositionToOpenCarDoor(CVehicle* vehicle, float animProgress, CAnimBlendAssociation* assoc);
    bool ProcessPed(CPed* ped, CVehicle* vehicle, CAnimBlendAssociation* assoc);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskUtilityLineUpPedWithCar* Constructor(const CVector& offsets, int32 nTime, int32 doorOpenPosType, int32 doorIdx) { this->CTaskUtilityLineUpPedWithCar::CTaskUtilityLineUpPedWithCar(offsets, nTime, doorOpenPosType, doorIdx); return this; }
    CTaskUtilityLineUpPedWithCar* Destructor() { this->CTaskUtilityLineUpPedWithCar::~CTaskUtilityLineUpPedWithCar(); return this; }

    friend class CTaskComplexEnterCar;
};
VALIDATE_SIZE(CTaskUtilityLineUpPedWithCar, 0x1C);
