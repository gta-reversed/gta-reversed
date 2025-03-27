#pragma once

#include "TaskComplex.h"
#include "Vector.h"
#include "Base.h"

class CVehicle;
class CPed;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskComplexDestroyCarArmed : public CTaskComplex {
public:
    bool      m_bNeedsToCreatefirstSubTask{};
    CVehicle* m_VehToDestroy{};
    CVector   m_PedPos{};
    CVector   m_VehiclePos{};
    CVector   m_PedToVehDirUnnorm{};
    float     m_fWeaponRange{};
    int32     dword3C{};
    float     m_fWeaponRangeClamped{};
    int32     dword44{};
    float     m_PedVehDist{};
    float     dword4C{};
    bool      m_bGotoPointAndStandStill{};
    int32     dword54{-1};
    uint32    ctor58{};
    uint32    ctor5C{};
    uint32    ctor60{};

public:
    static constexpr eTaskType Type = TASK_COMPLEX_DESTROY_CAR_ARMED;

    CTaskComplexDestroyCarArmed(CVehicle* vehicleToDestroy, uint32 a3 = 0, uint32 a4 = 0, uint32 a5 = 0);
    CTaskComplexDestroyCarArmed(const CTaskComplexDestroyCarArmed&); // NOTSA
    ~CTaskComplexDestroyCarArmed() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskComplexDestroyCarArmed{ *this }; }
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

    CTask* CreateSubTask(eTaskType taskType, CPed* ped);
    void CalculateSearchPositionAndRanges(CPed* ped);

    static void InjectHooks();
    CTaskComplexDestroyCarArmed* Constructor(CVehicle* vehicleToDestroy, uint32 a3, uint32 a4, uint32 a5) { this->CTaskComplexDestroyCarArmed::CTaskComplexDestroyCarArmed(vehicleToDestroy, a3, a4, a5); return this; }
    CTaskComplexDestroyCarArmed* Destructor() { this->CTaskComplexDestroyCarArmed::~CTaskComplexDestroyCarArmed(); return this; }
};
VALIDATE_SIZE(CTaskComplexDestroyCarArmed, 0x64);
