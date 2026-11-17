#pragma once

#include "TaskSimple.h"
#include "Vehicle.h"

class CVehicle;
class CEvent;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleCarForcePedOut : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_CAR_FORCE_PED_OUT;

    static void InjectHooks();

    CTaskSimpleCarForcePedOut(CVehicle* veh, int32 door);
    CTaskSimpleCarForcePedOut(const CTaskSimpleCarForcePedOut&);
    ~CTaskSimpleCarForcePedOut() = default;

    CTask*    Clone() const override { return new CTaskSimpleCarForcePedOut{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return false; }
    bool      ProcessPed(CPed* ped) override;

private:
public:
    CVehicle::Ref m_Veh{};
    int32         m_Door{};
};
