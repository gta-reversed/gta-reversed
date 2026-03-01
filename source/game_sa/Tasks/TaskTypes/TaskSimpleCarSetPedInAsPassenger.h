/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
#include "TaskUtilityLineUpPedWithCar.h"

class CVehicle;
class CAnimBlendAssociation;
class CTaskUtilityLineUpPedWithCar;


class NOTSA_EXPORT_VTABLE CTaskSimpleCarSetPedInAsPassenger : public CTaskSimple {
public:
    bool                          m_bIsFinished{};
    CAnimBlendAssociation*        m_pAnim{};
    CVehicle*                     m_pTargetVehicle{};
    eTargetDoor                   m_nTargetDoor{};
    CTaskUtilityLineUpPedWithCar* m_pUtility{};
    bool                          m_bWarpingInToCar{};
    uint8                         m_nDoorFlagsToClear{};
    uint8                         m_nNumGettingInToClear{};

public:
    static constexpr auto Type = TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER;

    CTaskSimpleCarSetPedInAsPassenger(CVehicle* targetVehicle, eTargetDoor nTargetDoor, bool warpingInToCar /* notsa arg */ = false, CTaskUtilityLineUpPedWithCar* utility = nullptr);
    CTaskSimpleCarSetPedInAsPassenger(const CTaskSimpleCarSetPedInAsPassenger&);
    ~CTaskSimpleCarSetPedInAsPassenger() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskSimpleCarSetPedInAsPassenger{*this}; }  // 0x649D90
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return false; }
    bool ProcessPed(CPed* ped) override;
};
VALIDATE_SIZE(CTaskSimpleCarSetPedInAsPassenger, 0x20);
