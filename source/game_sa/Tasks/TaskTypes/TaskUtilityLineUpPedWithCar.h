/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Base.h"
#include "Vector.h"
#include "eTargetDoor.h"

class CPed;
class CVehicle;
class CAnimBlendAssociation;

class CTaskUtilityLineUpPedWithCar {
public:
    //! NOTSA: Values of `m_nDoorOpenPosType`
    enum eLineUpType : int32 {
        LINE_UP_AT_DOOR         = 0, //!< The ped is put at the door, its height is blended towards the ground
        LINE_UP_AT_DOOR_FALLING = 1, //!< The ped is put at the door, but falls to the ground by gravity
        LINE_UP_FALLING         = 2, //!< The ped stays where it is and falls to the ground by gravity
    };

public:
    CTaskUtilityLineUpPedWithCar(const CVector& offset, int32 time, int32 doorOpenPosType, int32 doorIdx);
    ~CTaskUtilityLineUpPedWithCar() = default; // 0x64FC00

    CVector GetLocalPositionToOpenCarDoor(CVehicle* vehicle, float animProgress, CAnimBlendAssociation* assoc);
    CVector GetPositionToOpenCarDoor(CVehicle* vehicle, float animProgress, CAnimBlendAssociation* assoc);
    bool    ProcessPed(CPed* ped, CVehicle* vehicle, CAnimBlendAssociation* assoc);

    static void InjectHooks();

private:
    auto GetTargetDoor() const { return (eTargetDoor)m_nDoorIdx; } // NOTSA

private:
    CVector m_Offset{};       //!< From the door to where the ped was when the line up began (Blended out until `m_EndTime`)
    float   m_DoorOpenPosZ{}; //!< Height the ped stands at when it is at the door
    uint32  m_EndTime{};      //!< When the blend from the ped's initial position and heading is over

public:                         // TODO: Used by other tasks
    int32 m_nDoorOpenPosType{}; //!< See `eLineUpType`
    int32 m_nDoorIdx{};         //!< See `eTargetDoor`
};
VALIDATE_SIZE(CTaskUtilityLineUpPedWithCar, 0x1C);
