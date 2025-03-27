/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
#include "Vehicle.h"
#include "TaskUtilityLineUpPedWithCar.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleCarSetPedInAsDriver : public CTaskSimple {
public:
    bool                          m_bIsFinished;
    CAnimBlendAssociation*        m_pAnim;
    CVehicle*                     m_pTargetVehicle;
    CTaskUtilityLineUpPedWithCar* m_pUtility;
    bool                          m_bWarpingInToCar;
    uint8                         m_nDoorFlagsToClear;
    uint8                         m_nNumGettingInToClear;

public:
    static constexpr auto Type = TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER;

    CTaskSimpleCarSetPedInAsDriver(CVehicle *targetVehicle, CTaskUtilityLineUpPedWithCar *utility = nullptr);
    CTaskSimpleCarSetPedInAsDriver(CVehicle *targetVehicle, bool warpingInToCar, CTaskUtilityLineUpPedWithCar *utility = nullptr);
    ~CTaskSimpleCarSetPedInAsDriver() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override;
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return false; }
    bool ProcessPed(CPed* ped) override;
};
VALIDATE_SIZE(CTaskSimpleCarSetPedInAsDriver, 0x1C);
