#pragma once

#include "TaskComplex.h"
#include "Vehicle.h"
#include "Accident.h"

class NOTSA_EXPORT_VTABLE CTaskComplexMedicTreatInjuredPed : public CTaskComplex {
public:
    CVehicle*  m_pVehicle;
    CPed*      m_pPartnerMedic;
    uint8      m_bIsDriver;
    CAccident* m_pAccident;
    CVector    m_vecAccidentPosition;
    uint8      m_bLeftCarOnce;

public:
    static constexpr auto Type = TASK_COMPLEX_MEDIC_TREAT_INJURED_PED;

    CTaskComplexMedicTreatInjuredPed(CVehicle* vehicle, CPed* ped, bool isDriver);
    ~CTaskComplexMedicTreatInjuredPed() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

    CTask* CreateSubTask(eTaskType taskType);
    CTask* CreateDealWithNextAccidentTask(CPed* ped, CAccident* accident);
    void   FindNearestAccident(CVector& posn);
    void   CalcTargetPosWithOffset(CPed* ped, CPed* targetPed);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexMedicTreatInjuredPed* Constructor(CVehicle* vehicle, CPed* ped, bool isDriver);

};

VALIDATE_SIZE(CTaskComplexMedicTreatInjuredPed, 0x2C);
