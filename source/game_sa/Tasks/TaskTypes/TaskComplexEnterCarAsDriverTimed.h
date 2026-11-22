#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"

class CVehicle;
class CPed;
class CEvent;

/*!
* Similar to `TaskComplexEnterCarAsDriver` but ensures that ped
* will be in the car in the given `time`.
* Once `time` expires (counted from when the ped has started entering the vehicle)
* and they aren't in the car they'll be warped into it.
*/
class NOTSA_EXPORT_VTABLE CTaskComplexEnterCarAsDriverTimed : public CTaskComplex {

public:
    CVehicle*  m_veh{};                     /// The vehicle to enter
    int32      m_enterMaxInterval{};        /// The maximum interval [in ms] in which the ped has to be in the car.
    eMoveState m_moveState{ PEDMOVE_RUN };  /// The move state (forwarded to `CTaskComplexEnterCarAsDriver`)
    CTaskTimer m_enterBeganTimer{};         /// Timer started once ped has entered the vehicle, it's interval is `m_enterMaxInterval`

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_ENTER_CAR_AS_DRIVER_TIMED;

    CTaskComplexEnterCarAsDriverTimed(CVehicle* veh, int32 time);
    CTaskComplexEnterCarAsDriverTimed(const CTaskComplexEnterCarAsDriverTimed&);
    ~CTaskComplexEnterCarAsDriverTimed();

    CTask*    Clone() const override { return new CTaskComplexEnterCarAsDriverTimed{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override { return nullptr; }
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    /// Stop `m_enterBeganTimer` under given conditions - Doesn't seem to be used anywhere.
    virtual void StopTimer(CEvent const* event);

private: // Wrappers for hooks
    // 0x63AD70
    CTaskComplexEnterCarAsDriverTimed* Constructor(CVehicle* entity, int32 time) {
        this->CTaskComplexEnterCarAsDriverTimed::CTaskComplexEnterCarAsDriverTimed(entity, time);
        return this;
    }

    // 0x63AE00
    CTaskComplexEnterCarAsDriverTimed* Destructor() {
        this->CTaskComplexEnterCarAsDriverTimed::~CTaskComplexEnterCarAsDriverTimed();
        return this;
    }
};
