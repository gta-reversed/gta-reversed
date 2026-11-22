#pragma once

#include "TaskComplex.h"

class NOTSA_EXPORT_VTABLE CTaskComplexDieInCar : public CTaskComplex {
public:
    eWeaponType m_nWeaponType;
    uint32      m_nTimeMS;
    uint32      m_nOffset;
    bool        m_bPreparedForDeath;
    bool        m_bUpdateTime;

public:
    static constexpr auto Type = TASK_COMPLEX_DIE_IN_CAR;

    explicit CTaskComplexDieInCar(eWeaponType weaponType);
    ~CTaskComplexDieInCar() override = default; // 0x62FCF0, 0x6375D0

    eTaskType GetTaskType() const override { return Type; }; // 0x62FCB0
    CTask* Clone() const override { return new CTaskComplexDieInCar(m_nWeaponType); } // 0x635F90
    CTask* ControlSubTask(CPed* ped) override;
    CTask* CreateSubTask(eTaskType taskType, CPed* ped);
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;

    void PreparePedVehicleForPedDeath(CVehicle* vehicle);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

};
VALIDATE_SIZE(CTaskComplexDieInCar, 0x1C);
