#pragma once

#include "TaskComplex.h"

class CVehicle;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexEnterBoatAsDriver final : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_ENTER_BOAT_AS_DRIVER;

    static void InjectHooks();

    CTaskComplexEnterBoatAsDriver(CVehicle*);
    CTaskComplexEnterBoatAsDriver(const CTaskComplexEnterBoatAsDriver&);
    ~CTaskComplexEnterBoatAsDriver();

    CTask*    Clone() const override { return new CTaskComplexEnterBoatAsDriver{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }

    CTask* CreateSubTask(eTaskType tt);

protected:
    CVehicle* m_EnterInto = {};

private: // Wrappers for hooks
    // 0x63B5E0
    CTaskComplexEnterBoatAsDriver* Constructor(CVehicle* target) {
        this->CTaskComplexEnterBoatAsDriver::CTaskComplexEnterBoatAsDriver(target);
        return this;
    }

    // 0x63B650
    CTaskComplexEnterBoatAsDriver* Destructor() {
        this->CTaskComplexEnterBoatAsDriver::~CTaskComplexEnterBoatAsDriver();
        return this;
    }
};
