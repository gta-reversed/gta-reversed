#pragma once

#include "TaskSimple.h"
#include "Vector.h"

#include "Enums/eTargetDoor.h"
#include "Enums/eMoveState.h"

class CPed;
class CTaskSimpleGoToPoint;
class CTaskSimpleCarWaitForDoorNotToBeInUse;
class CEvent;
class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse : public CTaskSimple {
public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_GO_TO_POINT_NEAR_CAR_DOOR_UNTIL_DOOR_NOT_IN_USE;

    static void InjectHooks();

    CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse(
        CVehicle*      vehicle,
        int32          targetDoor,
        const CVector& targetPt,
        eMoveState     moveState
    );
    CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse(const CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse&);
    ~CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse();

    CTask*    Clone() const override { return new CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override { return true; }
    bool      ProcessPed(CPed* ped) override;

protected:
    CVector ComputeTarget() const;

protected:
    CTaskSimpleGoToPoint*                  m_TaskGoToPoint{}; 
    CTaskSimpleCarWaitForDoorNotToBeInUse* m_TaskWaitForDoor{};

private: // Wrappers for hooks
    // 0x646D70
    CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse* Constructor(CVehicle* vehicle, int32 targetDoor, const CVector& targetPt, eMoveState moveState) {
        this->CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse(vehicle, targetDoor, targetPt, moveState);
        return this;
    }

    // 0x646E50
    CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse* Destructor() {
        this->CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::~CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse();
        return this;
    }
};
