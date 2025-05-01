#pragma once

#include "Base.h"
#include "Vector.h"
#include "TaskComplex.h"
#include "TaskTimer.h"

class CPed;
class CEvent;
class CTaskSimpleStandStill;

class NOTSA_EXPORT_VTABLE CTaskComplexSmartFleePoint : public CTaskComplex {
public:
    CVector m_initalPos{};                  // 0xC
    CVector m_fleePoint{};                  // 0x18
    bool m_doScream{};                      // 0x24
    int32 m_fleeTimeMs{};                   // 0x28
    float m_safeDist{};                     // 0x2C
    eMoveState m_moveState{PEDMOVE_SPRINT}; // 0x30
    CTaskTimer m_timer{};                   // 0x34
    uint8 m_fleeDir{};                      // 0x40
    bool m_hasFleePointChanged{};           // 0x41

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_SMART_FLEE_POINT;

    CTaskComplexSmartFleePoint(CVector const& fleePos, bool doScream, float safeDist, int32 fleeTimeMs);
    CTaskComplexSmartFleePoint(const CTaskComplexSmartFleePoint& o); // NOTSA

    int8 SetDefaultTaskWanderDir(CPed* ped);
    uint32 ComputeFleeDir(CPed*);
    CTask* CreateSubTask(eTaskType taskType, CPed* ped);
    int8 SetFleePosition(CVector const& a2, float a3, bool a4);

    CTask*    Clone() const override { return new CTaskComplexSmartFleePoint{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;


private: // Wrappers for hooks
    // 0x65BD20
    CTaskComplexSmartFleePoint* Constructor(CVector const& fleePos, bool doScream, float safeDist, int32 fleeTimeMs) {
        this->CTaskComplexSmartFleePoint::CTaskComplexSmartFleePoint(fleePos, doScream, safeDist, fleeTimeMs);
        return this;
    }
    // 0x65BDB0
    CTaskComplexSmartFleePoint* Destructor() {
        this->CTaskComplexSmartFleePoint::~CTaskComplexSmartFleePoint();
        return this;
    }
};
