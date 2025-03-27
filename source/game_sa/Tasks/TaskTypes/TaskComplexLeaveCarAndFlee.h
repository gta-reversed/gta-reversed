#pragma once

#include "Vector.h"
#include "TaskComplex.h"

class CVehicle;
class CPed;
class CTaskComplexLeaveCarAndFlee;

class NOTSA_EXPORT_VTABLE CTaskComplexLeaveCarAndFlee : public CTaskComplex {

public:
    CVehicle*   m_veh{};
    eTargetDoor m_door{};
    CVector     m_fleePoint{};
    int32       m_fleeDelayMs{};
    bool        m_waitForCarToSlowDown{};

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_LEAVE_CAR_AND_FLEE;

    CTaskComplexLeaveCarAndFlee(
        CVehicle* targetVeh,
        CVector const& fleePoint,
        eTargetDoor targetDoor = TARGET_DOOR_FRONT_LEFT,
        int32 fleeDelayTime = 0,
        bool waitForCarToSlowDown = false
    );
    CTaskComplexLeaveCarAndFlee(const CTaskComplexLeaveCarAndFlee&);
    ~CTaskComplexLeaveCarAndFlee();

    CTask* CreateSubTask(eTaskType taskType);

    CTask*    Clone() const override { return new CTaskComplexLeaveCarAndFlee{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }

private: // Wrappers for hooks
    // 0x63BF90
    CTaskComplexLeaveCarAndFlee* Constructor(CVehicle* targetVeh, CVector const& fleePoint, eTargetDoor targetDoor, int32 fleeDelayTime, bool waitForCarToSlowDown) {
        this->CTaskComplexLeaveCarAndFlee::CTaskComplexLeaveCarAndFlee(targetVeh, fleePoint, targetDoor, fleeDelayTime, waitForCarToSlowDown);
        return this;
    }

    // 0x63C030
    CTaskComplexLeaveCarAndFlee* Destructor() {
        this->CTaskComplexLeaveCarAndFlee::~CTaskComplexLeaveCarAndFlee();
        return this;
    }
};
