#pragma once

#include "TaskComplex.h"

class CEvent;
class CTask;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexRoadRage : public CTaskComplex {

public:
    CPed* m_rageWith = {}; // 0xC

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_ROAD_RAGE;

    CTaskComplexRoadRage(CPed* rageWith);
    CTaskComplexRoadRage(const CTaskComplexRoadRage&);
    ~CTaskComplexRoadRage();

    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

    CTask*    Clone() const override { return new CTaskComplexRoadRage{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x6220A0
    CTaskComplexRoadRage* Constructor(CPed* rageWith) {
        this->CTaskComplexRoadRage::CTaskComplexRoadRage(rageWith);
        return this;
    }
    // 0x622110
    CTaskComplexRoadRage* Destructor() {
        this->CTaskComplexRoadRage::~CTaskComplexRoadRage();
        return this;
    }
};
