#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"
#include "Vector.h"

class CTaskSimpleGoToPoint;
class CEvent;
class CPed;

/*!
* Task to make a ped run away from a position ASAP.
* 
* Sequence goes like:
* - If in vehicle, TASK_COMPLEX_LEAVE_ANY_CAR
* - TASK_SIMPLE_GO_TO_POINT => Repeat until not in danger or `m_timer` expires
* - TASK_COMPLEX_SEQUENCE
* - TASK_FINISHED
*/
class NOTSA_EXPORT_VTABLE CTaskComplexFleePoint : public CTaskComplex {
public:
    CVector m_fleeFromPos{};    ///< Position to flee from
    CVector m_fleeToPos{};      ///< The position the ped should go(flee) to
    int32 m_durationMs{};       ///< The duration (todo)
    CTaskTimer m_timer{};       ///< Task timer: Started in `ControlSubTask`, if out of time skips to `TASK_COMPLEX_SEQUENCE`
    float m_safeDist{};         ///< The safe distance, once the ped is further away than this, the task is considerd done
    bool m_bScream{};           ///< If peds should scream while sprinting away
    bool m_newFleePosWasSet{};  ///< New `m_fleeFromPos` was set using `SetFleePosition`

public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_COMPLEX_FLEE_POINT;

    CTaskComplexFleePoint(CVector const& point, bool a3, float radius, int32 timeLimit);
    CTaskComplexFleePoint(const CTaskComplexFleePoint&);
    ~CTaskComplexFleePoint() = default;

    auto CreateSubTask(eTaskType taskType) -> CTask*;
    void SetFleePosition(CVector const& fleePos, float safeDistance, bool scream);
    void ComputeTargetPoint(CPed const* ped);

    CTask*    Clone() const override { return new CTaskComplexFleePoint{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed * ped) override;
    CTask*    CreateFirstSubTask(CPed * ped) override;
    CTask*    ControlSubTask(CPed * ped) override;
 
private: // Wrappers for hooks

// 0x65B390
CTaskComplexFleePoint* Constructor(CVector const& point, bool a3, float radius, int32 timeLimit) {
    this->CTaskComplexFleePoint::CTaskComplexFleePoint(point, a3, radius, timeLimit);
    return this;
}
// 0x65B410
CTaskComplexFleePoint* Destructor() {
    this->CTaskComplexFleePoint::~CTaskComplexFleePoint();
    return this;
}


};
